#pragma once
#include <ctime>
#include <odb/core.hxx>
#include <odb/nullable.hxx>

#pragma db object table("assignments")
class Assignment {
public:
    long id() const { return id_; }
    long assetId() const { return assetId_; }
    long employeeId() const { return employeeId_; }
    std::time_t issuedAt() const { return issuedAt_; }
    bool isActive() const { return returnedAt_.null(); }

    void setAssetId(long v) { assetId_ = v; }
    void setEmployeeId(long v) { employeeId_ = v; }
    void setIssuedAt(std::time_t v) { issuedAt_ = v; }
    void setReturnedAt(std::time_t v) { returnedAt_ = v; }

private:
    friend class odb::access;

    // TODO: допишите ODB-прагмы для полей класса Assignment.
    //
    // Требования к маппингу на таблицу assignments:
    //
    //   id_         — первичный ключ, генерируется базой.
    //   assetId_    — обязательный внешний ключ, колонка "asset_id".
    //   employeeId_ — обязательный внешний ключ, колонка "employee_id".
    //   issuedAt_   — обязательное, колонка "issued_at".
    //   returnedAt_ — НЕОБЯЗАТЕЛЬНОЕ (nullable): у активной выдачи ещё нет
    //                 даты возврата. Колонка "returned_at".
    //                 В C++ тип — odb::nullable<std::time_t>.
    //

    long id_ = 0;
    long assetId_ = 0;
    long employeeId_ = 0;
    std::time_t issuedAt_ = 0;
    odb::nullable<std::time_t> returnedAt_;
};