#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

#include <iostream>
#include <vector>




using namespace std;
// dimensions: owner, status, income, borrower
const int d = 4;

enum Status {
    Single, Married, Divorced
};

struct Record {

    Record(int id, bool owner, Status status, float income, bool borrower) {
        this->id = id;
        this->owner = owner;
        this->status = status;
        this->income = income;
        this->borrower = borrower;
        this->testCase = false;

        for (int i = 0; i < d; i++){
            this->missing[i] = false;
        }
    }

    // non-copy-and-swap assignment
    Record& operator=(Record other)
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

    void setMissing(bool owner, bool status, bool income, bool borrower){
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

    void toggleTestCase(bool testCase){ // true if it is a test case
        this->testCase = testCase;
    }

    string getStatus(Status status){
        return (missing[1]? "unknown ":
                (status==Single?"single  ":
                 (status==Married?"married":
                  "divorced")));
    }

    int id;
    bool owner;
    Status status;
    float income;
    bool borrower;
    bool testCase;

    // whether a field is missing or not, not taking into consideration id dimension, it must exist
    bool missing[d];
};


class Bayesian {
private:
    vector<Record> records;
    int id;

public:
    Bayesian()  {
        id = 1;
    }

    ~Bayesian() {}

    int size(){
        return records.size();
    }

    void addRecord( bool owner, Status status, float income, bool borrower){
        Record x(id++,owner, status, income, borrower);
        records.push_back(x);
    }

    void setMissing(int index, bool owner, bool status, bool income, bool borrower){
        records[index].setMissing(owner, status, income, borrower);
    }

    void propOfClass(bool classification){

    }

    void classifyRecord(Record r, bool someMissing = false){
//        classProbalility
        //probOfClass(false);

    }

    void test(){
        // todo: don't pop_back, and add smoothing function
        records.pop_back();

        // loop over the records and test each, based on the rest of the records
        for (auto r =  records.begin(); r != records.end(); ++r){
            Record test = *r;
            records.erase(r);
            print();

            classifyRecord(*r);

            records.insert(r,test);

        }
    }

    void print(){
        cout <<BCYAN<< "id\t\t|" <<  "owner\t\t" <<  "|status\t\t" <<  "|income\t\t\t|"<<"class\n"<<RESET;
        for(Record r : records) {
            cout << r.id << "\t\t|" << r.owner << "\t\t\t|" << r.getStatus(r.status) << "\t|" << r.income << "\t\t\t|" <<BRED<< r.borrower <<RESET<< '\n';
        }
    }

};

int main() {
    Bayesian model = Bayesian();

    /*  add records */
    model.addRecord(true,Single,125000,false);
    model.addRecord(false,Married,100000,false);
    model.addRecord(false,Single,70000,false);
    model.addRecord(true,Married,120000,false);
    model.addRecord(false,Divorced,95000,true);
    model.addRecord(false,Married,60000,false);
    model.addRecord(true,Divorced,220000,false);
    model.addRecord(false,Single,85000,true);
    model.addRecord(false,Married,75000,false);
    model.addRecord(false,Single,90000,true);

    model.print();


    // add the test record
    model.addRecord(true,Married,50700,false);
    // set missing attributes for that test case, which is the last record
    model.setMissing(model.size()-1, 0,0,0,1);


//    Record test(999,true,Married,50700,false);
//    test.setMissing(0,0,0,1);
//    test.toggleTestCase(true);




    model.test();
    model.print();
    return 0;
}