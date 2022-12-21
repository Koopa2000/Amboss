#include "ATime.h"

AHighPrecisionTimer::AHighPrecisionTimer()
{
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);

    frequency = freq.QuadPart;
}

long AHighPrecisionTimer::timerFrequency() const
{
    return frequency;
}

void AHighPrecisionTimer::startTimer()
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    startTime = time.QuadPart;
}

void AHighPrecisionTimer::stopTimer()
{
    LARGE_INTEGER time;
    QueryPerformanceCounter(&time);

    stopTime = time.QuadPart;
}

void AHighPrecisionTimer::setTimeUnit(const TimeUnit &timeUnit)
{
    tUnit = timeUnit;
}

AHighPrecisionTimer::TimeUnit AHighPrecisionTimer::timeUnit() const
{
    return tUnit;
}

void AHighPrecisionTimer::setDecimalPlaces(const int &places)
{
    decPlace = places;
}

int AHighPrecisionTimer::decimalPlaces() const
{
    return decPlace;
}

double AHighPrecisionTimer::elapsed() const
{
    return elapsed(tUnit);
}

double AHighPrecisionTimer::elapsed(const TimeUnit &timeUnit) const
{
    double time = ((stopTime - startTime) * 1000000.0) / frequency;

    if(timeUnit == Microsecond)
    {
        return time;
    }
    if(timeUnit == Millisecond)
    {
        return DROUND(time / 1000, decPlace);
    }
    if(timeUnit == Second)
    {
        return DROUND(time / 1000000, decPlace);
    }

    return 0;
}

ADateTime::ADateTime()
{

}

void ADateTime::setTimeCode(const TimeCode &code)
{
    tCode = code;
}

ADateTime::TimeCode ADateTime::timeCode() const
{
    return tCode;
}

ADateTime::Time ADateTime::time()
{
    return time(tCode);
}

ADateTime::Time ADateTime::time(const TimeCode &code)
{
    SYSTEMTIME st;
    Time t;

    if(code == UTC)
    {
        GetSystemTime(&st);

        t.Hour = st.wHour;
        t.Minute = st.wMinute;
        t.Second = st.wSecond;
        t.Milliseconds = st.wMilliseconds;
    }
    if(code == LocalTime)
    {
        GetLocalTime(&st);

        t.Hour = st.wHour;
        t.Minute = st.wMinute;
        t.Second = st.wSecond;
        t.Milliseconds = st.wMilliseconds;
    }

    return t;
}

ADateTime::Date ADateTime::date()
{
    return date(tCode);
}

ADateTime::Date ADateTime::date(const TimeCode &code)
{
    SYSTEMTIME st;
    Date d;

    if(code == UTC)
    {
        GetSystemTime(&st);

        d.day = st.wDay;
        d.dayOfWeek = (Day)st.wDayOfWeek;
        d.month = (Month)st.wMonth;
        d.year = st.wYear;
    }
    if(code == LocalTime)
    {
        GetLocalTime(&st);

        d.day = st.wDay;
        d.dayOfWeek = (Day)st.wDayOfWeek;
        d.month = (Month)st.wMonth;
        d.year = st.wYear;
    }

    return d;
}

ADateTime::DateTime ADateTime::dateTime()
{
    return dateTime(tCode);
}

ADateTime::DateTime ADateTime::dateTime(const TimeCode &code)
{
    SYSTEMTIME st;
    DateTime dt;

    if(code == UTC)
    {
        GetSystemTime(&st);

        dt.Hour = st.wHour;
        dt.Minute = st.wMinute;
        dt.Second = st.wSecond;
        dt.Milliseconds = st.wMilliseconds;
        dt.day = st.wDay;
        dt.dayOfWeek = (Day)st.wDayOfWeek;
        dt.month = (Month)st.wMonth;
        dt.year = st.wYear;
    }
    if(code == LocalTime)
    {
        GetLocalTime(&st);

        dt.Hour = st.wHour;
        dt.Minute = st.wMinute;
        dt.Second = st.wSecond;
        dt.Milliseconds = st.wMilliseconds;
        dt.day = st.wDay;
        dt.dayOfWeek = (Day)st.wDayOfWeek;
        dt.month = (Month)st.wMonth;
        dt.year = st.wYear;
    }

    return dt;
}
