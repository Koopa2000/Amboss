#ifndef AHIGHPRECISIONTIMER_H
#define AHIGHPRECISIONTIMER_H

#include "ANativeStruct.h"

#include<math.h>

#define FROUND(Num,dPlace) ((floorf(((Num) * powf(10, dPlace)) + 0.5)) / powf(10, dPlace))
#define DROUND(Num,dPlace) ((floor(((Num) * pow(10, dPlace)) + 0.5)) / pow(10, dPlace))
#define LDROUND(Num,dPlace) ((floorl(((Num) * powl(10, dPlace)) + 0.5)) / powl(10, dPlace))

class AHighPrecisionTimer
{
public:
    AHighPrecisionTimer();

    enum TimeUnit
    {
        Microsecond,
        Millisecond,
        Second
    };

    long timerFrequency() const;

    void startTimer();
    void stopTimer();

    void setTimeUnit(const TimeUnit &timeUnit);
    TimeUnit timeUnit() const;

    void setDecimalPlaces(const int &places);
    int decimalPlaces() const;

    double elapsed() const;
    double elapsed(const TimeUnit &timeUnit) const;

private:
    LONG64 frequency, startTime, stopTime;
    TimeUnit tUnit = Microsecond;

    int decPlace = 2;
};

class ADateTime
{
public:
    ADateTime();

    enum Month
    {
        January = 1,
        February,
        March,
        April,
        May,
        June,
        July,
        August,
        September,
        October,
        November,
        December
    };

    enum Day
    {
        Sunday,
        Monday,
        Tuesday,
        Wednesday,
        Thursday,
        Friday,
        Saturday
    };

    struct Date
    {
        Day dayOfWeek;
        Month month;
        USHORT year;
        USHORT day;
    };

    struct Time
    {
        USHORT Hour;
        USHORT Minute;
        USHORT Second;
        USHORT Milliseconds;
    };

    struct DateTime
    {
        Day dayOfWeek;
        Month month;
        USHORT year;
        USHORT day;

        USHORT Hour;
        USHORT Minute;
        USHORT Second;
        USHORT Milliseconds;
    };

    enum TimeCode
    {
        UTC,
        LocalTime
    };

    void setTimeCode(const TimeCode &code);
    TimeCode timeCode() const;

    Time time();
    Time time(const TimeCode &code);

    Date date();
    Date date(const TimeCode &code);

    DateTime dateTime();
    DateTime dateTime(const TimeCode &code);

private:
    TimeCode tCode = LocalTime;
};

#endif // AHIGHPRECISIONTIMER_H
