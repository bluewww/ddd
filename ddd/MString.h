// $Id$
// Simple interface to Motif composite strings

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

#ifndef _Nora_MString_h
#define _Nora_MString_h

#ifdef __GNUG__
#pragma interface
#endif


// An "MString" is but a C++ wrapper around Motif composite strings.

#include <Xm/Xm.h>
#include "strclass.h"

#ifdef XmFONTLIST_DEFAULT_TAG
#define MSTRING_DEFAULT_CHARSET XmFONTLIST_DEFAULT_TAG
#else
#define MSTRING_DEFAULT_CHARSET XmSTRING_DEFAULT_CHARSET
#endif

class MString {
private:
    XmString _mstring;		// Motif internals

public:
    // Constructors
    MString(String text = "", 
	    XmStringCharSet charset = MSTRING_DEFAULT_CHARSET):
	_mstring(XmStringCreateLtoR(text, charset))
    {}

    MString(const string& text, 
	    XmStringCharSet charset = MSTRING_DEFAULT_CHARSET):
	_mstring(XmStringCreateLtoR(String(text), charset))
    {}

    MString(XmString text):
	_mstring(XmStringCopy(text))
    {}

    // Copy constructor
    MString(const MString& m):
	_mstring(XmStringCopy(m._mstring))
    {}

    // Destructor
    ~MString() { XmStringFree(_mstring); }

    // Assignment
    MString& operator=(const MString& m)
    {
	// Make sure a = a works
	XmString tmp = XmStringCopy(m._mstring);
	XmStringFree(_mstring);
	_mstring = tmp;

	return *this;
    }

    // Concatenation
    MString& operator += (const MString& m)
    {
	XmString old = _mstring;
	_mstring = XmStringConcat(_mstring, m._mstring);
	XmStringFree(old);

	return *this;
    }
    MString& prepend(const MString& m)
    {
	XmString old = _mstring;
	_mstring = XmStringConcat(_mstring, m._mstring);
	XmStringFree(old);

	return *this;
    }

    // Comparison
    Boolean operator == (const MString& m) const
    {
	return XmStringCompare(_mstring, m._mstring);
    }

    Boolean operator != (const MString& m) const
    {
	return !XmStringCompare(_mstring, m._mstring);
    }

    // Conversions
    operator const XmString() const
    {
	return _mstring;
    }

    // Attributes
    Dimension baseline(XmFontList fontlist) const
    {
	return XmStringBaseline(fontlist, _mstring);
    }

    Boolean isEmpty() const
    {
	return XmStringEmpty(_mstring);
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

    int lineCount() const
    {
	return XmStringLineCount(_mstring);
    }

    string str(XmStringCharSet charset = MSTRING_DEFAULT_CHARSET) const
    {
	char *text;
	if (XmStringGetLtoR(_mstring, charset, &text))
	{
	    string s = text;
	    XtFree(text);
	    return s;
	}

	return "";
    }

    // Substrings
    Boolean contains(const MString& m) const
    {
	return XmStringHasSubstring(_mstring, m._mstring);
    }

    friend static MString operator + (const MString& m1, const MString& m2);
};

// Addition
inline MString operator + (const MString& m1, const MString& m2)
{
    return MString(XmStringConcat(m1._mstring, m2._mstring));
}

inline MString operator + (const MString& m, String s)
{
    return operator + (m, MString(s));
}

inline MString operator + (String s, const MString& m)
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

#endif
