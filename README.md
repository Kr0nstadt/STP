# Лабораторная работа №4 — Task Manager

Приложение для управления задачами, связанными с проектами. Графический интерфейс и инфраструктурные классы взяты из проекта преподавателя. В работе реализованы `Task` и `TaskManager`.

Поддерживаются перенос задачи между проектами, отвязка задачи, каскадное удаление проекта вместе с его задачами, подсчёт задач по статусам и процент выполнения проекта.

## Запуск в Windows

Если инструменты ещё не установлены:

```powershell
powershell -ExecutionPolicy Bypass -File .\setup.ps1
```

После установки перезапустите VS Code. Для сборки и запуска тестов выполните:

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1
```

Для сборки, тестирования и запуска графического приложения:

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1 -App
```

## Ручная сборка

```bash
cmake -S . -B build -DBUILD_GUI=OFF -DBUILD_TESTING=ON
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

Для сборки графического приложения укажите `-DBUILD_GUI=ON`. Исполняемый файл называется `TaskManager_FLTK`.
