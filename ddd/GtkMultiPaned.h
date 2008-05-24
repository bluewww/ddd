// $Id$
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

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
    static void show_child(Gtk::Widget* widget);
    static void hide_child(Gtk::Widget* widget);
};

#endif
