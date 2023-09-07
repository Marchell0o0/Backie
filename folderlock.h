//#ifndef FOLDERLOCK_H
//#define FOLDERLOCK_H


//#include <Windows.h>
//#include <string>

//class FolderLock {
//public:
//    FolderLock(const std::string& folderPath)
//        : handle(nullptr), locked(false)
//    {
//        // Create a mutex name by hashing or otherwise encoding the folder path
//        std::string mutexName = "FolderLock_" + folderPath;  // Replace spaces or other invalid characters if needed

//        handle = CreateMutex(NULL, FALSE, mutexName.c_str());
//        if (handle == NULL) {
//            // Handle error (use GetLastError())
//            return;
//        }

//        DWORD dwWaitResult = WaitForSingleObject(handle, INFINITE);
//        switch (dwWaitResult) {
//        case WAIT_OBJECT_0:
//            locked = true;
//            break;
//        case WAIT_TIMEOUT:
//            // Timeout logic here (should not happen in this case due to INFINITE)
//            break;
//        default:
//            // Handle error (use GetLastError())
//            break;
//        }
//    }

//    ~FolderLock() {
//        if (locked) {
//            ReleaseMutex(handle);
//        }
//        if (handle) {
//            CloseHandle(handle);
//        }
//    }

//    bool isLocked() const {
//        return locked;
//    }

//private:
//    HANDLE handle;
//    bool locked;
//};

//#endif // FOLDERLOCK_H
