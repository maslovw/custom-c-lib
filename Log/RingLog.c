#include "RingLog.h"
#include "TAssert.h"
#include <string.h>
#include <SYS_MemRetarget.h>
#include <FreeRTOS.h>
#include <task.h>
#include "GlobalStruct.h"
#include "..\..\Tool\EMU\EmuUtils.h"


char lmSaveLogExtStr[] = "SLog:\t";

void RingLogInit(TRingLog *apLog, void *apBuff, unsigned int  aBuffSize, const uint32_t *apTimeNow)
{
	ASSERT (apBuff != NULL);

	apLog->Hdr = (TRingLogHeader *) apBuff;
	apLog->Buff = (unsigned char *)((int)apBuff + sizeof (TRingLogHeader));
	apLog->BuffSize = aBuffSize - sizeof (TRingLogHeader);
	apLog->pTimeNow = apTimeNow;
}

void ReadRing (TRingLog *apLog, TOffset aOffset, void *apData, unsigned int  aDataSize)
{
	if (aOffset >= apLog->BuffSize)
		aOffset %= apLog->BuffSize;

	if (apLog->BuffSize < (unsigned int)aOffset + aDataSize)
	{
		memcpy (apData, &apLog->Buff[aOffset], apLog->BuffSize - aOffset);
		memcpy ((void *)((int)apData + (apLog->BuffSize - aOffset)), apLog->Buff, aDataSize - (apLog->BuffSize - aOffset));
	}
	else
		memcpy (apData, &apLog->Buff[aOffset], aDataSize);
}

static void WriteRing (TRingLog *apLog, TOffset aOffset, const void *const apData, unsigned int  aDataSize)
{
	if (aOffset >= apLog->BuffSize)
		aOffset %= apLog->BuffSize;

	if (apLog->BuffSize < (unsigned int)aOffset + aDataSize)
	{
		memcpy (&apLog->Buff[aOffset], apData, apLog->BuffSize - aOffset);
		memcpy (apLog->Buff, (void *)((int)apData + (apLog->BuffSize - aOffset)), aDataSize - (apLog->BuffSize - aOffset));
	}
	else
		memcpy (&apLog->Buff[aOffset], apData, aDataSize);
}

static void UpdateRecHdr (TRingLog *apLog, 
						  TRingLogRecHdr *apRecHdr, 
						  const char *const apTaskName, 
						  unsigned int aFid, 
						  unsigned int aLine, 
						  unsigned int aDataSize,
						  unsigned int aStoreId,
						  enum TLogRecType aType)
{
	apRecHdr->Type = aType;
	apRecHdr->DateTime = *apLog->pTimeNow;
	apRecHdr->Fid = aFid;
	apRecHdr->Line = aLine;
	memcpy (&apRecHdr->TaskName, apTaskName, sizeof (apRecHdr->TaskName));
	apRecHdr->Len = aDataSize + sizeof (*apRecHdr);
	apRecHdr->StoreId = aStoreId;
}

//************************************
// Method:    UpdateOldestRec
// FullName:  UpdateOldestRec
// Returns:   void
// Parameter: TRingLog * apLog
// Parameter: unsigned int aDataSize
// Descript:  Находит первую запись, которая не будет
//			  перекрыта записываемой записью
// UnitTest:  -
//************************************
static void UpdateOldestRec (TRingLog *apLog, unsigned int aDataSize)
{
	TRingLogRecHdr tOldRecHdr;
	unsigned int   tOldest = (unsigned int)apLog->Hdr->Oldest;
	//Располгаем файл в линию
	if (tOldest < apLog->Hdr->Teil)
		tOldest += apLog->BuffSize;

	while (tOldest < apLog->Hdr->Teil + aDataSize + sizeof (TRingLogRecHdr))
	{
		ReadRing (apLog, tOldest, &tOldRecHdr, sizeof (tOldRecHdr));
		if (tOldRecHdr.Len == 0)
			return;	
		tOldest += tOldRecHdr.Len;
	}
	//выравниваем указатель
	tOldest %= apLog->BuffSize;
	//Запись 
	apLog->Hdr->Oldest = (TOffset)tOldest;
}

//************************************
// Method:    RingLogAppend
// FullName:  RingLogAppend
// Returns:   void
// Parameter: TRingLog * apLog
// Parameter: unsigned int aFid - Номер файла
// Parameter: unsigned int aLine - номер строки
// Parameter: const void * const apData
// Parameter: unsigned int aDataSize
// Descript:  Добавление apData в лог
// UnitTest:  +
//************************************
void RingLogAppend(TRingLog *apLog, 
				   unsigned int aFid, 
				   unsigned int aLine, 
				   const void *const apData, 
				   unsigned int  aDataSize,
				   enum TLogRecType aType)
{
	TRingLogRecHdr tRecHdr;
	PDEBUG("%s%s\n", lmSaveLogExtStr, (const char *)apData);
	UpdateRecHdr (apLog, &tRecHdr, pcTaskGetTaskName (NULL), aFid, aLine, aDataSize, *gp_SavedData.StorageRecId, aType);
	UpdateOldestRec (apLog, aDataSize);

	WriteRing (apLog, apLog->Hdr->Teil, &tRecHdr, sizeof (tRecHdr));
	WriteRing (apLog, apLog->Hdr->Teil + sizeof (tRecHdr), apData, aDataSize);
	apLog->Hdr->Teil += (sizeof (tRecHdr) + aDataSize);
	apLog->Hdr->Teil %= apLog->BuffSize;
	SAVE_SRAM();
}

#if UNIT_TESTS

#include "GlobalStruct.h"

void RingLogTest(void)
{
	TRingLog tLog;
	// 	char *tpBuff = (char *)malloc_ext (RING_BUFF_SIZE);
	// 	tpBuff[100] = 0xFF;
	RingLogInit (&tLog, gp_SavedData.RingLog/*tpBuff*/, sizeof (* gp_SavedData.RingLog), gp_SavedData.timestamp);
	RingLogAppend (&tLog, 1, 2, "Hello", sizeof("Hello"));
	RingLogAppend (&tLog, 1, 2, "Hello1", sizeof("Hello1"));
	RingLogAppend (&tLog, 1, 2, "Hello2", sizeof("Hello2"));
	RingLogAppend (&tLog, 1, 2, "Hello3", sizeof("Hello3"));

	/*	free_ (tpBuff);*/
}  
#endif // UNIT_TESTS

