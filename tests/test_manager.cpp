// Minimal dependency-free tests: exits with a non-zero code if any check fails.
#include <cmath>
#include <cstdio>
#include <string>

#include "../src/student_manager.hpp"

static int failures = 0;

#define CHECK(cond)                                                          \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::printf("FAIL  %s:%d  %s\n", __FILE__, __LINE__, #cond);     \
            ++failures;                                                      \
        }                                                                    \
    } while (0)

static bool near(double a, double b) { return std::fabs(a - b) < 1e-9; }

static void testAddRemoveFind() {
    StudentManager m;
    int a = m.add("Ayesha", "Data Science");
    int b = m.add("Bilal", "Data Science");
    CHECK(a == 1 && b == 2);
    CHECK(m.find(a) != nullptr);
    CHECK(m.remove(a));
    CHECK(!m.remove(a));
    CHECK(m.find(a) == nullptr);
    CHECK(m.add("Cara", "CS") == 3);  // ids are never reused
}

static void testGrades() {
    StudentManager m;
    int id = m.add("Ayesha", "DS");
    CHECK(m.addGrade(id, 80));
    CHECK(m.addGrade(id, 100));
    CHECK(!m.addGrade(id, 101));
    CHECK(!m.addGrade(id, -1));
    CHECK(!m.addGrade(99, 50));
    CHECK(near(m.find(id)->average(), 90.0));
}

static void testRankingAndStats() {
    StudentManager m;
    int a = m.add("A", "x"), b = m.add("B", "x"), c = m.add("C", "x"), d = m.add("D-no-grades", "x");
    m.addGrade(a, 70);
    m.addGrade(b, 90);
    m.addGrade(c, 80);
    auto ranked = m.rankedByAverage();
    CHECK(ranked.size() == 4);
    CHECK(ranked[0].id == b && ranked[1].id == c && ranked[2].id == a && ranked[3].id == d);

    Stats s = m.stats();
    CHECK(s.count == 3);
    CHECK(near(s.mean, 80.0));
    CHECK(near(s.median, 80.0));
    CHECK(near(s.min, 70.0) && near(s.max, 90.0));
    CHECK(near(s.stddev, std::sqrt(200.0 / 3.0)));
}

static void testSearch() {
    StudentManager m;
    m.add("Ayesha Khan", "DS");
    m.add("Bilal", "DS");
    CHECK(m.searchByName("KHAN").size() == 1);
    CHECK(m.searchByName("zzz").empty());
}

static void testPersistence() {
    const std::string path = "test_students.csv";
    StudentManager m;
    int a = m.add("Ayesha, with comma", "Data;Science");  // separators are sanitised
    m.addGrade(a, 91.5);
    m.addGrade(a, 78);
    m.add("Bilal", "CS");
    CHECK(m.save(path));

    StudentManager n;
    CHECK(n.load(path));
    CHECK(n.all().size() == 2);
    CHECK(n.find(1) != nullptr && n.find(1)->grades.size() == 2);
    CHECK(near(n.find(1)->average(), 84.75));
    CHECK(n.find(2) != nullptr && n.find(2)->grades.empty());
    CHECK(n.add("Next", "x") == 3);
    std::remove(path.c_str());

    CHECK(!n.load("does_not_exist.csv"));
}

int main() {
    testAddRemoveFind();
    testGrades();
    testRankingAndStats();
    testSearch();
    testPersistence();
    if (failures == 0) std::printf("All tests passed.\n");
    return failures == 0 ? 0 : 1;
}
