#include "utility.h"
#include "constants.h"

#include <QCryptographicHash>
#include <QDataStream>
#include <QFile>
#include <QObject>

std::string Utility::getFileMD5(const std::string_view& filename) {
    QCryptographicHash hash(QCryptographicHash::Md5);
    QFile file(filename.data());
    file.open(QFile::ReadOnly);

    QDataStream stream(&file);
    char buffer[1024];
    while (size_t len = stream.readRawData(buffer, 1024)) {
        hash.addData(buffer, len);
    }

    file.close();

    return hash.result().toHex().toStdString();
}

GamePaths Utility::getGamePaths(const std::filesystem::path& gameFile) {
    std::filesystem::path gameDir = gameFile.parent_path();
    std::filesystem::path assetDir = gameDir / "Cuphead_Data/StreamingAssets";
    std::filesystem::path originalAssemblyFile = gameDir / "Cuphead_Data/Managed" / assemblyFile;
    std::filesystem::path assetFileInstallPath = assetDir / assetFile;
    return GamePaths{gameFile, gameDir, assetDir, originalAssemblyFile, assetFileInstallPath};
}

bool Utility::checkModIntegrity(std::function<void(const QString&)> error_callback) {
    if (std::filesystem::exists(assemblyFile) && std::filesystem::exists(assemblyBackupFile) &&
        std::filesystem::exists(assetFile)) {
        return true;
    }
    error_callback(QObject::tr(U8("MOD文件不完整")));
    return false;
}

bool Utility::checkGameIntergrity(const GamePaths& gamePaths, std::function<void(const QString&)> error_callback) {
    if (gamePaths.gameFile.empty()) {
        error_callback(QObject::tr(U8("游戏文件不能为空")));
        return false;
    }
    if (!std::filesystem::exists(gamePaths.gameFile)) {
        error_callback(QObject::tr(U8("游戏文件不存在")));
        return false;
    }

    if (!(std::filesystem::exists(gamePaths.originalAssemblyFile) && std::filesystem::exists(gamePaths.assetDir))) {
        error_callback(QObject::tr(U8("游戏文件不完整")));
        return false;
    }
    return true;
}

bool Utility::checkIntergrity(const GamePaths& gamePaths, std::function<void(const QString&)> error_callback) {
    if (!checkModIntegrity(error_callback)) {
        return false;
    }
    return checkGameIntergrity(gamePaths, error_callback);
}

InstallStatus Utility::checkInstallStatus(const GamePaths& gamePaths) {
    std::string originalAssemblyFileHash = getFileMD5(gamePaths.originalAssemblyFile.string());
    bool isInstalledAsset = std::filesystem::exists(gamePaths.assetFileInstallPath);
    bool isInstalledAssembly = (originalAssemblyFileHash == assemblyFileHash);
    if (isInstalledAsset && isInstalledAssembly) {
        return InstallStatus::Installed;
    }
    else if (isInstalledAsset || isInstalledAssembly) {
        return InstallStatus::Incomplete;
    }
    else {
        return InstallStatus::Uninstalled;
    }
}