# Кубокоп

Консольная пародия на Minecraft. Мир состоит из 12 × 8 клеток. В нём можно ставить блоки земли, камня и дерева и добывать их обратно. Имена миров и расположение блоков сохраняются в PostgreSQL.

## Сборка и запуск на Ubuntu

```sh
sudo apt install g++ cmake libpq-dev postgresql
cmake -S . -B build
cmake --build build --parallel
```

Для локального запуска от текущего пользователя в PostgreSQL нужна роль с тем же именем. Если роли ещё нет, создайте её командой `sudo -u postgres createuser "$USER"`. Затем создайте базу, примените схему и запустите игру:

```sh
sudo -u postgres createdb -O "$USER" kubokop
psql -v ON_ERROR_STOP=1 -d kubokop -f sql/init.sql
KUBOKOP_DB='dbname=kubokop' ./build/kubokop
```

Для удалённой базы в `KUBOKOP_DB` задайте строку подключения PostgreSQL с хостом, именем базы и пользователем.

## Пример партии

```text
new Деревня квадратных жителей
put 2 3 stone
put 3 3 wood
map
mine 2 3
rename Кубическая дача
worlds
quit
```

При следующем запуске откройте мир через `enter ID`. Номер можно посмотреть командой `worlds`. Команда `delete ID` удаляет мир вместе с блоками. Полный список команд выводит `help`.

`domain.h` описывает мир и блок. `game.cpp` содержит правила игры. `postgres_repository.cpp` хранит данные и выполняет параметризованные запросы, а `cli.cpp` разбирает команды и рисует карту.

