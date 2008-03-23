#include <iostream>

#include <GtkX/MultiPaned.h>
#include <cassert>

using namespace GtkX;

MultiPaned::MultiPaned(void)
{
}

MultiPaned::MultiPaned(GtkX::Container &parent, const GtkX::String &name,
		       const GtkX::String &label)
{
    set_name(name.s());
    parent.add_child(*this);
    postinit();
}

Gtk::Widget *
MultiPaned::internal(void)
{
    return this;
}

const Gtk::Widget *
MultiPaned::internal(void) const
{
    return this;
}

void
MultiPaned::on_add(Gtk::Widget* widget)
{
    int n = Gtk::Container::get_children().size();
    if (n == 0) {
	// Gtk::VPaned::on_add(widget);
	pack1(*widget, false, false);
    }
    else if (n == 1) {
	assert(xchild_.size() == 0);
	Gtk::VPaned *paned = new Gtk::VPaned();
	paned->set_name("MultiPaned");
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
MultiPaned::debug(void)
{
    Gtk::VPaned *paned = this;
    while (paned) {
	Gtk::Widget *c1 = paned->get_child1();
	Gtk::Allocation all = c1->get_allocation();
	std::cerr << c1 << " \"" << c1->get_name() << "\" "
		  << c1->is_visible() << " " << c1->is_mapped() << " ("
		  << all.get_x() << " " << all.get_y() << " "
		  << all.get_width() << " " << all.get_height() << ") "
		  << "\n";
	Gtk::Widget *c2 = paned->get_child2();
	if (c2)
	    paned = dynamic_cast<Gtk::VPaned *>(c2);
	else
	    break;
    }
}

void
MultiPaned::show_child(Gtk::Widget* w)
{
    w->show();
    Gtk::Widget *paned = w->get_parent();
    if (paned)
	paned->show();
}

void
MultiPaned::hide_child(Gtk::Widget *w)
{
    w->hide();
    Gtk::Widget *parent = w->get_parent();
    if (!parent) return;
    Gtk::Paned *paned = dynamic_cast<Gtk::Paned *>(parent);
    if (paned) {
	Gtk::Widget *w1 = paned->get_child1();
	Gtk::Widget *w2 = paned->get_child2();
	// A MultiPaned child is always the first child of a
	// Gtk::Paned widget.
	assert (w == w1);
	if (!(w2 && w2->is_visible()))
	    paned->hide();
    }
}

void
MultiPaned::show_child(GtkX::Widget* widget)
{
    MultiPaned::show_child(widget->internal());
}

void
MultiPaned::hide_child(GtkX::Widget* widget)
{
    MultiPaned::hide_child(widget->internal());
}

#ifdef NAG_ME
#warning DEBUGGING CODE
#endif
bool
MultiPaned::on_expose_event(GdkEventExpose* event)
{
    // debug();
    return Gtk::VPaned::on_expose_event(event);
}

