// $Id$
// Simple interface to Motif composite strings

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Universitaet Passau, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
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

#ifndef _DDD_MString_h
#define _DDD_MString_h

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(IF_XM)
// An "MString" is but a C++ wrapper around Motif composite strings.

#include <Xm/Xm.h>
#else
#include "gtk_wrapper.h"
#endif

#include "strclass.h"
#include "assert.h"
#include "casts.h"

#if defined(IF_XM)
#ifdef XmFONTLIST_DEFAULT_TAG
#define MSTRING_DEFAULT_CHARSET XmFONTLIST_DEFAULT_TAG
#else
#define MSTRING_DEFAULT_CHARSET XmSTRING_DEFAULT_CHARSET
#endif
#else
#define MSTRING_DEFAULT_CHARSET "IGNORED"
#endif

#if defined(IF_XM)
class MString {
private:
    XmString _mstring;		// Motif internals

public:
    // Constructors
    MString():
	_mstring(XmStringCreateLtoR(XMST(""), MSTRING_DEFAULT_CHARSET))
    {
	assert(OK());
    }

    MString(const char *text,
	    XmStringCharSet charset = MSTRING_DEFAULT_CHARSET):
	_mstring(text ? XmStringCreateLtoR(XMST(text), charset) : 0)
    {
	assert(OK());
    }

    MString(const string& text,
	    XmStringCharSet charset = MSTRING_DEFAULT_CHARSET):
	_mstring(XmStringCreateLtoR(XMST(text.chars()), charset))
    {
	assert(OK());
    }

    // In Motif 1.1, `XmString' is defined as `char *'; hence the
    // DUMMY parameter
    MString(XmString text, bool /* dummy */):
	_mstring(text ? XmStringCopy(text) : 0)
    {
	assert(OK());
    }

    // Copy constructor
    MString(const MString& m):
	_mstring(m._mstring ? XmStringCopy(m._mstring) : 0)
    {
	assert(m.OK());
	assert(OK());
    }

    // Destructor
    ~MString()
    {
	assert(OK());
	XmStringFree(_mstring);
    }

    // Resources
    Dimension baseline(XmFontList fontlist) const
    {
	return XmStringBaseline(fontlist, _mstring);
    }

    Boolean isEmpty() const
    {
	return XmStringEmpty(_mstring);
    }

    Boolean isNull() const
    {
	return xmstring() == 0;
    }

    int lineCount() const
    {
	return XmStringLineCount(_mstring);
    }

    void extent(Dimension& x, Dimension& y, XmFontList fontlist) const
    {
	XmStringExtent(fontlist, _mstring, &x, &y);
    }

    Dimension height(XmFontList fontlist) const
    {
	return XmStringHeight(fontlist, _mstring);
    }

    Dimension width(XmFontList fontlist) const
    {
	return XmStringWidth(fontlist, _mstring);
    }

    int length() const
    {
	return XmStringLength(_mstring);
    }

    // Return all characters, regardless of charset
    string str() const;

    // Assignment
    MString& operator = (const MString& m)
    {
	assert(OK());
	assert(m.OK());

	if ( this != &m ) { 
	  // Make sure a = a works
	  XmString tmp = XmStringCopy(m._mstring);
	  XmStringFree(_mstring);
	  _mstring = tmp;
	}

	return *this;
    }

    // Concatenation
    MString& operator += (const MString& m)
    {
	assert(OK());
	assert(m.OK());

	if (isNull())
	    return operator = (m);

	XmString old = _mstring;
	_mstring = XmStringConcat(_mstring, m._mstring);
	XmStringFree(old);

	return *this;
    }

    MString& prepend(const MString& m)
    {
	assert(OK());
	assert(m.OK());

	if (isNull())
	    return operator = (m);

	XmString old = _mstring;
	_mstring = XmStringConcat(m._mstring, _mstring);
	XmStringFree(old);

	return *this;
    }

    // Comparison
    Boolean operator == (const MString& m) const
    {
	assert(OK());
	assert(m.OK());
	return XmStringCompare(_mstring, m._mstring);
    }

    Boolean operator != (const MString& m) const
    {
	assert(OK());
	assert(m.OK());
	return !XmStringCompare(_mstring, m._mstring);
    }

    // Conversions
    XmString xmstring() const { return _mstring; }

    // Substrings
    Boolean contains(const MString& m) const
    {
	assert(OK());
	assert(m.OK());
	return XmStringHasSubstring(_mstring, m._mstring);
    }

    // Invariant
    Boolean OK() const;
};

// Concatenation
inline MString operator + (const MString& m1, const MString& m2)
{
    assert(m1.OK());
    assert(m2.OK());

    if (m1.isNull())
	return m2;
    if (m2.isNull())
	return m1;

    XmString tmp = XmStringConcat(m1.xmstring(), m2.xmstring());
    MString ret(tmp, true);
    XmStringFree(tmp);
    return ret;
}
#else
// Note: Motif Compound String should really be mapped to a
// Pango GlyphString?

class MString {
private:
    GUI::String _mstring;		// Motif internals

public:
    // Constructors
    MString():
	_mstring("")
    {
	assert(OK());
    }

    MString(const char *text,
	    const char *charset = MSTRING_DEFAULT_CHARSET):
	_mstring(text)
    {
	assert(OK());
    }

    MString(const string& text,
	    const char *charset = MSTRING_DEFAULT_CHARSET):
	_mstring(text.chars())
    {
	assert(OK());
    }

    MString(const GUI::String &text, bool /* dummy */):
	_mstring(text)
    {
	assert(OK());
    }

    // Copy constructor
    MString(const MString& m):
	_mstring(m._mstring)
    {
	assert(m.OK());
	assert(OK());
    }

    // Destructor
    ~MString()
    {
	assert(OK());
	// Nothing to do.
    }

    // Resources
    Dimension baseline(void *fontlist) const
    {
#ifdef NAG_ME
#warning Implement MString::baseline!
#endif
	fprintf(stderr, "Implement MString::baseline!\n");
	return 0;
    }

    Boolean isEmpty() const
    {
	return _mstring.empty();
    }

    Boolean isNull() const
    {
#ifdef NAG_ME
#warning Cannot happen
#endif
	return False;
    }

    int lineCount() const
    {
	if (_mstring.empty()) return 0;
	int lc = 1;
	for (GUI::String::const_iterator i = _mstring.begin();
	     i != _mstring.end();
	     i++) {
#ifdef NAG_ME
#warning Unicode?
#endif
	    if (*i == '\n') lc++;
	}
	return lc;
    }

#if defined(NAG_ME)
#warning extent, height, width require PangoContext.
#endif

#if defined(IF_XM)

    void extent(Dimension& x, Dimension& y, void *fontlist) const
    {
	Glib::RefPtr<Pango::Layout> lo = Pango::Layout::create(pango_context);
	lo->set_text(_mstring.s());
	lo->get_pixel_size(x, y);
    }

    Dimension height(void *fontlist) const
    {
	int x, y;
	Glib::RefPtr<Pango::Layout> lo = Pango::Layout::create(pango_context);
	lo->set_text(_mstring.s());
	lo->get_pixel_size(x, y);
	return y;
    }

    Dimension width(void *fontlist) const
    {
	int x, y;
	Glib::RefPtr<Pango::Layout> lo = Pango::Layout::create(pango_context);
	lo->set_text(_mstring.s());
	lo->get_pixel_size(x, y);
	return x;
    }

#endif

#ifdef NAG_ME
#warning Does length() have a meaning for our strings?
#endif

#if 0
    int length() const
    {
	return XmStringLength(_mstring);
    }
#endif

    // Return all characters, regardless of charset
    string str() const;

    // Assignment
    MString& operator = (const MString& m)
    {
	assert(OK());
	assert(m.OK());

	if ( this != &m ) { 
	  _mstring = m._mstring;
	}

	return *this;
    }

    // Concatenation
    MString& operator += (const MString& m)
    {
	assert(OK());
	assert(m.OK());

	_mstring = _mstring + m._mstring;

	return *this;
    }

    MString& prepend(const MString& m)
    {
	assert(OK());
	assert(m.OK());

	_mstring = m._mstring + _mstring;

	return *this;
    }

    // Comparison
    Boolean operator == (const MString& m) const
    {
	assert(OK());
	assert(m.OK());
	return (_mstring == m._mstring);
    }

    Boolean operator != (const MString& m) const
    {
	assert(OK());
	assert(m.OK());
	return !(_mstring == m._mstring);
    }

    // Conversions
    GUI::String xmstring() const { return _mstring; }

    // Substrings
    Boolean contains(const MString& m) const
    {
	assert(OK());
	assert(m.OK());
	return (_mstring.find(m._mstring) >= 0);
    }

    // Invariant
    Boolean OK() const;
};

// Concatenation
inline MString operator + (const MString& m1, const MString& m2)
{
    assert(m1.OK());
    assert(m2.OK());

    GUI::String tmp = m1.xmstring() + m2.xmstring();
    MString ret(tmp, true);
    return ret;
}
#endif


inline MString operator + (const MString& m, const char *s)
{
    return operator + (m, MString(s));
}

inline MString operator + (const MString& m, char *s)
{
    return operator + (m, MString(s));
}

inline MString operator + (const char *s, const MString& m)
{
    return operator + (MString(s), m);
}

inline MString operator + (char *s, const MString& m)
{
    return operator + (MString(s), m);
}

inline MString operator + (const MString& m, const string& s)
{
    return operator + (m, MString(s));
}

inline MString operator + (const string& s, const MString& m)
{
    return operator + (MString(s), m);
}

#endif // _DDD_MString_h
// DON'T ADD ANYTHING BEHIND THIS #endif
