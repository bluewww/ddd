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

char ArgField_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
#include "ArgField.h"
#include <ctype.h>

#include <Xm/TextF.h>
#include <Xm/PushB.h>

#include "verify.h"
#include "charsets.h"


// Constructor
ArgField::ArgField (Widget parent, const char* name)
    : arg_text_field(0), handlers(ArgField_NTypes), is_empty(true)
{
    arg_text_field = verify(XtVaCreateManagedWidget (name,
						     xmTextFieldWidgetClass,
						     parent,
						     NULL));
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
    for (int i = 0; text_ch[i] != '\0'; i++)
	if (text_ch[i] == '\n')
	    start = i + 1;

    String s = XmTextFieldGetString(arg_text_field);
    if (strcmp(s, text_ch + start) != 0)
    {
	XmTextFieldSetString(arg_text_field, text_ch + start);

	if (XtIsRealized(arg_text_field)) // LessTif 0.1 crashes otherwise
	{
	    XmTextPosition last_pos = 
		XmTextFieldGetLastPosition(arg_text_field);
	    XmTextFieldSetInsertionPosition(arg_text_field, last_pos);
	    XmTextFieldShowPosition(arg_text_field, 0);
	    XmTextFieldShowPosition(arg_text_field, last_pos);
	}

	XtFree(s);
    }
}

void ArgField::valueChangedCB(Widget,
			      XtPointer client_data,
			      XtPointer)
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


// Clear the text field given in Widget(CLIENT_DATA)
void ClearTextFieldCB(Widget, XtPointer client_data, XtPointer)
{
    Widget arg_field = Widget(client_data);
    XmTextFieldSetString(arg_field, "");
}

// Create a `():' label named "arg_label" for ARG_FIELD
Widget create_arg_label(Widget parent)
{
    Arg args[10];
    int arg = 0;

    arg = 0;
    XtSetArg(args[arg], XmNborderWidth,	       0);     arg++;
    XtSetArg(args[arg], XmNshadowThickness,    0);     arg++;
    XtSetArg(args[arg], XmNhighlightThickness, 0);     arg++;
    XtSetArg(args[arg], XmNfillOnArm,          False); arg++;
    Widget arg_label = 
	verify(XmCreatePushButton(parent, "arg_label", args, arg));
    XtManageChild(arg_label);

    return arg_label;
}
