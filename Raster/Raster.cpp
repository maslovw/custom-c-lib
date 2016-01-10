#include "Raster.h"
#include <string.h>

TRaster::TRaster (size_t aWidth, size_t aHeight) :
	mWidth (aWidth),
	mHeight (aHeight),
	mWidthByte (mWidth / 8 + ((mWidth % 8) ? 1 : 0)),
	mBufferSize (mHeight * mWidth),
	mpBuffer (new uint8_t[mBufferSize])
{
	memset (mpBuffer, 0, mBufferSize);
}

TRaster::TRaster (const TRaster& aRaster) :
	mWidth (aRaster.mWidth),
	mHeight (aRaster.mHeight),
	mWidthByte (mWidth / 8 + ((mWidth % 8) ? 1 : 0)),
	mBufferSize (aRaster.mBufferSize),
	mpBuffer (new uint8_t[mBufferSize])
{
	memcpy (mpBuffer, aRaster.mpBuffer, mBufferSize);
}

TRaster::TRaster (const uint8_t* apBuffer, size_t aWidth, size_t aHeight) :
	mWidth (aWidth),
	mHeight (aHeight),
	mWidthByte (mWidth / 8 + ((mWidth % 8) ? 1 : 0)),
	mBufferSize (mHeight * mWidth),
	mpBuffer (new uint8_t[mBufferSize])
{
	memcpy (mpBuffer, apBuffer, mBufferSize);
}

TRaster::~TRaster ()
{
	if (mpBuffer)
		delete [] mpBuffer;
}

bool TRaster::SetPixel (TCoord aX, TCoord aY)
{
	TCoord tX = aX / 8 + aY * mWidthByte;
	if (tX >= mBufferSize)
		return false;
	mpBuffer[tX] |= 1 << (7 - aX % 8);
	return true;
}

bool TRaster::ClearPixel (TCoord aX, TCoord aY)
{
	TCoord tX = aX / 8 + aY * mWidthByte;
	if (tX >= mBufferSize)
		return false;
	mpBuffer[tX] &= ~(1 << (7 - aX % 8));
	return true;
}

uint8_t TRaster::GetPixel (TCoord aX, TCoord aY)
{
	TCoord tX = aX / 8 + aY * mWidthByte;
	if (tX >= mBufferSize)
		return false;
	return (mpBuffer[tX] & (1 << (7 - aX % 8))) ? 1 : 0;
}

bool TRaster::VertLine (TCoord aX, TCoord aY, size_t aHeight)
{
	TCoord tY;
	for (tY = aY; tY < aY + aHeight; ++tY)
		if (!SetPixel (aX, tY))
			return false;
	return true;
}

void TRaster::Clean (bool aInverse/* = false*/)
{
	memset (mpBuffer, (aInverse) ? 0xFF : 0, mBufferSize);
}

void TRaster::Invers()
{
	for (uint32_t i = 0; i < mBufferSize; ++i)
		mpBuffer[i] = ~mpBuffer[i];
}
