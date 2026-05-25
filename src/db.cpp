#include "db.h"
#include <stdexcept>

static void exec(sqlite3* db, const char* sql) {
    char* err = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &err) != SQLITE_OK) {
        std::string msg = err ? err : "unknown";
        sqlite3_free(err);
        throw std::runtime_error("DB error: " + msg);
    }
}

sqlite3* db_open(const std::string& path) {
    sqlite3* db = nullptr;
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK)
        throw std::runtime_error("Cannot open DB: " + std::string(sqlite3_errmsg(db)));
    exec(db, "PRAGMA foreign_keys = ON;");
    return db;
}

void db_close(sqlite3* db) { if (db) sqlite3_close(db); }

void db_init(sqlite3* db) {
    exec(db,
        "CREATE TABLE IF NOT EXISTS TOURS_ROUTES ("
        "  route_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL UNIQUE,"
        "  start_pt TEXT NOT NULL,"
        "  end_pt TEXT NOT NULL,"
        "  distance_km REAL NOT NULL);"

        "CREATE TABLE IF NOT EXISTS TOURS_BUSES ("
        "  bus_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  number TEXT NOT NULL UNIQUE,"
        "  name TEXT NOT NULL,"
        "  total_km REAL NOT NULL DEFAULT 0);"

        "CREATE TABLE IF NOT EXISTS TOURS_CREW ("
        "  crew_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  surname TEXT NOT NULL,"
        "  tab_number TEXT NOT NULL UNIQUE,"
        "  experience INTEGER NOT NULL,"
        "  category TEXT NOT NULL,"
        "  address TEXT,"
        "  birth_year INTEGER NOT NULL,"
        "  bus_id INTEGER NOT NULL REFERENCES TOURS_BUSES(bus_id));"

        "CREATE TABLE IF NOT EXISTS TOURS_USERS ("
        "  user_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  login TEXT NOT NULL UNIQUE,"
        "  password TEXT NOT NULL,"
        "  role TEXT NOT NULL CHECK(role IN ('admin','crew')),"
        "  crew_id INTEGER REFERENCES TOURS_CREW(crew_id));"

        "CREATE TABLE IF NOT EXISTS TOURS_TRIPS ("
        "  trip_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  bus_id INTEGER NOT NULL REFERENCES TOURS_BUSES(bus_id),"
        "  dep_date TEXT NOT NULL,"
        "  arr_date TEXT NOT NULL,"
        "  route_id INTEGER NOT NULL REFERENCES TOURS_ROUTES(route_id),"
        "  passengers INTEGER NOT NULL CHECK(passengers >= 0),"
        "  ticket_price REAL NOT NULL CHECK(ticket_price >= 0));"

        "CREATE TABLE IF NOT EXISTS TOURS_CREW_EARNINGS ("
        "  earn_id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  crew_id INTEGER NOT NULL REFERENCES TOURS_CREW(crew_id),"
        "  period_from TEXT NOT NULL,"
        "  period_to TEXT NOT NULL,"
        "  amount REAL NOT NULL,"
        "  calculated_at TEXT NOT NULL DEFAULT (datetime('now')));"

        "CREATE TRIGGER IF NOT EXISTS trg_validate_trip "
        "BEFORE INSERT ON TOURS_TRIPS BEGIN "
        "  SELECT CASE "
        "    WHEN NEW.dep_date > NEW.arr_date "
        "    THEN RAISE(ABORT,'Departure date must be <= arrival date') "
        "  END; "
        "END;"
    );
}

void db_seed(sqlite3* db) {
    exec(db,
        "INSERT OR IGNORE INTO TOURS_ROUTES(name,start_pt,end_pt,distance_km) VALUES"
        "  ('Золотое кольцо','Москва','Суздаль',260),"
        "  ('Горный тур','Сочи','Красная Поляна',40),"
        "  ('Речной маршрут','Нижний Новгород','Казань',420);"

        "INSERT OR IGNORE INTO TOURS_BUSES(number,name,total_km) VALUES"
        "  ('АА 001 МО','Mercedes Sprinter',12000),"
        "  ('ВВ 002 МО','Ford Transit',8500);"

        "INSERT OR IGNORE INTO TOURS_CREW(surname,tab_number,experience,category,address,birth_year,bus_id) VALUES"
        "  ('Иванов','T001',10,'B','ул.Ленина 1',1980,1),"
        "  ('Орлова','T002',5,'B','ул.Мира 5',1990,1),"
        "  ('Смирнов','T003',8,'C','пр.Победы 12',1985,2);"

        "INSERT OR IGNORE INTO TOURS_USERS(login,password,role,crew_id) VALUES"
        "  ('admin','admin','admin',NULL),"
        "  ('ivanov','crew123','crew',1),"
        "  ('orlova','crew123','crew',2),"
        "  ('smirnov','crew123','crew',3);"

        "INSERT OR IGNORE INTO TOURS_TRIPS(bus_id,dep_date,arr_date,route_id,passengers,ticket_price) VALUES"
        "  (1,'2024-01-10','2024-01-12',1,35,1500),"
        "  (1,'2024-02-15','2024-02-17',2,40,2000),"
        "  (2,'2024-01-20','2024-01-22',3,30,1800),"
        "  (2,'2024-03-05','2024-03-07',1,38,1500),"
        "  (1,'2024-04-01','2024-04-03',2,42,2000);"
    );
}
