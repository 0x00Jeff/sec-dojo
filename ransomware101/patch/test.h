#ifndef _TEST_H
	#define _TEST_H

	#include<windows.h>
	#include<ntdef.h>	// for UNICODE_STRING
	#include<stdio.h>
	#include<wchar.h>
	#include<stdlib.h>	// malloc

// the address and the length of the call instruction that
    // invokes the dll entry point in ntdll.LdrpCallInitRoutine
    PBYTE call_rsi;
    size_t call_rsi_len;

    // winapi functions
	typedef void (WINAPI *_RtlInitUnicodeString)(
		PUNICODE_STRING OUT DestinationString,
		PCWSTR IN SourceString);

	typedef NTSTATUS(WINAPI *_LdrLoadDll)(
            PWSTR IN SearchPath,
            PULONG IN LoadFlags,
            PUNICODE_STRING DllName,
            HMODULE *BaseAddress);

	typedef BOOL APIENTRY (*DllMain)(HMODULE hModule,
		DWORD ul_reason_for_call,
		LPVOID lpReserved);

	typedef DllMain DllEntry;

	typedef char* (*key_gen)(void);

	// LdrLoadDll address
	_LdrLoadDll MyLdrLoadDll;

	NTSTATUS WINAPI FakeLdrLoadDll( PWSTR IN SearchPath,
		PULONG IN LoadFlags,
		PUNICODE_STRING DllName,
		HMODULE *BaseAddress);

    // other functions
	extern void hook(PVOID *OriginalFunction, PVOID FakeFunction);
	extern void unhook(PVOID function); // TODO : how the fuck do I make this one inline
	extern PVOID FindLdrpCallInitRoutine(void);
	extern PVOID SigScan(PVOID start, PVOID end, PBYTE sig, size_t len);
	void patch(PVOID addr, char *original_bytes);
	void PatchDllMain(PVOID addr);

	// needed values
	#define DLL_NAME "r101.dll"
	#define WIDE_DLL_NAME L""DLL_NAME

	// offsets
	#define DLL_TO_ENTRY	0x1C6AC
	#define DLL_TO_DLLMAIN	0x2600
	#define DLLMAIN_OFFSET         0x2600
	#define KEY_GENERATION_OFFSET   0x20e0 // returns a char *
	#define TRUE	1
	#define FALSE	0
#endif

