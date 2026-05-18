#include "db.h"

#include <stdexcept>

Database::Database(const std::string& path) : db_(nullptr) {
  if (sqlite3_open(path.c_str(), &db_) != SQLITE_OK) {
    std::string message = sqlite3_errmsg(db_);
    sqlite3_close(db_);
    throw std::runtime_error("SQLite open error: " + message);
  }
  exec("PRAGMA foreign_keys = ON;");
}

Database::~Database() {
  if (db_ != nullptr) {
    sqlite3_close(db_);
  }
}

void Database::exec(const std::string& sql) const {
  char* error = nullptr;
  if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &error) != SQLITE_OK) {
    std::string message = error == nullptr ? "unknown SQLite error" : error;
    sqlite3_free(error);
    throw std::runtime_error(message);
  }
}

std::vector<Row> Database::query(const std::string& sql) const {
  sqlite3_stmt* stmt = nullptr;
  if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
    throw std::runtime_error(sqlite3_errmsg(db_));
  }

  std::vector<Row> result;
  while (sqlite3_step(stmt) == SQLITE_ROW) {
    Row row;
    for (int i = 0; i < sqlite3_column_count(stmt); ++i) {
      const char* name = sqlite3_column_name(stmt, i);
      const unsigned char* text = sqlite3_column_text(stmt, i);
      row[name == nullptr ? "" : name] = text == nullptr ? "" : reinterpret_cast<const char*>(text);
    }
    result.push_back(row);
  }

  sqlite3_finalize(stmt);
  return result;
}

void initializeDatabase(Database& db) {
  db.exec(
      "CREATE TABLE IF NOT EXISTS buses ("
      "bus_number INTEGER PRIMARY KEY,"
      "model TEXT NOT NULL,"
      "total_mileage INTEGER NOT NULL CHECK (total_mileage >= 0),"
      "photo_path TEXT);"
      "CREATE TABLE IF NOT EXISTS routes ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "name TEXT NOT NULL UNIQUE,"
      "start_point TEXT NOT NULL,"
      "end_point TEXT NOT NULL,"
      "distance_km INTEGER NOT NULL CHECK (distance_km > 0));"
      "CREATE TABLE IF NOT EXISTS crew_members ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "last_name TEXT NOT NULL,"
      "personnel_number TEXT NOT NULL UNIQUE,"
      "experience_years INTEGER NOT NULL CHECK (experience_years >= 0),"
      "category TEXT NOT NULL,"
      "address TEXT NOT NULL,"
      "birth_year INTEGER NOT NULL CHECK (birth_year BETWEEN 1940 AND 2010),"
      "bus_number INTEGER NOT NULL REFERENCES buses(bus_number));"
      "CREATE TABLE IF NOT EXISTS users ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "username TEXT NOT NULL UNIQUE,"
      "password TEXT NOT NULL,"
      "role TEXT NOT NULL CHECK (role IN ('admin', 'crew')),"
      "crew_member_id INTEGER REFERENCES crew_members(id));"
      "CREATE TABLE IF NOT EXISTS trips ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "bus_number INTEGER NOT NULL REFERENCES buses(bus_number),"
      "departure_date TEXT NOT NULL,"
      "arrival_date TEXT NOT NULL,"
      "route_id INTEGER NOT NULL REFERENCES routes(id),"
      "passengers INTEGER NOT NULL CHECK (passengers >= 0),"
      "ticket_price REAL NOT NULL CHECK (ticket_price >= 0),"
      "CHECK (arrival_date >= departure_date));"
      "CREATE TABLE IF NOT EXISTS crew_payments ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "crew_member_id INTEGER NOT NULL REFERENCES crew_members(id),"
      "period_from TEXT NOT NULL,"
      "period_to TEXT NOT NULL,"
      "percent REAL NOT NULL,"
      "amount REAL NOT NULL,"
      "created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP);"
      "CREATE TABLE IF NOT EXISTS media_files ("
      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
      "entity_type TEXT NOT NULL,"
      "entity_id INTEGER NOT NULL,"
      "file_path TEXT NOT NULL,"
      "description TEXT);");

  if (scalarInt(db, "SELECT COUNT(*) FROM buses;") > 0) {
    return;
  }

  db.exec(
      "INSERT INTO buses(bus_number, model, total_mileage, photo_path) VALUES"
      "(101, 'MAN Lion''s Coach', 152000, 'docs/assets/bus-101.png'),"
      "(205, 'Mercedes Tourismo', 221500, 'docs/assets/bus-205.png'),"
      "(312, 'Volvo 9700', 98000, 'docs/assets/bus-312.png');"
      "INSERT INTO routes(name, start_point, end_point, distance_km) VALUES"
      "('Минск - Мир - Несвиж', 'Минск', 'Несвиж', 260),"
      "('Минск - Брестская крепость', 'Минск', 'Брест', 710),"
      "('Полоцкое кольцо', 'Минск', 'Полоцк', 470),"
      "('Нарочанский край', 'Минск', 'Нарочь', 330);"
      "INSERT INTO crew_members(last_name, personnel_number, experience_years, category, address, birth_year, bus_number) VALUES"
      "('Иванов', 'T-1001', 12, 'водитель', 'Минск, ул. Ленина, 10', 1984, 101),"
      "('Петров', 'T-1002', 8, 'экскурсовод', 'Минск, ул. Сурганова, 5', 1990, 101),"
      "('Сидорова', 'T-1003', 6, 'механик', 'Минск, пр. Победителей, 21', 1992, 101),"
      "('Козлов', 'T-2001', 15, 'водитель', 'Брест, ул. Московская, 7', 1979, 205),"
      "('Орлова', 'T-2002', 9, 'экскурсовод', 'Брест, ул. Гоголя, 12', 1988, 205),"
      "('Мельник', 'T-2003', 5, 'механик', 'Брест, ул. Советская, 9', 1994, 205),"
      "('Смирнов', 'T-3001', 10, 'водитель', 'Витебск, ул. Кирова, 4', 1986, 312),"
      "('Волкова', 'T-3002', 7, 'экскурсовод', 'Витебск, ул. Лазо, 17', 1991, 312),"
      "('Егоров', 'T-3003', 4, 'механик', 'Витебск, ул. Правды, 3', 1996, 312);"
      "INSERT INTO users(username, password, role, crew_member_id) VALUES"
      "('admin', 'admin', 'admin', NULL),"
      "('ivanov', 'crew123', 'crew', 1),"
      "('orlova', 'crew123', 'crew', 5),"
      "('smirnov', 'crew123', 'crew', 7);"
      "INSERT INTO trips(bus_number, departure_date, arrival_date, route_id, passengers, ticket_price) VALUES"
      "(101, '2026-04-01', '2026-04-01', 1, 38, 85.0),"
      "(101, '2026-04-05', '2026-04-06', 4, 31, 110.0),"
      "(205, '2026-04-03', '2026-04-04', 2, 44, 160.0),"
      "(205, '2026-04-11', '2026-04-12', 3, 41, 140.0),"
      "(312, '2026-04-08', '2026-04-08', 1, 29, 90.0),"
      "(312, '2026-04-15', '2026-04-15', 4, 35, 120.0);");
}

int scalarInt(Database& db, const std::string& sql) {
  auto rows = db.query(sql);
  if (rows.empty() || rows.front().empty()) return 0;
  return std::stoi(rows.front().begin()->second);
}

double scalarDouble(Database& db, const std::string& sql) {
  auto rows = db.query(sql);
  if (rows.empty() || rows.front().empty()) return 0.0;
  return std::stod(rows.front().begin()->second);
}
