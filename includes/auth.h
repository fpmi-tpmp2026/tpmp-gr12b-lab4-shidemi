#ifndef TOUR_AUTH_H
#define TOUR_AUTH_H

#include "db.h"

#include <optional>
#include <string>

enum class Role { Admin, Crew };

struct UserSession {
  int userId{};
  std::string username;
  Role role;
  int crewMemberId{};
};

std::string roleToString(Role role);
std::optional<UserSession> authenticate(Database& db,
                                        const std::string& username,
                                        const std::string& password);
bool canManageData(const UserSession& session);
bool canViewCrewData(const UserSession& session, int crewMemberId);

#endif
