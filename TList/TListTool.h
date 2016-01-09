#ifndef TList_h__
#define TList_h__

#include "stdint.h"
/*
* Класс описывает однонаправленный связанный список.
* Список не является Thread-Safe
*/

// forward declatarion
template <typename TValueType>
struct TListIter;

template <typename TValueType>
struct TListStruct;

template <typename TValueType>
class TList{
public:
	// Стандартный конструктор
	TList () : mpHead (nullptr), mpTeil (nullptr) {}
	// Конструктор копирвоания
	TList (const  TList<TValueType>&);
	// Конструктор перемещения
	TList (TList<TValueType>&&);
	// Конструктор с передачей константного массива
	template <size_t aCount>
	TList (const TValueType (&apArr)[aCount], size_t aSize = aCount);

	~TList () { Clear (); }

	// Методы добавления в список
	bool PushHead (const TValueType& aValue);
	bool PushTeil (const TValueType& aValue);
	bool PushHead (TValueType&& aValue);
	bool PushTeil (TValueType&& aValue);

	// добавление с сортировкой
	typedef bool (*TListOperatorCmp)(const TValueType&, const TValueType&);
	bool PushSort (const TValueType& aValue, TListOperatorCmp apFunc = CompareOperation);

	// методы извлечения из списка
	bool PopHead (TValueType& aValue);
	bool PopTeil (TValueType& aValue);
	bool PopHead ();
	bool PopTeil ();

	// удаление определенного элемента
	void Delete (const TValueType& aElem);	// элемент из списка
	void Delete (const TValueType* apElem); // указатель на элемент из списка

	// Получение указателя на первый элемент
	TValueType *Head ();
	const TValueType *Head ()const;

	// Получение указателя на крайний элемент
	TValueType *Teil ();
	const TValueType *Teil ()const;

	// Получение указателя на элемент по индексу
	TValueType *Element (uint32_t aIdx);
	const TValueType *Element (uint32_t aIdx) const;

	TValueType &operator[] (uint32_t aIdx);
	const TValueType &operator[] (uint32_t aIdx) const;

	// Возвращает количество элементов
	size_t Count () const;

	// Очистка списка
	bool Clear ();

	// Применить какую либо функцию к каждому элементу списка
	typedef void (*TListApply)(TValueType *);
	void Apply (TListApply apFunc);

	typedef void (*TListApplyRef)(const TValueType &);
	void Apply (TListApplyRef apFunc);

	// Применить какую либо функцию к каждому элементу списка
	typedef void (*TListApplyConst)(const TValueType *);
	void Apply (TListApplyConst apFunc) const;

	// Применить какую либо функцию к каждому элементу списка
	typedef void (*TListApplyVoid)(void*);
	void Apply (TListApplyVoid apFunc);

	//Копирование списка
	TList<TValueType>& operator= (const TList<TValueType>&);
	//Перемещение списка
	TList<TValueType>& operator= (TList<TValueType>&&);

	bool operator!() const;

	// Методы необходимые для использования в for_each
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



