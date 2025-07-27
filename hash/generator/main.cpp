#include <array>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <format>
#include <string_view>

#include <md5.h>

std::string getHash(std::istream& in) {
    constexpr std::size_t BUFFER_SIZE = 1024;

    MD5 md5;
    std::array<char, BUFFER_SIZE> buffer;
    while (in.read(buffer.data(), BUFFER_SIZE)) {
        std::size_t len = in.gcount();
        md5.add(buffer.data(), len);
    }
    return md5.getHash();
}

void generateHeader(std::ostream& out, const std::string_view& hash) {
    out << "#pragma once\n" 
        << "#include <string_view>\n" 
        << std::format("constexpr std::string_view assemblyFileHash = \"{}\";", hash);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: hash-generator [input-file] [output-file]\n";
        return 0;
    }
    std::string_view inputFile = argv[1];
    std::string_view outputFile = argv[2];
    if (!std::filesystem::exists(inputFile)) {
        std::cout << std::format("file '{}' not found", inputFile);
        return 0;
    }

    std::ifstream ifs(inputFile.data(), std::ios::in | std::ios::binary);
    std::string fileHash = getHash(ifs);
    ifs.close();

    std::ofstream ofs(outputFile.data());
    generateHeader(ofs, fileHash);
    ofs.close();

    return 0;
}