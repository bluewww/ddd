#include <GtkX/Container.h>
#include <gtkmm/box.h>

using namespace GtkX;

Container::Container(void)
{
}

Container::~Container(void)
{
}

// This may be overridden.  The container to which we will add stuff
// may not necessarily be the directly corresponding GTK widget.  For
// example, in the case of Dialogs, we almost always want to add to
// the VBox returned by get_vbox().
Gtk::Container *
Container::gtk_container(void)
{
    Gtk::Widget *widget = internal();
    if (!widget) return NULL;
    return static_cast<Gtk::Container *>(widget);
}

// If the container is an instance of Gtk::Box, pack_start() is used,
// and the options will be honoured.
// Otherwise, the default add() method is invoked.

void
Container::add_child(Widget &child, PackOptions options, int padding)
{
    Gtk::Container *cont = gtk_container();
    Gtk::Box *box = dynamic_cast<Gtk::Box *>(cont);
    Gtk::PackOptions tr[] = {Gtk::PACK_SHRINK,
			     Gtk::PACK_EXPAND_PADDING,
			     Gtk::PACK_EXPAND_WIDGET};
    if (box)
	box->pack_start(*child.internal(), tr[options], padding);
    else
	cont->add(*child.internal());
}

ChildList
Container::get_children(void)
{
    Gtk::Container *cont = gtk_container();
    return ChildList(cont->get_children());
}

void
Container::remove(Widget &w)
{
    // We cannot reassign w to a new parent, therefore
    // it must be finished with.
    delete &w;
}

