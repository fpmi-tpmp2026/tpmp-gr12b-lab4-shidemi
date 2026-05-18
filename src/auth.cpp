#include "auth.h"

#include <sstream>

std::string roleToString(Role role) {
  return role == Role::Admin ? "admin" : "crew";
}

std::optional<UserSession> authenticate(Database& db, const std::string& username,
                                        const std::string& password) {
  std::ostringstream sql;
  sql << "SELECT id, username, role, COALESCE(crew_member_id, 0) AS crew_member_id "
      << "FROM users WHERE username = '" << username << "' AND password = '" << password
      << "' LIMIT 1;";
  auto rows = db.query(sql.str());
  if (rows.empty()) {
    return std::nullopt;
  }

  const Row& row = rows.front();
  return UserSession{std::stoi(row.at("id")),
                     row.at("username"),
                     row.at("role") == "admin" ? Role::Admin : Role::Crew,
                     std::stoi(row.at("crew_member_id"))};
}

bool canManageData(const UserSession& session) {
  return session.role == Role::Admin;
}

bool canViewCrewData(const UserSession& session, int crewMemberId) {
  return session.role == Role::Admin || session.crewMemberId == crewMemberId;
}
