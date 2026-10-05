#include "db.h"

#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace
{
bool looksLikePhone(const std::string& value)
{
    if (value.empty())
    {
        return false;
    }

    int digitCount = 0;
    for (char character : value)
    {
        const unsigned char current = static_cast<unsigned char>(character);
        if (std::isdigit(current))
        {
            ++digitCount;
        }
        else if (character != '+' && character != '-' && character != ' ' &&
                 character != '(' && character != ')')
        {
            return false;
        }
    }
    return digitCount >= 10;
}

std::vector<Order> makeOrders(const pqxx::result& result)
{
    std::vector<Order> orders;
    orders.reserve(result.size());
    for (const pqxx::row& row : result)
    {
        orders.push_back({
            row[0].as<int>(),
            row[1].as<std::string>(),
            row[2].as<std::string>(),
            row[3].as<std::string>(),
            row[4].as<std::string>(),
            row[5].as<std::string>(),
            row[6].as<std::string>()});
    }
    return orders;
}

std::string makeOrderCode(int id)
{
    std::ostringstream stream;
    stream << "PVZ-" << std::setw(6) << std::setfill('0') << id;
    return stream.str();
}

const char* orderColumns()
{
    return R"(
        id,
        order_code,
        COALESCE(phone, ''),
        COALESCE(parcel_article, ''),
        COALESCE(cell, ''),
        status,
        TO_CHAR(created_at, 'YYYY-MM-DD HH24:MI')
    )";
}
}

Database::Database(const std::string& conn_str)
    : conn_(conn_str)
{
    if (!conn_.is_open())
    {
        throw std::runtime_error("Не удалось подключиться к PostgreSQL");
    }
}

std::vector<Order> Database::listOrders(const std::string& filter)
{
    pqxx::work transaction(conn_);
    const std::string query = std::string("SELECT ") + orderColumns() + R"(
        FROM orders
        WHERE $1::text = ''
           OR order_code ILIKE '%' || $1 || '%'
           OR COALESCE(phone, '') ILIKE '%' || $1 || '%'
           OR COALESCE(parcel_article, '') ILIKE '%' || $1 || '%'
        ORDER BY created_at DESC, id DESC
    )";
    const pqxx::result result = transaction.exec_params(query, filter);
    std::vector<Order> orders = makeOrders(result);
    transaction.commit();
    return orders;
}

std::string Database::addOrder(const std::string& contact,
                               const std::string& cell)
{
    const bool isPhone = looksLikePhone(contact);
    pqxx::work transaction(conn_);
    const pqxx::result inserted = transaction.exec_params(
        R"(
            INSERT INTO orders (order_code, phone, parcel_article, cell)
            VALUES (
                'TMP-' || txid_current()::text,
                CASE WHEN $1::boolean THEN $2::text ELSE NULL END,
                CASE WHEN $1::boolean THEN NULL ELSE $2::text END,
                NULLIF($3::text, '')
            )
            RETURNING id
        )",
        isPhone,
        contact,
        cell);

    const int id = inserted[0][0].as<int>();
    const std::string code = makeOrderCode(id);
    const pqxx::result updated = transaction.exec_params(
        "UPDATE orders SET order_code = $1 WHERE id = $2",
        code,
        id);
    if (updated.affected_rows() != 1)
    {
        throw std::runtime_error("Не удалось сформировать код заказа");
    }

    transaction.commit();
    return code;
}

bool Database::issueOrder(const std::string& code)
{
    pqxx::work transaction(conn_);
    const pqxx::result result = transaction.exec_params(
        R"(
            UPDATE orders
            SET status = 'issued', issued_at = NOW()
            WHERE order_code = $1 AND status = 'ready'
        )",
        code);
    const bool issued = result.affected_rows() == 1;
    transaction.commit();
    return issued;
}

bool Database::cancelOrder(const std::string& code)
{
    pqxx::work transaction(conn_);
    const pqxx::result result = transaction.exec_params(
        R"(
            UPDATE orders
            SET status = 'cancelled'
            WHERE order_code = $1 AND status = 'ready'
        )",
        code);
    const bool cancelled = result.affected_rows() == 1;
    transaction.commit();
    return cancelled;
}

Report Database::buildReport()
{
    pqxx::work transaction(conn_);
    const pqxx::row row = transaction.exec1(R"(
        SELECT
            COUNT(*) FILTER (WHERE status = 'ready'),
            COUNT(*) FILTER (WHERE status = 'issued'),
            COUNT(*) FILTER (WHERE status = 'cancelled')
        FROM orders
    )");
    const Report report{
        row[0].as<int>(),
        row[1].as<int>(),
        row[2].as<int>()};
    transaction.commit();
    return report;
}

std::vector<Order> Database::listByStatus(const std::string& status)
{
    pqxx::work transaction(conn_);
    const std::string query = std::string("SELECT ") + orderColumns() + R"(
        FROM orders
        WHERE $1::text = '' OR status = $1
        ORDER BY created_at DESC, id DESC
    )";
    const pqxx::result result = transaction.exec_params(query, status);
    std::vector<Order> orders = makeOrders(result);
    transaction.commit();
    return orders;
}

int Database::countClosedOrders()
{
    pqxx::work transaction(conn_);
    const pqxx::row row = transaction.exec1(
        "SELECT COUNT(*) FROM orders WHERE status IN ('issued', 'cancelled')");
    const int count = row[0].as<int>();
    transaction.commit();
    return count;
}

int Database::deleteClosedOrders()
{
    pqxx::work transaction(conn_);
    const pqxx::result result = transaction.exec(
        "DELETE FROM orders WHERE status IN ('issued', 'cancelled')");
    const int count = static_cast<int>(result.affected_rows());
    transaction.commit();
    return count;
}
