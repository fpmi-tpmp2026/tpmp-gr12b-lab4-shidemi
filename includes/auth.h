#pragma once
#ifdef __has_include
#  if __has_include(<sqlite3.h>)
#    include <sqlite3.h>
#  else
#    include "sqlite3_min.h"
#  endif
#else
#  include "sqlite3_min.h"
#endif
#include <string>

enum class Role { ADMIN, CREW };

struct User {
    int         id;
    std::string login;
    Role        role;
    int         crew_id;
};

bool auth_login(sqlite3* db, const std::string& login,
                const std::string& password, User& user);
