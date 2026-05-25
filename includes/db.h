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

sqlite3* db_open(const std::string& path);
void     db_close(sqlite3* db);
void     db_init(sqlite3* db);
void     db_seed(sqlite3* db);
