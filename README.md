# Лабораторная работа №3 — Task Manager

Приложение для управления задачами и проектами. Графический интерфейс на FLTK взят из проекта преподавателя; в рамках работы реализованы классы предметной области:

- `Task` — данные задачи, значения по умолчанию и проверка статуса/приоритета;
- `Repository<T>` — обобщённое хранилище на основе `std::vector`;
- `TaskManager` — создание, изменение и удаление задач и проектов.

## Сборка приложения

Требуются CMake 3.15+, компилятор C++17 и подключение к Интернету при первой конфигурации.

### Быстрый запуск в Windows

Один раз установите инструменты и перезапустите VS Code:

```powershell
powershell -ExecutionPolicy Bypass -File .\setup.ps1
```

Запуск только модульных тестов:

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1
```

Сборка, тестирование и запуск графического приложения:

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1 -App
```

### Ручной запуск

```bash
cmake -S . -B build
cmake --build build --parallel
```

После сборки запустите `TaskManager_FLTK` из каталога `build`.

## Запуск модульных тестов

При проверке только классов предметной области графический интерфейс можно не собирать:

```bash
cmake -S . -B build -DBUILD_GUI=OFF -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Неверные индексы репозитория приводят к `std::out_of_range`, а недопустимые значения статуса и приоритета — к `std::invalid_argument`.
