#ifndef APROCESSCONTROL_H
#define APROCESSCONTROL_H

#include "AHandleManager.h"
#include "AProcessorTopology.h"

class AProcessControl
{
public:
    AProcessControl();
    ~AProcessControl();

    enum TimeCode
    {
        UTC,
        LocalTime
    };

    bool setAffinityMask(const AHandle &handle, const BitMask &Mask) const;
    bool setAffinityMaskEx(const AHandle &handle, const BitMaskEx &Mask) const;
    int setAffinityMasks(const std::vector<AHandle> &handles, const std::vector<BitMask> &Masks) const;
    int setAffinityMasksEx(const std::vector<AHandle> &handles, const std::vector<BitMaskEx> &Masks) const;

    bool affinityMask(const AHandle &handle, BitMask &Mask) const;
    bool affinityMaskEx(const AHandle &handle, BitMaskEx &Mask) const;

    bool setPriority(const AHandle &handle, const ULONG &ProcessPriority, const int &ThreadPriority) const;
    bool priority(const AHandle &handle, ULONG &ProcessPriority, int &ThreadPriority) const;

    bool setIOPriority(const AHandle &handle, IO_PRIORITY_HINT Priority) const;
    bool ioPriority(const AHandle &handle, IO_PRIORITY_HINT &Priority) const;

    bool setMemoryPriority(const AHandle &handle, const ULONG &Priority) const;
    bool memoryPriority(const AHandle &handle, ULONG &Priority) const;

    bool setEcoQoSMode(const AHandle &handle, const bool &ModeOnOff) const;
    bool ecoQoSMode(const AHandle &handle, bool &ModeOnOff) const;

    bool setThreadIdealProcessor(const AHandle &threadHandle, const BitMask &Mask, const int &BitMaskVectorPosition) const;
    bool setThreadIdealProcessor(const AHandle &threadHandle, const BitMask &Mask) const;
    bool setThreadIdealProcessorEx(const AHandle &threadHandle, const BitMaskEx &Mask, const int &BitMaskVectorPosition) const;
    bool setThreadIdealProcessorEx(const AHandle &threadHandle, const BitMaskEx &Mask) const;

    bool threadIdealProcessor(const AHandle &threadHandle, BitMask &Mask) const;
    bool threadIdealProcessorEx(const AHandle &threadHandle, BitMaskEx &Mask) const;

    bool suspend(const AHandle &handle, ULONG &suspendCount) const;
    bool resume(const AHandle &handle, ULONG &suspendCount) const;

    bool times(const AHandle &handle, FILETIME &CreationTime, FILETIME &ExitTime, FILETIME &KernelTime, FILETIME &UserTime) const;
    bool times(const AHandle &handle, SYSTEMTIME &CreationTime, SYSTEMTIME &ExitTime, SYSTEMTIME &KernelTime, SYSTEMTIME &UserTime, const TimeCode &tCode) const;

    bool processMemoryInfo(const AHandle &processHandle, PROCESS_MEMORY_COUNTERS &pmc) const;

    bool setProcessWorkingSetSize(const AHandle &processHandle, const ULONG64 &MinSize, const ULONG &MaxSize) const;
    bool setProcessWorkingSetSize(const AHandle &processHandle, const ULONG64 &MinSize, const ULONG &MaxSize, const ULONG &Flags) const;
    bool processWorkingSetSize(const AHandle &processHandle, ULONG64 &MinSize, ULONG64 &MaxSize) const;
    bool processWorkingSetSize(const AHandle &processHandle, ULONG64 &MinSize, ULONG64 &MaxSize, ULONG &Flags) const;

    bool cycleTime(const AHandle &handle, ULONG64 &cycles) const;

    bool processHandleCount(const AHandle &processHandle, ULONG &handleCount) const;

    bool processIOCounters(const AHandle &processHandle, IO_COUNTERS &counters) const;

    bool terminate(const AHandle &handle, const ULONG &exitCode) const;
    bool exitCode(const AHandle &handle, ULONG &exitCode) const;

    bool processThreadID(const AHandle &handle, ULONG &ID) const;
    bool processIdFromThread(const AHandle &threadHandle, ULONG &ID) const;

private:
    ANTKernel kernel;

    typedef NTSTATUS (__stdcall *_NTSuspendProcess)(HANDLE);
    typedef NTSTATUS (__stdcall *_NTResumeProcess)(HANDLE);

    _NTSuspendProcess ntSuspendProcess = NULL;
    _NTResumeProcess ntResumeProcess = NULL;
};

class AProcessThreadCPULoad
{
public:
    bool initCalculateCPULoad(const AHandle &handle);
    double calculateCPULoad();

private:
    ULONG64 fileTimeToULONG64(const FILETIME &ft) const;

    ULONG64 lastSysTime, lastKTime, lastUTime;
    int numProcessors;
    AHandle ahandle;
};

#endif // APROCESSCONTROL_H
