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

#ifndef XMMM_NOTEBOOK3P_H
#define XMMM_NOTEBOOK3P_H

#include <Xm/ManagerP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define the Notebook3 instance part */
typedef struct {
    int current_pageno;
    WidgetList  buttons;
    Cardinal    num_buttons;
    Cardinal    max_buttons;
    WidgetList  pages;
    Cardinal    num_pages;
    Cardinal    max_pages;
} XmmmNotebook3Part;

/* Define the full instance record */
typedef struct _XmmmNotebook3Rec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmmmNotebook3Part notebook3;
} XmmmNotebook3Rec;

/* Define class part structure */
typedef struct {
    int mumble;
} XmmmNotebook3ClassPart;

/* Define the full class record */
typedef struct _XmmmNotebook3ClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmmmNotebook3ClassPart notebook3_class;
} XmmmNotebook3ClassRec;

/* Define the Constraint Resources */
typedef struct XmmmNotebook3ConstraintPart {
    Cardinal pack_options;
} XmmmNotebook3ConstraintPart;

typedef struct XmmmNotebook3ConstraintRec {
    XmManagerConstraintPart manager;
    XmmmNotebook3ConstraintPart notebook3;
} XmmmNotebook3ConstraintRec, *XmmmNotebook3Constraints;

extern XmmmNotebook3ClassRec xmmmNotebook3ClassRec;

/*
 * easy access macros
 */
#define NB_Buttons(n) \
    (((XmmmNotebook3Widget)(n))->notebook3.buttons)

#define NB_Change(n) \
    (((XmmmNotebook3Widget)(n))->notebook3.change)

#ifdef __cplusplus
}
#endif

#endif /* XMMM_NOTEBOOK3P_H */
