
#define	__FID__	80

/* Defining MPU_WRAPPERS_INCLUDED_FROM_API_FILE prevents task.h from redefining
all the API functions to use the MPU wrappers.  That should only be done when
task.h is included from an application file. */
#define MPU_WRAPPERS_INCLUDED_FROM_API_FILE

#include "FreeRTOS.h"
#include "Debug.h"
#include "CustDefine.h"
#include <String.h> 
#include "task.h"
#include "semphr.h"
#include "HW_SysTimer.h"
#include "os_lib.h"


Memman_Struct RTOS_ms;
Memman_Struct USER_ms;


#ifdef EMU
#define RTOS_HEAP_SIZE 128 * 1024
#define USER_HEAP_SIZE 512 * 1024

static unsigned char lOsHeap[RTOS_HEAP_SIZE];
static unsigned char lUsrHeap[USER_HEAP_SIZE];

static void *const lpRtosHeap = lOsHeap;
static const size_t lRtosHeapSize = RTOS_HEAP_SIZE;

static void *const lpUserHeap = lUsrHeap;
static const size_t lUserHeapSize = USER_HEAP_SIZE;
#elif defined ARM

extern void *_os_heap_begin, *_usr_heap_begin;
extern int _os_heap_size, _usr_heap_size;

static void *const lpRtosHeap = &_os_heap_begin;
static const size_t lRtosHeapSize = (size_t)(&_os_heap_size);

static void *const lpUserHeap = &_usr_heap_begin;
static const size_t lUserHeapSize= (size_t)(&_usr_heap_size);

#endif




void MemoryHeapInit ()
{
	memcreate (&RTOS_ms, lRtosHeapSize, lpRtosHeap, "RTOS_ms");			// инициализирует память во внутренней ОЗУ
	memcreate (&USER_ms, lUserHeapSize, lpUserHeap, "USER_ms");
	memInit_CriticalSection ();
}

#if !MEM_TEST_ON
 
void *pvPortMalloc( size_t xWantedSize )
{
	void *tRet = pool_memmake (&RTOS_ms, xWantedSize);
	if (!tRet)
		tRet = pool_memmake (&USER_ms, xWantedSize);
	return tRet;
}

void vPortFree (void *ptr)
{
	if (CheckPointerBelongs (&RTOS_ms, (void*)(ptr)))
		pool_memfree_ (&RTOS_ms, (char *)ptr);
	else if (CheckPointerBelongs (&USER_ms, (void*)(ptr)))
		pool_memfree_ (&USER_ms, (char *)ptr);
}

void *pvPortMalloc_ext( size_t xWantedSize )
{
	return pool_memmake (&USER_ms, xWantedSize);
}


#else	// MEM_TEST_ON

void *pvPortMalloc_debug (size_t xWantedSize, const char * file_id, const char *func_name, unsigned int line_id)
{
	void *ret;
	ret = pool_memmake_ext (&RTOS_ms, xWantedSize, (unsigned int)file_id, line_id);

#if MALLOC_DEBUG
	if (CriticalSectionDEBUG != NULL)
		if (OS_SUCCESS == OsMutexTake (CriticalSectionDEBUG, DEBUG_TIMEOUT)) 
		{
			MEM_HEADER_REC *pp = (MEM_HEADER_REC*)((int)ret - sizeof(MEM_HEADER_REC));
			printf("^Mi^%u^%s^Line^%u^Addr^0x%x^%u^%u^\n", TIMER_COUNTER, func_name, line_id, ret, xWantedSize, pp->Length);
			OsMutexRelease( CriticalSectionDEBUG );
		}
#endif

	return ret;
}

void *pvPortMalloc_ext_debug (size_t xWantedSize, const char * file_id, const char *func_name, unsigned int line_id)
{
	void *ret;
	ret = pool_memmake_ext (&USER_ms, xWantedSize, (unsigned int)file_id, line_id);

#if MALLOC_DEBUG
	if (OS_SUCCESS == OsMutexTake (CriticalSectionDEBUG, DEBUG_TIMEOUT)) 
	{
		MEM_HEADER_REC *pp = (MEM_HEADER_REC*)((int)ret - sizeof(MEM_HEADER_REC));
		printf("^Me^%u^%s^Line^%u^Addr^0x%x^%u^%u^\n", TIMER_COUNTER, func_name, line_id, ret, xWantedSize, pp->Length);
		OsMutexRelease( CriticalSectionDEBUG );
	}
#endif

	return ret;
}

void vPortFree_debug (void *ptr, unsigned int file_id, const char *func_name, unsigned int line_id)
{
#if MALLOC_DEBUG
	if (OS_SUCCESS == OsMutexTake (CriticalSectionDEBUG, DEBUG_TIMEOUT)) 
	{
		MEM_HEADER_REC *pp = (MEM_HEADER_REC*)((int)pv - sizeof(MEM_HEADER_REC));
		printf("^Fi^%u^%s^Line^%u^Addr^0x%x^S^-%u^\n", TIMER_COUNTER, func_name, line_id, pv, pp->Length);
		OsMutexRelease( CriticalSectionDEBUG );
	}
#endif

	if (CheckPointerBelongs (&RTOS_ms, (void*)(ptr)))
		pool_memfree_ (&RTOS_ms, (char *)ptr);
	else if (CheckPointerBelongs (&USER_ms, (void*)(ptr)))
		pool_memfree_ (&USER_ms, (char *)ptr);
}

#endif		// MEM_TEST_ON



