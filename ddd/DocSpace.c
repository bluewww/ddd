/* $Id$ */
/* DocSpace-Widget (Implementation) */

/*
    Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
    Written by Andreas Zeller <zeller@gnu.org>.

    This file is part of DDD.

    DDD is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public
    License as published by the Free Software Foundation; either
    version 3 of the License, or (at your option) any later version.

    DDD is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with DDD -- see the file COPYING.
    If not, see <http://www.gnu.org/licenses/>.

    DDD is the data display debugger.
    For details, see the DDD World-Wide-Web page, 
    `http://www.gnu.org/software/ddd/',
    or send a mail to the DDD developers <ddd@gnu.org>.
*/

char DocSpace_rcsid[] = 
    "$Id$";

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "DocSpaceP.h"

static XtResource resources[] = {
#define offset(field) XtOffset(DocSpaceWidget, docSpace.field)
    /* {name, class, type, size, offset, default_type, default_addr}, */
    { XtNexposeCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
    offset(expose_callback), XtRCallback, (XtPointer)NULL },
    { XtNcallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
    offset(input_callback), XtRCallback, (XtPointer)NULL },
    { XtNquitCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
    offset(quit_callback), XtRCallback, (XtPointer)NULL }
#undef offset
};

static void InputAction(Widget w, XEvent *event, 
			String params[], Cardinal *num_params)
{
    XtCallCallbacks(w, XtNcallback, (XtPointer)event);
}

static void Redisplay(Widget w, XEvent *event, Region region)
{
    XtCallCallbacks(w, XtNexposeCallback, (XtPointer)event);
}

static void QuitAction(Widget w, XEvent *event, 
		       String params[], Cardinal *num_params)
{
    XtCallCallbacks(w, XtNquitCallback, (XtPointer)event);
}

static XtActionsRec actions[] =
{
  /* {name, procedure}, */
    {"input",   InputAction},
    {"quit",    QuitAction},
};

static char translations[] =
"<BtnDown>:     input() \n\
 <Key>q:        quit() \n\
 <Key>Escape:   quit() \n\
";

DocSpaceClassRec docSpaceClassRec = {
  { /* core fields */
    /* superclass           */  (WidgetClass) &widgetClassRec,
    /* class_name           */  "DocSpace",
    /* widget_size          */  sizeof(DocSpaceRec),
    /* class_initialize     */  (XtProc)NULL,
    /* class_part_initialize*/  (XtWidgetClassProc)NULL,
    /* class_inited         */  FALSE,
    /* initialize           */  (XtInitProc)NULL,
    /* initialize_hook      */  (XtArgsProc)NULL,
    /* realize              */  XtInheritRealize,
    /* actions              */  actions,
    /* num_actions          */  XtNumber(actions),
    /* resources            */  resources,
    /* num_resources        */  XtNumber(resources),
    /* xrm_class            */  NULLQUARK,
    /* compress_motion      */  TRUE,
    /* compress_exposure    */  TRUE,
    /* compress_enterleave  */  TRUE,
    /* visible_interest     */  FALSE,
    /* destroy              */  (XtWidgetProc)NULL,
    /* resize               */  (XtWidgetProc)NULL,
    /* expose               */  Redisplay,
    /* set_values           */  (XtSetValuesFunc)NULL,
    /* set_values_hook      */  (XtArgsFunc)NULL,
    /* set_values_almost    */  XtInheritSetValuesAlmost,
    /* get_values_hook      */  (XtArgsProc)NULL,
    /* accept_focus         */  (XtAcceptFocusProc)NULL,
    /* version              */  XtVersion,
    /* callback_private     */  (XtPointer)NULL,
    /* tm_table             */  translations,
    /* query_geometry       */  XtInheritQueryGeometry,
    /* display_accelerator  */  XtInheritDisplayAccelerator,
    /* extension            */  (XtPointer)NULL
  },
  { /* docSpace fields */
    /* empty                */  0
  }
};

WidgetClass docSpaceWidgetClass = (WidgetClass)&docSpaceClassRec;
