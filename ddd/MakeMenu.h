// $Id$
// Simple interface to motif menus

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

// $Log$
// Revision 1.1  1995/05/01 15:47:10  zeller
// Initial revision
//
// Revision 1.16  1995/04/24  11:37:52  zeller
// New: radio menus
//

#ifndef _Nora_MakeMenu_h
#define _Nora_MakeMenu_h

// Main types
typedef int MMType;

const MMType MMPush      = 0;  // create PushButton (default)
const MMType MMToggle    = 1;  // create ToggleButton
const MMType MMMenu      = 2;  // create CascadeButton with menu
const MMType MMSeparator = 3;  // create Separator
const MMType MMLabel     = 4;  // create Label
const MMType MMRadioMenu = 5;  // create CascadeButton with RadioBox menu

const MMType MMTypeMask  = 15; // mask to find type


// Special attributes, to be ORed with types

typedef int MMAttr;

const MMAttr MMHelp        = 16; // this is a help button
const MMAttr MMInsensitive = 32; // make item insensitive
const MMAttr MMUnmanaged   = 64; // make item unmanaged
const MMAttr MMAttrMask    = ~MMTypeMask;



// Description data structure

struct MMDesc {
    String name;		 // widget name
    MMType type;		 // widget type
    XtCallbackRec callback;	 // associated callback
    MMDesc *items;		 // submenus (NULL if none)
    Widget *widgetptr;           // where to store the resulting widget
    Widget widget;		 // the resulting widget
};


// Fields

/*
    Type        Callback                 Items   Widget
    ----------- ------------------------ ------- ---------------------------
    MMPush      XmNactivateCallback      -/-     PushButtonGadget
    MMToggle    XmNvalueChangedCallback  -/-     ToggleButtonGadget
    MMMenu      XmNmapCallback           items   CascadeButtonGadget
    MMSeparator -/-                      -/-     SeparatorGadget
    MMLabel     -/-                      -/-     LabelGadget
*/


// Procs

typedef void (*MMItemProc)(MMDesc items[], XtPointer closure);


// Functions

Widget MMcreatePulldownMenu      (Widget parent, String name, MMDesc items[]);
Widget MMcreateRadioPulldownMenu (Widget parent, String name, MMDesc items[]);
Widget MMcreatePopupMenu         (Widget parent, String name, MMDesc items[]);
Widget MMcreateMenuBar           (Widget parent, String name, MMDesc items[]);
Widget MMcreateWorkArea          (Widget parent, String name, MMDesc items[]);

void   MMaddCallbacks   (MMDesc items[], XtPointer default_closure = 0);
void   MMaddHelpCallback(MMDesc items[], XtCallbackProc proc);
void   MMonItems        (MMDesc items[], MMItemProc proc, 
			 XtPointer closure = 0);

// Conveniences

#define MMNoCB { NULL }
#define MMEnd  { NULL }
#define MMSep  { "separator", MMSeparator }

#endif
