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

#include <GtkX/GUIString.h>

using namespace GtkX;

String::String(const Glib::ustring &s0):
    s_(s0)
{
}

String::String(const char *s0):
    s_(s0?s0:"")
{
}

String::String(void)
{
}

String::~String(void)
{
}

bool
String::empty(void) const
{
    return s_.empty();
}

Glib::ustring &
String::s(void)
{
    return s_;
}

const Glib::ustring &
String::s(void) const
{
    return s_;
}

String
String::operator+(const String &str) const
{
    return String(s_+str.s_);
}

String &
String::operator+=(const String &str)
{
    s_ += str.s_;
    return *this;
}

bool
String::operator==(const String &str) const
{
    return s_ == str.s_;
}

bool
String::operator!=(const String &str) const
{
    return s_ != str.s_;
}

UChar
String::operator[](int i) const
{
    return s_[i];
}

const char *
String::c(void) const
{
  return s_.c_str();
}

const char *
String::c_str(void) const
{
  return s_.c_str();
}

String::iterator
String::begin(void)
{
    return s_.begin();
}

String::iterator
String::end(void)
{
    return s_.end();
}

String::const_iterator
String::begin(void) const
{
    return s_.begin();
}

String::const_iterator
String::end(void) const
{
    return s_.end();
}
