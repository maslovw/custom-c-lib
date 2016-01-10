//AKSmartPtr.h
// v.1.0.0.0

#ifndef __AKSMARTPTR_H__
	#define __AKSMARTPTR_H__

	/********************************
	 **        CAK_RefCounter      **
	 ********************************/
	class CAK_RefCounter;	

	/********************************
	 **         CAK_SmartPtr       **
	 ********************************/
	template <typename T> class CAK_SmartPtr
	{
	public:
		CAK_SmartPtr ();
		CAK_SmartPtr (T* apValue);		 
		template <typename T2> CAK_SmartPtr (T2* apValue);		 
		CAK_SmartPtr (const CAK_SmartPtr<T>& aSP); //Copy constructor
		CAK_SmartPtr (CAK_SmartPtr<T>&& aSP); //Move constructor
		template <typename T2> CAK_SmartPtr (const CAK_SmartPtr<T2>& aSP); //Copy constructor
		template <typename T2> CAK_SmartPtr (CAK_SmartPtr<T2>&& aSP); //Move constructor
		~CAK_SmartPtr ();				  // Destructor
		T& operator* ();
		T* operator-> ();
		CAK_SmartPtr<T>& operator = (const T* apValue); // Assignment operator
		template <typename T2> CAK_SmartPtr<T>& operator = (const T2* apValue);		 
		CAK_SmartPtr<T>& operator = (const CAK_SmartPtr<T>& aSP); // Assignment operator
		template <typename T2> CAK_SmartPtr<T>& operator = (const CAK_SmartPtr<T2>& aSP);
		bool operator == (const CAK_SmartPtr<T>& aSP) const;
		bool operator == (const T* const aP) const;
		template <typename T2> bool operator == (const CAK_SmartPtr<T2>& aSP) const;
		bool operator != (const CAK_SmartPtr<T>& aSP) const;
		template <typename T2> bool operator != (const CAK_SmartPtr<T2>& aSP) const;
		bool operator != (const T* const aP) const;
		bool IsNull (void) const;
		inline bool operator! () const { return IsNull(); }
		inline operator T* () { return fpData; }
		inline operator const T* () const { return fpData; }
		unsigned int Size ()const { return sizeof (T); }
		inline T* Get () { return fpData; }
		inline const T* Get ()const { return fpData; }
	private:
		T*			   fpData;		   //pointer
		CAK_RefCounter*  fpRC;			 //Reference count
		void Release ();
	};

	class CAK_RefCounter
	{
	public:
		CAK_RefCounter () : fRefCount (1) {}                 //Constructor
		inline void AddRef (void) { ++fRefCount; };          //Increment the reference count
		inline int Release (void) { return --fRefCount; }    //Decrement the reference count and return the reference count
	private:
		int fRefCount;                     //Reference count
	};

	template <typename T>
	void CAK_SmartPtr<T>::Release ()
	{
		if ((fpRC != nullptr) && fpRC->Release () == 0)             //Decrement the reference count if reference become zero delete the data
		{
			delete fpRC;
			fpRC = nullptr;
			if (fpData)
				delete fpData;
		}
	}


	//-----------------------------
	template <typename T>
	CAK_SmartPtr<T>::CAK_SmartPtr () : fpData (NULL), fpRC (NULL)
	{
	}


	//-----------------------------
	template <typename T>
	CAK_SmartPtr<T>::CAK_SmartPtr (T* apValue) : fpData (apValue), fpRC (new CAK_RefCounter ())
	{
	}


	//-----------------------------
	template <typename T>  template <typename T2>
	CAK_SmartPtr<T>::CAK_SmartPtr (T2* apValue) : fpData ((T*)apValue), fpRC (new CAK_RefCounter ())
	{
	}


	//-----------------------------
	template <typename T>
	CAK_SmartPtr<T>::CAK_SmartPtr (const CAK_SmartPtr<T>& aSP) : fpData (aSP.fpData), fpRC (aSP.fpRC)
	{
		fpRC->AddRef ();                       //Copy the data and reference pointer and increment the reference count
	}


	//-----------------------------
	template <typename T> template <typename T2>
	CAK_SmartPtr<T>::CAK_SmartPtr (const CAK_SmartPtr<T2>& aSP) : fpData ((T*)(aSP.fpData)), fpRC (aSP.fpRC)
	{
		fpRC->AddRef ();                       //Copy the data and reference pointer and increment the reference count
	}


	//-----------------------------
	template <typename T>
	CAK_SmartPtr<T>::~CAK_SmartPtr ()
	{
		Release ();
	}


	//-----------------------------
	template <typename T>
	T& CAK_SmartPtr<T>::operator* ()
	{
		return *fpData;
	}


	//-----------------------------
	template <typename T>
	T* CAK_SmartPtr<T>::operator-> ()
	{
		return fpData;
	}


	//-----------------------------
	template <typename T>
	CAK_SmartPtr<T>& CAK_SmartPtr<T>::operator = (const T* apValue)
	{
		if (fpData != apValue)             //Avoid self assignment
		{
			Release ();
			fpRC = new CAK_RefCounter ();          //Create a new reference 
			fpData = (T*)apValue;
		}
		return *this;
	}


	template <typename T> template <typename T2>
	CAK_SmartPtr<T>& CAK_SmartPtr<T>::operator = (const T2* apValue)
	{
		if (fpData != (T*)apValue)         //Avoid self assignment
		{
			Release ();
			fpRC = new CAK_RefCounter ();          //Create a new reference 
			fpData = (T*)apValue;
		}
		return *this;
	}


	//-----------------------------
	template <typename T>
	CAK_SmartPtr<T>& CAK_SmartPtr<T>::operator = (const CAK_SmartPtr<T>& aSP)
	{
		if (this != &aSP)                      //Avoid self assignment
		{
			Release ();
			fpData = aSP.fpData;
			fpRC = aSP.fpRC;
			fpRC->AddRef ();
		}
		return *this;
	}


	//-----------------------------
	template <typename T>
	bool CAK_SmartPtr<T>::operator == (const CAK_SmartPtr<T>& aSP) const
	{
		return ((fpData == aSP.fpData) && (fpRC == aSP.fpRC));
	}


	//-----------------------------
	template <typename T> template <typename T2>
	bool CAK_SmartPtr<T>::operator == (const CAK_SmartPtr<T2>& aSP) const
	{
		return ((fpData == aSP.fpData) && (fpRC == aSP.fpRC));
	}


	//-----------------------------
	template <typename T>
	bool CAK_SmartPtr<T>::operator == (const T* const aP) const
	{
		return (fpData == aP);
	}


	//-----------------------------
	template <typename T>
	bool CAK_SmartPtr<T>::operator != (const CAK_SmartPtr<T>& aSP) const
	{
		return ((fpData != aSP.fpData) || (fpRC != aSP.fpRC));
	}


	//-----------------------------
	template <typename T> template <typename T2>
	bool CAK_SmartPtr<T>::operator != (const CAK_SmartPtr<T2>& aSP) const
	{
		return ((fpData != aSP.fpData) || (fpRC != aSP.fpRC));
	}


	//-----------------------------
	template <typename T>
	bool CAK_SmartPtr<T>::operator != (const T* const aP) const
	{
		return (fpData != aP);
	}
	
	//-----------------------------
	template <typename T> template <typename T2>
	CAK_SmartPtr<T>& CAK_SmartPtr<T>::operator = (const CAK_SmartPtr<T2>& aSP)
	{
		if (fpData != (T*)aSP.fpData) //Avoid self assignment
		{
			Release ();
			fpData = (T*)aSP.fpData; //fpData = static_cast <T*> aSP.pData;
			fpRC = aSP.fpRC;
			fpRC->AddRef ();
		}
		return *this;
	}


	//-----------------------------
	template <typename T>
        bool CAK_SmartPtr<T>::IsNull (void) const
	{
		return (fpData == NULL);
	}

#endif
