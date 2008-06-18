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

#ifndef GTKX_GUISTRING_H
#define GTKX_GUISTRING_H

#include <glibmm/ustring.h>

namespace GtkX {

    // This class is used to simplify constructors which can take a
    // unicode or old-fashioned C string.
    typedef unsigned int UChar;

    class String {
	Glib::ustring s_;
    public:
	typedef std::string::size_type size_type;
	typedef Glib::ustring::iterator iterator;
	typedef Glib::ustring::const_iterator const_iterator;
	String(const Glib::ustring &s0);
	String(const char *s0);
	String(void);
	~String(void);
	Glib::ustring &s(void);
	UChar operator[](int i) const;
	const char *c(void) const; // FIXME - don't expose raw char*
	const char *c_str(void) const; // FIXME - don't expose raw char*
	const Glib::ustring &s(void) const;
	String operator+(const String &str) const;
	String &operator+=(const String &str);
	bool operator==(const String &s) const;
	bool operator!=(const String &s) const;
	bool empty(void) const;
	iterator begin(void);
	iterator end(void);
	const_iterator begin(void) const;
	const_iterator end(void) const;
	size_type find(const String &str, size_type i=0) const;
    };

    const GtkX::String mklabel(const GtkX::String &name,
			       const GtkX::String &label);


}

#endif
