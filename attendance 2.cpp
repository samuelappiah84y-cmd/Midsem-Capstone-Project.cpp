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

// =======================
// PART 2 – Lecture session structure & basic persistence
// =======================

// ────────────────────────────────────────────────
// Attendance record (needed already here)
// ────────────────────────────────────────────────
struct Attendance {
    string indexNumber;
    char status;   // 'P','A','L'
};

// ────────────────────────────────────────────────
// Lecture session class – basic version
// ────────────────────────────────────────────────
class LectureSession {
public:
    string courseCode;
    string date;        // YYYY-MM-DD
    string startTime;   // HH:MM
    int durationHours;

    vector<Attendance> records;   // ← will be used later

    string getFilename() const {
        string fname = "data/sessions/" + courseCode + "_" + date + "_" + startTime + ".txt";
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
        // attendance records will be added later
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

        records.clear();   // we'll fill this later
        file.close();
        return true;
    }
};

const string SESSION_DIR = "data/sessions/";

// Check whether a directory exists
bool directoryExists(const string &path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) return false;
    return (info.st_mode & S_IFDIR) != 0;
}

// Ensure required directories exist
void initDirectories() {
    if (!directoryExists("data")) _mkdir("data");
    if (!directoryExists(SESSION_DIR)) _mkdir(SESSION_DIR.c_str());
}

// ────────────────────────────────────────────────
// Create empty session (Week 2 feature)
// ────────────────────────────────────────────────
LectureSession createSession() {
    LectureSession s;

    cout << "Course code (e.g. EEE227): ";     getline(cin, s.courseCode);
    cout << "Date (YYYY-MM-DD): ";             getline(cin, s.date);
    cout << "Start time (HH:MM): ";            getline(cin, s.startTime);
    cout << "Duration in hours: ";             cin >> s.durationHours; cin.ignore();

    initDirectories();  // make sure sessions folder exists

    s.save();
    cout << "\nSession created (empty attendance).\n";
    return s;
}

// Minimal test entry so this translation unit links standalone
int main() {
    initDirectories();
    
    int choice;
    do {
        cout << "\n=== Attendance Session Manager ===\n";
        cout << "1. Create session\n";
        cout << "0. Exit\n";
        cout << "Choice: ";
        cin >> choice;
        cin.ignore();
        
        if (choice == 1) {
            createSession();
        }
        else if (choice == 0) {
            cout << "Goodbye!\n";
        }
        else {
            cout << "Invalid choice.\n";
        }
    } while (choice != 0);
    
    return 0;
}