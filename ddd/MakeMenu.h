// $Id$
// Simple interface to motif menus

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
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

#ifndef _DDD_MakeMenu_h
#define _DDD_MakeMenu_h

#include <X11/Intrinsic.h>
#include "bool.h"
#include "StringA.h"

// Main types
typedef int MMType;

const MMType MMPush        =  0; // Create PushButton (default)
const MMType MMToggle      =  1; // Create ToggleButton
const MMType MMMenu        =  2; // Create CascadeButton with menu
const MMType MMSeparator   =  3; // Create Separator
const MMType MMLabel       =  4; // Create Label
const MMType MMRadioMenu   =  5; // Create CascadeButton with RadioBox menu
const MMType MMOptionMenu  =  6; // Create an option menu
const MMType MMPanel       =  7; // Create a panel
const MMType MMRadioPanel  =  8; // Create a panel with RadioBox menu
const MMType MMButtonPanel =  9; // Like MMRadioPanel, but no radio behavior
const MMType MMScale       = 10; // Create a scale
const MMType MMTextField   = 11; // Create a text field
const MMType MMEnterField  = 12; // Like MMTextField, but use Enter to activate
const MMType MMFlatPush    = 13; // Create `flat' PushButton without shadows
const MMType MMArrow       = 14; // Create an arrow button
const MMType MMSpinBox     = 15; // Like MMTextField, but add two spin buttons
const MMType MMComboBox    = 16; // Create a combo box

const MMType MMTypeMask    = 31; // mask to find type


// Special attributes, to be ORed with types

typedef int MMAttr;

const MMAttr MMHelp           =   32; // Mark as help button
const MMAttr MMInsensitive    =   64; // Make item insensitive
const MMAttr MMUnmanaged      =  128; // Make item unmanaged
const MMAttr MMUnmanagedLabel =  256; // Don't manage label (in panels)
const MMAttr MMIgnore         =  512; // Don't create item
const MMAttr MMVertical       = 1024; // Make panel vertical
const MMAttr MMAttrMask       = ~MMTypeMask;



// Description data structure

struct MMDesc {
    const char *name;	     // Widget name
    MMType type;	     // Widget type
    XtCallbackRec callback;  // Associated callback
    MMDesc *items;	     // Submenus (0 if none)
    Widget *widgetptr;       // Where to store the resulting widget (0 if not)
    Widget widget;	     // The resulting widget
    Widget label;	     // The resulting label
};


// Fields

/*
    Type        Callback                 Items   Widget
    ----------- ------------------------ ------- -------------
    MMPush      XmNactivateCallback      -/-     PushButton
    MMToggle    XmNvalueChangedCallback  -/-     ToggleButton
    MMMenu      XmNmapCallback           items   CascadeButton
    MMSeparator -/-                      -/-     Separator
    MMLabel     -/-                      -/-     Label
*/


// Procs
typedef void (*MMItemProc)(const MMDesc items[], XtPointer closure);


// Creators
Widget MMcreatePulldownMenu      (Widget parent, const _XtString name, MMDesc items[],
				  ArgList args = 0, Cardinal arg = 0);
Widget MMcreateRadioPulldownMenu (Widget parent, const _XtString name, MMDesc items[],
				  ArgList args = 0, Cardinal arg = 0);
Widget MMcreatePopupMenu         (Widget parent, const _XtString name, MMDesc items[],
				  ArgList args = 0, Cardinal arg = 0);
Widget MMcreateMenuBar           (Widget parent, const _XtString name, MMDesc items[],
				  ArgList args = 0, Cardinal arg = 0);
Widget MMcreateWorkArea          (Widget parent, const _XtString name, MMDesc items[],
				  ArgList args = 0, Cardinal arg = 0);
Widget MMcreatePanel             (Widget parent, const _XtString name, MMDesc items[],
				  ArgList args = 0, Cardinal arg = 0);
Widget MMcreateRadioPanel        (Widget parent, const _XtString name, MMDesc items[],
				  ArgList args = 0, Cardinal arg = 0);
Widget MMcreateButtonPanel       (Widget parent, const _XtString name, MMDesc items[],
				  ArgList args = 0, Cardinal arg = 0);
Widget MMcreatePushMenu          (Widget parent, const _XtString name, MMDesc items[],
				  ArgList args = 0, Cardinal arg = 0);


// Align panel items along their labels
void MMadjustPanel(const MMDesc items[], Dimension space = 15);

// Add callbacks
void MMaddCallbacks(const MMDesc items[],
		    XtPointer default_closure = 0,
		    int depth = -1);
void MMaddHelpCallback(const MMDesc items[], XtCallbackProc proc, int depth = -1);

// Apply PROC on all ITEMS
void MMonItems(const MMDesc items[], MMItemProc proc, XtPointer closure = 0,
	       int depth = -1);

// Add ITEMS to SHELL.  If IGNORE_SEPS is set, all separators are ignored.
void MMaddItems(Widget shell, MMDesc items[], bool ignore_seps = false);

// Conveniences
#define MMNoCB { 0, 0 }
#define MMEnd  { 0, MMPush, MMNoCB, 0, 0, 0, 0 }
#define MMSep  { "separator", MMSeparator, MMNoCB, 0, 0, 0, 0 }

// New resources
#define XtNpushMenuPopupTime  "pushMenuPopupTime"
#define XtCPushMenuPopupTime  "PushMenuPopupTime"

// Helpers

// Set sensitivity of W to STATE
extern void set_sensitive(Widget w, bool state);

// Manage W iff STATE
inline void manage_child(Widget w, bool state)
{
    if (w != 0)
    {
	if (state)
	    XtManageChild(w);
	else
	    XtUnmanageChild(w);
    }
}

#endif // _DDD_MakeMenu_h
// DON'T ADD ANYTHING BEHIND THIS #endif
