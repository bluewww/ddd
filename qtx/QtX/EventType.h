// -*- C++ -*-

// High-level GUI wrapper for Gtkmm and Qt.

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

// This event type is common to GtkX and QtX implementations.
// "Raw" events from the underlying toolkit should be converted to
// this type.

#ifndef EVENT_TYPE_H
#define EVENT_TYPE_H

typedef unsigned long Atom;

class Window;

union Event;

typedef void (*EventFunc) (Event *event,
			   void *data);

enum EventType
{
    NOTHING			= -1,
    DELETE			= 0,
    DESTROY			= 1,
    EXPOSE			= 2,
    MOTION_NOTIFY		= 3,
    BUTTON_PRESS		= 4,
    TWO_BUTTON_PRESS	= 5,
    THREE_BUTTON_PRESS	= 6,
    BUTTON_RELEASE		= 7,
    KEY_PRESS		= 8,
    KEY_RELEASE		= 9,
    ENTER_NOTIFY		= 10,
    LEAVE_NOTIFY		= 11,
    FOCUS_CHANGE		= 12,
    CONFIGURE		= 13,
    MAP			= 14,
    UNMAP			= 15,
    PROPERTY_NOTIFY		= 16,
    SELECTION_CLEAR		= 17,
    SELECTION_REQUEST	= 18,
    SELECTION_NOTIFY	= 19,
    PROXIMITY_IN		= 20,
    PROXIMITY_OUT		= 21,
    DRAG_ENTER		= 22,
    DRAG_LEAVE		= 23,
    DRAG_MOTION       	= 24,
    DRAG_STATUS       	= 25,
    DROP_START        	= 26,
    DROP_FINISHED     	= 27,
    CLIENT_EVENT		= 28,
    VISIBILITY_NOTIFY 	= 29,
    NO_EXPOSE		= 30,
    SCROLL            	= 31,
    WINDOW_STATE      	= 32,
    SETTING           	= 33,
    OWNER_CHANGE      	= 34,
    GRAB_BROKEN       	= 35
};

typedef enum
{
    EXPOSURE_MASK			= 1 << 1,
    POINTER_MOTION_MASK		= 1 << 2,
    POINTER_MOTION_HINT_MASK	= 1 << 3,
    BUTTON_MOTION_MASK		= 1 << 4,
    BUTTON1_MOTION_MASK		= 1 << 5,
    BUTTON2_MOTION_MASK		= 1 << 6,
    BUTTON3_MOTION_MASK		= 1 << 7,
    BUTTON_PRESS_MASK		= 1 << 8,
    BUTTON_RELEASE_MASK		= 1 << 9,
    KEY_PRESS_MASK			= 1 << 10,
    KEY_RELEASE_MASK		= 1 << 11,
    ENTER_NOTIFY_MASK		= 1 << 12,
    LEAVE_NOTIFY_MASK		= 1 << 13,
    FOCUS_CHANGE_MASK		= 1 << 14,
    STRUCTURE_MASK			= 1 << 15,
    PROPERTY_CHANGE_MASK		= 1 << 16,
    VISIBILITY_NOTIFY_MASK		= 1 << 17,
    PROXIMITY_IN_MASK		= 1 << 18,
    PROXIMITY_OUT_MASK		= 1 << 19,
    SUBSTRUCTURE_MASK		= 1 << 20,
    SCROLL_MASK               	= 1 << 21,
    ALL_EVENTS_MASK			= 0x3FFFFE
} EventMask;

typedef enum
{
    VISIBILITY_UNOBSCURED,
    VISIBILITY_PARTIAL,
    VISIBILITY_FULLY_OBSCURED
} VisibilityState;

typedef enum
{
    SCROLL_UP,
    SCROLL_DOWN,
    SCROLL_LEFT,
    SCROLL_RIGHT
} ScrollDirection;

typedef enum
{
    NOTIFY_ANCESTOR			= 0,
    NOTIFY_VIRTUAL			= 1,
    NOTIFY_INFERIOR			= 2,
    NOTIFY_NONLINEAR		= 3,
    NOTIFY_NONLINEAR_VIRTUAL	= 4,
    NOTIFY_UNKNOWN			= 5
} NotifyType;

/* Enter/leave event modes.
 */
typedef enum
{
    CROSSING_NORMAL,
    CROSSING_GRAB,
    CROSSING_UNGRAB
} CrossingMode;

typedef enum
{
    PROPERTY_NEW_VALUE,
    PROPERTY_DELETE
} PropertyState;

typedef enum
{
    WINDOW_STATE_WITHDRAWN  = 1 << 0,
    WINDOW_STATE_ICONIFIED  = 1 << 1,
    WINDOW_STATE_MAXIMIZED  = 1 << 2,
    WINDOW_STATE_STICKY     = 1 << 3,
    WINDOW_STATE_FULLSCREEN = 1 << 4,
    WINDOW_STATE_ABOVE      = 1 << 5,
    WINDOW_STATE_BELOW      = 1 << 6
} WindowState;

typedef enum
{
    SETTING_ACTION_NEW,
    SETTING_ACTION_CHANGED,
    SETTING_ACTION_DELETED
} SettingAction;

typedef enum
{
    OWNER_CHANGE_NEW_OWNER,
    OWNER_CHANGE_DESTROY,
    OWNER_CHANGE_CLOSE
} OwnerChange;

enum ModifierType {
    SHIFT_MASK	= 1 << 0,
    LOCK_MASK	= 1 << 1,
    CONTROL_MASK	= 1 << 2,
    MOD1_MASK	= 1 << 3,
    MOD2_MASK	= 1 << 4,
    MOD3_MASK	= 1 << 5,
    MOD4_MASK	= 1 << 6,
    MOD5_MASK	= 1 << 7,
    BUTTON1_MASK	= 1 << 8,
    BUTTON2_MASK	= 1 << 9,
    BUTTON3_MASK	= 1 << 10,
    BUTTON4_MASK	= 1 << 11,
    BUTTON5_MASK	= 1 << 12,
    /* The next few modifiers are used by XKB, so we skip to the end.
     * Bits 15 - 25 are currently unused. Bit 29 is used internally.
     */
    SUPER_MASK	= 1 << 26,
    HYPER_MASK	= 1 << 27,
    META_MASK	= 1 << 28,
    RELEASE_MASK	= 1 << 30,
    MODIFIER_MASK	= 0x5c001fff
};

struct EventAny
{
    EventType type;
    Window *window;
    char send_event;
};

struct EventExpose
{
    EventType type;
    Window *window;
    char send_event;
    int x;
    int y;
    int width;
    int height;
    int count; /* If non-zero, how many more events follow. */
};

struct EventNoExpose
{
    EventType type;
    Window *window;
    char send_event;
};

struct EventVisibility
{
    EventType type;
    Window *window;
    char send_event;
    VisibilityState state;
};

struct EventMotion
{
    EventType type;
    Window *window;
    char send_event;
    unsigned int time;
    double x;
    double y;
    double *axes;
    unsigned int state;
    short is_hint;
    /* FIXME Device *device; */
    double x_root, y_root;
};

struct EventButton
{
    EventType type;
    Window *window;
    char send_event;
    unsigned int time;
    double x;
    double y;
    double *axes;
    unsigned int state;
    unsigned int button;
    /* FIXME Device *device; */
    double x_root, y_root;
};

struct EventScroll
{
    EventType type;
    Window *window;
    char send_event;
    unsigned int time;
    double x;
    double y;
    unsigned int state;
    ScrollDirection direction;
    /* FIXME Device *device; */
    double x_root, y_root;
};

struct EventKey
{
    EventType type;
    Window *window;
    char send_event;
    unsigned int time;
    unsigned int state;
    unsigned int keyval;
    int length;
    char *string;
    unsigned short hardware_keycode;
    unsigned char group;
    unsigned int is_modifier : 1;
};

struct EventCrossing
{
    EventType type;
    Window *window;
    char send_event;
    Window *subwindow;
    unsigned int time;
    double x;
    double y;
    double x_root;
    double y_root;
    CrossingMode mode;
    NotifyType detail;
    char focus;
    unsigned int state;
};

struct EventFocus
{
    EventType type;
    Window *window;
    char send_event;
    short in;
};

struct EventConfigure
{
    EventType type;
    Window *window;
    char send_event;
    int x, y;
    int width;
    int height;
};

struct EventProperty
{
    EventType type;
    Window *window;
    char send_event;
    Atom atom;
    unsigned int time;
    unsigned int state;
};

struct EventSelection
{
    EventType type;
    Window *window;
    char send_event;
    Atom selection;
    Atom target;
    Atom property;
    unsigned int time;
    /* FIXME NativeWindow requestor; */
};

struct EventOwnerChange
{
    EventType type;
    Window *window;
    char send_event;
    /* FIXME NativeWindow owner; */
    OwnerChange reason;
    Atom selection;
    unsigned int time;
    unsigned int selection_time;
};

/* This event type will be used pretty rarely. It only is important
   for XInput aware programs that are drawing their own cursor */

struct EventProximity
{
    EventType type;
    Window *window;
    char send_event;
    unsigned int time;
    /* FIXME Device *device; */
};

struct EventClient
{
    EventType type;
    Window *window;
    char send_event;
    Atom message_type;
    unsigned short data_format;
    union {
	char b[20];
	short s[10];
	long l[5];
    } data;
};

struct EventSetting
{
    EventType type;
    Window *window;
    char send_event;
    SettingAction action;
    char *name;
};

struct EventWindowState
{
    EventType type;
    Window *window;
    char send_event;
    WindowState changed_mask;
    WindowState new_window_state;
};

struct EventGrabBroken {
    EventType type;
    Window *window;
    char send_event;
    char keyboard;
    char implicit;
    Window *grab_window;
};

union Event
{
    EventType		type;
    EventAny		any;
    EventExpose		expose;
    EventNoExpose		no_expose;
    EventVisibility		visibility;
    EventMotion		motion;
    EventButton		button;
    EventScroll		scroll;
    EventKey		key;
    EventCrossing		crossing;
    EventFocus		focus_change;
    EventConfigure		configure;
    EventProperty		property;
    EventSelection		selection;
    EventOwnerChange	owner_change;
    EventProximity		proximity;
    EventClient		client;
    EventWindowState	window_state;
    EventSetting		setting;
    EventGrabBroken		grab_broken;
};

#endif
