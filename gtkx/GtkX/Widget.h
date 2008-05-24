// -*- C++ -*-

#ifndef GTKX_WIDGET_H
#define GTKX_WIDGET_H

#include <vector>

#include <gtkmm/widget.h>
#include <gdkmm/pixmap.h>
#include <gdkmm/cursor.h>

#include <GtkX/Events.h>
#include <GtkX/Main.h>

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

    // This class is used to simplify constructors which can take a
    // unicode or old-fashioned C string.
    class String {
	Glib::ustring s_;
    public:
	typedef std::string::size_type size_type;
	typedef Glib::ustring::iterator iterator;
	typedef Glib::ustring::const_iterator const_iterator;
	String(const Glib::ustring &s0);
	String(const char *s0);
	String(void);
	~String(void);
	Glib::ustring &s(void);
	gunichar operator[](int i) const;
	const char *c(void) const; // FIXME - don't expose raw char*
	const char *c_str(void) const; // FIXME - don't expose raw char*
	const Glib::ustring &s(void) const;
	String operator+(const String &str) const;
	String &operator+=(const String &str);
	bool operator==(const String &s) const;
	operator bool(void) const;
	bool empty(void) const;
	iterator begin(void);
	iterator end(void);
	const_iterator begin(void) const;
	const_iterator end(void) const;
	size_type find(const String &str, size_type i=0) const;
    };

    const GtkX::String mklabel(const GtkX::String &name,
			       const GtkX::String &label);

    template <class T>
    class RefPtr {
	T *p;
    public:
	RefPtr(void) {
	    p = NULL;
	}
	RefPtr(T *ptr) {
	    p = ptr;
	    if (p) p->ref();
	}
	RefPtr(const RefPtr &rp) {
	    p = rp.p;
	    if (p) p->ref();
	}
	template <class S> inline RefPtr(const RefPtr<S>& src);
	RefPtr &operator=(const RefPtr &rp) {
	    if (rp.p) rp.p->ref();
	    if (p) p->unref();
	    p = rp.p;
	    return *this;
	}
	RefPtr &operator=(T *val) {
	    if (val) val->ref();
	    if (p) p->unref();
	    p = val;
	    return *this;
	}
	~RefPtr(void) {
	    if (p) p->unref();
	}
	operator T *(void) const {
	    return p;
	}
	T &operator*(void) const {
	    return *p;
	}
	T *operator->() const {
	    return &(operator*());
	}
	void clear(void) {
	}
    };

    template <class T>
    template <class S>
    inline RefPtr<T>::RefPtr(const RefPtr<S>& src)
    {
	p = src.operator->();
	if (p) {p->ref();}
    }

    class Screen;

    class Display {
	int nrefs_;
	Glib::RefPtr<Gdk::Display> disp_;
    public:
	Display(Glib::RefPtr<Gdk::Display> d0);
	static RefPtr<Display> wrap(Glib::RefPtr<Gdk::Display> d0);
	static RefPtr<const Display> wrap(Glib::RefPtr<const Gdk::Display> d0);
	Glib::RefPtr<Gdk::Display> internal(void);
	Glib::RefPtr<const Gdk::Display> internal(void) const;
	int ref(void) {return nrefs_++;}
	int unref(void) {if (!--nrefs_) delete this;}
	String get_name(void) const;
	void flush(void);
	static RefPtr<Display> open(const String& display_name);
	void close(void);
	void pointer_ungrab(unsigned int);
	void keyboard_ungrab(unsigned int);
	RefPtr<Screen> get_default_screen(void);
    };

    class XWindow;

    class Screen {
	int nrefs_;
	Glib::RefPtr<Gdk::Screen> screen_;
    public:
	Screen(Glib::RefPtr<Gdk::Screen> s0);
	static RefPtr<Screen> wrap(Glib::RefPtr<Gdk::Screen> s0);
	static RefPtr<const Screen> wrap(Glib::RefPtr<const Gdk::Screen> s0);
	Glib::RefPtr<Gdk::Screen> internal(void);
	Glib::RefPtr<const Gdk::Screen> internal(void) const;
	int ref(void) {return nrefs_++;}
	int unref(void) {if (!--nrefs_) delete this;}
	RefPtr<XWindow> get_root_window(void);
	String make_display_name(void) const;
    };

    class FontDescription: public Pango::FontDescription {
    public:
	FontDescription(void);
	FontDescription(const Pango::FontDescription &);
    };

    class Font {
	int nrefs_;
	Glib::RefPtr<Pango::Font> font_;
    public:
	Font(Glib::RefPtr<Pango::Font> f0);
	static RefPtr<Font> wrap(Glib::RefPtr<Pango::Font> f0);
	static RefPtr<const Font> wrap(Glib::RefPtr<const Pango::Font> f0);
	Glib::RefPtr<Pango::Font> internal(void);
	Glib::RefPtr<const Pango::Font> internal(void) const;
	int ref(void) {return nrefs_++;}
	int unref(void) {if (!--nrefs_) delete this;}
	FontDescription describe(void) const;
    };

    class Drawable;

    class Pixmap;

    struct Color {
	double r;
	double g;
	double b;
	Color(void) {
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
	static RefPtr<GC> wrap(Glib::RefPtr<Gdk::GC> g0);
	static RefPtr<const GC> wrap(Glib::RefPtr<const Gdk::GC> g0);
	Glib::RefPtr<Gdk::GC> internal(void);
	Glib::RefPtr<const Gdk::GC> internal(void) const;
	GC(const RefPtr<Drawable> &d);
	int ref(void) {return nrefs_++;}
	int unref(void) {if (!--nrefs_) delete this;}
	void set_foreground(const Color &color);
	Color get_foreground(void) const;
	void set_background(const Color &color);
	Color get_background(void) const;
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
	Point(void) {}
	Point(int x0, int y0) {
	    x = x0;
	    y = y0;
	}
    };

    class Drawable {
	int nrefs_;
    public:
	virtual Glib::RefPtr<Gdk::Drawable> internal(void) = 0;
	virtual Glib::RefPtr<const Gdk::Drawable> internal(void) const = 0;
	int ref(void) {return nrefs_++;}
	int unref(void) {if (!--nrefs_) delete this;}
	int get_depth(void) const;
	void draw_line(RefPtr<GC> gc,
		       int x1, int y1, int x2, int y2);
	void draw_rectangle(RefPtr<GC> gc, bool filled,
			    int x, int y, int w, int h);
	void draw_arc(RefPtr<GC> gc, bool filled,
		      int x, int y, int w, int h,
		      int angle1, int angle2);
	void draw_polygon(RefPtr<GC> gc, bool filled,
			  std::vector<Point> &points);
    };

    class Pixmap: public Drawable {
	Glib::RefPtr<Gdk::Pixmap> pixmap_;
    public:
	Pixmap(Glib::RefPtr<Gdk::Pixmap> p0);
	static RefPtr<Pixmap> wrap(Glib::RefPtr<Gdk::Pixmap> p0);
	Glib::RefPtr<Gdk::Drawable> internal(void);
	Glib::RefPtr<const Gdk::Drawable> internal(void) const;
	Glib::RefPtr<Gdk::Pixmap> internal_pixmap(void);
	Glib::RefPtr<const Gdk::Pixmap> internal_pixmap(void) const;
	static RefPtr<Pixmap> create_bitmap_from_data(const char *bits, int width, int height);
	static RefPtr<Pixmap> create_from_data(RefPtr<Drawable> drawable,
                                               const char *data, int width, int height,
					       int depth, const Color &fg, const Color &bg);
	Glib::RefPtr<Gdk::Pixmap> gdk_pixmap(void);
	const Glib::RefPtr<Gdk::Pixmap> gdk_pixmap(void) const;
    };

    class Cursor;

    class XWindow: public Drawable {
	Glib::RefPtr<Gdk::Window> win_;
    public:
	XWindow(Glib::RefPtr<Gdk::Window> w0);
	static RefPtr<XWindow> wrap(Glib::RefPtr<Gdk::Window> w0);
	static RefPtr<const XWindow> wrap(Glib::RefPtr<const Gdk::Window> w0);
	Glib::RefPtr<Gdk::Drawable> internal(void);
	Glib::RefPtr<const Gdk::Drawable> internal(void) const;
	Glib::RefPtr<Gdk::Window> internal_window(void);
	Glib::RefPtr<const Gdk::Window> internal_window(void) const;
	void set_back_pixmap(const RefPtr<Pixmap> &pixmap, bool parent_relative);
	void clear(void);
	void clear_area(int x, int y, int w, int h);
	GrabStatus pointer_grab(bool owner_events, EventMask event_mask,
				RefPtr<XWindow>& confine_to,
				Cursor &cursor, unsigned int time_);
	GrabStatus pointer_grab(bool owner_events, EventMask event_mask,
				unsigned int time_);
	void set_title(const String &);
    };

    class Cursor: public Gdk::Cursor {
    public:
	Cursor(const RefPtr<Pixmap> &source, const RefPtr<Pixmap> &mask,
	       const Color &fg, const Color &bg,
	       int x, int y);
    };

    class Widget {
    protected:
	// In GTK, events may be connected after, or optionally
	// before, the class closure.  For the latter case we provide
	// the pre_event_callbacks.
	sigc::signal<bool, GtkX::EventButton *> signal_button_press_event_;
	bool button_press_event_callback(GdkEventButton *ev);
	sigc::signal<bool, GtkX::EventButton *> signal_button_release_event_;
	bool button_release_event_callback(GdkEventButton *ev);
	sigc::signal<bool, GtkX::EventButton *> signal_button_press_pre_event_;
	bool button_press_pre_event_callback(GdkEventButton *ev);
	sigc::signal<bool, GtkX::EventButton *> signal_button_release_pre_event_;
	bool button_release_pre_event_callback(GdkEventButton *ev);
	sigc::signal<void> signal_map_;
	void signal_map_callback(void);
	sigc::signal<void> signal_unmap_;
	void signal_unmap_callback(void);
    public:
	Widget(void);
	virtual ~Widget(void);
	void init_signals(void);
	void postinit(void);
	// Instantiable subclasses will have an associated Gtk widget:
	virtual Gtk::Widget *internal(void) = 0;
	virtual const Gtk::Widget *internal(void) const = 0;
	// Widget signals are forwarded from this (may differ from
	// internal() for a composite widget).
	virtual Gtk::Widget *signals_from(void);
	Container *get_parent(void);
	RefPtr<Display> get_display(void);
	RefPtr<Screen> get_screen(void);
	RefPtr<XWindow> get_window(void);
	Main *get_main(void) const;
	void show(void);
	void hide(void);
	bool is_visible(void);
	bool is_realized(void);
	String get_name(void);
	void set_name(const String &s);
	void set_sensitive(bool b);
	void add_destroy_notify_callback(void *data, void *(*f)(void *));
	void remove_destroy_notify_callback(void *data);
	bool translate_coordinates(GtkX::Widget &, int, int, int &, int &);
	Requisition size_request(void) const;
	Color get_bg(void) const;
	Color get_fg(void) const;
	Color get_bg(StateType) const;
	Color get_fg(StateType) const;
	void set_bg(const Color &);
	void set_fg(const Color &);
	void set_bg(StateType, const Color &);
	void set_fg(StateType, const Color &);
	// FIXME: should be const...
	RefPtr<GC> get_black_gc();
	RefPtr<GC> get_white_gc();
	PropertyProxy<void *> property_user_data(void);
	PropertyProxy_RO<void *> property_user_data(void) const;
	sigc::signal<bool, GtkX::EventButton *> &signal_button_press_event();
	sigc::signal<bool, GtkX::EventButton *> &signal_button_release_event();
	sigc::signal<bool, GtkX::EventButton *> &signal_button_press_pre_event();
	sigc::signal<bool, GtkX::EventButton *> &signal_button_release_pre_event();
	sigc::signal<void> &signal_map();
	sigc::signal<void> &signal_unmap();
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
	SignalTimeout(void);
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
	SignalIdle(void);
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
