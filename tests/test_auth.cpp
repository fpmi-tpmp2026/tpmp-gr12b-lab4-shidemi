#include "auth.h"

#include <cassert>

int main() {
  Database db(":memory:");
  initializeDatabase(db);

  auto admin = authenticate(db, "admin", "admin");
  assert(admin.has_value());
  assert(admin->role == Role::Admin);
  assert(canManageData(*admin));

  auto crew = authenticate(db, "ivanov", "crew123");
  assert(crew.has_value());
  assert(crew->role == Role::Crew);
  assert(!canManageData(*crew));
  assert(canViewCrewData(*crew, crew->crewMemberId));
  assert(!canViewCrewData(*crew, crew->crewMemberId + 1));

  auto denied = authenticate(db, "admin", "wrong");
  assert(!denied.has_value());
  return 0;
}
