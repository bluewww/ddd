// $Id$  -*- C++ -*-
// Dynamic array -- generates the referenced entries

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

#ifndef _DDD_DynArray_h
#define _DDD_DynArray_h

#include "assert.h"
#include "bool.h"

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
    const T& _value(int i) const
    {
	assert(i >= 0 && i < size());
	return _values[i];
    }
    T& _value(int i)
    {
	assert(i >= 0 && i < size());
	return _values[i];
    }

public:
    // Resources
    virtual int size() const   { return _allocated_size; }
    const T& operator[](int i) const { return _value(i); }
    T& operator[](int i)       { return value(i); }
    const T* values() const    { return _values; }
    T* values()                { return _values; }
#if 0
    operator const T*() const  { return values(); }
    operator       T*() const  { return values(); }
#endif

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
        for (int i = 0; i < _allocated_size; i++)
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
        // Make sure self-assignment A = A works
	if (this != &m)
	{
	    T *old_values = _values;

	    _allocated_size = m.size();
	    _values = new T [_allocated_size];

	    for (int i = 0; i < _allocated_size; i++)
		_values[i] = m._values[i];

	    delete[] old_values;
	}

	return *this;
    }

    // Comparison
    bool operator == (const DynArray<T>& m) const
    {
	if (this == &m)
	    return true;	// THIS == THIS

	if (size() != m.size())
	    return false;

	for (int i = size() - 1; i >= 0; i--)
	{
	    if (!(_values[i] == m._values[i]))
		return false;
	}

	return true;
    }

    bool operator != (const DynArray<T>& m) const
    {
	return !(operator == (m));
    }
};    

#endif // _DDD_DynArray_h
// DON'T ADD ANYTHING BEHIND THIS #endif
