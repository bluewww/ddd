// $Id$
// Variable-sized array

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the ICE Library.
// 
// The ICE Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The ICE Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the ICE Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// ICE is the incremental configuration environment.
// For details, see the ICE World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ice/',
// or send a mail to the ICE developers <ice@ips.cs.tu-bs.de>.

#ifndef _ICE_VarArray_h
#define _ICE_VarArray_h

#if defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 5)
#pragma interface
#endif

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
	value(_size) = v;	// gcc-2.3.3 chokes on having _size++ here
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
	DynArray<T>::operator = (m);
	_size = m.size();
	return *this;
    }

    // Operators
    void operator += (const T& value) { add(value); }
    void operator -= (const T& value) { remove(value); }

    T& operator[](int i) const 
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

#endif // _ICE_VarArray_h
// DON'T ADD ANYTHING BEHIND THIS #endif
