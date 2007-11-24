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

#ifndef _DDD_MakeMenu_h
#define _DDD_MakeMenu_h

#if defined(IF_MOTIF)

#include <X11/Intrinsic.h>

#endif

#include "gtk_wrapper.h"

#if !defined(IF_XM)
#include <GUI/Container.h>
#include <GUI/Label.h>
#include <GUI/Box.h>
#endif

#include "bool.h"
#include "StringA.h"

#if defined(IF_XM)
#define NM(a,b) a
#define IM(a,b) a
#define NIM(a,b,c) a
#else
#define NM(a,b) a, b, NULL
#define IM(a,b) a, "", b
#define NIM(a,b,c) a, b, c
#endif

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

#if defined(IF_MOTIF)
#define MMMenuItem MMPush
#define MMCheckItem MMToggle
#define MMRadio MMToggle
#else
const MMType MMMenuItem    = 17; // Create a MenuItem (same as MMPush for Motif)
const MMType MMCheckItem   = 18; // Create a CheckMenuItem (same as MMToggle for Motif)
const MMType MMRadio       = 19; // Create a RadioButton (same as MMToggle for Motif)
#endif

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
#if defined(IF_XM)
    const char *name;	     // Widget name
#else
    GUI::String name;	     // Widget name
    GUI::String label_string;
    XIMAGE_P *image;
#endif
    MMType type;	     // Widget type
    XtCallbackRec callback;  // Associated callback
    MMDesc *items;	     // Submenus (0 if none)
    Widget *widgetptr;       // Where to store the resulting widget (0 if not)
    Widget widget;	     // The resulting widget
    LABEL_P label;	     // The resulting label
#if !defined(IF_XM)
    GUI::Widget **xwidgetptr;
    GUI::Widget *xwidget;
    GUI::Label *xlabel;	     // The resulting label
#endif
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
MENU_P       MMcreatePulldownMenu        (CONTAINER_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
					  , ArgList args = 0, Cardinal arg = 0
#endif
					  );
MENU_P       MMcreateRadioPulldownMenu   (CONTAINER_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
					  , ArgList args = 0, Cardinal arg = 0
#endif
					  );
MENU_P       MMcreatePopupMenu           (Widget parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
					  , ArgList args = 0, Cardinal arg = 0
#endif
					  );
MENUBAR_P    MMcreateMenuBar             (CONTAINER_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
					  , ArgList args = 0, Cardinal arg = 0
#endif
					  );
CONTAINER_P  MMcreateWorkArea            (DIALOG_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
					  , ArgList args = 0, Cardinal arg = 0
#endif
					  );
#if defined(IF_XM)
BOX_P        MMcreatePanel               (CONTAINER_P parent, NAME_T name, MMDesc items[],
					  ArgList args = 0, Cardinal arg = 0);
BOX_P        MMcreateRadioPanel          (CONTAINER_P parent, NAME_T name, MMDesc items[],
					  ArgList args = 0, Cardinal arg = 0);
BOX_P        MMcreateButtonPanel         (CONTAINER_P parent, NAME_T name, MMDesc items[],
					  ArgList args = 0, Cardinal arg = 0);
//BOX_P        MMcreateVButtonPanel        (CONTAINER_P parent, NAME_T name, MMDesc items[],
//					  ArgList args = 0, Cardinal arg = 0);
#else
GUI::Container *MMcreatePanel(GUI::Container *parent, cpString name, MMDesc items[]);
GUI::Container *MMcreateRadioPanel(GUI::Container *parent, cpString name, MMDesc items[]);
GUI::Container *MMcreateButtonPanel(GUI::Container *parent, cpString name, MMDesc items[]);
GUI::Container *MMcreateVButtonPanel(CONTAINER_P parent, NAME_T name, MMDesc items[]);
#endif
MENU_P       MMcreatePushMenu            (CONTAINER_P parent, NAME_T name, MMDesc items[]
#if defined(IF_MOTIF)
					  , ArgList args = 0, Cardinal arg = 0
#endif
					  );

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
void MMaddItems(CONTAINER_P shell,
#if !defined(IF_XM)
		GUI::Container *xshell,
#endif
		MMDesc items[], bool ignore_seps = false);

#if !defined(IF_XM)
void MMaddItems(GUI::Container *shell, MMDesc items[], bool ignore_seps = false);
#endif

// Conveniences
#if defined(IF_MOTIF)
#define MMNoCB { 0, 0 }
#else
extern void dummy_callback(Widget);
#define MMNoCB PTR_FUN(dummy_callback)
#endif

#if defined(XM)
#define MMEnd  { 0, MMPush, MMNoCB, 0, 0, 0, 0 }
#define MMSep  { "separator", MMSeparator, MMNoCB, 0, 0, 0, 0 }
#else
#define MMEnd  { "", "", NULL, MMPush, MMNoCB, 0, 0, 0, 0 }
#define MMSep  { "separator", "", NULL, MMSeparator, MMNoCB, 0, 0, 0, 0 }
#endif

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

#if defined(IF_XM)
#define MENTRY(n,s,t,c,sub,w) { NM(n,s), t, c, sub, (Widget *)w, 0, 0}
#define IMENTRY(n,i,t,c,sub,w) { IM(n,i), t, c, sub, (Widget *)w, 0, 0}
#define NIMENTRY(n,s,i,t,c,sub,w) { NIM(n,s,i), t, c, sub, (Widget *)w, 0, 0}
#define MENTRYX(n,s,t,c,sub,w) { NM(n,s), t, c, sub, (Widget *)w, 0, 0}
#define IMENTRYX(n,i,t,c,sub,w) { IM(n,i), t, c, sub, (Widget *)w, 0, 0}
#define NIMENTRYX(n,s,i,t,c,sub,w) { NIM(n,s,i), t, c, sub, (Widget *)w, 0, 0}
#elif defined(IF_XMMM)
#define MENTRY(n,s,t,c,sub,w) { NM(n,s), t, c, sub, w, 0, 0, 0, 0}
#define IMENTRY(n,i,t,c,sub,w) { IM(n,i), t, c, sub, w, 0, 0, 0, 0}
#define NIMENTRY(n,s,i,t,c,sub,w) { NIM(n,s,i), t, c, sub, w, 0, 0, 0, 0}
#define MENTRYX(n,s,t,c,sub,w) { NM(n,s), t, c, sub, 0, 0, 0, (Xmmm::Widget **)w, 0}
#define IMENTRYX(n,i,t,c,sub,w) { IM(n,i), t, c, sub, 0, 0, 0, (Xmmm::Widget **)w, 0}
#define NIMENTRYX(n,s,i,t,c,sub,w) { NIM(n,s,i), t, c, sub, 0, 0, 0, (Xmmm::Widget **)w, 0}
#else
#define MENTRY(n,s,t,c,sub,w) { NM(n,s), t, c, sub, (Widget *)w, 0, 0, 0}
#define IMENTRY(n,i,t,c,sub,w) { IM(n,i), t, c, sub, (Widget *)w, 0, 0, 0}
#define NIMENTRY(n,s,i,t,c,sub,w) { NIM(n,s,i), t, c, sub, (Widget *)w, 0, 0, 0}
#define MENTRYX(n,s,t,c,sub,w) { NM(n,s), t, c, sub, 0, 0, 0, (GUI::Widget **)w, 0, 0}
#define IMENTRYX(n,i,t,c,sub,w) { IM(n,i), t, c, sub, 0, 0, 0, (GUI::Widget **)w, 0, 0}
#define NIMENTRYX(n,s,i,t,c,sub,w) { NIM(n,s,i), t, c, sub, 0, 0, 0, (GUI::Widget **)w, 0, 0}
#endif


#endif // _DDD_MakeMenu_h
// DON'T ADD ANYTHING BEHIND THIS #endif
