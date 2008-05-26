// -*- C++ -*-

#ifndef GTKX_WIDGET_H
#define GTKX_WIDGET_H

#include <sigc++/signal.h>

#include <qwidget.h>

#include <QtX/Events.h>
#include <QtX/Main.h>

namespace QtX {

    class Widget;
    class Container;

    struct Requisition
    {
	int width;
	int height;
    };

    template <class T>
    class PropertyProxy {
	Widget *w;
	const char *name;
    public:
	PropertyProxy(Widget *w0, const char *name0);
	PropertyProxy &operator=(T);
    };

    template <class T>
    class PropertyProxy_RO {
	const Widget *w;
	const char *name;
    public:
	PropertyProxy_RO(const Widget *w0, const char *name0);
    };

    // This class is used to simplify constructors which can take a
    // unicode or old-fashioned C string.
    typedef unsigned int UChar;

    class String {
	QString s_;
    public:
	typedef std::string::size_type size_type;
	typedef QString::iterator iterator;
	typedef QString::const_iterator const_iterator;
	String(const QString &s0);
	String(const char *s0);
	String(void);
	~String(void);
	QString &s(void);
	QChar operator[](int i) const;
	const char *c(void) const; // FIXME - don't expose raw char*
	const char *c_str(void) const; // FIXME - don't expose raw char*
	const QString &s(void) const;
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

#if 0
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
#endif

    class XWindow;

#if 0
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
#endif

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

    class Widget {
    protected:
	sigc::signal<bool, QtX::EventButton *> signal_button_press_event_;
	bool button_press_event_callback(QMouseEvent *ev);
	sigc::signal<bool, QtX::EventButton *> signal_button_release_event_;
	bool button_release_event_callback(QMouseEvent *ev);
	sigc::signal<void> signal_map_;
	void signal_map_callback(void);
	sigc::signal<void> signal_unmap_;
	void signal_unmap_callback(void);
    public:
	Widget(void);
	~Widget(void);
	void init_signals(void);
	void postinit(void);
	// *Instantiable* subclasses will have an associated Qt widget:
	virtual QWidget *internal(void) = 0;
	virtual const QWidget *internal(void) const = 0;
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
	bool translate_coordinates(QtX::Widget &, int, int, int &, int &);
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
	sigc::signal<bool, QtX::EventButton *> &signal_button_press_event();
	sigc::signal<bool, QtX::EventButton *> &signal_button_release_event();
	sigc::signal<void> &signal_map();
	sigc::signal<void> &signal_unmap();
    };

}

#endif // GTKX_WIDGET_H
