#include <iostream>

#include <GtkX/Events.h>

using namespace GtkX;

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

int GtkX::translate_event(GdkEvent *in, Event *out)
{
    bool onoff;
    switch (in->any.type) {
    case GDK_KEY_PRESS:
    case GDK_KEY_RELEASE:
	onoff = (in->any.type == GDK_KEY_PRESS);
	std::cerr << (onoff ? "KeyPress\n" : "KeyRelease\n");
	out->type = (onoff ? KEY_PRESS : KEY_RELEASE);
	out->key.window = NULL; /* FIXME */
	out->key.send_event = in->key.send_event;
	out->key.time = in->key.time;
	out->key.state = keystate(in->key.state);
	out->key.keyval = keyval(in->key.keyval);
	out->key.length = in->key.length; /* FIXME */
	out->key.string = in->key.string; /* FIXME survives for lifetime of event? */
	out->key.hardware_keycode = in->key.hardware_keycode;
	out->key.group = in->key.group;
	out->key.is_modifier = in->key.is_modifier;
	break;
    case GDK_BUTTON_PRESS:
    case GDK_BUTTON_RELEASE:
	onoff = (in->any.type == GDK_BUTTON_PRESS);
	std::cerr << (onoff ? "ButtonPress\n" : "ButtonRelease\n");
	out->type = (onoff ? BUTTON_PRESS : BUTTON_RELEASE);
	out->button.window = NULL; /* FIXME */
	out->button.send_event = in->button.send_event;
	out->button.time = in->button.time;
	out->button.x = in->button.x;
	out->button.y = in->button.y;
	out->button.axes = NULL; /* FIXME */
	out->button.state = in->button.state;
	out->button.button = in->button.button;
	break;
    case GDK_MOTION_NOTIFY:
	std::cerr << "MotionNotify\n";
	out->type = MOTION_NOTIFY;
	out->motion.window = NULL; /* FIXME */
	out->motion.send_event = in->motion.send_event;
	out->motion.time = in->motion.time;
	out->motion.x = in->motion.x;
	out->motion.y = in->motion.y;
	out->motion.axes = NULL; /* FIXME */
	out->motion.state = in->motion.state;
	out->motion.is_hint = in->motion.is_hint;
	out->motion.x_root = in->motion.x_root;
	out->motion.y_root = in->motion.y_root;
	break;
    case GDK_ENTER_NOTIFY:
    case GDK_LEAVE_NOTIFY:
	onoff = (in->any.type == GDK_ENTER_NOTIFY);
	std::cerr << (onoff ? "EnterNotify\n" : "LeaveNotify\n");
	out->type = (onoff ? ENTER_NOTIFY: LEAVE_NOTIFY);
	out->crossing.window = NULL; /* FIXME */
	out->crossing.send_event = in->crossing.send_event;
	out->crossing.subwindow = NULL; /* FIXME */
	out->crossing.time = in->crossing.time;
	out->crossing.x = in->crossing.x;
	out->crossing.y = in->crossing.y;
	out->crossing.x_root = in->crossing.x_root;
	out->crossing.y_root = in->crossing.y_root;
	switch(in->crossing.mode) {
	case GDK_CROSSING_NORMAL: out->crossing.mode = CROSSING_NORMAL; break;
	case GDK_CROSSING_GRAB: out->crossing.mode = CROSSING_GRAB;break;
	case GDK_CROSSING_UNGRAB: out->crossing.mode = CROSSING_UNGRAB;break;
	default:
	    std::cerr << "Unknown crossing mode " << in->crossing.mode << "\n";
	    out->crossing.mode = CROSSING_NORMAL;
	    break;
	}
	switch(in->crossing.detail) {
	case GDK_NOTIFY_ANCESTOR: out->crossing.detail = NOTIFY_ANCESTOR; break;
	case GDK_NOTIFY_VIRTUAL: out->crossing.detail = NOTIFY_VIRTUAL; break;
	case GDK_NOTIFY_INFERIOR: out->crossing.detail = NOTIFY_INFERIOR; break;
	case GDK_NOTIFY_NONLINEAR: out->crossing.detail = NOTIFY_NONLINEAR; break;
	case GDK_NOTIFY_NONLINEAR_VIRTUAL: out->crossing.detail = NOTIFY_NONLINEAR_VIRTUAL; break;
	default:
	    std::cerr << "Unknown crossing detail " << in->crossing.detail << "\n";
	    out->crossing.detail = NOTIFY_UNKNOWN; 
	    break;
	}
	out->crossing.focus = in->crossing.focus;
	out->crossing.state = in->crossing.state;
	break;
#if 0
    case FocusIn:
    case FocusOut:
	onoff = (in->any.type == FocusIn);
	std::cerr << (onoff ? "FocusIn\n" : "FocusOut\n");
	out->type = FOCUS_CHANGE;
	out->focus.window = NULL; /* FIXME */
	out->focus.send_event = in->xfocus.send_event;
	out->focus.in = (in->any.type == FocusIn);
	break;
    case KeymapNotify:
	std::cerr << "KeymapNotify\n";
	break;
#endif
    case GDK_EXPOSE:
	std::cerr << "Expose\n";
	out->type = EXPOSE;
	out->expose.window = NULL; /* FIXME */
	out->expose.send_event = in->expose.send_event;
	out->expose.x = in->expose.area.x;
	out->expose.y = in->expose.area.y;
	out->expose.width = in->expose.area.width;
	out->expose.height = in->expose.area.height;
	out->expose.count = in->expose.count;
	break;
#if 0
    case GraphicsExpose:
	std::cerr << "GraphicsExpose\n";
	break;
#endif
    case GDK_NO_EXPOSE:
	std::cerr << "NoExpose\n";
	out->type = NO_EXPOSE;
	out->no_expose.window = NULL; /* FIXME */
	out->no_expose.send_event = in->no_expose.send_event;
	break;
    case GDK_VISIBILITY_NOTIFY:
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
    case GDK_UNMAP:
	std::cerr << "UnmapNotify\n";
	out->type = UNMAP;
	break;
    case GDK_MAP:
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
    case GDK_CONFIGURE:
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
    case GDK_SELECTION_CLEAR:
	std::cerr << "SelectionClear\n";
	out->type = SELECTION_CLEAR;
	break;
    case GDK_SELECTION_REQUEST:
	std::cerr << "SelectionRequest\n";
	out->type = SELECTION_REQUEST;
	break;
    case GDK_SELECTION_NOTIFY:
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
	std::cerr << "UNKNOWN EVENT TYPE " << in->any.type << "\n";
	out->type = NOTHING;
	break;
    }
}


