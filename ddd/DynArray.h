// $Id$  -*- C++ -*-
// Dynamic array -- generates the referenced entries

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
// ICE is the incremental configuration engine.
// Contact ice@ips.cs.tu-bs.de for details.

#ifndef _ICE_DynArray_h
#define _ICE_DynArray_h

#if defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 5)
#pragma interface
#endif

#include "assert.h"


// A DynArray acts like an ordinary array,
// but it automatically expands to hold the referenced entry.
// Thus, if you say DynArray<int> d; d[100] = 10,
// it automatically expands to 100 elements.
// Note that a const DynArray will not grow.

// If you prefer sparse storage, see the Assoc template.

template<class T>
class DynArray {
private:
    int _allocated_size;        // size of _values
    T *_values;			// values

    static int max(int a, int b) { return (a > b) ? a : b; }

    // Grow by half the current size, at least to new_size_min
    void grow(int new_size_min = 0)
    {
	int new_size = max(_allocated_size + _allocated_size / 2 + 1, 
			   new_size_min);
	T *new_values = new T [new_size];
        for (int i = 0; i < _allocated_size; i++)
            new_values[i] = _values[i];

        delete[] _values;
        _values         = new_values;
        _allocated_size = new_size;
    }

protected:
    T& value(int i)
    {
	assert(i >= 0);
	if (i >= _allocated_size)
	    grow(i + 1);
	return _values[i];
    }
    T& _value(int i) const  
    { 
	assert(i >= 0 && i < size());
	return _values[i]; 
    }

public:
    // Resources
    virtual int size() const   { return _allocated_size; }
    T& operator[](int i) const { return _value(i); }
    T& operator[](int i)       { return value(i); }
    T* values() const          { return _values; }
    operator const T*() const  { return values(); }
    operator       T*() const  { return values(); }

    // Constructors
    DynArray(int initial_size = 0):
        _allocated_size(initial_size),
        _values(new T [initial_size])
    {}

    DynArray(T *v, int n):
        _allocated_size(n),
        _values(new T [n])
    {
        for (int i = 0; i < n; i++)
            _values[i] = v[i];
    }

    // Copy constructor
    DynArray(const DynArray<T>& m):
        _allocated_size(m.size()),
        _values(new T [m.size()])
    {
        for (int i = 0; i < m.size(); i++)
            _values[i] = m._values[i];
    }

    // Destructor
    virtual ~DynArray()
    {
	delete[] _values;
    }

    // Assignment
    DynArray<T>& operator = (const DynArray<T>& m)
    {
        // make sure a = a works
	if (this != &m)
	{
	    T *old_values = _values;

	    _allocated_size = m.size();
	    _values = new T [_allocated_size];
	    
	    for (int i = 0; i < m.size(); i++)
		_values[i] = m._values[i];

	    delete[] old_values;
	}

	return *this;
    }
};    

#endif // _ICE_DynArray_h
// DON'T ADD ANYTHING BEHIND THIS #endif
