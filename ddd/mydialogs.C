// $Id$
// Create dialogs to select display expressions

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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

char mydialogs_rcsid[] =
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

//-----------------------------------------------------------------------------
// Erzeugung von Motif-Dialogen zur Auswahl von Display-Ausdruecken
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
#include "mydialogs.h"

#include <stdlib.h>

// Motif includes
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/List.h>

// Misc includes
#include "assert.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "MString.h"

// Own includes
#include "string-fun.h"


//-----------------------------------------------------------------------------
// Vorwaerts-Deklarationen
//-----------------------------------------------------------------------------

static XmStringTable makeXmStringTable (string label_list[], int list_length);
static void freeXmStringTable (XmStringTable xmlabel_list, int list_length);

//-----------------------------------------------------------------------------


// ***************************************************************************
// Erzeugt Dialog zur (beliebigen) Auswahl aus einer Liste von Display-Labels.
// Die Callbacks werden so verwaltet, dass dispSQ mit den ausgewaehlten
// Display-Nummern (und deren Anzahl) aufgerufen wird.
// Rueckgabewert ist das List-Widget, dessen Initialisierung mit setLabelList
// erfolgt.
//
Widget createDisplaySelectionList(Widget parent, String name,
				  ArgList args, Cardinal num_args)
{
    Widget selectionList = XmCreateScrolledList(parent, name, args, num_args);
    XtManageChild (selectionList);
    return selectionList;
}



// ***************************************************************************
// Setzen der Listenelemente.
// label_list : Array der Label in der Form: disp_nr": "disp_name.
// selected   : Array mit Wert true an den Positionen der Label, die schon
//              ausgewaehlt sein sollen.
// list_length: Laenge der gesamten Liste.
//
void setLabelList (Widget  selectionList,
		   string  label_list[],
		   bool    selected[],
		   int     list_length,
		   bool    notify)
{
    XmStringTable xmlabel_list = makeXmStringTable(label_list, list_length);

    XtVaSetValues (selectionList,
		   XmNitems,     xmlabel_list,
		   XmNitemCount, list_length,
		   NULL);

    XmListDeselectAllItems (selectionList);
    XtVaSetValues (selectionList,
		   XmNselectionPolicy, XmMULTIPLE_SELECT,
		   NULL);

    for (int i = 0; i < list_length; i++) 
	if (selected[i] == true) 
	    XmListSelectItem (selectionList, xmlabel_list[i], Boolean(notify));

    XtVaSetValues (selectionList,
		   XmNselectionPolicy, XmEXTENDED_SELECT,
		   NULL);

    freeXmStringTable(xmlabel_list, list_length);
}

// ***************************************************************************
// Fill the display numbers in DISP_NRS
void getDisplayNumbers(Widget selectionList, IntArray& disp_nrs)
{
    static IntArray empty;
    disp_nrs = empty;

    XmStringTable selected_items;
    int selected_items_count;

    XtVaGetValues(selectionList,
		  XmNselectedItemCount, &selected_items_count,
		  XmNselectedItems, &selected_items,
		  NULL);

    for (int i = 0; i < selected_items_count; i++)
    {
	String _item;
	XmStringGetLtoR(selected_items[i], LIST_CHARSET, &_item);
	string item(_item);
	XtFree(_item);

	if (item.contains(rxint, 0))
	    disp_nrs += get_nr(item);
    }
}

// ***************************************************************************
// Erzeugt aus Array von string ein Array von XmString
//
static XmStringTable makeXmStringTable (string label_list[], int list_length)
{
    XmStringTable xmlist = 
	XmStringTable(XtMalloc(list_length * sizeof(XmString)));

    for (int i = 0; i < list_length; i++)
	xmlist[i] = XmStringCreateLtoR(label_list[i], LIST_CHARSET);

    return xmlist;
}

// ***************************************************************************
// Gibt Array-Elemente vom Typ XmString frei .
//
static void freeXmStringTable (XmStringTable xmlist, int list_length)
{
    for (int i = 0; i < list_length; i++) {
	XmStringFree (*xmlist);
	xmlist++;
    }
}


// ***************************************************************************
// Select POS in LIST and make it visible
// ***************************************************************************

void ListSetAndSelectPos(Widget list, int pos)
{
    int top_item      = 0;
    int visible_items = 0;
    int items         = 0;
    XtVaGetValues(list,
		  XmNtopItemPosition, &top_item,
		  XmNvisibleItemCount, &visible_items,
		  XmNitemCount, &items,
		  NULL);

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
