#include "db.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cctype>

namespace {

bool looksLikePhone(const std::string& s) {
    if (s.empty()) return false;
    int digits = 0;
    for (char c : s) {
        unsigned char uc = static_cast<unsigned char>(c);
        if (std::isdigit(uc)) { digits++; continue; }
        if (c == '+' || c == '-' || c == ' ' || c == '(' || c == ')') continue;
        return false;
    }
    return digits >= 10;
}

} 

Database::Database(const std::string& conn_str)
    : conn_(conn_str)
{
    if (!conn_.is_open())
        throw std::runtime_error("Не удалось подключиться к PostgreSQL");
}

std::vector<Order> Database::listOrders(const std::string& filter) {
    // TODO
    return {};
}

std::string Database::addOrder(const std::string& contact,
                               const std::string& cell) {
    // TODO
    return "";
}

bool Database::issueOrder(const std::string& code) {
    // TODO
    return false;
}

bool Database::cancelOrder(const std::string& code) {
    // TODO
    return false;
}

Report Database::buildReport() {
    // TODO
    return {};
}

std::vector<Order> Database::listByStatus(const std::string& status) {
    // TODO
    return {};
}

int Database::countClosedOrders() {
    // TODO
    return 0;
}

int Database::deleteClosedOrders() {
    // TODO
    return 0;
}