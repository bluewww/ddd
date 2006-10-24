// $Id$
// Event access

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char events_rcsid[] =
    "$Id$";

#include "config.h"

#include "events.h"

static void invalid_event(const char *func)
{
    std::cerr << func << ": invalid event\n";
}

// Event location
BoxPoint point(XEvent *ev)
{
    if (ev == 0)
    {
	// LessTif 0.79 and Motif 1.1 sometimes pass 0 as event;
	// provide some reasonable default
	invalid_event("point");
	return BoxPoint();
    }

#ifdef NAG_ME
#warning Most Gdk events do not have an associated (x,y)
#endif
    switch (ev->type)
    {
#ifdef IF_MOTIF
	case KeyPress:
	case KeyRelease:
	    return BoxPoint(ev->xkey.x,
			    ev->xkey.y);

	case ButtonPress:
	case ButtonRelease:
	    return BoxPoint(ev->xbutton.x,
			    ev->xbutton.y);

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
#else // NOT IF_MOTIF

	case KeyPress:
	case KeyRelease:
	    return BoxPoint(0, 0);

	case ButtonPress:
	case ButtonRelease:
	    return BoxPoint((BoxCoordinate)ev->button.x,
			    (BoxCoordinate)ev->button.y);

	case MotionNotify:
	    return BoxPoint((BoxCoordinate)ev->motion.x,
			    (BoxCoordinate)ev->motion.y);

	case EnterNotify:
	case LeaveNotify:
	    return BoxPoint((BoxCoordinate)ev->crossing.x,
			    (BoxCoordinate)ev->crossing.y);

	case Expose:
	    return BoxPoint(0, 0);

	case ConfigureNotify:
	    return BoxPoint((BoxCoordinate)ev->configure.x,
			    (BoxCoordinate)ev->configure.y);
#endif // IF_MOTIF

	default:
	    invalid_event("point");
	    return BoxPoint();
    }
}


// Event time
Time time(XEvent *ev)
{
    if (ev == 0)
    {
	// LessTif 0.79 and Motif 1.1 sometimes pass 0 as event;
	// provide some reasonable default
	invalid_event("time");
	return CurrentTime;
    }

    switch (ev->type)
    {
#ifdef IF_MOTIF
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

#else // NOT IF_MOTIF
	case KeyPress:
	case KeyRelease:
	    return ev->key.time;

	case ButtonPress:
	case ButtonRelease:
	    return ev->button.time;

	case MotionNotify:
	    return ev->button.time;

	case EnterNotify:
	case LeaveNotify:
	    return ev->crossing.time;

	case PropertyNotify:
	    return ev->property.time;

	case SelectionClear:
	    return ev->selection.time;

	case SelectionRequest:
	    return ev->selection.time;

	case SelectionNotify:
	    return ev->selection.time;
#endif // IF_MOTIF

	default:
	    invalid_event("time");
	    return CurrentTime;
    }
}


// Event size
BoxSize size(XEvent *ev)
{
    if (ev == 0)
    {
	// LessTif 0.79 and Motif 1.1 sometimes pass 0 as event;
	// provide some reasonable default
	invalid_event("size");
	return BoxSize(0, 0);
    }

    switch (ev->type)
    {
#ifdef IF_MOTIF
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

#else // NOT IF_MOTIF
#ifdef NAG_ME
#warning Width and height not available for most event types.
#endif
	case ConfigureNotify:
	    return BoxSize(ev->configure.width,
			   ev->configure.height);

	case ConfigureRequest:
	    return BoxSize(ev->configure.width, 
			   ev->configure.height);

#endif // IF_MOTIF
	default:
	    invalid_event("size");
	    return BoxSize(0, 0);
    }
}
