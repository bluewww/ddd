// $Id$ -*- C++ -*-
// Show startup tips

// Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
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

char tips_rcsid[] = 
    "$Id$";

#ifdef __GNUG__
#pragma implementation
#endif

#include "tips.h"

#include "AppData.h"
#include "Command.h"
#include "DestroyCB.h"
#include "HelpCB.h"
#include "cook.h"
#include "ddd.h"
#include "post.h"
#include "session.h"
#include "string-fun.h"
#include "verify.h"
#include "wm.h"

#include <iostream.h>
#include <fstream.h>

#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>


static MString get_startup_tip(Widget w, int n)
{
    struct tip_resource_values {
	XmString tip;
    };

    string tip_name = "tip" + itostring(n);

    XtResource r;
    r.resource_name   = tip_name;
    r.resource_class  = "Tip";
    r.resource_type   = XmRXmString;
    r.resource_size   = sizeof(XmString);
    r.resource_offset = XtOffsetOf(tip_resource_values, tip);
    r.default_type    = XtRImmediate;
    r.default_addr    = XtPointer(0);

    tip_resource_values values;
    XtGetApplicationResources(w, &values, &r, 1, NULL, 0);

    return MString(values.tip, true);
}

static string app_value(string resource, const string& value)
{
    String app_name;
    String app_class;
    XtGetApplicationNameAndClass(XtDisplay(find_shell()), 
				 &app_name, &app_class);

    if (resource.contains(string(app_name) + ".", 0))
	return string(app_class) + resource.from(".") + ": " + value;
    else
	return string(app_class) + "*" + resource + ": " + value;
}

static void SaveTipCountCB(Widget, XtPointer = 0, XtPointer = 0)
{
    create_session_dir(DEFAULT_SESSION);
    const string file = session_tips_file();

    ofstream os(file);
    os << 
	"! " DDD_NAME " tips file\n"
	"\n"
       << app_value(XtNstartupTipCount,
		    itostring(++app_data.startup_tip_count)) << "\n";

    os.close();
    if (os.bad())
    {
	post_error("Cannot save tip count in " + quote(file),
		   "options_save_error");
    }
}

inline bool is_tip(const MString& m)
{
    return !m.isNull() && !m.isEmpty();
}

static bool refresh_tip_dialog(Widget w)
{
    MString tip = get_startup_tip(w, app_data.startup_tip_count);
    if (!is_tip(tip))
    {
	// Restart from first tip
	app_data.startup_tip_count = 1;
	tip = get_startup_tip(w, app_data.startup_tip_count);
    }
    if (!is_tip(tip))
	return false;

    XtVaSetValues(w, XmNmessageString, tip.xmstring(), NULL);

    MString next_tip = get_startup_tip(w, app_data.startup_tip_count + 1);
    MString prev_tip = get_startup_tip(w, app_data.startup_tip_count - 1);

    XtSetSensitive(XmMessageBoxGetChild(w, XmDIALOG_OK_BUTTON),
		   is_tip(prev_tip));
    XtSetSensitive(XmMessageBoxGetChild(w, XmDIALOG_CANCEL_BUTTON),
		   is_tip(next_tip));

    string title = DDD_NAME " Tip of the Day #" + 
	itostring(app_data.startup_tip_count);
    XtVaSetValues(XtParent(w), XmNtitle, title.chars(), NULL);

    return true;
}

static void PrevTipCB(Widget w, XtPointer, XtPointer)
{
    app_data.startup_tip_count--;
    refresh_tip_dialog(w);
}

static void NextTipCB(Widget w, XtPointer, XtPointer)
{
    app_data.startup_tip_count++;
    refresh_tip_dialog(w);
}

void SetStartupTipsCB(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmToggleButtonCallbackStruct *info = 
	(XmToggleButtonCallbackStruct *)call_data;

    app_data.startup_tips = info->set;

    update_options();
}

void TipOfTheDayCB(Widget w, XtPointer, XtPointer)
{
    static Widget tip_dialog = 0;

    if (tip_dialog == 0)
    {
	Arg args[10];
	Cardinal arg = 0;
	XtSetArg(args[arg], XmNautoUnmanage, False); arg++;
	tip_dialog = 
	    verify(XmCreateInformationDialog(find_shell(w), "tip_dialog", 
					     args, arg));

	XtAddCallback(tip_dialog, XmNokCallback,     PrevTipCB, 0);
	XtAddCallback(tip_dialog, XmNcancelCallback, NextTipCB, 0);
	XtAddCallback(tip_dialog, XmNunmapCallback,  SaveTipCountCB, 0);
	XtAddCallback(tip_dialog, XmNhelpCallback,
		      UnmanageThisCB, XtPointer(tip_dialog));
    }

    bool ok = refresh_tip_dialog(tip_dialog);
    if (!ok)
	return;

    manage_and_raise(tip_dialog);
}
