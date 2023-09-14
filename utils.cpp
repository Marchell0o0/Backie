#include <QApplication>
#include <QWidget>
#include <QFile>

#include "spdlog/spdlog.h"
#include "utils.h"

#include <winnls.h>
#include <windows.h>

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

//BackupType typeFromStr(const std::string& string){
//    BackupType type;
//    if (string == "FULL"){
//        type = BackupType::FULL;
//    } else if (string == "INCREMENTAL") {
//        type = BackupType::INCREMENTAL;
//    } else {
//        type = BackupType::NONE;
//    }
//    return type;
//}

//const std::string strFromType(const BackupType type){
//    switch (type) {
//    case BackupType::FULL:
//        return "FULL";
//    case BackupType::INCREMENTAL:
//        return "INCREMENTAL";
//    case BackupType::NONE:
//        return "NONE";
//    }
//    return "";
//}

void loadStyleSheet(const QString& stylePath, QWidget* widget) {
    QFile styleFile(stylePath);
    if (styleFile.open(QFile::ReadOnly)) {
        QString screenStyleSheet = QString::fromUtf8(styleFile.readAll());

        if (widget != nullptr) {
            if (widget->window() != nullptr && widget->isEnabled()) {
                widget->setStyleSheet(screenStyleSheet);
            } else {
                SPDLOG_WARN(
                    "Widget is either orphan or disabled."
                    " Stylesheet might not be applied as expected for widget: {}",
                    widget ? widget->objectName().toStdString() : "null");
            }
        } else {
            qApp->setStyleSheet(screenStyleSheet);
        }
    } else {
        SPDLOG_ERROR("Could not open styles for widget: {}", widget ? widget->objectName().toStdString() : "null");
    }
}

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
