#ifndef APROCESSORTOPOLOGY_H
#define APROCESSORTOPOLOGY_H

#include "H:/Amboss/ANtKernel.h"

#include<bitset>

struct BitMask
{
    std::string Bits;
    ULONG64 Mask;

    BitMask()
    {
        reset();
    }

    bool operator ==(const BitMask &other) const
    {
        if(Bits == other.Bits && Mask == other.Mask)
        {
            return true;
        }

        return false;
    }

    bool operator !=(const BitMask &other) const
    {
        if(Bits == other.Bits && Mask == other.Mask)
        {
            return false;
        }

        return true;
    }

    void setBits(const std::string &bits)
    {
        Bits = bits;

        std::bitset<64> b(Bits);
        Mask = b.to_ullong();
    }

    void setMask(const ULONG64 &mask)
    {
        Mask = mask;

        std::bitset<64> b(mask);
        Bits = b.to_string();
    }

    void enable(const std::vector<int> &Cores)
    {
        reset();

        for(int i = 0; i < Cores.size(); i++)
        {
            Bits[63 - Cores[i]] = '1';
        }

        std::bitset<64> b(Bits);
        Mask = b.to_ullong();
    }

    void enable(const int &Core)
    {
        Bits[63 - Core] = '1';

        std::bitset<64> b(Bits);
        Mask = b.to_ullong();
    }

    void disable(const std::vector<int> &Cores)
    {
        reset();

        for(int i = 0; i < Cores.size(); i++)
        {
            Bits[63 - Cores[i]] = '0';
        }

        std::bitset<64> b(Bits);
        Mask = b.to_ullong();
    }

    void disable(const int &Core)
    {
        Bits[63 - Core] = '0';

        std::bitset<64> b(Bits);
        Mask = b.to_ullong();
    }

    void enableAll()
    {
        SYSTEM_INFO sInfo;
        GetSystemInfo(&sInfo);

        for(int i = 0; i < sInfo.dwNumberOfProcessors; i++)
        {
            Bits[63 - i] = '1';
        }

        std::bitset<64> b(Bits);
        Mask = b.to_ullong();
    }

    void disableAll()
    {
        SYSTEM_INFO sInfo;
        GetSystemInfo(&sInfo);

        for(int i = 0; i < sInfo.dwNumberOfProcessors; i++)
        {
            Bits[63 - i] = '0';
        }

        std::bitset<64> b(Bits);
        Mask = b.to_ullong();
    }

    std::vector<int> cores() const
    {
        std::vector<int> Cores;

        for(int i = 0; i < 64; i++)
        {
            if(Bits[63 - i] == '1')
            {
                Cores.push_back(i);
            }
        }

        return Cores;
    }

    bool isActive(const std::vector<int> &Cores) const
    {
        for(int i = 0; i < Cores.size(); i++)
        {
            if(Bits[63 - Cores[i]] != '1')
            {
                return false;
            }
        }

        return true;
    }

    bool isActive(const int &Cores) const
    {
        if(Bits[63 - Cores] == '1')
        {
            return true;
        }

        return false;
    }

    void reset()
    {
        Bits.clear();
        Mask = 0;

        for(int i = 0; i < 64; i++)
        {
            Bits.push_back('0');
        }
    }

};

class _SysCpuLoad
{
public:
    float calculateCPULoad(const ULONG64 &idleTicks, const ULONG64 &totalTicks);

private:
    ULONG64 _previousTotalTicks = 0, _previousIdleTicks = 0;
};

enum CoreType
{
    Logical,
    Physical
};

struct BitMaskEx
{
    short Group = -1;
    BitMask Mask;
};

struct CoreLocation
{
    short Socket = -1;
    int ThreadNumber = -1;
    BitMaskEx Mask;
};

enum CacheLevel
{
    L1 = 1,
    L2,
    L3
};

struct CacheInfo
{
    CacheLevel Level;
    short Associativity;
    USHORT LineSize;
    ULONG CacheSize;
    PROCESSOR_CACHE_TYPE CacheType;
};

class AProcessorTopology
{
public:
    AProcessorTopology();

    ULONG64 coreToMask(const int &Core) const;
    int maskToCore(const ULONG64 &Mask) const;

    USHORT socketCount() const;
    USHORT numaNodeCount() const;
    USHORT groupCount() const;

    int coreCountPerSocket(const CoreType &Type) const;
    short coreOnSocket(const int &Core) const;
    std::vector<short> coreOnSocket(const BitMask &Mask) const;

    short groupCountPerSocket() const;

    int totalCoreCount(const CoreType &Type) const;
    ULONG totalCoreCount() const;

    ULONG coreCountInGroup(const short &Group) const;

    CoreLocation coreLocation(const int &Core) const;
    std::vector<CoreLocation> coreLocation(const std::vector<int> &Cores) const;
    std::vector<CoreLocation> coreLocation(const BitMask &Mask) const;
    std::vector<CoreLocation> allCoreLocations() const;

    BitMaskEx coreMask(const int &Core) const;
    std::vector<BitMaskEx> coreMask(const std::vector<int> &Cores) const;
    std::vector<BitMaskEx> allCoreMasks() const;

    int coreEfficiency(const int &Core) const;
    std::vector<int> efficiency() const;
    std::vector<int> efficiency(const BitMask &Mask) const;

    bool isHomogenCoreArch() const;
    bool isHeterogenCoreArch() const;

    std::vector<CacheInfo> cacheInfo() const;
    std::vector<CacheInfo> cacheInfo(const CacheLevel &Level) const;
    std::vector<CacheInfo> cacheInfo(const USHORT &Socket) const;
    std::vector<CacheInfo> cacheInfo(const CacheLevel &Level, const USHORT &Socket) const;

    int cacheCount(const CacheLevel &Level) const;
    int cacheCount(const CacheLevel &Level, const USHORT &Socket) const;

    float GetCPULoad();
    std::vector<float> getCPULoadPerCore();

private:
    ULONG64 FileTimeToULONG64(const FILETIME &ft) const;

    ANTKernel kernel;
    _SysCpuLoad sLoad;
    std::vector<_SysCpuLoad> sLoadVec;

    ULONG tCoreCount;
};

#endif // APROCESSORTOPOLOGY_H
