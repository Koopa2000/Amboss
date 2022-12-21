#ifndef APROCESSHANDLE_H
#define APROCESSHANDLE_H

#include "asystemprocessinfo.h"

enum HandleType
{
    ProcessHandle,
    ThreadHandle,
    Error
};

struct AHandle
{
    bool operator ==(const AHandle &other) const;
    bool operator !=(const AHandle &other) const;

    bool isVaild() const;
    bool isProcess() const;
    bool isThread() const;

    HANDLE handle = NULL;
    ULONG pid;
    ULONG tid;
    ULONG accessRights;
    HandleType type = Error;
    bool inherit;
};

enum HandleOpenMode
{
    CheckExisting,
    AlwaysNew,
    DontAddToVector
};

class AProcessHandleManager
{
public:
    AProcessHandleManager();
    ~AProcessHandleManager();

    HANDLE openHandle(const ULONG &ID, const ULONG &AccessRights, const bool &InheritHandle, HandleType &Type) const;

    void setHandleOpenMode(const HandleOpenMode &Mode);
    HandleOpenMode handleOpenMode() const;

    AHandle openHandle(const ULONG &ID, const ULONG &AccessRights, const bool &InheritHandle, const HandleOpenMode &Mode);
    AHandle openHandle(const ULONG &ID, const ULONG &AccessRights, const bool &InheritHandle);

    std::vector<AHandle> openProcessThreads(const ULONG &PID, const ULONG &AccessRights, const bool &InheritHandle, const HandleOpenMode &Mode);

    bool isAlreadyOpen(const AHandle &handle) const;
    int isAlreadyOpen(const ULONG &ID, const ULONG &AccessRights, const bool &InheritHandle) const;

    bool isValid(const std::vector<AHandle> &Handles) const;

    AHandle currentProcessPseudo() const;
    AHandle currentProcessReal(const bool &InheritHandle);
    AHandle currentProcessReal(const bool &InheritHandle, const HandleOpenMode &Mode);

    AHandle currentThreadPseudo() const;
    AHandle currentThreadReal(const bool &InheritHandle);
    AHandle currentThreadReal(const bool &InheritHandle, const HandleOpenMode &Mode);

    AHandle atPos(const int &vectorPosition) const;

    int closeAll();
    int close(const HandleType &Type);
    int close(const AHandle &Handle);
    int close(const std::vector<AHandle> &Handles);

    int totalHandleCount() const;
    int handleCount(const HandleType &Type) const;

    std::vector<AHandle> allHandles() const;
    std::vector<AHandle> handles(const HandleType &Type) const;

    void addToVector(const AHandle &handle);
    void addToVector(const std::vector<AHandle> &handle);

private:
    ASystemProcessInfo spi;

    std::vector<AHandle> hVec;
    HandleOpenMode hom = CheckExisting;
};

#endif // APROCESSHANDLE_H
