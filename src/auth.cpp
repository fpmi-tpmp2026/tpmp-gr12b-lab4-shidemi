#include "auth.h"

bool auth_login(sqlite3* db, const std::string& login,
                const std::string& password, User& user) {
    const char* sql =
        "SELECT user_id,login,role,COALESCE(crew_id,0) "
        "FROM TOURS_USERS WHERE login=? AND password=?;";
    sqlite3_stmt* s = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &s, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_text(s, 1, login.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(s, 2, password.c_str(), -1, SQLITE_STATIC);
    bool ok = false;
    if (sqlite3_step(s) == SQLITE_ROW) {
        user.id      = sqlite3_column_int(s, 0);
        user.login   = reinterpret_cast<const char*>(sqlite3_column_text(s, 1));
        std::string r = reinterpret_cast<const char*>(sqlite3_column_text(s, 2));
        user.role    = (r == "admin") ? Role::ADMIN : Role::CREW;
        user.crew_id = sqlite3_column_int(s, 3);
        ok = true;
    }
    sqlite3_finalize(s);
    return ok;
}
