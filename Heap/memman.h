#ifndef _MEMMAN_H_
#define _MEMMAN_H_

#include <stdlib.h>
#include "CustDefine.h"

#ifdef  __cplusplus
extern  "C" {
#endif
typedef int s_int;

typedef struct {
	unsigned int  gbuffer_data_length;
	unsigned int  gbuffer_data_p;	//планка heap на данный момент
	char *gbuffer_data;				//указатель на блок памяти

	s_int FreeSpace;
	unsigned int WaterMark;
	const char *Name;
} Memman_Struct;

typedef struct {
	unsigned int Use	: 1;
	unsigned int Crc	: 16;		// тут хранится crc заголовка
	unsigned int Length : 15;		// длина данных в блоке
#if MEM_TEST_ON
	const char* File;
	unsigned int Line;
#endif
} MEM_HEADER_REC;


#define BLOCK_VALUE_LENGTH_MASK 0x7FFF


int memcreate(Memman_Struct *ms, s_int size, void * Pointer, const char *name);
int stat_info_new(Memman_Struct *ms);
void memInit_CriticalSection(void);

char *pool_memmake(Memman_Struct *ms, s_int length);
char* pool_memmake_ext(Memman_Struct *ms, s_int length1, unsigned int file_id, unsigned int line_id);

void pool_memfree_(Memman_Struct *ms, char *p);


int CheckPointerBelongs(Memman_Struct *aMS, void * aPointer);
unsigned int sizeofblock(void *pv);
void HeapFreeBlockCollect (Memman_Struct *apMemory);

#ifdef  __cplusplus
}
#endif

#endif
