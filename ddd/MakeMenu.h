// $Id$
// Simple interface to motif menus

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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

#ifndef _DDD_MakeMenu_h
#define _DDD_MakeMenu_h

#include <X11/Intrinsic.h>

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
const MMType MMSpinField   = 13; // Like MMTextField, but add two spin buttons
const MMType MMFlatPush    = 14; // Create `flat' PushButton without shadows
const MMType MMArrow       = 15; // Create an arrow button

const MMType MMTypeMask    = 31; // mask to find type


// Special attributes, to be ORed with types

typedef int MMAttr;

const MMAttr MMHelp        =  32; // This is a help button
const MMAttr MMInsensitive =  64; // Make item insensitive
const MMAttr MMUnmanaged   = 128; // Make item unmanaged
const MMAttr MMIgnore      = 256; // Don't create item
const MMAttr MMAttrMask    = ~MMTypeMask;



// Description data structure

struct MMDesc {
    String name;		 // Widget name
    MMType type;		 // Widget type
    XtCallbackRec callback;	 // Associated callback
    MMDesc *items;		 // Submenus (NULL if none)
    Widget *widgetptr;           // Where to store the resulting widget
    Widget widget;		 // The resulting widget
    Widget label;		 // The resulting label
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

typedef void (*MMItemProc)(MMDesc items[], XtPointer closure);


// Functions
Widget MMcreatePulldownMenu      (Widget parent, String name, MMDesc items[]);
Widget MMcreateRadioPulldownMenu (Widget parent, String name, MMDesc items[]);
Widget MMcreatePopupMenu         (Widget parent, String name, MMDesc items[]);
Widget MMcreateMenuBar           (Widget parent, String name, MMDesc items[]);
Widget MMcreateWorkArea          (Widget parent, String name, MMDesc items[]);
Widget MMcreatePanel             (Widget parent, String name, MMDesc items[]);
Widget MMcreateRadioPanel        (Widget parent, String name, MMDesc items[]);
Widget MMcreateButtonPanel       (Widget parent, String name, MMDesc items[]);

void   MMaddCallbacks   (MMDesc items[], XtPointer default_closure = 0);
void   MMaddHelpCallback(MMDesc items[], XtCallbackProc proc);
void   MMonItems        (MMDesc items[], MMItemProc proc, 
			 XtPointer closure = 0);

// Add items to shell.  If IGNORE_SEPS is set, all separators are ignored.
void MMaddItems(Widget shell, MMDesc items[], bool ignore_seps = false);

// Conveniences
#define MMNoCB { NULL }
#define MMEnd  { NULL }
#define MMSep  { "separator", MMSeparator }

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
