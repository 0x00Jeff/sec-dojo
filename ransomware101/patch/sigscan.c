#include<stdio.h>
#include<windows.h>
#include<psapi.h>
#include<winnt.h>
//#include"test.h"
#include"sigscan.h"

// the length of the opcode calling DllEntry
//size_t call_dll_main_len;

extern PBYTE call_rsi;
extern size_t call_rsi_len;

#define CHECK_RESULT(ptr, old_ptr, sig_size, sig, call_opcode_size) \
	if(old_ptr){	\
		if(ptr && ptr != old_ptr){	\
			fprintf(stderr, "FOUND MORE THAN ONE SIG, EXITING ..\n"); \
			return NULL;	\
		}	\
	}else{	\
		old_ptr = ptr;	\
		sig_size = sizeof(sig);	\
		call_rsi_len = call_opcode_size; 	\
	}

PVOID FindLdrpCallInitRoutine(void){

	// Get a handle to ntdll
//	HANDLE HNtdll = GetModuleHandleA(NTDLL);
	HANDLE HNtdll = LoadLibraryA(NTDLL);
	if(!HNtdll){
		fprintf(stderr, "couldn't find %s :(\n", NTDLL);
		exit(EXIT_FAILURE);
	}

	// can't use an array of pointers to arrays because without
	// storing the size somehow, cause the arrays are of different sizes,
	// using vectors ~~probably~~ could have solved this AND would have
	// made it a LOT cleaner but we don't do that here

	// sig1
	CHAR sig1[] = {
		0xff, 0xd0,		// call rax
		0x4d, 0x8b, 0xc6,	// mov r8, rXX
		0x8b, 0xd3, 		// mov edx, reg
		0x48, 0x8b, 0xcf,	// mov rcx, reg
		0xff, 0xd6		// call rsi
	};
	size_t sig1_call_len = 2;

	char *mask1 = "xx"
		"xxx"
		"xx"
		"xxx"
		"xx";

	// sig2;;
	CHAR sig2[] = {
		0x4d, 0x8b, 0xc0,		// mov r8, rXX
		0x8b, 0xd0, 			// mov edx, reg
		0x40, 0x8b, 0xc0,		// mov rcx, reg
		0x40, 0x8b, 0xc0,		// mov rax, reg
		0x4c, 0x8b, 0x0d, 0, 0, 0, 0,	// mov r9, FunctionAddress
		0x41, 0xff, 0xd1		// call r9
	};
	size_t sig2_call_len = 3;

	char *mask2 = "hxh"
		"xh"
		"hxh"
		"hxh"
		"xxx????"
		"xxx";

	// sig3
	CHAR sig3[] = {
		0x4d, 0x8b, 0xc0,	// mov r8, rXX
		0x8b, 0xd0, 		// mov edx, reg
		0x40, 0x8b, 0xc0,	// mov rcx, reg
		0x40, 0x8b, 0xc0,	// mov rax, reg
		0xff, 0x15, 0, 0, 0, 0	// call cs:function // 6 bytes
	};

	char *mask3 = "xxh"
		"xh"
		"hxh"
		"hxh"
		"xx????";

	size_t sig3_call_len = 6;


	// sig4
	CHAR sig4[] = {
		0x45, 0x33, 0xc0,	// xor r8d, r8d
		0x41, 0x8b, 0xd1,	// mov edx, r9d
		0x48, 0x8b, 0x49, 0x30,	// mov rcx, [rcx + 0x30]
		0xff, 0xd0		// call rax
	};

	char *mask4 = "xxx"
		"xxx"
		"xxxx"
		"xx";

	size_t sig4_call_len = 2;


	// sig5
	CHAR sig5[] = {
		0x4c, 0x8b, 0xc6,	// mov r8, rsi
		0x8b, 0xd7, 		// mov edx, edi
		0x48, 0x8b, 0xcb,	// mov rcx, ebx
		0x41, 0xff, 0xd6	// call r14
	};

	char *mask5 = "xxx"
		"xx"
		"xxx"
		"xxx";

	size_t sig5_call_len = 3;

	//
	DWORD SizeOfCode;
	PVOID StartOfCode = GetCode(NTDLL, &SizeOfCode);

	PVOID ptr = NULL, old_ptr = NULL;
	size_t sig_size = 0;

	// first sig
	ptr = try_sig(StartOfCode, SizeOfCode, sig1, sizeof(sig1), mask1, strlen(mask1));
	CHECK_RESULT(ptr, old_ptr, sig_size, sig1, sig1_call_len);

	// second sig
	ptr = try_sig(StartOfCode, SizeOfCode, sig2, sizeof(sig2), mask2, strlen(mask2));
	CHECK_RESULT(ptr, old_ptr, sig_size, sig2, sig2_call_len);

	// third sig
	ptr = try_sig(StartOfCode, SizeOfCode, sig3, sizeof(sig3), mask3, strlen(mask3));
	CHECK_RESULT(ptr, old_ptr, sig_size, sig3, sig3_call_len);

	// forth sig
	ptr = try_sig(StartOfCode, SizeOfCode, sig4, sizeof(sig4), mask4, strlen(mask4));
	CHECK_RESULT(ptr, old_ptr, sig_size, sig4, sig4_call_len);

	// fifth sig
	ptr = try_sig(StartOfCode, SizeOfCode, sig5, sizeof(sig5), mask5, strlen(mask5));
	CHECK_RESULT(ptr, old_ptr, sig_size, sig5, sig5_call_len);

	if(!old_ptr)
		return NULL;
	else
		return old_ptr + sig_size - call_rsi_len;
}

PVOID GetCode(char *dll_name, DWORD *SizeOfCode){

	HANDLE dll = GetModuleHandleA(dll_name);
	if(!dll){
		fprintf(stderr, "couldn't get a handle to %s\n", dll_name);
		return 0;
	}

	IMAGE_DOS_HEADER *dos = (PIMAGE_DOS_HEADER)dll;

	IMAGE_FILE_HEADER *head = (PIMAGE_FILE_HEADER)((PBYTE)dll + dos -> e_lfanew + sizeof(DWORD));

	IMAGE_OPTIONAL_HEADER *opt_head = (PIMAGE_OPTIONAL_HEADER)(head + 1);

	if(SizeOfCode)
		*SizeOfCode = opt_head ->SizeOfCode;

	return (PVOID)((PBYTE)dll + opt_head -> BaseOfCode);
}

PVOID try_sig(void *start, size_t range, char *sig, size_t sig_len, char *mask, size_t mask_len){

	PBYTE scan_ptr = start;
	PVOID ptr, old_ptr = NULL;
	size_t size = range;
	unsigned int sig_count = 0;

	while( (ptr = NibbleSigScan(scan_ptr, size, sig, sig_len, mask, mask_len)) ){
		old_ptr = ptr; // the last iteration will NULL out `ptr` so we
			       // need to save the last non-NULL address somewhere

		// increase the size by how many bytes were scanned
		size -= (DWORD64)ptr - (DWORD64)scan_ptr;

		// skip the size of the sig
		scan_ptr = ptr + sizeof(sig);

		sig_count++;
	}

	if(sig_count > 1){
		fprintf(stderr, "SIGNATURE COLISSION ERROR, found %d sigs\n", sig_count);
		exit(EXIT_FAILURE);
	}

	return old_ptr;
}
