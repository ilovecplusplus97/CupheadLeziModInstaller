#include "mainwindow.h"
#include "constants.h"
#include "ui_mainwindow.h"
#include "utility.h"

#include <QFileDialog>
#include <QMessageBox>
#include <filesystem>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    this->setFixedSize(this->size());

    if (!Utility::checkModIntegrity(
            [this](const QString& error) { QMessageBox::critical(this, tr(U8("错误")), error); })) {
        QApplication::quit();
    }
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_gamePathSelectButton_clicked() {
    QString gameFile =
        QFileDialog::getOpenFileName(this, tr(U8("请选择你的游戏文件")), {}, tr(U8("游戏文件 (Cuphead.exe)")));
    if (gameFile.isEmpty()) {
        return;
    }
    ui->gamePathInput->setText(gameFile);
}

void MainWindow::on_installButton_clicked() {
    GamePaths gamePaths = Utility::getGamePaths(ui->gamePathInput->text().toStdString());
    if (!Utility::checkIntergrity(
            gamePaths, [this](const QString& error) { QMessageBox::critical(this, tr(U8("安装失败")), error); })) {
        return;
    }
    if (Utility::checkInstallStatus(gamePaths) != InstallStatus::Uninstalled) {
        QMessageBox::StandardButton result =
            QMessageBox::information(this, tr(U8("提示")), tr(U8("检查到你已安装乐子mod，\n是否要重新安装？")),
                                     QMessageBox::Yes | QMessageBox::No);
        if (!(result & QMessageBox::Yes)) {
            return;
        }
    }
    std::filesystem::remove(gamePaths.originalAssemblyFile);
    std::filesystem::copy(assemblyFile, gamePaths.originalAssemblyFile);
    if (std::filesystem::exists(gamePaths.assetFileInstallPath)) {
        std::filesystem::remove(gamePaths.assetFileInstallPath);
    }
    std::filesystem::copy(assetFile, gamePaths.assetFileInstallPath);

    QMessageBox::information(this, tr(U8("完成")), tr(U8("已完成安装")));
}

void MainWindow::on_uninstallButton_clicked() {
    GamePaths gamePaths = Utility::getGamePaths(ui->gamePathInput->text().toStdString());
    if (!Utility::checkIntergrity(
            gamePaths, [this](const QString& error) { QMessageBox::critical(this, tr(U8("卸载失败")), error); })) {
        return;
    }
    if (Utility::checkInstallStatus(gamePaths) == InstallStatus::Uninstalled) {
        QMessageBox::critical(this, tr(U8("卸载失败")), U8("未安装乐子mod"));
        return;
    }
    std::filesystem::remove(gamePaths.originalAssemblyFile);
    std::filesystem::copy(assemblyBackupFile, gamePaths.originalAssemblyFile);
    if (std::filesystem::exists(gamePaths.assetFileInstallPath)) {
        std::filesystem::remove(gamePaths.assetFileInstallPath);
    }

    QMessageBox::information(this, tr(U8("完成")), tr(U8("已完成卸载")));
}
