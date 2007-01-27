#include <GtkX/Container.h>

GtkX::Container::Container(void)
{
}

GtkX::Container::~Container(void)
{
}

// This may be overridden.  The container to which we will add stuff
// may not necessarily be the directly corresponding GTK widget.  For
// example, in the case of Dialogs, we almost always want to add to
// the VBox returned by get_vbox().
Gtk::Container *
GtkX::Container::gtk_container(void)
{
    Gtk::Widget *widget = gtk_widget();
    if (!widget) return NULL;
    return static_cast<Gtk::Container *>(widget);
}

void
GtkX::Container::add_child(GtkX::Widget &child)
{
    gtk_container()->add(*child.gtk_widget());
}

