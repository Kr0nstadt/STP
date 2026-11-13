#include "postgres_repository.h"

namespace {
World readWorld(PGresult* result, int row) {
    return World(std::stoi(PQgetvalue(result, row, 0)), PQgetvalue(result, row, 1));
}
}

PostgresRepository::PostgresRepository(const char* connectionString)
    : connection_(PQconnectdb(connectionString), PQfinish) {
    if (!connection_)
        throw std::runtime_error("PostgreSQL: не удалось создать подключение");
    if (PQstatus(connection_.get()) != CONNECTION_OK)
        throw std::runtime_error(std::string("Подключение к PostgreSQL: ") +
                                 PQerrorMessage(connection_.get()));
    if (PQsetClientEncoding(connection_.get(), "UTF8") != 0)
        throw std::runtime_error("PostgreSQL: не удалось установить кодировку UTF-8");
}

PostgresRepository::Result PostgresRepository::query(
        const char* sql, std::initializer_list<std::string> parameters) {
    std::vector<const char*> values;
    values.reserve(parameters.size());
    for (const auto& parameter : parameters)
        values.push_back(parameter.c_str());

    Result result(PQexecParams(connection_.get(), sql, static_cast<int>(values.size()),
                               nullptr, values.data(), nullptr, nullptr, 0), PQclear);
    if (!result)
        throw std::runtime_error(std::string("PostgreSQL: ") + PQerrorMessage(connection_.get()));
    if (PQresultStatus(result.get()) != PGRES_TUPLES_OK)
        throw std::runtime_error(std::string("PostgreSQL: ") + PQresultErrorMessage(result.get()));
    return result;
}

World PostgresRepository::create(const World& world) {
    const auto result = query("INSERT INTO worlds (name) VALUES ($1) RETURNING id, name",
                              {world.name()});
    return readWorld(result.get(), 0);
}

std::vector<World> PostgresRepository::list() {
    const auto result = query("SELECT id, name FROM worlds ORDER BY id");
    std::vector<World> worlds;
    for (int row = 0; row < PQntuples(result.get()); ++row)
        worlds.push_back(readWorld(result.get(), row));
    return worlds;
}

std::optional<World> PostgresRepository::find(int id) {
    const auto result = query("SELECT id, name FROM worlds WHERE id = $1", {std::to_string(id)});
    if (PQntuples(result.get()) == 0)
        return std::nullopt;
    return readWorld(result.get(), 0);
}

bool PostgresRepository::save(const World& world) {
    const auto result = query("UPDATE worlds SET name = $2 WHERE id = $1 RETURNING id",
                              {std::to_string(world.id()), world.name()});
    return PQntuples(result.get()) != 0;
}

bool PostgresRepository::erase(int id) {
    const auto result = query("DELETE FROM worlds WHERE id = $1 RETURNING id", {std::to_string(id)});
    return PQntuples(result.get()) != 0;
}

std::vector<Block> PostgresRepository::blocks(int worldId) {
    const auto result = query("SELECT x, y, material FROM blocks WHERE world_id = $1",
                              {std::to_string(worldId)});
    std::vector<Block> blocks;
    for (int row = 0; row < PQntuples(result.get()); ++row) {
        blocks.emplace_back(std::stoi(PQgetvalue(result.get(), row, 0)),
                            std::stoi(PQgetvalue(result.get(), row, 1)),
                            parseMaterial(PQgetvalue(result.get(), row, 2)));
    }
    return blocks;
}

bool PostgresRepository::place(int worldId, const Block& block) {
    const auto result = query(
        "INSERT INTO blocks (world_id, x, y, material) VALUES ($1, $2, $3, $4) "
        "ON CONFLICT (world_id, x, y) DO NOTHING RETURNING x",
        {std::to_string(worldId), std::to_string(block.x()), std::to_string(block.y()),
         materialName(block.material())});
    return PQntuples(result.get()) != 0;
}

bool PostgresRepository::mine(int worldId, int x, int y) {
    const auto result = query("DELETE FROM blocks WHERE world_id = $1 AND x = $2 AND y = $3 RETURNING x",
                              {std::to_string(worldId), std::to_string(x), std::to_string(y)});
    return PQntuples(result.get()) != 0;
}

