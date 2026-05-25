#include <iostream>
#include <string>
#include <limits>
#include "db.h"
#include "auth.h"
#include "reports.h"

static void show_menu(const User& u) {
    std::cout << "\n============================\n";
    std::cout << "  Туристическое бюро\n";
    std::cout << "  Пользователь: " << u.login
              << " [" << (u.role==Role::ADMIN?"admin":"crew") << "]\n";
    std::cout << "============================\n";
    std::cout << "1. Рейсы автобуса за период\n";
    std::cout << "2. Сводка по автобусу\n";
    std::cout << "3. Начисления экипажам\n";
    if (u.role == Role::ADMIN) {
        std::cout << "4. Самый дорогой маршрут\n";
        std::cout << "5. Автобус с наибольшим пробегом\n";
        std::cout << "6. Добавить рейс\n";
        std::cout << "7. Изменить пассажиров рейса\n";
        std::cout << "8. Удалить рейс\n";
        std::cout << "9. Рассчитать и сохранить начисления\n";
    }
    std::cout << "0. Выход\n> ";
}

int main() {
    sqlite3* db = nullptr;
    try {
        db = db_open("build/tour_bureau.sqlite");
        db_init(db); db_seed(db);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка БД: " << e.what() << "\n"; return 1;
    }
    std::string login, password;
    std::cout << "=== Авторизация ===\nЛогин: "; std::cin >> login;
    std::cout << "Пароль: "; std::cin >> password;
    User user;
    if (!auth_login(db, login, password, user)) {
        std::cout << "Неверные данные.\n"; db_close(db); return 1;
    }
    std::cout << "Добро пожаловать, " << user.login << "!\n";
    int choice = -1;
    while (choice != 0) {
        show_menu(user);
        if (!(std::cin >> choice)) { std::cin.clear(); std::cin.ignore(10000,'\n'); continue; }
        std::cin.ignore(10000,'\n');
        try {
            if (choice==1) {
                int id; std::string f,t;
                std::cout<<"ID автобуса: ";std::cin>>id;
                std::cout<<"С (YYYY-MM-DD): ";std::cin>>f;
                std::cout<<"По (YYYY-MM-DD): ";std::cin>>t;
                report_bus_trips(db,id,f,t);
            } else if (choice==2) {
                int id; std::cout<<"ID автобуса: ";std::cin>>id;
                report_bus_summary(db,id);
            } else if (choice==3) {
                std::string f,t;
                std::cout<<"С (YYYY-MM-DD): ";std::cin>>f;
                std::cout<<"По (YYYY-MM-DD): ";std::cin>>t;
                report_crew_earnings(db,f,t,user);
            } else if (user.role==Role::ADMIN) {
                if (choice==4) report_most_expensive_route(db);
                else if (choice==5) report_max_mileage_bus(db);
                else if (choice==6) {
                    int id,pax; std::string dep,arr,route; double price;
                    std::cout<<"ID автобуса: ";std::cin>>id;
                    std::cout<<"Дата отбытия: ";std::cin>>dep;
                    std::cout<<"Дата прибытия: ";std::cin>>arr;
                    std::cin.ignore();
                    std::cout<<"Маршрут: ";std::getline(std::cin,route);
                    std::cout<<"Пассажиры: ";std::cin>>pax;
                    std::cout<<"Цена: ";std::cin>>price;
                    trip_add(db,id,dep,arr,route,pax,price);
                } else if (choice==7) {
                    int id,pax;
                    std::cout<<"ID рейса: ";std::cin>>id;
                    std::cout<<"Новое кол-во: ";std::cin>>pax;
                    trip_update_passengers(db,id,pax);
                } else if (choice==8) {
                    int id; std::cout<<"ID рейса: ";std::cin>>id;
                    trip_delete(db,id);
                } else if (choice==9) {
                    std::string f,t;
                    std::cout<<"С (YYYY-MM-DD): ";std::cin>>f;
                    std::cout<<"По (YYYY-MM-DD): ";std::cin>>t;
                    fn_crew_earnings_period(db,f,t);
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Ошибка: " << e.what() << "\n";
        }
    }
    db_close(db);
    std::cout << "До свидания!\n";
    return 0;
}
