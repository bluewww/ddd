#ifndef GTKX_WIDGET_H
#define GTKX_WIDGET_H

#include <gtkmm/widget.h>

namespace GtkX {

    // This class is used to simplify constructors which can take a
    // unicode or old-fashioned C string.
    class String {
	Glib::ustring s_;
    public:
	String(const Glib::ustring &s0);
	String(const char *s0);
	~String(void);
	Glib::ustring &s(void);
	const Glib::ustring &s(void) const;
	String operator+(const String &str) const;
    };

    class Widget {
    public:
	Widget(void);
	~Widget(void);
	// *Instantiable* subclasses will have an associated GTK(mm) widget:
	virtual Gtk::Widget *internal(void) = 0;
	void show(void);
	void hide(void);
    };

}

#endif // GTKX_WIDGET_H
