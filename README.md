# Лабораторная работа №6 — DataConverter

Консольная утилита преобразует табличные данные между CSV, JSON и YAML. Для сборки нужны CMake 3.20 или новее и компилятор C++17. CMake загружает yaml-cpp и GoogleTest при конфигурировании.

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
./build/dataconv examples/people.csv converted.json
```

При использовании генератора Visual Studio исполняемый файл находится в `build/Release/dataconv.exe`.

Результаты матрицы, экспериментов с fail-fast и кэшем описаны в [отчёте](CI-REPORT.md).

