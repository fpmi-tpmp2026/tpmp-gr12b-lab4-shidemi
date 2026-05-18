PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS users (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  username TEXT NOT NULL UNIQUE,
  password TEXT NOT NULL,
  role TEXT NOT NULL CHECK (role IN ('admin', 'crew')),
  crew_member_id INTEGER,
  FOREIGN KEY (crew_member_id) REFERENCES crew_members(id)
);

CREATE TABLE IF NOT EXISTS routes (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  name TEXT NOT NULL UNIQUE,
  start_point TEXT NOT NULL,
  end_point TEXT NOT NULL,
  distance_km INTEGER NOT NULL CHECK (distance_km > 0)
);

CREATE TABLE IF NOT EXISTS buses (
  bus_number INTEGER PRIMARY KEY,
  model TEXT NOT NULL,
  total_mileage INTEGER NOT NULL CHECK (total_mileage >= 0),
  photo_path TEXT
);

CREATE TABLE IF NOT EXISTS crew_members (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  last_name TEXT NOT NULL,
  personnel_number TEXT NOT NULL UNIQUE,
  experience_years INTEGER NOT NULL CHECK (experience_years >= 0),
  category TEXT NOT NULL,
  address TEXT NOT NULL,
  birth_year INTEGER NOT NULL CHECK (birth_year BETWEEN 1940 AND 2010),
  bus_number INTEGER NOT NULL,
  FOREIGN KEY (bus_number) REFERENCES buses(bus_number)
);

CREATE TABLE IF NOT EXISTS trips (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  bus_number INTEGER NOT NULL,
  departure_date TEXT NOT NULL,
  arrival_date TEXT NOT NULL,
  route_id INTEGER NOT NULL,
  passengers INTEGER NOT NULL CHECK (passengers >= 0),
  ticket_price REAL NOT NULL CHECK (ticket_price >= 0),
  FOREIGN KEY (bus_number) REFERENCES buses(bus_number),
  FOREIGN KEY (route_id) REFERENCES routes(id),
  CHECK (arrival_date >= departure_date)
);

CREATE TABLE IF NOT EXISTS crew_payments (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  crew_member_id INTEGER NOT NULL,
  period_from TEXT NOT NULL,
  period_to TEXT NOT NULL,
  percent REAL NOT NULL,
  amount REAL NOT NULL,
  created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
  FOREIGN KEY (crew_member_id) REFERENCES crew_members(id)
);

CREATE TABLE IF NOT EXISTS media_files (
  id INTEGER PRIMARY KEY AUTOINCREMENT,
  entity_type TEXT NOT NULL,
  entity_id INTEGER NOT NULL,
  file_path TEXT NOT NULL,
  description TEXT
);
