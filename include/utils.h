#ifndef UTILS_H
#define UTILS_H

std::wstring strToWStr(const std::string& s);

std::string generate_uuid_v4();

bool IsRunningAsAdmin();

std::optional<fs::path> findAppDataFolder();

// void loadDefaultFonts();

// void loadStyleSheet(const QString& stylePath, QWidget* widget);

#endif // UTILS_H
