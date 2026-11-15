#include "cli.h"
#include <iomanip>
#include <istream>
#include <ostream>
#include <sstream>

namespace {
void help(std::ostream& output) {
    output << "new ИМЯ                 создать и открыть мир\n"
           << "worlds                  список миров\n"
           << "enter ID                открыть мир\n"
           << "rename ИМЯ              переименовать текущий мир\n"
           << "delete ID               удалить мир и его блоки\n"
           << "map                     показать карту\n"
           << "put X Y МАТЕРИАЛ        поставить dirt, stone или wood\n"
           << "mine X Y                сломать блок\n"
           << "help                    показать команды\n"
           << "quit                    выйти\n";
}

void requireEnd(std::istream& input) {
    std::string extra;
    if (input >> extra)
        throw std::invalid_argument("Лишние аргументы");
}

int readId(std::istream& input) {
    int id;
    if (!(input >> id) || id < 1)
        throw std::invalid_argument("Нужен положительный ID мира");
    requireEnd(input);
    return id;
}

std::string readName(std::istream& input) {
    std::string name;
    std::getline(input >> std::ws, name);
    const auto end = name.find_last_not_of(" \t\r");
    if (end != std::string::npos)
        name.erase(end + 1);
    return name;
}

void printWorld(const World& world, std::ostream& output) {
    output << world.id() << "  " << world.name() << '\n';
}

void printMap(const std::vector<std::string>& grid, std::ostream& output) {
    output << "   ";
    for (int x = 0; x < World::width; ++x)
        output << x % 10;
    output << '\n';
    for (int y = 0; y < World::height; ++y)
        output << std::setw(2) << y << ' ' << grid[y] << '\n';
    output << ". воздух, D земля, S камень, W дерево\n";
}
}

void runConsole(Game& game, std::istream& input, std::ostream& output) {
    output << "Кубокоп. Весь мир квадратный, даже земля.\n"
           << "Введите help, чтобы увидеть команды.\n";
    std::string line;
    while (output << "> " && std::getline(input, line)) {
        std::istringstream args(line);
        std::string command;
        if (!(args >> command))
            continue;
        try {
            if (command == "quit") {
                requireEnd(args);
                return;
            }
            if (command == "help") {
                requireEnd(args);
                help(output);
            } else if (command == "new") {
                printWorld(game.create(readName(args)), output);
            } else if (command == "worlds") {
                requireEnd(args);
                for (const auto& world : game.worlds())
                    printWorld(world, output);
            } else if (command == "enter") {
                printWorld(game.enter(readId(args)), output);
            } else if (command == "rename") {
                game.rename(readName(args));
                output << "Мир переименован.\n";
            } else if (command == "delete") {
                game.erase(readId(args));
                output << "Мир удалён. Все кубы отправились в квадратный рай.\n";
            } else if (command == "map") {
                requireEnd(args);
                printMap(game.map(), output);
            } else if (command == "put" || command == "mine") {
                int x;
                int y;
                if (!(args >> x >> y))
                    throw std::invalid_argument("Нужны целые координаты X и Y");
                if (command == "put") {
                    std::string material;
                    args >> material;
                    requireEnd(args);
                    game.place(x, y, parseMaterial(material));
                    output << "Блок поставлен.\n";
                } else {
                    requireEnd(args);
                    game.mine(x, y);
                    output << "Блок добыт.\n";
                }
            } else {
                output << "Неизвестная команда. Список команд: help.\n";
            }
        } catch (const std::exception& error) {
            output << "Ошибка: " << error.what() << '\n';
        }
    }
}

