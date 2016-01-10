#define __FID__ 0xB13F11D
#include "BitField.h"
#include "string.h"

const size_t TBitField::mElemSize = 8;

TBitField::TBitField(TBitNumber aFieldSize):
	mFieldSize (aFieldSize),
	mBuffSize (mFieldSize / mElemSize +  ((mFieldSize % mElemSize)? 1: 0)),
	mpBuffer (new uint8_t[mBuffSize]),
	mIsHeap (true)
{
	memset (mpBuffer, 0, mBuffSize);
}

TBitField::TBitField(TBitField& aField):
	mFieldSize (aField.mFieldSize),
	mBuffSize (mFieldSize / mElemSize +  ((mFieldSize % mElemSize)? 1: 0)),
	mpBuffer (new uint8_t[mBuffSize]),
	mIsHeap (true)
{
	memcpy (mpBuffer, aField.mpBuffer, mBuffSize);
}

TBitField::TBitField(void* apBuffer, size_t aBufferSize):
	mFieldSize (aBufferSize * mElemSize),
	mBuffSize (aBufferSize),
	mpBuffer (reinterpret_cast<uint8_t *> (apBuffer)),
	mIsHeap (false)
{
}

TBitField::~TBitField()
{
	if (mpBuffer && mIsHeap)
		delete [] mpBuffer;
}

TBitField::TBitValue TBitField::SetBit(TBitNumber aBitNum)
{
	if (aBitNum > mFieldSize)
		return OutOfMem;
	mpBuffer [aBitNum / mElemSize] |= 1 << (aBitNum % 8); 
	return BitSet;
}

TBitField::TBitValue TBitField::ClearBit(TBitNumber aBitNum)
{
	if (aBitNum > mFieldSize)
		return OutOfMem;
	mpBuffer [aBitNum / mElemSize] &= ~(1 << (aBitNum % 8));
	return BitClear;
}

TBitField::TBitValue TBitField::GetBit(TBitNumber aBitNum) const
{
	if (aBitNum > mFieldSize)
		return OutOfMem;
	return (mpBuffer[aBitNum / mElemSize] & (1 << (aBitNum % 8))) ? BitSet : BitClear;
}

