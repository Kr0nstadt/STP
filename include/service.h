#pragma once
#include "domain.hxx"
#include <odb/pgsql/database.hxx>
#include <vector>
#include <memory>
#include <string>

struct AssetRow {
    Asset asset;
    std::string holder;
};

class Service {
public:
    explicit Service(odb::pgsql::database& db) : db_(db) {}

    // ---------- Сотрудники ----------
    std::vector<Employee> employees();
    void addEmployee(const std::string& name, const std::string& email);
    void setEmployeeActive(long id, bool active);
    void updateEmployee(long id, const std::string& name,
                        const std::string& email, bool active);

    // ---------- Оборудование ----------
    std::vector<AssetRow> assets();
    void addAsset(const std::string& inv, const std::string& model,
                  const std::string& category);

    // ---------- Выдача ----------
    void issue(long assetId, long employeeId);
    void giveBack(long assetId);

    // ---------- Для диалогов ----------
    std::vector<Employee> activeEmployees();
    std::vector<Asset>    freeAssets();

private:
    void persistAsset(const std::string& inv, const std::string& model,
                      const std::string& category);
    Asset lockAsset(long id);
    Employee lockEmployee(long id);
    std::unique_ptr<Assignment> activeAssignment(long assetId);
    void recordIssue(long assetId, long employeeId, std::time_t issuedAt);
    void assign(Asset& asset, long employeeId, std::time_t issuedAt);

    odb::pgsql::database& db_;
};