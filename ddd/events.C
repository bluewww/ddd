// $Id$
// Event access

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller (zeller@ips.cs.tu-bs.de).
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

char events_rcsid[] =
    "$Id$";

#include "events.h"


// Event location
BoxPoint point(XEvent *ev)
{
    switch (ev->type)
    {
	case KeyPress:
	case KeyRelease:
	    return BoxPoint(ev->xkey.x,
			    ev->xkey.y);

	case ButtonPress:
	case ButtonRelease:
	    return BoxPoint(ev->xbutton.x,
			    ev->xkey.y);

	case MotionNotify:
	    return BoxPoint(ev->xmotion.x,
			    ev->xmotion.y);

	case EnterNotify:
	case LeaveNotify:
	    return BoxPoint(ev->xcrossing.x,
			    ev->xcrossing.y);

	case Expose:
	    return BoxPoint(ev->xexpose.x,
			    ev->xexpose.y);

	case GraphicsExpose:
	    return BoxPoint(ev->xgraphicsexpose.x,
			    ev->xgraphicsexpose.y);

	case CreateNotify:
	    return BoxPoint(ev->xcreatewindow.x,
			    ev->xcreatewindow.y);

	case ReparentNotify:
	    return BoxPoint(ev->xreparent.x,
			    ev->xreparent.y);

	case ConfigureNotify:
	    return BoxPoint(ev->xconfigure.x,
			    ev->xconfigure.y);

	case GravityNotify:
	    return BoxPoint(ev->xgravity.x,
			    ev->xgravity.y);

	case ConfigureRequest:
	    return BoxPoint(ev->xconfigurerequest.x,
			    ev->xconfigurerequest.y);

	default:
	    cerr << "Warning: cannot extract point from event type "
		 << ev->type << "\n";
	    return BoxPoint(-1, -1);
    }
}


// Event time
Time time(XEvent *ev)
{
    switch (ev->type)
    {
	case KeyPress:
	case KeyRelease:
	    return ev->xkey.time;

	case ButtonPress:
	case ButtonRelease:
	    return ev->xbutton.time;

	case MotionNotify:
	    return ev->xbutton.time;

	case EnterNotify:
	case LeaveNotify:
	    return ev->xcrossing.time;

	case PropertyNotify:
	    return ev->xproperty.time;

	case SelectionClear:
	    return ev->xselectionclear.time;

	case SelectionRequest:
	    return ev->xselectionrequest.time;

	case SelectionNotify:
	    return ev->xselection.time;

	default:
	    cerr << "Warning: cannot extract time from event type "
		 << ev->type << "\n";
	    return 0;
    }
}


// Event size
BoxSize size(XEvent *ev)
{
    switch (ev->type)
    {
	case Expose:
	    return BoxSize(ev->xexpose.width,
			   ev->xexpose.height);

	case GraphicsExpose:
	    return BoxSize(ev->xgraphicsexpose.width,
			   ev->xgraphicsexpose.height);

	case CreateNotify:
	    return BoxSize(ev->xcreatewindow.width,
			   ev->xcreatewindow.height);

	case ConfigureNotify:
	    return BoxSize(ev->xconfigure.width,
			   ev->xconfigure.height);

	case ResizeRequest:
	    return BoxSize(ev->xresizerequest.width,
			   ev->xresizerequest.height);

	case ConfigureRequest:
	    return BoxSize(ev->xconfigurerequest.width, 
			   ev->xconfigurerequest.height);

	default:
	    cerr << "Warning: cannot extract size from event type "
		 << ev->type << "\n";
	    return BoxSize(0, 0);
    }
}
