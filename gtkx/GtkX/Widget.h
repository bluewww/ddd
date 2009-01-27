// -*- C++ -*-

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
// A brief look at QT indicates that this will be required there as
// well.

#ifndef GTKX_WIDGET_H
#define GTKX_WIDGET_H

#include <vector>

#include <gtkmm/widget.h>
#include <gdkmm/pixmap.h>
#include <gdkmm/cursor.h>

#include <GtkX/Events.h>
#include <GtkX/GUIString.h>
#include <GtkX/Main.h>
#include <GtkX/Font.h>

// Typedefs for compatibility with Motif version
typedef int Position;
typedef int Dimension;
typedef guint32 Time;

// FIXME: default time, how is it used?
#define CurrentTime 0 /* special time */

// i18n not supported yet.
#define _(x) (x)
#define N_(x) (x)

namespace GtkX {

    enum Orientation
    {
	ORIENTATION_HORIZONTAL,
	ORIENTATION_VERTICAL
    };

    enum AttachOptions
    {
	EXPAND = 1 << 0,
	SHRINK = 1 << 1,
	FILL = 1 << 2
    };

    extern Glib::Quark gtkx_super_quark;

    struct Requisition
    {
	int width;
	int height;
    };

    class Widget;
    class Container;

    template <class T>
    class PropertyProxy: public Glib::PropertyProxy<T> {
	Widget *w;
	const char *name;
    public:
	PropertyProxy(Widget *w0, const char *name0);
	PropertyProxy &operator=(T);
    };

    template <class T>
    class PropertyProxy_RO: public Glib::PropertyProxy_ReadOnly<T> {
	const Widget *w;
	const char *name;
    public:
	PropertyProxy_RO(const Widget *w0, const char *name0);
    };

    class Screen;

    class Display {
	int nrefs_;
	Glib::RefPtr<Gdk::Display> disp_;
    public:
	Display(Glib::RefPtr<Gdk::Display> d0);
	~Display();
	static RefPtr<Display> wrap(Glib::RefPtr<Gdk::Display> d0);
	static RefPtr<const Display> wrap(Glib::RefPtr<const Gdk::Display> d0);
	Glib::RefPtr<Gdk::Display> internal();
	Glib::RefPtr<const Gdk::Display> internal() const;
	int ref() {return nrefs_++;}
	int unref() {if (!--nrefs_) delete this;}
	String get_name() const;
	void flush();
	static RefPtr<Display> open(const String& display_name);
	void close();
	void pointer_ungrab(unsigned int);
	void keyboard_ungrab(unsigned int);
	RefPtr<Screen> get_default_screen();
    };

    class XWindow;

    class Screen {
	int nrefs_;
	Glib::RefPtr<Gdk::Screen> screen_;
    public:
	Screen(Glib::RefPtr<Gdk::Screen> s0);
	~Screen();
	static RefPtr<Screen> wrap(Glib::RefPtr<Gdk::Screen> s0);
	static RefPtr<const Screen> wrap(Glib::RefPtr<const Gdk::Screen> s0);
	Glib::RefPtr<Gdk::Screen> internal();
	Glib::RefPtr<const Gdk::Screen> internal() const;
	int ref() {return nrefs_++;}
	int unref() {if (!--nrefs_) delete this;}
	RefPtr<XWindow> get_root_window();
	String make_display_name() const;
    };

    class Drawable;

    class Pixmap;

    struct Color {
	double r;
	double g;
	double b;
	Color() {
	    r = g = b = 0.0;
	}
	Color(double r0, double g0, double b0) {
	    r = r0;
	    g = g0;
	    b = b0;
	}
	Color(const String &s);
    };

    enum StateType
    {
	STATE_NORMAL,
	STATE_ACTIVE,
	STATE_PRELIGHT,
	STATE_SELECTED,
	STATE_INSENSITIVE
    };

    enum LineStyle
    {
	LINE_SOLID,
	LINE_ON_OFF_DASH,
	LINE_DOUBLE_DASH
    };

    enum CapStyle
    {
	CAP_NOT_LAST,
	CAP_BUTT,
	CAP_ROUND,
	CAP_PROJECTING
    };

    enum JoinStyle
    {
	JOIN_MITER,
	JOIN_ROUND,
	JOIN_BEVEL
    };

    enum Fill
    {
	SOLID,
	TILED,
	STIPPLED,
	OPAQUE_STIPPLED
    };

    enum Function
    {
	COPY,
	INVERT,
	XOR,
	CLEAR,
	AND,
	AND_REVERSE,
	AND_INVERT,
	NOOP,
	OR,
	EQUIV,
	OR_REVERSE,
	COPY_INVERT,
	OR_INVERT,
	NAND,
	NOR,
	SET
    };

    struct GCValues {
	Color foreground;
	Color background;
	int line_width;
	LineStyle line_style;
	CapStyle cap_style;
	JoinStyle join_style;
	Fill fill;
	const RefPtr<Pixmap> stipple;
	Function function;
    };

    enum GCValuesMask
    {
	GC_FOREGROUND = 1 << 0,
	GC_BACKGROUND = 1 << 1,
	GC_LINE_WIDTH = 1 << 2,
	GC_LINE_STYLE = 1 << 3,
	GC_CAP_STYLE = 1 << 4,
	GC_JOIN_STYLE = 1 << 5,
	GC_FILL = 1 << 6,
	GC_STIPPLE = 1 << 7,
	GC_FUNCTION = 1 << 8
    };

    enum GrabStatus
    {
	GRAB_SUCCESS,
	GRAB_ALREADY_GRABBED,
	GRAB_INVALID_TIME,
	GRAB_NOT_VIEWABLE,
	GRAB_FROZEN
    };

    class GC {
	int nrefs_;
	Glib::RefPtr<Gdk::GC> gc_;
    public:
	GC(Glib::RefPtr<Gdk::GC> g0);
	~GC();
	static RefPtr<GC> wrap(Glib::RefPtr<Gdk::GC> g0);
	static RefPtr<const GC> wrap(Glib::RefPtr<const Gdk::GC> g0);
	Glib::RefPtr<Gdk::GC> internal();
	Glib::RefPtr<const Gdk::GC> internal() const;
	GC(const RefPtr<Drawable> &d);
	int ref() {return nrefs_++;}
	int unref() {if (!--nrefs_) delete this;}
	void set_foreground(const Color &color);
	Color get_foreground() const;
	void set_background(const Color &color);
	Color get_background() const;
	void set_values(const GCValues &values, GCValuesMask mask);
	void get_values(GCValues &values);
	void set_line_attributes(int line_width, LineStyle line_style, CapStyle cap_style, JoinStyle join_style);
	void set_fill(Fill fill);
	void set_stipple(const RefPtr<Pixmap> &stipple);
	void set_function(Function function);
    };

    struct Point {
	int x;
	int y;
	Point() {}
	Point(int x0, int y0) {
	    x = x0;
	    y = y0;
	}
    };

    class Drawable {
	int nrefs_;
    public:
	Drawable();
	virtual ~Drawable();
	virtual Glib::RefPtr<Gdk::Drawable> internal() = 0;
	virtual Glib::RefPtr<const Gdk::Drawable> internal() const = 0;
	int ref() {return nrefs_++;}
	int unref() {if (!--nrefs_) delete this;}
	int get_depth() const;
	void draw_line(RefPtr<GC> gc,
		       int x1, int y1, int x2, int y2);
	void draw_rectangle(RefPtr<GC> gc, bool filled,
			    int x, int y, int w, int h);
	void draw_arc(RefPtr<GC> gc, bool filled,
		      int x, int y, int w, int h,
		      int angle1, int angle2);
	void draw_polygon(RefPtr<GC> gc, bool filled,
			  std::vector<Point> &points);
	void draw_layout(RefPtr<GC> &gc, int x, int y,
			 RefPtr<Layout> &layout);
    };

    class Pixmap: public Drawable {
	Glib::RefPtr<Gdk::Pixmap> pixmap_;
    public:
	Pixmap(Glib::RefPtr<Gdk::Pixmap> p0);
	~Pixmap();
	static RefPtr<Pixmap> wrap(Glib::RefPtr<Gdk::Pixmap> p0);
	Glib::RefPtr<Gdk::Drawable> internal();
	Glib::RefPtr<const Gdk::Drawable> internal() const;
	Glib::RefPtr<Gdk::Pixmap> internal_pixmap();
	Glib::RefPtr<const Gdk::Pixmap> internal_pixmap() const;
	static RefPtr<Pixmap> create_bitmap_from_data(const char *bits, int width, int height);
	static RefPtr<Pixmap> create_from_data(RefPtr<Drawable> drawable,
                                               const char *data, int width, int height,
					       int depth, const Color &fg, const Color &bg);
	Glib::RefPtr<Gdk::Pixmap> gdk_pixmap();
	const Glib::RefPtr<Gdk::Pixmap> gdk_pixmap() const;
    };

    class Cursor;

    class XWindow: public Drawable {
	Glib::RefPtr<Gdk::Window> win_;
    public:
	XWindow(Glib::RefPtr<Gdk::Window> w0);
	~XWindow();
	static RefPtr<XWindow> wrap(Glib::RefPtr<Gdk::Window> w0);
	static RefPtr<const XWindow> wrap(Glib::RefPtr<const Gdk::Window> w0);
	static RefPtr<XWindow> wrap(GdkWindow *w0);
	static RefPtr<const XWindow> wrap(const GdkWindow *w0);
	Glib::RefPtr<Gdk::Drawable> internal();
	Glib::RefPtr<const Gdk::Drawable> internal() const;
	Glib::RefPtr<Gdk::Window> internal_window();
	Glib::RefPtr<const Gdk::Window> internal_window() const;
	void set_back_pixmap(const RefPtr<Pixmap> &pixmap, bool parent_relative);
	void set_cursor(const Cursor &cursor);
	void clear();
	void clear_area(int x, int y, int w, int h);
	GrabStatus pointer_grab(bool owner_events, EventMask event_mask,
				RefPtr<XWindow>& confine_to,
				Cursor &cursor, unsigned int time_);
	GrabStatus pointer_grab(bool owner_events, EventMask event_mask,
				unsigned int time_);
	void set_title(const String &);
    };

    class Widget {
    protected:
	// In GTK, events may be connected after, or optionally
	// before, the class closure.  For the latter case we provide
	// the pre_event_callbacks.
	sigc::signal<bool, GtkX::Event *> signal_event_;
	bool event_callback(GdkEvent *ev);
	sigc::signal<bool, GtkX::EventButton *> signal_button_press_event_;
	bool button_press_event_callback(GdkEventButton *ev);
	sigc::signal<bool, GtkX::EventButton *> signal_button_release_event_;
	bool button_release_event_callback(GdkEventButton *ev);
	sigc::signal<bool, GtkX::EventButton *> signal_button_press_pre_event_;
	bool button_press_pre_event_callback(GdkEventButton *ev);
	sigc::signal<bool, GtkX::EventButton *> signal_button_release_pre_event_;
	bool button_release_pre_event_callback(GdkEventButton *ev);
	sigc::signal<void> signal_map_;
	void map_callback();
	sigc::signal<void> signal_unmap_;
	void unmap_callback();
	sigc::signal<bool, GtkX::EventAny *> signal_delete_event_;
	bool delete_event_callback(GdkEventAny *ev);
	sigc::signal<void, GtkX::Requisition *> signal_size_request_;
	void size_request_callback(Gtk::Requisition *r);
    public:
	Widget();
	virtual ~Widget();
	void init_signals();
	void postinit();
	// Instantiable subclasses will have an associated Gtk widget:
	virtual Gtk::Widget *internal() = 0;
	virtual const Gtk::Widget *internal() const = 0;
	// Widget signals are forwarded from this (may differ from
	// internal() for a composite widget).
	virtual Gtk::Widget *signals_from();
	Container *get_parent();
	RefPtr<Display> get_display();
	RefPtr<Screen> get_screen();
	RefPtr<XWindow> get_window();
	RefPtr<Context> get_font_context();
	Main *get_main() const;
	void show();
	void hide();
	int get_width() const;
	int get_height() const;
	void set_size_request(int width=-1, int height=-1);
	bool is_visible() const;
	bool is_realized() const;
	bool is_sensitive() const;
	bool is_mapped() const;
	bool activate();
	String get_name();
	void set_name(const String &s);
	void set_sensitive(bool b);
	void add_destroy_notify_callback(void *data, void *(*f)(void *));
	void remove_destroy_notify_callback(void *data);
	bool translate_coordinates(GtkX::Widget &, int, int, int &, int &);
	Requisition size_request() const;
	virtual void get_pointer(int &x, int &y) const;
	void queue_draw();
	Color get_bg() const;
	Color get_fg() const;
	Color get_bg(StateType) const;
	Color get_fg(StateType) const;
	void set_bg(const Color &);
	void set_fg(const Color &);
	void set_bg(StateType, const Color &);
	void set_fg(StateType, const Color &);
	// FIXME: should be const...
	RefPtr<GC> get_black_gc();
	RefPtr<GC> get_white_gc();
	PropertyProxy<void *> property_user_data();
	PropertyProxy_RO<void *> property_user_data() const;
	sigc::signal<bool, GtkX::Event *> &signal_event();
	sigc::signal<bool, GtkX::EventButton *> &signal_button_press_event();
	sigc::signal<bool, GtkX::EventButton *> &signal_button_release_event();
	sigc::signal<bool, GtkX::EventButton *> &signal_button_press_pre_event();
	sigc::signal<bool, GtkX::EventButton *> &signal_button_release_pre_event();
	sigc::signal<void> &signal_map();
	sigc::signal<void> &signal_unmap();
	sigc::signal<bool, GtkX::EventAny *> signal_delete_event();
	sigc::signal<void, GtkX::Requisition *> signal_size_request();
	void set_event_mask(EventMask events);
	void add_event_mask(EventMask events);
    };

    template <class T>
    PropertyProxy<T>::PropertyProxy(Widget *w0, const char *name0):
	Glib::PropertyProxy<T>(w0->internal(), name0)
    {
    }

    template <class T>
    PropertyProxy<T> &PropertyProxy<T>::operator=(T t)
    {
	Glib::PropertyProxy<T>::operator=(t);
	return *this;
    }

    template <class T>
    PropertyProxy_RO<T>::PropertyProxy_RO(const Widget *w0, const char *name0):
	Glib::PropertyProxy_ReadOnly<T>(const_cast<Widget *>(w0)->internal(), name0)
    {
    }

    class SignalTimeout {
    private:
	int foo;
	// no copy assignment
	SignalTimeout &operator=(const SignalTimeout&);
    public:
	SignalTimeout();
	sigc::connection connect(const sigc::slot<bool>& slot, unsigned int interval);
    };

    enum
    {
	PRIORITY_HIGH = -100,
	PRIORITY_DEFAULT = 0,
	PRIORITY_HIGH_IDLE = 100,
	PRIORITY_DEFAULT_IDLE = 200,
	PRIORITY_LOW = 300
    };

    class SignalIdle
    {
    private:
	int foo;
	// no copy assignment
	SignalIdle& operator=(const SignalIdle&);
    public:
	SignalIdle();
	sigc::connection connect(const sigc::slot<bool> &slot, int priority = PRIORITY_DEFAULT_IDLE);
    };

    SignalTimeout &signal_timeout();
    SignalIdle &signal_idle();

    typedef sigc::slot<void, GtkX::Widget*, GtkX::Event*, GtkX::String*, unsigned int*> ActionProc;
    typedef sigc::connection connection;

    struct ActionRec {
	String string;
	ActionProc proc;
    };

    Widget *get_wrapper(Gtk::Widget *);
}

#endif // GTKX_WIDGET_H
