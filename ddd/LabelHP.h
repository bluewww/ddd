/* $Id$ */
/*
 * Copyright 1998 John L. Cwikla
 *
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appears in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of John L. Cwikla or
 * Wolfram Research, Inc not be used in advertising or publicity
 * pertaining to distribution of the software without specific, written
 * prior permission.  John L. Cwikla and Wolfram Research, Inc make no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 * John L. Cwikla and Wolfram Research, Inc disclaim all warranties with
 * regard to this software, including all implied warranties of
 * merchantability and fitness, in no event shall John L. Cwikla or
 * Wolfram Research, Inc be liable for any special, indirect or
 * consequential damages or any damages whatsoever resulting from loss of
 * use, data or profits, whether in an action of contract, negligence or
 * other tortious action, arising out of or in connection with the use or
 * performance of this software.
 *
 * Author:
 *  John L. Cwikla
 *  X Programmer
 *  Wolfram Research Inc.
 *
 *  cwikla@wolfram.com
*/


#ifndef _LabelHP_h
#define _LabelHP_h

#include <Xm/XmP.h>
#include <Xm/LabelP.h>

#include "LabelH.h"

typedef struct _XmLabelHackClassPart
{
	int on;
} XmLabelHackClassPart;


typedef struct _XmLabelHackClassRec
{
	CoreClassPart core_class;
	XmPrimitiveClassPart primitive_class;
	XmLabelClassPart label_class;
	XmLabelHackClassPart label_hack_class;
} XmLabelHackClassRec;

externalref XmLabelHackClassRec xmLabelHackWidgetClassRec;

typedef struct _XmLabelHackPart
{
	Boolean insensitive3D;
	GC topGC, bottomGC;
} XmLabelHackPart;


/*  Full instance record declaration  */

typedef struct _XmLabelHackRec
{
	CorePart core;
	XmPrimitivePart primitive;
	XmLabelPart label;
	XmLabelHackPart label_hack;
} XmLabelHackRec;


#endif  /* LabelHP_h */
