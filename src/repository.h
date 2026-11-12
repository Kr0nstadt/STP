#pragma once

#include "domain.h"
#include <optional>
#include <vector>

class WorldRepository {
public:
    virtual ~WorldRepository() = default;

    virtual World create(const World& world) = 0;
    virtual std::vector<World> list() = 0;
    virtual std::optional<World> find(int id) = 0;
    virtual bool save(const World& world) = 0;
    virtual bool erase(int id) = 0;
    virtual std::vector<Block> blocks(int worldId) = 0;
    virtual bool place(int worldId, const Block& block) = 0;
    virtual bool mine(int worldId, int x, int y) = 0;
};

