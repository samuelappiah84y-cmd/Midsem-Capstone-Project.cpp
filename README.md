 # Digital Attendance System for Hour-Based Lectures

A console-based C++ application designed to replace paper-based attendance taking in lecture halls. It helps reduce disruptions, prevents misplaced records, and allows easy generation of attendance summaries.

**Course:** EEE227 – Introduction to C++ Programming  
**Program:** HND Electrical Engineering (Level 200)  
**Assessment:** Mid-Semester Capstone Project (30%)  
**Submitted by:** Elisha Twum and Appiah Samuel Boateng
                   (01243225D)             (01241322D)
**Date:** February 18, 2026.

## Project Objectives (as per assignment)
- Apply C++ concepts (variables, arrays, functions, classes) to a real engineering problem
- Design a menu-driven console application
- Implement file handling for persistent storage using `<fstream>`
- Demonstrate professional software development practices

## Features Implemented (Matching Assignment Requirements)

### Student Management
- Register new students (index number, name, program)
- View all registered students
- Search students by index number

### Attendance Session Management
- Create a new lecture session (course code, date, start time, duration in hours)
- Mark attendance for students (Present 'P', Absent 'A', Late 'L')
- Update attendance records if needed

### Reports and Summary
- Display full attendance list for a specific session
- Show summary counts: number of Present, Absent, Late, and total marked

### Data Storage
- Student records saved persistently in `data/students.txt`
- Each lecture session saved in its own file in `data/sessions/` (e.g. `EEE227_2026-02-15_10-00.txt`)
- Data persists between program runs (load on startup, save on changes/exit)

## Technologies Used
- C++17 (console application)
- Standard Library: `<iostream>`, `<fstream>`, `<vector>`, `<string>`, `<map>`, `<iomanip>`, `<algorithm>`
- Windows-specific: `<direct.h>` for directory creation, `<winsock2.h>` for hostname (optional)

## How to Compile and Run (Windows / VS Code)
1. Open the project folder in VS Code
2. Open integrated terminal (Ctrl+`)
3. Compile the project:
