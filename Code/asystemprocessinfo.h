#ifndef ASYSTEMPROCESSINFO_H
#define ASYSTEMPROCESSINFO_H

#include "ANtKernel.h"
#include<psapi.h>

class sysProcessThreadCPULoad
{
public:
    sysProcessThreadCPULoad();
    double calculateCPULoad(const ULONG64 &kernelTime, const ULONG64 &userTime);

private:
    ULONG64 fileTimeToULONG64(const FILETIME &ft) const;

    ULONG cpuCount;
    ULONG64 lastSysTime, lastKernelTime, lastUserTime;
};

class ASystemProcessInfo
{
public:
    ASystemProcessInfo();

    std::vector<PSYSTEM_PROCESS_INFORMATION> totalProcessInfo();
    PSYSTEM_THREAD_INFORMATION totalThreadInfo(const ULONG &pid, ULONG &threadCount);

    PSYSTEM_PROCESS_INFORMATION processInfo(const ULONG &pid);
    PSYSTEM_THREAD_INFORMATION threadInfo(const ULONG &pid, const ULONG &tid);

    std::vector<ULONG> pidList() const;
    int processCount() const;

    std::wstring uniStringToWString(const UNICODE_STRING &string) const;

    void freeNTData();

    double processCPULoad(const ULONG &pid);
    double processCPULoad(const ULONG &pid, PSYSTEM_PROCESS_INFORMATION &pInfo);

    double threadCPULoad(const ULONG &pid, const ULONG &tid);
    double threadCPULoad(const ULONG &pid, const ULONG &tid, PSYSTEM_THREAD_INFORMATION &tInfo);

    std::vector<double>processCPULoad();
    std::vector<double>processCPULoad(std::vector<PSYSTEM_PROCESS_INFORMATION> &pInfo);

    std::vector<double>threadCPULoad(const ULONG &pid);
    std::vector<double>threadCPULoad(const ULONG &pid, PSYSTEM_THREAD_INFORMATION &tInfo);

private:
    ANTKernel nt;

    sysProcessThreadCPULoad pLoad, tLoad;
    std::vector<sysProcessThreadCPULoad> pLoadVec, tLoadVec;
};

#endif // ASYSTEMPROCESSINFO_H
