// $Id$
// Associative array

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Free Software Foundation, Inc.
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

#ifndef _DDD_Assoc_h
#define _DDD_Assoc_h

#include "bool.h"
#include "assert.h"

#include <stdlib.h>		// abort()

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
    AssocRec(const K& k, const V& v)
	: next(0), key(k), value(v)
    {}
    AssocRec(const K& k)
	: next(0), key(k)
    {}

private:
    AssocRec(const AssocRec<K, V>&);
    AssocRec<K,V>& operator = (const AssocRec<K, V>&);
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

private:
    // Destroy all elements
    void destroyAll()
   {
      AssocRec<K,V> *next = 0;
      for (AssocRec<K,V> *e = entries; e != 0; e = next)
      {
          next = e->next;
          delete e;
      }
      entries = 0;
   }

public:
    // Constructors
    _Assoc():
	entries(0)
    {}

    // Destructor
    virtual ~_Assoc()
    {
	destroyAll();
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

	AssocRec<K,V> *prev = 0;
	AssocRec<K,V> *e = entries;
	while (e != 0)
	{
	    AssocRec<K,V> *next = e->next;

	    if (key == e->key)
	    {
		if (prev == 0)
		    entries = next;
		else
		    prev->next = next;
		delete e;

		if (--n == 0)
		    return;
	    }
	    else
	    {
		prev = e;
	    }

	    e = next;
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
    _Assoc<K, V>& operator = (const _Assoc<K,V>& m)
    {
	if (this != &m)
	{
	    if (entries)
		destroyAll();
	    entries = 0;

	    for (AssocRec<K,V> *e = m.entries; e != 0; e = e->next)
		(*this)[e->key] = e->value;
	}

	return *this;
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
    AssocMark<K,V>& operator = (const Assoc<K,V>& assoc)
    {
	rec = assoc.entries;
	return *this;
    }
    AssocMark<K,V>& operator = (const AssocMark<K,V>& mark)
    {
        if (this != &mark)
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
    AssocIter<K,V>& operator = (const Assoc<K,V>& assoc)
    {
	AssocMark<K,V>::operator = (assoc);
	return *this;
    }
    AssocIter<K,V>& operator = (const AssocIter<K,V>& iter)
    {
        if (this != &iter)
	  AssocMark<K,V>::operator = (iter);
	return *this;
    }

    const K& key() const   { return this->rec->key;   }
    const V& value() const { return this->rec->value; }
    V& value()             { return this->rec->value; }

    bool ok() const { return this->rec != 0; }
    AssocIter<K,V> next() const { return AssocIter<K,V>(this->rec->next); }
    const AssocIter<K,V>& operator ++ ()
    {
	this->rec = this->rec->next; return *this;
    }
    const AssocIter<K,V> operator ++ (int)
    {
	AssocIter<K,V> old(*this);
	this->rec = this->rec->next;
	return old;
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
	AssocRec<K, V> *e = this->entries;

	while (e != 0 && e != mark.rec)
	{
	    AssocRec<K, V> *tmp = e;
	    e = e->next;
	    tmp->next = 0;
	    delete tmp;
	}

	this->entries = e;
    }

    // Constructor
    Assoc()
	: _Assoc<K, V>() 
    {}

    // Destructor
    ~Assoc() {}

    // Copy
    Assoc(const Assoc<K, V>& m)
	: _Assoc<K, V>(m)
    {}

    // Assignment
    Assoc<K, V>& operator = (const Assoc<K, V>& m)
    {
        if (this != &m) {
	  _Assoc<K, V>::operator =(m);
	}
	return *this;
    }
};

#endif // _DDD_Assoc_h
// DON'T ADD ANYTHING BEHIND THIS #endif
