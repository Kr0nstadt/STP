#pragma once

#include "repository.h"
#include <libpq-fe.h>
#include <initializer_list>
#include <memory>

class PostgresRepository : public WorldRepository {
public:
    explicit PostgresRepository(const char* connectionString);

    World create(const World& world) override;
    std::vector<World> list() override;
    std::optional<World> find(int id) override;
    bool save(const World& world) override;
    bool erase(int id) override;
    std::vector<Block> blocks(int worldId) override;
    bool place(int worldId, const Block& block) override;
    bool mine(int worldId, int x, int y) override;

private:
    using Result = std::unique_ptr<PGresult, decltype(&PQclear)>;

    Result query(const char* sql, std::initializer_list<std::string> parameters = {});
    std::unique_ptr<PGconn, decltype(&PQfinish)> connection_;
};

