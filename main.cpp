#define RESET   "\033[0m"
#define BRED     "\033[1m\033[31m"      /* Bold Red */
#define BGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BCYAN    "\033[1m\033[36m"      /* Bold Cyan */

#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>


using namespace std;
const int d = 4; // dimensions: owner, status, income, borrower

enum Status { Single, Married, Divorced };

struct Record : public __wrap_iter<::Record *> {
    Record(int id, bool owner, Status status, double income, bool borrower);
    Record& operator=(Record r);
    void setMissing(bool owner, bool status, bool income, bool borrower);

    string num(int value, int space);
    string getOwner();
    string getStatus();
    string getProp();


    int id;
    bool owner;
    Status status;
    double income;
    bool borrower;
    double classProb;
    bool testCase;

    // whether a field is missing or not, not taking into consideration id dimension, it must exist
    bool missing[d];
};


class Bayesian {
private:
    int idGenerator;
    double ownerProb;
    double statusProb[3];
    double mean, variance;
    vector<Record> records;


public:
    Bayesian();
    ~Bayesian() {}
    int size();
    void addRecord( bool owner, Status status, double income, bool borrower);
    void removeRecord();
    void setMissing(int index, bool owner, bool status, bool income, bool borrower);
    void train(bool classValue = false);
    void setContinuous(bool classValue = false);
    void classifyRecord(Record& test, bool someMissing = false);
    void test();
    void print();

};

static void error(string s);

int main() {
    try{
        Bayesian model = Bayesian();

        model.addRecord(true,Single,125,false);
        model.addRecord(false,Married,100,false);
        model.addRecord(false,Single,70,false);
        model.addRecord(true,Married,120,false);
        model.addRecord(false,Divorced,95,true);
        model.addRecord(false,Married,60,false);
        model.addRecord(true,Divorced,220,false);
        model.addRecord(false,Single,85,true);
        model.addRecord(false,Married,75,false);
        model.addRecord(false,Single,90,true);

        // check for zero-frequency problem
        model.train();
        model.addRecord(false,Married,120,false);

        model.print();
        model.test();
        model.print();
        model.removeRecord();


        model.train(true);
        model.addRecord(false,Married,120,false);

        model.print();
        model.test();

        return 0;
    } catch(string error){
        cout << error;
        return 1;
    }

}


/*
 *
 *
 *
 *
 * struct Record
 */

Record::Record(int id, bool owner, Status status, double income, bool borrower) {
    this->id = id;
    this->owner = owner;
    this->status = status;
    this->income = income;
    this->borrower = borrower;
    this->testCase = false;
    this->classProb = -1;

    for (int i = 0; i < d; i++){
        this->missing[i] = false;
    }
}

Record&
Record::operator=(Record other)
{
    if(&other == this)
        return *this;
    this->id = other.id;
    this->owner = other.owner;
    this->status = other.status;
    this->income = other.income;
    this->borrower = other.borrower;
    this->testCase = other.testCase;
    for (int i = 0; i < d; i++){
        this->missing[i] = other.missing[i];
    }
    return *this;
}

void
Record::setMissing(bool owner, bool status, bool income, bool borrower){
    int i = 0;
    missing[i++] = owner;
    missing[i++] = status;
    missing[i++] = income;
    missing[i++] = borrower;
    cout << "Record #"<< this->id<<" has the following attributes:\n";
    for (int i = 0; i < d; i++){
        cout <<(this->missing[i]? "[missing]":"[not missing]")<<"\t";
    }
    cout <<endl;

}


string
Record::getStatus() {
    return (missing[1] ? "unknown ":
           (status == Single) ? "single  " :
           (status == Married) ? "married " : "divorced");
}

string Record::num(int value, int space) {
    stringstream stream;
    stream << setfill(' ') << setw(space) << value;
    return stream.str();
}

string Record::getOwner() {
    return owner? "Yes  " : "No   ";
}

string Record::getProp() {

    return (this->classProb == -1)? "?": to_string(this->classProb);
}

/*
 *
 *
 *
 *
 * Bayesian
 *
 *
 *
 */

Bayesian::Bayesian()  { idGenerator = 1; ownerProb = statusProb[0]  =  -1; }

void
Bayesian::addRecord( bool owner, Status status, double income, bool borrower){
    Record x(idGenerator++,owner, status, income, borrower);
    records.push_back(x);
}

void
Bayesian::setMissing(int index, bool owner, bool status, bool income, bool borrower){
    records[index].setMissing(owner, status, income, borrower);
}

void
Bayesian::train(bool classValue){
    double borrower = 0, owner=0, single=0, married = 0, divorced = 0;
    for (Record r: records){
        if (r.borrower != classValue)
            continue;

        borrower++;
        if (r.owner) owner++;
        if (r.status == Single) single++;
        if (r.status == Married) married++;
        if (r.status == Divorced) divorced++;
    }
    if (!borrower || borrower == 1)// later in the deviation
        error("division by zero.");

    cout << double(owner/borrower)<<"-------\n";

    this->ownerProb = (double )owner/borrower;

    this->statusProb[Single] = single/borrower; // If the first enumerator has no initializer, the value of the corresponding constant is zero
    this->statusProb[Married] = married/borrower;
    this->statusProb[Divorced] = divorced/borrower;

    setContinuous();

}

void
Bayesian::setContinuous(bool classValue) {
    int count = 0, total = 0;
    for (auto r: records){
        if (r.borrower  == classValue) {
            total += r.income;
            count++;
        }
    }

    mean = total / count;
    variance = 0; // reset in case retrained

    for (auto r: records)
        if (r.borrower == classValue)
            variance += (r.income - mean)*(r.income - mean) ;

    // standard deviation of a sample (not / count)
    variance = variance/(count -1);
    double income = 120;

    double result = exp(-1 * pow((income-mean),2) / (2 * variance))
                    /  sqrt(2 * M_PI * variance);


}

static void error(string s){
    stringstream stream;
    stream << "Error: "<<BRED<< s <<RESET<<endl;
    throw stream.str();
}

void
Bayesian::test() {
    for (auto &r: records) 
        classifyRecord(r);
}


void
Bayesian::classifyRecord(Record& test, bool someMissing){

    double income = exp(-1 * pow((test.income-mean),2) / (2 * variance))
                    /  sqrt(2 * M_PI * variance);

    double owner = test.owner? this->ownerProb:(1-this->ownerProb);
    double result = this->statusProb[test.status] * owner * income;
    test.classProb = result;


}

void
Bayesian::removeRecord() {
    idGenerator--;
    records.erase(records.end()-1);
}

inline void
Bayesian::print(){
    cout <<BCYAN<< "id\t| owner\t| status   \t| income\t| class\t| predicted class\n"<<RESET;

    for(Record r : records) {
        cout << r.num(r.id,2) << "\t| " << r.getOwner()
             << "\t| " << r.getStatus() << "\t| "
             << r.num(r.income,5) << "K\t| " <<BGREEN
             << r.num(r.borrower,2) <<RESET<<"   \t| "
             <<BRED<<r.getProp() <<RESET<< '\n';
    }
    cout << endl;
}
