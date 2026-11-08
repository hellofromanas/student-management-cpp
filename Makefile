CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic -O2

SRC  = src/student_manager.cpp
APP  = student_manager
TEST = test_manager

all: $(APP)

$(APP): src/main.cpp $(SRC) src/student.hpp src/student_manager.hpp
	$(CXX) $(CXXFLAGS) -o $@ src/main.cpp $(SRC)

$(TEST): tests/test_manager.cpp $(SRC) src/student.hpp src/student_manager.hpp
	$(CXX) $(CXXFLAGS) -o $@ tests/test_manager.cpp $(SRC)

test: $(TEST)
	./$(TEST)

clean:
	rm -f $(APP) $(TEST) test_students.csv

.PHONY: all test clean
