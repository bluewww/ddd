// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#include <iostream>

#include <GtkX/Events.h>

using namespace GtkX;

static unsigned int keystate(unsigned int in)
{
    GdkModifierType gin = GdkModifierType(in);
    ModifierType out;
    translate_mod_mask(gin, out);
    return out;
}

static unsigned int keyval(unsigned int in)
{
    std::cerr << "KEYVAL " << in << "\n";
    return in;
}

static unsigned int buttonstate(unsigned int in)
{
    GdkModifierType gin = GdkModifierType(in);
    ModifierType out;
    translate_mod_mask(gin, out);
    return out;
}

int GtkX::translate_event(GdkEvent *in, Event *out)
{
    bool onoff;
    switch (in->any.type) {
    case GDK_KEY_PRESS:
    case GDK_KEY_RELEASE:
	onoff = (in->any.type == GDK_KEY_PRESS);
	// std::cerr << (onoff ? "KeyPress\n" : "KeyRelease\n");
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
	// std::cerr << (onoff ? "ButtonPress\n" : "ButtonRelease\n");
	out->type = (onoff ? BUTTON_PRESS : BUTTON_RELEASE);
	out->button.window = NULL; /* FIXME */
	out->button.send_event = in->button.send_event;
	out->button.time = in->button.time;
	out->button.x = in->button.x;
	out->button.y = in->button.y;
	out->button.axes = NULL; /* FIXME */
	out->button.state = buttonstate(in->button.state);
	out->button.button = in->button.button;
	break;
    case GDK_MOTION_NOTIFY:
	// std::cerr << "MotionNotify\n";
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
	// std::cerr << (onoff ? "EnterNotify\n" : "LeaveNotify\n");
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
	// std::cerr << (onoff ? "FocusIn\n" : "FocusOut\n");
	out->type = FOCUS_CHANGE;
	out->focus.window = NULL; /* FIXME */
	out->focus.send_event = in->xfocus.send_event;
	out->focus.in = (in->any.type == FocusIn);
	break;
    case KeymapNotify:
	// std::cerr << "KeymapNotify\n";
	break;
#endif
    case GDK_EXPOSE:
	// std::cerr << "Expose\n";
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
	// std::cerr << "GraphicsExpose\n";
	break;
#endif
    case GDK_NO_EXPOSE:
	// std::cerr << "NoExpose\n";
	out->type = NO_EXPOSE;
	out->no_expose.window = NULL; /* FIXME */
	out->no_expose.send_event = in->no_expose.send_event;
	break;
    case GDK_VISIBILITY_NOTIFY:
	// std::cerr << "VisibilityNotify\n";
	out->type = VISIBILITY_NOTIFY;
	break;
#if 0
    case CreateNotify:
	// std::cerr << "CreateNotify\n";
	break;
    case DestroyNotify:
	// std::cerr << "DestroyNotify\n";
	break;
#endif
    case GDK_UNMAP:
	// std::cerr << "UnmapNotify\n";
	out->type = UNMAP;
	break;
    case GDK_MAP:
	// std::cerr << "MapNotify\n";
	out->type = MAP;
	break;
#if 0
    case MapRequest:
	// std::cerr << "MapRequest\n";
	break;
    case ReparentNotify:
	// std::cerr << "ReparentNotify\n";
	break;
#endif
    case GDK_CONFIGURE:
	// std::cerr << "ConfigureNotify\n";
	out->type = CONFIGURE;
	break;
#if 0
    case ConfigureRequest:
	// std::cerr << "ConfigureRequest\n";
	break;
    case GravityNotify:
	// std::cerr << "GravityNotify\n";
	break;
    case ResizeRequest:
	// std::cerr << "ResizeRequest\n";
	break;
    case CirculateNotify:
	// std::cerr << "CirculateNotify\n";
	break;
    case CirculateRequest:
	// std::cerr << "CirculateRequest\n";
	break;
    case PropertyNotify:
	// std::cerr << "PropertyNotify\n";
	break;
#endif
    case GDK_SELECTION_CLEAR:
	// std::cerr << "SelectionClear\n";
	out->type = SELECTION_CLEAR;
	break;
    case GDK_SELECTION_REQUEST:
	// std::cerr << "SelectionRequest\n";
	out->type = SELECTION_REQUEST;
	break;
    case GDK_SELECTION_NOTIFY:
	// std::cerr << "SelectionNotify\n";
	out->type = SELECTION_NOTIFY;
	break;
#if 0
    case ColormapNotify:
	// std::cerr << "ColormapNotify\n";
	break;
    case ClientMessage:
	// std::cerr << "ClientMessage\n";
	break;
    case MappingNotify:
	// std::cerr << "MappingNotify\n";
	break;
#endif
    default:
	// std::cerr << "UNKNOWN EVENT TYPE " << in->any.type << "\n";
	out->type = NOTHING;
	break;
    }
}

int
GtkX::translate_event_mask(EventMask &in, Gdk::EventMask &out)
{
    out = Gdk::EventMask(0);
    if (in&EXPOSURE_MASK) {out|=Gdk::EXPOSURE_MASK;}
    if (in&POINTER_MOTION_MASK) {out|=Gdk::POINTER_MOTION_MASK;}
    if (in&POINTER_MOTION_HINT_MASK) {out|=Gdk::POINTER_MOTION_HINT_MASK;}
    if (in&BUTTON_MOTION_MASK) {out|=Gdk::BUTTON_MOTION_MASK;}
    if (in&BUTTON1_MOTION_MASK) {out|=Gdk::BUTTON1_MOTION_MASK;}
    if (in&BUTTON2_MOTION_MASK) {out|=Gdk::BUTTON2_MOTION_MASK;}
    if (in&BUTTON3_MOTION_MASK) {out|=Gdk::BUTTON3_MOTION_MASK;}
    if (in&BUTTON_PRESS_MASK) {out|=Gdk::BUTTON_PRESS_MASK;}
    if (in&BUTTON_RELEASE_MASK) {out|=Gdk::BUTTON_RELEASE_MASK;}
    if (in&KEY_PRESS_MASK) {out|=Gdk::KEY_PRESS_MASK;}
    if (in&ENTER_NOTIFY_MASK) {out|=Gdk::ENTER_NOTIFY_MASK;}
    if (in&LEAVE_NOTIFY_MASK) {out|=Gdk::LEAVE_NOTIFY_MASK;}
    if (in&FOCUS_CHANGE_MASK) {out|=Gdk::FOCUS_CHANGE_MASK;}
    if (in&STRUCTURE_MASK) {out|=Gdk::STRUCTURE_MASK;}
    if (in&PROPERTY_CHANGE_MASK) {out|=Gdk::PROPERTY_CHANGE_MASK;}
    if (in&VISIBILITY_NOTIFY_MASK) {out|=Gdk::VISIBILITY_NOTIFY_MASK;}
    if (in&PROXIMITY_IN_MASK) {out|=Gdk::PROXIMITY_IN_MASK;}
    if (in&PROXIMITY_OUT_MASK) {out|=Gdk::PROXIMITY_OUT_MASK;}
    if (in&SUBSTRUCTURE_MASK) {out|=Gdk::SUBSTRUCTURE_MASK;}
    if (in&SCROLL_MASK) {out|=Gdk::SCROLL_MASK;}
    return 1;
}

int
GtkX::translate_mod_mask(GdkModifierType &in, ModifierType &out)
{
    unsigned long out1 = 0;
    if (in&GDK_SHIFT_MASK) {out1|=SHIFT_MASK;}
    if (in&GDK_LOCK_MASK) {out1|=LOCK_MASK;}
    if (in&GDK_CONTROL_MASK) {out1|=CONTROL_MASK;}
    if (in&GDK_MOD1_MASK) {out1|=MOD1_MASK;}
    if (in&GDK_MOD2_MASK) {out1|=MOD2_MASK;}
    if (in&GDK_MOD3_MASK) {out1|=MOD3_MASK;}
    if (in&GDK_MOD4_MASK) {out1|=MOD4_MASK;}
    if (in&GDK_MOD5_MASK) {out1|=MOD5_MASK;}
    if (in&GDK_BUTTON1_MASK) {out1|=BUTTON1_MASK;}
    if (in&GDK_BUTTON2_MASK) {out1|=BUTTON2_MASK;}
    if (in&GDK_BUTTON3_MASK) {out1|=BUTTON3_MASK;}
    if (in&GDK_BUTTON4_MASK) {out1|=BUTTON4_MASK;}
    if (in&GDK_BUTTON5_MASK) {out1|=BUTTON5_MASK;}
    if (in&GDK_SUPER_MASK) {out1|=SUPER_MASK;}
    if (in&GDK_HYPER_MASK) {out1|=HYPER_MASK;}
    if (in&GDK_META_MASK) {out1|=META_MASK;}
    if (in&GDK_RELEASE_MASK) {out1|=RELEASE_MASK;}
    out = ModifierType(out1);
    return 1;
}

