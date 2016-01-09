//************************************
// List
// 26.11.2014 v.maslov
//
//************************************
#include "TListTool.h"

#ifndef _countof
#define _countof(arr)		(sizeof(arr)/sizeof((arr)[0]))
#endif

#ifndef LOBYTE
#define LOBYTE(n) ((n)&0xFF)
#endif
#ifndef HIBYTE
#define HIBYTE(n) LOBYTE((n)>>8)
#endif
#ifndef LOWORD
#define LOWORD(n) ((n)&0xFFFF)
#endif
#ifndef HIWORD
#define HIWORD(n) LOWORD((n)>>16)
#endif

#ifndef SWAP16
#define SWAP16(n) ((LOBYTE(n)<<8)|HIBYTE(n))
#endif
#ifndef SWAP32
#define SWAP32(n) ((SWAP16(LOWORD(n))<<16)|SWAP16(HIWORD(n)))
#endif

#ifndef swap_16
#define swap_16(n) n=SWAP16(n)
#endif
#ifndef swap_32
#define swap_32(n) n=SWAP32(n)
#endif

#if !UNIT_TESTS

static uint16_t lpTestTableSrc1[] = { 5, 4, 3, 2, 1 };
static uint16_t lpTestTableSrc2[] = { 6, 7, 8, 9, 10 };
static uint16_t lpTestTableSrc3[] = { 1, 2, 3, 4, 5 };
static uint16_t lpTestTableSrc4[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
static uint16_t lpTestTableSrc5[] = { 5, 2, 8, 3, 1, 4, 10, 6, 7, 9 };

uint16_t GetU16Swap (uint16_t aValue)
{
	return SWAP16 (aValue);
}

void U16Swap (uint16_t* aValue)
{
	swap_16 (*aValue);
}

template <typename T, size_t aSize>
static bool ConstListTest (const TList<T>& aList, T (&apTable)[aSize])
{
	uint32_t tErr = 0;
	const uint16_t* tpData = apTable;
	for (size_t i = 0; i < _countof (apTable); ++i)
	{
		if (*aList.Element (i) != tpData[i])
			++ tErr;
	}

	for (size_t i = 0; i < _countof (lpTestTableSrc1); ++i)
	{
		if (aList[i] != tpData[i])
			++ tErr;
	}
	return tErr == 0;
}

template <typename T, size_t aSize>
static bool Compare (TList<T>& aList, T (&apTable)[aSize])
{
	const T* tpData = apTable;
	const T* tpDataEnd = (const T*)((int)apTable + sizeof(apTable));
	uint32_t tErr = 0;
	for (const auto& tElem : aList)
	{
		if (*tpData != tElem)
			++tErr;
		++tpData;
	}
	if ((int)tpDataEnd != (int)tpData)
		++tErr;
	return (tErr == 0);
}

static bool ListFromArrayTest ()
{
	int tErr = 0;
	TList<uint16_t> tList (lpTestTableSrc1);
	if (!Compare (tList, lpTestTableSrc1))
		++tErr;
	if (tList.Count () != _countof (lpTestTableSrc1))
		++tErr;
	tList.PushHead (55);
	if (Compare (tList, lpTestTableSrc1))
		++tErr;
	tList = lpTestTableSrc2;
	if (!Compare (tList, lpTestTableSrc2))
		++tErr;
	return (tErr == 0);
}

static bool ListPushTest ()
{
	int tErr = 0;
	TList<uint16_t> tList;
	for (const auto& tElem : lpTestTableSrc1)
	{
		if (!tList.PushHead (tElem))
			++ tErr;
	}
	if (!Compare (tList, lpTestTableSrc3))
		++tErr;



	return (tErr == 0);
}

static bool ListPushSortTest ()
{
	int tErr = 0;
	TList<uint16_t> tList;
	for (const auto& tElem : lpTestTableSrc5)
	{
		if (!tList.PushSort (tElem))
			++ tErr;
	}
	if (!Compare (tList, lpTestTableSrc4))
		++tErr;

	if (!tList.PushSort (0))
		++ tErr;
	if (Compare (tList, lpTestTableSrc4))
		++tErr;
	if (*(tList.Head ()) != 0)
		++tErr;
	if (!tList.PushSort (20))
		++ tErr;
	if (*(tList.Teil ()) != 20)
		++tErr;

	tList.Clear ();
	for (const auto& tElem : lpTestTableSrc1)
	{
		if (!tList.PushSort (tElem))
			++ tErr;
	}
	if (!Compare (tList, lpTestTableSrc3))
		++tErr;


	tList.Clear ();
	for (const auto& tElem : lpTestTableSrc1)
	{
		if (!tList.PushSort (tElem, [](const uint16_t& aL, const uint16_t&aR){ return (aL > aR); }))
			++ tErr;
	}
	if (!Compare (tList, lpTestTableSrc1))
		++tErr;

	return (tErr == 0);
}

static bool ListElementTest ()
{
	int tErr = 0;
	TList<uint16_t> tList = lpTestTableSrc3;
	const uint16_t* tpData = lpTestTableSrc3;
	for (size_t i = 0; i < _countof (lpTestTableSrc3); ++i)
	{
		if (*tList.Element (i) != tpData[i])
			++ tErr;
	}

	for (size_t i = 0; i < _countof (lpTestTableSrc3); ++i)
	{
		if ((tList[i]) != tpData[i])
			++ tErr;
	}
	tList.Clear ();
	if (tList.Count ())
		++ tErr;

	tList = lpTestTableSrc4;
	for (const auto& tTableEl : lpTestTableSrc4)
	{
		uint16_t  tElem;
		if (!tList.PopHead (tElem))
			++ tErr;
		if (tElem != tTableEl)
			++tErr;
	}
	if (tList.Count ())
		++ tErr;

	tList = lpTestTableSrc1;
	for (const auto& tTableEl : lpTestTableSrc3)
	{
		uint16_t  tElem;
		if (!tList.PopTeil (tElem))
			++ tErr;
		if (tElem != tTableEl)
			++tErr;
	}
	if (tList.Count ())
		++ tErr;

	return (tErr == 0);
}

static bool ListDeleteTest ()
{
	int tErr = 0;
	TList<uint16_t> tList (lpTestTableSrc4);
	for (const auto &tElem : tList)
	{
		if (tElem < lpTestTableSrc2[0])
			tList.Delete (tElem);
	}
	if (!Compare (tList, lpTestTableSrc2))
		++tErr;

	tList = lpTestTableSrc4;
	for (const auto &tElem : tList)
	{
		if (tElem >= lpTestTableSrc2[0])
			tList.Delete (tElem);
	}
	if (!Compare (tList, lpTestTableSrc3))
		++tErr;

	for (const auto &tElem : tList)
	{
		tList.Delete (tElem);
	}
	if (tList.Count ())
		++tErr;
	if (!tList != false)
		++tErr;

	tList = lpTestTableSrc3;
	for (const auto &tElem : tList)
	{
		if (tElem == lpTestTableSrc3[2])
			tList.Delete (tElem);
	}
	if (Compare (tList, lpTestTableSrc3))
		++tErr;
	tList.PushSort (lpTestTableSrc3[2]);

	if (!Compare (tList, lpTestTableSrc3))
		++tErr;
	return (tErr == 0);
}

static bool ListApplyTest ()
{
	int tErr = 0;
	TList<uint16_t> tList = lpTestTableSrc3;
	tList.Apply ([](uint16_t *aVal) { *aVal = SWAP16 (*aVal); });
	auto tListElem = tList.begin ();
	for (const auto& tElem : lpTestTableSrc3)
	{
		if (SWAP16 (tElem) != *(tListElem))
			++ tErr;
		++tListElem;
	}

	tList.Apply ([](void *aVal) { *(uint16_t*)aVal = SWAP16 (*(uint16_t*)aVal); });
	tListElem = tList.begin ();
	for (const auto& tElem : lpTestTableSrc3)
	{
		if (tElem != *(tListElem))
			++ tErr;
		++tListElem;
	}

	const auto tConstList = tList;
	tConstList.Apply ([](const uint16_t *aVal) { });
	tListElem = tConstList.begin ();
	for (const auto& tElem : lpTestTableSrc3)
	{
		if ((tElem) != *(tListElem))
			++ tErr;
		++tListElem;
	}

	return (tErr == 0);
}

bool ListTest ()
{
	uint32_t tErr = 0;

	if (!ListFromArrayTest ())
		++tErr;

	if (!ListPushTest ())
		++tErr;

	if (!ListPushSortTest ())
		++tErr;

	if (!ListElementTest ())
		++tErr;

	if (!ListDeleteTest ())
		++tErr;

	if (! ListApplyTest ())
		++tErr;

	if (!ConstListTest (TList<uint16_t> (lpTestTableSrc1), lpTestTableSrc1))
		++ tErr;


	return (0 == tErr);
}

#endif // UNIT_TESTS
