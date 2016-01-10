#pragma once
#include "CustTypes.h"
#include <GlobalStruct.h>

#ifdef __cplusplus
extern "C" {
#endif

/************************************************************************/
/* Структуры, хранимые в Секторе4 ПЗУ процессора
 * всё на TLV объектах
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

// Находит последнюю запись типа aType
// в секторе4, возращает указатель
T_TlvHdr *FindTlv4(uint8_t  aType);

// Запись TLV объекта в сектор 4
// apTlvObj - базовый объект
// tpValue - укзатель на данные объекта
T_TlvHdr *AddTlv4(const T_TlvHdr *apTlvObj);

//Изменение объекта без расширения его размера 
T_TlvHdr *EditTlv4(T_TlvHdr *apTlvObj);

//Удаление объекта без расширения его размера 
int DeleteTlv4(uint8_t aType);




#ifdef __cplusplus
}
#endif
