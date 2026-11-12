#include "game.h"

World Game::create(std::string name) {
    auto world = repository_.create(World(0, std::move(name)));
    current_ = world.id();
    return world;
}

std::vector<World> Game::worlds() {
    return repository_.list();
}

World Game::enter(int id) {
    auto world = repository_.find(id);
    if (!world)
        throw std::runtime_error("Мир не найден");
    current_ = id;
    return *world;
}

World Game::currentWorld() {
    if (!current_)
        throw std::runtime_error("Сначала создайте или откройте мир");
    auto world = repository_.find(*current_);
    if (!world) {
        current_.reset();
        throw std::runtime_error("Мир не найден");
    }
    return *world;
}

void Game::rename(std::string name) {
    auto world = currentWorld();
    world.rename(std::move(name));
    if (!repository_.save(world))
        throw std::runtime_error("Мир не найден");
}

void Game::erase(int id) {
    if (!repository_.erase(id))
        throw std::runtime_error("Мир не найден");
    if (current_ == id)
        current_.reset();
}

std::vector<std::string> Game::map() {
    const auto world = currentWorld();
    std::vector<std::string> grid(World::height, std::string(World::width, '.'));
    for (const auto& block : repository_.blocks(world.id()))
        grid[block.y()][block.x()] = block.symbol();
    return grid;
}

void Game::place(int x, int y, Material material) {
    const Block block(x, y, material);
    if (!repository_.place(currentWorld().id(), block))
        throw std::runtime_error("Клетка уже занята. Два куба в один куб не лезут.");
}

void Game::mine(int x, int y) {
    Block::checkPosition(x, y);
    if (!repository_.mine(currentWorld().id(), x, y))
        throw std::runtime_error("Пусто. Воздух добывать ещё не научились.");
}

