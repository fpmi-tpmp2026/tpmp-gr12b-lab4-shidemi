#include <cassert>
#include <iostream>
#include "db.h"
#include "auth.h"

static sqlite3* setup() {
    sqlite3* db = db_open(":memory:");
    db_init(db); db_seed(db); return db;
}

void test_valid_admin_login() {
    sqlite3* db = setup(); User u;
    assert(auth_login(db,"admin","admin",u));
    assert(u.role == Role::ADMIN);
    assert(u.crew_id == 0);
    db_close(db); std::cout << "[PASS] test_valid_admin_login\n";
}
void test_valid_crew_login() {
    sqlite3* db = setup(); User u;
    assert(auth_login(db,"ivanov","crew123",u));
    assert(u.role == Role::CREW);
    assert(u.crew_id > 0);
    db_close(db); std::cout << "[PASS] test_valid_crew_login\n";
}
void test_wrong_password() {
    sqlite3* db = setup(); User u;
    assert(!auth_login(db,"admin","wrong",u));
    db_close(db); std::cout << "[PASS] test_wrong_password\n";
}
void test_nonexistent_user() {
    sqlite3* db = setup(); User u;
    assert(!auth_login(db,"nobody","pass",u));
    db_close(db); std::cout << "[PASS] test_nonexistent_user\n";
}
void test_empty_credentials() {
    sqlite3* db = setup(); User u;
    assert(!auth_login(db,"","",u));
    db_close(db); std::cout << "[PASS] test_empty_credentials\n";
}

int main() {
    std::cout << "=== AUTH TESTS ===\n";
    test_valid_admin_login();
    test_valid_crew_login();
    test_wrong_password();
    test_nonexistent_user();
    test_empty_credentials();
    std::cout << "All auth tests passed!\n";
    return 0;
}
