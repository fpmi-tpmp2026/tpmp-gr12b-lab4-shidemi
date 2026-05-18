# Спецификация проекта

## Диаграмма классов

```mermaid
classDiagram
  class Database {
    +exec(sql)
    +query(sql)
  }
  class UserSession {
    +userId
    +username
    +role
    +crewMemberId
  }
  class CrewPayment {
    +crewMemberId
    +lastName
    +busNumber
    +amount
  }
  Database <.. UserSession
  Database <.. CrewPayment
```

## Диаграмма деятельности: вход в систему

```mermaid
flowchart TD
  Start([Старт]) --> Input[Ввод логина и пароля]
  Input --> Check{Пользователь найден?}
  Check -- нет --> Error[Сообщение об ошибке]
  Error --> Finish([Завершение])
  Check -- да --> Role[Определение роли]
  Role --> Menu[Показ доступного меню]
  Menu --> Finish
```

## Диаграмма последовательности: добавление рейса

```mermaid
sequenceDiagram
  actor A as Администратор
  participant UI as Консольное меню
  participant R as reports.cpp
  participant DB as SQLite
  A->>UI: ввод данных рейса
  UI->>R: addTrip(...)
  R->>DB: проверить автобус
  R->>DB: проверить маршрут
  R->>DB: INSERT INTO trips
  DB-->>UI: результат
  UI-->>A: рейс добавлен
```

## Диаграмма компонентов

```mermaid
flowchart LR
  Main[src/main.cpp] --> Auth[src/auth.cpp]
  Main --> Reports[src/reports.cpp]
  Auth --> DB[src/db.cpp]
  Reports --> DB
  DB --> SQLite[(SQLite database)]
  Tests[tests] --> Auth
  Tests --> Reports
  Workflow[GitHub Actions] --> Makefile[Makefile]
  Makefile --> Main
```

## Диаграмма развертывания

```mermaid
flowchart TB
  Dev[Рабочая станция разработчика] --> Git[GitHub Repository]
  Git --> Actions[GitHub Actions Ubuntu Runner]
  Actions --> Build[make all]
  Actions --> Tests[make test]
  Actions --> Coverage[make coverage]
  Dev --> App[bin/tour_bureau]
  App --> DB[(build/tour_bureau.sqlite)]
```
