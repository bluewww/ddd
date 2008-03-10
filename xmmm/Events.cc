#include <iostream>

#include <Xmmm/Events.h>

using namespace Xmmm;

unsigned int keystate(unsigned int in)
{
    std::cerr << "KEYSTATE " << in << "\n";
    return in;
}

unsigned int keyval(unsigned int in)
{
    std::cerr << "KEYVAL " << in << "\n";
    return in;
}

int translate_event(XEvent *in, Event *out)
{
    bool onoff;
    switch (in->xany.type) {
    case KeyPress:
    case KeyRelease:
	onoff = (in->xany.type == KeyPress);
	std::cerr << (onoff ? "KeyPress\n" : "KeyRelease\n");
	out->type = (onoff ? KEY_PRESS : KEY_RELEASE);
	out->key.window = NULL; /* FIXME */
	out->key.send_event = in->xkey.send_event;
	out->key.time = in->xkey.time;
	out->key.state = keystate(in->xkey.state);
	out->key.keyval = keyval(in->xkey.keycode);
	out->key.length = 0; /* FIXME */
	out->key.string = ""; /* FIXME */
	out->key.hardware_keycode = in->xkey.keycode;
	out->key.group = 0; /* FIXME */
	out->key.is_modifier = 0; /* FIXME */
	break;
    case ButtonPress:
    case ButtonRelease:
	onoff = (in->xany.type == ButtonPress);
	std::cerr << (onoff ? "ButtonPress\n" : "ButtonRelease\n");
	out->type = (onoff ? BUTTON_PRESS : BUTTON_RELEASE);
	out->button.window = NULL; /* FIXME */
	out->button.send_event = in->xbutton.send_event;
	out->button.time = in->xbutton.time;
	out->button.x = in->xbutton.x;
	out->button.y = in->xbutton.y;
	out->button.axes = NULL; /* FIXME */
	out->button.state = in->xbutton.state;
	out->button.button = in->xbutton.button;
	break;
    case MotionNotify:
	std::cerr << "MotionNotify\n";
	out->type = MOTION_NOTIFY;
	out->motion.window = NULL; /* FIXME */
	out->motion.send_event = in->xmotion.send_event;
	out->motion.time = in->xmotion.time;
	out->motion.x = in->xmotion.x;
	out->motion.y = in->xmotion.y;
	out->motion.axes = NULL; /* FIXME */
	out->motion.state = in->xmotion.state;
	out->motion.is_hint = in->xmotion.is_hint;
	out->motion.x_root = in->xmotion.x_root;
	out->motion.y_root = in->xmotion.y_root;
	break;
    case EnterNotify:
    case LeaveNotify:
	onoff = (in->xany.type == EnterNotify);
	std::cerr << (onoff ? "EnterNotify\n" : "LeaveNotify\n");
	out->type = (onoff ? ENTER_NOTIFY: LEAVE_NOTIFY);
	out->crossing.window = NULL; /* FIXME */
	out->crossing.send_event = in->xcrossing.send_event;
	out->crossing.subwindow = NULL; /* FIXME */
	out->crossing.time = in->xcrossing.time;
	out->crossing.x = in->xcrossing.x;
	out->crossing.y = in->xcrossing.y;
	out->crossing.x_root = in->xcrossing.x_root;
	out->crossing.y_root = in->xcrossing.y_root;
	switch(in->xcrossing.mode) {
	case NotifyNormal: out->crossing.mode = CROSSING_NORMAL; break;
	case NotifyGrab: out->crossing.mode = CROSSING_GRAB;break;
	case NotifyUngrab: out->crossing.mode = CROSSING_UNGRAB;break;
	default:
	    std::cerr << "Unknown crossing mode " << in->xcrossing.mode << "\n";
	    out->crossing.mode = CROSSING_NORMAL;
	    break;
	}
	switch(in->xcrossing.detail) {
	case NotifyAncestor: out->crossing.detail = NOTIFY_ANCESTOR; break;
	case NotifyVirtual: out->crossing.detail = NOTIFY_VIRTUAL; break;
	case NotifyInferior: out->crossing.detail = NOTIFY_INFERIOR; break;
	case NotifyNonlinear: out->crossing.detail = NOTIFY_NONLINEAR; break;
	case NotifyNonlinearVirtual: out->crossing.detail = NOTIFY_NONLINEAR_VIRTUAL; break;
	default:
	    std::cerr << "Unknown crossing detail " << in->xcrossing.detail << "\n";
	    out->crossing.detail = NOTIFY_UNKNOWN; 
	    break;
	}
	out->crossing.focus = in->xcrossing.focus;
	out->crossing.state = in->xcrossing.state;
	break;
#if 0
    case FocusIn:
    case FocusOut:
	onoff = (in->xany.type == FocusIn);
	std::cerr << (onoff ? "FocusIn\n" : "FocusOut\n");
	out->type = FOCUS_CHANGE;
	out->focus.window = NULL; /* FIXME */
	out->focus.send_event = in->xfocus.send_event;
	out->focus.in = (in->xany.type == FocusIn);
	break;
    case KeymapNotify:
	std::cerr << "KeymapNotify\n";
	break;
#endif
    case Expose:
	std::cerr << "Expose\n";
	out->type = EXPOSE;
	out->expose.window = NULL; /* FIXME */
	out->expose.send_event = in->xexpose.send_event;
	out->expose.x = in->xexpose.x;
	out->expose.y = in->xexpose.y;
	out->expose.width = in->xexpose.width;
	out->expose.height = in->xexpose.height;
	out->expose.count = in->xexpose.count;
	break;
#if 0
    case GraphicsExpose:
	std::cerr << "GraphicsExpose\n";
	break;
#endif
    case NoExpose:
	std::cerr << "NoExpose\n";
	out->type = NO_EXPOSE;
	out->no_expose.window = NULL; /* FIXME */
	out->no_expose.send_event = in->xnoexpose.send_event;
	break;
    case VisibilityNotify:
	std::cerr << "VisibilityNotify\n";
	out->type = VISIBILITY_NOTIFY;
	break;
#if 0
    case CreateNotify:
	std::cerr << "CreateNotify\n";
	break;
    case DestroyNotify:
	std::cerr << "DestroyNotify\n";
	break;
#endif
    case UnmapNotify:
	std::cerr << "UnmapNotify\n";
	out->type = UNMAP;
	break;
    case MapNotify:
	std::cerr << "MapNotify\n";
	out->type = MAP;
	break;
#if 0
    case MapRequest:
	std::cerr << "MapRequest\n";
	break;
    case ReparentNotify:
	std::cerr << "ReparentNotify\n";
	break;
#endif
    case ConfigureNotify:
	std::cerr << "ConfigureNotify\n";
	out->type = CONFIGURE;
	break;
#if 0
    case ConfigureRequest:
	std::cerr << "ConfigureRequest\n";
	break;
    case GravityNotify:
	std::cerr << "GravityNotify\n";
	break;
    case ResizeRequest:
	std::cerr << "ResizeRequest\n";
	break;
    case CirculateNotify:
	std::cerr << "CirculateNotify\n";
	break;
    case CirculateRequest:
	std::cerr << "CirculateRequest\n";
	break;
    case PropertyNotify:
	std::cerr << "PropertyNotify\n";
	break;
#endif
    case SelectionClear:
	std::cerr << "SelectionClear\n";
	out->type = SELECTION_CLEAR;
	break;
    case SelectionRequest:
	std::cerr << "SelectionRequest\n";
	out->type = SELECTION_REQUEST;
	break;
    case SelectionNotify:
	std::cerr << "SelectionNotify\n";
	out->type = SELECTION_NOTIFY;
	break;
#if 0
    case ColormapNotify:
	std::cerr << "ColormapNotify\n";
	break;
    case ClientMessage:
	std::cerr << "ClientMessage\n";
	break;
    case MappingNotify:
	std::cerr << "MappingNotify\n";
	break;
#endif
    default:
	std::cerr << "UNKNOWN EVENT TYPE " << in->xany.type << "\n";
	out->type = NOTHING;
	break;
    }
}


