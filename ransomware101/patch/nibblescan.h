#include<stdio.h>
#include<string.h>

typedef struct {
	unsigned char low:4;
	unsigned char high:4;
} Byte;

// not my proudest function declaration
void *NibbleSigScan(void *start, size_t range, char *sig, size_t sig_len, char *mask, size_t mask_len);
