// -*- C++ -*-

// High-level GUI wrapper for Xm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of Xmmm.
// 
// Xmmm is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// Xmmm is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with Xmmm -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.

#ifndef XMMM_LISTVIEW_H
#define XMMM_LISTVIEW_H

#include <vector>
#include <sigc++/signal.h>
#include <Xmmm/Container.h>
#include <Xm/List.h>

namespace Xmmm {

    class ListView: public Widget {
    private:
	::Widget list_;
    protected:
	sigc::signal<void> signal_selection_changed_;
	static void selection_changed_callback(::Widget widget, XtPointer data);
    public:
	void init_signals(void);
	void init(::Widget parent, const String &name,
		  const std::vector<String> &headers);
	ListView(Container &parent, const String &name,
		 const std::vector<String> &headers);
	ListView(::Widget parent, const String &name,
		 const std::vector<String> &headers); // TEMPORARY
	~ListView(void);
	::Widget internal(void); // TEMPORARY
	std::string get_selected(void);
	void clear(void);
	void append(const String &item);
	int get_selected_pos(void);
	int count(void) const;
	sigc::signal<void> &signal_selection_changed(void);
    };

}

#endif // XMMM_LISTVIEW_H
