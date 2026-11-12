#pragma once

#include <stdexcept>
#include <string>
#include <utility>

class World {
public:
    static constexpr int width = 12;
    static constexpr int height = 8;

    World(int id, std::string name) : id_(id) { rename(std::move(name)); }

    int id() const { return id_; }
    const std::string& name() const { return name_; }

    void rename(std::string name) {
        if (name.empty() || name.size() > 80 || name.find_first_not_of(" \t\r\n") == std::string::npos)
            throw std::invalid_argument("Имя мира должно содержать от 1 до 80 байт");
        name_ = std::move(name);
    }

private:
    int id_;
    std::string name_;
};

enum class Material { dirt, stone, wood };

inline Material parseMaterial(const std::string& name) {
    if (name == "dirt") return Material::dirt;
    if (name == "stone") return Material::stone;
    if (name == "wood") return Material::wood;
    throw std::invalid_argument("Материалы: dirt, stone, wood");
}

inline const char* materialName(Material material) {
    switch (material) {
        case Material::dirt: return "dirt";
        case Material::stone: return "stone";
        case Material::wood: return "wood";
    }
    throw std::invalid_argument("Неизвестный материал");
}

class Block {
public:
    Block(int x, int y, Material material) : x_(x), y_(y), material_(material) {
        checkPosition(x, y);
    }

    int x() const { return x_; }
    int y() const { return y_; }
    Material material() const { return material_; }

    char symbol() const {
        switch (material_) {
            case Material::dirt: return 'D';
            case Material::stone: return 'S';
            case Material::wood: return 'W';
        }
        throw std::invalid_argument("Неизвестный материал");
    }

    static void checkPosition(int x, int y) {
        if (x < 0 || x >= World::width || y < 0 || y >= World::height)
            throw std::invalid_argument("Координаты за пределами карты");
    }

private:
    int x_;
    int y_;
    Material material_;
};

