#pragma once

#include <QString>

#include <filesystem>
#include <functional>

#define U8(string) ((const char*)u8##string)

enum class InstallStatus {
    Installed,
    Uninstalled,
    Incomplete,
};

struct GamePaths {
    std::filesystem::path gameFile;
    std::filesystem::path gameDir;
    std::filesystem::path assetDir;
    std::filesystem::path originalAssemblyFile;
    std::filesystem::path assetFileInstallPath;
};

struct Utility {
    Utility() = delete;

    static std::string getFileHash(const std::string_view& filename);
    static GamePaths getGamePaths(const std::filesystem::path& gameFile);
    static bool checkModIntegrity(std::function<void(const QString&)> error_callback);
    static bool checkGameIntergrity(const GamePaths& gamePaths, std::function<void(const QString&)> error_callback);
    static bool checkIntergrity(const GamePaths& gamePaths, std::function<void(const QString&)> error_callback);
    static InstallStatus checkInstallStatus(const GamePaths& gamePaths);
};