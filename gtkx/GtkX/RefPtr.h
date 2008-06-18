// -*- C++ -*-

// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.
// A brief look at QT indicates that this will be required there as
// well.

#ifndef GTKX_REFPTR_H
#define GTKX_REFPTR_H


namespace GtkX {

    template <class T>
    class RefPtr {
	T *p;
    public:
	RefPtr(void) {
	    p = NULL;
	}
	RefPtr(T *ptr) {
	    p = ptr;
	    if (p) p->ref();
	}
	RefPtr(const RefPtr &rp) {
	    p = rp.p;
	    if (p) p->ref();
	}
	template <class S> inline RefPtr(const RefPtr<S>& src);
	RefPtr &operator=(const RefPtr &rp) {
	    if (rp.p) rp.p->ref();
	    if (p) p->unref();
	    p = rp.p;
	    return *this;
	}
	RefPtr &operator=(T *val) {
	    if (val) val->ref();
	    if (p) p->unref();
	    p = val;
	    return *this;
	}
	~RefPtr(void) {
	    if (p) p->unref();
	}
	operator T *(void) const {
	    return p;
	}
	T &operator*(void) const {
	    return *p;
	}
	T *operator->() const {
	    return &(operator*());
	}
	void clear(void) {
	}
    };

    template <class T>
    template <class S>
    inline RefPtr<T>::RefPtr(const RefPtr<S>& src)
    {
	p = src.operator->();
	if (p) {p->ref();}
    }

}

#endif
