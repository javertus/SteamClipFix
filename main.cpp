#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <regex>
#include "json.hpp"

namespace fs = std::__fs::filesystem;
using json = nlohmann::json;

void processJsonFile(const fs::path &filePath) {
    std::ifstream inFile(filePath.string());
    if (!inFile.is_open()) {
        std::cerr << "Cannot open: " << filePath << std::endl;
        return;
    }

    json j;
    try {
        inFile >> j;
    } catch (const std::exception &e) {
        std::cerr << "JSON parse error (" << filePath << "): " << e.what() << std::endl;
        return;
    }

    inFile.close();

    if (!j.contains("entries") || !j["entries"].is_object()) {
        std::cerr << "Cannot find 'entries', maybe JSON file isn't corrupted?: " << filePath << std::endl;
        return;
    }

    json newEntries = json::array();
    for (auto &item : j["entries"].items()) {
        newEntries.push_back(item.value());
    }

    j["entries"] = newEntries;

    std::ofstream outFile(filePath.string());
    outFile << std::setw(4) << j;
    std::cout << "Fixed: " << filePath << std::endl;
}

int main() {
    std::string folderPath;
    std::cout << "Enter the folder path that contains JSONs: ";
    std::getline(std::cin, folderPath);

    fs::path folder(folderPath);
    if (!fs::exists(folder) || !fs::is_directory(folder)) {
        std::cerr << "Invalid file path!" << std::endl;
        return 1;
    }

    for (const auto &entry : fs::directory_iterator(folder)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            processJsonFile(entry.path());
        }
    }

    std::cout << "Completed! Restart Steam to see changes.\n";
    std::cin.get();
    return 0;
}