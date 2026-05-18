#ifndef TOUR_DB_H
#define TOUR_DB_H

#include <sqlite3.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

using Row = std::map<std::string, std::string>;

class Database {
 public:
  explicit Database(const std::string& path);
  ~Database();

  Database(const Database&) = delete;
  Database& operator=(const Database&) = delete;

  void exec(const std::string& sql) const;
  std::vector<Row> query(const std::string& sql) const;
  sqlite3* handle() const { return db_; }

 private:
  sqlite3* db_;
};

void initializeDatabase(Database& db);
int scalarInt(Database& db, const std::string& sql);
double scalarDouble(Database& db, const std::string& sql);

#endif
