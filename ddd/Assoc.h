// $Id$
// Associative array

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

#ifndef _ICE_Assoc_h
#define _ICE_Assoc_h

#if defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 5)
#pragma interface
#endif

#include "bool.h"

template<class K, class V> class AssocMark;
template<class K, class V> class _Assoc;
template<class K, class V> class Assoc;
template<class K, class V> class AssocIter;

template<class K, class V>
class AssocRec {
    friend class _Assoc<K,V>;
    friend class Assoc<K,V>;
    friend class AssocIter<K,V>;

private:
    AssocRec<K,V> *next;		// For Assoc usage only

public:
    K key;
    V value;
    
    // Constructor
    AssocRec(const K& k, const V& v):
	next(0), key(k), value(v)
    {}
    AssocRec(const K& k):
	next(0), key(k)
    {}
};

template<class K, class V>
class _Assoc {
    friend class AssocMark<K,V>;

protected:
    AssocRec<K,V> *entries;	// Entries

    virtual AssocRec<K,V> *lookup(const K& key) const
    {
	for (AssocRec<K,V> *e = entries; e != 0; e = e->next)
	    if (key == e->key)
		return e;

	return 0;
    }

    virtual AssocRec<K,V> *insert(const K& key)
    {
	AssocRec<K,V> *e = new AssocRec<K,V>(key);
	e->next = entries;
	return entries = e;
    }

public:
    // Constructors
    _Assoc():
	entries(0)
    {}

    // Destructor
    virtual ~_Assoc()
    {
	AssocRec<K,V> *next = 0;
	for (AssocRec<K,V> *e = entries; e != 0; e = next)
	{
	    next = e->next;
	    delete e;
	}
    }

    // Resources

    // Access (or create) element KEY
    V& operator[] (const K& key)
    {
	AssocRec<K,V> *e = lookup(key);
	if (e == 0)
	    e = insert(key);
	return e->value;
    }

    // Access element KEY; abort if non-existent
    const V& operator[] (const K& key) const
    {
	AssocRec<K,V> *e = lookup(key);
	if (e == 0)
	    abort();
	return e->value;
    }

    // Add element KEY
    void add(const K& key)
    {
	insert(key);
    }

    // Check if element KEY is present
    bool has(const K& key) const
    {
	return lookup(key) != 0;
    }

    // Remove up to N elements KEY
    void remove(const K& key, int n = -1)
    {
	if (n == 0)
	    return;

	AssocRec<K,V> *e0 = 0;
	for (AssocRec<K,V> *e = entries; e != 0; e0 = e, e = e->next)
	    if (key == e->key)
	    {
		if (e0 == 0)
		    entries = e->next;
		else
		    e0->next = e->next;
		e->next = 0; delete e;

		if (--n == 0)
		    return;
	    }
    }

    // Copy constructor
    _Assoc(const _Assoc<K,V>& m):
	entries(0)
    {
	for (AssocRec<K,V> *e = m.entries; e != 0; e = e->next)
	    (*this)[e->key] = e->value;
    }

    // Assignment
    void operator = (const _Assoc<K,V>& m)
    {
	if (entries)
	    delete entries;
	entries = 0;

	for (AssocRec<K,V> *e = m.entries; e != 0; e = e->next)
	    (*this)[e->key] = e->value;
    }
};


template<class K, class V>
class AssocMark {
    friend class Assoc<K,V>;

protected:
    AssocRec<K,V> *rec;

    AssocMark(AssocRec<K,V> *ptr):
	rec(ptr)
    {}

public:
    AssocMark(const Assoc<K,V>& assoc):
	rec(assoc.entries)
    {}
    AssocMark(const AssocMark<K,V>& mark):
	rec(mark.rec)
    {}
    const AssocMark<K,V>& operator = (const Assoc<K,V>& assoc)
    {
	rec = assoc.entries;
	return *this;
    }
    const AssocMark<K,V>& operator = (const AssocMark<K,V>& mark)
    {
	rec = mark.rec;
	return *this;
    }
};


template<class K, class V>
class AssocIter: public AssocMark<K,V> {
protected:
    AssocIter(AssocRec<K,V> *ptr):
	AssocMark<K,V>(ptr)
    {}

public:
    AssocIter(const Assoc<K,V>& assoc):
	AssocMark<K,V>(assoc)
    {}
    AssocIter(const AssocIter<K,V>& iter):
	AssocMark<K,V>(iter)
    {}
    const AssocIter<K,V>& operator = (const Assoc<K,V>& assoc)
    {
	AssocMark<K,V>::operator = (assoc);
	return *this;
    }
    const AssocIter<K,V>& operator = (const AssocIter<K,V>& iter)
    {
	AssocMark<K,V>::operator = (iter);
	return *this;
    }

    const K& key() const   { return rec->key;   }
    const V& value() const { return rec->value; }
    V& value()             { return rec->value; }

    bool ok() { return rec != 0; }
    AssocIter<K,V> next() { return AssocIter<K,V>(rec->next); }
    void operator ++ ()
    {
	rec = rec->next;
    }
    void operator ++ (int)
    {
	rec = rec->next;
    }
};


template<class K, class V>
class Assoc: public _Assoc<K, V>
{
protected:
    AssocRec<K,V> *lookup(const K& key) const
    {
	return _Assoc<K, V>::lookup(key);
    }

    AssocRec<K,V> *insert(const K& key)
    {
	return _Assoc<K, V>::insert(key);
    }

public:
    // Truncate array up to assoc iterator
    void release (const AssocMark<K, V>& mark)
    {
	AssocRec<K, V> *e = entries;

	while (e != 0 && e != mark.rec)
	{
	    AssocRec<K, V> *tmp = e;
	    e = e->next;
	    tmp->next = 0;
	    delete tmp;
	}

	entries = e;
    }
};

#endif // _ICE_Assoc_h
// DON'T ADD ANYTHING BEHIND THIS #endif
