#ifndef __PROGTEST__
#include <vector>
#include <cassert>
#include <algorithm>
using namespace std;
#endif /* __PROGTEST__ */
class CEFaceMask;
class CContact;
class CTimeStamp;

class CTimeStamp {
private:
    const int year, month, day, hour, minute, second;
public:
    CTimeStamp(int year, int month, int day, int hour, int minute, int second)
            : year(year), month(month), day(day), hour(hour), minute(minute), second(second) {}

    bool operator<=(const CTimeStamp &date2) const {
        if (year != date2.year) return year < date2.year;
        if (month != date2.month) return month < date2.month;
        if (day != date2.day) return day < date2.day;
        if (hour != date2.hour) return hour < date2.hour;
        if (minute != date2.minute) return minute < date2.minute;
        if (second != date2.second) return second < date2.second;
        return true;
    }
};

class CContact {
private:
    CTimeStamp time;
    int id1, id2;
public:
    CContact(const CTimeStamp time, const int id1, const int id2)
        : time(time), id1(id1), id2(id2) {}

    friend CEFaceMask;
};

class CEFaceMask {
private:
    vector<CContact> database;

    /**
     * @param array to check
     * @param number to look for
     * @return true if array contains number
     */
    static bool contains(vector<int> &array, int number) {
        return std::any_of(array.begin(), array.end(), [&number](int i){ return i == number; });
    }

public:

    /**
     * adds a new contact to the database\n
     * does not check input and duplicates
     * @param contact to be added
     * @return reference to object
     */
    CEFaceMask &addContact(CContact contact) {
        database.push_back(contact);
        return *this;
    }

    /**
     * @param id of a person
     * @return list of unique id's the person was in contact with
     */
    vector<int> listContacts(int id) const {
        vector<int> results;
        for (auto contact: database) {
            if (contact.id1 == id xor contact.id2 == id) {
                int new_infected = contact.id1 == id ? contact.id2 : contact.id1;
                if (!contains(results, new_infected)) results.push_back(new_infected);
            }
        }
        return results;
    }

    /**
     * @param id of a person
     * @return list of unique id's the person was in contact with in interval \<begin,end\>
     */
    vector<int> listContacts(int id, CTimeStamp begin, CTimeStamp end) const {
        vector<int> results;
        for (auto contact: database) {
            if (contact.id1 == id xor contact.id2 == id) {
                if (begin <= contact.time && contact.time <= end) {
                    int new_infected = contact.id1 == id ? contact.id2 : contact.id1;
                    if (!contains(results, new_infected)) results.push_back(new_infected);
                }
            }
        }
        return results;
    }
};

#ifndef __PROGTEST__

int main() {
    CEFaceMask test;

    test.addContact(CContact(CTimeStamp(2021, 1, 10, 12, 40, 10), 123456789, 999888777))
            .addContact(CContact(CTimeStamp(2021, 1, 12, 12, 40, 10), 123456789, 111222333))
            .addContact(CContact(CTimeStamp(2021, 2, 5, 15, 30, 28), 999888777, 555000222))
            .addContact(CContact(CTimeStamp(2021, 2, 21, 18, 0, 0), 123456789, 999888777))
            .addContact(CContact(CTimeStamp(2021, 1, 5, 18, 0, 0), 123456789, 456456456))
            .addContact(CContact(CTimeStamp(2021, 2, 1, 0, 0, 0), 123456789, 123456789));

    assert (test.listContacts(123456789) == (vector<int>{999888777, 111222333, 456456456}));
    assert (test.listContacts(999888777) == (vector<int>{123456789, 555000222}));
    assert (test.listContacts(191919191) == (vector<int>{}));
    assert (test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 0), CTimeStamp(2021, 2, 21, 18, 0, 0)) ==
            (vector<int>{999888777, 111222333, 456456456}));
    assert (test.listContacts(123456789, CTimeStamp(2021, 1, 5, 18, 0, 1), CTimeStamp(2021, 2, 21, 17, 59, 59)) ==
            (vector<int>{999888777, 111222333}));
    assert (test.listContacts(123456789, CTimeStamp(2021, 1, 10, 12, 41, 9), CTimeStamp(2021, 2, 21, 17, 59, 59)) ==
            (vector<int>{111222333}));
    return 0;
}

#endif /* __PROGTEST__ */
