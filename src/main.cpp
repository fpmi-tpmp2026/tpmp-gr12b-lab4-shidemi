#include "auth.h"
#include "reports.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void printRows(const std::vector<Row>& rows) {
  if (rows.empty()) {
    std::cout << "Данные не найдены.\n";
    return;
  }
  for (const auto& row : rows) {
    for (const auto& [key, value] : row) {
      std::cout << key << ": " << value << "  ";
    }
    std::cout << "\n";
  }
}

std::string readText(const std::string& label) {
  std::string value;
  std::cout << label;
  std::getline(std::cin, value);
  return value;
}

int readInt(const std::string& label) {
  return std::stoi(readText(label));
}

double readDouble(const std::string& label) {
  return std::stod(readText(label));
}

void showMenu(const UserSession& session) {
  std::cout << "\nТуристическое бюро | пользователь: " << session.username
            << " (" << roleToString(session.role) << ")\n";
  std::cout << "1. Рейсы указанного автобуса за период\n";
  std::cout << "2. Сводка по автобусу: поездки, пассажиры, деньги\n";
  std::cout << "3. Начисления каждому экипажу за период\n";
  std::cout << "4. Самый дорогой маршрут: автобусы, экипажи, билеты\n";
  std::cout << "5. Автобус с наибольшим пробегом\n";
  std::cout << "6. Добавить выполненный рейс\n";
  std::cout << "7. Изменить количество пассажиров в рейсе\n";
  std::cout << "8. Удалить рейс\n";
  std::cout << "9. Начисления указанному члену экипажа на дату\n";
  std::cout << "0. Выход\n";
}
}

int main(int argc, char** argv) {
  try {
    std::string dbPath = argc > 1 ? argv[1] : "build/tour_bureau.sqlite";
    Database db(dbPath);
    initializeDatabase(db);

    std::cout << "Лабораторная работа 4. Вариант 6: Туристическое бюро\n";
    std::string username = readText("Логин: ");
    std::string password = readText("Пароль: ");
    auto session = authenticate(db, username, password);
    if (!session) {
      std::cout << "Ошибка аутентификации.\n";
      return 1;
    }

    while (true) {
      showMenu(*session);
      int choice = readInt("Выберите пункт: ");
      if (choice == 0) break;

      if (choice == 1) {
        int bus = readInt("Номер автобуса: ");
        std::string from = readText("Дата с (YYYY-MM-DD): ");
        std::string to = readText("Дата по (YYYY-MM-DD): ");
        printRows(tripsByBus(db, bus, from, to));
      } else if (choice == 2) {
        int bus = readInt("Номер автобуса: ");
        printRows({busSummary(db, bus)});
      } else if (choice == 3) {
        if (!canManageData(*session)) {
          std::cout << "Недостаточно прав.\n";
          continue;
        }
        std::string from = readText("Дата с (YYYY-MM-DD): ");
        std::string to = readText("Дата по (YYYY-MM-DD): ");
        double percent = readDouble("Процент начисления: ");
        storeCrewPayments(db, from, to, percent);
        for (const auto& payment : calculateCrewPayments(db, from, to, percent)) {
          std::cout << payment.lastName << " | автобус " << payment.busNumber
                    << " | начислено " << std::fixed << std::setprecision(2)
                    << payment.amount << "\n";
        }
      } else if (choice == 4) {
        printRows(mostExpensiveRouteDetails(db));
      } else if (choice == 5) {
        printRows({busWithMaxMileage(db)});
      } else if (choice == 6) {
        if (!canManageData(*session)) {
          std::cout << "Недостаточно прав.\n";
          continue;
        }
        int bus = readInt("Номер автобуса: ");
        std::string departure = readText("Дата отбытия: ");
        std::string arrival = readText("Дата прибытия: ");
        int routeId = readInt("ID маршрута: ");
        int passengers = readInt("Количество пассажиров: ");
        double price = readDouble("Стоимость билета: ");
        addTrip(db, bus, departure, arrival, routeId, passengers, price);
        std::cout << "Рейс добавлен.\n";
      } else if (choice == 7) {
        if (!canManageData(*session)) {
          std::cout << "Недостаточно прав.\n";
          continue;
        }
        int tripId = readInt("ID рейса: ");
        int passengers = readInt("Новое количество пассажиров: ");
        updateTripPassengers(db, tripId, passengers);
        std::cout << "Рейс обновлен.\n";
      } else if (choice == 8) {
        if (!canManageData(*session)) {
          std::cout << "Недостаточно прав.\n";
          continue;
        }
        int tripId = readInt("ID рейса: ");
        deleteTrip(db, tripId);
        std::cout << "Рейс удален.\n";
      } else if (choice == 9) {
        int crewId = canManageData(*session) ? readInt("ID члена экипажа: ") : session->crewMemberId;
        if (!canViewCrewData(*session, crewId)) {
          std::cout << "Недостаточно прав.\n";
          continue;
        }
        std::string date = readText("Дата (YYYY-MM-DD): ");
        double percent = readDouble("Процент начисления: ");
        std::cout << "Начислено: " << std::fixed << std::setprecision(2)
                  << paymentForCrewOnDate(db, crewId, date, percent) << "\n";
      }
    }
  } catch (const std::exception& ex) {
    std::cerr << "Ошибка: " << ex.what() << "\n";
    return 2;
  }
  return 0;
}
