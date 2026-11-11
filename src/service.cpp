#include "service.h"
#include "employee-odb.hxx"
#include "asset-odb.hxx"
#include "assignment-odb.hxx"
#include <odb/pgsql/exceptions.hxx>
#include <odb/transaction.hxx>
#include <ctime>
#include <memory>
#include <unordered_map>

using QEmp = odb::query<Employee>;
using QAst = odb::query<Asset>;
using QAsg = odb::query<Assignment>;

namespace {
template<class T>
std::vector<T> collect(odb::result<T> rows) {
    std::vector<T> result;
    for (const auto& row : rows)
        result.push_back(row);
    return result;
}

template<class T>
T lockRow(odb::pgsql::database& db, long id) {
    using Query = odb::query<T>;
    std::unique_ptr<T> row(db.query_one<T>((Query::id == id) + " FOR UPDATE"));
    if (!row)
        throw odb::object_not_persistent();
    return *row;
}
}

std::vector<Employee> Service::employees() {
    odb::transaction tx(db_.begin());
    auto result = collect(db_.query<Employee>());
    tx.commit();
    return result;
}

void Service::addEmployee(const std::string& name, const std::string& email) {
    odb::transaction tx(db_.begin());
    Employee employee;
    employee.setFullName(name);
    employee.setEmail(email);
    try {
        db_.persist(employee);
        tx.commit();
    } catch (const odb::pgsql::database_exception& error) {
        if (error.sqlstate() == "23505")
            throw DomainError("Фотограф с email " + email + " уже существует");
        throw;
    }
}

void Service::updateEmployee(long id, const std::string& name,
                             const std::string& email, bool active) {
    odb::transaction tx(db_.begin());
    auto employee = lockRow<Employee>(db_, id);
    if (!active && !db_.query<Assignment>(
            QAsg::employeeId == id && QAsg::returnedAt.is_null()).empty())
        throw DomainError("У фотографа на руках техника — сначала примите возврат");
    employee.setFullName(name);
    employee.setEmail(email);
    employee.setActive(active);
    try {
        db_.update(employee);
        tx.commit();
    } catch (const odb::pgsql::database_exception& error) {
        if (error.sqlstate() == "23505")
            throw DomainError("Фотограф с email " + email + " уже существует");
        throw;
    }
}

std::vector<AssetRow> Service::assets() {
    odb::transaction tx(db_.begin());
    db_.execute("SET TRANSACTION ISOLATION LEVEL REPEATABLE READ READ ONLY");
    std::unordered_map<long, std::string> names;
    for (const auto& employee : db_.query<Employee>())
        names.emplace(employee.id(), employee.fullName());
    std::unordered_map<long, std::string> holders;
    for (const auto& assignment : db_.query<Assignment>(QAsg::returnedAt.is_null()))
        holders.emplace(assignment.assetId(), names.at(assignment.employeeId()));

    std::vector<AssetRow> result;
    for (const auto& asset : db_.query<Asset>()) {
        const auto holder = holders.find(asset.id());
        result.push_back({asset, holder == holders.end() ? std::string{} : holder->second});
    }
    tx.commit();
    return result;
}

void Service::persistAsset(const std::string& inv, const std::string& model,
                           const std::string& category) {
    Asset asset;
    asset.setInventoryNumber(inv);
    asset.setModel(model);
    asset.setCategory(category);
    try {
        db_.persist(asset);
    } catch (const odb::pgsql::database_exception& error) {
        if (error.sqlstate() == "23505")
            throw DomainError("Инвентарный номер " + inv + " уже существует");
        throw;
    }
}

void Service::addAsset(const std::string& inv, const std::string& model,
                       const std::string& category) {
    odb::transaction tx(db_.begin());
    persistAsset(inv, model, category);
    tx.commit();
}

Asset Service::lockAsset(long id) {
    return lockRow<Asset>(db_, id);
}

Employee Service::lockEmployee(long id) {
    return lockRow<Employee>(db_, id);
}

std::unique_ptr<Assignment> Service::activeAssignment(long assetId) {
    return std::unique_ptr<Assignment>(db_.query_one<Assignment>(
        QAsg::assetId == assetId && QAsg::returnedAt.is_null()));
}

void Service::recordIssue(long assetId, long employeeId, std::time_t issuedAt) {
    Assignment assignment;
    assignment.setAssetId(assetId);
    assignment.setEmployeeId(employeeId);
    assignment.setIssuedAt(issuedAt);
    db_.persist(assignment);
}

void Service::assign(Asset& asset, long employeeId, std::time_t issuedAt) {
    recordIssue(asset.id(), employeeId, issuedAt);
    asset.setAssigned(true);
    db_.update(asset);
}

void Service::issue(long assetId, long employeeId) {
    odb::transaction tx(db_.begin());
    const auto employee = lockEmployee(employeeId);
    auto asset = lockAsset(assetId);
    if (asset.isAssigned())
        throw DomainError("Техника уже выдана");
    if (!employee.isActive())
        throw DomainError("Нельзя выдать уволенному фотографу");
    assign(asset, employeeId, std::time(nullptr));
    tx.commit();
}

void Service::giveBack(long assetId) {
    odb::transaction tx(db_.begin());
    auto asset = lockAsset(assetId);
    auto assignment = activeAssignment(assetId);
    if (!assignment)
        throw DomainError("Нет активной выдачи для этой техники");
    assignment->setReturnedAt(std::time(nullptr));
    db_.update(*assignment);
    asset.setAssigned(false);
    db_.update(asset);
    tx.commit();
}

std::vector<Employee> Service::activeEmployees() {
    odb::transaction tx(db_.begin());
    auto result = collect(db_.query<Employee>(QEmp::active == true));
    tx.commit();
    return result;
}

std::vector<Asset> Service::freeAssets() {
    odb::transaction tx(db_.begin());
    auto result = collect(db_.query<Asset>(QAst::assigned == false));
    tx.commit();
    return result;
}

