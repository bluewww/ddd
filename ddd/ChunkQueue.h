// $Id$
// Chunk buffer

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

#ifndef _DDD_ChunkQueue_h
#define _DDD_ChunkQueue_h

#include <stdio.h>
#include <string.h>
#include "TypeInfo.h"
#include "assert.h"

#if HAVE_MEMCPY && !HAVE_MEMCPY_DECL
extern "C" void *memcpy(void *to, const void *from, size_t size);
#endif

/*
    ChunkQueue is a character array that grows dynamically.
    Characters can easily be appended at the end and
    removed from the beginning (hence the name "queue").
*/

class ChunkQueue {
public:
    DECLARE_TYPE_INFO

private:
    int _length;	// length of valid data
    int _size;		// size of allocated memory
    char *_data;	// data

    // Grow to specific size
    void grow(int newSize)
    {
	_size = newSize;
	char *newData = new char[_size];
	memcpy(newData, _data, _length);
	delete[] _data;
	_data = newData;
    }

    ChunkQueue(const ChunkQueue&);

    ChunkQueue& operator = (const ChunkQueue&);

public:
    // Constructor
    ChunkQueue(int initialSize = BUFSIZ)
	: _length(0), _size(initialSize), _data(new char [initialSize])
    {}

    // Destructor
    virtual ~ChunkQueue()
    {
	delete[] _data;
    }

    // Append data <dta> with length <len> at the end
    void append(const char *dta, int len)
    {
	if (_length + len + 1 > _size)
	    grow(_length + len + 1);

	memcpy(_data + _length, dta, len + 1);
	_length += len;
	_data[_length] = '\0';
    }

    // Discard <len> characters from beginning
    void discard(int len)
    {
	assert(len <= _length);

	if (len > 0)
	{
	    if (len < _length)
		memcpy(_data, _data + len, _length);
	    _length -= len;
	    _data[_length] = '\0';
	}
    }

    // Discard entire queue
    void discard()
    {
	_length = 0;
	_data[_length] = '\0';
    }

    // Resources
    const char *data() const { return _data; }
    int length() const { return _length; }
};

#endif
