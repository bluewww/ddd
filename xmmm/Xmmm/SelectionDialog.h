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

#ifndef XMMM_SELECTIONDIALOG_H
#define XMMM_SELECTIONDIALOG_H

#include <Xmmm/Container.h>
#include <Xmmm/Box.h>
#include <Xmmm/Box1.h>
#include <Xmmm/ScrolledWindow.h>
#include <Xmmm/ListView.h>
#include <Xmmm/Button.h>

namespace Xmmm {

    class SelectionDialog: public Container {
	::Widget dlg_;
	VBox *box_;
	Box1 *box1_;
	ScrolledWindow *sw_;
	ListView *list_;
    public:
	void init(::Widget parent, const Xmmm::String &name,
		  const std::vector<String> &headers);
	SelectionDialog(::Widget parent, const Xmmm::String &name,
			const std::vector<String> &headers); // TEMPORARY
	SelectionDialog(Shell &parent, const Xmmm::String &name,
			const std::vector<String> &headers);
	~SelectionDialog(void);
	::Widget internal(void);
	::Widget xt_container(void);
	ListView *list(void) const;
	Button *add_button(const String &name="", const String &label="");
	std::string get_text(void);
	void set_text(const std::string &s);
	void set_text(const XmString s);
    };

}

#endif // XMMM_SELECTIONDIALOG_H
