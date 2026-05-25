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
#include "auth.h"

void report_bus_trips(sqlite3* db, int bus_id,
                      const std::string& from, const std::string& to);
void report_bus_summary(sqlite3* db, int bus_id);
void report_crew_earnings(sqlite3* db,
                          const std::string& from, const std::string& to,
                          const User& user);
void report_most_expensive_route(sqlite3* db);
void report_max_mileage_bus(sqlite3* db);

void trip_add(sqlite3* db, int bus_id, const std::string& dep,
              const std::string& arr, const std::string& route,
              int passengers, double ticket_price);
void trip_update_passengers(sqlite3* db, int trip_id, int new_passengers);
void trip_delete(sqlite3* db, int trip_id);

void fn_crew_earnings_period(sqlite3* db,
                             const std::string& from, const std::string& to);
void fn_crew_earnings_by_date(sqlite3* db, int crew_id,
                              const std::string& date);
