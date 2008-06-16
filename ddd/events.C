// $Id$
// Event access

// Copyright (C) 1995 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

char events_rcsid[] =
    "$Id$";

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#include "events.h"

static void invalid_event(const char *func)
{
    std::cerr << func << ": invalid event\n";
}

#if defined(IF_XM)
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

    switch (ev->type)
    {
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

	default:
	    invalid_event("point");
	    return BoxPoint();
    }
}
#else
// Event location
BoxPoint point(GUI::Event *ev)
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
    GUI::EventKey *evk;
    GUI::EventButton *evb;
    GUI::EventMotion *evm;
    GUI::EventCrossing *evc;
    GUI::EventExpose *eve;
    GUI::EventConfigure *evcf;
    if (evk = dynamic_cast<GUI::EventKey *>(ev)) {
	return BoxPoint(0, 0);
    }
    else if (evb = dynamic_cast<GUI::EventButton *>(ev)) {
	return BoxPoint((BoxCoordinate)evb->x,
			(BoxCoordinate)evb->y);
    }
    else if (evm = dynamic_cast<GUI::EventMotion *>(ev)) {
	return BoxPoint((BoxCoordinate)evm->x,
			(BoxCoordinate)evm->y);
    }
    else if (evc = dynamic_cast<GUI::EventCrossing *>(ev)) {
	return BoxPoint((BoxCoordinate)evc->x,
			(BoxCoordinate)evc->y);
    }
    else if (eve = dynamic_cast<GUI::EventExpose *>(ev)) {
	return BoxPoint(0, 0);
    }
    else if (evcf = dynamic_cast<GUI::EventConfigure *>(ev)) {
	return BoxPoint((BoxCoordinate)evcf->x,
			(BoxCoordinate)evcf->y);
    }
    else {
	invalid_event("point");
	return BoxPoint();
    }
}
#endif


#if defined(IF_XM)
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
	    invalid_event("time");
	    return CurrentTime;
    }
}
#else
// Event time
Time time(GUI::Event *ev)
{
    if (ev == 0)
    {
	// LessTif 0.79 and Motif 1.1 sometimes pass 0 as event;
	// provide some reasonable default
	invalid_event("time");
	return CurrentTime;
    }

#ifdef NAG_ME
#warning Push this down to the Event mechanism itself (virtual).
#endif
    GUI::EventKey *evk;
    GUI::EventButton *evb;
    GUI::EventMotion *evm;
    GUI::EventCrossing *evc;
    GUI::EventExpose *eve;
    GUI::EventConfigure *evcf;
    GUI::EventSelection *evs;
    if (evk = dynamic_cast<GUI::EventKey *>(ev)) {
	return evk->time;
    }
    else if (evb = dynamic_cast<GUI::EventButton *>(ev)) {
	return evb->time;
    }
    else if (evm = dynamic_cast<GUI::EventMotion *>(ev)) {
	return evm->time;
    }
    else if (evc = dynamic_cast<GUI::EventCrossing *>(ev)) {
	return evc->time;
    }
    // GUI::PROPERTY_NOTIFY:
    else if (evs = dynamic_cast<GUI::EventSelection *>(ev)) {
	return evs->time;
    }
    else {
	invalid_event("time");
	return CurrentTime;
    }
}
#endif


#if defined(IF_XM)
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
	    invalid_event("size");
	    return BoxSize(0, 0);
    }
}
#else
// Event size
BoxSize size(GUI::Event *ev)
{
    if (ev == 0)
    {
	// LessTif 0.79 and Motif 1.1 sometimes pass 0 as event;
	// provide some reasonable default
	invalid_event("size");
	return BoxSize(0, 0);
    }

    GUI::EventExpose *eve;
    GUI::EventConfigure *evcf;
    if (eve = dynamic_cast<GUI::EventExpose *>(ev)) {
	return BoxSize(eve->width,
		       eve->height);
    }
    else if (evcf = dynamic_cast<GUI::EventConfigure *>(ev)) {
	return BoxSize(evcf->width,
		       evcf->height);
    }
    else {
	invalid_event("size");
	return BoxSize(0, 0);
    }
}
#endif
