#include "Image.h"
#include <algorithm>
#include <charconv>
#include <chrono>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace fs = std::filesystem;

enum class Operation { grayscale = 1, invert, both };

struct Task {
    fs::path input;
    fs::path output;
};

struct Statistics {
    std::size_t success = 0;
    std::size_t failed = 0;
};

std::string readLine(const char* prompt) {
    std::cout << prompt;
    std::string value;
    if (!std::getline(std::cin, value) || value.empty())
        throw std::runtime_error("Expected a value");
    return value;
}

unsigned readNumber(const char* prompt, unsigned maximum) {
    const auto value = readLine(prompt);
    unsigned number = 0;
    const auto end = value.data() + value.size();
    const auto parsed = std::from_chars(value.data(), end, number);
    if (parsed.ec != std::errc{} || parsed.ptr != end || number == 0 || number > maximum)
        throw std::runtime_error("Invalid number");
    return number;
}

std::vector<Task> findImages(const fs::path& input, const fs::path& output) {
    std::vector<Task> tasks;
    bool sameDirectory = fs::weakly_canonical(input) == fs::weakly_canonical(output);
    for (const auto& entry : fs::directory_iterator(input)) {
        if (!entry.is_regular_file())
            continue;

        auto extension = entry.path().extension().string();
        for (char& ch : extension) {
            if (ch >= 'A' && ch <= 'Z')
                ch += 'a' - 'A';
        }
        const auto name = entry.path().filename().string();
        const std::string suffix = ".processed.png";
        if (sameDirectory && name.size() >= suffix.size() &&
            name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
            continue;
        if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" ||
            extension == ".bmp" || extension == ".tga")
            tasks.push_back({entry.path(), output / (name + suffix)});
    }
    return tasks;
}

bool processImage(const Task& task, Operation operation) {
    try {
        Image image;
        if (!image.load(task.input.string()))
            return false;
        if (operation != Operation::invert)
            image.grayscale();
        if (operation != Operation::grayscale)
            image.invert();
        if (fs::is_symlink(task.output))
            return false;
        return image.save(task.output.string());
    } catch (const std::exception&) {
        return false;
    }
}

Statistics processImages(const std::vector<Task>& tasks, Operation operation, bool parallel,
                         unsigned workers) {
    Statistics statistics;
    std::mutex outputMutex;
    std::mutex statisticsMutex;
    const auto process = [&](const Task& task, unsigned worker) {
        const bool success = processImage(task, operation);
        {
            std::lock_guard<std::mutex> lock(outputMutex);
            if (parallel)
                std::cout << "[Thread " << worker << "] ";
            std::cout << (success ? "Completed: " : "Failed: ")
                      << task.input.filename().string() << '\n';
        }
        {
            std::lock_guard<std::mutex> lock(statisticsMutex);
            ++(success ? statistics.success : statistics.failed);
        }
    };

    if (!parallel) {
        for (const auto& task : tasks)
            process(task, 1);
        return statistics;
    }

    std::queue<Task> queue;
    for (const auto& task : tasks)
        queue.push(task);
    std::mutex queueMutex;
    const auto work = [&](unsigned worker) {
        while (true) {
            Task task;
            {
                std::lock_guard<std::mutex> lock(queueMutex);
                if (queue.empty())
                    return;
                task = std::move(queue.front());
                queue.pop();
            }
            process(task, worker);
        }
    };

    std::vector<std::thread> threads;
    threads.reserve(workers);
    try {
        for (unsigned i = 0; i < workers; ++i)
            threads.emplace_back(work, i + 1);
    } catch (...) {
        for (auto& thread : threads)
            thread.join();
        throw;
    }
    for (auto& thread : threads)
        thread.join();
    return statistics;
}

int main() {
    try {
        std::cout << "IMAGE PROCESSING UTILITY\n";
        const fs::path input = readLine("Enter input directory: ");
        const fs::path output = readLine("Enter output directory: ");
        if (!fs::is_directory(input))
            throw std::runtime_error("Input directory does not exist");
        std::cout << "Scanning input directory...\n";
        const auto tasks = findImages(input, output);
        std::cout << "Found " << tasks.size() << " supported image files.\n";
        if (tasks.empty())
            return 0;

        std::cout << "1. Grayscale\n2. Invert\n3. Grayscale + Invert\n";
        const auto choice = readNumber("Enter operation number: ", 3);
        const auto operation = static_cast<Operation>(choice);
        std::cout << "1. Sequential processing\n2. Parallel processing\n";
        const bool parallel = readNumber("Enter mode: ", 2) == 2;
        const auto workers = parallel
            ? readNumber("Enter number of worker threads (> 0): ", static_cast<unsigned>(-1))
            : 1u;
        fs::create_directories(output);

        const auto start = std::chrono::steady_clock::now();
        const auto stats = processImages(tasks, operation, parallel, workers);
        const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
        const char* names[] = {"Grayscale", "Invert", "Grayscale + Invert"};
        std::cout << "\nPROCESSING SUMMARY\n"
                  << "Mode:                    " << (parallel ? "Parallel" : "Sequential") << '\n'
                  << "Operation:               " << names[choice - 1] << '\n'
                  << "Worker threads:          " << workers << '\n'
                  << "Total images:            " << tasks.size() << '\n'
                  << "Successfully processed:  " << stats.success << '\n'
                  << "Failed:                  " << stats.failed << '\n'
                  << "Processing time:         " << std::fixed << std::setprecision(3)
                  << elapsed.count() << " seconds\n"
                  << "Results directory:       " << output.string() << '\n';
        return 0;
    } catch (const std::exception& error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}

