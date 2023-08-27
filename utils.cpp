#include "utils.h"

#include <winnls.h>
#include <windows.h>

std::wstring stringToWString(const std::string& s) {
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

BackupType typeFromStr(const std::string& string){
    BackupType type;
    if (string == "FULL"){
        type = BackupType::FULL;
    } else if (string == "INCREMENTAL") {
        type = BackupType::INCREMENTAL;
    } else {
        type = BackupType::NONE;
    }
    return type;
}

const std::string strFromType(const BackupType type){
    switch (type) {
    case BackupType::FULL:
        return "FULL";
    case BackupType::INCREMENTAL:
        return "INCREMENTAL";
    default:
        return "";
    }
}
