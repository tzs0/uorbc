#include "mod_uorbc_platform.h"


int mod_uorbc_platform_thread_create(void* id, void* (*start_rtn)(void*), void* arg)
{
	return pthread_create((pthread_t*) id, NULL, start_rtn, arg);
}


