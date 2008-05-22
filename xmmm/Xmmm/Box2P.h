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

#ifndef XMMM_BOX2P_H
#define XMMM_BOX2P_H

#include <Xm/RowColumn.h>
#include <Xm/ManagerP.h>
#include <Xm/RowColumnP.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Define the box2 instance part */
typedef struct {

    Dimension spacing;

    XtCallbackList entry_callback;
    XtCallbackList map_callback;
    XtCallbackList unmap_callback;

    unsigned char orientation;

    Boolean homogeneous;
    Boolean resize_width;
    Boolean resize_height;

    Dimension old_width;
    Dimension old_height;
    Dimension old_shadow_thickness;

} XmmmBox2Part;

/* Define the full instance record */
typedef struct _XmmmBox2Rec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmmmBox2Part box_2;
} XmmmBox2Rec;

/* Define class part structure */
typedef struct {
    int mumble;
} XmmmBox2ClassPart;

/* Define the full class record */
typedef struct _XmmmBox2ClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmmmBox2ClassPart box_2_class;
} XmmmBox2ClassRec;

/* Define the Constraint Resources */
typedef struct XmmmBox2ConstraintPart {
    Cardinal pack_options;
} XmmmBox2ConstraintPart;

typedef struct XmmmBox2ConstraintRec {
    XmManagerConstraintPart manager;
    XmmmBox2ConstraintPart box_2;
} XmmmBox2ConstraintRec, *XmmmBox2Constraints;

extern XmmmBox2ClassRec xmmmBox2ClassRec;

/*
 * easy access macros
 */
#define B2_Orientation(m) \
    (((XmmmBox2Widget)(m))->box_2.orientation)

#define B2_Spacing(m) \
    (((XmmmBox2Widget)(m))->box_2.spacing)

#define B2_OldWidth(m) \
    (((XmmmBox2Widget)(m))->box_2.old_width)

#define B2_OldHeight(m) \
    (((XmmmBox2Widget)(m))->box_2.old_height)

#define B2_OldShadowThickness(m) \
    (((XmmmBox2Widget)(m))->box_2.old_shadow_thickness)

#define B2_ResizeHeight(m) \
    (((XmmmBox2Widget)(m))->box_2.resize_height)

#define B2_ResizeWidth(m) \
    (((XmmmBox2Widget)(m))->box_2.resize_width)

#define B2_IsHomogeneous(m) \
    (((XmmmBox2Widget)(m))->box_2.homogeneous)

#define B2_Boxes(m) \
    (((XmmmBox2Widget)(m))->box_2.boxes)

    XtGeometryResult _XmmmB2AdjustSize(Widget, Widget, XtWidgetGeometry *);

#ifdef __cplusplus
}
#endif

#endif /* XMMM_BOX2P_H */
