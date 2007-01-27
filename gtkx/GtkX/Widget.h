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
    };

    class Widget {
    public:
	Widget(void);
	~Widget(void);
	// *Instantiable* subclasses will have an associated GTK(mm) widget:
	virtual Gtk::Widget *gtk_widget(void) = 0;
    };

}

#endif // GTKX_WIDGET_H
