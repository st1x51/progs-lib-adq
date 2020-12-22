#include <pspkernel.h>
#include <stdio.h>

PSP_MODULE_INFO("progs", PSP_MODULE_USER, 1, 0);
/*void Com_Printf(char* msg, ...);*/

int __psp_free_heap(void);
int module_start( SceSize arglen, void *argp )
{
	return 0;
}

int module_stop( SceSize arglen, void *argp )
{
	/*Com_Printf("PRX Stopped\n");*/
	__psp_free_heap(); //libc heap
	return 0;
}