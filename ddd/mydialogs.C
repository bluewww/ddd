// $Id$
// Create dialogs to select display expressions

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

#include <malloc.h>
#include <stdlib.h>

// includes von Motif-Zeugs
#include <Xm/Xm.h>
#include <Xm/SelectioB.h>
#include <Xm/List.h>

// includes von Nora-Zeugs
#include "assert.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "MString.h"

// includes eigener Sachen
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
		   bool selected[],
		   int     list_length)
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
    for (int i = 0; i < list_length; i++) {
	if (selected[i] == true) {
	    XmListSelectItem (selectionList, xmlabel_list[i], true);
	}
    }
    XtVaSetValues (selectionList,
		   XmNselectionPolicy, XmEXTENDED_SELECT,
		   NULL);

    freeXmStringTable(xmlabel_list, list_length);
}

// ***************************************************************************
// Get the display numbers in a NULL-terminated list
int *getDisplayNumbers(Widget selectionList)
{
    XmStringTable selected_items;
    int selected_items_count;

    XtVaGetValues(selectionList,
		  XmNselectedItemCount, &selected_items_count,
		  XmNselectedItems, &selected_items,
		  NULL);

    int *disp_nrs = new int[selected_items_count + 1];

    int j = 0;
    for (int i = 0; i < selected_items_count; i++)
    {
	String _item;
	XmStringGetLtoR(selected_items[i], LIST_CHARSET, &_item);
	string item(_item);
	XtFree(_item);

	int disp_nr = get_positive_nr(item);
	if (disp_nr >= 0)
	    disp_nrs[j++] = disp_nr;
    }

    disp_nrs[j] = 0;
    return disp_nrs;
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
