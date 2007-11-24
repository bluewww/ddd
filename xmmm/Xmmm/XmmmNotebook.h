/* High-level GUI wrapper for Xm. */

/* Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
 * 
 * This file is part of Xmmm.
 * 
 * Xmmm is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * 
 * Xmmm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public
 * License along with Xmmm -- see the file COPYING.
 * If not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

#ifndef XMMM_XMMMNOTEBOOK_H
#define XMMM_XMMMNOTEBOOK_H

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

extern WidgetClass xmmmNotebookWidgetClass;

typedef struct _XmmmNotebookRec *XmmmNotebookWidget;
typedef struct _XmmmNotebookClassRec *XmmmNotebookWidgetClass;

#define XmmmIsNotebook(w) XtIsSubclass((w), xmmmNotebookWidgetClass)

extern Widget XmmmCreateNotebook(Widget parent, char *name, Arg *arglist, Cardinal argcount);
extern void XmmmNotebookInsertChild(Widget w, char *name, Widget child);
extern Widget XmmmNotebookInsertPage(Widget w, char *name, Arg *arglist, Cardinal argcount);
extern Cardinal XmmmNotebookNumPages(Widget w);
extern Widget XmmmNotebookCurrentPage(Widget w);

#ifdef __cplusplus
}
#endif

#endif /* XMMM_XMMMNOTEBOOK_H */
