// $Id$
// Own converters

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of the DDD Library.
// 
// The DDD Library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Library General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// The DDD Library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Library General Public License for more details.
// 
// You should have received a copy of the GNU Library General Public
// License along with the DDD Library -- see the file COPYING.LIB.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers at `ddd@ips.cs.tu-bs.de'.

#ifndef _DDD_converters_h
#define _DDD_converters_h

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

// Convert String to XmString, using `@' for font specs
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

// Define a macro: @NAME@ will be replaced by VALUE in CvtStringToXmString
void defineConversionMacro(String name, String value);

#endif // _DDD_converters_h
// DON'T ADD ANYTHING BEHIND THIS #endif
