#ifndef MULTIPANED_H
#define MULTIPANED_H

#include <gtkmm/paned.h>
#include <list>

#include <GtkX/Container.h>

namespace GtkX {

    class MultiPaned: public GtkX::Container, public Gtk::VPaned
    {
    private:
	std::list<Gtk::VPaned *> xchild_;
    public:
	MultiPaned(void);
	MultiPaned(GtkX::Container &parent, const GtkX::String &name="",
		   const GtkX::String &label="");
	Gtk::Widget *internal(void);
	const Gtk::Widget *internal(void) const;
	void on_add(Gtk::Widget* widget);
	bool on_expose_event(GdkEventExpose* event);
	void debug(void);
	static void show_child(Gtk::Widget* widget);
	static void hide_child(Gtk::Widget* widget);
	static void show_child(GtkX::Widget* widget);
	static void hide_child(GtkX::Widget* widget);
#include <GtkX/redirect.h>
    };

}

#endif
