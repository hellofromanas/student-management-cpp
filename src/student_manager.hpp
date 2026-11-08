#pragma once
#include <string>
#include <vector>

#include "student.hpp"

// Owns the collection of students and implements all operations on it.
class StudentManager {
public:
    // Adds a student and returns the new unique id. Commas/newlines in text are replaced by spaces
    // so the CSV format stays valid.
    int add(const std::string& name, const std::string& course);

    // Removes a student by id. Returns false when the id does not exist.
    bool remove(int id);

    // Returns nullptr when the id does not exist.
    Student* find(int id);
    const Student* find(int id) const;

    // Adds a grade in [0, 100]. Returns false if the id is unknown or the grade is out of range.
    bool addGrade(int id, double grade);

    const std::vector<Student>& all() const { return students_; }

    // Students ordered by average grade, best first (students without grades go last).
    std::vector<Student> rankedByAverage() const;

    // Case-insensitive substring search on the name.
    std::vector<Student> searchByName(const std::string& query) const;

    Stats stats() const;

    // CSV persistence: id,name,course,g1;g2;g3
    bool save(const std::string& path) const;
    bool load(const std::string& path);

private:
    std::vector<Student> students_;
    int nextId_ = 1;
};
