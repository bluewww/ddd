// $Id$
// Argument field Implementation

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _ArgField_h
#define _ArgField_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// ArgField realisiert ein TextField. 
// Es koennen Handler-Prozeduren angemeldet werden, die aufgerufen werden,
// wenn das Textfeld leer wird, gefuellt wird bzw. geandert wird.
//-----------------------------------------------------------------------------

// includes von Motif-Zeugs
#include <Xm/TextF.h>

// includes von Nora-Zeugs
#include "HandlerL.h"
#include "bool.h"
#include "strclass.h"


//-----------------------------------------------------------------------------
// Event types
const unsigned Empty       = 0;
const unsigned Changed     = Empty + 1;
const unsigned LosePrimary = Changed + 1;

const unsigned ArgField_NTypes = LosePrimary + 1;

class ArgField {
    Widget      arg_text_field;
    HandlerList handlers;
    bool     is_empty;

    static void valueChangedCB(Widget w,
			       XtPointer client_data, 
			       XtPointer call_data);
    static void losePrimaryCB(Widget w,
			      XtPointer client_data, 
			      XtPointer call_data);

public:
    // Constructor
    ArgField (Widget parent, const char* name);

    bool empty () const { return is_empty; }

    string get_string () const;

    void set_string (char* text_ch);
    void set_string (string s)
    {
	set_string((char *)s);
    }

    Widget widget() { return arg_text_field; };

    void addHandler (unsigned    type,
		     HandlerProc proc,
		     void*       client_data = 0);

    void removeHandler (unsigned    type,
			HandlerProc proc,
			void        *client_data = 0);

    void callHandlers ();
};


#endif
