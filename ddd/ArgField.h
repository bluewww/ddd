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
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_ArgField_h
#define _DDD_ArgField_h

//-----------------------------------------------------------------------------
// An `ArgField' is a Text field with handler procs.
//-----------------------------------------------------------------------------

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#if defined(IF_MOTIF)

// Motif includes
#include <X11/Intrinsic.h>

#endif

#if !defined(IF_XM)
#include <GUI/Widget.h>
#include <GUI/ComboBox.h>
#include <GUI/Button.h>
#endif

#include "gtk_wrapper.h"

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
#if defined(IF_XM)
    Widget                  arg_text_field;
#else
    GUI::ComboBoxEntryText *arg_text_field;
#endif
    HandlerList handlers;
    bool     is_empty;
    bool     locked;

    static void valueChangedCB(Widget w,
			       XtPointer client_data, 
			       XtPointer call_data);
    static void losePrimaryCB(Widget w,
			      XtPointer client_data, 
			      XtPointer call_data);

private:
    ArgField(const ArgField&);
    ArgField& operator = (const ArgField&);

public:
    // Constructor
#if defined(IF_XM)
    ArgField(Widget parent, const char *name);
#else
    ArgField(GUI::Container *parent, const char *name);
#endif

    bool empty () const { return is_empty; }

    string get_string () const;
    void set_string(string s);

    void lock(bool locked = true);

#if defined(IF_XM)
    Widget text() const { return arg_text_field; };
    Widget top()  const;
#else
    GUI::ComboBoxEntryText *text() const { return arg_text_field; };
    GUI::Widget *top()  const;
#endif

    void addHandler (unsigned    type,
		     HandlerProc proc,
		     void*       client_data = 0);

    void removeHandler (unsigned    type,
			HandlerProc proc,
			void        *client_data = 0);

    void callHandlers ();
};

#if defined(IF_XM)

// Create a `():' label named "arg_label"
Widget create_arg_label(Widget parent);

// Clear the text field given in Widget(CLIENT_DATA)
void ClearTextFieldCB(Widget, XtPointer, XtPointer);

#else

// Create a `():' label named "arg_label"
GUI::Button *create_arg_label(GUI::Container *parent);

// Clear the text field given in Widget(CLIENT_DATA)
void ClearTextFieldCB(GUI::ComboBoxEntryText *);

#endif

#endif // _DDD_ArgField_h
// DON'T ADD ANYTHING BEHIND THIS #endif

