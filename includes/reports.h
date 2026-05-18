#ifndef TOUR_REPORTS_H
#define TOUR_REPORTS_H

#include "db.h"

#include <string>
#include <vector>

struct CrewPayment {
  int crewMemberId{};
  std::string lastName;
  int busNumber{};
  double amount{};
};

std::vector<Row> tripsByBus(Database& db, int busNumber, const std::string& from,
                            const std::string& to);
Row busSummary(Database& db, int busNumber);
std::vector<CrewPayment> calculateCrewPayments(Database& db,
                                               const std::string& from,
                                               const std::string& to,
                                               double percent);
void storeCrewPayments(Database& db, const std::string& from,
                       const std::string& to, double percent);
double paymentForCrewOnDate(Database& db, int crewMemberId,
                            const std::string& date, double percent);
std::vector<Row> mostExpensiveRouteDetails(Database& db);
Row busWithMaxMileage(Database& db);
void addTrip(Database& db, int busNumber, const std::string& departure,
             const std::string& arrival, int routeId, int passengers,
             double ticketPrice);
void updateTripPassengers(Database& db, int tripId, int passengers);
void deleteTrip(Database& db, int tripId);

#endif
