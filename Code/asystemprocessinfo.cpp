#include "asystemprocessinfo.h"

ASystemProcessInfo::ASystemProcessInfo()
{
}

std::vector<PSYSTEM_PROCESS_INFORMATION> ASystemProcessInfo::totalProcessInfo()
{
    PSYSTEM_PROCESS_INFORMATION spi = nt.ntData<SYSTEM_PROCESS_INFORMATION>(SystemProcessInformation);
    std::vector<PSYSTEM_PROCESS_INFORMATION> vec;

    if(spi != NULL)
    {
        while(1)
        {
            vec.push_back(spi);

            if(!spi->NextEntryOffset)
            {
                break;
            }

            spi = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)spi + spi->NextEntryOffset);
        }

        return vec;
    }

    return std::vector<PSYSTEM_PROCESS_INFORMATION>();
}

PSYSTEM_PROCESS_INFORMATION ASystemProcessInfo::processInfo(const ULONG &pid)
{
    PSYSTEM_PROCESS_INFORMATION spi = nt.ntData<SYSTEM_PROCESS_INFORMATION>(SystemProcessInformation);

    if(spi != NULL)
    {
        while(1)
        {
            if((ULONG64)spi->UniqueProcessId == pid)
            {
                return spi;
            }
            if(!spi->NextEntryOffset)
            {
                return NULL;
            }

            spi = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)spi + spi->NextEntryOffset);
        }
    }

    return NULL;
}

PSYSTEM_THREAD_INFORMATION ASystemProcessInfo::totalThreadInfo(const ULONG &pid, ULONG &threadCount)
{
    PSYSTEM_PROCESS_INFORMATION spi = processInfo(pid);

    if(spi != NULL)
    {
        threadCount = spi->NumberOfThreads;
        return spi->Threads;
    }

    return NULL;
}

PSYSTEM_THREAD_INFORMATION ASystemProcessInfo::threadInfo(const ULONG &pid, const ULONG &tid)
{
    ULONG tCount;
    PSYSTEM_THREAD_INFORMATION sti = totalThreadInfo(pid, tCount);

    if(sti != NULL)
    {
        for(ULONG i = 0; i < tCount; i++)
        {
            if((ULONG64)sti[i].ClientId.UniqueThread == tid)
            {
                return &sti[i];
            }
        }
    }

    return NULL;
}

std::vector<ULONG> ASystemProcessInfo::pidList() const
{
    std::vector<ULONG> pList;

    ULONG aProcesses[4096], cbNeeded, cProcesses;

    if(!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return pList;
    }

    cProcesses = cbNeeded / sizeof(ULONG);
    pList.reserve(cProcesses);

    for(ULONG i = 0; i < cProcesses; i++)
    {
        pList.push_back(aProcesses[i]);
    }

    return pList;
}

int ASystemProcessInfo::processCount() const
{
    ULONG aProcesses[4096], cbNeeded;

    if(!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
    {
        return -1;
    }

    return cbNeeded / sizeof(ULONG);
}

std::wstring ASystemProcessInfo::uniStringToWString(const UNICODE_STRING &string) const
{
    return std::wstring(string.Buffer, string.Length / sizeof(WCHAR));
}

void ASystemProcessInfo::freeNTData()
{
    nt.freeNTData();
}

double ASystemProcessInfo::processCPULoad(const ULONG &pid)
{
    PSYSTEM_PROCESS_INFORMATION spi = processInfo(pid);

    if(spi == NULL)
    {
        return -1;
    }

    double ret = pLoad.calculateCPULoad(spi->KernelTime.QuadPart, spi->UserTime.QuadPart);

    nt.freeNTData();
    return ret;
}

double ASystemProcessInfo::processCPULoad(const ULONG &pid, PSYSTEM_PROCESS_INFORMATION &pInfo)
{
    pInfo = processInfo(pid);

    if(pInfo == NULL)
    {
        return -1;
    }

    return pLoad.calculateCPULoad(pInfo->KernelTime.QuadPart, pInfo->UserTime.QuadPart);
}

double ASystemProcessInfo::threadCPULoad(const ULONG &pid, const ULONG &tid)
{
    PSYSTEM_THREAD_INFORMATION sti = threadInfo(pid, tid);

    if(sti == NULL)
    {
        return -1;
    }

    double ret = tLoad.calculateCPULoad(sti->KernelTime.QuadPart, sti->UserTime.QuadPart);

    nt.freeNTData();
    return ret;
}

double ASystemProcessInfo::threadCPULoad(const ULONG &pid, const ULONG &tid, PSYSTEM_THREAD_INFORMATION &tInfo)
{
    tInfo = threadInfo(pid, tid);

    if(tInfo == NULL)
    {
        return -1;
    }

    return tLoad.calculateCPULoad(tInfo->KernelTime.QuadPart, tInfo->UserTime.QuadPart);
}

std::vector<double> ASystemProcessInfo::processCPULoad()
{
    std::vector<PSYSTEM_PROCESS_INFORMATION> spi = totalProcessInfo();
    std::vector<double> ret;

    pLoadVec.resize(spi.size());

    for(int i = 0; i < spi.size(); i++)
    {
        ret.push_back(pLoadVec[i].calculateCPULoad(spi[i]->KernelTime.QuadPart, spi[i]->UserTime.QuadPart));
    }

    nt.freeNTData();
    return ret;
}

std::vector<double> ASystemProcessInfo::processCPULoad(std::vector<PSYSTEM_PROCESS_INFORMATION> &pInfo)
{
    pInfo = totalProcessInfo();
    std::vector<double> ret;

    pLoadVec.resize(pInfo.size());

    for(int i = 0; i < pInfo.size(); i++)
    {
        ret.push_back(pLoadVec[i].calculateCPULoad(pInfo[i]->KernelTime.QuadPart, pInfo[i]->UserTime.QuadPart));
    }

    return ret;
}

std::vector<double> ASystemProcessInfo::threadCPULoad(const ULONG &pid)
{
    ULONG tCount = 0;
    PSYSTEM_THREAD_INFORMATION sti = totalThreadInfo(pid, tCount);
    std::vector<double> ret;

    tLoadVec.resize(tCount);

    for(int i = 0; i < tCount; i++)
    {
        ret.push_back(tLoadVec[i].calculateCPULoad(sti[i].KernelTime.QuadPart, sti[i].UserTime.QuadPart));
    }

    nt.freeNTData();
    return ret;
}

std::vector<double> ASystemProcessInfo::threadCPULoad(const ULONG &pid, PSYSTEM_THREAD_INFORMATION &tInfo)
{
    ULONG tCount = 0;
    tInfo = totalThreadInfo(pid, tCount);
    std::vector<double> ret;

    tLoadVec.resize(tCount);

    for(int i = 0; i < tCount; i++)
    {
        ret.push_back(tLoadVec[i].calculateCPULoad(tInfo[i].KernelTime.QuadPart, tInfo[i].UserTime.QuadPart));
    }

    return ret;
}

sysProcessThreadCPULoad::sysProcessThreadCPULoad()
{
    cpuCount = GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
}

ULONG64 sysProcessThreadCPULoad::fileTimeToULONG64(const FILETIME &ft) const
{
    ULARGE_INTEGER li;

    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    return li.QuadPart;
}

double sysProcessThreadCPULoad::calculateCPULoad(const ULONG64 &kernelTime, const ULONG64 &userTime)
{
    FILETIME ftime;
    ULONG64 sysTime;
    double percent;

    GetSystemTimeAsFileTime(&ftime);
    sysTime = fileTimeToULONG64(ftime);

    percent = (kernelTime - lastKernelTime) + (userTime - lastUserTime);
    percent /= (sysTime - lastSysTime);
    percent /= cpuCount;
    lastSysTime = sysTime;
    lastUserTime = userTime;
    lastKernelTime = kernelTime;

    percent *= 100;

    if(percent < 0)
    {
        return 0;
    }
    if(percent > 100)
    {
        return 100;
    }

    return percent;
}
