#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <algorithm>
#include <direct.h>     // Windows: _mkdir
#include <sys/stat.h>   // for directory check

using namespace std;

// ────────────────────────────────────────────────
// Simple structure for one attendance record
// ────────────────────────────────────────────────
struct Attendance {
    string indexNumber;
    char status;   // 'P' = Present, 'A' = Absent, 'L' = Late
};

// ────────────────────────────────────────────────
// One lecture session
// ────────────────────────────────────────────────
class LectureSession {
public:
    string courseCode;
    string date;        // YYYY-MM-DD
    string startTime;   // HH:MM
    int durationHours;

    vector<Attendance> records;

    string getFilename() const {
        string fname = "data/sessions/" + courseCode + "_" + date + "_" + startTime + ".txt";
        // Replace : with - in time
        replace(fname.begin(), fname.end(), ':', '-');
        return fname;
    }

    void save() const {
        ofstream file(getFilename());
        if (!file.is_open()) {
            cout << "Error: Cannot save session file.\n";
            return;
        }

        file << courseCode << "," << date << "," << startTime << "," << durationHours << "\n";

        for (const auto& rec : records) {
            file << rec.indexNumber << "," << rec.status << "\n";
        }
        file.close();
    }

    bool load(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) return false;

        string line;
        getline(file, line);
        stringstream header(line);
        getline(header, courseCode, ',');
        getline(header, date, ',');
        getline(header, startTime, ',');
        header >> durationHours;

        records.clear();
        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            Attendance att;
            getline(ss, att.indexNumber, ',');
            string stat;
            getline(ss, stat);
            if (!stat.empty()) att.status = stat[0];
            records.push_back(att);
        }
        file.close();
        return true;
    }

    void mark(const string& idx, char stat) {
        // normalize input
        stat = toupper(stat);

        if (stat != 'P' && stat != 'A' && stat != 'L') {
            cout << "Invalid status. Use P, A or L.\n";
            return;
        }

        // update if exists
        for (auto& r : records) {
            if (r.indexNumber == idx) {
                r.status = stat;
                cout << "Updated attendance for " << idx << " → " << stat << "\n";
                return;
            }
        }

        // add new
        records.push_back({idx, stat});
        cout << "Marked " << idx << " as " << stat << "\n";
    }

    void showList(const vector<string>& allStudents) const {
        cout << "\nAttendance for " << courseCode << " on " << date << " " << startTime << " (" << durationHours << " hrs)\n";
        cout << string(65, '-') << "\n";
        cout << left << setw(12) << "Index" << setw(28) << "Name" << "Status\n";
        cout << string(65, '-') << "\n";

        map<string, char> attMap;
        for (const auto& r : records) {
            attMap[r.indexNumber] = r.status;
        }

        for (const string& idx : allStudents) {
            auto it = attMap.find(idx);
            char st = (it != attMap.end()) ? it->second : '-';
            cout << left << setw(12) << idx << "  " << st << "\n";
        }
        cout << string(65, '-') << "\n";
    }

    void showSummary() const {
        int p = 0, a = 0, l = 0, total = records.size();
        for (const auto& r : records) {
            if (r.status == 'P') p++;
            else if (r.status == 'A') a++;
            else if (r.status == 'L') l++;
        }

        cout << "\nSummary:\n";
        cout << "Present : " << p << "\n";
        cout << "Absent  : " << a << "\n";
        cout << "Late    : " << l << "\n";
        cout << "Total marked : " << total << "\n\n";
    }
};

// ────────────────────────────────────────────────
// Global data
// ────────────────────────────────────────────────
vector<string> studentIndexes;          // only index numbers (sorted)
map<string, string> studentNames;       // index → name

const string STUDENT_FILE = "data/students.txt";
const string SESSION_DIR  = "data/sessions/";

// ────────────────────────────────────────────────
// Helper functions
// ────────────────────────────────────────────────
bool directoryExists(const string& dir) {
    struct stat info;
    if (stat(dir.c_str(), &info) != 0) return false;
    return (info.st_mode & S_IFDIR) != 0;
}

void initDirectories() {
    if (!directoryExists("data"))        _mkdir("data");
    if (!directoryExists(SESSION_DIR))   _mkdir(SESSION_DIR.c_str());
}

void loadStudents() {
    ifstream file(STUDENT_FILE);
    if (!file.is_open()) return;

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string idx, name;
        getline(ss, idx, ',');
        getline(ss, name);
        if (!idx.empty()) {
            studentIndexes.push_back(idx);
            studentNames[idx] = name.empty() ? "Unknown" : name;
        }
    }
    file.close();

    sort(studentIndexes.begin(), studentIndexes.end());
}

void saveStudents() {
    ofstream file(STUDENT_FILE);
    if (!file.is_open()) {
        cout << "Cannot save students file!\n";
        return;
    }

    for (const string& idx : studentIndexes) {
        file << idx << "," << studentNames[idx] << "\n";
    }
    file.close();
}

bool studentExists(const string& idx) {
    return studentNames.find(idx) != studentNames.end();
}

void registerStudent() {
    string idx, name;
    cout << "Enter student index number (e.g. EE20045): ";
    getline(cin, idx);
    if (idx.empty()) {
        cout << "Index cannot be empty.\n";
        return;
    }

    if (studentExists(idx)) {
        cout << "Student " << idx << " already registered.\n";
        return;
    }

    cout << "Enter full name: ";
    getline(cin, name);
    if (name.empty()) name = "Unknown";

    studentIndexes.push_back(idx);
    studentNames[idx] = name;
    sort(studentIndexes.begin(), studentIndexes.end());

    saveStudents();
    cout << "Student " << idx << " registered.\n";
}

void viewAllStudents() {
    if (studentIndexes.empty()) {
        cout << "No students registered yet.\n";
        return;
    }

    cout << "\nRegistered students:\n";
    cout << string(50, '-') << "\n";
    cout << left << setw(14) << "Index" << "Name\n";
    cout << string(50, '-') << "\n";

    for (const string& idx : studentIndexes) {
        cout << left << setw(14) << idx << studentNames[idx] << "\n";
    }
    cout << string(50, '-') << "\n";
}

void searchStudent() {
    string idx;
    cout << "Enter index number to search: ";
    getline(cin, idx);

    if (studentExists(idx)) {
        cout << "\nFound: " << idx << " - " << studentNames[idx] << "\n";
    } else {
        cout << "Student " << idx << " not found.\n";
    }
}

LectureSession createSession() {
    LectureSession s;

    cout << "Course code (e.g. EEE227): ";
    getline(cin, s.courseCode);

    cout << "Date (YYYY-MM-DD): ";
    getline(cin, s.date);

    cout << "Start time (HH:MM): ";
    getline(cin, s.startTime);

    cout << "Duration in hours: ";
    cin >> s.durationHours;
    cin.ignore();

    cout << "\nSession created (empty attendance).\n";
    s.save();

    return s;
}

void markAttendanceMenu() {
    string filename;
    cout << "Enter session filename (without path, e.g. EEE227_2025-10-15_09-00.txt): ";
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
        cout << "> ";
        getline(cin, idx);
        if (idx == "done" || idx == "exit") break;

        if (!studentExists(idx)) {
            cout << "Student not registered.\n";
            continue;
        }

        char stat;
        cout << "Status (P/A/L): ";
        cin >> stat;
        cin.ignore();

        sess.mark(idx, stat);
    }

    sess.save();
    cout << "Session saved.\n";
}

void viewSessionList() {
    string filename;
    cout << "Enter session filename: ";
    getline(cin, filename);

    string fullpath = SESSION_DIR + filename;

    LectureSession sess;
    if (!sess.load(fullpath)) {
        cout << "Cannot open session.\n";
        return;
    }

    sess.showList(studentIndexes);
}

void viewSessionSummary() {
    string filename;
    cout << "Enter session filename: ";
    getline(cin, filename);

    string fullpath = SESSION_DIR + filename;

    LectureSession sess;
    if (!sess.load(fullpath)) {
        cout << "Cannot open session.\n";
        return;
    }

    sess.showSummary();
}

// ────────────────────────────────────────────────
// Main menu
// ────────────────────────────────────────────────
int main() {
    initDirectories();
    loadStudents();

    int choice;
    do {
        cout << "\n=== Digital Attendance System ===\n";
        cout << "1. Register new student\n";
        cout << "2. View all students\n";
        cout << "3. Search student by index\n";
        cout << "4. Create new lecture session\n";
        cout << "5. Mark / update attendance\n";
        cout << "6. View attendance list for session\n";
        cout << "7. View attendance summary for session\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();   // consume newline

        switch (choice) {
            case 1: registerStudent(); break;
            case 2: viewAllStudents(); break;
            case 3: searchStudent();   break;
            case 4: createSession();   break;
            case 5: markAttendanceMenu(); break;
            case 6: viewSessionList(); break;
            case 7: viewSessionSummary(); break;
            case 0: cout << "Goodbye.\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 0);

    saveStudents();
    return 0;
}