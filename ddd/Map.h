// $Id$
// Map template

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

//-----------------------------------------------------------------------------
// Template fuer eine Map.
// Key sollte nicht 0 sein, da dies fuer first() und next() besondere
// Bedeutung hat. Ausserdem muss '==' fuer Key definiert sein.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef _DDD_Map_h
#define _DDD_Map_h

#if defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 5)
#pragma interface
#endif

#include "bool.h"
#include "assert.h"

typedef void *MapRef;

template <class Key, class Contents>
class Map {
    // Internal list node
    struct MapNode {
	Key key;
	Contents *cont;
	MapNode *_next;
    };

private:
    MapNode *_first;
    int _length;

    // Search K; return 0 if not found
    MapNode *search(Key k) const
    {
	MapNode *ln;
	
	ln = _first;
	while (ln != 0 && !(ln->key == k)) 
	    ln = ln->_next;
	return ln;
    }

public:
    // Create empty map
    Map()
	: _first(0), _length(0) 
    {}

    // Remove all elements
    void clear()
    {
	MapNode *ln;
	MapNode *prev;

	ln = _first;
	while (ln != 0) {
	    prev = ln;
	    ln = ln->_next;
	    delete prev;
	}
	_first = 0;
	_length = 0;
    }

    // Remove all elements, delete'ing each content
    void delete_all_contents()
    {
	MapNode *ln   = _first;
	MapNode *prev = 0;
	while (ln != 0)
	{
	    prev = ln;
	    ln = ln->_next;
	    delete prev->cont;
	    delete prev;
	}
	_first = 0;
	_length = 0;
    }

    
    // Destructor
    ~Map()
    {
	clear();
    }
    
    // Insert or overwrite
    int insert(Key k, Contents* c)
    {
	if (c == 0) // Don't add empty stuff
	    return 0;

	MapNode *ln = search(k);
	if (ln == 0)
	{
	    // Insert
	    ln = new MapNode;
	    ln->key = k;
	    ln->cont = c;

	    ln->_next = _first;
	    _first = ln;
	    _length++;
	}
	else
	{
	    // Overwrite
	    ln->cont = c;
	}
	return 1;
    }
    
    // Delete K if found
    void del(Key k)
    {
	if (_first == 0)
	    return;
    
	MapNode *prev = 0;
	MapNode *ln = _first;
    
	while (ln != 0 && !(ln->key == k))
	{
	    prev = ln;
	    ln = ln->_next;
	}

	if (ln == 0)
	    return; // not found
		       
	if (prev == 0)
	{
	    // delete first element
	    assert(_first == ln);
	    _first = _first->_next;
	    delete ln;
	    _length--;
	}
	else
	{
	    prev->_next = ln->_next;
	    delete ln;
	    _length--;
	}
	assert(!contains(k));
    }

    // Get contents of K; return 0 if not found
    Contents *get(Key k) const
    {
	MapNode *ln = search(k);
	if (ln == 0)
	    return 0;
	else 
	    return ln->cont;
    }
    
    // true if K is contained
    bool contains(Key k) const
    {
	return search (k) != 0;
    }
    
    // Return first key, or 0 if not found.
    // This simulates a 0-terminated list.
    Key first_key(MapRef& ref) const
    {
	if (_first == 0)
	{
	    ref = 0;
	    return 0;
	}
	else
	{
	    ref = _first->_next;
	    return _first->key;
	}
    }

    static Key next_key(MapRef& ref)
    {
	if (ref == 0)
	{
	    // at the end
	    return 0;
	}
	else
	{
	    MapNode *current_ln = (MapNode *) ref;
	    ref = current_ln->_next;
	    return current_ln->key;
	}
    }

    // Return first contents, or 0 if not found
    Contents *first(MapRef& ref) const
    {
	if (_first == 0)
	{
	    ref = 0;
	    return 0;
	}
	else
	{
	    ref = _first->_next;
	    return _first->cont;
	}
    }

    static Contents *next(MapRef& ref)
    {
	if (ref == 0)
	{
	    // at the end
	    return 0;
	}
	else
	{
	    MapNode *current_ln = (MapNode *) ref;
	    ref = current_ln->_next;
	    return current_ln->cont;
	}
    }

    inline int length()  const { return _length; }

private:
    // No copy constructor
    Map(const Map<Key, Contents>&)
	: _first(0), _length(0)
    {
	assert(0);
    }

    // No assignment
    Map<Key, Contents>& operator = (const Map<Key, Contents>&)
    {
	assert(0); return *this;
    }
};

#endif // _DDD_Map_h
// DON'T ADD ANYTHING BEHIND THIS #endif
