#ifndef ANTKERNEL_H
#define ANTKERNEL_H

#include "ANativeStruct.h"

#include<ntstatus.h>

class ANTKernel
{
public:
    ANTKernel()
    {
        ntQuerySystemInformation = (_NTQuerySystemInformation)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQuerySystemInformation");
        _ntQueryThreadInformation = (_NTQueryThreadInformation)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread");
        _ntSetInformationProcess = (_NTSetInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSetInformationProcess");
        _ntQueryProcessInformation = (_NTQueryInformationProcess)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationProcess");
        _ntSetInformationThread = (_NTSetInformationThread)GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtSetInformationThread");
    }

    ~ANTKernel()
    {
        freeNTData();

        ntQuerySystemInformation = NULL;
        _ntQueryThreadInformation = NULL;
        _ntSetInformationProcess = NULL;
        _ntQueryProcessInformation = NULL;
        _ntSetInformationThread = NULL;
    }

    NTSTATUS ntQueryThreadInformation(const HANDLE &tHandle, const THREADINFOCLASS &tInfoClass, const PVOID &threadInformation,
                                                      const ULONG &threadInformationLength, PULONG ReturnLength) const
    {
        return _ntQueryThreadInformation(tHandle, tInfoClass, threadInformation, threadInformationLength, ReturnLength);
    }

    NTSTATUS ntSetProcessInformation(const HANDLE &pHandle, const PROCESSINFOCLASS &pInfoClass, const PVOID &processInformation,
                                     const ULONG &processInformationLength) const
    {
        return _ntSetInformationProcess(pHandle, pInfoClass, processInformation, processInformationLength);
    }

    NTSTATUS ntSetThreadInformation(const HANDLE &tHandle, const THREADINFOCLASS &tInfoClass, const PVOID &threadInformation,
                                    const ULONG &threadInformationLength) const
    {
        return _ntSetInformationThread(tHandle, tInfoClass, threadInformation, threadInformationLength);
    }

    NTSTATUS ntQueryProcessInformation(const HANDLE &pHandle, const PROCESSINFOCLASS &pInfoClass, const PVOID &processInformation,
                                       const ULONG &processInformationLength, PULONG ReturnLength) const
    {
        return _ntQueryProcessInformation(pHandle, pInfoClass, processInformation, processInformationLength, ReturnLength);
    }

    template <typename T>
    T *ntData(const SYSTEM_INFORMATION_CLASS &ntInfoClass)
    {
        ULONG bufferLength = 0;
        NTSTATUS status = ntQuerySystemInformation(ntInfoClass, NULL, 0, &bufferLength);

        if(status != STATUS_INFO_LENGTH_MISMATCH)
        {
            return NULL;
        }

        freeNTData();

        NTData = new T[bufferLength];

        status = ntQuerySystemInformation(ntInfoClass, NTData, bufferLength, &bufferLength);

        if(NT_SUCCESS(status))
        {
            return (T *)NTData;
        }

        delete [] (T *)NTData;
        NTData = NULL;
        return NULL;
    }

    template <typename T>
    T *ntData(const SYSTEM_INFORMATION_CLASS &ntInfoClass, const ULONG &bufferSize, ULONG &returnBufferSize)
    {
        NTSTATUS status;

        freeNTData();

        NTData = new T[bufferSize];

        status = ntQuerySystemInformation(ntInfoClass, NTData, bufferSize, &returnBufferSize);

        if(NT_SUCCESS(status))
        {
            return (T *)NTData;
        }

        delete [] (T *)NTData;
        NTData = NULL;
        return NULL;
    }

    void freeNTData()
    {
        if(NTData != NULL)
        {
            delete [] (PULONG)NTData;
            NTData = NULL;
        }
    }

private:
    void *NTData = NULL;

    typedef NTSTATUS (__stdcall *_NTQuerySystemInformation)(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG);
    typedef NTSTATUS (__stdcall *_NTQueryThreadInformation)(HANDLE, THREADINFOCLASS, PVOID, ULONG, PULONG);
    typedef NTSTATUS (__stdcall *_NTSetInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG);
    typedef NTSTATUS (__stdcall *_NTSetInformationThread)(HANDLE, THREADINFOCLASS, PVOID, ULONG);
    typedef NTSTATUS (__stdcall *_NTQueryInformationProcess)(HANDLE, PROCESSINFOCLASS, PVOID, ULONG, PULONG);

    _NTQuerySystemInformation ntQuerySystemInformation = NULL;
    _NTQueryThreadInformation _ntQueryThreadInformation = NULL;
    _NTSetInformationProcess _ntSetInformationProcess = NULL;
    _NTSetInformationThread _ntSetInformationThread = NULL;
    _NTQueryInformationProcess _ntQueryProcessInformation = NULL;
};

#endif // ANTKERNEL_H
