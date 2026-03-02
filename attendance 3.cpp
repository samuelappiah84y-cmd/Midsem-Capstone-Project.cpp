#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <algorithm>
#include <filesystem>  // C++17 directory iteration
#include <direct.h>     // Windows: _mkdir
#include <sys/stat.h>   // for directory check

using namespace std;

const string SESSION_DIR = "data/sessions/";

struct Attendance {
    string indexNumber;
    char status;  // P, A, L
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

    void showList(const vector<string>& allStudents) {
        cout << "\nAttendance for " << courseCode << " on " << date << " " << startTime
             << " (" << durationHours << " hrs)\n";
        cout << string(65, '-') << "\n";
        cout << left << setw(12) << "Index" << setw(28) << "Name" << "Status\n";
        cout << string(65, '-') << "\n";

        map<string, char> attMap;
        for (const auto& r : records) attMap[r.indexNumber] = r.status;

        for (const string& idx : allStudents) {
            auto it = attMap.find(idx);
            char st = (it != attMap.end()) ? it->second : '-';
            cout << left << setw(12) << idx << "  " << st << "\n";
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
        cout << "\nSummary:\n";
        cout << "Present : " << p << "\n";
        cout << "Absent  : " << a << "\n";
        cout << "Late    : " << l << "\n";
        cout << "Total marked : " << total << "\n\n";
    }
};

vector<string> studentIndexes;  // Global list of registered student indexes

bool studentExists(const string& idx) {
    return find(studentIndexes.begin(), studentIndexes.end(), idx) != studentIndexes.end();
}

// ────────────────────────────────────────────────
// Attendance marking menu
// ────────────────────────────────────────────────
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
    sess.showList(studentIndexes);
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
    cout << "Attendance System - Part 3 Test\n";
    return 0;
}