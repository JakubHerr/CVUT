#ifndef __PROGTEST__
#include <string>
#include <vector>
#include <cassert>
#include <algorithm>

using namespace std;
#endif /* __PROGTEST__ */

/**
 * structure for storing a single record about a car (ID) and its owner (name, surname).
 */
struct CRecord {
    string surname;
    string name;
    string rz;

    CRecord(const string &surname, const string &name, const string &rz = "") : surname(surname), name(name), rz(rz) {}

    bool operator==(const CRecord &rhs) const {
        return (surname == rhs.surname && name == rhs.name && rz == rhs.rz);
    }

    bool operator<(const CRecord &rhs) const {
        if (surname != rhs.surname) return surname < rhs.surname;
        else if (name != rhs.name) return name < rhs.name;
        //rz is  not compared here on purpose, it is not required and makes CPersonList easier to implement
        else return false;
    }
};

/**
 * Simple forward list, used to store unsorted IDs of
 * cars owned by a single person.
 * Can return car ID, check the end of list and advance
 * to the next car.
 * Instead of copying all of the records, the list referring to
 * records already present in the database
 * The list can only advance forward and
 * once the end is reached, it is useless.
 */
class CCarList {
public:
    CCarList(const CRecord *curr, const CRecord *end) : curr(curr), end(end) {}

    const string &RZ() const { return curr->rz; }

    bool AtEnd() const { return curr == end; }

    void Next() { if (!AtEnd())curr++; }

    CCarList &operator=(const CCarList &rhs) {
        if (this == &rhs) return *this;
        curr = rhs.curr;
        end = rhs.end;
        return *this;
    }

private:
    const CRecord *curr;
    const CRecord *end;
};

/**
 * Simple forward list, used to store a list of car owners
 * sorted by surname and name in ascending order.
 */
class CPersonList {
public:
    CPersonList(const CRecord *curr, const CRecord *end) : curr(curr), end(end) {}

    const string &Name() const { return curr->name; }

    const string &Surname() const { return curr->surname; }

    bool AtEnd() const { return curr == end; }

    //the current pointer can't be simply incremented, because the list can contain
    //multiple cars owned by the same person
    void Next() { if (!AtEnd()) curr = upper_bound(curr, end, *curr); }

    CPersonList &operator=(const CPersonList &rhs) {
        if (this == &rhs) return *this;
        curr = rhs.curr;
        end = rhs.end;
        return *this;
    }

private:
    const CRecord *curr;
    const CRecord *end;
};

/**
 * Main class implementing a register of cars and their owners.
 * Cars can be added, deleted, transferred, listed and counted.
 * Records about cars and their owners are stored in a sorted vector.
 */
class CRegister {
public:

    CRegister() {}

    ~CRegister() {}

    CRegister(const CRegister &src) = delete;

    CRegister &operator=(const CRegister &src) = delete;

    /**
     * Method for adding a new car to the register
     * @param rz        car ID
     * @param name      owner name
     * @param surname   owner surname
     * @return true     if car was added successfully
     * @return false    if car is already in the register
     */
    bool AddCar(const string &rz, const string &name, const string &surname) {
        CRecord new_car(surname, name, rz);
        /* check if the car is in the database.
           can't use binary_search, vector is not completely sorted */
        for (CRecord &saved_car: reg) {
            if (new_car.rz == saved_car.rz) return false;
        }
        //add a record to the right position in a sorted vector
        auto it = lower_bound(reg.begin(), reg.end(), new_car);
        reg.insert(it, new_car);
        return true;
    }

    /**
     * Method for deleting an existing car from the register
     * @param rz        car ID
     * @return true     if car was deleted successfully
     * @return false    if car was not found in the register
     */
    bool DelCar(const string &rz) {
        //check if the car is in the database
        for (auto it = reg.begin(); it < reg.end(); ++it) {
            //remove record if it is
            if (it->rz == rz) {
                reg.erase(it);
                return true;
            }
        }
        //car is not in the database
        return false;
    }

    /**
     * Method for transferring a car to another person
     * if an owner transfers all of his cars, he will not be listed in the register anymore
     * @param rz ID of car to be transferred
     * @param nName name of new owner
     * @param nSurname surname of new owner
     * @return true if car was transferred successfully
     * @return false if car is not in the database, or if the former and new owner is the same person
     */
    bool Transfer(const string &rz, const string &nName, const string &nSurname) {
        CRecord new_record(nSurname, nName, rz);
        //check if the car is in the database
        for (auto it = reg.begin(); it < reg.end(); ++it) {
            if (it->rz == new_record.rz) {
                if (*it == new_record) return false; //car can't be transferred to the same person
                //remove the old record and insert a new one
                reg.erase(it);
                auto new_position = lower_bound(reg.begin(), reg.end(), new_record);
                reg.insert(new_position, new_record);
                return true;
            }
        }
        //car is not in the database, can't transfer
        return false;
    }

    /**
     * finds a person identified by his name and surname and lists all of his cars
     * @param name
     * @param surname
     * @return list of cars owned by wanted person
     */
    CCarList ListCars(const string &name, const string &surname) const {
        CRecord wanted(surname, name);
        //find the beginning of the list
        auto begin = lower_bound(reg.begin(), reg.end(), wanted);
        //find the end of the list
        auto end = begin;
        while (end != reg.end()) {
            if (end->name != wanted.name || end->surname != wanted.surname) break;
            end++;
        }
        //if the person was not found, both begin and end will be equal, and AtEnd must be checked first
        return CCarList(&(*begin), &(*end));
    }

    /**
     * Method for counting cars owned by a single person
     * @param name name of wanted person
     * @param surname surname of wanted person
     * @return number of cars owned by a person
     */
    int CountCars(const string &name, const string &surname) const {
        //create a list of cars and then count the records
        CCarList list = ListCars(name, surname);
        int count = 0;
        while (!list.AtEnd()) {
            count++;
            list.Next();
        }
        return count;
    }

    /**
     * Method for listing all car owners in the register
     * @return list of car owners
     */
    CPersonList ListPersons() const {
        return CPersonList(&(*reg.begin()), &(*reg.end()));
    }

private:
    /*the vector is sorted primarily by surname and then by name in ascending order.
      this makes it possible to create CCarList and CPersonList easily just by referencing this vector.
      On the other hand, searching for rz has to be linear. */
    vector<CRecord> reg;
};

#ifndef __PROGTEST__

bool checkPerson(CRegister &r,
                 const string &name,
                 const string &surname,
                 vector<string> result) {
    for (CCarList l = r.ListCars(name, surname); !l.AtEnd(); l.Next()) {
        auto pos = find(result.begin(), result.end(), l.RZ());
        if (pos == result.end())
            return false;
        result.erase(pos);
    }
    return result.size() == 0;
}

/**
 * driver code for testing
 */
int main() {
    CRegister b1;
    assert ( b1 . AddCar ( "ABC-12-34", "John", "Smith" ) == true );
    assert ( b1 . AddCar ( "ABC-32-22", "John", "Hacker" ) == true );
    assert ( b1 . AddCar ( "XYZ-11-22", "Peter", "Smith" ) == true );
    assert ( b1 . CountCars ( "John", "Hacker" ) == 1 );
    assert ( checkPerson ( b1, "John", "Hacker", { "ABC-32-22" } ) );
    assert ( b1 . Transfer ( "XYZ-11-22", "John", "Hacker" ) == true );
    assert ( b1 . AddCar ( "XYZ-99-88", "John", "Smith" ) == true );
    assert ( b1 . CountCars ( "John", "Smith" ) == 2 );
    assert ( checkPerson ( b1, "John", "Smith", { "ABC-12-34", "XYZ-99-88" } ) );
    assert ( b1 . CountCars ( "John", "Hacker" ) == 2 );
    assert ( checkPerson ( b1, "John", "Hacker", { "ABC-32-22", "XYZ-11-22" } ) );
    assert ( b1 . CountCars ( "Peter", "Smith" ) == 0 );
    assert ( checkPerson ( b1, "Peter", "Smith", {  } ) );
    assert ( b1 . Transfer ( "XYZ-11-22", "Jane", "Black" ) == true );
    assert ( b1 . CountCars ( "Jane", "Black" ) == 1 );
    assert ( checkPerson ( b1, "Jane", "Black", { "XYZ-11-22" } ) );
    assert ( b1 . CountCars ( "John", "Smith" ) == 2 );
    assert ( checkPerson ( b1, "John", "Smith", { "ABC-12-34", "XYZ-99-88" } ) );
    assert ( b1 . DelCar ( "XYZ-11-22" ) == true );
    assert ( b1 . CountCars ( "Jane", "Black" ) == 0 );
    assert ( checkPerson ( b1, "Jane", "Black", {  } ) );
    assert ( b1 . AddCar ( "XYZ-11-22", "George", "White" ) == true );
    CPersonList i1 = b1 . ListPersons ();
    assert ( ! i1 . AtEnd () && i1 . Surname () == "Hacker" && i1 . Name () == "John" );
    assert ( checkPerson ( b1, "John", "Hacker", { "ABC-32-22" } ) );
    i1 . Next ();
    assert ( ! i1 . AtEnd () && i1 . Surname () == "Smith" && i1 . Name () == "John" );
    assert ( checkPerson ( b1, "John", "Smith", { "ABC-12-34", "XYZ-99-88" } ) );
    i1 . Next ();
    assert ( ! i1 . AtEnd () && i1 . Surname () == "White" && i1 . Name () == "George" );
    assert ( checkPerson ( b1, "George", "White", { "XYZ-11-22" } ) );
    i1 . Next ();
    assert ( i1 . AtEnd () );

    CRegister b2;
    assert ( b2 . AddCar ( "ABC-12-34", "John", "Smith" ) == true );
    assert ( b2 . AddCar ( "ABC-32-22", "John", "Hacker" ) == true );
    assert ( b2 . AddCar ( "XYZ-11-22", "Peter", "Smith" ) == true );
    assert ( b2 . AddCar ( "XYZ-11-22", "Jane", "Black" ) == false );
    assert ( b2 . DelCar ( "AAA-11-11" ) == false );
    assert ( b2 . Transfer ( "BBB-99-99", "John", "Smith" ) == false );
    assert ( b2 . Transfer ( "ABC-12-34", "John", "Smith" ) == false );
    assert ( b2 . CountCars ( "George", "White" ) == 0 );
    assert ( checkPerson ( b2, "George", "White", {  } ) );
    CPersonList i2 = b2 . ListPersons ();
    assert ( ! i2 . AtEnd () && i2 . Surname () == "Hacker" && i2 . Name () == "John" );
    assert ( checkPerson ( b2, "John", "Hacker", { "ABC-32-22" } ) );
    i2 . Next ();
    assert ( ! i2 . AtEnd () && i2 . Surname () == "Smith" && i2 . Name () == "John" );
    assert ( checkPerson ( b2, "John", "Smith", { "ABC-12-34" } ) );
    i2 . Next ();
    assert ( ! i2 . AtEnd () && i2 . Surname () == "Smith" && i2 . Name () == "Peter" );
    assert ( checkPerson ( b2, "Peter", "Smith", { "XYZ-11-22" } ) );
    i2 . Next ();
    assert ( i2 . AtEnd () );
    return 0;
}

#endif /* __PROGTEST__ */
