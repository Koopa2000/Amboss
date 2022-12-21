#include "aprocessortopology.h"

AProcessorTopology::AProcessorTopology()
{
    tCoreCount = totalCoreCount();
    sLoadVec.resize(tCoreCount);
}

USHORT AProcessorTopology::socketCount() const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationProcessorPackage, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationProcessorPackage, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            USHORT count = 0;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationProcessorPackage)
                {
                    count++;
                }
            }

            delete [] dData;
            return count;
        }

        delete [] pData;
    }

    return 0;
}

USHORT AProcessorTopology::numaNodeCount() const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationNumaNode, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationNumaNode, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            USHORT count = 0;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationNumaNode)
                {
                    count++;
                }
            }

            delete [] dData;
            return count;
        }

        delete [] pData;
    }

    return 0;
}

int AProcessorTopology::coreCountPerSocket(const CoreType &Type) const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            int count = 0;
            short socket = 0;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationProcessorPackage)
                {
                    socket++;
                }

                if(socket == 1)
                {
                    if(pData->Relationship == RelationProcessorCore)
                    {
                        if(Type == Logical)
                        {
                            if(pData->Processor.Flags == LTP_PC_SMT)
                            {
                                count++;
                            }
                        }
                        count++;
                    }
                }
            }

            delete [] dData;
            return count;
        }

        delete [] pData;
    }

    return -1;
}

short AProcessorTopology::groupCountPerSocket() const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            int count = 0;
            short socket = 0;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationProcessorPackage)
                {
                    socket++;
                }
                if(socket == 1)
                {
                    if(pData->Relationship == RelationGroup)
                    {
                        count++;
                    }
                }
            }

            delete [] dData;
            return count;
        }

        delete [] pData;
    }

    return -1;
}

int AProcessorTopology::totalCoreCount(const CoreType &Type) const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationProcessorCore, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationProcessorCore, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            int count = 0;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationProcessorCore)
                {
                    if(Type == Logical)
                    {
                        if(pData->Processor.Flags == LTP_PC_SMT)
                        {
                            count++;
                        }
                    }
                    count++;
                }
            }

            delete [] dData;
            return count;
        }

        delete [] pData;
    }

    return -1;
}

int AProcessorTopology::coreEfficiency(const int &Core) const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationProcessorCore, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationProcessorCore, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            int count = 0;
            int eff = -1;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationProcessorCore)
                {
                    if(count == Core)
                    {
                        eff = pData->Processor.EfficiencyClass;
                    }

                    count++;
                }
            }

            delete [] dData;
            return eff;
        }

        delete [] pData;
    }

    return -1;
}

std::vector<CacheInfo> AProcessorTopology::cacheInfo() const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationCache, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationCache, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            std::vector<CacheInfo> cInfoVec;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationCache)
                {
                    CacheInfo cInfo;

                    cInfo.Associativity = pData->Cache.Associativity;
                    cInfo.CacheSize = pData->Cache.CacheSize;
                    cInfo.CacheType = pData->Cache.Type;
                    cInfo.Level = (CacheLevel)pData->Cache.Level;
                    cInfo.LineSize = pData->Cache.LineSize;

                    cInfoVec.push_back(cInfo);
                }
            }

            delete [] dData;
            return cInfoVec;
        }

        delete [] pData;
    }

    return std::vector<CacheInfo>();
}

std::vector<CacheInfo> AProcessorTopology::cacheInfo(const CacheLevel &Level) const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationCache, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationCache, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            std::vector<CacheInfo> cInfoVec;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationCache && pData->Cache.Level == Level)
                {
                    CacheInfo cInfo;

                    cInfo.Associativity = pData->Cache.Associativity;
                    cInfo.CacheSize = pData->Cache.CacheSize;
                    cInfo.CacheType = pData->Cache.Type;
                    cInfo.Level = (CacheLevel)pData->Cache.Level;
                    cInfo.LineSize = pData->Cache.LineSize;

                    cInfoVec.push_back(cInfo);
                }
            }

            delete [] dData;
            return cInfoVec;
        }

        delete [] pData;
    }

    return std::vector<CacheInfo>();
}

std::vector<CacheInfo> AProcessorTopology::cacheInfo(const USHORT &Socket) const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            std::vector<CacheInfo> cInfoVec;
            int socket = -1;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationProcessorPackage)
                {
                    socket++;
                }
                if(pData->Relationship == RelationCache && socket == Socket)
                {
                    CacheInfo cInfo;

                    cInfo.Associativity = pData->Cache.Associativity;
                    cInfo.CacheSize = pData->Cache.CacheSize;
                    cInfo.CacheType = pData->Cache.Type;
                    cInfo.Level = (CacheLevel)pData->Cache.Level;
                    cInfo.LineSize = pData->Cache.LineSize;

                    cInfoVec.push_back(cInfo);
                }
            }

            delete [] dData;
            return cInfoVec;
        }

        delete [] pData;
    }

    return std::vector<CacheInfo>();
}

std::vector<CacheInfo> AProcessorTopology::cacheInfo(const CacheLevel &Level, const USHORT &Socket) const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            std::vector<CacheInfo> cInfoVec;
            int socket = -1;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationProcessorPackage)
                {
                    socket++;
                }
                if(pData->Relationship == RelationCache && pData->Cache.Level == Level && socket == Socket)
                {
                    CacheInfo cInfo;

                    cInfo.Associativity = pData->Cache.Associativity;
                    cInfo.CacheSize = pData->Cache.CacheSize;
                    cInfo.CacheType = pData->Cache.Type;
                    cInfo.Level = (CacheLevel)pData->Cache.Level;
                    cInfo.LineSize = pData->Cache.LineSize;

                    cInfoVec.push_back(cInfo);
                }
            }

            delete [] dData;
            return cInfoVec;
        }

        delete [] pData;
    }

    return std::vector<CacheInfo>();
}

int AProcessorTopology::cacheCount(const CacheLevel &Level) const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationCache, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationCache, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;
            char *end = current + buffer;

            int cCount = 0;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationCache)
                {
                    if(Level == L1 && pData->Cache.Level == 1)
                    {
                        cCount++;
                    }
                    if(Level == L2 && pData->Cache.Level == 2)
                    {
                        cCount++;
                    }
                    if(Level == L3 && pData->Cache.Level == 3)
                    {
                        cCount++;
                    }
                }
            }

            delete [] dData;
            return cCount;
        }

        delete [] pData;
    }

    return 0;
}

int AProcessorTopology::cacheCount(const CacheLevel &Level, const USHORT &Socket) const
{
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pData;
    ULONG buffer = 1;
    char dummy[256] = {};

    pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)dummy;

    if(!GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
    {
        pData = new SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX[buffer];

        if(GetLogicalProcessorInformationEx(RelationAll, pData, &buffer))
        {
            PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX dData = pData;

            char *current = (char *)pData;

            int cCount = 0;
            int socket = -1;

            char *end = current + buffer;

            for(int idx = 0; current < end; current += ((PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current)->Size, idx++)
            {
                pData = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)current;

                if(pData->Relationship == RelationProcessorPackage)
                {
                    socket++;
                }

                if(pData->Relationship == RelationCache && socket == Socket)
                {
                    if(Level == L1 && pData->Cache.Level == 1)
                    {
                        cCount++;
                    }
                    if(Level == L2 && pData->Cache.Level == 2)
                    {
                        cCount++;
                    }
                    if(Level == L3 && pData->Cache.Level == 3)
                    {
                        cCount++;
                    }
                }
            }

            delete [] dData;
            return cCount;
        }

        delete [] pData;
    }


    return 0;
}

ULONG64 AProcessorTopology::coreToMask(const int &Core) const
{
    int cMask = 0;

    if(Core > 63)
    {
        for(int i = 0; i < Core; i++)
        {
            cMask++;

            if(i == 63)
            {
                cMask = 0;
            }
        }
    }
    else
    {
        cMask = Core;
    }

    return static_cast<ULONG64>(1) << cMask;
}

int AProcessorTopology::maskToCore(const ULONG64 &Mask) const
{
    for(int c = 0; c < 64; c++)
    {
        if(static_cast<ULONG64>(1) << c == Mask)
        {
            return c;
        }
    }

    return 0;
}

short AProcessorTopology::coreOnSocket(const int &Core) const
{
    int cCount = totalCoreCount() - 1;

    if(Core > cCount || Core < 0)
    {
        return -1;
    }

    int cps = coreCountPerSocket(Logical);
    int sCount = 0;
    int cPerS = 0;

    for(int i = 0; i < cCount; i++)
    {
        cPerS++;
        if(cPerS == cps)
        {
            cPerS = 0;
            sCount++;
        }
        if(i == Core)
        {
            break;
        }
    }

    return sCount;
}

std::vector<short> AProcessorTopology::coreOnSocket(const BitMask &Mask) const
{
    std::vector<short> sockets;
    std::vector<int> masks = Mask.cores();

    for(int i = 0; i < masks.size(); i++)
    {
        sockets.push_back(coreOnSocket(masks[i]));
    }

    return sockets;
}

ULONG AProcessorTopology::totalCoreCount() const
{
    return GetActiveProcessorCount(ALL_PROCESSOR_GROUPS);
}

USHORT AProcessorTopology::groupCount() const
{
    USHORT group = 0;

    while(GetActiveProcessorCount(group) != 0)
    {
        group++;
    }

    return group;
}


ULONG AProcessorTopology::coreCountInGroup(const short &Group) const
{
    return GetActiveProcessorCount(Group);
}

CoreLocation AProcessorTopology::coreLocation(const int &Core) const
{
    if(Core > totalCoreCount() || Core < 0)
    {
        return CoreLocation();
    }

    short group = -1;
    short socket = -1;

    for(int i = -1; i < Core; i += 64) // Calculate the Processor Group.
    {
        group++;
    }

    socket = coreOnSocket(Core); // Get the processor socket that the core sits on.

    CoreLocation cp;

    cp.Mask.Group = group;
    cp.Socket = socket;
    cp.ThreadNumber = Core;
    cp.Mask.Mask.setMask(coreToMask(Core));

    return cp;
}

std::vector<CoreLocation> AProcessorTopology::coreLocation(const std::vector<int> &Cores) const
{
    std::vector<CoreLocation> cPos;

    for(int i = 0; i < Cores.size(); i++)
    {
        cPos.push_back(coreLocation(Cores[i]));
    }

    return cPos;
}

std::vector<CoreLocation> AProcessorTopology::coreLocation(const BitMask &Mask) const
{
    return coreLocation(Mask.cores());
}

std::vector<CoreLocation> AProcessorTopology::allCoreLocations() const
{
    std::vector<CoreLocation> cPos;
    int cCount = totalCoreCount();

    for(int i = 0; i < cCount; i++)
    {
        cPos.push_back(coreLocation(i));
    }

    return cPos;
}

BitMaskEx AProcessorTopology::coreMask(const int &Core) const
{
    return coreLocation(Core).Mask;
}

std::vector<BitMaskEx> AProcessorTopology::coreMask(const std::vector<int> &Cores) const
{
    std::vector<BitMaskEx> Mask;

    for(int i = 0; i < Cores.size(); i++)
    {
        Mask.push_back(coreLocation(Cores[i]).Mask);
    }

    return Mask;
}

std::vector<BitMaskEx> AProcessorTopology::allCoreMasks() const
{
    std::vector<BitMaskEx> Mask;
    ULONG cCount = totalCoreCount();

    for(ULONG i = 0; i < cCount; i++)
    {
        Mask.push_back(coreLocation(i).Mask);
    }

    return Mask;
}

std::vector<int> AProcessorTopology::efficiency() const
{
    std::vector<int> eff;
    ULONG cCount = totalCoreCount(Physical);

    for(ULONG i = 0; i < cCount; i++)
    {
        eff.push_back(coreEfficiency(i));
    }

    return eff;
}

std::vector<int> AProcessorTopology::efficiency(const BitMask &Mask) const
{
    std::vector<int> eff, cores = Mask.cores();

    for(int i = 0; i < cores.size(); i++)
    {
        eff.push_back(coreEfficiency(cores[i]));
    }

    return eff;
}

bool AProcessorTopology::isHomogenCoreArch() const
{
    std::vector<int> eff = efficiency();

    int stdEfficiency = eff[0];

    for(int i = 0; i < eff.size(); i++)
    {
        if(eff[i] != stdEfficiency)
        {
            return false;
        }
    }

    return true;
}

bool AProcessorTopology::isHeterogenCoreArch() const
{
    std::vector<int> eff = efficiency();

    int stdEfficiency = eff[0];

    for(int i = 0; i < eff.size(); i++)
    {
        if(eff[i] != stdEfficiency)
        {
            return true;
        }
    }

    return false;
}

ULONG64 AProcessorTopology::FileTimeToULONG64(const FILETIME &ft) const
{
    ULARGE_INTEGER li;

    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;

    return li.QuadPart;
}

float AProcessorTopology::GetCPULoad()
{
    FILETIME idleTime, kernelTime, userTime;

    if(GetSystemTimes(&idleTime, &kernelTime, &userTime))
    {
        return sLoad.calculateCPULoad(FileTimeToULONG64(idleTime), FileTimeToULONG64(kernelTime) + FileTimeToULONG64(userTime));
    }

    return -1;
}

std::vector<float> AProcessorTopology::getCPULoadPerCore()
{
    std::vector<float> values;
    ULONG retSize = 0;

    values.reserve(tCoreCount);
    PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION sppi = kernel.ntData<SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION>(SystemProcessorPerformanceInformation,
                                                        sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * tCoreCount, retSize);

    for(ULONG i = 0; i < tCoreCount; i++)
    {
        values.push_back(sLoadVec[i].calculateCPULoad(sppi[i].IdleTime.QuadPart, sppi[i].KernelTime.QuadPart + sppi[i].UserTime.QuadPart));
    }

    kernel.freeNTData();

    return values;
}

float _SysCpuLoad::calculateCPULoad(const ULONG64 &idleTicks, const ULONG64 &totalTicks)
{
   ULONG64 totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
   ULONG64 idleTicksSinceLastTime  = idleTicks - _previousIdleTicks;

   float ret = 0;

   if(totalTicksSinceLastTime > 0)
   {
       ret = 1 - ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime;
   }

   _previousTotalTicks = totalTicks;
   _previousIdleTicks  = idleTicks;

   ret *= 100;

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
