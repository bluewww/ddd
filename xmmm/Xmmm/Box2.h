/* High-level GUI wrapper for Xm. */

/* Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
 * 
 * This file is part of Xmmm.
 * 
 * Xmmm is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 * 
 * Xmmm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Xmmm -- see the file COPYING.
 * If not, see <http://www.gnu.org/licenses/>.

#ifndef XMMM_BOX2_H
#define XMMM_BOX2_H

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

extern WidgetClass xmmmBox2WidgetClass;

typedef struct _XmmmBox2Rec *XmmmBox2Widget;
typedef struct _XmmmBox2ClassRec *XmmmBox2WidgetClass;

#define XmmmIsBox2(w) XtIsSubclass((w), xmmmBox2WidgetClass)

extern Widget XmmmCreateBox2(Widget parent, char *name, Arg *arglist, Cardinal argcount);

#ifdef __cplusplus
}
#endif

#endif /* XMMM_BOX2_H */
