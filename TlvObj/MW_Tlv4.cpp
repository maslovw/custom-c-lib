//************************************
// MW_Tlv4
// 21.4.2015 v.maslov
//
//************************************
#include "MW_Tlv4.h"

#	define DEF_TLV4_TYPE(X) template <>  const Tlv4::TTlv4Type Tlv4::TTlv4Id<X>::Id = Tlv4::Tlv4_##X; 

#	define TLV4_SET(type) DEF_TLV4_TYPE (type);
#	include "Tlv4Types.h"	// установление связи Тип - номер