// $Id$
// Argument field Implementation

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_ArgField_h
#define _DDD_ArgField_h

#ifdef __GNUG__
#pragma interface
#endif

//-----------------------------------------------------------------------------
// ArgField realisiert ein TextField. 
// Es koennen Handler-Prozeduren angemeldet werden, die aufgerufen werden,
// wenn das Textfeld leer wird, gefuellt wird bzw. geandert wird.
//-----------------------------------------------------------------------------

// Motif includes
#include <X11/Intrinsic.h>

// Misc includes
#include "HandlerL.h"
#include "bool.h"
#include "strclass.h"
#include "assert.h"


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

private:
    ArgField(const ArgField&): 
	arg_text_field(0), handlers(), is_empty(false) { assert(0); }
    ArgField& operator = (const ArgField&)       { assert(0); return *this; }

public:
    // Constructor
    ArgField(Widget parent, const char *name);

    bool empty () const { return is_empty; }

    string get_string () const;

    void set_string(string s);

    Widget widget() { return arg_text_field; };

    void addHandler (unsigned    type,
		     HandlerProc proc,
		     void*       client_data = 0);

    void removeHandler (unsigned    type,
			HandlerProc proc,
			void        *client_data = 0);

    void callHandlers ();
};

// Create a `():' label named "arg_label"
Widget create_arg_label(Widget parent);

// Clear the text field given in Widget(CLIENT_DATA)
void ClearTextFieldCB(Widget w, XtPointer client_data, XtPointer call_data);

#endif // _DDD_ArgField_h
// DON'T ADD ANYTHING BEHIND THIS #endif

