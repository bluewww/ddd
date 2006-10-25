#ifndef GTKMULTIPANED_H
#define GTKMULTIPANED_H

#include <gtkmm/paned.h>
#include <list>

class GtkMultiPaned: public Gtk::VPaned
{
private:
    std::list<Gtk::VPaned *> xchild_;
public:
    GtkMultiPaned(void);
    void on_add(Gtk::Widget* widget);
    bool on_expose_event(GdkEventExpose* event);
    void debug(void);
};

#endif
