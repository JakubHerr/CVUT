#ifndef __PROGTEST__

#include <cassert>
#include <string>
#include <iostream>
#include <sstream>
#include <set>
#include <list>
#include <map>
#include <algorithm>
#include <vector>

using namespace std;

class CResult {
public:
    CResult(const string &name, unsigned int studentID, const string &test, int result)
            : m_Name(name), m_StudentID(studentID), m_Test(test), m_Result(result) {}

    bool operator==(const CResult &other) const {
        return m_Name == other.m_Name
               && m_StudentID == other.m_StudentID
               && m_Test == other.m_Test
               && m_Result == other.m_Result;
    }

    string m_Name;
    unsigned int m_StudentID;
    string m_Test;
    int m_Result;
};

#endif /* __PROGTEST__ */

/**
 * encapsulates the result of a student from a test
 */
struct TResult {
    unsigned int student_id;
    bool is_graded = false;
    unsigned int assessment_order = 0;
    int grade = 0;

    explicit TResult(unsigned int id) : student_id(id) {}

    bool operator<(const TResult &rhs) const {
        return student_id < rhs.student_id;
    }
};

/**
*  encapsulates a single test\n
*  keeps track of students signed up for the test and their results
*/
class TTest {
private:
    const string m_TestName;
    set<TResult> results;
    unsigned int grade_order = 0; //keeps track of who was graded first
public:
    explicit TTest(const string &test_name) : m_TestName(test_name) {}

    //attempts to add student to a test
    //returns false if student was already signed up
    bool addStudent(unsigned int studentID) {
        auto test = results.emplace(studentID);
        return test.second;
    }

    //attempts to grade a student
    //returns false if he is already graded or not signed up for the test
    bool gradeStudent(unsigned int studentID, int grade) {
        for (auto result: results) {
            if ((result.student_id == studentID) && !result.is_graded) {
                result.is_graded = true;
                result.grade = grade;
                result.assessment_order = grade_order++;
                //test must be reinserted, result is a copy
                results.erase(result);
                results.insert(result);
                return true;
            }
        }
        return false;
    }

    //returns a set of students not yet graded from the test
    set<unsigned int> getUngraded() const {
        set<unsigned int> ungraded;
        for (const auto &result: results) {
            if (!result.is_graded) ungraded.insert(result.student_id);
        }
        return ungraded;
    }

    //returns a set of all graded results
    vector<TResult> getResults() const {
        vector<TResult> graded;
        for (auto result: results) {
            if (result.is_graded) graded.emplace_back(result);
        }
        return graded;
    }

    //test is uniquely identified by its name
    bool operator==(const TTest &rhs) const {
        return m_TestName == rhs.m_TestName;
    }
};

class CExam {
private:
    map<unsigned int, string> students; //map of all students
    map<string, TTest> tests; //map of all tests
    map<string, unsigned int> cards; //map that links cards to students

    /**
     * sorts results with different criteria
     * @param results to sort
     * @param sortBy predefined constant with sorting type
     */
    void sort_results(vector<TResult> &results, int sortBy) const {
        switch (sortBy) {
            case SORT_NONE: { //sort by who was graded first
                std::sort(results.begin(), results.end(),
                          [](TResult &l, TResult r) { return l.assessment_order < r.assessment_order; });
                break;
            }
            case SORT_ID: { //sort by student id ascending
                std::sort(results.begin(), results.end(),
                          [](TResult &l, TResult r) { return l.student_id < r.student_id; });
                break;
            }
            case SORT_NAME: { //sort by student name ascending
                std::sort(results.begin(), results.end(),
                          [this](TResult &l, TResult r) {
                              return students.find(l.student_id)->second < students.find(r.student_id)->second;
                          });
                break;
            }
            case SORT_RESULT: { //sort by test results descending
                std::sort(results.begin(), results.end(),
                          [](TResult &l, TResult r) { return l.grade > r.grade; });
                break;
            }
        }
    }
public:
    //parameters that results can be sorted by
    static const int SORT_NONE = 0;
    static const int SORT_ID = 1;
    static const int SORT_NAME = 2;
    static const int SORT_RESULT = 3;

    /**
     * loads students and their cards from a file in a format defined by the specification
     * validates that the format is correct and checks duplicates
     * if the format is incorrect, nothing new is added to the database
     * @param cardMap
     * @return
     */
    bool Load(istream &cardMap) {
        string line_buffer; //holds the current line
        map<unsigned int, string> new_students; //holds new students - dumped on error
        map<string, unsigned int> new_cards; //holds new cards - dumped on error
        while (getline(cardMap, line_buffer)) { //while lines are being read (not EOF)

            istringstream line(line_buffer);
            string value;
            char delim;

            //student data
            unsigned int studentID;
            string studentName;

            //card data
            string cardID;
            //read student ID (unsigned int)
            line >> studentID >> delim;

            //read student name (string delimited with :)
            getline(line, value, ':');
            studentName = value;

            //kontrola duplicity student ID
            //return false, if new student is already in set of new students or already present in the database
            if (new_students.find(studentID) != new_students.end() || students.find(studentID) != students.end())
                return false;
            else new_students.emplace(studentID, studentName); //else add new student to set of new students

            //nacteni karet
            while (getline(line, value, ',')) { //while cards are being read from the line
                value.erase(remove(value.begin(), value.end(), ' '), value.end()); //probably removes whitespace
                cardID = value;
                if (cards.find(cardID) != cards.end()
                    || new_cards.find(cardID) != new_cards.end())
                    return false; //return if card is a duplicate
                //vlozeni karty
                new_cards.emplace(cardID, studentID);
            }
        }
        //add new data to database
        students.merge(new_students);
        cards.merge(new_cards);
        return true;
    }

    /**
     * registers a student to a given test. New test is created if it does not exist
     * @param cardID
     * @param testName
     * @return
     */
    bool Register(const string &cardID, const string &testName) {
        auto card = cards.find(cardID);
        if (card == cards.end()) return false; //card is invalid

        unsigned int studentID = card->second;

        auto test = tests.find(testName);
        if (test != tests.end()) return test->second.addStudent(studentID); //result is true if student was added
        else { //test does not exist yet, create a new one
            TTest new_test = TTest(testName);
            new_test.addStudent(studentID);
            tests.emplace(testName, new_test);
            return true;
        }
    }

    /**
     * assigns a grade to a student in a test
     * @param studentID student to grade
     * @param testName
     * @param grade to be added
     * @return true if test exists, the student is signed up for it and was not graded already
     */
    bool Assess(unsigned int studentID, const string &testName, int grade) {
        auto test = tests.find(testName);
        if (test == tests.end()) return false;
        return test->second.gradeStudent(studentID, grade);
    }

    /**
     * takes graded results of a test, sorts them by given criteria and converts them to a list
     * @param testName
     * @param sortBy
     */
    list<CResult> ListTest(const string &testName, int sortBy) const {
        list<CResult> empty;
        auto test = tests.find(testName);
        if (test == tests.end()) return empty; //test not found
        vector<TResult> unsorted_results = test->second.getResults();

        //sort results and add them to list of results
        sort_results(unsorted_results, sortBy);
        list<CResult> final_results;
        for (const auto &element: unsorted_results) {
            final_results.emplace_back(students.find(element.student_id)->second, element.student_id, testName,
                                       element.grade);
        }

        return final_results;
    }

    /**
     * @param testName string id of test
     * @return set of students not yet graded from given test (empty if test does not exist)
     */
    set<unsigned int> ListMissing(const string &testName) const {
        set<unsigned int> empty;
        auto test = tests.find(testName);
        if (test == tests.end()) return empty; //test not found
        return test->second.getUngraded();
    }
};

#ifndef __PROGTEST__
/**
 * driver code for testing purposes only
 */
int main() {
    istringstream iss;
    CExam m;
    iss.clear();
    iss.str("123456:Smith John:er34252456hjsd2451451, 1234151asdfe5123416, asdjklfhq3458235\n"
            "654321:Nowak Jane: 62wtsergtsdfg34\n"
            "456789:Nowak Jane: okjer834d34\n"
            "987:West Peter Thomas:sdswertcvsgncse\n");
    assert (m.Load(iss));
    assert (m.Register("62wtsergtsdfg34", "PA2 - #1"));
    assert (m.Register("62wtsergtsdfg34", "PA2 - #2"));
    assert (m.Register("er34252456hjsd2451451", "PA2 - #1"));
    assert (m.Register("er34252456hjsd2451451", "PA2 - #3"));
    assert (m.Register("sdswertcvsgncse", "PA2 - #1"));
    assert (!m.Register("1234151asdfe5123416", "PA2 - #1"));
    assert (!m.Register("aaaaaaaaaaaa", "PA2 - #1"));
    assert (m.Assess(123456, "PA2 - #1", 50));
    assert (m.Assess(654321, "PA2 - #1", 30));
    assert (m.Assess(654321, "PA2 - #2", 40));
    assert (m.Assess(987, "PA2 - #1", 100));
    assert (!m.Assess(654321, "PA2 - #1", 35)); //student was graded already
    assert (!m.Assess(654321, "PA2 - #3", 35));
    assert (!m.Assess(999999, "PA2 - #1", 35));
    assert (m.ListTest("PA2 - #1", CExam::SORT_RESULT) == (list<CResult>
            {
                    CResult("West Peter Thomas", 987, "PA2 - #1", 100),
                    CResult("Smith John", 123456, "PA2 - #1", 50),
                    CResult("Nowak Jane", 654321, "PA2 - #1", 30)
            }));
    assert (m.ListTest("PA2 - #1", CExam::SORT_NAME) == (list<CResult>
            {
                    CResult("Nowak Jane", 654321, "PA2 - #1", 30),
                    CResult("Smith John", 123456, "PA2 - #1", 50),
                    CResult("West Peter Thomas", 987, "PA2 - #1", 100)
            }));
    assert (m.ListTest("PA2 - #1", CExam::SORT_NONE) == (list<CResult>
            {
                    CResult("Smith John", 123456, "PA2 - #1", 50),
                    CResult("Nowak Jane", 654321, "PA2 - #1", 30),
                    CResult("West Peter Thomas", 987, "PA2 - #1", 100)
            }));
    assert (m.ListTest("PA2 - #1", CExam::SORT_ID) == (list<CResult>
            {
                    CResult("West Peter Thomas", 987, "PA2 - #1", 100),
                    CResult("Smith John", 123456, "PA2 - #1", 50),
                    CResult("Nowak Jane", 654321, "PA2 - #1", 30)
            }));
    assert (m.ListMissing("PA2 - #3") == (set<unsigned int>{123456}));
    iss.clear();

    iss.str("888:Watson Joe:25234sdfgwer52, 234523uio, asdf234235we, 234234234\n");
    assert (m.Load(iss));
    assert (m.Register("234523uio", "PA2 - #1"));
    assert (m.Assess(888, "PA2 - #1", 75));
    iss.clear();

    iss.str("555:Gates Bill:ui2345234sdf\n"
            "888:Watson Joe:2345234634\n");
    assert (!m.Load(iss));
    assert (!m.Register("ui2345234sdf", "PA2 - #1"));
    iss.clear();

    iss.str("555:Gates Bill:ui2345234sdf\n"
            "666:Watson Thomas:okjer834d34\n");
    assert (!m.Load(iss));
    assert (!m.Register("ui2345234sdf", "PA2 - #3"));
    iss.clear();

    iss.str("555:Gates Bill:ui2345234sdf\n"
            "666:Watson Thomas:jer834d3sdf4\n");
    assert (m.Load(iss));
    assert (m.Register("ui2345234sdf", "PA2 - #3"));
    assert (m.ListMissing("PA2 - #3") == (set<unsigned int>{555, 123456}));
    return 0;
}
#endif /* __PROGTEST__ */
