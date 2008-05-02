// $Id$ -*- C++ -*-
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
#include <GUI/Image.h>
#include <GUI/Menu.h>
#include <GUI/MenuBar.h>
#include <GUI/Dialog.h>
#endif

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

#if defined(IF_XM)
#define MMRadio MMToggle
#define MMMenuItem MMPush
#define MMCheckItem MMToggle
#define MMRadioItem MMToggle
#else
const MMType MMRadio       = 17; // Create a RadioButton (same as MMToggle for Motif)
const MMType MMMenuItem    = 18; // Create a MenuItem (same as MMPush for Motif)
const MMType MMCheckItem   = 19; // Create a CheckMenuItem (same as MMToggle for Motif)
const MMType MMRadioItem   = 20; // Create a RadioMenuItem (same as MMToggle for Motif)
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
    GUI::ImageHandle *image;
#endif
    MMType type;	     // Widget type
#if defined(IF_XM)
    XtCallbackRec callback;  // Associated callback
#else
    sigc::slot<void, GUI::Widget *> callback;
#endif
    MMDesc *items;	     // Submenus (0 if none)
#if defined(IF_XM)
    Widget *widgetptr;       // Where to store the resulting widget (0 if not)
    Widget widget;	     // The resulting widget
    Widget label;	     // The resulting label
#else
    GUI::Widget **widgetptr;
    GUI::Widget *widget;
    GUI::Label *label;	     // The resulting label
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


#if defined(IF_XM)

// Procs
typedef void (*MMItemProc)(const MMDesc items[], XtPointer closure);


// Creators
Widget         MMcreatePulldownMenu       (Widget parent, const char *name, MMDesc items[],
					   ArgList args = 0, Cardinal arg = 0);
Widget         MMcreateRadioPulldownMenu  (Widget parent, const char *name, MMDesc items[],
					   ArgList args = 0, Cardinal arg = 0);
Widget         MMcreatePopupMenu          (Widget parent, const char *name, MMDesc items[],
					   ArgList args = 0, Cardinal arg = 0);
Widget         MMcreateMenuBar            (Widget parent, const char *name, MMDesc items[],
					   ArgList args = 0, Cardinal arg = 0);
#else

// Procs
typedef void (*MMItemProc)(const MMDesc items[], void *closure);


// Creators
GUI::PulldownMenu  *MMcreatePulldownMenu       (GUI::Container &parent, cpString name, MMDesc items[]);
GUI::PulldownMenu  *MMcreateRadioPulldownMenu  (GUI::Container &parent, cpString name, MMDesc items[]);
GUI::PopupMenu     *MMcreatePopupMenu          (GUI::Widget &parent, cpString name, MMDesc items[]);
GUI::WidgetPtr<GUI::MenuBar> MMcreateMenuBar   (GUI::Container &parent, cpString name, MMDesc items[]);

#endif

#if defined(IF_XM)

Widget         MMcreateWorkArea           (Widget parent, const char *name, MMDesc items[],
					   ArgList args = 0, Cardinal arg = 0);
Widget         MMcreatePanel              (Widget parent, const char *name, MMDesc items[],
					   ArgList args = 0, Cardinal arg = 0);
Widget         MMcreateRadioPanel         (Widget parent, const char *name, MMDesc items[],
					   ArgList args = 0, Cardinal arg = 0);
Widget         MMcreateButtonPanel        (Widget parent, const char *name, MMDesc items[],
					   ArgList args = 0, Cardinal arg = 0);
Widget         MMcreatePushMenu           (Widget parent, const char *name, MMDesc items[],
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

#else

GUI::Container *MMcreateWorkArea(GUI::Dialog *parent, GUI::String name, MMDesc items[]);
GUI::Container *MMcreatePanel(GUI::Container *parent, cpString name, MMDesc items[],
			      GUI::Orientation=GUI::ORIENTATION_VERTICAL);
GUI::Container *MMcreateRadioPanel(GUI::Container *parent, cpString name, MMDesc items[],
				   GUI::Orientation=GUI::ORIENTATION_VERTICAL);
GUI::Container *MMcreateButtonPanel(GUI::Container *parent, cpString name, MMDesc items[],
				    GUI::Orientation=GUI::ORIENTATION_VERTICAL);
GUI::Menu      *MMcreatePushMenu(GUI::Container *parent, GUI::String name, MMDesc items[]);

// Align panel items along their labels
void MMadjustPanel(const MMDesc items[], Dimension space = 15);

// Add callbacks
void MMaddCallbacks(const MMDesc items[],
		    void *default_closure = 0,
		    int depth = -1);

void MMaddHelpCallback(const MMDesc items[], sigc::slot<void, GUI::Widget *> proc, int depth = -1);

// Apply PROC on all ITEMS
void MMonItems(const MMDesc items[], MMItemProc proc, void *closure = 0,
	       int depth = -1);

#endif

// Add ITEMS to SHELL.  If IGNORE_SEPS is set, all separators are ignored.
#if defined(IF_XM)
void MMaddItems(Widget shell, MMDesc items[], bool ignore_seps = false);
#else
void MMaddItems(GUI::Container *shell, MMDesc items[], bool ignore_seps = false);
#endif

// Conveniences
#if defined(IF_MOTIF)
#define MMNoCB { 0, 0 }
#else
extern void dummy_callback(Widget);
#define MMNoCB PTR_FUN(dummy_callback)
#endif

#if defined(IF_XM)
#define MMEnd  { 0, MMPush, MMNoCB, 0, 0, 0, 0 }
#define MMSep  { "separator", MMSeparator, MMNoCB, 0, 0, 0, 0 }
#else
#define MMEnd GENTRYL("", "", MMPush, MMNoCB, MDUMMY, 0, 0)
#define MMSep GENTRYL("separator", "", MMSeparator, MMNoCB, MDUMMY, 0, 0)
#endif

// New resources
#define XtNpushMenuPopupTime  "pushMenuPopupTime"
#define XtCPushMenuPopupTime  "PushMenuPopupTime"

// Helpers

#if defined(IF_XM)
// Set sensitivity of W to STATE
extern void set_sensitive(Widget w, bool state);
#else
// Set sensitivity of W to STATE
extern void set_sensitive(GUI::Widget *w, bool state);
#endif

#if defined(IF_XM)

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

#else

// Manage W iff STATE
inline void manage_child(GUI::Widget *w, bool state)
{
    if (w)
    {
	if (state)
	    w->show();
	else
	    w->hide();
    }
}

#endif

#if !defined(IF_XM)
extern void dummy_xcallback(GUI::Widget *);
#define MDUMMY sigc::ptr_fun(dummy_xcallback)
#endif

// Macros for menu entries.

// MENTRYL: Entry with a label
// MENTRYI: Entry with an icon
// MENTRYLI: Entry with a label and an icon

// Note that in old-style Motif (IF_XM), the label and icon are
// deduced from the Widget name by reference to the resource database.
// In this case the label (s) and image (i) arguments are unused.

// XENTRY*: New-style callbacks using abstract GUI::Widget classes.

#if defined(IF_XM)
#if 1
#define MENTRYL(n,s,t,c,sub,w) { n, t, c, sub, w, 0, 0}
#define MENTRYI(n,i,t,c,sub,w) { n, t, c, sub, w, 0, 0}
#define MENTRYLI(n,s,i,t,c,sub,w) { n, t, c, sub, w, 0, 0}
#endif
#define GENTRYL(n,s,t,c,cx,sub,w) { n, t, c, sub, w, 0, 0}
#define GENTRYI(n,i,t,c,cx,sub,w) { n, t, c, sub, w, 0, 0}
#define GENTRYLI(n,s,i,t,c,cx,sub,w) { n, t, c, sub, w, 0, 0}
#define BIND(f,d) { f, XtPointer(d) }
#elif defined(IF_XMMM)
#if 0
#define MENTRYL(n,s,t,c,sub,w) { n, s, NULL, t, c, sub, w, 0, 0, MDUMMY, 0, 0}
#define MENTRYI(n,i,t,c,sub,w) { n, "", i, t, c, sub, w, 0, 0, MDUMMY, 0, 0}
#define MENTRYLI(n,s,i,t,c,sub,w) { n, s, i, t, c, sub, w, 0, 0, MDUMMY, 0, 0}
#endif
#define GENTRYL(n,s,t,c,cx,sub,w) { n, s, NULL, t, MMNoCB, sub, 0, 0, 0, cx, (Xmmm::Widget **)w, 0}
#define GENTRYI(n,i,t,c,cx,sub,w) { n, "", i, t, MMNoCB, sub, 0, 0, 0, cx, (Xmmm::Widget **)w, 0}
#define GENTRYLI(n,s,i,t,c,cx,sub,w) { n, s, i, t, MMNoCB, sub, 0, 0, 0, cx, (Xmmm::Widget **)w, 0}
#else
#if 0
#define MENTRYL(n,s,t,c,sub,w) { n, s, NULL, t, MDUMMY, sub, 0, 0, 0}
#define MENTRYI(n,i,t,c,sub,w) { n, "", i, t, MDUMMY, sub, 0, 0, 0}
#define MENTRYLI(n,s,i,t,c,sub,w) { n, s, i, t, MDUMMY, sub, 0, 0, 0}
#endif
#define GENTRYL(n,s,t,c,cx,sub,w) { n, s, NULL, t, cx, sub, (GUI::Widget **)w, 0, 0}
#define GENTRYI(n,i,t,c,cx,sub,w) { n, "", i, t, cx, sub, (GUI::Widget **)w, 0, 0}
#define GENTRYLI(n,s,i,t,c,cx,sub,w) { n, s, i, t, cx, sub, (GUI::Widget **)w, 0, 0}
#endif


#endif // _DDD_MakeMenu_h
// DON'T ADD ANYTHING BEHIND THIS #endif
