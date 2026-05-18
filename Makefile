CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -pedantic -Iincludes
LDFLAGS ?= -lsqlite3
SRC := src/db.cpp src/auth.cpp src/reports.cpp
APP := bin/tour_bureau
TEST_AUTH := build/test_auth
TEST_REPORTS := build/test_reports

.PHONY: all clean test coverage check distcheck run

all: $(APP)

$(APP): $(SRC) src/main.cpp | bin build
	$(CXX) $(CXXFLAGS) $(SRC) src/main.cpp -o $@ $(LDFLAGS)

$(TEST_AUTH): $(SRC) tests/test_auth.cpp | build
	$(CXX) $(CXXFLAGS) --coverage $(SRC) tests/test_auth.cpp -o $@ $(LDFLAGS)

$(TEST_REPORTS): $(SRC) tests/test_reports.cpp | build
	$(CXX) $(CXXFLAGS) --coverage $(SRC) tests/test_reports.cpp -o $@ $(LDFLAGS)

bin build:
	mkdir -p $@

test: $(TEST_AUTH) $(TEST_REPORTS)
	$(TEST_AUTH)
	$(TEST_REPORTS)

coverage: clean test
	gcov build/test_reports-db.gcno build/test_reports-auth.gcno build/test_reports-reports.gcno >/dev/null
	@echo "Coverage files generated: *.gcov"

check: test

distcheck: clean all test

run: all
	$(APP)

clean:
	rm -rf build/*.o build/*.gcda build/*.gcno build/test_* bin/tour_bureau *.gcov build/tour_bureau.sqlite
