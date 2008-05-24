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

#ifndef GTKSCROLLEDTEXT_H
#define GTKSCROLLEDTEXT_H

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>

enum XmHighlightMode {
  XmHIGHLIGHT_NORMAL,
  XmHIGHLIGHT_SELECTED,
  XmHIGHLIGHT_SECONDARY_SELECTED,
  XmSEE_DETAIL
};

struct GtkGlyphMark
{
    Glib::RefPtr<Gdk::Pixbuf> glyph;
    int x;
    int y;
    GtkGlyphMark(Glib::RefPtr<Gdk::Pixbuf> g0, int x0, int y0) {
	glyph = g0;
	x = x0;
	y = y0;
    }
};

class GtkMarkedTextView: public Gtk::TextView
{
    bool on_expose_event(GdkEventExpose *event);
    std::list<GtkGlyphMark *> marks;
public:
    void pos_to_xy(long pos, int &x, int &y);
    GtkGlyphMark *map_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph, int x, int y);
    GtkGlyphMark *map_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph, long pos);
    // Unmark a particular glyph:
    void unmap_glyph(GtkGlyphMark *mark);
    // Unmark all instances of a glyph:
    void unmap_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph);
    void refresh_line(int y, int height);
};

class GtkScrolledText: public Gtk::ScrolledWindow
{
private:
    GtkMarkedTextView tv_;
    Glib::RefPtr<Gtk::TextBuffer> tb_;
public:
    GtkScrolledText(void);
    Glib::SignalProxy0<void> signal_changed(void); // From the TextBuffer
    bool get_editable(void) const;
    void set_editable(bool);
    long get_last_position(void);
    void set_insertion_position(long pos);
    long get_insertion_position(void);
    bool get_selection_bounds(long &begin, long &end);
    void clear_selection(void);
    void replace(long pos1, long pos2, const Glib::ustring &repl);
    void insert(long pos, const Glib::ustring &repl);
    void set_text(const Glib::ustring &repl);
    Glib::ustring get_text(void);
    Glib::ustring get_text(long, long);
    void show_position(long pos);
    GtkMarkedTextView &view(void);
    Glib::RefPtr<Gtk::TextBuffer> buffer(void);
    void set_highlight(long pos1, long pos2, XmHighlightMode);
    long find_forward(Glib::ustring str, long start=0);
    long find_backward(Glib::ustring str, long start=-1);
    long find_forward(gunichar c, long start=0);
    long find_backward(gunichar c, long start=-1);
    long xy_to_pos(double x, double y);
    bool pos_to_xy(long pos, int &x, int &y);
    int get_rows();
    int get_columns();
};

#endif
