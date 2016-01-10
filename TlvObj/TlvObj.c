/************************************************************************/
/* ���� ������ � TLV ��������� � ��������� � �������4 flash  ���        */
/************************************************************************/
#include <string.h>
#include "CustTypes.h"
#include "TlvObj.h"
#include "version.h"
#include "EstrTypes.h"
#include "GlobalStruct.h"
#include "stm32f4xx.h"
#include <stm32f4xx_flash.h>

static T_TlvHdr *lpLastTlvObj = NULL;


/* ������� ��������� ������ ���� aType
 * � �������4, ��������� ���������
 */
T_TlvHdr *FindTlv4(uint8_t aType)
{
	T_TlvHdr *tpTlvRet = 0;

	lpLastTlvObj = (T_TlvHdr*)(SERIAL_NO_OFFSET //��������� ������
		+ sizeof(VuSerialNumber) + sizeof(uint16_t));  //��������� �� ������ 4 + ������� ����� ��������� + crc 
		
	while ((int)lpLastTlvObj < (int)SERIAL_NO_OFFSET + SECTOR_SERIAL_NO_SIZE)
	{
		if(lpLastTlvObj->Type == aType)
		{
			tpTlvRet = lpLastTlvObj;
		}
		else if(lpLastTlvObj->Type == 0xFF)
		{
			// ������ �����������
			return tpTlvRet;
		}

		lpLastTlvObj =  (T_TlvHdr*)((int)lpLastTlvObj + lpLastTlvObj->Len+sizeof(T_TlvHdr));
	}

	return tpTlvRet;
}

/* ������ TLV ������� � ������ 4
 * apTlvObj - ������� ������
 * tpValue - �������� �� ������ �������
 */
T_TlvHdr *AddTlv4(const T_TlvHdr *apTlvObj)
{
	uint32_t	addr, start_addr;
	uint8_t		*data, tObjType, tObjTempType;
	T_TlvHdr	*tRet = 0;

	FindTlv4(1); //���������������� ������ lpLastTlvObj

	// �������� ������������ ���������
	if (apTlvObj == 0)
		return NULL;

	//�������� ��� � ������� ������ ��� ������
	if ((lpLastTlvObj->Type != 0xFF) || 
		(lpLastTlvObj->Len != 0xFFFF))
		return NULL;
	//��������, ��� ������������ ������ �� ������� �� �������
	if ((int)lpLastTlvObj + apTlvObj->Len+sizeof(T_TlvHdr) > (int)SERIAL_NO_OFFSET + SECTOR_SERIAL_NO_SIZE)
		return NULL;

	// �������� ���� ������
	if(apTlvObj->Type >= 0x7F)
		return NULL;

	// ������� ���������� ��� = 0x7F, ����� ������������ ��� �� ������ ���
	tObjType = apTlvObj->Type;
	tObjTempType = 0x7F;

	__disable_irq ();
	FLASH_ClearFlag (	FLASH_FLAG_EOP | 
		FLASH_FLAG_OPERR | 
		FLASH_FLAG_WRPERR | 
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_PGSERR);

	FLASH_Unlock ();

	start_addr = (uint32_t)lpLastTlvObj; 

	// ��� ������ (���������) ����� ��������
	FLASH_ProgramByte (start_addr, tObjTempType);

	// ���������� ������
	data = (uint8_t *)apTlvObj + 1;
	for (addr = start_addr + 1; addr < start_addr + apTlvObj->Len+sizeof(T_TlvHdr); addr ++)
		FLASH_ProgramByte (addr, *data ++);

	// ��������� ������
	if (memcmp ((char*)lpLastTlvObj + 1, (char*)apTlvObj + 1, apTlvObj->Len+sizeof(T_TlvHdr) - 1) == 0)
	{
		tRet = lpLastTlvObj;
		lpLastTlvObj =  (T_TlvHdr*)((int)lpLastTlvObj + lpLastTlvObj->Len+sizeof(T_TlvHdr));

		//��������� ������� ���� TLV �������
		if ((FLASH_ProgramByte (start_addr, tObjType) != FLASH_COMPLETE) || 
			(tObjType != *(uint8_t*)start_addr))
			tRet = NULL;
	}

	FLASH_Lock ();
	__enable_irq ();

	return tRet;	
}

BOOL CheckDataEdit (const uint8_t *apDataDst, const uint8_t* apDataSrc, size_t aDataSize)
{
	int i;
	for (i = 0; i < aDataSize; ++i)
	{
		if (((*apDataDst) & (*apDataSrc)) != (*apDataSrc))
			return FALSE;
		apDataDst++;
		apDataSrc++;
	}
	return TRUE;
}

//��������� ������� ��� ���������� ��� ������� 
T_TlvHdr *EditTlv4(T_TlvHdr *apTlvObj)
{
	uint32_t	addr, start_addr;
	uint8_t		*data;
	volatile T_TlvHdr	*tpTlvDst;

	tpTlvDst = FindTlv4(apTlvObj->Type); //���������������� ����� � ������� ����� ������ ������

	if((tpTlvDst == 0)||(apTlvObj->Type >= 0x7F))
		return 0;

	if (FALSE == CheckDataEdit ((uint8_t *)tpTlvDst, (uint8_t *)apTlvObj, apTlvObj->Len + sizeof (T_TlvHdr)))
		return 0;

	start_addr = (uint32_t)tpTlvDst + sizeof(T_TlvHdr); 
	data = (uint8_t *)apTlvObj + sizeof(T_TlvHdr);

	__disable_irq ();
	FLASH_ClearFlag (	FLASH_FLAG_EOP | 
		FLASH_FLAG_OPERR | 
		FLASH_FLAG_WRPERR | 
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_PGSERR);
	FLASH_Unlock ();
	
	for (addr = start_addr ; addr < start_addr + tpTlvDst->Len; addr ++)
		FLASH_ProgramByte (addr, *data ++);

	FLASH_Lock ();
	__enable_irq ();

	if(memcmp((void*)tpTlvDst, apTlvObj, apTlvObj->Len+sizeof(T_TlvHdr)) == 0)
	{
		return (T_TlvHdr*)tpTlvDst;
	}

	return 0;	
}

//�������� ������� ��� ���������� ��� ������� 
int DeleteTlv4(uint8_t aType)
{
	uint32_t	start_addr;
	int			res = 0;
	T_TlvHdr	*tpTlvDst;

	tpTlvDst = FindTlv4(aType); //���������������� ����� � ������� ����� ������ ������

	if(tpTlvDst == 0)
	{
		return 0;
	}

	start_addr = (uint32_t)tpTlvDst; 

	__disable_irq ();
	FLASH_ClearFlag (	FLASH_FLAG_EOP | 
		FLASH_FLAG_OPERR | 
		FLASH_FLAG_WRPERR | 
		FLASH_FLAG_PGAERR |
		FLASH_FLAG_PGPERR |
		FLASH_FLAG_PGSERR);
	FLASH_Unlock ();

	res = FLASH_ProgramByte (start_addr, 0);
	if (0 != *(uint8_t*)start_addr)
	{
		res = FLASH_ERROR_PROGRAM;
	}
	FLASH_Lock ();
	__enable_irq ();

	return res;	
}


