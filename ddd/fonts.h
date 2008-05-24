// $Id$ -*- C++ -*-
// Setup DDD fonts

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_fonts_h
#define _DDD_fonts_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "strclass.h"
#include "AppData.h"

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <GUI/Entry.h>
#include <libxml/tree.h>
#include "gtk_wrapper.h"
#endif

// Font types
enum DDDFont { DefaultDDDFont       = 0,
	       VariableWidthDDDFont = 1,
	       FixedWidthDDDFont    = 2,
	       SymbolDDDFont        = 3,
               DataDDDFont          = 4 };

#if defined(IF_XM)
// Setup font specs.  DB is the resource database in use.
extern void setup_fonts(AppData& app_data, XrmDatabase db = 0);
#else
// Setup font specs.  DB is the resource database in use.
extern void setup_fonts(AppData& app_data, xmlDoc *db = 0);
#endif

// Return font name from BASE, overriding with parts from OVERRIDE.
// Override contains "-SPEC-SPEC-..."; each non-empty SPEC overrides
// the default in BASE.
extern string make_font(const AppData& ad, DDDFont base, 
			const string& override = "");

// Set a new font resource
extern void set_font(DDDFont n, const string& name);

#if defined(IF_XM)
// Browse fonts
extern void BrowseFontCB(Widget, XtPointer, XtPointer);

// Set font name and size
extern void SetFontNameCB(Widget, XtPointer, XtPointer);
extern void SetFontSizeCB(Widget, XtPointer, XtPointer);
#else
// Browse fonts
extern void BrowseFontCB(GUI::Button *, DDDFont);

// Set font name and size
extern void SetFontNameCB(GUI::Entry *, DDDFont);
extern void SetFontSizeCB(GUI::Entry *, DDDFont);
#endif

#endif // _DDD_fonts_h
// DON'T ADD ANYTHING BEHIND THIS #endif
