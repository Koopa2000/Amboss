#include "aprocesscontrol.h"

AProcessControl::AProcessControl()
{
    ntSuspendProcess = (_NTSuspendProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSuspendProcess");
    ntResumeProcess = (_NTResumeProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtResumeProcess");
}

AProcessControl::~AProcessControl()
{
    ntSuspendProcess = NULL;
    ntResumeProcess = NULL;
}

bool AProcessControl::setAffinityMask(const AHandle &handle, const BitMask &Mask) const
{
    if(handle.isProcess())
    {
        return SetProcessAffinityMask(handle.handle, Mask.Mask);
    }
    if(handle.isThread())
    {
        if(SetThreadAffinityMask(handle.handle, Mask.Mask) != 0)
        {
            return true;
        }
    }

    return false;
}

bool AProcessControl::setAffinityMaskEx(const AHandle &handle, const BitMaskEx &Mask) const
{
    if(handle.isProcess())
    {
        GROUP_AFFINITY gAffinity;
        ZeroMemory(&gAffinity, sizeof(gAffinity));

        gAffinity.Group = Mask.Group;
        gAffinity.Mask = Mask.Mask.Mask;

        if(NT_SUCCESS(kernel.ntSetProcessInformation(handle.handle, ProcessAffinityMask, &gAffinity, sizeof(GROUP_AFFINITY))))
        {
            return true;
        }
    }
    if(handle.isThread())
    {
        GROUP_AFFINITY gAffinity;
        ZeroMemory(&gAffinity, sizeof(gAffinity));

        gAffinity.Group = Mask.Group;
        gAffinity.Mask = Mask.Mask.Mask;

        if(SetThreadGroupAffinity(handle.handle, &gAffinity, NULL))
        {
            return true;
        }
    }

    return false;
}

int AProcessControl::setAffinityMasks(const std::vector<AHandle> &handles, const std::vector<BitMask> &Masks) const
{
    if(handles.size() > Masks.size())
    {
        return 0;
    }

    int successCount = 0;

    for(int i = 0; i < handles.size(); i++)
    {
        if(setAffinityMask(handles[i], Masks[i]))
        {
            successCount++;
        }
    }

    return successCount;
}

int AProcessControl::setAffinityMasksEx(const std::vector<AHandle> &handles, const std::vector<BitMaskEx> &Masks) const
{
    if(handles.size() > Masks.size())
    {
        return 0;
    }

    int successCount = 0;

    for(int i = 0; i < handles.size(); i++)
    {
        if(setAffinityMaskEx(handles[i], Masks[i]))
        {
            successCount++;
        }
    }

    return successCount;
}

bool AProcessControl::affinityMask(const AHandle &handle, BitMask &Mask) const
{
    if(handle.isProcess())
    {
        ULONG64 pAffinity, sAffinity;

        if(GetProcessAffinityMask(handle.handle, &pAffinity, &sAffinity))
        {
            Mask.setMask(pAffinity);
            return true;
        }
    }
    if(handle.isThread())
    {
        THREAD_BASIC_INFORMATION tInfo;

        if(NT_SUCCESS(kernel.ntQueryThreadInformation(handle.handle, ThreadBasicInformation, &tInfo, sizeof(tInfo), 0)))
        {
            Mask.setMask(tInfo.AffinityMask);
            return true;
        }
    }

    return false;
}

bool AProcessControl::affinityMaskEx(const AHandle &handle, BitMaskEx &Mask) const
{
    if(handle.isProcess())
    {
        GROUP_AFFINITY gAffinity;

        if(NT_SUCCESS(kernel.ntQueryProcessInformation(handle.handle, ProcessAffinityMask, &gAffinity, sizeof(GROUP_AFFINITY), 0)))
        {
            Mask.Group = gAffinity.Group;
            Mask.Mask.setMask(gAffinity.Mask);

            return true;
        }
    }
    if(handle.isThread())
    {
        GROUP_AFFINITY gAffinity;

        if(NT_SUCCESS(kernel.ntQueryThreadInformation(handle.handle, ThreadGroupInformation, &gAffinity, sizeof(GROUP_AFFINITY), 0)))
        {
            Mask.Group = gAffinity.Group;
            Mask.Mask.setMask(gAffinity.Mask);

            return true;
        }
    }

    return false;
}

bool AProcessControl::setPriority(const AHandle &handle, const ULONG &ProcessPriority, const int &ThreadPriority) const
{
    if(handle.isProcess())
    {
        return SetPriorityClass(handle.handle, ProcessPriority);
    }
    if(handle.isThread())
    {
        return SetThreadPriority(handle.handle, ThreadPriority);
    }

    return false;
}

bool AProcessControl::priority(const AHandle &handle, ULONG &ProcessPriority, int &ThreadPriority) const
{
    if(handle.isProcess())
    {
        ProcessPriority = GetPriorityClass(handle.handle);
        ThreadPriority = 0;

        if(ProcessPriority == 0)
        {
            return false;
        }

        return true;
    }
    if(handle.isThread())
    {
         ThreadPriority = GetThreadPriority(handle.handle);
         ProcessPriority = 0;

         if(ThreadPriority == THREAD_PRIORITY_ERROR_RETURN)
         {
             return false;
         }

         return true;
    }

    return false;
}

bool AProcessControl::setIOPriority(const AHandle &handle, IO_PRIORITY_HINT Priority) const
{
    if(handle.isProcess())
    {
        if(NT_SUCCESS(kernel.ntSetProcessInformation(handle.handle, ProcessIoPriority, &Priority, sizeof(IO_PRIORITY_HINT))))
        {
            return true;
        }
    }
    if(handle.isThread())
    {
        if(NT_SUCCESS(kernel.ntSetThreadInformation(handle.handle, ThreadIoPriority, &Priority, sizeof(IO_PRIORITY_HINT))))
        {
            return true;
        }
    }

    return false;
}

bool AProcessControl::ioPriority(const AHandle &handle, IO_PRIORITY_HINT &Priority) const
{
    if(handle.isProcess())
    {
        if(kernel.ntQueryProcessInformation(handle.handle, ProcessIoPriority, &Priority, sizeof(IO_PRIORITY_HINT), 0))
        {
            return true;
        }
    }
    if(handle.isThread())
    {
        if(kernel.ntQueryThreadInformation(handle.handle, ThreadIoPriority, &Priority, sizeof(IO_PRIORITY_HINT), 0))
        {
            return true;
        }
    }

    return false;
}

bool AProcessControl::setMemoryPriority(const AHandle &handle, const ULONG &Priority) const
{
    if(handle.isProcess())
    {
        MEMORY_PRIORITY_INFORMATION MemPrio;

        ZeroMemory(&MemPrio, sizeof(MemPrio));
        MemPrio.MemoryPriority = Priority;

        return SetProcessInformation(handle.handle, ProcessMemoryPriority, &MemPrio, sizeof(MemPrio));
    }
    if(handle.isThread())
    {
        MEMORY_PRIORITY_INFORMATION MemPrio;

        ZeroMemory(&MemPrio, sizeof(MemPrio));
        MemPrio.MemoryPriority = Priority;

        return SetThreadInformation(handle.handle, ThreadMemoryPriority, &MemPrio, sizeof(MemPrio));
    }

    return false;
}

bool AProcessControl::memoryPriority(const AHandle &handle, ULONG &Priority) const
{
    if(handle.isProcess())
    {
        MEMORY_PRIORITY_INFORMATION MemPrio;
        ZeroMemory(&MemPrio, sizeof(MemPrio));

        if(GetProcessInformation(handle.handle, ProcessMemoryPriority, &MemPrio, sizeof(MemPrio)))
        {
            Priority = MemPrio.MemoryPriority;
            return true;
        }
    }
    if(handle.isThread())
    {
        MEMORY_PRIORITY_INFORMATION MemPrio;
        ZeroMemory(&MemPrio, sizeof(MemPrio));

        if(GetThreadInformation(handle.handle, ThreadMemoryPriority, &MemPrio, sizeof(MemPrio)))
        {
            Priority = MemPrio.MemoryPriority;
            return true;
        }
    }

    return 0;
}

bool AProcessControl::setEcoQoSMode(const AHandle &handle, const bool &ModeOnOff) const
{
    if(handle.isProcess())
    {
        PROCESS_POWER_THROTTLING_STATE PowerThrottling;
        ZeroMemory(&PowerThrottling, sizeof(PowerThrottling));

        if(ModeOnOff)
        {
            PowerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
            PowerThrottling.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
            PowerThrottling.StateMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;

            return SetProcessInformation(handle.handle, ProcessPowerThrottling, &PowerThrottling, sizeof(PowerThrottling));
        }
        else
        {
            PowerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;
            PowerThrottling.ControlMask = PROCESS_POWER_THROTTLING_EXECUTION_SPEED;
            PowerThrottling.StateMask = 0;

            return SetProcessInformation(handle.handle, ProcessPowerThrottling, &PowerThrottling, sizeof(PowerThrottling));
        }
    }
    if(handle.isThread())
    {
        THREAD_POWER_THROTTLING_STATE  PowerThrottling;
        ZeroMemory(&PowerThrottling, sizeof(PowerThrottling));

        if(ModeOnOff)
        {
            PowerThrottling.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;
            PowerThrottling.ControlMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;
            PowerThrottling.StateMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;

            return SetThreadInformation(handle.handle, ThreadPowerThrottling, &PowerThrottling, sizeof(PowerThrottling));
        }
        else
        {
            PowerThrottling.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;
            PowerThrottling.ControlMask = THREAD_POWER_THROTTLING_EXECUTION_SPEED;
            PowerThrottling.StateMask = 0;

            return SetThreadInformation(handle.handle, ThreadPowerThrottling, &PowerThrottling, sizeof(PowerThrottling));
        }
    }

    return false;
}

bool AProcessControl::ecoQoSMode(const AHandle &handle, bool &ModeOnOff) const
{
    if(handle.isProcess())
    {
        PROCESS_POWER_THROTTLING_STATE PowerThrottling;
        ZeroMemory(&PowerThrottling, sizeof(PowerThrottling));

        PowerThrottling.Version = PROCESS_POWER_THROTTLING_CURRENT_VERSION;

        if(GetProcessInformation(handle.handle, ProcessPowerThrottling, &PowerThrottling, sizeof(PowerThrottling)))
        {
            if(PowerThrottling.ControlMask == PROCESS_POWER_THROTTLING_EXECUTION_SPEED && PowerThrottling.StateMask == PROCESS_POWER_THROTTLING_EXECUTION_SPEED)
            {
                ModeOnOff = true;
            }
            else
            {
                ModeOnOff = false;
            }

            return true;
        }
    }
    if(handle.isThread())
    {
        THREAD_POWER_THROTTLING_STATE PowerThrottling;
        ZeroMemory(&PowerThrottling, sizeof(PowerThrottling));

        PowerThrottling.Version = THREAD_POWER_THROTTLING_CURRENT_VERSION;

        if(GetThreadInformation(handle.handle, ThreadPowerThrottling, &PowerThrottling, sizeof(PowerThrottling)))
        {
            if(PowerThrottling.ControlMask == THREAD_POWER_THROTTLING_EXECUTION_SPEED && PowerThrottling.StateMask == THREAD_POWER_THROTTLING_EXECUTION_SPEED)
            {
                ModeOnOff = true;
            }
            else
            {
                ModeOnOff = false;
            }

            return true;
        }
    }

    return false;
}

bool AProcessControl::setThreadIdealProcessor(const AHandle &threadHandle, const BitMask &Mask, const int &BitMaskVectorPosition) const
{
    if(threadHandle.isThread())
    {
        return SetThreadIdealProcessor(threadHandle.handle, Mask.cores()[BitMaskVectorPosition]);
    }

    return false;
}

bool AProcessControl::setThreadIdealProcessor(const AHandle &threadHandle, const BitMask &Mask) const
{
    return setThreadIdealProcessor(threadHandle, Mask, 0);
}

bool AProcessControl::setThreadIdealProcessorEx(const AHandle &threadHandle, const BitMaskEx &Mask, const int &BitMaskVectorPosition) const
{
    if(threadHandle.isThread())
    {
        PROCESSOR_NUMBER pNum;

        pNum.Group = Mask.Group;
        pNum.Number = Mask.Mask.cores()[BitMaskVectorPosition];

        return SetThreadIdealProcessorEx(threadHandle.handle, &pNum, NULL);
    }

    return false;
}

bool AProcessControl::setThreadIdealProcessorEx(const AHandle &threadHandle, const BitMaskEx &Mask) const
{
    return setThreadIdealProcessorEx(threadHandle, Mask, 0);
}

bool AProcessControl::threadIdealProcessor(const AHandle &threadHandle, BitMask &Mask) const
{
    if(threadHandle.isThread())
    {
        PROCESSOR_NUMBER pNum;

        if(GetThreadIdealProcessorEx(threadHandle.handle, &pNum))
        {
            Mask.setMask(static_cast<ULONG64>(1) << pNum.Number);
            return true;
        }
    }

    return false;
}

bool AProcessControl::threadIdealProcessorEx(const AHandle &threadHandle, BitMaskEx &Mask) const
{
    if(threadHandle.isThread())
    {
        PROCESSOR_NUMBER pNum;

        if(GetThreadIdealProcessorEx(threadHandle.handle, &pNum))
        {
            Mask.Mask.setMask(static_cast<ULONG64>(1) << pNum.Number);
            Mask.Group = pNum.Group;

            return true;
        }
    }

    return false;
}

bool AProcessControl::suspend(const AHandle &handle, ULONG &suspendCount) const
{
    if(handle.isProcess())
    {
        if(NT_SUCCESS(ntSuspendProcess(handle.handle)))
        {
            suspendCount = 0;
            return true;
        }
    }
    if(handle.isThread())
    {
        ULONG sCount = SuspendThread(handle.handle);

        if(sCount != (ULONG) - 1)
        {
            suspendCount = sCount + 1;
            return true;
        }
    }

    return false;
}

bool AProcessControl::resume(const AHandle &handle, ULONG &suspendCount) const
{
    if(handle.isProcess())
    {
        if(NT_SUCCESS(ntResumeProcess(handle.handle)))
        {
            suspendCount = 0;
            return true;
        }
    }
    if(handle.isThread())
    {
        ULONG sCount = ResumeThread(handle.handle);

        if(sCount != (ULONG) - 1)
        {
            suspendCount = sCount - 1;
            return true;
        }
    }

    return false;
}

bool AProcessControl::times(const AHandle &handle, FILETIME &CreationTime, FILETIME &ExitTime, FILETIME &KernelTime, FILETIME &UserTime) const
{
    if(handle.isProcess())
    {
        return GetProcessTimes(handle.handle, &CreationTime, &ExitTime, &KernelTime, &UserTime);
    }
    if(handle.isThread())
    {
        return GetThreadTimes(handle.handle, &CreationTime, &ExitTime, &KernelTime, &UserTime);
    }

    return false;
}

bool AProcessControl::times(const AHandle &handle, SYSTEMTIME &CreationTime, SYSTEMTIME &ExitTime,
                            SYSTEMTIME &KernelTime, SYSTEMTIME &UserTime, const TimeCode &tCode) const
{
    FILETIME cTime, eTime, kTime, uTime;

    if(handle.isProcess())
    {
        if(!GetProcessTimes(handle.handle, &cTime, &eTime, &kTime, &uTime))
        {
            return false;
        }
    }
    if(handle.isThread())
    {
        if(!GetThreadTimes(handle.handle, &cTime, &eTime, &kTime, &uTime))
        {
            return false;
        }
    }

    if(tCode == UTC)
    {
        if(!FileTimeToSystemTime(&cTime, &CreationTime)){return false;}
        if(!FileTimeToSystemTime(&eTime, &ExitTime)){return false;}
        if(!FileTimeToSystemTime(&kTime, &KernelTime)){return false;}
        if(!FileTimeToSystemTime(&uTime, &UserTime)){return false;}

        return true;
    }
    if(tCode == LocalTime)
    {
        FILETIME _cTime, _eTime, _kTime, _uTime;

        if(!FileTimeToLocalFileTime(&cTime, &_cTime)){return false;}
        if(!FileTimeToLocalFileTime(&eTime, &_eTime)){return false;}
        if(!FileTimeToLocalFileTime(&kTime, &_kTime)){return false;}
        if(!FileTimeToLocalFileTime(&uTime, &_uTime)){return false;}

        if(!FileTimeToSystemTime(&_cTime, &CreationTime)){return false;}
        if(!FileTimeToSystemTime(&_eTime, &ExitTime)){return false;}
        if(!FileTimeToSystemTime(&_kTime, &KernelTime)){return false;}
        if(!FileTimeToSystemTime(&_uTime, &UserTime)){return false;}

        return true;
    }

    return false;
}

bool AProcessControl::processMemoryInfo(const AHandle &processHandle, PROCESS_MEMORY_COUNTERS &pmc) const
{
    if(processHandle.isProcess())
    {
        return GetProcessMemoryInfo(processHandle.handle, &pmc, sizeof(pmc));
    }

    return false;
}

bool AProcessControl::setProcessWorkingSetSize(const AHandle &processHandle, const ULONG64 &MinSize, const ULONG &MaxSize) const
{
    if(processHandle.isProcess())
    {
        return SetProcessWorkingSetSize(processHandle.handle, MinSize, MaxSize);
    }

    return false;
}

bool AProcessControl::setProcessWorkingSetSize(const AHandle &processHandle, const ULONG64 &MinSize, const ULONG &MaxSize, const ULONG &Flags) const
{
    if(processHandle.isProcess())
    {
        return SetProcessWorkingSetSizeEx(processHandle.handle, MinSize, MaxSize, Flags);
    }

    return false;
}

bool AProcessControl::processWorkingSetSize(const AHandle &processHandle, ULONG64 &MinSize, ULONG64 &MaxSize) const
{
    if(processHandle.isProcess())
    {
        return GetProcessWorkingSetSize(processHandle.handle, &MinSize, &MaxSize);
    }

    return false;
}

bool AProcessControl::processWorkingSetSize(const AHandle &processHandle, ULONG64 &MinSize, ULONG64 &MaxSize, ULONG &Flags) const
{
    if(processHandle.isProcess())
    {
        return GetProcessWorkingSetSizeEx(processHandle.handle, &MinSize, &MaxSize, &Flags);
    }

    return false;
}

bool AProcessControl::cycleTime(const AHandle &handle, ULONG64 &cycles) const
{
    if(handle.isProcess())
    {
        return QueryProcessCycleTime(handle.handle, &cycles);
    }
    if(handle.isThread())
    {
        return QueryThreadCycleTime(handle.handle, &cycles);
    }

    return false;
}

bool AProcessControl::processHandleCount(const AHandle &processHandle, ULONG &handleCount) const
{
    if(processHandle.isProcess())
    {
        return GetProcessHandleCount(processHandle.handle, &handleCount);
    }

    return false;
}

bool AProcessControl::processIOCounters(const AHandle &processHandle, IO_COUNTERS &counters) const
{
    if(processHandle.isProcess())
    {
        return GetProcessIoCounters(processHandle.handle, &counters);
    }

    return false;
}

bool AProcessControl::terminate(const AHandle &handle, const ULONG &exitCode) const
{
    if(handle.isProcess())
    {
        return TerminateProcess(handle.handle, exitCode);
    }
    if(handle.isThread())
    {
        return TerminateThread(handle.handle, exitCode);
    }

    return false;
}

bool AProcessControl::exitCode(const AHandle &handle, ULONG &exitCode) const
{
    if(handle.isProcess())
    {
        return GetExitCodeProcess(handle.handle, &exitCode);
    }
    if(handle.isThread())
    {
        return GetExitCodeThread(handle.handle, &exitCode);
    }

    return false;
}

bool AProcessControl::processThreadID(const AHandle &handle, ULONG &ID) const
{
    if(handle.isProcess())
    {
        ID = GetProcessId(handle.handle);

        if(ID != 0)
        {
            return true;
        }
    }
    if(handle.isThread())
    {
        ID = GetThreadId(handle.handle);

        if(ID != 0)
        {
            return true;
        }
    }

    return false;
}

bool AProcessControl::processIdFromThread(const AHandle &threadHandle, ULONG &ID) const
{
    if(threadHandle.isThread())
    {
        ID = GetProcessIdOfThread(threadHandle.handle);

        if(ID != 0)
        {
            return true;
        }
    }

    return false;
}

ULONG64 AProcessThreadCPULoad::fileTimeToULONG64(const FILETIME &ft) const
{
    ULARGE_INTEGER li;

    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    return li.QuadPart;
}

bool AProcessThreadCPULoad::initCalculateCPULoad(const AHandle &handle)
{
    FILETIME ftime, fsys, fuser;

    numProcessors = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);

    GetSystemTimeAsFileTime(&ftime);
    lastSysTime = fileTimeToULONG64(ftime);

    if(handle.isProcess())
    {
        if(!GetProcessTimes(handle.handle, &ftime, &ftime, &fsys, &fuser))
        {
            return false;
        }
    }
    if(handle.isThread())
    {
        if(!GetThreadTimes(handle.handle, &ftime, &ftime, &fsys, &fuser))
        {
            return false;
        }
    }

    ahandle = handle;
    lastKTime = fileTimeToULONG64(fsys);
    lastUTime = fileTimeToULONG64(fuser);

    return true;
}

double AProcessThreadCPULoad::calculateCPULoad()
{
    FILETIME ftime, fKTime, fUTime;
    ULONG64 sysTime, kTime, uTime;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    sysTime = fileTimeToULONG64(ftime);

    if(ahandle.isProcess())
    {
        if(!GetProcessTimes(ahandle.handle, &ftime, &ftime, &fKTime, &fUTime))
        {
            return -1;
        }
    }
    if(ahandle.isThread())
    {
        if(!GetThreadTimes(ahandle.handle, &ftime, &ftime, &fKTime, &fUTime))
        {
            return -1;
        }
    }

    kTime = fileTimeToULONG64(fKTime);
    uTime = fileTimeToULONG64(fUTime);

    percent = (kTime - lastKTime) + (uTime - lastUTime);
    percent /= (sysTime - lastSysTime);
    percent /= numProcessors;

    lastSysTime = sysTime;
    lastUTime = uTime;
    lastKTime = kTime;

    double ret = percent * 100;

    if(ret < 0)
    {
        return 0;
    }
    if(ret > 100)
    {
        return 100;
    }

    return ret;
}
