#pragma once
#include <string>
#include <vector>

// A single student record with a list of grades (0-100).
struct Student {
    int id = 0;
    std::string name;
    std::string course;
    std::vector<double> grades;

    // Mean of all grades; 0.0 when the student has no grades yet.
    double average() const;
};

// Summary statistics over every student's average grade.
struct Stats {
    std::size_t count = 0;     // students that have at least one grade
    double mean = 0.0;
    double median = 0.0;
    double stddev = 0.0;       // population standard deviation
    double min = 0.0;
    double max = 0.0;
};
