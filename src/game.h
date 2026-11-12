#pragma once

#include "repository.h"

class Game {
public:
    explicit Game(WorldRepository& repository) : repository_(repository) {}

    World create(std::string name);
    std::vector<World> worlds();
    World enter(int id);
    void rename(std::string name);
    void erase(int id);
    std::vector<std::string> map();
    void place(int x, int y, Material material);
    void mine(int x, int y);

private:
    World currentWorld();

    WorldRepository& repository_;
    std::optional<int> current_;
};

