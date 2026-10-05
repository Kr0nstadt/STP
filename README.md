# Лабораторная работа №5 — Пункт выдачи заказов

Приложение оператора ПВЗ с интерфейсом FLTK и хранением данных в PostgreSQL. В рамках работы реализован класс `Database` из файла `src/db.cpp`; интерфейс, заголовочный файл и схема БД сохранены из проекта преподавателя.

Реализованы приёмка и поиск заказов, выдача, отмена, фильтрация по статусу, сводный отчёт и очистка истории. Все пользовательские значения передаются в параметризованные запросы.

## Зависимости

```bash
sudo apt install build-essential cmake libfltk1.3-dev libpqxx-dev libpq-dev postgresql
```

## Подготовка базы данных

```bash
sudo -i -u postgres createdb pickup
sudo -i -u postgres psql pickup -f /absolute/path/to/schema.sql
sudo -i -u postgres psql pickup
```

В консоли PostgreSQL выполните:

```sql
CREATE USER pickup_user WITH PASSWORD 'pickup_pass';
GRANT ALL PRIVILEGES ON DATABASE pickup TO pickup_user;
GRANT ALL ON SCHEMA public TO pickup_user;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO pickup_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO pickup_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO pickup_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON SEQUENCES TO pickup_user;
```

## Сборка и запуск

```bash
cmake -S . -B build -DBUILD_GUI=ON -DBUILD_TESTING=OFF
cmake --build build --parallel
PICKUP_DB="host=localhost dbname=pickup user=pickup_user password=pickup_pass" ./build/pickup_point
```

Если `PICKUP_DB` не задана, приложение выводит подсказку и завершается без открытия окна.

## Интеграционные тесты

Тесты очищают таблицу `orders`, поэтому для них нужна отдельная тестовая база.

```bash
cmake -S . -B build -DBUILD_GUI=OFF -DBUILD_TESTING=ON
cmake --build build --parallel
PICKUP_TEST_DB="host=localhost dbname=pickup_test user=pickup_user password=pickup_pass" ctest --test-dir build --output-on-failure
```
