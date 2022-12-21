#include "AHandleManager.h"

bool AHandle::operator ==(const AHandle &other) const
{
    if(this->handle == other.handle && this->pid == other.pid && this->tid == other.tid &&
            this->accessRights == other.accessRights && this->type == other.type && this->inherit == other.inherit)
    {
        return true;
    }

    return false;
}

bool AHandle::operator !=(const AHandle &other) const
{
    if(this->handle == other.handle && this->pid == other.pid && this->tid == other.tid &&
            this->accessRights == other.accessRights && this->type == other.type && this->inherit == other.inherit)
    {
        return false;
    }

    return true;
}

bool AHandle::isVaild() const
{
    if(this->handle == NULL)
    {
        return false;
    }

    return true;
}

bool AHandle::isProcess() const
{
    if(this->type == ProcessHandle)
    {
        return true;
    }

    return false;
}

bool AHandle::isThread() const
{
    if(this->type == ThreadHandle)
    {
        return true;
    }

    return false;
}

AProcessHandleManager::AProcessHandleManager()
{

}

AProcessHandleManager::~AProcessHandleManager()
{
    closeAll();
}

HANDLE AProcessHandleManager::openHandle(const ULONG &ID, const ULONG &AccessRights, const bool &InheritHandle, HandleType &Type) const
{
    HANDLE handle = OpenProcess(AccessRights, InheritHandle, ID);

    if(handle == NULL)
    {
        handle = OpenThread(AccessRights, InheritHandle, ID);

        if(handle == NULL)
        {
            Type = Error;
            return handle;
        }

        Type = ThreadHandle;
        return handle;
    }

    Type = ProcessHandle;
    return handle;
}

void AProcessHandleManager::setHandleOpenMode(const HandleOpenMode &Mode)
{
    hom = Mode;
}

HandleOpenMode AProcessHandleManager::handleOpenMode() const
{
    return hom;
}

AHandle AProcessHandleManager::openHandle(const ULONG &ID, const ULONG &AccessRights, const bool &InheritHandle, const HandleOpenMode &Mode)
{
    int alreadyOpen = -1;

    if(Mode == CheckExisting)
    {
        alreadyOpen = isAlreadyOpen(ID, AccessRights, InheritHandle);
    }

    if(alreadyOpen == -1)
    {
        HandleType hType;
        HANDLE handle = openHandle(ID, AccessRights, InheritHandle, hType);

        if(handle != NULL)
        {
            AHandle aHandle;

            aHandle.handle = handle;

            if(hType == ProcessHandle)
            {
                aHandle.pid = ID;
                aHandle.tid = 0;
            }
            if(hType == ThreadHandle)
            {
                aHandle.pid = GetProcessIdOfThread(handle);
                aHandle.tid = ID;
            }

            aHandle.accessRights = AccessRights;
            aHandle.type = hType;
            aHandle.inherit = InheritHandle;

            if(Mode != DontAddToVector)
            {
                hVec.push_back(aHandle);
            }

            return aHandle;
        }
    }
    else
    {
        return hVec[alreadyOpen];
    }

    return AHandle();
}

AHandle AProcessHandleManager::openHandle(const ULONG &ID, const ULONG &AccessRights, const bool &InheritHandle)
{
    return openHandle(ID, AccessRights, InheritHandle, hom);
}

std::vector<AHandle> AProcessHandleManager::openProcessThreads(const ULONG &PID, const ULONG &AccessRights, const bool &InheritHandle, const HandleOpenMode &Mode)
{
    PSYSTEM_PROCESS_INFORMATION pInfo = spi.processInfo(PID);
    std::vector<AHandle> tHandles;

    for(ULONG i = 0; i < pInfo->NumberOfThreads; i++)
    {
        ULONG tid = (ULONG64)pInfo->Threads[i].ClientId.UniqueThread;
        int alreadyOpen = -1;

        if(Mode == CheckExisting)
        {
            alreadyOpen = isAlreadyOpen(tid, AccessRights, InheritHandle);
        }

        if(alreadyOpen == -1)
        {
            HANDLE tHandle = NULL;

            tHandle = OpenThread(AccessRights, InheritHandle, tid);

            if(tHandle == NULL)
            {
                spi.freeNTData();
                return std::vector<AHandle>();
            }

            AHandle aTHandle;

            aTHandle.handle = tHandle;
            aTHandle.pid = (ULONG64)pInfo->Threads[i].ClientId.UniqueProcess;
            aTHandle.tid = tid;
            aTHandle.accessRights = AccessRights;
            aTHandle.type = ThreadHandle;
            aTHandle.inherit = InheritHandle;

            if(Mode != DontAddToVector)
            {
                tHandles.push_back(aTHandle);
                hVec.push_back(aTHandle);
            }
        }
        else
        {
            tHandles.push_back(hVec[alreadyOpen]);
        }
    }

    return tHandles;
}

bool AProcessHandleManager::isAlreadyOpen(const AHandle &handle) const
{
    for(int i = 0; i < hVec.size(); i++)
    {
        if(hVec[i] == handle)
        {
            return true;
        }
    }

    return false;
}

int AProcessHandleManager::isAlreadyOpen(const ULONG &ID, const ULONG &AccessRights, const bool &InheritHandle) const
{
    for(int i = 0; i < hVec.size(); i++)
    {
        if(hVec[i].pid == ID || hVec[i].tid == ID && hVec[i].accessRights == AccessRights && hVec[i].inherit == InheritHandle)
        {
            return i;
        }
    }

    return -1;
}

bool AProcessHandleManager::isValid(const std::vector<AHandle> &Handles) const
{
    for(int i = 0; i < Handles.size(); i++)
    {
        if(Handles[i].handle == NULL)
        {
            return false;
        }
    }

    return true;
}

AHandle AProcessHandleManager::currentProcessPseudo() const
{
    AHandle handle;

    handle.handle = GetCurrentProcess();
    handle.pid = GetCurrentProcessId();
    handle.tid = GetCurrentThreadId();
    handle.accessRights = PROCESS_ALL_ACCESS;
    handle.type = ProcessHandle;
    handle.inherit = false;

    return handle;
}

AHandle AProcessHandleManager::currentProcessReal(const bool &InheritHandle)
{
    return openHandle(GetCurrentProcessId(), PROCESS_ALL_ACCESS, InheritHandle);
}

AHandle AProcessHandleManager::currentProcessReal(const bool &InheritHandle, const HandleOpenMode &Mode)
{
    return openHandle(GetCurrentProcessId(), PROCESS_ALL_ACCESS, InheritHandle, Mode);
}

AHandle AProcessHandleManager::currentThreadPseudo() const
{
    AHandle handle;

    handle.handle = GetCurrentThread();
    handle.pid = GetCurrentProcessId();
    handle.tid = GetCurrentThreadId();
    handle.accessRights = THREAD_ALL_ACCESS;
    handle.type = ThreadHandle;
    handle.inherit = false;

    return handle;
}

AHandle AProcessHandleManager::currentThreadReal(const bool &InheritHandle)
{
    return openHandle(GetCurrentThreadId(), THREAD_ALL_ACCESS, InheritHandle);
}

AHandle AProcessHandleManager::currentThreadReal(const bool &InheritHandle, const HandleOpenMode &Mode)
{
    return openHandle(GetCurrentThreadId(), THREAD_ALL_ACCESS, InheritHandle, Mode);
}

AHandle AProcessHandleManager::atPos(const int &vectorPosition) const
{
    if(vectorPosition <= hVec.size())
    {
        return hVec[vectorPosition];
    }

    return AHandle();
}

int AProcessHandleManager::closeAll()
{
    int closeCount = 0;

    for(int i = 0; i < hVec.size(); i++)
    {
        if(CloseHandle(hVec[i].handle))
        {
            closeCount++;
        }
    }

    hVec.clear();
    hVec.shrink_to_fit();

    return closeCount;
}

int AProcessHandleManager::close(const HandleType &Type)
{
    int closeCount = 0;

    if(Type == ProcessHandle)
    {
        for(int i = 0; i < hVec.size(); i++)
        {
            if(hVec[i].type == ProcessHandle)
            {
                if(CloseHandle(hVec[i].handle))
                {
                    hVec.erase(hVec.begin() + i);
                    closeCount++;
                }
            }
        }
    }
    if(Type == ThreadHandle)
    {
        for(int i = 0; i < hVec.size(); i++)
        {
            if(hVec[i].type == ThreadHandle)
            {
                if(CloseHandle(hVec[i].handle))
                {
                    hVec.erase(hVec.begin() + i);
                    closeCount++;
                }
            }
        }
    }

    return closeCount;
}

int AProcessHandleManager::close(const AHandle &Handle)
{
    int closeCount = 0;

    for(int i = 0; i < hVec.size(); i++)
    {
        if(hVec[i] == Handle)
        {
            if(CloseHandle(hVec[i].handle))
            {
                hVec.erase(hVec.begin() + i);
                closeCount++;
            }
        }
    }

    return closeCount;
}

int AProcessHandleManager::close(const std::vector<AHandle> &Handles)
{
    int closeCount = 0;

    for(int i = 0; i < Handles.size(); i++)
    {
        closeCount += close(Handles[i]);
    }

    return closeCount;
}

int AProcessHandleManager::totalHandleCount() const
{
    return hVec.size();
}

int AProcessHandleManager::handleCount(const HandleType &Type) const
{
    int count = 0;

    if(Type == ProcessHandle)
    {
        for(int i = 0; i < hVec.size(); i++)
        {
            if(hVec[i].type == ProcessHandle)
            {
                count++;
            }
        }
    }
    if(Type == ThreadHandle)
    {

        for(int i = 0; i < hVec.size(); i++)
        {
            if(hVec[i].type == ThreadHandle)
            {
                count++;
            }
        }
    }

    return count;
}

std::vector<AHandle> AProcessHandleManager::allHandles() const
{
    return hVec;
}

std::vector<AHandle> AProcessHandleManager::handles(const HandleType &Type) const
{
    std::vector<AHandle> handle;

    if(Type == ProcessHandle)
    {
        for(int i = 0; i < hVec.size(); i++)
        {
            if(hVec[i].type == ProcessHandle)
            {
                handle.push_back(hVec[i]);
            }
        }
    }
    if(Type == ThreadHandle)
    {
        for(int i = 0; i < hVec.size(); i++)
        {
            if(hVec[i].type == ThreadHandle)
            {
                handle.push_back(hVec[i]);
            }
        }
    }

    return handle;
}

void AProcessHandleManager::addToVector(const AHandle &handle)
{
    hVec.push_back(handle);
}

void AProcessHandleManager::addToVector(const std::vector<AHandle> &handle)
{
    for(int i = 0; i < handle.size(); i++)
    {
        hVec.push_back(handle[i]);
    }
}
