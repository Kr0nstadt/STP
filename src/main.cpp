#include "converter.hpp"

#include <exception>
#include <iostream>
#include <string_view>

int main(int argc, char* argv[]) {
    constexpr auto usage = "Usage: dataconv <input_file> <output_file>\n";
    if (argc == 2 && std::string_view(argv[1]) == "--help") {
        std::cout << usage;
        return 0;
    }
    if (argc != 3) {
        std::cerr << usage;
        return 2;
    }
    try {
        dataconv::convert_file(argv[1], argv[2]);
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "dataconv: " << e.what() << '\n';
        return 1;
    }
}
