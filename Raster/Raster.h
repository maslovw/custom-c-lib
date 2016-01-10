#pragma once
#include "stdint.h"
#include "stddef.h"

class TRaster
{
public:
	typedef size_t TCoord;
	TRaster (size_t aWidth, size_t aHeight);
	TRaster (const TRaster&);
	TRaster (const uint8_t*apBuffer, size_t aWidth, size_t aHeight);
	~TRaster ();
	const uint8_t* GetRaster () { return mpBuffer; }
	bool SetPixel (TCoord aX, TCoord aY);
	bool ClearPixel (TCoord aX, TCoord aY);
	uint8_t GetPixel (TCoord aX, TCoord aY);
	TCoord Width () { return mWidth; }
	TCoord WidthByte () { return mWidthByte; }
	TCoord Height () { return mHeight; }

	bool VertLine (TCoord aX, TCoord aY, size_t aHeight);
	void Clean (bool aInverse = false);
	void Invers ();
private:
	const size_t mWidth;
	const size_t mHeight;
	const size_t mWidthByte;
	const size_t mBufferSize;
	uint8_t *const mpBuffer;
};

