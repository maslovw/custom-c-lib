#include "TlvObj.h"
#include "crc.h"
#include <..\..\Component\GUI\MW_StringResources.h>

#define SERIAL_NO_OFFSET_EMU 0
#define TLV4_BUFFER 512    
#define SECTOR_SERIAL_NO_SIZE_EMU lTlv4BufDataSize
#define SECT4_FILE_NAME "Memory\\SECT4.DAT"

static uint8_t *lpTlv4Buf;
static uint32_t lTlv4BufDataSize;
static T_TlvHdr *lpLastTlvObj = NULL;

typedef enum
{
	TlvInitOk,
	TlvInitError
}TlvInitResult;

TlvInitResult InitTlv4(void)
{
	FILE *tpFile;
	uint32_t tReadSize = 0;
	TlvInitResult tResult = TlvInitError;
	if (lpTlv4Buf == 0)
	{
		tpFile=fopen(SECT4_FILE_NAME, "rb");
		if (tpFile == NULL) 
			return TlvInitError;
		fseek(tpFile, 0, SEEK_END);
		fseek(tpFile, 0, SEEK_SET);
		lpTlv4Buf = (uint8_t*)malloc(TLV4_BUFFER);
		memset(lpTlv4Buf, 0xFF, TLV4_BUFFER);
		tReadSize = fread(lpTlv4Buf, 1, TLV4_BUFFER, tpFile);
		lTlv4BufDataSize = tReadSize;
		tResult = TlvInitOk;
		lpLastTlvObj = NULL;
		fclose(tpFile);
	}
	else
		tResult = TlvInitOk;
	return tResult;
}

static TlvInitResult SaveTlv4(void)
{
	FILE *tpFile;
	if (lpTlv4Buf != NULL)
	{
		tpFile=fopen(SECT4_FILE_NAME, "wb");
		if (tpFile != NULL)
		{
			fwrite(lpTlv4Buf, 1, lTlv4BufDataSize, tpFile);
			fflush(tpFile);
			fclose(tpFile);
			free(lpTlv4Buf);
			lpTlv4Buf = 0;
		}
	}
	return TlvInitOk;
}


T_TlvHdr *FindTlv4(uint8_t aType)
{
	T_TlvHdr *tpTlvRet = NULL;

	if (InitTlv4() != TlvInitOk)
		return 0;

	lpLastTlvObj = (T_TlvHdr*)(lpTlv4Buf + SERIAL_NO_OFFSET_EMU //поисковой объект
		+ sizeof(VuSerialNumber) + sizeof(uint16_t));  //указатель на сектор 4 + учетный номер тахографа + crc 
		
	while ((int)lpLastTlvObj < (int)(lpTlv4Buf + SERIAL_NO_OFFSET_EMU + SECTOR_SERIAL_NO_SIZE_EMU))
	{
		if(lpLastTlvObj->Type == (uint8_t)aType)
		{
			tpTlvRet = lpLastTlvObj;
		}
		else if(lpLastTlvObj->Type == 0xFF)
		{
			// записи закончились
			return tpTlvRet;
		}

		lpLastTlvObj =  (T_TlvHdr*)((int)lpLastTlvObj + lpLastTlvObj->Len+sizeof(T_TlvHdr));
	}
	return tpTlvRet;
}

T_TlvHdr *AddTlv4(const T_TlvHdr *apTlvObj)
{
	uint8_t		tObjType = 0;
	T_TlvHdr	*tRet = 0;

	if (InitTlv4() != TlvInitOk)
		return 0;

	FindTlv4(1); //инициализируется объект lpLastTlvObj

	//Проверка что в текущем адресе нет данных
	if(((lpLastTlvObj->Type != 0xFF) 
				&& (lpLastTlvObj->Len != 0xFFFF)
				//проверка, что записываемые данные не выходят за пределы
				&& ((int)lpLastTlvObj + apTlvObj->Len+sizeof(T_TlvHdr) < (int)SERIAL_NO_OFFSET_EMU + SECTOR_SERIAL_NO_SIZE_EMU))
		|| (apTlvObj == 0))
	{
		return tRet;
	}

	tObjType = apTlvObj->Type;

	if(tObjType>=0x7F)
		return 0;

	memcpy(lpLastTlvObj, apTlvObj, sizeof(T_TlvHdr) +  apTlvObj->Len);

	lTlv4BufDataSize += apTlvObj->Len + 3;

	tRet = lpLastTlvObj;
	lpLastTlvObj =  (T_TlvHdr*)((int)lpLastTlvObj + lpLastTlvObj->Len+sizeof(T_TlvHdr));

	SaveTlv4();

	return tRet;	
}

T_TlvHdr *EditTlv4(T_TlvHdr *apTlvObj)
{
	uint32_t	tStartAddr;
	uint8_t		*data;
	T_TlvHdr	*tpTlvDst;

	if (InitTlv4() != TlvInitOk)
		return 0;

	tpTlvDst = FindTlv4(apTlvObj->Type); //инициализируется адрес в котором нужно менять объект

	if((tpTlvDst == 0)||(apTlvObj->Type >= 0x7F))
		return 0;

	tStartAddr = (uint32_t)tpTlvDst + sizeof(T_TlvHdr); 
	data = (uint8_t *)apTlvObj + sizeof(T_TlvHdr);
	
	memcpy((void *)tStartAddr, data, sizeof(apTlvObj->Len));

	SaveTlv4();
	return tpTlvDst;	
}

int DeleteTlv4(uint8_t aType)
{
	int			tRes = 0;
	T_TlvHdr	*tpTlvDst;

	if (InitTlv4() != TlvInitOk)
		return 0;

	tpTlvDst = FindTlv4(aType); //инициализируется адрес в котором нужно менять объект

	if(tpTlvDst == 0)
	{
		return 0;
	}

	*((uint8_t *)tpTlvDst) = 0;

	SaveTlv4();

	return tRes;
}


