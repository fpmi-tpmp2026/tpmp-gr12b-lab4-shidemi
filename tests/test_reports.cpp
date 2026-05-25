#include <cassert>
#include <iostream>
#include <stdexcept>
#include "db.h"
#include "auth.h"
#include "reports.h"

static sqlite3* setup() {
    sqlite3* db = db_open(":memory:");
    db_init(db); db_seed(db); return db;
}

void test_bus_summary() {
    sqlite3* db = setup();
    report_bus_summary(db, 1);
    db_close(db); std::cout << "[PASS] test_bus_summary\n";
}
void test_bus_trips() {
    sqlite3* db = setup();
    report_bus_trips(db, 1, "2024-01-01", "2024-12-31");
    db_close(db); std::cout << "[PASS] test_bus_trips\n";
}
void test_crew_earnings_admin() {
    sqlite3* db = setup();
    User u; u.role=Role::ADMIN; u.crew_id=0;
    report_crew_earnings(db,"2024-01-01","2024-12-31",u);
    db_close(db); std::cout << "[PASS] test_crew_earnings_admin\n";
}
void test_crew_earnings_crew() {
    sqlite3* db = setup();
    User u; u.role=Role::CREW; u.crew_id=1;
    report_crew_earnings(db,"2024-01-01","2024-12-31",u);
    db_close(db); std::cout << "[PASS] test_crew_earnings_crew\n";
}
void test_most_expensive_route() {
    sqlite3* db = setup();
    report_most_expensive_route(db);
    db_close(db); std::cout << "[PASS] test_most_expensive_route\n";
}
void test_max_mileage_bus() {
    sqlite3* db = setup();
    report_max_mileage_bus(db);
    db_close(db); std::cout << "[PASS] test_max_mileage_bus\n";
}
void test_trip_add() {
    sqlite3* db = setup();
    trip_add(db, 1, "2024-05-01", "2024-05-03", "Золотое кольцо", 30, 1500.0);
    sqlite3_stmt* s=nullptr;
    sqlite3_prepare_v2(db,"SELECT COUNT(*) FROM TOURS_TRIPS WHERE dep_date='2024-05-01';",-1,&s,nullptr);
    sqlite3_step(s);
    assert(sqlite3_column_int(s,0)==1);
    sqlite3_finalize(s); db_close(db);
    std::cout << "[PASS] test_trip_add\n";
}
void test_trip_update() {
    sqlite3* db = setup();
    trip_update_passengers(db, 1, 99);
    sqlite3_stmt* s=nullptr;
    sqlite3_prepare_v2(db,"SELECT passengers FROM TOURS_TRIPS WHERE trip_id=1;",-1,&s,nullptr);
    sqlite3_step(s);
    assert(sqlite3_column_int(s,0)==99);
    sqlite3_finalize(s); db_close(db);
    std::cout << "[PASS] test_trip_update\n";
}
void test_trip_delete() {
    sqlite3* db = setup();
    trip_delete(db, 1);
    sqlite3_stmt* s=nullptr;
    sqlite3_prepare_v2(db,"SELECT COUNT(*) FROM TOURS_TRIPS WHERE trip_id=1;",-1,&s,nullptr);
    sqlite3_step(s);
    assert(sqlite3_column_int(s,0)==0);
    sqlite3_finalize(s); db_close(db);
    std::cout << "[PASS] test_trip_delete\n";
}
void test_invalid_dates() {
    sqlite3* db = setup();
    bool thrown = false;
    try { trip_add(db,1,"2024-12-31","2024-01-01","Золотое кольцо",10,100); }
    catch (const std::exception&) { thrown = true; }
    assert(thrown);
    db_close(db); std::cout << "[PASS] test_invalid_dates\n";
}
void test_fn_earnings_period() {
    sqlite3* db = setup();
    fn_crew_earnings_period(db,"2024-01-01","2024-12-31");
    sqlite3_stmt* s=nullptr;
    sqlite3_prepare_v2(db,"SELECT COUNT(*) FROM TOURS_CREW_EARNINGS;",-1,&s,nullptr);
    sqlite3_step(s);
    assert(sqlite3_column_int(s,0)>0);
    sqlite3_finalize(s); db_close(db);
    std::cout << "[PASS] test_fn_earnings_period\n";
}
void test_fn_earnings_by_date() {
    sqlite3* db = setup();
    fn_crew_earnings_by_date(db, 1, "2024-12-31");
    db_close(db); std::cout << "[PASS] test_fn_earnings_by_date\n";
}

int main() {
    std::cout << "=== REPORTS TESTS ===\n";
    test_bus_summary();
    test_bus_trips();
    test_crew_earnings_admin();
    test_crew_earnings_crew();
    test_most_expensive_route();
    test_max_mileage_bus();
    test_trip_add();
    test_trip_update();
    test_trip_delete();
    test_invalid_dates();
    test_fn_earnings_period();
    test_fn_earnings_by_date();
    std::cout << "All report tests passed!\n";
    return 0;
}
