// $Id$  -*- C++ -*-
// Interactive Help Callbacks

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
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
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

#ifndef _DDD_HelpCB_h
#define _DDD_HelpCB_h

#include <X11/Intrinsic.h>
#include <Xm/Text.h>
#include "MString.h"
#include "bool.h"

// Resources
// Text to be displayed in context-sensitive help (via HelpOnContextCB)
#define XtNhelpString "helpString"
#define XtCHelpString "HelpString"

// Text to be displayed in `Help On Version'
#define XtNhelpOnVersionString "helpOnVersionString"
#define XtCHelpOnVersionString "HelpOnVersionString"

// Text to be displayed in button tips (via InstallTips)
#define XtNtipString  "tipString"
#define XtCTipString  "TipString"

// Text to be displayed in the status line (via DisplayDocumentation)
#define XtNdocumentationString  "documentationString"
#define XtCDocumentationString  "DocumentationString"

// If true, give widget names in context-sensitive help
#define XtNhelpShowTitle "helpShowTitle"
#define XtCHelpShowTitle "HelpShowTitle"


// Select widget and call help on context.
// May be used in a menu entry "Help On Context".
extern void HelpOnContextCB(Widget widget, XtPointer client_data, 
			    XtPointer call_data);

// Call help on current shell window.
// May be used in a menu entry "Help On Window".
extern void HelpOnWindowCB(Widget widget, XtPointer client_data, 
			   XtPointer call_data);

// Call help on the top-level window.
// May be used in a menu entry "Help On Version".
extern void HelpOnVersionCB(Widget widget, XtPointer client_data, 
			    XtPointer call_data);

// Call help on help.
// May be used in a menu entry "Help On Help".
extern void HelpOnHelpCB(Widget widget, XtPointer client_data, 
			 XtPointer call_data);

// Call help for associated widget.
// May be used as help callback for any primitive widget.
extern void ImmediateHelpCB(Widget widget, XtPointer client_data, 
			    XtPointer call_data);

// Call help for widget given in "Widget w = (Widget)client_data".
extern void HelpOnThisCB(Widget widget, XtPointer client_data, 
			 XtPointer call_data);

// Call help with "XmString s = (XmString)client_data" as text.
// May be used for unchanged text display.
extern void MStringHelpCB(Widget widget, XtPointer client_data, 
			  XtPointer call_data);

// Call help with "String s = (String)client_data" as text.
// May be used for unchanged text display.
extern void StringHelpCB(Widget widget, XtPointer client_data, 
			 XtPointer call_data);

// Call help with "String s = (String)client_data" as text.
// May be used for unchanged text display.
extern void TextHelpCB(Widget widget, XtPointer client_data, 
		       XtPointer call_data);

// Call help with a built-in formatted manual page "String s =
// (String)client_data".
extern void ManualStringHelpCB(Widget widget, XtPointer client_data, 
			       XtPointer call_data);
extern void ManualStringHelpCB(Widget widget, const MString& title,
			       const string& text);

// (Un)install button tips on W.
extern void InstallButtonTips(Widget w, bool install = true);

// (Un)install text tips on W.
extern void InstallTextTips(Widget w, bool install = true);

// Enable or disable all installed button tips.
extern void EnableButtonTips(bool enable = true);

// Enable or disable all installed text tips.
extern void EnableTextTips(bool enable = true);

// Enable or disable all installed button docs.
extern void EnableButtonDocs(bool enable = true);

// Enable or disable all installed text docs.
extern void EnableTextDocs(bool enable = true);


// Data

// Create a help text if the `help' resource is empty
extern MString (*DefaultHelpText)(Widget widget);

// Create a help text if the `tipString' resource is empty
extern MString (*DefaultTipText)(Widget widget, XEvent *event);

// Create a help text for the documentation if the `documentationString'
// resource is empty
extern MString (*DefaultDocumentationText)(Widget widget, XEvent *event);

// Return a text position associated with this event
extern XmTextPosition (*TextPosOfEvent)(Widget widget, XEvent *event);

// Hook for displaying documentation
extern void (*DisplayDocumentation)(const MString& doc);

// Pixmap to display at ``help on version''
extern Pixmap (*helpOnVersionPixmapProc)(Widget w);

// Additional text to display at ``help on version''
extern MString helpOnVersionExtraText;

// Delay times (in ms)
extern int help_button_tip_delay; // delay before raising button tip
extern int help_value_tip_delay;  // delay before raising value tip
extern int help_button_doc_delay; // delay before showing button doc
extern int help_value_doc_delay;  // delay before showing value doc
extern int help_clear_doc_delay;  // delay before clearing doc
extern int help_clear_tip_delay;  // delay before clearing tip

#endif // _DDD_HelpCB_h
// DON'T ADD ANYTHING BEHIND THIS #endif
