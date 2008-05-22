// $Id$ -*- C++ -*-
// Create toolbar

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

#ifndef _DDD_toolbar_h
#define _DDD_toolbar_h

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "strclass.h"
#include "MakeMenu.h"
#include "ArgField.h"

#include "gtk_wrapper.h"

#if !defined(IF_XM)
#include <GUI/Container.h>
#include <GUI/Button.h>
#endif

#if defined(IF_XM)
#define LABEL_TYPE_STRING XmSTRING
#define LABEL_TYPE_PIXMAP XmPIXMAP
#else
enum LabelType {
    LABEL_TYPE_STRING,
    LABEL_TYPE_PIXMAP
};
#endif

// Create a toolbar as child of parent, named `NAME_toolbar', having
// the buttons ITEMS.  Return LABEL and ARGFIELD.
#if defined(IF_XM)
extern Widget create_toolbar(Widget parent, const string& name, 
			     MMDesc *items1, MMDesc *items2,
			     Widget &label, ArgField*& argfield,
			     unsigned char label_type);
#else
extern GUI::Container *create_toolbar(GUI::Container *parent, const string& name, 
				      MMDesc *items1, MMDesc *items2,
				      GUI::Button *&label, ArgField*& argfield,
				      LabelType label_type);
#endif


#endif // _DDD_toolbar_h
// DON'T ADD ANYTHING BEHIND THIS #endif
