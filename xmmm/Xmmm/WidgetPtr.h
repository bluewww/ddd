// -*- C++ -*-

// High-level GUI wrapper for Xm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of Xmmm.
// 
// Xmmm is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// Xmmm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with Xmmm -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#ifndef XMMM_WIDGETPTR_H
#define XMMM_WIDGETPTR_H

namespace Xmmm {

    template <class T>
    class WidgetPtr {
	T *p_;
    public:
	WidgetPtr(void)
	    {
		p_ = NULL;
	    }
	WidgetPtr(T *p)
	    {
		p_ = p;
	    }
	operator void *(void)
	    {
		// FIXME: This is a very temporary kludge while we use XtPointer
		// data in callbacks.
		return p_->internal();
	    };
	operator T *(void)
	    {
		return p_;
	    };
	operator bool(void)
	    {
		return p_;
	    };
	operator ::Widget(void)
	    {
		return p_?p_->internal():NULL;
	    }
	T &operator*(void) const
	    {
		return *p_;
	    }
	T *operator->() const
	    {
		return p_;
	    }
    };

}

#endif // XMMM_WIDGETPTR_H
