#include <algorithm>
#include <charconv>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_set>
#include <vector>

namespace fs = std::filesystem;

enum class Output { lines, count, files };
enum class Prefix { automatic, always, never };

struct Options {
    bool recursive = false;
    bool ignoreCase = false;
    bool invert = false;
    bool lineNumbers = false;
    bool word = false;
    bool exact = false;
    bool quiet = false;
    unsigned jobs = std::max(1u, std::thread::hardware_concurrency());
    Output output = Output::lines;
    Prefix prefix = Prefix::automatic;
    std::string pattern;
    std::vector<fs::path> paths;
};

unsigned parseJobs(std::string_view value) {
    unsigned jobs = 0;
    const auto end = value.data() + value.size();
    const auto parsed = std::from_chars(value.data(), end, jobs);
    if (parsed.ec != std::errc{} || parsed.ptr != end || jobs == 0)
        throw std::runtime_error("-j requires a positive integer");
    return jobs;
}

Options parseArguments(int argc, char** argv) {
    Options options;
    int i = 1;
    for (; i < argc; ++i) {
        const std::string_view arg = argv[i];
        if (arg == "--") {
            ++i;
            break;
        }
        if (arg.size() < 2 || arg.front() != '-')
            break;
        for (std::size_t j = 1; j < arg.size(); ++j) {
            switch (arg[j]) {
                case 'r': options.recursive = true; break;
                case 'i': options.ignoreCase = true; break;
                case 'v': options.invert = true; break;
                case 'n': options.lineNumbers = true; break;
                case 'H': options.prefix = Prefix::always; break;
                case 'h': options.prefix = Prefix::never; break;
                case 'c':
                    if (options.output != Output::files)
                        options.output = Output::count;
                    break;
                case 'l': options.output = Output::files; break;
                case 'w': options.word = true; break;
                case 'x': options.exact = true; break;
                case 'q': options.quiet = true; break;
                case 'j':
                    if (j + 1 < arg.size()) {
                        options.jobs = parseJobs(arg.substr(j + 1));
                    } else {
                        if (++i == argc)
                            throw std::runtime_error("Missing value after -j");
                        options.jobs = parseJobs(argv[i]);
                    }
                    j = arg.size() - 1;
                    break;
                default: throw std::runtime_error(std::string("Unknown option -") + arg[j]);
            }
        }
    }
    if (i == argc)
        throw std::runtime_error("Missing pattern");

    options.pattern = argv[i++];
    for (; i < argc; ++i)
        options.paths.emplace_back(argv[i]);
    if (options.paths.empty())
        options.paths.emplace_back(".");
    return options;
}

char lowerAscii(char ch) {
    return ch >= 'A' && ch <= 'Z' ? static_cast<char>(ch + ('a' - 'A')) : ch;
}

bool wordCharacter(char ch) {
    ch = lowerAscii(ch);
    return (ch >= 'a' && ch <= 'z') || (ch >= '0' && ch <= '9') || ch == '_';
}

class Matcher {
public:
    explicit Matcher(const Options& options) : options_(options), pattern_(options.pattern) {
        if (options_.ignoreCase)
            std::transform(pattern_.begin(), pattern_.end(), pattern_.begin(), lowerAscii);
    }

    bool selects(const std::string& line) const {
        if (!options_.ignoreCase)
            return matches(line) != options_.invert;

        auto folded = line;
        std::transform(folded.begin(), folded.end(), folded.begin(), lowerAscii);
        return matches(folded) != options_.invert;
    }

private:
    bool matches(std::string_view line) const {
        if (options_.exact)
            return line == pattern_;
        if (pattern_.empty())
            return true;
        auto pos = line.find(pattern_);
        if (!options_.word)
            return pos != std::string_view::npos;
        while (pos != std::string_view::npos) {
            const auto end = pos + pattern_.size();
            if ((pos == 0 || !wordCharacter(line[pos - 1])) &&
                (end == line.size() || !wordCharacter(line[end])))
                return true;
            pos = line.find(pattern_, pos + 1);
        }
        return false;
    }

    const Options& options_;
    std::string pattern_;
};

struct Summary {
    std::size_t processed = 0;
    std::size_t matches = 0;
    std::size_t errors = 0;
};

class Search {
public:
    explicit Search(const Options& options)
        : options_(options), matcher_(options),
          showPath_(options.prefix == Prefix::always ||
              (options.prefix == Prefix::automatic &&
                   (options.recursive || options.paths.size() > 1))) {}

    Summary run() {
        for (const auto& path : options_.paths)
            collect(path);
        if (options_.jobs == 1 || files_.empty()) {
            work();
        } else {
            std::vector<std::thread> threads;
            const auto jobs = std::min<std::size_t>(options_.jobs, files_.size());
            threads.reserve(jobs);
            try {
                for (std::size_t i = 0; i < jobs; ++i)
                    threads.emplace_back([this] { work(); });
            } catch (const std::system_error&) {
                work();
            }
            for (auto& thread : threads)
                thread.join();
        }
        return summary_;
    }

private:
    void collect(const fs::path& root) {
        std::vector<fs::path> pending{root};
        while (!pending.empty()) {
            auto path = std::move(pending.back());
            pending.pop_back();

            std::error_code error;
            const auto status = fs::symlink_status(path, error);
            if (error) {
                reportError(path, error.message());
            } else if (fs::is_regular_file(status)) {
                const auto canonical = fs::weakly_canonical(path, error);
                if (seen_.insert(error ? path.lexically_normal() : canonical).second)
                    files_.push(path);
            } else if (fs::is_directory(status)) {
                if (!options_.recursive) {
                    reportError(path, "is a directory");
                    continue;
                }
                fs::directory_iterator it(path, error), end;
                while (!error && it != end) {
                    pending.push_back(it->path());
                    it.increment(error);
                }
                if (error)
                    reportError(path, error.message());
            } else if (!fs::is_symlink(status)) {
                reportError(path, "not a regular file");
            }
        }
    }

    void reportError(const fs::path& path, const std::string& message) {
        {
            std::lock_guard<std::mutex> lock(summaryMutex_);
            ++summary_.errors;
        }
        if (!options_.quiet) {
            std::lock_guard<std::mutex> lock(outputMutex_);
            std::cerr << "pgrep: " << path.string() << ": " << message << '\n';
        }
    }

    void work() {
        while (true) {
            fs::path path;
            {
                std::lock_guard<std::mutex> lock(queueMutex_);
                if (files_.empty())
                    return;
                path = std::move(files_.front());
                files_.pop();
            }
            searchFile(path);
        }
    }

    void searchFile(const fs::path& path) {
        std::ifstream input(path, std::ios::binary);
        if (!input) {
            reportError(path, "cannot open file");
            return;
        }

        const auto name = path.string();
        const auto prefix = showPath_ ? name + ':' : std::string{};
        std::string line;
        std::size_t number = 0;
        std::size_t matches = 0;
        while (std::getline(input, line)) {
            ++number;
            if (!line.empty() && line.back() == '\r' && !input.eof())
                line.pop_back();
            if (!matcher_.selects(line))
                continue;
            ++matches;
            if (!options_.quiet && options_.output == Output::lines) {
                std::lock_guard<std::mutex> lock(outputMutex_);
                std::cout << prefix;
                if (options_.lineNumbers)
                    std::cout << number << ':';
                std::cout << line << '\n';
            }
        }
        if (input.bad() || !input.eof()) {
            reportError(path, "read failed");
            return;
        }
        if (!options_.quiet && options_.output != Output::lines) {
            std::lock_guard<std::mutex> lock(outputMutex_);
            if (options_.output == Output::count)
                std::cout << prefix << matches << '\n';
            else if (matches != 0)
                std::cout << name << '\n';
        }
        {
            std::lock_guard<std::mutex> lock(summaryMutex_);
            ++summary_.processed;
            summary_.matches += matches;
        }
    }

    const Options& options_;
    const Matcher matcher_;
    const bool showPath_;
    Summary summary_;
    std::unordered_set<fs::path> seen_;
    std::queue<fs::path> files_;
    std::mutex queueMutex_;
    std::mutex outputMutex_;
    std::mutex summaryMutex_;
};

int main(int argc, char** argv) {
    try {
        const auto options = parseArguments(argc, argv);
        const auto start = std::chrono::steady_clock::now();
        const auto summary = Search(options).run();
        const std::chrono::duration<double> elapsed = std::chrono::steady_clock::now() - start;
        if (!options.quiet)
            std::cerr << "Processed: " << summary.processed << " files, "
                      << summary.matches << " matches, " << summary.errors << " errors, "
                      << std::fixed << std::setprecision(3) << elapsed.count() << " s\n";
        if (summary.errors != 0)
            return 2;
        return summary.matches != 0 ? 0 : 1;
    } catch (const std::exception& error) {
        std::cerr << "pgrep: " << error.what() << '\n'
                  << "Usage: pgrep [OPTIONS] PATTERN [PATH...]\n";
        return 2;
    }
}

