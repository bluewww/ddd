// $Id$
// Chunk buffer

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

#ifndef _Nora_ChunkQueue_h
#define _Nora_ChunkQueue_h

#ifdef __GNUG__
#pragma interface
#endif


#include "assert.h"
#include <stdio.h>
#include <memory.h>
#include "TypeInfo.h"

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

public:
    // Constructor
    ChunkQueue(int initialSize = BUFSIZ):
	_length(0), _size(initialSize), _data(new char [initialSize])
    {}

    // Destructor
    virtual ~ChunkQueue()
    {
	delete[] _data;
    }

    // Append data <dta> with length <len> at the end
    void append(char *dta, int len)
    {
	if (_length + len + 1 > _size)
	    grow(_length + len);

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
    char *data() { return _data; }
    int length() { return _length; }
};

#endif
