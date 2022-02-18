#include"hook.h"

void hook(PVOID *OriginalFunction, PVOID FakeFunction){
	unsigned char trampoline[] = {
				0x48, 0xb8,	// mov rax
				[2 ... 9] = 0,	// FakeFunction address
				0xff, 0xe0	// jmp rax
			};

//	*(PDWORD64)((PBYTE)trampoline + 2) = FakeFunction;
	*(PVOID *)((PBYTE)trampoline + 2) = FakeFunction;

	// save the original bytes
	memmove(original_bytes, OriginalFunction, HOOK_SIZE);

    // change memory protections
	DWORD OriginalFLags;
	if(!VirtualProtect(OriginalFunction, sizeof(trampoline), PAGE_EXECUTE_READWRITE, &OriginalFLags)){
		fprintf(stderr, "VirtualProtect failed with code = %ld\n", GetLastError());
		return;
	}

//	printf("FakeLdrLoadDll is located @ %p\n", FakeFunction);

    // place the hook
	memmove(OriginalFunction, trampoline, sizeof(trampoline));
}

void unhook(PVOID function){
    memmove(function, original_bytes, HOOK_SIZE);
}
