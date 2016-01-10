#include "stdafx.h"
#define	__FID__	233

#include "CustDefine.h"
#include <string.h>
#include "os_lib.h"
#include "memman.h"
#include "Debug.H"
#include "crc32_2.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "crc.h"
#include "MW_Misc.h"
#include "TAssert.h"
#include "SavedLog.h"


#ifndef BOOT
	#define MEMORY_CRITICAL_SECTION 1
#endif

#ifdef MEMORY_CRITICAL_SECTION
TOsMutex CriticalSectionMemMake;
int firstMemMake = 1;

#define SEMPHR_TAKE()	if((firstMemMake)|| (OsMutexTake( CriticalSectionMemMake, MUTEX_MAX_DELAY ) == OS_SUCCESS)){ 
#define SEMPHR_GIVE()	if(!firstMemMake) OsMutexRelease( CriticalSectionMemMake ); }

#else
#define SEMPHR_TAKE()
#define SEMPHR_GIVE()
#endif


unsigned int get_block_value_length(MEM_HEADER_REC *p);
void set_block_value_length(MEM_HEADER_REC *p,unsigned int a);


unsigned int get_block_use(MEM_HEADER_REC *p);
void set_block_use(MEM_HEADER_REC *p);
void clear_block_use(MEM_HEADER_REC *p);

void set_crc32(MEM_HEADER_REC *p);
int check_crc32(MEM_HEADER_REC *p);

void set_block_value_file_id(MEM_HEADER_REC *p, unsigned int a);
void set_block_value_line_id(MEM_HEADER_REC *p, unsigned int a);
const char*  get_block_value_file_id (MEM_HEADER_REC *p);
unsigned int get_block_value_line_id(MEM_HEADER_REC *p);




void memInit_CriticalSection(void)
{
	#ifdef MEMORY_CRITICAL_SECTION
	firstMemMake = 1;
	if(!CriticalSectionMemMake)
		OsMutexCreate( &CriticalSectionMemMake );
	firstMemMake = 0;
	#endif
}

//реализация функции memcreate
int memcreate(Memman_Struct *ms, s_int size, void * Pointer, const char *name)
{
	ms->gbuffer_data_length=size;
	ms->gbuffer_data= (char *) Pointer;

	ms->Name = (char *)name;
	memset(ms->gbuffer_data,0, ms->gbuffer_data_length);
	ms->WaterMark = 0xFFFFFFFF;
	ms->gbuffer_data_p = 0;
	return 0;
};



/*   C H E C K   P O I N T E R   B E L O N G S   */
/*-------------------------------------------------------------------------
    Owner: v.maslov
 
    проверка если указатель принадлежит heap'у aMS
-------------------------------------------------------------------------*/
s_int CheckPointerBelongs(Memman_Struct *aMS, void * aPointer)
{
	s_int tRet = 0;
	tRet = (((s_int)aPointer > (s_int)aMS->gbuffer_data) 
			&& ((s_int)aPointer < (s_int)aMS->gbuffer_data + (s_int)aMS->gbuffer_data_length)
			&& (check_crc32 (((MEM_HEADER_REC*)((int)aPointer - (s_int)sizeof(MEM_HEADER_REC))))));
	return tRet;
}


/*   S I Z E O F B L O C K   */
/*-------------------------------------------------------------------------
    Owner: v.maslov
 
    возвращает размер выделенной памяти
-------------------------------------------------------------------------*/
unsigned int sizeofblock(void *pv)
{
	return get_block_value_length((MEM_HEADER_REC*)((int)pv-sizeof(MEM_HEADER_REC)));  
}

void PrintMem (Memman_Struct *ms)
{
#if MEM_TEST_ON
	unsigned int pos;
	MEM_HEADER_REC * p = NULL;
	int len;

	for (pos = 0; pos < ms->gbuffer_data_p;)
	{
		p = (MEM_HEADER_REC *)((s_int)ms->gbuffer_data + pos);
		if (check_crc32 (p) == 0)
			return;
		if (p->Use)
			PDEBUG ("$%X %s\n", (uint32_t)p, p->File);

		len = get_block_value_length (p) + sizeof (MEM_HEADER_REC);	
		pos += len;
	}
#endif
}

int stat_info_new(Memman_Struct *ms)
{
#if MEM_TEST_ON
	unsigned int pos;
	MEM_HEADER_REC * p=NULL, *p_old;
	int len;
	int tLenUsed, tLenUnused;
	int tCntUnused, tCntUsed;
	int tLenBiggestUsed;
	int tLenBiggestUnused;
	int tRet = false;	
	unsigned int tFreeSpace = 0;

	tLenUsed=0;   // размер использованных блоков
	tLenUnused=0;   // размер неиспользованных блоков
	tCntUnused=0; // количество использованных блоков
	tCntUsed=0; // количество неиспользованных блоков
	tLenBiggestUsed = 0;   // максимальный выделенный блок
	tLenBiggestUnused = 0;   // максимальный свободный блок

	SEMPHR_TAKE ();
	for (pos = 0; pos < ms->gbuffer_data_p;) 
	{
		p_old=p;  
		p=(MEM_HEADER_REC *)((s_int)ms->gbuffer_data+pos);
		if(check_crc32(p)==0)
		{			
			PrintMem (ms);
			if(p_old!=NULL)
			{
				PDEBUG ("<stat_info: %s:Block Corrupt %x, FileId %s, Line %d>\r\n",
						ms->Name,
						p_old,
						(char*)p_old->File,
						p_old->Line);
				PDEBUG("<stat_info: %s:Block Corrupt %x, FileId %s, Line %d>\r\n",
					   ms->Name,
					   p, 
					   (char*)p->File,
					   p->Line);
			}
			while(1);
		}

		len = get_block_value_length (p) + sizeof (MEM_HEADER_REC);
		
		if (get_block_use(p)==0)
		{
			tLenUnused += len;
			tCntUnused++;
			if (tLenBiggestUnused < len)
				tLenBiggestUnused = len;
		}
		else
		{
			tLenUsed += len;
			tCntUsed++;
			if (tLenBiggestUsed < len)
				tLenBiggestUsed = len;
		};
		pos += len;
	};

	tFreeSpace = ms->gbuffer_data_length - tLenUsed;
	if (tFreeSpace < ms->WaterMark)
		ms->WaterMark = tFreeSpace;

	if (ms->FreeSpace != tFreeSpace)
	{
		tRet = true;

		if(ms->Name == NULL)
			ms->Name = "";

		PDEBUG ("@#%s:%6u;dt=%6d; WM:%6lu\n",
			   ms->Name, 
			   ms->FreeSpace,
			   tFreeSpace - ms->FreeSpace,
			   ms->WaterMark);
		ms->FreeSpace = tFreeSpace;
	}
	
	SEMPHR_GIVE();
	return tRet;
#else
	return false;
#endif
}

void info_block(Memman_Struct *ms, s_int pos_new)
{
	unsigned int pos;
	MEM_HEADER_REC * p;
	s_int len;

	for (pos=0; pos<ms->gbuffer_data_p;)
	{
		p=(MEM_HEADER_REC *)((s_int)ms->gbuffer_data+pos);
		len=get_block_value_length(p);
		if(pos==pos_new){
//			PDEBUG("\rBlock %x len=%d use=%d\r\n",pos,len,get_block_use(p));
		};
		pos += sizeof(MEM_HEADER_REC)+len;
	};
};


MEM_HEADER_REC * p_old;

// компактифицируются блоки
// склеивание смежных пустых блоков
void compact_free_block(Memman_Struct *ms)
{
	unsigned int pos;
	MEM_HEADER_REC * p = 0;
	int flag = 0,
		pos_old = 0, 
		len = 0, 	// суммарная длина данных (!) свободных блоков
		sc = 0;		// счетчик свободных блоков

	//PDEBUG("\r\nCompact free block\r\n");
	for (pos=0; pos<ms->gbuffer_data_p;)
	{
		p_old=p;
		p=(MEM_HEADER_REC *)((s_int)ms->gbuffer_data+pos);
		switch (flag)
		{
			// первый блок
			case 0:
				if (get_block_use(p)==0)
				{
					len=get_block_value_length(p);
					flag=1;
					pos_old=pos;
					sc=1;
				};
			break;

			// последующие блоки
			case 1:
				if (get_block_use(p)==0)
				{
					flag=1;
					len += get_block_value_length(p);
					sc++;
				}
				else
				{
					if(sc>1)
					{
						p=(MEM_HEADER_REC *)((s_int)ms->gbuffer_data+pos_old);
						set_block_value_length(p,(len+(sc-1)*sizeof(MEM_HEADER_REC)));
						clear_block_use(p);
						set_crc32(p);
						//if((pos_old+sizeof(MEM_HEADER_REC)+get_block_value_length(p))!=pos)
						//	PDEBUG("\r\nError Ptr=%06x len=%d use %d  len %d sizeof(MEM_HEADER_REC) %d\r\n",p,get_block_value_length(p),get_block_use(p), len, sizeof(MEM_HEADER_REC));

						p=(MEM_HEADER_REC *)((s_int)ms->gbuffer_data+pos);
					};
					flag=0;
					sc=0;
				};
				break;
		};
		pos += sizeof(MEM_HEADER_REC)+get_block_value_length(p);
	};
	if(sc>1)
	{
		p=(MEM_HEADER_REC *)((s_int)ms->gbuffer_data+pos_old);
		clear_block_use(p);
		set_block_value_length(p,len+(sc-1)*sizeof(MEM_HEADER_REC));
		set_crc32(p);
	}
	//PDEBUG("\r\nEnd Compact free block\r\n");
};


MEM_HEADER_REC * get_free_block(Memman_Struct *ms, s_int l1)  // передается длина, ищется максимально похожий по длине блок
{
	unsigned int pos, pos_next;
	MEM_HEADER_REC * p, *p_next;
	int len;
	char *ret1;

	for (pos=0; pos<ms->gbuffer_data_p;)
	{
		p=(MEM_HEADER_REC *)((s_int)ms->gbuffer_data+pos);
		len = get_block_value_length(p);
		if (get_block_use(p)==0)
		{
			if (len >= l1)
			{
				// нашли
				if ((len - l1) >= (sizeof(MEM_HEADER_REC) + sizeof(int)))
				{
					// делим блок так как есть место для самого маленького атома
					pos_next = pos + (sizeof(MEM_HEADER_REC) + l1);
					p_next=(MEM_HEADER_REC *)((s_int)ms->gbuffer_data + pos_next);
					clear_block_use(p_next);
					set_block_value_length(p_next, len - (l1 + sizeof(MEM_HEADER_REC)));
					set_block_value_length(p,l1);
					set_crc32(p_next);
				};
				// иначе всю длину возвращаем
				// очистить
				ret1 = (char *)((s_int)p + sizeof(MEM_HEADER_REC));
				memset_(ret1, 0, l1);
				return p;
			};
		};
		pos += sizeof(MEM_HEADER_REC) + len;
	};
	return(NULL);
};



void HeapFreeBlockCollect (Memman_Struct *apMemory)
{
	SEMPHR_TAKE ();
	compact_free_block (apMemory); // запускаем компактификацию
	SEMPHR_GIVE ();
}




//функция выделяющая память для NODE
char* pool_memmake(Memman_Struct *apMemory, s_int aLength)
{
	ASSERT (aLength <= BLOCK_VALUE_LENGTH_MASK);
	char *ret = NULL;
	s_int i, n;
	MEM_HEADER_REC * p;
	s_int length = aLength;

	SEMPHR_TAKE();

	i=0;
	n=0;
	if ( (aLength%4) != 0)
		length += (4-aLength%4);
	
	while(i!=10)
	{
		switch(i)
		{
			case 0:
				if((apMemory->gbuffer_data_p+length+sizeof(MEM_HEADER_REC))>apMemory->gbuffer_data_length)
				{
					if(n>0)
					{
						//закончилась память
						//идет либо перераспределение памяти, либо останов VM
						LOG_ADD_DATA (apMemory->Name, 4, LRT_Text);
						LOG_ADD_STR("Data pool end!");
						//stat_info_new (apMemory);     // выводим статистикику.
						ret=NULL;
						i=10;
						continue;
					};
					n=0;
					i=5;
				}
				else
					i=9;
				break;
			case 5:
				// память закончилась, следовательно, надо с самого начала искать блоки которые свободны
				//PDEBUG(" >> pool_memmake: ");
				p=get_free_block(apMemory, length);
				//PDEBUG("%x\r\n", (unsigned int)p);
				if (p!=NULL)
				{
					ret=(char *)((s_int)p+sizeof(MEM_HEADER_REC));
					set_block_use(p);
					set_crc32(p);
					i=10;
				}
				else
				{
					if(n==0)
					{
						compact_free_block(apMemory); // запускаем компактификацию
						i=5;
						n=1;
					}
					else
						i=0;
				};
				break;
			case 9:
				// ищем узел под который можно запихать ячейку
				//выделим столько сколько просили
				p=(MEM_HEADER_REC *)((s_int)apMemory->gbuffer_data+apMemory->gbuffer_data_p);
				set_block_use(p);
				set_block_value_length(p,length);
				set_crc32(p);

				ret=(char *)((s_int)apMemory->gbuffer_data+apMemory->gbuffer_data_p+sizeof(MEM_HEADER_REC));
				memset(ret, 0, aLength);
				apMemory->gbuffer_data_p += length+sizeof(MEM_HEADER_REC);
				i=10;
				break;
			default:
				i=10;			
				PDEBUG("__ERR:%s\n",__func__);
				break;
		};
	};
	SEMPHR_GIVE();
	return(ret);
};

#if MEM_TEST_ON

char* pool_memmake_ext(Memman_Struct *ms, s_int length1, unsigned int file_id, unsigned int line_id)
{
	char* ret = pool_memmake (ms, length1);
	SEMPHR_TAKE ();
	if (ret)
	{
		MEM_HEADER_REC * p;
		p=(MEM_HEADER_REC *)((s_int)ret-sizeof(MEM_HEADER_REC));

		set_block_value_file_id(p, file_id);
		set_block_value_line_id(p, line_id);	

		set_crc32(p);

		//stat_info_new(ms);
	}
	SEMPHR_GIVE ();
	return ret;
};
#endif






void pool_memfree_(Memman_Struct *ms, char *n)
{
	MEM_HEADER_REC * p;
	SEMPHR_TAKE();
	if(n!=NULL) 
	{
		p=(MEM_HEADER_REC *)((s_int)n-sizeof(MEM_HEADER_REC));
		clear_block_use(p);
		set_crc32(p);
	}
	//stat_info_new(ms);
	SEMPHR_GIVE();
};

#include "crc.h"
void set_crc32(MEM_HEADER_REC *p)
{
	unsigned int ret_crc = 0xADEA;
#if MEM_TEST_ON
	p->Crc = ret_crc;
	ret_crc = crc16_calc (ret_crc, (unsigned char*)p, sizeof(p));
#endif // MEM_TEST_ON
	p->Crc=ret_crc;
};

int check_crc32(MEM_HEADER_REC *p)
{
	unsigned int ret_crc = 0xADEA;
#if MEM_TEST_ON
	unsigned int tCrc = p->Crc;
	p->Crc = ret_crc;
	ret_crc = crc16_calc (ret_crc, (unsigned char*)p, sizeof(p));
	p->Crc = tCrc;
#endif // MEM_TEST_ON
	return (p->Crc==ret_crc);
};


unsigned int get_block_value_length(MEM_HEADER_REC *p)
{
	return p->Length;
};

void set_block_value_length(MEM_HEADER_REC *p,unsigned int a)
{
	p->Length = a & BLOCK_VALUE_LENGTH_MASK;
};


#if MEM_TEST_ON

const char* get_block_value_file_id(MEM_HEADER_REC *p)
{
	return p->File;
}

void set_block_value_file_id(MEM_HEADER_REC *p,unsigned int a)
{
	p->File = (const char*)a;
};

unsigned int get_block_value_line_id(MEM_HEADER_REC *p)
{
	return p->Line;
};

void set_block_value_line_id(MEM_HEADER_REC *p, unsigned int a)
{
	p->Line = a;
};
#endif // MEM_TEST_ON




unsigned int get_block_use(MEM_HEADER_REC *p)
{
	return p->Use;
}

void set_block_use(MEM_HEADER_REC *p)
{
	p->Use = 1;
}

void clear_block_use(MEM_HEADER_REC *p)
{
	p->Use = 0;
}
