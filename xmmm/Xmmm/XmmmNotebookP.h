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

#ifndef XMMM_XMMMNOTEBOOKP_H
#define XMMM_XMMMNOTEBOOKP_H

#include <Xm/ManagerP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define the XmmmNotebook instance part */
typedef struct {
    int current_pageno;
    WidgetList  buttons;
    Cardinal    num_buttons;
    Cardinal    max_buttons;
    WidgetList  pages;
    Cardinal    num_pages;
    Cardinal    max_pages;
} XmmmNotebookPart;

/* Define the full instance record */
typedef struct _XmmmNotebookRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmmmNotebookPart notebook;
} XmmmNotebookRec;

/* Define class part structure */
typedef struct {
    int mumble;
} XmmmNotebookClassPart;

/* Define the full class record */
typedef struct _XmmmNotebookClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmmmNotebookClassPart notebook_class;
} XmmmNotebookClassRec;

/* Define the Constraint Resources */
typedef struct XmmmNotebookConstraintPart {
    Cardinal pack_options;
} XmmmNotebookConstraintPart;

typedef struct XmmmNotebookConstraintRec {
    XmManagerConstraintPart manager;
    XmmmNotebookConstraintPart notebook;
} XmmmNotebookConstraintRec, *XmmmNotebookConstraints;

extern XmmmNotebookClassRec xmmmNotebookClassRec;

/*
 * easy access macros
 */
#define NB_Buttons(n) \
    (((XmmmNotebookWidget)(n))->notebook.buttons)

#define NB_Change(n) \
    (((XmmmNotebookWidget)(n))->notebook.change)

#ifdef __cplusplus
}
#endif

#endif /* XMMM_XMMMNOTEBOOKP_H */
