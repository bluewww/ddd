// $Id$
// Map template

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

//-----------------------------------------------------------------------------
// Template fuer eine Map.
// Key sollte nicht 0 sein, da dies fuer first() und next() besondere
// Bedeutung hat. Ausserdem muss '==' fuer Key definiert sein.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#ifndef _Map_h
#define _Map_h

#if defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 5)
#pragma interface
#endif

#include "assert.h"

typedef void* MapRef;

template <class Key, class Contents>
class Map {
    int _length;
public:
    // leere Map erzeugen
    Map ();
    
    // leert die Map
    void clear ();

    // leert die Map, und ruft delete fuer alle Contents-Pointer auf
    void delete_all_contents ();
    
    // Destructor (Map vorher leeren)
    ~Map() { clear(); }
    
    // einfuegen bzw ueberschreiben; 0 bei c == 0 (kein Einfuegen)
    int insert (Key k, Contents* c);
    
    // loeschen, falls vorhanden
    void del (Key k);


    // falls nicht vorhanden 0 zurueckgeben
    Contents* get (Key k) const;
    
    // 1, wenn enthalten, sonst 0
    int contains (Key k) const;
    
    // 0, falls nicht vorhanden, sonst bel. enth. Key
    // simuliert 0-terminierte Liste
    Key first_key(MapRef& ln) const;
    static Key next_key(MapRef& ln);
    
    Contents* first(MapRef& ln) const;
    static Contents* next(MapRef& ln);
    
    inline int length()  const { return _length; }

private:
    //Realisierung als Liste;
    struct ListNode {
	Key key;
	Contents* cont;
	ListNode* _next;
    };
    
    ListNode* _first;
    
    ListNode* search (Key k) const {
	// falls nicht vorhanden 0 zurueckgeben
	ListNode* ln;
	
	ln = _first;
	while (ln != 0 && !(ln->key == k)) 
	    ln = ln->_next;
	return ln;
    }
};



template <class Key, class Contents>
Map<Key,Contents>::Map () : _length(0), _first(0) {}
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
//----------------------======------
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
Key Map<Key, Contents>::next_key(MapRef& ref) {
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
Contents* Map<Key, Contents>::next(MapRef& ref) {
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

#endif
