#include "reports.h"

#include <cassert>
#include <cmath>

int main() {
  Database db(":memory:");
  initializeDatabase(db);

  auto trips = tripsByBus(db, 101, "2026-04-01", "2026-04-30");
  assert(trips.size() == 2);

  Row summary = busSummary(db, 101);
  assert(summary.at("trip_count") == "2");
  assert(summary.at("passengers") == "69");
  assert(std::fabs(std::stod(summary.at("income")) - 6640.0) < 0.001);

  auto payments = calculateCrewPayments(db, "2026-04-01", "2026-04-30", 20.0);
  assert(payments.size() == 9);
  assert(std::fabs(payments.front().amount - 1328.0) < 0.001);

  storeCrewPayments(db, "2026-04-01", "2026-04-30", 20.0);
  assert(scalarInt(db, "SELECT COUNT(*) FROM crew_payments;") == 9);

  double dayPayment = paymentForCrewOnDate(db, 1, "2026-04-01", 20.0);
  assert(std::fabs(dayPayment - 646.0) < 0.001);

  auto expensive = mostExpensiveRouteDetails(db);
  assert(expensive.size() == 3);
  assert(expensive.front().at("route") == "Минск - Брестская крепость");

  Row maxMileage = busWithMaxMileage(db);
  assert(maxMileage.at("bus_number") == "205");

  int before = scalarInt(db, "SELECT COUNT(*) FROM trips;");
  addTrip(db, 101, "2026-05-01", "2026-05-01", 1, 20, 95.0);
  assert(scalarInt(db, "SELECT COUNT(*) FROM trips;") == before + 1);
  int newTripId = scalarInt(db, "SELECT MAX(id) FROM trips;");
  updateTripPassengers(db, newTripId, 22);
  assert(scalarInt(db, "SELECT passengers FROM trips WHERE id = " +
                           std::to_string(newTripId) + ";") == 22);
  deleteTrip(db, newTripId);
  assert(scalarInt(db, "SELECT COUNT(*) FROM trips;") == before);
  return 0;
}
