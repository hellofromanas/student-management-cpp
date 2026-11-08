#include "student_manager.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>

double Student::average() const {
    if (grades.empty()) return 0.0;
    return std::accumulate(grades.begin(), grades.end(), 0.0) / static_cast<double>(grades.size());
}

namespace {

std::string sanitize(std::string s) {
    for (char& c : s) {
        if (c == ',' || c == '\n' || c == '\r' || c == ';') c = ' ';
    }
    return s;
}

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

}  // namespace

int StudentManager::add(const std::string& name, const std::string& course) {
    Student s;
    s.id = nextId_++;
    s.name = sanitize(name);
    s.course = sanitize(course);
    students_.push_back(std::move(s));
    return students_.back().id;
}

bool StudentManager::remove(int id) {
    auto it = std::remove_if(students_.begin(), students_.end(), [id](const Student& s) { return s.id == id; });
    if (it == students_.end()) return false;
    students_.erase(it, students_.end());
    return true;
}

Student* StudentManager::find(int id) {
    for (auto& s : students_) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

const Student* StudentManager::find(int id) const {
    for (const auto& s : students_) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

bool StudentManager::addGrade(int id, double grade) {
    if (!(grade >= 0.0 && grade <= 100.0)) return false;  // also rejects NaN
    Student* s = find(id);
    if (!s) return false;
    s->grades.push_back(grade);
    return true;
}

std::vector<Student> StudentManager::rankedByAverage() const {
    std::vector<Student> out = students_;
    std::stable_sort(out.begin(), out.end(), [](const Student& a, const Student& b) {
        if (a.grades.empty() != b.grades.empty()) return !a.grades.empty();
        return a.average() > b.average();
    });
    return out;
}

std::vector<Student> StudentManager::searchByName(const std::string& query) const {
    std::vector<Student> out;
    const std::string q = lower(query);
    for (const auto& s : students_) {
        if (lower(s.name).find(q) != std::string::npos) out.push_back(s);
    }
    return out;
}

Stats StudentManager::stats() const {
    std::vector<double> avgs;
    for (const auto& s : students_) {
        if (!s.grades.empty()) avgs.push_back(s.average());
    }
    Stats st;
    st.count = avgs.size();
    if (avgs.empty()) return st;

    st.mean = std::accumulate(avgs.begin(), avgs.end(), 0.0) / static_cast<double>(avgs.size());
    std::sort(avgs.begin(), avgs.end());
    st.min = avgs.front();
    st.max = avgs.back();
    const std::size_t n = avgs.size();
    st.median = (n % 2 == 1) ? avgs[n / 2] : (avgs[n / 2 - 1] + avgs[n / 2]) / 2.0;
    double sq = 0.0;
    for (double a : avgs) sq += (a - st.mean) * (a - st.mean);
    st.stddev = std::sqrt(sq / static_cast<double>(n));
    return st;
}

bool StudentManager::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) return false;
    out << "id,name,course,grades\n";
    for (const auto& s : students_) {
        out << s.id << ',' << s.name << ',' << s.course << ',';
        for (std::size_t i = 0; i < s.grades.size(); ++i) {
            if (i) out << ';';
            out << s.grades[i];
        }
        out << '\n';
    }
    return static_cast<bool>(out);
}

bool StudentManager::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) return false;
    std::vector<Student> loaded;
    int maxId = 0;
    std::string line;
    std::getline(in, line);  // header
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        std::vector<std::string> cols;
        std::stringstream ss(line);
        std::string cell;
        while (std::getline(ss, cell, ',')) cols.push_back(cell);
        if (line.back() == ',') cols.push_back("");  // empty trailing "grades" column
        if (cols.size() < 4) return false;
        Student s;
        try {
            s.id = std::stoi(cols[0]);
            std::stringstream gs(cols[3]);
            std::string g;
            while (std::getline(gs, g, ';')) {
                if (!g.empty()) s.grades.push_back(std::stod(g));
            }
        } catch (const std::exception&) {
            return false;  // malformed file: keep current data untouched
        }
        s.name = cols[1];
        s.course = cols[2];
        maxId = std::max(maxId, s.id);
        loaded.push_back(std::move(s));
    }
    students_ = std::move(loaded);
    nextId_ = maxId + 1;
    return true;
}
