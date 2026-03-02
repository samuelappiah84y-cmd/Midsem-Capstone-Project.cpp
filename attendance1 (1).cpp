// =======================
// PART 1 – Student management & persistence
// =======================

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <map>
#include <iomanip>
#include <algorithm>
#include <direct.h>     // Windows: _mkdir
#include <sys/stat.h>

using namespace std;

// ────────────────────────────────────────────────
// Global student data
// ────────────────────────────────────────────────
vector<string> studentIndexes;          // sorted index numbers
map<string, string> studentNames;       // index → name

const string STUDENT_FILE = "data/students.txt";

// ────────────────────────────────────────────────
// Directory & file helpers
// ────────────────────────────────────────────────
bool directoryExists(const string& dir) {
    struct stat info;
    if (stat(dir.c_str(), &info) != 0) return false;
    return (info.st_mode & S_IFDIR) != 0;
}

void initDirectories() {
    if (!directoryExists("data")) _mkdir("data");
}

// ────────────────────────────────────────────────
// Student functions
// ────────────────────────────────────────────────
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
    if (idx.empty()) { cout << "Index cannot be empty.\n"; return; }

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