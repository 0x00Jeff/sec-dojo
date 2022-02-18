#include"test.h"

int main(void){

	// Get a handle to ntdll
	HANDLE HNtdll = LoadLibraryA("ntdll.dll");
	if(!HNtdll){
		fprintf(stderr, "couldn't find ntdll.dll :(");
		return -1;
	}

	// find the address of the instruction we need to patch later
	call_rsi = FindLdrpCallInitRoutine();
	if(!call_rsi){
		fprintf(stderr, "couldn't find LdrpCallInitRoutine using sigs :(\n");
		fprintf(stderr, "probably duo tp my signatures not supporting on your ntdll");
		fprintf(stderr, "upload your ntdll in a github issue if you want it to be supported\n");
		return -1;
	}

	printf("[+] found the call instruction @ %p\n", call_rsi);

	// get the address of LdrLoadDll
	MyLdrLoadDll = (_LdrLoadDll)(PVOID)GetProcAddress(HNtdll, "LdrLoadDll");
	if(!MyLdrLoadDll){
		fprintf(stderr, "couldn't find LdrLoadDll :(");
		return -1;
	}

	// hook LdrLoadDll
	hook((PVOID)MyLdrLoadDll, (PVOID)FakeLdrLoadDll);

	// load the dll
	HMODULE dll = NULL;

	// just testing out the trolling
	dll = LoadLibraryA("iphlpapi.dll");

	puts("[+] loading ransomware");
	dll = LoadLibraryA(DLL_NAME);
	if(!dll){
		fprintf(stderr, "LdrLoadDll failed with code %ld\n", GetLastError());
		return -1;
	}

	DllEntry MyDllEntry = (DllEntry)((PBYTE)dll + DLL_TO_ENTRY);
	key_gen my_key_gen = (key_gen)((PBYTE)dll + KEY_GENERATION_OFFSET);
	DllMain MyDllMain = (DllMain)((PBYTE)dll + DLLMAIN_OFFSET);

	puts("[+] patching DllMain");
	PatchDllMain(MyDllMain);

	printf("[+] calling DllEntry @ \t%p\n", MyDllEntry);
	MyDllEntry(dll, DLL_PROCESS_ATTACH, NULL);

	printf("[+] calling key generation function @ %p\n", my_key_gen);
	printf("[+] key = %s", my_key_gen());

	return 0;
}

void PatchDllMain(PVOID addr){
	DWORD whatever;

	if(!VirtualProtect(addr, 1, PAGE_EXECUTE_READWRITE, &whatever)){
		fprintf(stderr, "VirtualProtect failed with code = %ld\n", GetLastError());
		return;
	}

	PBYTE ptr = (PBYTE)addr;
	*ptr = 0xc3;
}

void patch(PVOID addr, char *original){

	PBYTE ptr = (PBYTE)addr;
	unsigned int i;
	for(i = 0; i < call_rsi_len; i++)
		original[i] = ptr[i];

	DWORD whatever;
	if(!VirtualProtect(addr, 2, PAGE_EXECUTE_READWRITE, &whatever)){
		fprintf(stderr, "VirtualProtect failed with code = %ld\n", GetLastError());
		return;
	}

	ptr[0] = 0x90;
	ptr[1] = 0x90;
}

void unpatch(PVOID addr, char *original){

	PBYTE ptr = (PBYTE)addr;

	unsigned int i;
	for(i = 0; i < call_rsi_len; i++)
		ptr[i] = original[i];

	DWORD whatever;
	if(!VirtualProtect(addr, 2, PAGE_EXECUTE, &whatever)){
		fprintf(stderr, "VirtualProtect failed with code = %ld\n", GetLastError());
	}
}

NTSTATUS WINAPI FakeLdrLoadDll( PWSTR IN SearchPath,
		PULONG IN LoadFlags,
		PUNICODE_STRING DllName,
		HMODULE *BaseAddress){

	char *original_bytes = malloc(call_rsi_len * sizeof(char));
	if(!original_bytes){
		fprintf(stderr, "calloc failed with error : %ld\n", GetLastError());
		exit(EXIT_FAILURE);
	}

	BOOL IsTargetDll = !!wcscmp(WIDE_DLL_NAME, DllName -> Buffer);

	if(!IsTargetDll)
		patch(call_rsi, original_bytes);

	// un-hook LdrLoadDll
	unhook(MyLdrLoadDll);

	// invoke the unhooked LdrLoadDll
	if(MyLdrLoadDll(SearchPath, LoadFlags, DllName, BaseAddress))
		fprintf(stderr, "I couldn't load %ls :(\n", DllName -> Buffer);

	// re-hook
	hook((PVOID)MyLdrLoadDll, FakeLdrLoadDll);

	// unpatch
	if(!IsTargetDll)
		unpatch(call_rsi, original_bytes);

	free(original_bytes);

	// return True meaning the dll was sucessfully loaded
	return TRUE;
}
