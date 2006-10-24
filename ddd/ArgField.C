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

char ArgField_rcsid[] =
    "$Id$";

//-----------------------------------------------------------------------------
#include "config.h"

#include "ArgField.h"
#include <ctype.h>

#ifdef IF_MOTIF
#include <Xm/TextF.h>
#include <Xm/PushB.h>
#endif // IF_MOTIF

#include "verify.h"
#include "charsets.h"
#include "AppData.h"
#include "buttons.h"
#include "string-fun.h"		// strip_space()
#include "tabs.h"		// tabify()
#ifdef IF_MOTIF
#include "ComboBox.h"
#endif // IF_MOTIF


// Constructor
ArgField::ArgField (CONTAINER_P parent, const char* name)
    : arg_text_field(0), handlers(ArgField_NTypes), is_empty(true), 
      locked(false)
{
#ifdef IF_MOTIF
    Arg args[10];
    Cardinal arg = 0;

    if (!app_data.button_captions)
    {
	// Make argument field a little less high
	XtSetArg(args[arg], XmNmarginHeight, 2); arg++;
    }

    arg_text_field = CreateComboBox(parent, name, args, arg);

    XtAddCallback(arg_text_field, XmNvalueChangedCallback,
		  valueChangedCB, this);
    XtAddCallback(arg_text_field, XmNlosePrimaryCallback,
		  losePrimaryCB, this);
#else // NOT IF_MOTIF

    arg_text_field = new Gtk::ComboBoxEntryText();
    arg_text_field->set_name(name);
    parent->add(*arg_text_field);
    arg_text_field->show();
#endif // IF_MOTIF
}

string ArgField::get_string () const
{
#ifdef IF_MOTIF
    String arg = XmTextFieldGetString (arg_text_field);
    string str(arg);
    XtFree (arg);
#else // NOT IF_MOTIF
    string str(arg_text_field->get_entry()->get_text().c_str());
#endif // IF_MOTIF
    strip_space(str);
    return str;
}

void ArgField::set_string(string s)
{
    if (locked)
	return;

    // Strip blanks
    strip_space(s);

    // XmTextField cannot display tabs
    untabify(s);

    // Don't use newlines
    s.gsub('\n', ' ');

    // Set it
#ifdef IF_MOTIF
    String old_s = XmTextFieldGetString(arg_text_field);
#else // NOT IF_MOTIF
    string old_s(arg_text_field->get_entry()->get_text().c_str());
#endif // IF_MOTIF
    if (s != old_s)
    {
#ifdef IF_MOTIF
	XmTextFieldSetString(arg_text_field, XMST(s.chars()));
#else // NOT IF_MOTIF
	arg_text_field->get_entry()->set_text(XMST(s.chars()));
#endif // IF_MOTIF

#ifdef IF_MOTIF
	if (XtIsRealized(arg_text_field)) // LessTif 0.1 crashes otherwise
	{
	    XmTextPosition last_pos = 
		XmTextFieldGetLastPosition(arg_text_field);
	    XmTextFieldSetInsertionPosition(arg_text_field, last_pos);
	    XmTextFieldShowPosition(arg_text_field, 0);
	    XmTextFieldShowPosition(arg_text_field, last_pos);
	}
#endif // IF_MOTIF
    }

#ifdef IF_MOTIF
    XtFree(old_s);
#endif // IF_MOTIF
}

void ArgField::valueChangedCB(Widget,
			      XtPointer client_data,
			      XtPointer)
{
    ArgField *arg_field = (ArgField *)client_data;
    arg_field->handlers.call(Changed, arg_field);

    const string s = arg_field->get_string();

    if (s.empty())
    {
	if (!arg_field->is_empty)
	{
	    arg_field->is_empty = true;
	    arg_field->handlers.call(Empty, arg_field, (void *)true);
	}
    }
    else if (arg_field->is_empty)
    {
	arg_field->is_empty = false;
	arg_field->handlers.call(Empty, arg_field, (void *)false);
    }
}

void ArgField::lock(bool arg)
{
    locked = arg;
    XtSetSensitive(top(), !locked);
    XtSetSensitive(text(), !locked);
}

void ArgField::losePrimaryCB(Widget,
			     XtPointer client_data,
			     XtPointer)
{
    ArgField *arg_field = (ArgField *)client_data;
    arg_field->handlers.call(LosePrimary, arg_field, 0);
}

void ArgField::addHandler (unsigned    type,
			   HandlerProc proc,
			   void*       client_data)
{
    handlers.add(type, proc, client_data);
}

void ArgField::removeHandler (unsigned    type,
			      HandlerProc proc,
			      void        *client_data)
{
    handlers.remove(type, proc, client_data);
}

void ArgField::callHandlers ()
{
    handlers.call(Empty, this, (void*)is_empty);
}

Widget ArgField::top() const
{
#ifdef IF_MOTIF
    return ComboBoxTop(text());
#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning ArgField::top()?
#endif
    return text();
#endif // IF_MOTIF
}


// Clear the text field given in Widget(CLIENT_DATA)
void ClearTextFieldCB(CB_ALIST_2(XtP(COMBOBOXENTRYTEXT_P) client_data))
{
#ifdef IF_MOTIF
    Widget arg_field = Widget(client_data);
    XmTextFieldSetString(arg_field, XMST(""));
#else // NOT IF_MOTIF
    client_data->get_entry()->set_text(XMST(""));
#endif // IF_MOTIF
}

// Create a `():' label named "arg_label" for ARG_FIELD
BUTTON_P create_arg_label(CONTAINER_P parent)
{
    return create_flat_button(parent, "arg_label");
}
