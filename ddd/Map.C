// $Id$
// Map template

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

char Map_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Implementierung des Map-Template
//-----------------------------------------------------------------------------

#include "Map.h"

//-----------------------------------------------------------------------------

template <class Key, class Contents>
Map<Key,Contents>::Map () : _first(0), _length(0) {}
//-----------------===---------------


template <class Key, class Contents>
void Map<Key, Contents>::clear() {
//-----------------------=====-------
    // leert die Map
    ListNode* ln;
    ListNode* prev;

    ln = _first;
    while (ln != 0) {
	prev = ln;
	ln = ln->_next;
	delete prev;
    }
    _first = 0;
    _length = 0;
}


template <class Key, class Contents>
void Map<Key, Contents>::delete_all_contents() {
//-----------------------===================-------
    // leert die Map und ruft delete fuer alle Contents-Pointer
    ListNode* ln;
    ListNode* prev;

    ln = _first;
    while (ln != 0) {
	prev = ln;
	ln = ln->_next;
	delete prev->cont;
	delete prev;
    }
    _first = 0;
    _length = 0;
}


template <class Key, class Contents>
int Map<Key, Contents>::insert (Key k, Contents* c) {
//-----------------------======------
    if (c == 0) // nichts leeres einfuegen
	return 0;

    // einfuegen bzw ueberschreiben
    ListNode* ln = search (k);
    if (ln == 0) {

	ln = new ListNode;
	ln->key = k;
	ln->cont = c;

	ln->_next = _first;
	_first = ln;
	_length++;
    }
    else {
	ln->cont = c;
    }
    return 1;
}

template <class Key, class Contents>
void Map<Key, Contents>::del (Key k) {
//-----------------------===---------
    // loeschen, falls vorhanden
    if (_first == 0)
	return;
    
    ListNode* prev = 0;
    ListNode* ln = _first;
    
    while (ln != 0 && !(ln->key == k)) {
	prev = ln;
	ln = ln->_next;
    }

    if (ln == 0)
	return; // nicht gefunden
		       
    if (prev == 0) {
	// erstes Element loeschen
	assert (_first == ln);
	_first = _first->_next;
	delete ln;
	_length--;
    }
    else {
	prev->_next = ln->_next;
	delete ln;
	_length--;
    }
    assert (!contains(k));
}

template <class Key, class Contents>
Contents* Map<Key, Contents>::get (Key k) const {
//----------------------------===----
    // falls nicht vorhanden 0 zurueckgeben
    ListNode* ln = search (k);
    if (ln == 0)
	return 0;
    else 
	return ln->cont;
}

template <class Key, class Contents>
int Map<Key, Contents>::contains (Key k) const {
//----------------------========-----
    // 1, wenn enthalten, sonst 0
    return (search (k) != 0);
}

template <class Key, class Contents>
Key Map<Key, Contents>::first_key(MapRef& ref) const {
//----------------------=========----
    // 0, falls leer, sonst _first
    if (_first == 0) {
	ref = 0;
	return 0;
    }
    else {
	ref = _first->_next;
	return _first->key;
    }
}

template <class Key, class Contents>
Key Map<Key, Contents>::next_key(MapRef& ref) const {
//----------------------========-----
    // durchlaeuft 0-terminierte Liste
    if (ref == 0 ) {
	//am Ende angelangt
	return 0;
    }
    else {
	ListNode* current_ln = (ListNode *) ref;
	ref = current_ln->_next;
	return current_ln->key;
    }
}


template <class Key, class Contents>
Contents* Map<Key, Contents>::first(MapRef& ref) const {
//----------------------------=====--------
    // 0, falls leer, sonst _first
    if (_first == 0) {
	ref = 0;
	return 0;
    }
    else {
	ref = _first->_next;
	return _first->cont;
    }
}

template <class Key, class Contents>
Contents* Map<Key, Contents>::next(MapRef& ref) const {
//----------------------------====---------
    // durchlaeuft 0-terminierte Liste
    if (ref == 0 ) {
	//am Ende angelangt
	return 0;
    }
    else {
	ListNode* current_ln = (ListNode *) ref;
	ref = current_ln->_next;
	return current_ln->cont;
    }
}
