// $Id$
// Variable-sized array

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_VarArray_h
#define _DDD_VarArray_h

#include "DynArray.h"


// A VarArray is assigned elements using the += operator
// and removed elements using the -= operator.
// size() returns the number of elements in the array.

template<class T>
class VarArray: public DynArray<T> {
private:
    int _size;	// number of added values

public:
    // Resources
    virtual int size() const { return _size; }

protected:
    // Add a value
    virtual void add(const T& v)
    {
        this->value(_size) = v;	// gcc-2.3.3 chokes on having _size++ here
	_size++;
    }

    // Remove an indexed value
    virtual void _remove(int n)
    {
	assert(n >= 0 && n < size());

	int sz = size();
	for (int i = n; i < sz - 1; i++)
	    this->_value(i) = this->_value(i + 1);

	_size--;
    }

    // Remove a given value
    virtual void remove(const T& v)
    {
	int i = 0;
	int sz = size();
	while (i < sz)
	{
	    if (this->_value(i) == v)
	    {
		_remove(i);
		sz = size();
	    }
	    else
		i++;
	}
    }

public:
    // Constructors
    VarArray(int initial_size = 0):
        DynArray<T>(initial_size), _size(0)
    {}

    VarArray(T *v, int n):
        DynArray<T>(v, n), _size(n)
    {}  

    // Copy constructor
    VarArray(const VarArray<T>& m):
        DynArray<T>(m), _size(m.size())
    {}

    // Destructor.
    // Omitting this one triggers an internal compiler error in GCC 2.7.2 -O2
    virtual ~VarArray()
    {}

    // Assignment
    VarArray<T>& operator = (const VarArray<T>& m)
    { 
        if (this != &m) {
	  DynArray<T>::operator = (m);
	  _size = m.size();
	}
	return *this;
    }

    // Operators
    void operator += (const T& value) { add(value); }
    void operator -= (const T& value) { remove(value); }

    const T& operator[](int i) const 
    { 
	assert(i >= 0 && i < size());
	return DynArray<T>::operator[](i); 
    }
    T& operator[](int i)       
    { 
	assert(i >= 0 && i < size());
	return DynArray<T>::operator[](i);
    }
};

#endif // _DDD_VarArray_h
// DON'T ADD ANYTHING BEHIND THIS #endif
