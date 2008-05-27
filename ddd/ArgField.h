// $Id$
// Argument field Implementation

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

#ifndef _DDD_ArgField_h
#define _DDD_ArgField_h

//-----------------------------------------------------------------------------
// An `ArgField' is a Text field with handler procs.
//-----------------------------------------------------------------------------

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(IF_XM)
// Motif includes
#include <X11/Intrinsic.h>
#else
#include <GUI/Widget.h>
#include <GUI/ComboBox.h>
#include <GUI/Button.h>
#include "gtk_wrapper.h"
#endif

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

#if defined(IF_XM)
class ArgField {
    Widget      arg_text_field;
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
    ArgField(Widget parent, const char *name);

    bool empty () const { return is_empty; }

    string get_string () const;
    void set_string(string s);

    void lock(bool locked = true);

    Widget text() const { return arg_text_field; };
    Widget top()  const;

    void addHandler (unsigned    type,
		     HandlerProc proc,
		     void*       client_data = 0);

    void removeHandler (unsigned    type,
			HandlerProc proc,
			void        *client_data = 0);

    void callHandlers ();
};
#else
class ArgField {
    GUI::ComboBoxEntryText *arg_text_field;
    HandlerList handlers;
    bool     is_empty;
    bool     locked;

    static void valueChangedCB(GUI::Widget *w,
			       ArgField *client_data);
    static void losePrimaryCB(GUI::Widget *w,
			      ArgField *client_data);

private:
    ArgField(const ArgField&);
    ArgField& operator = (const ArgField&);

public:
    // Constructor
    ArgField(GUI::Container *parent, const char *name);

    bool empty () const { return is_empty; }

    string get_string () const;
    void set_string(string s);

    void lock(bool locked = true);

    GUI::ComboBoxEntryText *text() const { return arg_text_field; };
    GUI::Widget *top()  const;

    void addHandler (unsigned    type,
		     HandlerProc proc,
		     void*       client_data = 0);

    void removeHandler (unsigned    type,
			HandlerProc proc,
			void        *client_data = 0);

    void callHandlers ();
};
#endif

#if defined(IF_XM)
// Create a `():' label named "arg_label"
Widget create_arg_label(Widget parent);

// Clear the text field given in Widget(CLIENT_DATA)
void ClearTextFieldCB(Widget w, XtPointer client_data, XtPointer call_data);
#else
// Create a `():' label named "arg_label"
GUI::Button *create_arg_label(GUI::Container *parent);

// Clear the text field given in Widget(CLIENT_DATA)
void ClearTextFieldCB(GUI::ComboBoxEntryText *);
#endif

#endif // _DDD_ArgField_h
// DON'T ADD ANYTHING BEHIND THIS #endif

