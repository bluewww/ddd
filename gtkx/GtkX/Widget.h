// -*- C++ -*-

#ifndef GTKX_WIDGET_H
#define GTKX_WIDGET_H

#include <gtkmm/widget.h>

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

    class Widget {
    public:
	Widget(void);
	~Widget(void);
	void postinit(void);
	// *Instantiable* subclasses will have an associated GTK(mm) widget:
	virtual Gtk::Widget *internal(void) = 0;
	Container *get_parent(void);
	void show(void);
	void hide(void);
	String get_name(void);
	void set_name(const String &s);
	void set_sensitive(bool b);
	PropertyProxy<void *> property_user_data(void);
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

}

#endif // GTKX_WIDGET_H
