#include <iomanip>
#include <iostream>
#include <limits>
#include <string>

#include "student_manager.hpp"

namespace {

const char* kDataFile = "students.csv";

void clearLine() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

bool readInt(const std::string& prompt, int& out) {
    std::cout << prompt;
    if (std::cin >> out) {
        clearLine();
        return true;
    }
    if (std::cin.eof()) return false;
    clearLine();
    std::cout << "  Please enter a whole number.\n";
    return false;
}

bool readDouble(const std::string& prompt, double& out) {
    std::cout << prompt;
    if (std::cin >> out) {
        clearLine();
        return true;
    }
    if (std::cin.eof()) return false;
    clearLine();
    std::cout << "  Please enter a number.\n";
    return false;
}

std::string readLine(const std::string& prompt) {
    std::cout << prompt;
    std::string s;
    std::getline(std::cin, s);
    return s;
}

void printTable(const std::vector<Student>& list) {
    if (list.empty()) {
        std::cout << "  (no students)\n";
        return;
    }
    std::cout << std::left << std::setw(5) << "ID" << std::setw(22) << "Name" << std::setw(20) << "Course"
              << std::setw(8) << "Grades" << "Average\n";
    std::cout << std::string(62, '-') << '\n';
    for (const auto& s : list) {
        std::cout << std::left << std::setw(5) << s.id << std::setw(22) << s.name.substr(0, 21) << std::setw(20)
                  << s.course.substr(0, 19) << std::setw(8) << s.grades.size();
        if (s.grades.empty()) std::cout << "-\n";
        else std::cout << std::fixed << std::setprecision(2) << s.average() << '\n';
    }
}

void printStats(const Stats& st) {
    if (st.count == 0) {
        std::cout << "  No grades recorded yet.\n";
        return;
    }
    std::cout << std::fixed << std::setprecision(2) << "  Students with grades : " << st.count << '\n'
              << "  Mean average         : " << st.mean << '\n'
              << "  Median average       : " << st.median << '\n'
              << "  Std deviation        : " << st.stddev << '\n'
              << "  Lowest / Highest     : " << st.min << " / " << st.max << '\n';
}

void menu() {
    std::cout << "\n=== Student Management System ===\n"
                 " 1) Add student        5) Search by name\n"
                 " 2) Remove student     6) Ranking (by average)\n"
                 " 3) Add grade          7) Class statistics\n"
                 " 4) List students      8) Save and exit\n"
                 " 0) Exit without saving\n";
}

}  // namespace

int main() {
    StudentManager mgr;
    if (mgr.load(kDataFile)) std::cout << "Loaded " << mgr.all().size() << " student(s) from " << kDataFile << ".\n";

    for (;;) {
        menu();
        int choice = 0;
        if (!readInt("Choose: ", choice)) {
            if (std::cin.eof()) break;
            continue;
        }
        if (choice == 0) break;
        if (choice == 8) {
            std::cout << (mgr.save(kDataFile) ? "Saved to " : "Could not write ") << kDataFile << ".\n";
            break;
        }
        switch (choice) {
            case 1: {
                std::string name = readLine("Name: ");
                std::string course = readLine("Course: ");
                if (name.empty()) {
                    std::cout << "  Name cannot be empty.\n";
                    break;
                }
                std::cout << "  Added with id " << mgr.add(name, course) << ".\n";
                break;
            }
            case 2: {
                int id = 0;
                if (!readInt("Student id: ", id)) break;
                std::cout << (mgr.remove(id) ? "  Removed.\n" : "  No student with that id.\n");
                break;
            }
            case 3: {
                int id = 0;
                double g = 0.0;
                if (!readInt("Student id: ", id) || !readDouble("Grade (0-100): ", g)) break;
                std::cout << (mgr.addGrade(id, g) ? "  Grade recorded.\n" : "  Unknown id or grade out of range.\n");
                break;
            }
            case 4: printTable(mgr.all()); break;
            case 5: printTable(mgr.searchByName(readLine("Search: "))); break;
            case 6: printTable(mgr.rankedByAverage()); break;
            case 7: printStats(mgr.stats()); break;
            default: std::cout << "  Unknown option.\n";
        }
    }
    return 0;
}
