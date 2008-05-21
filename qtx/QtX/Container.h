// -*- C++ -*-

#ifndef QTX_CONTAINER_H
#define QTX_CONTAINER_H

#include <QtX/Widget.h>

namespace QtX {

    // Compatibility with Gtkmm:
    enum PackOptions
    {
	PACK_SHRINK,
	PACK_EXPAND_PADDING,
	PACK_EXPAND_WIDGET
    };

    class RadioButton;

#if 0
    class ChildIterator {
	Glib::ListHandle<Gtk::Widget*>::iterator iter;
    public:
	ChildIterator(void):
	    iter(0) {
	}
	ChildIterator(const ChildIterator &src):
	    iter(src.iter) {
	}
	ChildIterator(const Glib::ListHandle<Gtk::Widget*>::iterator &src):
	    iter(src) {
	}
	ChildIterator &operator=(const ChildIterator &src) {
	    iter = src.iter;
	    return *this;
	}
	bool operator==(const ChildIterator &src) {
	    return (iter == src.iter);
	}
	bool operator!=(const ChildIterator &src) {
	    return (iter != src.iter);
	}
	Widget *operator*(void) {
	    return (Widget *)(*iter)->get_data(qtx_super_quark);
	}
	ChildIterator &operator++(void) {
	    ++iter;
	    return *this;
	}
	const ChildIterator operator++(int) {
	    const ChildIterator tmp(*this);
	    iter++;
	    return tmp;
	}
    };

    class ChildList {
	Glib::ListHandle<Gtk::Widget*> list;
	// No copy assignment
	ChildList &operator=(const ChildList &src);
    public:
	ChildList(const ChildList &src):
	    list(src.list) {
	}
	ChildList(const Glib::ListHandle<Gtk::Widget*> &src):
	    list(src) {
	}
	ChildIterator begin(void) {
	    return ChildIterator(list.begin());
	}
	ChildIterator end(void) {
	    return ChildIterator(list.end());
	}
    };
#endif

    class Container: public QtX::Widget {
    protected:
	// Gtk::RadioButtonGroup grp;
    public:
	Container(void);
	~Container(void);
	// virtual Gtk::Container *gtk_container(void);
	virtual void add_child(QtX::Widget &child,
			       PackOptions options=PACK_EXPAND_WIDGET,
			       int padding=0);
	// ChildList get_children(void);
	void remove(Widget &w);
	// Gtk::RadioButtonGroup &button_group(void);
    };

    class Shell: public Container {
    };
}

#endif // QTX_CONTAINER_H
