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

#ifndef XMMM_NOTEBOOK2P_H
#define XMMM_NOTEBOOK2P_H

#include <Xm/RowColumn.h>
#include <Xm/ManagerP.h>
#include <Xm/RowColumnP.h>
#include <Xmmm/Box2P.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define the notebook instance part */
typedef struct {
    int current_pageno;
    Widget buttons;
    Widget frame;
    Widget change;
} XmmmNotebookPart;

/* Define the full instance record */
typedef struct _XmmmNotebookRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmmmBox2Part box2;
    XmmmNotebookPart notebook2;
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
    XmmmBox2ClassPart box2_class;
    XmmmNotebookClassPart notebook2_class;
} XmmmNotebookClassRec;

/* Define the Constraint Resources */
typedef struct XmmmNotebookConstraintPart {
    Cardinal pack_options;
} XmmmNotebookConstraintPart;

typedef struct XmmmNotebookConstraintRec {
    XmManagerConstraintPart manager;
    XmmmBox2ConstraintPart box2;
    XmmmNotebookConstraintPart notebook2;
} XmmmNotebookConstraintRec, *XmmmNotebookConstraints;

extern XmmmNotebookClassRec xmmmNotebookClassRec;

/*
 * easy access macros
 */
#define NB_Buttons(n) \
    (((XmmmNotebookWidget)(n))->notebook2.buttons)

#define NB_Change(n) \
    (((XmmmNotebookWidget)(n))->notebook2.change)

#ifdef __cplusplus
}
#endif

#endif /* XMMM_NOTEBOOK2P_H */
