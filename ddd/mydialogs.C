// $Id$
// Create dialogs to select display expressions

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Copyright (C) 2001 Free Software Foundation, Inc.
// Written by Dorothea Luetkehaus <luetke@ips.cs.tu-bs.de>.
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

char mydialogs_rcsid[] =
    "$Id$";

//-----------------------------------------------------------------------------
// Create dialogs to select display expressions
//-----------------------------------------------------------------------------

#include "mydialogs.h"
#include "LessTifH.h"
#include "config.h"


// System includes
#include <stdlib.h>

// Motif includes
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/List.h>
#include <Xm/MwmUtil.h>
#include <X11/Shell.h>

// Misc includes
#include "assert.h"
#include "verify.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "MString.h"
#include "AppData.h"

// Own includes
#include "string-fun.h"


// Create a selection box with a top-level shell
Widget createTopLevelSelectionDialog(Widget parent, const _XtString name,
				     ArgList args, Cardinal num_args)
{
    if (app_data.transient_dialogs)
	return XmCreateSelectionDialog(parent, XMST(name), args, num_args);

    ArgList new_args = new Arg[num_args + 10];
    Cardinal arg = 0;

    // Give'em every decoration except `maximize'.
    int decorations = 
	MWM_DECOR_BORDER | MWM_DECOR_RESIZEH | 
	MWM_DECOR_TITLE | MWM_DECOR_MENU | 
	MWM_DECOR_MINIMIZE;
    int functions = 
	MWM_FUNC_RESIZE | MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE | MWM_FUNC_CLOSE;

    XtSetArg(new_args[arg], XmNdialogType,       XmDIALOG_SELECTION); arg++;
    XtSetArg(new_args[arg], XmNallowShellResize, True);               arg++;
    XtSetArg(new_args[arg], XmNdeleteResponse,   XmUNMAP);            arg++;
    XtSetArg(new_args[arg], XmNmwmDecorations,   decorations);        arg++;
    XtSetArg(new_args[arg], XmNmwmFunctions,     functions);          arg++;

    for (Cardinal i = 0; i < num_args; i++)
    {
	XtSetArg(new_args[arg], args[i].name, args[i].value); arg++;
    }

    string shell_name = string(name) + "_popup";
    Widget shell = verify(XtCreateWidget(shell_name.chars(), 
					 topLevelShellWidgetClass,
					 parent, new_args, arg));
    Widget box = XmCreateSelectionBox(shell, XMST(name), new_args, arg);

    delete[] new_args;

    // Set a reasonable icon name
    String title    = 0;
    String iconName = 0;
    XtVaGetValues(shell,
		  XmNtitle, &title,
		  XmNiconName, &iconName,
		  XtPointer(0));
    if (title != 0 && (iconName == 0 || string(iconName) == XtName(shell)))
	XtVaSetValues(shell, XmNiconName, title, XtPointer(0));

    return box;
}



// Set the elements of the display selection list
// LABEL_LIST:      Labels, using the format disp_nr ": " disp_name.
// SELECTED:        Whether labels are to be selected
// LIST_LENGTH:     Length of label_list[] and selected[]
// HIGHLIGHT_TITLE: Whether the first line should be highlighted
// NOTIFY:          Whether callbacks should be invoked
//
void setLabelList (Widget  selectionList,
		   const string  label_list[],
		   const bool selected[],
		   int     list_length,
		   bool    highlight_title,
		   bool    notify)
{
    if (selectionList == 0)
	return;

    XmStringTable xmlabel_list = 
	makeXmStringTable(label_list, list_length, highlight_title);

    assert(XmIsList(selectionList));

    XtVaSetValues (selectionList,
		   XmNitems,     xmlabel_list,
		   XmNitemCount, list_length,
		   XtPointer(0));

    XmListDeselectAllItems (selectionList);
    XtVaSetValues (selectionList,
		   XmNselectionPolicy, XmMULTIPLE_SELECT,
		   XtPointer(0));

    for (int i = 0; i < list_length; i++) 
	if (selected != 0 && selected[i] == true)
	    XmListSelectPos(selectionList, i + 1, Boolean(notify));

    XtVaSetValues (selectionList,
		   XmNselectionPolicy, XmEXTENDED_SELECT,
		   XtPointer(0));

    freeXmStringTable(xmlabel_list, list_length);
}

// Replace all elements in SELECTIONLIST with the corresponding
// entries in LABEL_LIST (i.e. with the same leading number).
void updateLabelList (Widget  selectionList,
		      const string  label_list[],
		      int     list_length)
{
    if (selectionList == 0)
	return;

    XmStringTable items;
    int items_count = 0;

    assert(XmIsList(selectionList));

    XtVaGetValues(selectionList,
		  XmNitemCount, &items_count,
		  XmNitems,     &items,
		  XtPointer(0));

    for (int i = 0; i < items_count; i++)
    {
	String _item;
	XmStringGetLtoR(items[i], LIST_CHARSET, &_item);
	string item(_item);
	XtFree(_item);

	if (!has_nr(item))
	    continue;

	int nr_i = get_nr(item);
	
	for (int j = 0; j < list_length; j++)
	{
	    if (!has_nr(label_list[j]))
		continue;
	    int nr_j = get_nr(label_list[j]);

	    if (nr_i != nr_j)
		continue;

	    MString mlabel(label_list[j], LIST_CHARSET);
	    XmString xmlabel = mlabel.xmstring();
	    int pos = (i == items_count - 1 ? 0 : i + 1);
	    XmListReplaceItemsPos(selectionList, &xmlabel, 1, pos);
	    break;
	}
    }
}


// Fill the item numbers in DISP_NRS
void getItemNumbers(Widget selectionList, IntArray& numbers)
{
    static const IntArray empty;
    numbers = empty;

    if (selectionList == 0)
	return;

    XmStringTable selected_items;
    int selected_items_count = 0;

    assert(XmIsList(selectionList));

    XtVaGetValues(selectionList,
		  XmNselectedItemCount, &selected_items_count,
		  XmNselectedItems, &selected_items,
		  XtPointer(0));

    for (int i = 0; i < selected_items_count; i++)
    {
	String _item;
	XmStringGetLtoR(selected_items[i], LIST_CHARSET, &_item);
	string item(_item);
	XtFree(_item);

	if (has_nr(item))
	    numbers += get_nr(item);
    }
}

// Create an array of XmStrings from the list LABEL_LIST of length
// LIST_LENGTH.  If HIGHLIGHT_TITLE is set, let the first line be bold.
XmStringTable makeXmStringTable (const string label_list[],
				 int list_length, bool highlight_title)
{
    XmStringTable xmlist = 
	XmStringTable(XtMalloc(list_length * sizeof(XmString)));

    int i = 0;
    if (highlight_title && i < list_length)
    {
	xmlist[i] = XmStringCreateLtoR(XMST(label_list[i].chars()), LIST_TITLE_CHARSET);
	i++;
    }
	
    while (i < list_length)
    {
	xmlist[i] = XmStringCreateLtoR(XMST(label_list[i].chars()), LIST_CHARSET);
	i++;
    }

    return xmlist;
}

// Select POS in LIST and make it visible
void ListSetAndSelectPos(Widget list, int pos)
{
    if (list == 0)
	return;

    assert(XmIsList(list));

    int top_item      = 0;
    int visible_items = 0;
    int items         = 0;
    XtVaGetValues(list,
		  XmNtopItemPosition, &top_item,
		  XmNvisibleItemCount, &visible_items,
		  XmNitemCount, &items,
		  XtPointer(0));

    // Eugene M. Indenbom <bom@classic.iki.rssi.ru> reports that we
    // cannot use XmListSelectPos() unconditionally here because on
    // his Motif 1.2, you will get two highlighted items.

    int *pos_list, pos_count;
    bool do_select = true;
    if (XmListGetSelectedPos(list, &pos_list, &pos_count))
    {
        if (pos_count != 1 || pos_list[0] != pos)
	{
	    // Deselect all other items first
            XmListDeselectAllItems(list);
	}
        else
	{
	    // Already selected -- do not select again
	    do_select = false;
	}

	XtFree((String)pos_list);
    }

    if (do_select)
        XmListSelectPos(list, pos, False);

    if (pos == 1)
	XmListSetPos(list, pos);
    else if (pos == 0 || pos >= items)
	XmListSetBottomPos(list, 0);
    else if (pos - 1 < top_item)
	XmListSetPos(list, pos - 1);
    else if (pos + 1 >= top_item + visible_items)
	XmListSetBottomPos(list, pos + 1);
}
