-- Туристическое бюро — схема БД (Вариант 6, ЛР №4)
PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS TOURS_ROUTES (
    route_id    INTEGER PRIMARY KEY AUTOINCREMENT,
    name        TEXT NOT NULL UNIQUE,
    start_pt    TEXT NOT NULL,
    end_pt      TEXT NOT NULL,
    distance_km REAL NOT NULL
);
CREATE TABLE IF NOT EXISTS TOURS_BUSES (
    bus_id   INTEGER PRIMARY KEY AUTOINCREMENT,
    number   TEXT NOT NULL UNIQUE,
    name     TEXT NOT NULL,
    total_km REAL NOT NULL DEFAULT 0
);
CREATE TABLE IF NOT EXISTS TOURS_CREW (
    crew_id    INTEGER PRIMARY KEY AUTOINCREMENT,
    surname    TEXT NOT NULL,
    tab_number TEXT NOT NULL UNIQUE,
    experience INTEGER NOT NULL,
    category   TEXT NOT NULL,
    address    TEXT,
    birth_year INTEGER NOT NULL,
    bus_id     INTEGER NOT NULL REFERENCES TOURS_BUSES(bus_id)
);
CREATE TABLE IF NOT EXISTS TOURS_USERS (
    user_id  INTEGER PRIMARY KEY AUTOINCREMENT,
    login    TEXT NOT NULL UNIQUE,
    password TEXT NOT NULL,
    role     TEXT NOT NULL CHECK(role IN ('admin','crew')),
    crew_id  INTEGER REFERENCES TOURS_CREW(crew_id)
);
CREATE TABLE IF NOT EXISTS TOURS_TRIPS (
    trip_id      INTEGER PRIMARY KEY AUTOINCREMENT,
    bus_id       INTEGER NOT NULL REFERENCES TOURS_BUSES(bus_id),
    dep_date     TEXT NOT NULL,
    arr_date     TEXT NOT NULL,
    route_id     INTEGER NOT NULL REFERENCES TOURS_ROUTES(route_id),
    passengers   INTEGER NOT NULL CHECK(passengers >= 0),
    ticket_price REAL NOT NULL CHECK(ticket_price >= 0)
);
CREATE TABLE IF NOT EXISTS TOURS_CREW_EARNINGS (
    earn_id       INTEGER PRIMARY KEY AUTOINCREMENT,
    crew_id       INTEGER NOT NULL REFERENCES TOURS_CREW(crew_id),
    period_from   TEXT NOT NULL,
    period_to     TEXT NOT NULL,
    amount        REAL NOT NULL,
    calculated_at TEXT NOT NULL DEFAULT (datetime('now'))
);
CREATE TRIGGER IF NOT EXISTS trg_validate_trip
BEFORE INSERT ON TOURS_TRIPS BEGIN
  SELECT CASE WHEN NEW.dep_date > NEW.arr_date
    THEN RAISE(ABORT,'Departure date must be <= arrival date') END;
END;
