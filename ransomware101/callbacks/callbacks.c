#include<windows.h>
#include<ntdef.h>         // for UNICODE_STRING
#include<stdio.h>
#include"callbacks.h"

VOID CALLBACK LdrDllNotification(
  _In_     ULONG                       NotificationReason,
  _In_     PCLDR_DLL_NOTIFICATION_DATA NotificationData,
  _In_opt_ PVOID                       Context
);

int main(void){

    // Get a handle to ntdll
	HANDLE HNtdll = GetModuleHandleA("ntdll.dll");
	if(!HNtdll){
		fprintf(stderr, "couldn't find ntdll.dll :(");
		return -1;
	}

    // get the address of LdrRegisterDllNotification
    LdrRegisterDllNotification f = (LdrRegisterDllNotification)(PVOID)GetProcAddress(HNtdll, "LdrRegisterDllNotification");
    if(!f){
        fprintf(stderr, "LdrRegisterDllNotification not found :(");
        return -1;
    }

    // register a dll callback
    PVOID cookie = NULL;
    //wchar_t TargetDll[] = WIDE_DLL_NAME;
    if(f(0, LdrDllNotification, WIDE_DLL_NAME, &cookie) != STATUS_SUCCESS){
        fprintf(stderr, "LdrRegisterDllNotification failed with error code = %ld\n", GetLastError());
        return -1;
    }

    puts("[+] call back registered");

	HMODULE Htry = LoadLibraryA(DLL_NAME);
	if(!Htry){
		fprintf(stderr, "couldn't find %s :(\n", DLL_NAME);
		return -1;
	}


   char * (*key_gen)(void) = (char *(*)(void))((PBYTE)Htry + KEY_GENERATION_OFFSET);
   puts("[+] calling key generation function");

   char *key = key_gen();
   printf("[+] key = %s\n", key);
    LdrUnregisterDllNotification unf = (LdrUnregisterDllNotification)(PVOID)GetProcAddress(HNtdll, "LdrUnregisterDllNotification");
    if(!unf){;
        fprintf(stderr, "LdrUnregisterDllNotification not found :(\n");
        return -1;
    }
    unf(cookie);

    puts("[+] callback unregistered");

    return 0;
}

VOID CALLBACK LdrDllNotification(
  _In_     ULONG                       NotificationReason,
  _In_     PCLDR_DLL_NOTIFICATION_DATA NotificationData,
  _In_opt_ PVOID                       Context
){

	PBYTE ptr;
	DWORD OldProtection;

	wchar_t *target_dll = (wchar_t *)Context;

	if(NotificationReason != LDR_DLL_NOTIFICATION_REASON_LOADED)
		return;

	if(!wcscmp(target_dll, NotificationData -> Loaded.BaseDllName -> Buffer)){

		ptr = NotificationData -> Loaded.DllBase;
		//if(!VirtualProtect(ptr, NotificationData -> Loaded.SizeOfImage, PAGE_EXECUTE_READWRITE, &OldProtection)){
		if(!VirtualProtect(ptr + DLL_MAIN_OFFSET, 1, PAGE_EXECUTE_READWRITE, &OldProtection)){
			fprintf(stderr, "VirtualProtect failed with code = %ld\n", GetLastError());
			return;
		}
		// make DllMain return as soon as it's called
		// so our files don't get encrypted
		*(ptr + DLL_MAIN_OFFSET) = 0xc3;
	}
}
