#ifndef TList_h__
#define TList_h__

#include "stdint.h"
/*
* ����� ��������� ���������������� ��������� ������.
* ������ �� �������� Thread-Safe
*/

// forward declatarion
template <typename TValueType>
struct TListIter;

template <typename TValueType>
struct TListStruct;

template <typename TValueType>
class TList{
public:
	// ����������� �����������
	TList () : mpHead (nullptr), mpTeil (nullptr) {}
	// ����������� �����������
	TList (const  TList<TValueType>&);
	// ����������� �����������
	TList (TList<TValueType>&&);
	// ����������� � ��������� ������������ �������
	template <size_t aCount>
	TList (const TValueType (&apArr)[aCount], size_t aSize = aCount);

	~TList () { Clear (); }

	// ������ ���������� � ������
	bool PushHead (const TValueType& aValue);
	bool PushTeil (const TValueType& aValue);
	bool PushHead (TValueType&& aValue);
	bool PushTeil (TValueType&& aValue);

	// ���������� � �����������
	typedef bool (*TListOperatorCmp)(const TValueType&, const TValueType&);
	bool PushSort (const TValueType& aValue, TListOperatorCmp apFunc = CompareOperation);

	// ������ ���������� �� ������
	bool PopHead (TValueType& aValue);
	bool PopTeil (TValueType& aValue);
	bool PopHead ();
	bool PopTeil ();

	// �������� ������������� ��������
	void Delete (const TValueType& aElem);	// ������� �� ������
	void Delete (const TValueType* apElem); // ��������� �� ������� �� ������

	// ��������� ��������� �� ������ �������
	TValueType *Head ();
	const TValueType *Head ()const;

	// ��������� ��������� �� ������� �������
	TValueType *Teil ();
	const TValueType *Teil ()const;

	// ��������� ��������� �� ������� �� �������
	TValueType *Element (uint32_t aIdx);
	const TValueType *Element (uint32_t aIdx) const;

	TValueType &operator[] (uint32_t aIdx);
	const TValueType &operator[] (uint32_t aIdx) const;

	// ���������� ���������� ���������
	size_t Count () const;

	// ������� ������
	bool Clear ();

	// ��������� ����� ���� ������� � ������� �������� ������
	typedef void (*TListApply)(TValueType *);
	void Apply (TListApply apFunc);

	typedef void (*TListApplyRef)(const TValueType &);
	void Apply (TListApplyRef apFunc);

	// ��������� ����� ���� ������� � ������� �������� ������
	typedef void (*TListApplyConst)(const TValueType *);
	void Apply (TListApplyConst apFunc) const;

	// ��������� ����� ���� ������� � ������� �������� ������
	typedef void (*TListApplyVoid)(void*);
	void Apply (TListApplyVoid apFunc);

	//����������� ������
	TList<TValueType>& operator= (const TList<TValueType>&);
	//����������� ������
	TList<TValueType>& operator= (TList<TValueType>&&);

	bool operator!() const;

	// ������ ����������� ��� ������������� � for_each
	TListIter<TValueType> begin ();
	TListIter<TValueType> end ();

	const TListIter<TValueType> begin () const;
	TListIter<TValueType> end ()const;
private:
	static bool CompareOperation (const TValueType& aL, const TValueType& aR);
	bool PushVal (const TValueType& aValue, TListStruct<TValueType>* apPrev, TListStruct<TValueType>* apNext);
	bool PushVal (TValueType&& aValue, TListStruct<TValueType>* apPrev, TListStruct<TValueType>* apNext);
	bool Push (TListStruct<TValueType> *apVal, TListStruct<TValueType>* apPrev, TListStruct<TValueType>* apNext);

	bool Pop (TListStruct<TValueType>* apPrev, TListStruct<TValueType>* apNext = nullptr);
	const TValueType *ElementGet (uint32_t aIdx) const;
	TListStruct<TValueType>* mpHead;
	TListStruct<TValueType>* mpTeil;
};


#include "TListToolImplement.h"

bool ListTest ();

#endif // TList_h__



