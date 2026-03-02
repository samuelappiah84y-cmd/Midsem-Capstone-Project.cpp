#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <algorithm>
#include <filesystem>
#include <direct.h>
#include <sys/stat.h>

using namespace std;

const string DATA_DIR = "data/";
const string STUDENTS_FILE = "data/students.txt";
const string SESSION_DIR = "data/sessions/";

struct Student {
    string index;
    string name;
};

struct Attendance {
    string indexNumber;
    char status;
};

class LectureSession {
public:
    string courseCode;
    string date;
    string startTime;
    int durationHours;
    vector<Attendance> records;

    LectureSession() : durationHours(0) {}

    bool load(const string& fullpath) {
        ifstream fin(fullpath);
        if (!fin) return false;

        getline(fin, courseCode);
        getline(fin, date);
        getline(fin, startTime);
        fin >> durationHours;
        fin.ignore();

        string idx, stat_str;
        while (fin >> idx >> stat_str) {
            records.push_back({idx, stat_str[0]});
        }
        fin.close();
        return true;
    }

    void save() const {
        string filename = courseCode + "_" + date + "_" + startTime + ".txt";
        ofstream fout(SESSION_DIR + filename);
        fout << courseCode << "\n" << date << "\n" << startTime << "\n" << durationHours << "\n";
        for (const auto& rec : records) {
            fout << rec.indexNumber << " " << rec.status << "\n";
        }
        fout.close();
    }

    void mark(const string& idx, char stat) {
        stat = toupper(stat);
        if (stat != 'P' && stat != 'A' && stat != 'L') {
            cout << "Invalid status. Use P, A or L.\n";
            return;
        }

        for (auto& r : records) {
            if (r.indexNumber == idx) {
                r.status = stat;
                cout << "Updated attendance for " << idx << " → " << stat << "\n";
                return;
            }
        }

        records.push_back({idx, stat});
        cout << "Marked " << idx << " as " << stat << "\n";
    }

    void showList(const vector<Student>& allStudents) {
        cout << "\nAttendance for " << courseCode << " on " << date << " " << startTime
             << " (" << durationHours << " hrs)\n";
        cout << string(65, '-') << "\n";
        cout << left << setw(12) << "Index" << setw(28) << "Name" << "Status\n";
        cout << string(65, '-') << "\n";

        map<string, char> attMap;
        for (const auto& r : records) attMap[r.indexNumber] = r.status;

        for (const auto& student : allStudents) {
            auto it = attMap.find(student.index);
            char st = (it != attMap.end()) ? it->second : '-';
            cout << left << setw(12) << student.index << setw(28) << student.name << st << "\n";
        }
        cout << string(65, '-') << "\n";
    }

    void showSummary() {
        int p = 0, a = 0, l = 0, total = records.size();
        for (const auto& r : records) {
            if (r.status == 'P') p++;
            else if (r.status == 'A') a++;
            else if (r.status == 'L') l++;
        }
        cout << "\nSummary for " << courseCode << " on " << date << ":\n";
        cout << "Present : " << p << "\n";
        cout << "Absent  : " << a << "\n";
        cout << "Late    : " << l << "\n";
        cout << "Total marked : " << total << "\n\n";
    }
};

vector<Student> students;

bool directoryExists(const string& dirName) {
    struct stat buffer;
    return (stat(dirName.c_str(), &buffer) == 0);
}

void initDirectories() {
    if (!directoryExists(DATA_DIR)) _mkdir(DATA_DIR.c_str());
    if (!directoryExists(SESSION_DIR)) _mkdir(SESSION_DIR.c_str());
}

void loadStudents() {
    ifstream fin(STUDENTS_FILE);
    if (!fin) return;
    string idx, name;
    while (fin >> idx && getline(fin, name)) {
        name.erase(0, name.find_first_not_of(" "));
        students.push_back({idx, name});
    }
    fin.close();
}

void saveStudents() {
    ofstream fout(STUDENTS_FILE);
    for (const auto& s : students) {
        fout << s.index << " " << s.name << "\n";
    }
    fout.close();
}

bool studentExists(const string& idx) {
    return any_of(students.begin(), students.end(), 
                  [&](const Student& s) { return s.index == idx; });
}

void registerStudent() {
    string idx, name;
    cout << "Student Index: "; getline(cin, idx);
    if (studentExists(idx)) {
        cout << "Student already registered.\n";
        return;
    }
    cout << "Student Name: "; getline(cin, name);
    students.push_back({idx, name});
    cout << "Student registered successfully.\n";
}

void viewAllStudents() {
    if (students.empty()) {
        cout << "No students registered.\n";
        return;
    }
    cout << "\n" << left << setw(12) << "Index" << "Name\n";
    cout << string(40, '-') << "\n";
    for (const auto& s : students) {
        cout << left << setw(12) << s.index << s.name << "\n";
    }
}

void searchStudent() {
    string idx;
    cout << "Enter student index: "; getline(cin, idx);
    for (const auto& s : students) {
        if (s.index == idx) {
            cout << "Found: " << s.index << " - " << s.name << "\n";
            return;
        }
    }
    cout << "Student not found.\n";
}

void createSession() {
    string courseCode, date, startTime;
    int hours;
    cout << "Course Code (e.g. EEE227): "; getline(cin, courseCode);
    cout << "Date (YYYY-MM-DD): "; getline(cin, date);
    cout << "Start Time (HH-MM): "; getline(cin, startTime);
    cout << "Duration (hours): "; cin >> hours; cin.ignore();

    LectureSession sess;
    sess.courseCode = courseCode;
    sess.date = date;
    sess.startTime = startTime;
    sess.durationHours = hours;
    sess.save();
    cout << "Session created: " << courseCode << " on " << date << "\n";
}

void markAttendanceMenu() {
    string filename;
    cout << "Enter session filename (e.g. EEE227_2025-10-15_09-00.txt): ";
    getline(cin, filename);

    string fullpath = SESSION_DIR + filename;
    LectureSession sess;
    if (!sess.load(fullpath)) {
        cout << "Session file not found.\n";
        return;
    }

    cout << "\nMarking attendance for " << sess.courseCode << " " << sess.date << " " << sess.startTime << "\n";
    cout << "Enter student index (or 'done' to finish)\n";

    while (true) {
        string idx;
        cout << "> "; getline(cin, idx);
        if (idx == "done" || idx == "exit") break;

        if (!studentExists(idx)) {
            cout << "Student not registered.\n";
            continue;
        }

        char stat;
        cout << "Status (P/A/L): "; cin >> stat; cin.ignore();
        sess.mark(idx, stat);
    }

    sess.save();
    cout << "Session saved.\n";
}

void viewSessionList() {
    string filename;
    cout << "Enter session filename: "; getline(cin, filename);
    string fullpath = SESSION_DIR + filename;

    LectureSession sess;
    if (!sess.load(fullpath)) {
        cout << "Cannot open session.\n";
        return;
    }
    sess.showList(students);
}

void viewSessionSummary() {
    string filename;
    cout << "Enter session filename: "; getline(cin, filename);
    string fullpath = SESSION_DIR + filename;

    LectureSession sess;
    if (!sess.load(fullpath)) {
        cout << "Cannot open session.\n";
        return;
    }
    sess.showSummary();
}

int main() {
    initDirectories();
    loadStudents();

    int choice;
    do {
        cout << "\n=== Digital Attendance System ===\n"
             << "1. Register new student\n"
             << "2. View all students\n"
             << "3. Search student by index\n"
             << "4. Create new lecture session\n"
             << "5. Mark / update attendance\n"
             << "6. View attendance list for session\n"
             << "7. View attendance summary for session\n"
             << "0. Exit\n"
             << "Choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice) {
            case 1: registerStudent();      break;
            case 2: viewAllStudents();      break;
            case 3: searchStudent();        break;
            case 4: createSession();        break;
            case 5: markAttendanceMenu();   break;
            case 6: viewSessionList();      break;
            case 7: viewSessionSummary();   break;
            case 0: cout << "Goodbye.\n";   break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    saveStudents();
    return 0;
}