#ifndef GTKX_CONTAINER_H
#define GTKX_CONTAINER_H

#include <gtkmm/container.h>
#include <GtkX/Widget.h>

namespace GtkX {

    // Compatibility with Gtkmm:
    enum PackOptions
    {
	PACK_SHRINK,
	PACK_EXPAND_PADDING,
	PACK_EXPAND_WIDGET
    };

    class Container: public GtkX::Widget {
    public:
	Container(void);
	~Container(void);
	virtual Gtk::Container *gtk_container(void);
	virtual void add_child(GtkX::Widget &child,
			       PackOptions options=PACK_SHRINK,
			       int padding=0);
    };

    class Shell: public Container {
    };
}

#endif // GTKX_CONTAINER_H
