#include <utility>
using namespace std;
// Описание элемента, со ссылкой на следующий
template <typename TValueType>
struct TListStruct
{
	TValueType Value;
	TListStruct *Next;
	TListStruct (const TValueType& aValue) :Value (aValue), Next (0){ }
	TListStruct (TValueType&& aValue) :Value (std::move (aValue)), Next (0){ }

	inline const TListStruct<TValueType>* GetNextElem () const { return Next; }
	inline TListStruct<TValueType>* GetNextElem () { return Next; }
};

// Описание итерратора для использования в циклах
template <typename TValueType>
struct TListIter
{
	TListIter (TListStruct<TValueType>*aElem)
	: mpCurr (aElem),
	mpNext ((mpCurr) ? mpCurr->GetNextElem () : nullptr)
	{ 	}

	TListIter& operator= (TListStruct<TValueType>*aElem)
	{
		mpCurr = aElem;
		mpNext = (mpCurr) ? mpCurr->GetNextElem () : nullptr;
		return *this;
	}

	TListIter& operator= (const TListIter<TValueType> &aIter)
	{
		mpCurr = aIter.mpCurr;
		mpNext = aIter.mpNext;
		return *this;
	}

	TListIter& operator++ ()
	{
		mpCurr = mpNext;
		mpNext = (mpCurr) ? mpCurr->GetNextElem () : nullptr;
		return *this;
	}

	bool operator!= (const TListIter& aElem) { return (this->mpCurr) != (aElem.mpCurr); }

	operator TValueType*() { if (!mpCurr)return nullptr; return &this->mpCurr->Value; }
	operator const	TValueType*()const { if (!mpCurr)return nullptr; return &this->mpCurr->Value; }

	bool operator!() const { return this->mpCurr == nullptr; }
	TListStruct<TValueType>* Get () { return this->mpCurr; }
	const TListStruct<TValueType>* Get ()const { return this->mpCurr; }
	TListStruct<TValueType>* operator-> () { return this->mpCurr; }
	void Release () { if (mpCurr) delete mpCurr; }
private:
	TListStruct<TValueType>* mpCurr;
	TListStruct<TValueType>* mpNext;
};

// Реализация методов класса TList

// контсруктор копирования
template <typename TValueType>
TList<TValueType>::TList (const  TList<TValueType>& aList) : mpHead (nullptr), mpTeil (nullptr)
{
	TListStruct<TValueType> *tpElem = aList.mpHead;
	while (tpElem)
	{
		if (!PushTeil (tpElem->Value))
			return;
		tpElem = tpElem->GetNextElem ();
	}
}

// Конструктор перемещения
template <typename TValueType>
TList<TValueType>::TList (TList<TValueType>&& aList) :
mpHead (aList.mpHead),
mpTeil (aList.mpTeil)
{
	aList.mpHead = nullptr;
	aList.mpTeil = nullptr;
}

// Конструктор с передачей константного массива
template <typename TValueType>
template <size_t aCount>
TList<TValueType>::TList (const TValueType (&apArr)[aCount], size_t aSize) : mpHead (nullptr), mpTeil (nullptr)
{
	for (uint32_t i = 0; i < MIN (aSize, aCount); ++i)
	{
		if (!PushTeil (apArr[i]))
			return;
	}
}

template <typename TValueType>
void TList<TValueType>::Apply (TListApplyConst apFunc) const
{
	const TListStruct<TValueType> *tpElem = mpHead;
	while (tpElem)
	{
		apFunc (&tpElem->Value);
		tpElem = tpElem->GetNextElem ();
	}
}

template <typename TValueType>
void TList<TValueType>::Apply (TListApplyVoid apFunc)
{
	this->Apply ((TListApplyConst)apFunc);
}

template <typename TValueType>
void TList<TValueType>::Apply (TListApply apFunc)
{
	this->Apply ((TListApplyConst)apFunc);
}

template <typename TValueType>
bool TList<TValueType>::operator!() const
{
	return (mpHead == nullptr) ? true : false;
}

template <typename TValueType>
TValueType &TList<TValueType>::operator[](uint32_t aIdx)
{
	return *(const_cast<TValueType*>(this->ElementGet (aIdx)));
}

template <typename TValueType>
const TValueType &TList<TValueType>::operator[](uint32_t aIdx) const
{
	return *(this->ElementGet (aIdx));
}

template <typename TValueType>
TList<TValueType>& TList<TValueType>::operator=(const TList<TValueType>& aList)
{
	this->Clear ();
	TListStruct<TValueType> *tpElem = aList.mpHead;
	while (tpElem)
	{
		if (!PushTeil (tpElem->Value))
			return *this;
		tpElem = tpElem->GetNextElem ();
	}
	return *this;
}

template <typename TValueType>
TList<TValueType>& TList<TValueType>::operator=(TList<TValueType>&& aList)
{
	this->Clear ();
	mpHead = aList.mpHead;
	mpTeil = aList.mpTeil;

	aList.mpHead = nullptr;
	aList.mpTeil = nullptr;

	return *this;
}

template <typename TValueType>
size_t TList<TValueType>::Count () const
{
	const TListStruct<TValueType> *tpElem = mpHead;
	size_t tCnt = 0;

	while (nullptr != tpElem)
	{
		++tCnt;
		tpElem = tpElem->Next;
	}
	return tCnt;
}

template <typename TValueType>
bool TList<TValueType>::Clear ()
{
	while (Pop (mpHead))
	{ /*do nothing */
	}

	return true;
}

template <typename TValueType>
const TValueType * TList<TValueType>::ElementGet (uint32_t aIdx) const
{
	uint32_t tIndex = 0;
	const TListStruct<TValueType> *tpElem = mpHead;

	if (!mpHead)
		return nullptr;

	for (tIndex = 0; tIndex < aIdx; ++tIndex)
	{
		tpElem = tpElem->GetNextElem ();
		if (!tpElem)
			return nullptr;
	}
	return &tpElem->Value;
}

template <typename TValueType>
const TValueType * TList<TValueType>::Element (uint32_t aIdx) const
{
	return this->ElementGet (aIdx);
}

template <typename TValueType>
TValueType * TList<TValueType>::Element (uint32_t aIdx)
{
	return const_cast<TValueType*>(this->ElementGet (aIdx));
}

template <typename TValueType>
TValueType * TList<TValueType>::Teil ()
{
	if (!mpTeil)
		return nullptr;
	return &mpTeil->Value;
}

template <typename TValueType>
TValueType * TList<TValueType>::Head ()
{
	if (!mpHead)
		return nullptr;
	return &mpHead->Value;
}

template <typename TValueType>
bool TList<TValueType>::PushVal (const TValueType& aValue, TListStruct<TValueType>* apPrev, TListStruct<TValueType>* apNext)
{
	TListStruct<TValueType> *tpNewElem = new TListStruct<TValueType> (aValue);
	return Push (tpNewElem, apPrev, apNext);
}

template <typename TValueType>
bool TList<TValueType>::PushVal (TValueType&& aValue, TListStruct<TValueType>* apPrev, TListStruct<TValueType>* apNext)
{
	TListStruct<TValueType> *tpNewElem = new TListStruct<TValueType> (std::move (aValue));
	return Push (tpNewElem, apPrev, apNext);
}

template <typename TValueType>
bool TList<TValueType>::Push (TListStruct<TValueType> *apNewElem, TListStruct<TValueType>* apPrev, TListStruct<TValueType>* apNext)
{
	if (!apNewElem)
		return false;
	apNewElem->Next = apNext;
	if (apPrev)
		apPrev->Next = apNewElem;

	if (!mpHead || mpHead == apNext)
		mpHead = apNewElem;
	if (!mpTeil || mpTeil == apPrev)
		mpTeil = apNewElem;

	return true;
}

template <typename TValueType>
bool TList<TValueType>::PushHead (const TValueType& aValue)
{
	return (PushVal (aValue, nullptr, mpHead));
}

template <typename TValueType>
bool TList<TValueType>::PushHead (TValueType&& aValue)
{
	return (PushVal (std::move (aValue), nullptr, mpHead));
}

template <typename TValueType>
bool TList<TValueType>::PushTeil (const TValueType& aValue)
{
	return (PushVal (aValue, mpTeil, nullptr));
}

template <typename TValueType>
bool TList<TValueType>::PushTeil (TValueType&& aValue)
{
	return (PushVal (std::move (aValue), mpTeil, nullptr));
}

template <typename TValueType>
bool TList<TValueType>::CompareOperation (const TValueType& aL, const TValueType& aR)
{
	return aL < aR;
}

template <typename TValueType>
bool TList<TValueType>::PushSort (const TValueType& aValue, TListOperatorCmp apFunc)
{
	if (mpTeil)
	{
		if (apFunc (mpTeil->Value, aValue))
			return PushTeil (aValue);
	}
	else
		return PushTeil (aValue);

	TListStruct<TValueType> *tpCurrent = mpHead;
	TListStruct<TValueType> *tpPrev = nullptr;

	while (tpCurrent != mpTeil)
	{
		if (apFunc (aValue, tpCurrent->Value))
			break;

		tpPrev = tpCurrent;
		tpCurrent = tpCurrent->GetNextElem ();
		if (!tpCurrent)
			return false;
	}
	return (PushVal (aValue, tpPrev, tpCurrent));
}

template <typename TValueType>
bool TList<TValueType>::Pop (TListStruct<TValueType>* apCur, TListStruct<TValueType>* apPrev)
{
	if (!apCur)
		return false;
	TListStruct<TValueType> *tpNext = apCur->GetNextElem ();
	if (apPrev)
		apPrev->Next = tpNext;
	if (mpHead == apCur)
		mpHead = tpNext;
	if (mpTeil == apCur)
		mpTeil = apPrev;
	delete apCur;
	return true;
}

template <typename TValueType>
bool TList<TValueType>::PopHead (TValueType& aValue)
{
	if (mpHead)
		aValue = mpHead->Value;
	return (this->PopHead ());
}

template <typename TValueType>
bool TList<TValueType>::PopHead ()
{
	return Pop (mpHead);
}

template <typename TValueType>
bool TList<TValueType>::PopTeil (TValueType& aValue)
{
	if (mpTeil)
		aValue = mpTeil->Value;
	return this->PopTeil ();
}

template <typename TValueType>
bool TList<TValueType>::PopTeil ()
{
	TListStruct<TValueType> *tpCur = mpHead;
	TListStruct<TValueType> *tpPrev = nullptr;
	while (tpCur != mpTeil)
	{
		tpPrev = tpCur;
		tpCur = tpCur->GetNextElem ();
	}
	return Pop (mpTeil, tpPrev);
}

template <typename TValueType>
void TList<TValueType>::Delete (const TValueType& aElem)
{
	if (!mpHead)
		return;
	TListStruct<TValueType> *tpCurrent = mpHead;
	TListStruct<TValueType> *tpPrev = nullptr;

	while ((int)&tpCurrent->Value != (int)&aElem)
	{
		if (!tpCurrent)
			return;
		tpPrev = tpCurrent;
		tpCurrent = tpCurrent->GetNextElem ();
	}
	Pop (tpCurrent, tpPrev);
}


template <typename TValueType>
void TList<TValueType>::Delete (const TValueType* apElem)
{
	if (!apElem)
		return;
	this->Delete (*apElem);
}

template <typename TValueType>
inline TListIter<TValueType> TList<TValueType>::end ()
{
	return nullptr;
}

template <typename TValueType>
inline TListIter<TValueType> TList<TValueType>::begin ()
{
	return TListIter<TValueType> (this->mpHead);
}

template <typename TValueType>
inline TListIter<TValueType> TList<TValueType>::end () const
{
	return nullptr;
}

template <typename TValueType>
inline const TListIter<TValueType> TList<TValueType>::begin () const
{
	return TListIter<TValueType> (this->mpHead);
}
