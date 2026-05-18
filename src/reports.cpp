#include "reports.h"

#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace {
std::string quote(const std::string& value) {
  std::string escaped;
  for (char ch : value) {
    escaped += ch;
    if (ch == '\'') escaped += '\'';
  }
  return "'" + escaped + "'";
}
}

std::vector<Row> tripsByBus(Database& db, int busNumber, const std::string& from,
                            const std::string& to) {
  std::ostringstream sql;
  sql << "SELECT t.id, t.bus_number, t.departure_date, t.arrival_date, r.name AS route, "
      << "t.passengers, t.ticket_price, (t.passengers * t.ticket_price) AS income "
      << "FROM trips t JOIN routes r ON r.id = t.route_id "
      << "WHERE t.bus_number = " << busNumber << " AND t.departure_date BETWEEN "
      << quote(from) << " AND " << quote(to) << " ORDER BY t.departure_date;";
  return db.query(sql.str());
}

Row busSummary(Database& db, int busNumber) {
  std::ostringstream sql;
  sql << "SELECT b.bus_number, b.model, COUNT(t.id) AS trip_count, "
      << "COALESCE(SUM(t.passengers), 0) AS passengers, "
      << "COALESCE(SUM(t.passengers * t.ticket_price), 0) AS income "
      << "FROM buses b LEFT JOIN trips t ON t.bus_number = b.bus_number "
      << "WHERE b.bus_number = " << busNumber << " GROUP BY b.bus_number;";
  auto rows = db.query(sql.str());
  return rows.empty() ? Row{} : rows.front();
}

std::vector<CrewPayment> calculateCrewPayments(Database& db,
                                               const std::string& from,
                                               const std::string& to,
                                               double percent) {
  std::ostringstream sql;
  sql << "SELECT c.id AS crew_member_id, c.last_name, c.bus_number, "
      << "COALESCE(SUM(t.passengers * t.ticket_price), 0) * " << (percent / 100.0)
      << " AS amount "
      << "FROM crew_members c LEFT JOIN trips t ON t.bus_number = c.bus_number "
      << "AND t.departure_date BETWEEN " << quote(from) << " AND " << quote(to)
      << " GROUP BY c.id ORDER BY c.bus_number, c.last_name;";
  std::vector<CrewPayment> payments;
  for (const Row& row : db.query(sql.str())) {
    payments.push_back(CrewPayment{std::stoi(row.at("crew_member_id")),
                                   row.at("last_name"),
                                   std::stoi(row.at("bus_number")),
                                   std::stod(row.at("amount"))});
  }
  return payments;
}

void storeCrewPayments(Database& db, const std::string& from,
                       const std::string& to, double percent) {
  db.exec("DELETE FROM crew_payments WHERE period_from = " + quote(from) +
          " AND period_to = " + quote(to) + ";");
  for (const auto& payment : calculateCrewPayments(db, from, to, percent)) {
    std::ostringstream sql;
    sql << "INSERT INTO crew_payments(crew_member_id, period_from, period_to, percent, amount) "
        << "VALUES(" << payment.crewMemberId << ", " << quote(from) << ", "
        << quote(to) << ", " << percent << ", " << payment.amount << ");";
    db.exec(sql.str());
  }
}

double paymentForCrewOnDate(Database& db, int crewMemberId,
                            const std::string& date, double percent) {
  std::ostringstream sql;
  sql << "SELECT COALESCE(SUM(t.passengers * t.ticket_price), 0) * "
      << (percent / 100.0) << " AS amount "
      << "FROM crew_members c JOIN trips t ON t.bus_number = c.bus_number "
      << "WHERE c.id = " << crewMemberId << " AND t.departure_date <= " << quote(date)
      << " AND t.arrival_date >= " << quote(date) << ";";
  return scalarDouble(db, sql.str());
}

std::vector<Row> mostExpensiveRouteDetails(Database& db) {
  return db.query(
      "SELECT r.name AS route, b.bus_number, b.model, c.last_name, c.category, "
      "t.ticket_price FROM trips t "
      "JOIN routes r ON r.id = t.route_id "
      "JOIN buses b ON b.bus_number = t.bus_number "
      "JOIN crew_members c ON c.bus_number = b.bus_number "
      "WHERE t.ticket_price = (SELECT MAX(ticket_price) FROM trips) "
      "ORDER BY c.last_name;");
}

Row busWithMaxMileage(Database& db) {
  auto rows = db.query(
      "SELECT b.bus_number, b.model, b.total_mileage, "
      "COALESCE(SUM(t.passengers), 0) AS passengers "
      "FROM buses b LEFT JOIN trips t ON t.bus_number = b.bus_number "
      "GROUP BY b.bus_number ORDER BY b.total_mileage DESC LIMIT 1;");
  return rows.empty() ? Row{} : rows.front();
}

void addTrip(Database& db, int busNumber, const std::string& departure,
             const std::string& arrival, int routeId, int passengers,
             double ticketPrice) {
  if (busNumber <= 0 || passengers < 0 || ticketPrice < 0) {
    throw std::invalid_argument("Некорректные данные рейса");
  }
  if (scalarInt(db, "SELECT COUNT(*) FROM buses WHERE bus_number = " +
                        std::to_string(busNumber) + ";") == 0) {
    throw std::invalid_argument("Автобус не найден");
  }
  if (scalarInt(db, "SELECT COUNT(*) FROM routes WHERE id = " +
                        std::to_string(routeId) + ";") == 0) {
    throw std::invalid_argument("Маршрут не найден");
  }
  std::ostringstream sql;
  sql << "INSERT INTO trips(bus_number, departure_date, arrival_date, route_id, passengers, ticket_price) "
      << "VALUES(" << busNumber << ", " << quote(departure) << ", " << quote(arrival)
      << ", " << routeId << ", " << passengers << ", " << ticketPrice << ");";
  db.exec(sql.str());
}

void updateTripPassengers(Database& db, int tripId, int passengers) {
  if (tripId <= 0 || passengers < 0) {
    throw std::invalid_argument("Некорректные данные для обновления рейса");
  }
  if (scalarInt(db, "SELECT COUNT(*) FROM trips WHERE id = " +
                        std::to_string(tripId) + ";") == 0) {
    throw std::invalid_argument("Рейс не найден");
  }
  db.exec("UPDATE trips SET passengers = " + std::to_string(passengers) +
          " WHERE id = " + std::to_string(tripId) + ";");
}

void deleteTrip(Database& db, int tripId) {
  if (tripId <= 0) {
    throw std::invalid_argument("Некорректный номер рейса");
  }
  if (scalarInt(db, "SELECT COUNT(*) FROM trips WHERE id = " +
                        std::to_string(tripId) + ";") == 0) {
    throw std::invalid_argument("Рейс не найден");
  }
  db.exec("DELETE FROM trips WHERE id = " + std::to_string(tripId) + ";");
}
