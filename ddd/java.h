// $Id$ -*- C++ -*-
// Find Java class

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_java_h
#define _DDD_java_h

#ifdef __GNUG__
#pragma interface
#endif

#include "StringA.h"

// Store all classes in current use path in CLASSES_LIST.  If
// WITH_SOURCE_ONLY is set, consider only classes with loadable sources.
void get_java_classes(StringArray& classes_list, bool with_source_only = true);

// Return source file of CLASS_NAME; "" if none
string java_class_file(const string& class_name, bool search_classes = true);

// Remove `.java' and `.class' suffix from S
void strip_java_suffix(string& s);

// Return the position of CLASS_NAME definition in TEXT; -1 if none
int java_class_start(const string& text, const string& class_name,
		     bool first_line = true);

#endif // _DDD_java_h
// DON'T ADD ANYTHING BEHIND THIS #endif
