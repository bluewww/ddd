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

/*
  This is just a simple hack widget.  Basically you instantiale
  an instance of it, and then you can choose whether you want
  all of your Label and LabelGadget widgets and descendents to
  have a 3d insensitive border instead of the standard "50_foreground".

  If the depth is <= 4 it will just use the regular stuff since it
  probably won't look right.

  This widget only has one resource
  
  XmNinsensitive3D: (on/off)
*/

#ifndef _LabelH_h
#define _LabelH_h

#include <Xm/Xm.h>

#ifndef XmIsLabelHack
#define XmIsLabelHack(w) XtIsSubclass((Widget)w, xmLabelHackWidgetClass)
#endif 

extern WidgetClass xmLabelHackWidgetClass;

typedef struct _XmLabelHackClassRec *XmLabelHackWidgetClass;
typedef struct _XmLabelHackRec  *XmLabelHackWidget;

#ifndef XmNinsensitive3D
#define XmNinsensitive3D "insensitive3D"
#define XmCInsensitive3D "Insensitive3D"
#endif

#endif /* _LabelH_h */
