// $Id$
// Argument field Implementation

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Dorothea Luetkehaus (luetke@ips.cs.tu-bs.de).
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
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// DDD is the GDB-based data display debugger.
// Contact ddd@ips.cs.tu-bs.de for details.

char ArgField_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
#include "ArgField.h"
#include <ctype.h>

// Constructor
ArgField::ArgField (Widget parent, const char* name)
    : arg_text_field(0), handlers(ArgField_NTypes), is_empty(true)
{
    arg_text_field = XtVaCreateManagedWidget (name,
					      xmTextFieldWidgetClass,
					      parent,
					      NULL);
    XtAddCallback(arg_text_field, XmNvalueChangedCallback,
		  valueChangedCB, this);
    XtAddCallback(arg_text_field, XmNlosePrimaryCallback,
		  losePrimaryCB, this);
}

string ArgField::get_string () const
{
    String arg = XmTextFieldGetString (arg_text_field);
    string str(arg);
    XtFree (arg);

    // Strip final whitespace
    while (str.length() > 0 &&
	   isspace(str[str.length() - 1]))
	str = str.before(int(str.length() - 1));

    return str;
}

void ArgField::set_string (char* text_ch)
{
    // Use the last line only
    int start = 0;
    for (int i = 0; text_ch[i]; i++)
	if (text_ch[i] == '\n')
	    start = i + 1;

    XmTextFieldSetString(arg_text_field, text_ch + start);

    XmTextPosition last_pos = XmTextFieldGetLastPosition(arg_text_field);
    XmTextFieldSetInsertionPosition(arg_text_field, last_pos);
    XmTextFieldShowPosition(arg_text_field, last_pos);
}

void ArgField::valueChangedCB(Widget w,
			      XtPointer client_data,
			      XtPointer call_data)
{
    ArgField *arg_field = (ArgField *)client_data;
    arg_field->handlers.call(Changed, arg_field);

    string s = arg_field->get_string();

    if (s == "")
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

void ArgField::losePrimaryCB(Widget w,
			     XtPointer client_data,
			     XtPointer call_data)
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


