/* $Id$ */
/* DocSpace-Widget (Implementation) */

/*
    Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
    Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).

    This file is part of the NORA Library.

    The NORA Library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    The NORA Library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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
    offset(expose_callback), XtRCallback, NULL },
    { XtNcallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
    offset(input_callback), XtRCallback, NULL },
    { XtNquitCallback, XtCCallback, XtRCallback, sizeof(XtCallbackList),
    offset(quit_callback), XtRCallback, NULL }
#undef offset
};

static void InputAction(Widget w, XEvent *event, 
			String params[], Cardinal *num_params)
{
    XtCallCallbacks(w, XtNcallback, (caddr_t) event);
}

static void Redisplay(Widget w, XEvent *event, Region region)
{
    XtCallCallbacks(w, XtNexposeCallback, (caddr_t) event);
}

static void QuitAction(Widget w, XEvent *event, 
		       String params[], Cardinal *num_params)
{
    XtCallCallbacks(w, XtNquitCallback, (caddr_t) event);
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
    /* class_initialize     */  NULL,
    /* class_part_initialize*/  NULL,
    /* class_inited         */  FALSE,
    /* initialize           */  NULL,
    /* initialize_hook      */  NULL,
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
    /* destroy              */  NULL,
    /* resize               */  NULL,
    /* expose               */  Redisplay,
    /* set_values           */  NULL,
    /* set_values_hook      */  NULL,
    /* set_values_almost    */  XtInheritSetValuesAlmost,
    /* get_values_hook      */  NULL,
    /* accept_focus         */  NULL,
    /* version              */  XtVersion,
    /* callback_private     */  NULL,
    /* tm_table             */  translations,
    /* query_geometry       */  XtInheritQueryGeometry,
    /* display_accelerator  */  XtInheritDisplayAccelerator,
    /* extension            */  NULL
  },
  { /* docSpace fields */
    /* empty                */  0
  }
};

WidgetClass docSpaceWidgetClass = (WidgetClass)&docSpaceClassRec;
