#include "pch.h"

#include "utils.h"
#include <random>

std::wstring strToWStr(const std::string& s) {
    int len;
    int slength = (int)s.length() + 1;
    len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

std::optional<fs::path> findAppDataFolder() {
	char pathToAppData[MAX_PATH];
	if (SUCCEEDED(
			SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, pathToAppData))) {
		fs::path appData = pathToAppData;
		fs::path backieFolder = appData / "Backie";
		if (!fs::exists(backieFolder)) {
			SPDLOG_DEBUG("Creating new folder in AppData");
			fs::create_directory(backieFolder);
		}
		SPDLOG_DEBUG("AppData Backie folder already exists");
		return backieFolder;
	} else {
		// TODO: Couldn't find appdata (crossplatform)
		return std::nullopt;
	}
}

bool IsRunningAsAdmin() {
    BOOL isRunAsAdmin = FALSE;
    PSID adminGroupSid = NULL;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;
    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
                                 DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroupSid)) {
        if (!CheckTokenMembership(NULL, adminGroupSid, &isRunAsAdmin)) {
            isRunAsAdmin = FALSE;
        }
        FreeSid(adminGroupSid);
    }
    return isRunAsAdmin;
}

// void loadDefaultFonts() {
//     QFontDatabase::addApplicationFont(":/resources/assets/fonts/Roboto/Roboto-Regular.ttf");
//     QFontDatabase::addApplicationFont(":/resources/assets/fonts/Roboto/Roboto-Italic.ttf");
//     QFontDatabase::addApplicationFont(":/resources/assets/fonts/Roboto/Roboto-Medium.ttf");
//     QFontDatabase::addApplicationFont(":/resources/assets/fonts/Roboto/Roboto-Bold.ttf");

// //    QFont defaultFont("Roboto", 14);
// //    QApplication::setFont(defaultFont);
// }

// void loadStyleSheet(const QString& stylePath, QWidget* widget) {
//     QFile styleFile(stylePath);
//     if (styleFile.open(QFile::ReadOnly)) {
//         QString screenStyleSheet = QString::fromUtf8(styleFile.readAll());

//         if (widget != nullptr) {
//             if (widget->window() != nullptr && widget->isEnabled()) {
//                 widget->setStyleSheet(screenStyleSheet);
//             } else {
//                 SPDLOG_WARN(
//                     "Widget is either orphan or disabled."
//                     " Stylesheet might not be applied as expected for widget:
//                     {}", widget ? widget->objectName().toStdString() :
//                     "null");
//             }
//         } else {
//             qApp->setStyleSheet(screenStyleSheet);
//         }
//     } else {
//         SPDLOG_ERROR("Could not open styles for widget: {}", widget ?
//         widget->objectName().toStdString() : "null");
//     }
// }

std::string generate_uuid_v4() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    std::uniform_int_distribution<> dis2(8, 11);

    std::stringstream ss;
    int i;
    ss << std::hex;
    for (i = 0; i < 8; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 4; i++) {
        ss << dis(gen);
    }
    ss << "-4";
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    ss << dis2(gen);
    for (i = 0; i < 3; i++) {
        ss << dis(gen);
    }
    ss << "-";
    for (i = 0; i < 12; i++) {
        ss << dis(gen);
    };
    return ss.str();
}
