#ifndef PCH_H
#define PCH_H

#include "json.hpp"
#include "spdlog/spdlog.h"

#include "windows.h"

#include <ShlObj.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_set>
#include <vector>

// #include <QApplication>
// #include <QFile>
// #include <QFontDatabase>
// #include <QString>
// #include <QWidget>

#include <comdef.h>
#include <taskschd.h>
#include <wrl/client.h>

// #include "utils.h"

namespace fs = std::filesystem;
using json = nlohmann::json;
using Microsoft::WRL::ComPtr;

#endif // PCH_H
