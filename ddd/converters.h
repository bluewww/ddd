// $Id$
// Own converters

// Copyright (C) 1993 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
// 
// This file is part of the NORA Library.
// 
// The NORA Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The NORA Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the NORA Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 675 Mass Ave, Cambridge, MA 02139, USA.
// 
// NORA is an experimental inference-based software development
// environment. Contact nora@ips.cs.tu-bs.de for details.

#ifndef _Nora_converters_h
#define _Nora_converters_h

// Convert String to Widget
extern Boolean CvtStringToWidget(Display *display, 
				 XrmValue *args, Cardinal *num_args, 
				 XrmValue *fromVal, XrmValue *toVal,
				 XtPointer *converter_data);

// Convert String to Pixmap, converting 1s and 0s to fg/bg color
extern Boolean CvtStringToPixmap(Display *display, 
				 XrmValue *args, Cardinal *num_args, 
				 XrmValue *fromVal, XrmValue *toVal,
				 XtPointer *converter_data);

// Convert String to Bitmap, leaving 1s and 0s untouched
extern Boolean CvtStringToBitmap(Display *display, 
				 XrmValue *args, Cardinal *num_args, 
				 XrmValue *fromVal, XrmValue *toVal,
				 XtPointer *converter_data);

// Convert String to XmString, using @ for font specs
extern Boolean CvtStringToXmString(Display *display, 
				   XrmValue *args, Cardinal *num_args, 
				   XrmValue *fromVal, XrmValue *toVal,
				   XtPointer *converter_data);

// Convert String to Alignment
extern Boolean CvtStringToAlignment(Display *display, 
				    XrmValue *args, Cardinal *num_args, 
				    XrmValue *fromVal, XrmValue *toVal,
				    XtPointer *converter_data);

// Convert String to Orientation
extern Boolean CvtStringToOrientation(Display *display, 
				      XrmValue *args, Cardinal *num_args, 
				      XrmValue *fromVal, XrmValue *toVal,
				      XtPointer *converter_data);

// Convert String to Packing
extern Boolean CvtStringToPacking(Display *display, 
				  XrmValue *args, Cardinal *num_args, 
				  XrmValue *fromVal, XrmValue *toVal,
				  XtPointer *converter_data);

// Register all converters
extern void registerOwnConverters();

#endif
