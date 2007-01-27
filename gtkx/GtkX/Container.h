#ifndef GTKX_CONTAINER_H
#define GTKX_CONTAINER_H

#include <gtkmm/container.h>
#include <GtkX/Widget.h>

namespace GtkX {

    class Container: public GtkX::Widget {
    public:
	Container(void);
	~Container(void);
	virtual Gtk::Container *gtk_container(void);
	virtual void add_child(GtkX::Widget &child);
    };

}

#endif // GTKX_CONTAINER_H
