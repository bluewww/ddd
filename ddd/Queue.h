// $Id$  -*- C++ -*-
// Queue template

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

#ifndef _ICE_Queue_h
#define _ICE_Queue_h

#if defined(__GNUC_MINOR__) && (__GNUC_MINOR__ >= 5)
#pragma interface
#endif

#include "bool.h"

template<class E>
struct QueueRec {
    E elem;
    QueueRec<E> *next;

    // Constructor
    QueueRec(const E& e):
        elem(e), next(0)
    {}
};

template<class E>
class QueueIter;

template<class E>
class Queue {
private:
    QueueRec<E> *_first;
    QueueRec<E> *_last;

public:
    // Constructor
    Queue(): 
        _first(0), _last(0)
    {}

    // Destructor
    ~Queue()
    {
	while (_first)
	    operator -= (_first->elem);
    }

    // Copy Constructor
    Queue(const Queue<E>& e): 
        _first(0), _last(0)
    {
	for (QueueIter<E> i = e; i.ok(); i = i.next())
	    operator += (i());
    }

    // Assignment
    void operator = (const Queue<E>& e)
    {
#if 0
	while (_first)
	    operator -= (_first->elem);
#endif

	for (QueueIter<E> i = e; i.ok(); i = i.next())
	    operator += (i());
    }

    // Adding
    void operator += (const E& e)
    {
	QueueRec<E> *rec = new QueueRec<E>(e);
	if (_last == 0)
	    _first = rec;
	else
	    _last->next = rec;
	_last = rec;
    }

    // Removing
    void operator -= (const E& e)
    {
	QueueRec<E> *prev = 0;      // kill predecessor
        QueueRec<E> *rec = _first;   // loop var

	while (rec != 0)
	{
	    QueueRec<E> *kill = rec;
	    rec = rec->next;

	    if (kill->elem == e)
	    {
		// setup _last
		if (kill == _last)
		    _last = prev;

		// dequeue kill
		if (prev == 0)
		    _first = kill->next;
		else
		    prev->next = kill->next;

		// kill it
		delete kill;

		// don't search for further occurrences
		break;
	    }
	    else
		prev = kill;
	}
    }

    // Access
    const E& first() const { return _first->elem; }
    const E& last()  const { return _last->elem; }
    E& first() { return _first->elem; }
    E& last()  { return _last->elem; }

    // For QueueIter only
    QueueRec<E> *firstRec() const { return _first; }

    // Testing
    bool isEmpty() const { return _first == 0; }
};

template<class E>
class QueueIter {
private:
    QueueRec<E> *rec;
    QueueIter(QueueRec<E> *ptr):
        rec(ptr)
    {}

public:
    QueueIter(const Queue<E>& queue):
        rec(queue.firstRec())
    {}
    QueueIter(const QueueIter<E>& iter):
        rec(iter.rec)
    {}
    QueueIter<E>& operator = (const QueueIter<E>& iter)
    {
	rec = iter.rec;
	return *this;
    }
    QueueIter<E>& operator = (const Queue<E>& queue)
    {
	rec = queue.firstRec();
	return *this;
    }

    const E& operator()() const { return rec->elem; }
    E& operator()()             { return rec->elem; }

    bool ok() const { return rec != 0; }
    QueueIter<E> next() const { return QueueIter<E>(rec->next); }
};

#endif // _ICE_Queue_h
// DON'T ADD ANYTHING BEHIND THIS #endif
