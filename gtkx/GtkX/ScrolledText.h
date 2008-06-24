// -*- C++ -*-

// High-level GUI wrapper for Gtkmm.

// Copyright (C) 2007 Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of GtkX.
// 
// GtkX is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// GtkX is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with GtkX -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.

// High level object-oriented wrapper must support Gtk and Motif.
// Unfortunately Motif widgets require parent and name arguments to
// the constructor, unlike the Gtk ones.  Motif (Xt) widgets cannot be
// reparented.  Therefore we need a constructor with extra arguments.
// A brief look at QT indicates that this will be required there as
// well.

#ifndef SCROLLEDTEXT_H
#define SCROLLEDTEXT_H

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>

#include <GtkX/Container.h>

namespace GtkX {

    enum HighlightMode {
	HIGHLIGHT_NORMAL,
	HIGHLIGHT_SELECTED,
	HIGHLIGHT_SECONDARY_SELECTED,
	SEE_DETAIL
    };

    struct GlyphMark
    {
	Glib::RefPtr<Gdk::Pixbuf> glyph;
	int x;
	int y;
	GlyphMark(Glib::RefPtr<Gdk::Pixbuf> g0, int x0, int y0) {
	    glyph = g0;
	    x = x0;
	    y = y0;
	}
    };

    class Rectangle {
      int x_;
      int y_;
      int w_;
      int h_;
    public:
      Rectangle(int x0=0, int y0=0, int w0=0, int h0=0) {
	x_ = x0;
	y_ = y0;
	w_ = w0;
	h_ = h0;
      }
      Rectangle(const Rectangle &src) {
	x_ = src.x_;
	y_ = src.y_;
	w_ = src.w_;
	h_ = src.h_;
      }
      Rectangle &operator=(const Rectangle &src) {
	x_ = src.x_;
	y_ = src.y_;
	w_ = src.w_;
	h_ = src.h_;
	return *this;
      }
      int get_x() const {return x_;}
      void set_x(int x0) {x_ = x0;}
      int get_y() const {return y_;}
      void set_y(int y0) {y_ = y0;}
      int get_width() const {return w_;}
      void set_width(int w0) {w_ = w0;}
      int get_height() const {return h_;}
      void set_height(int h0) {h_ = h0;}
    };

    class MarkedTextView: public GtkX::Widget, public Gtk::TextView
    {
	bool on_expose_event(GdkEventExpose *event);
	std::list<GlyphMark *> marks;
    public:
	MarkedTextView();
	Gtk::Widget *internal();
	const Gtk::Widget *internal() const;
	void pos_to_xy(long pos, int &x, int &y);
	void pos_to_rect(long pos, Rectangle &rect);
	GlyphMark *map_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph, int x, int y);
	GlyphMark *map_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph, long pos);
	// Unmark a particular glyph:
	void unmap_glyph(GlyphMark *mark);
	// Unmark all instances of a glyph:
	void unmap_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph);
	void refresh_line(int y, int height);
	void buffer_to_window_xy(int xin, int yin, int &xout, int &yout);
#include <GtkX/redirect.h>
    };

    class ScrolledText: public GtkX::Widget
    {
	Gtk::ScrolledWindow *sw_;
	MarkedTextView *tv_;
	Glib::RefPtr<Gtk::TextBuffer> tb_;
	sigc::signal<void> signal_changed_;
	void changed_callback();
    public:
	ScrolledText(GtkX::Container &parent, PackOptions po=PACK_SHRINK,
		     const GtkX::String &name="", const GtkX::String &label="");
	~ScrolledText();
	Gtk::Widget *internal();
	const Gtk::Widget *internal() const;
	void init_signals();
	void postinit();
	Gtk::Widget *signals_from();
	bool get_editable() const;
	void set_editable(bool);
	long get_last_position();
	void set_insertion_position(long pos);
	long get_insertion_position();
	bool get_selection_bounds(long &begin, long &end);
	void clear_selection();
	void replace(long pos1, long pos2, const String &repl);
	void insert(long pos, const String &repl);
	void set_text(const String &repl);
	String get_text();
	String get_text(long, long);
	void show_position(long pos);
	MarkedTextView &view();
	Glib::RefPtr<Gtk::TextBuffer> buffer();
	void set_highlight(long pos1, long pos2, HighlightMode);
	long find_forward(String str, long start=0);
	long find_backward(String str, long start=-1);
	long find_forward(UChar c, long start=0);
	long find_backward(UChar c, long start=-1);
	long xy_to_pos(double x, double y);
	void window_to_buffer_xy(double xin, double yin,
				 double &xout, double &yout);
	bool pos_to_xy(long pos, int &x, int &y);
	int get_rows();
	int get_columns();
	sigc::signal<void> &signal_changed();
    };

}

#endif
