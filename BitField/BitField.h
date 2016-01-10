#ifndef BitField_h__
#define BitField_h__

#include "stdint.h"
#include "stddef.h"

class TBitField
{
public:
	typedef unsigned long int TBitNumber;
	enum TBitValue {
		BitClear = 0,
		BitSet = 1,
		OutOfMem
	};

	TBitField (TBitNumber aFieldSize);
	TBitField (TBitField&);
	TBitField (void* apBuffer, size_t aBufferSize);
	~TBitField ();

	TBitValue SetBit      (TBitNumber aBitNum);
	TBitValue ClearBit    (TBitNumber aBitNum);
	TBitValue GetBit (TBitNumber aBitNum) const;
	TBitNumber Width () const { return mFieldSize; }		// возвращает размер битового поля
	size_t Size () const { return mBuffSize; }				// возвращает размер буфера
private:
	static const size_t mElemSize;
	const TBitNumber mFieldSize;
	const size_t mBuffSize;
	const bool mIsHeap;
	unsigned char *const mpBuffer;
};


#endif // BitField_h__



