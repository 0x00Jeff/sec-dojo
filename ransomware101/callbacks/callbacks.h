/* pasted from https://github.com/tpn/tracer/blob/master/Rtl/Rtl.h */

typedef struct _LDR_DLL_LOADED_NOTIFICATION_DATA {  
    ULONG Flags;                   //Reserved.  
    PCUNICODE_STRING FullDllName;  //The full path name of the DLL module.  
    PCUNICODE_STRING BaseDllName;  //The base file name of the DLL module.  
    PVOID DllBase;                 //A pointer to the base address for the DLL in memory.  
    ULONG SizeOfImage;             //The size of the DLL p_w_picpath, in bytes.  
} LDR_DLL_LOADED_NOTIFICATION_DATA, *PLDR_DLL_LOADED_NOTIFICATION_DATA;

typedef struct _LDR_DLL_UNLOADED_NOTIFICATION_DATA {
    ULONG Flags;
    PCUNICODE_STRING FullDllName;
    PCUNICODE_STRING BaseDllName;
    PVOID DllBase;
    ULONG SizeOfImage;
} LDR_DLL_UNLOADED_NOTIFICATION_DATA, *PLDR_DLL_UNLOADED_NOTIFICATION_DATA;

typedef union _LDR_DLL_NOTIFICATION_DATA {
    LDR_DLL_LOADED_NOTIFICATION_DATA Loaded;
    LDR_DLL_UNLOADED_NOTIFICATION_DATA Unloaded;
} LDR_DLL_NOTIFICATION_DATA, *PLDR_DLL_NOTIFICATION_DATA;

typedef const LDR_DLL_NOTIFICATION_DATA *PCLDR_DLL_NOTIFICATION_DATA;

typedef VOID (CALLBACK LDR_DLL_NOTIFICATION_FUNCTION)(
    _In_     ULONG                       NotificationReason,
    _In_     PCLDR_DLL_NOTIFICATION_DATA NotificationData,
    _In_opt_ PVOID                       Context
);
typedef LDR_DLL_NOTIFICATION_FUNCTION *PLDR_DLL_NOTIFICATION_FUNCTION;

typedef NTSTATUS (NTAPI *LdrRegisterDllNotification)(
  _In_     ULONG                          Flags,
  _In_     PLDR_DLL_NOTIFICATION_FUNCTION NotificationFunction,
  _In_opt_ PVOID                          Context,
  _Out_    PVOID                          *Cookie
);

#ifndef LDR_DLL_NOTIFICATION_REASON_LOADED
    #define LDR_DLL_NOTIFICATION_REASON_LOADED 1
#endif

// TODO : figure out what header to include to get these
#ifndef STATUS_SUCCESS
    #define STATUS_SUCCESS 0
#endif

typedef NTSTATUS (NTAPI *LdrUnregisterDllNotification)(
  _In_ PVOID Cookie
);

#define DLL_MAIN_OFFSET 	0x2600
#define KEY_GENERATION_OFFSET	0x20e0
#define DLL_NAME		"r101.dll"
#define WIDE_DLL_NAME		L""DLL_NAME
