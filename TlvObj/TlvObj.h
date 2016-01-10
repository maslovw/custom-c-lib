#pragma once
#include "CustTypes.h"
#include <GlobalStruct.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* ���������, �������� � �������4 ��� ����������
 * �� �� TLV ��������
 ************************************************************************/

#pragma pack(push, 1)
	typedef struct  T_TlvHdr
	{
		uint8_t Type;	// TLV4 :: ;
		uint16_t Len;
	#	ifdef __cplusplus
		T_TlvHdr (uint8_t aType, uint16_t aLen) : Type (aType), Len (aLen) {}
	#	endif
	}T_TlvHdr;

#pragma pack(pop)

// ������� ��������� ������ ���� aType
// � �������4, ��������� ���������
T_TlvHdr *FindTlv4(uint8_t  aType);

// ������ TLV ������� � ������ 4
// apTlvObj - ������� ������
// tpValue - �������� �� ������ �������
T_TlvHdr *AddTlv4(const T_TlvHdr *apTlvObj);

//��������� ������� ��� ���������� ��� ������� 
T_TlvHdr *EditTlv4(T_TlvHdr *apTlvObj);

//�������� ������� ��� ���������� ��� ������� 
int DeleteTlv4(uint8_t aType);




#ifdef __cplusplus
}
#endif
