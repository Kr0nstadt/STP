#include "cli.h"
#include "postgres_repository.h"
#include <cstdlib>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif

int main() {
#ifdef _WIN32
    SetConsoleCP(CP_UTF8);
    SetConsoleOutputCP(CP_UTF8);
#endif
    const char* connection = std::getenv("KUBOKOP_DB");
    if (!connection || !*connection) {
        std::cerr << "Задайте параметры PostgreSQL в KUBOKOP_DB.\n";
        return 1;
    }
    try {
        PostgresRepository repository(connection);
        Game game(repository);
        runConsole(game, std::cin, std::cout);
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

