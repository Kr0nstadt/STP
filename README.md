# Лабораторная работа №1 — структурное тестирование

Реализация функций расчёта стоимости заказа и индивидуального варианта №4. Для каждой функции построен управляющий граф и подготовлены тесты по критериям C0, C1 и C2.

## Состав работы

- `applyDiscount`, `calcShipping`, `finalPrice`, `countExpensiveItems` — функции из приложения к заданию;
- `reversePrices` — разворот массива цен (вариант №4);
- `minOnAndAboveSecondaryDiagonal` — минимум на побочной диагонали и выше неё (вариант №4);
- Google Test — модульные тесты;
- GitHub Actions — автоматическая сборка, тестирование и отчёт покрытия.

Ошибка в исходной версии `calcShipping` исправлена: бесплатная доставка применяется при сумме заказа от 5000 включительно.

## Сборка и запуск тестов

### Быстрый запуск в Windows

Один раз установите инструменты сборки:

```powershell
powershell -ExecutionPolicy Bypass -File .\setup.ps1
```

После установки перезапустите VS Code. Затем для сборки и запуска всех тестов используйте:

```powershell
powershell -ExecutionPolicy Bypass -File .\run.ps1
```

### Ручной запуск

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Отчётные материалы

- [Управляющие графы](docs/CONTROL_FLOW_GRAPHS.md)
- [Тестовые случаи и анализ покрытия](docs/COVERAGE_ANALYSIS.md)
