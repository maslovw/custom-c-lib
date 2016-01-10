
#ifndef OS_HEAP
#	define OS_HEAP

#include "memman.h"

#ifdef  __cplusplus
extern  "C" {
#endif

#ifdef SYS_HEAP_C
#define SYSextern
#else
#define SYSextern extern
#endif
	

/**********************************/
/************************ VARIABLES ******************************************/	
	
	extern Memman_Struct USER_ms;
	extern Memman_Struct RTOS_ms;

	/************************ FUNCTIONS ******************************************/
	void MemoryHeapInit ();

	
	#if !MEM_TEST_ON
		void *pvPortMalloc( size_t xWantedSize );
		void vPortFree( void *pv );

		void *pvPortMalloc_ext( size_t xWantedSize );
		void vPortfree_( void *pv );
	#else

		//#ifndef __FID__
		//	#define __FID__	9999
		//#endif

	void *pvPortMalloc_debug (size_t xWantedSize, const char * file_id, const char *func_name, unsigned int line_id);
	void *pvPortMalloc_ext_debug (size_t xWantedSize, const char * file_id, const char *func_name, unsigned int line_id);

		#define	pvPortMalloc(size)			pvPortMalloc_debug		(size, __FILE__,0, __LINE__)
		#define pvPortMalloc_ext(size)		pvPortMalloc_ext_debug	(size, __FILE__,0, __LINE__)
		
		void vPortFree_debug	(void *pv, unsigned int file_id, const char *func_name, unsigned int line_id);
		#define	vPortFree(ptr)				vPortFree_debug		(ptr, 0, __func__, __LINE__)
#endif
	
#ifdef  __cplusplus
}
#endif
#endif /* OS_HEAP */

