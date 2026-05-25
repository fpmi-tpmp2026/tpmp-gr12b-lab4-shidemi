#include "reports.h"
#include <iostream>
#include <vector>
#include <stdexcept>

static void print_row(sqlite3_stmt* s) {
    int cols = sqlite3_column_count(s);
    for (int i = 0; i < cols; i++) {
        if (i) std::cout << " | ";
        const char* v = reinterpret_cast<const char*>(sqlite3_column_text(s, i));
        std::cout << (v ? v : "NULL");
    }
    std::cout << "\n";
}

static void run_select(sqlite3* db, const char* sql,
                       const std::vector<std::string>& p = {}) {
    sqlite3_stmt* s = nullptr;
    if (sqlite3_prepare_v2(db, sql, -1, &s, nullptr) != SQLITE_OK)
        throw std::runtime_error(sqlite3_errmsg(db));
    for (int i = 0; i < (int)p.size(); i++)
        sqlite3_bind_text(s, i+1, p[i].c_str(), -1, SQLITE_STATIC);
    int cols = sqlite3_column_count(s);
    for (int i = 0; i < cols; i++) { if (i) std::cout<<" | "; std::cout<<sqlite3_column_name(s,i); }
    std::cout << "\n" << std::string(60,'-') << "\n";
    int rows = 0;
    while (sqlite3_step(s) == SQLITE_ROW) { print_row(s); rows++; }
    if (!rows) std::cout << "(нет данных)\n";
    sqlite3_finalize(s);
}

void report_bus_trips(sqlite3* db, int bus_id,
                      const std::string& from, const std::string& to) {
    std::cout << "\n=== Рейсы автобуса #" << bus_id << " за " << from << " — " << to << " ===\n";
    const char* sql =
        "SELECT t.trip_id, b.number, r.name, t.dep_date, t.arr_date, t.passengers, t.ticket_price "
        "FROM TOURS_TRIPS t JOIN TOURS_BUSES b ON b.bus_id=t.bus_id "
        "JOIN TOURS_ROUTES r ON r.route_id=t.route_id "
        "WHERE t.bus_id=? AND t.dep_date>=? AND t.dep_date<=? ORDER BY t.dep_date;";
    sqlite3_stmt* s = nullptr;
    sqlite3_prepare_v2(db, sql, -1, &s, nullptr);
    sqlite3_bind_int(s, 1, bus_id);
    sqlite3_bind_text(s, 2, from.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(s, 3, to.c_str(), -1, SQLITE_STATIC);
    int cols = sqlite3_column_count(s), rows = 0;
    for (int i=0;i<cols;i++){if(i)std::cout<<" | ";std::cout<<sqlite3_column_name(s,i);}
    std::cout<<"\n"<<std::string(70,'-')<<"\n";
    while (sqlite3_step(s) == SQLITE_ROW) { print_row(s); rows++; }
    if (!rows) std::cout << "(нет данных)\n";
    sqlite3_finalize(s);
}

void report_bus_summary(sqlite3* db, int bus_id) {
    std::cout << "\n=== Сводка по автобусу #" << bus_id << " ===\n";
    run_select(db,
        "SELECT b.number, b.name, b.total_km, COUNT(t.trip_id) AS trips, "
        "SUM(t.passengers) AS total_pass, SUM(t.passengers*t.ticket_price) AS revenue "
        "FROM TOURS_BUSES b LEFT JOIN TOURS_TRIPS t ON t.bus_id=b.bus_id "
        "WHERE b.bus_id=? GROUP BY b.bus_id;", {std::to_string(bus_id)});
}

void report_crew_earnings(sqlite3* db, const std::string& from,
                          const std::string& to, const User& user) {
    std::cout << "\n=== Начисления экипажам за " << from << " — " << to << " ===\n";
    std::string sql =
        "SELECT c.surname, c.tab_number, b.number, "
        "ROUND(SUM(t.passengers*t.ticket_price)*0.20,2) AS earnings "
        "FROM TOURS_CREW c JOIN TOURS_BUSES b ON b.bus_id=c.bus_id "
        "JOIN TOURS_TRIPS t ON t.bus_id=b.bus_id "
        "WHERE t.dep_date>=? AND t.dep_date<=? ";
    if (user.role == Role::CREW)
        sql += "AND c.crew_id=" + std::to_string(user.crew_id) + " ";
    sql += "GROUP BY c.crew_id ORDER BY earnings DESC;";
    run_select(db, sql.c_str(), {from, to});
}

void report_most_expensive_route(sqlite3* db) {
    std::cout << "\n=== Самый дорогой маршрут ===\n";
    run_select(db,
        "SELECT r.name, r.start_pt, r.end_pt, r.distance_km, b.number, c.surname, t.ticket_price "
        "FROM TOURS_TRIPS t JOIN TOURS_ROUTES r ON r.route_id=t.route_id "
        "JOIN TOURS_BUSES b ON b.bus_id=t.bus_id JOIN TOURS_CREW c ON c.bus_id=b.bus_id "
        "WHERE t.ticket_price=(SELECT MAX(ticket_price) FROM TOURS_TRIPS) ORDER BY c.surname;");
}

void report_max_mileage_bus(sqlite3* db) {
    std::cout << "\n=== Автобус с наибольшим пробегом ===\n";
    run_select(db,
        "SELECT b.number, b.name, b.total_km, COUNT(t.trip_id) AS trips, SUM(t.passengers) AS pass "
        "FROM TOURS_BUSES b LEFT JOIN TOURS_TRIPS t ON t.bus_id=b.bus_id "
        "WHERE b.total_km=(SELECT MAX(total_km) FROM TOURS_BUSES) GROUP BY b.bus_id;");
}

void trip_add(sqlite3* db, int bus_id, const std::string& dep,
              const std::string& arr, const std::string& route,
              int passengers, double price) {
    sqlite3_stmt* s = nullptr;
    sqlite3_prepare_v2(db,
        "INSERT INTO TOURS_TRIPS(bus_id,dep_date,arr_date,route_id,passengers,ticket_price) "
        "VALUES(?,?,?,(SELECT route_id FROM TOURS_ROUTES WHERE name=?),?,?);",
        -1, &s, nullptr);
    sqlite3_bind_int(s,1,bus_id);
    sqlite3_bind_text(s,2,dep.c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(s,3,arr.c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(s,4,route.c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_int(s,5,passengers);
    sqlite3_bind_double(s,6,price);
    if (sqlite3_step(s) != SQLITE_DONE) {
        std::string err = sqlite3_errmsg(db); sqlite3_finalize(s);
        throw std::runtime_error("trip_add: " + err);
    }
    sqlite3_finalize(s);
    std::cout << "Рейс добавлен (ID=" << sqlite3_last_insert_rowid(db) << ")\n";
}

void trip_update_passengers(sqlite3* db, int trip_id, int pax) {
    sqlite3_stmt* s = nullptr;
    sqlite3_prepare_v2(db,"UPDATE TOURS_TRIPS SET passengers=? WHERE trip_id=?;",-1,&s,nullptr);
    sqlite3_bind_int(s,1,pax); sqlite3_bind_int(s,2,trip_id);
    sqlite3_step(s); sqlite3_finalize(s);
    std::cout << "Рейс #" << trip_id << " обновлён: passengers=" << pax << "\n";
}

void trip_delete(sqlite3* db, int trip_id) {
    sqlite3_stmt* s = nullptr;
    sqlite3_prepare_v2(db,"DELETE FROM TOURS_TRIPS WHERE trip_id=?;",-1,&s,nullptr);
    sqlite3_bind_int(s,1,trip_id); sqlite3_step(s); sqlite3_finalize(s);
    std::cout << "Рейс #" << trip_id << " удалён.\n";
}

void fn_crew_earnings_period(sqlite3* db,
                             const std::string& from, const std::string& to) {
    std::cout << "\n=== Расчёт начислений за " << from << " — " << to << " ===\n";
    sqlite3_stmt *sel=nullptr, *ins=nullptr;
    sqlite3_prepare_v2(db,
        "SELECT c.crew_id, ROUND(SUM(t.passengers*t.ticket_price)*0.20,2) "
        "FROM TOURS_CREW c JOIN TOURS_TRIPS t ON t.bus_id=c.bus_id "
        "WHERE t.dep_date>=? AND t.dep_date<=? GROUP BY c.crew_id;",
        -1, &sel, nullptr);
    sqlite3_bind_text(sel,1,from.c_str(),-1,SQLITE_STATIC);
    sqlite3_bind_text(sel,2,to.c_str(),-1,SQLITE_STATIC);
    sqlite3_prepare_v2(db,
        "INSERT INTO TOURS_CREW_EARNINGS(crew_id,period_from,period_to,amount) VALUES(?,?,?,?);",
        -1, &ins, nullptr);
    while (sqlite3_step(sel) == SQLITE_ROW) {
        int cid = sqlite3_column_int(sel,0);
        double amt = sqlite3_column_double(sel,1);
        sqlite3_bind_int(ins,1,cid);
        sqlite3_bind_text(ins,2,from.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_text(ins,3,to.c_str(),-1,SQLITE_STATIC);
        sqlite3_bind_double(ins,4,amt);
        sqlite3_step(ins); sqlite3_reset(ins);
        std::cout << "  crew_id=" << cid << "  amount=" << amt << "\n";
    }
    sqlite3_finalize(sel); sqlite3_finalize(ins);
    std::cout << "Данные сохранены в TOURS_CREW_EARNINGS.\n";
}

void fn_crew_earnings_by_date(sqlite3* db, int crew_id, const std::string& date) {
    std::cout << "\n=== Начисления crew_id=" << crew_id << " на " << date << " ===\n";
    sqlite3_stmt* s = nullptr;
    sqlite3_prepare_v2(db,
        "SELECT c.surname, c.tab_number, ROUND(SUM(t.passengers*t.ticket_price)*0.20,2) "
        "FROM TOURS_CREW c JOIN TOURS_TRIPS t ON t.bus_id=c.bus_id "
        "WHERE c.crew_id=? AND t.dep_date<=? GROUP BY c.crew_id;",
        -1, &s, nullptr);
    sqlite3_bind_int(s,1,crew_id);
    sqlite3_bind_text(s,2,date.c_str(),-1,SQLITE_STATIC);
    if (sqlite3_step(s) == SQLITE_ROW) print_row(s);
    else std::cout << "(нет данных)\n";
    sqlite3_finalize(s);
}
