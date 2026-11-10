#pragma once
#include <string>
#include <odb/core.hxx>

#pragma db object table("assets")
class Asset {
public:
    long id() const { return id_; }
    const std::string& inventoryNumber() const { return inventoryNumber_; }
    const std::string& model() const { return model_; }
    const std::string& category() const { return category_; }
    bool isAssigned() const { return assigned_; }

    void setInventoryNumber(const std::string& v) { inventoryNumber_ = v; }
    void setModel(const std::string& v) { model_ = v; }
    void setCategory(const std::string& v) { category_ = v; }
    void setAssigned(bool v) { assigned_ = v; }

private:
    friend class odb::access;

    // TODO: допишите ODB-прагмы для полей класса Asset.
    //
    // Требования к маппингу на таблицу assets:
    //
    //   id_              — первичный ключ, генерируется базой (BIGSERIAL).
    //   inventoryNumber_ — обязательное, уникальное, колонка "inventory_number".
    //   model_           — обязательное, колонка "model".
    //   category_        — обязательное, колонка "category".
    //   assigned_        — обязательное, колонка "assigned".
    //
    // Подсказка: в include/domain/employee.hxx все прагмы уже написаны —
    // используйте его как образец. Там есть id auto, not_null, unique, column.

    long id_ = 0;
    std::string inventoryNumber_;
    std::string model_;
    std::string category_;
    bool assigned_ = false;
};