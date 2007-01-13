#include <iostream>

#include "GtkMultiPaned.h"
#include <cassert>

GtkMultiPaned::GtkMultiPaned(void)
{
    set_name("GtkMultiPaned 0");
}

void
GtkMultiPaned::on_add(Gtk::Widget* widget)
{
    int n = get_children().size();
    if (n == 0) {
	// Gtk::VPaned::on_add(widget);
	pack1(*widget, false, false);
    }
    else if (n == 1) {
	assert(xchild_.size() == 0);
	Gtk::VPaned *paned = new Gtk::VPaned();
	paned->set_name("GtkMultiPaned");
	Gtk::VPaned::on_add(paned);
	xchild_.push_back(paned);
	paned->show();
	paned->pack1(*widget, false, false);
    }
    else {
	Gtk::VPaned *paned = new Gtk::VPaned();
	Gtk::VPaned *parent = *xchild_.rbegin();
	paned->set_name(parent->get_name()+Glib::ustring("+"));
	xchild_.push_back(paned);
	parent->add(*paned);
	paned->show();
	paned->pack1(*widget, false, false);
    }
}

void
GtkMultiPaned::debug(void)
{
    Gtk::VPaned *paned = this;
    while (paned) {
	Widget *c1 = paned->get_child1();
	Gtk::Allocation all = c1->get_allocation();
	std::cerr << c1 << " \"" << c1->get_name() << "\" "
		  << c1->is_visible() << " " << c1->is_mapped() << " ("
		  << all.get_x() << " " << all.get_y() << " "
		  << all.get_width() << " " << all.get_height() << ") "
		  << "\n";
	Widget *c2 = paned->get_child2();
	if (c2)
	    paned = dynamic_cast<Gtk::VPaned *>(c2);
	else
	    break;
    }
}

void
GtkMultiPaned::show_child(Gtk::Widget* w)
{
    w->show();
    Gtk::Widget *paned = w->get_parent();
    if (paned)
	paned->show();
}

void
GtkMultiPaned::hide_child(Gtk::Widget *w)
{
    w->hide();
    Gtk::Widget *parent = w->get_parent();
    if (!parent) return;
    Gtk::Paned *paned = dynamic_cast<Gtk::Paned *>(parent);
    if (paned) {
	Gtk::Widget *w1 = paned->get_child1();
	Gtk::Widget *w2 = paned->get_child2();
	// A GtkMultiPaned child is always the first child of a
	// Gtk::Paned widget.
	assert (w == w1);
	if (!(w2 && w2->is_visible()))
	    paned->hide();
    }
}

#ifdef NAG_ME
#warning DEBUGGING CODE
#endif
bool
GtkMultiPaned::on_expose_event(GdkEventExpose* event)
{
    // debug();
    return Gtk::VPaned::on_expose_event(event);
}

