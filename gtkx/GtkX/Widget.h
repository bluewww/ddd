// -*- C++ -*-

#ifndef GTKX_WIDGET_H
#define GTKX_WIDGET_H

#include <gtkmm/widget.h>

#include <GtkX/Events.h>

// ***** VERY IMPORTANT NOTE ****
// All GtkX classes EXCEPT the leaves are pure interface classes,
// that is, they contain no data.
// This avoids problems with multiple inheritance.

namespace GtkX {

    extern Glib::Quark gtkx_super_quark;

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

    // This class is used to simplify constructors which can take a
    // unicode or old-fashioned C string.
    class String {
	Glib::ustring s_;
    public:
	String(const Glib::ustring &s0);
	String(const char *s0);
	~String(void);
	Glib::ustring &s(void);
	gunichar operator[](int i) const;
	const char *c(void) const; // FIXME - don't expose raw char*
	const char *c_str(void) const; // FIXME - don't expose raw char*
	const Glib::ustring &s(void) const;
	String operator+(const String &str) const;
	bool operator==(const String &s) const;
	operator bool(void) const;
    };

    template <class T>
    class RefPtr: public Glib::RefPtr<T> {
    public:
	RefPtr(void);
	RefPtr(Glib::RefPtr<T> p);
    };

    template<class T>
    RefPtr<T>::RefPtr(void)
    {
    }
    
    template<class T>
    RefPtr<T>::RefPtr(Glib::RefPtr<T> t):
	Glib::RefPtr<T>(t)
    {
    }

    typedef Gdk::Display Display;

    class Widget {
    protected:
	sigc::signal<bool, GtkX::EventButton *> signal_button_press_event_;
	bool button_press_event_callback(GdkEventButton *ev);
	sigc::signal<bool, GtkX::EventButton *> signal_button_release_event_;
	bool button_release_event_callback(GdkEventButton *ev);
	sigc::signal<void> signal_map_;
	void signal_map_callback(void);
	sigc::signal<void> signal_unmap_;
	void signal_unmap_callback(void);
    public:
	Widget(void);
	~Widget(void);
	void init_signals(void);
	void postinit(void);
	// *Instantiable* subclasses will have an associated GTK(mm) widget:
	virtual Gtk::Widget *internal(void) = 0;
	Container *get_parent(void);
	RefPtr<Display> get_display(void);
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
	PropertyProxy<void *> property_user_data(void);
	sigc::signal<bool, GtkX::EventButton *> &signal_button_press_event();
	sigc::signal<bool, GtkX::EventButton *> &signal_button_release_event();
	sigc::signal<void> &signal_map();
	sigc::signal<void> &signal_unmap();
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

    sigc::signal<bool> &signal_idle();

    typedef sigc::slot<void, GtkX::Widget*, GtkX::Event*, char **, unsigned int*> ActionProc;

    struct ActionRec {
	String string;
	ActionProc proc;
    };
}

#endif // GTKX_WIDGET_H
