#ifndef MW_Tlv4_h__
#define MW_Tlv4_h__

#include "TlvObj.h"

#ifdef __cplusplus

// declaration of all structures
#	define TLV4_SET(type)		struct type;
#	include "Tlv4Types.h"

namespace Tlv4 
{
	// Описание типов
	enum TTlv4Type : unsigned char{
		TlvUndefined = 0,
#		define TLV4_SET(type) Tlv4_ ## type,
#		include "Tlv4Types.h"
	};
	
	// Стурктура - соответствие Тип структуры = TTlv4Type
	template <typename TValueType> struct TTlv4Id
	{ static const TTlv4Type Id; };

	// Структура, хранящаяся в памяти Сектора4
#	pragma pack(1)
	template<typename TValueType>
	struct TTlv4Obj
	{
		T_TlvHdr Hdr;
		TValueType Data;

		TTlv4Obj(const TValueType& aData) : Hdr(TTlv4Id<TValueType>::Id, sizeof (Data)), Data (aData){ }	
	};
#	pragma pack()

	// обертка функции AddTlv4
	template<typename TValueType>
	bool AddObject(const TValueType& aData)
	{
		auto tpObj = new TTlv4Obj<TValueType> (aData);
		const bool tRet = (AddTlv4 (&tpObj->Hdr) != NULL)? true : false;
		delete tpObj;
		return tRet;
	}

	// обертка функции EditTlv4
	template<typename TValueType>
	bool EditObject(const TValueType& aData)
	{
		auto tpObj = new TTlv4Obj<TValueType> (aData);
		const bool tRet = (EditTlv4 (&tpObj->Hdr) != NULL)? true : false;
		delete tpObj;
		return tRet;
	}

	// обертка функции FindTlv4
	template<typename TValueType>
	const TValueType *FindObject()
	{
		auto tpRec = (const TTlv4Obj<TValueType>*)FindTlv4 (TTlv4Id<TValueType>::Id);
		if (tpRec == NULL)
			return NULL;
			
		if (tpRec->Hdr.Len != sizeof (TValueType))
			return NULL;
		return &tpRec->Data;
	}

	// обертка функции FindTlv4
	template<typename TValueType>
	void DeleteObject()
	{
		while (NULL != FindTlv4 (TTlv4Id<TValueType>::Id))
			DeleteTlv4 (TTlv4Id<TValueType>::Id);
	}
}

#endif // __cplusplus
#endif // MW_Tlv4_h__



