#ifndef _HOOK
    #define _HOOK

    #include<windows.h>
    #include<stdio.h>
    #include<ntdef.h>	// for UNICODE_STRING

    // the original bytes in LdrLoadDll
    char original_bytes[15];
    #define HOOK_SIZE sizeof(original_bytes)

    // functions
    void hook(PVOID *OriginalFunction, PVOID FakeFunction);
    inline void unhook(PVOID function);
     
#endif