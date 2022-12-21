#ifndef MEMORYUNITCONVERTER_H
#define MEMORYUNITCONVERTER_H

enum MemUnits
{
    Bit,
    Byte,

    Kilobyte,
    Kibibyte,

    Megabyte,
    Mebibyte,

    Gigabyte,
    Gibibyte,

    Terabyte,
    Tebibyte,

    Petabyte,
    Pebibyte
};

template <class OUTTYPE, typename INTYPE>
OUTTYPE unitConvert(const INTYPE &Size, const MemUnits &CurrentUnit, const MemUnits &TargetUnit)
{
    OUTTYPE ret;

    if(CurrentUnit == Bit)
    {
        ret = Size / 8;
    }
    if(CurrentUnit == Byte)
    {
       ret = Size;
    }
    if(CurrentUnit == Kilobyte)
    {
        ret = Size * 1000;
    }
    if(CurrentUnit == Kibibyte)
    {
        ret = Size * 1024;
    }
    if(CurrentUnit == Megabyte)
    {
        ret = Size * 1000000;
    }
    if(CurrentUnit == Mebibyte)
    {
        ret = Size * 1048576;
    }
    if(CurrentUnit == Gigabyte)
    {
        ret = Size * 1000000000;
    }
    if(CurrentUnit == Gibibyte)
    {
        ret = Size * 1073741824;
    }
    if(CurrentUnit == Terabyte)
    {
        ret = Size * 1000000000000;
    }
    if(CurrentUnit == Tebibyte)
    {
        ret = Size * 1099511627776;
    }
    if(CurrentUnit == Petabyte)
    {
        ret = Size * 1000000000000000;
    }
    if(CurrentUnit == Pebibyte)
    {
        ret = Size * 1125899906842624;
    }

    if(TargetUnit == Bit)
    {
        return ret * 8;
    }
    if(TargetUnit == Byte)
    {
        return ret;
    }
    if(TargetUnit == Kilobyte)
    {
        return ret / 1000;
    }
    if(TargetUnit == Kibibyte)
    {
        return ret / 1024;
    }
    if(TargetUnit == Megabyte)
    {
        return ret / 1000000;
    }
    if(TargetUnit == Mebibyte)
    {
        return ret / 1048576;
    }
    if(TargetUnit == Gigabyte)
    {
        return ret / 1000000000;
    }
    if(TargetUnit == Gibibyte)
    {
        return ret / 1073741824;
    }
    if(TargetUnit == Terabyte)
    {
        return ret / 1000000000000;
    }
    if(TargetUnit == Tebibyte)
    {
        return ret / 1099511627776;
    }
    if(TargetUnit == Petabyte)
    {
        return ret / 1000000000000000;
    }
    if(TargetUnit == Pebibyte)
    {
        return ret / 1125899906842624;
    }

    return 0;
}

#endif // MEMORYUNITCONVERTER_H
