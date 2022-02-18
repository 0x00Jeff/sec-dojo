//#ifndef	_SIGSCAN_H
	#define _SIGSCAN_H

	#define NTDLL	"ntdll.dll"
	PVOID try_sig(void *start, size_t range, char *sig, size_t sig_len, char *mask, size_t mask_len);
	PVOID GetCode(char *dll_name, DWORD *SizeOfCode);
	PVOID FindLdrpCallInitRoutine();
	extern void *NibbleSigScan(void *start, size_t range, char *sig, size_t sig_len, char *mask, size_t mask_len);
//#endif
