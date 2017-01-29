#include <stdio.h>
#include <sys/syscall.h>

#define EUDYPTULA_SYSCALL_ID 329

static inline int make_syscall(unsigned int high_id, unsigned int low_id)
{
	return syscall(EUDYPTULA_SYSCALL_ID, high_id, low_id);
}

int main(int argc, char *argv[])
{
	printf("*******************************************************************\n");
	printf("This is the test program for eudyptula's task15 about syscalls...\n\n");
	printf("* Calling syscall with wrong parameters (0xa, 0xc)\n");
	printf("\t - Syscall retuns: %d\n", make_syscall(0xa, 0xc));
	// My eudyptula id (just to don't forget it): a8a3ead89815
	printf("* Calling syscall with good parameters (0xa8a3, 0xead89815)\n");
	printf("\t - Syscall returns: %d\n", make_syscall(0xa8a3, 0xead89815));
	printf("\nTest ended.\n");
	printf("*******************************************************************\n");

	return 0;
}
