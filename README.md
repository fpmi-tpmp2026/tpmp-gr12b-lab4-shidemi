# Туристическое бюро

![C/C++ CI](https://github.com/fpmi-tpmp2026/tpmp-gr12b-lab4-shidemi/actions/workflows/c-cpp.yml/badge.svg)

## Project Name
**«Туристическое бюро»** — консольное приложение для управления экскурсионными маршрутами, автобусным парком и экипажами. Вариант 6, ЛР №4.

## Description
Приложение реализует систему учёта для бюро путешествий: хранит данные о маршрутах, автобусах, экипажах и выполненных рейсах в SQLite3. Поддерживает два уровня доступа — **администратор** (полный доступ) и **член экипажа** (только свои данные). Реализованы все 5 аналитических SELECT-запросов по варианту, операции INSERT/UPDATE/DELETE, триггер проверки дат, функции автоматического расчёта начислений экипажам.

## Installation

```bash
git clone https://github.com/fpmi-tpmp2026/tpmp-gr12b-lab4-shidemi.git
cd tpmp-gr12b-lab4-shidemi
sudo apt-get install -y libsqlite3-dev
make all
```

## Usage

```bash
make run        # запустить приложение
make test       # запустить unit-тесты (16 тестов)
make coverage   # отчёт покрытия кода
make distcheck  # полный цикл: clean + build + test

# Учётные записи:
# admin   / admin    — полный доступ
# ivanov  / crew123  — член экипажа
# orlova  / crew123
# smirnov / crew123
```

## Contributing

| Участник | Задачи |
|----------|--------|
| Сафиулин Артём | Проектирование БД, реализация всех модулей (db/auth/reports/main), 16 unit-тестов, GitHub Actions, wiki, README |

---
🔗 [Wiki с документацией](../../wiki) | 🔗 [GitHub Actions](../../actions)
