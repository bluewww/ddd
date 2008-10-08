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

#include <iostream>
#include <iomanip>

#include <GtkX/Container.h>
#include <GtkX/ScrolledText.h>

// #include "SourceView.h"

using namespace GtkX;

extern ScrolledText *gdb_w;

// MarkedTextView must be a subclass of TextView, since it calls
// TextView::on_expose_event() which is protected.

MarkedTextView::MarkedTextView()
{
    postinit();
}

Gtk::Widget *
MarkedTextView::internal()
{
    return this;
}

const Gtk::Widget *
MarkedTextView::internal() const
{
    return this;
}

void
MarkedTextView::init_signals()
{
    Gtk::Widget::signal_key_press_event().connect(sigc::mem_fun(*this, &MarkedTextView::key_press_event_callback));
    Gtk::Widget::signal_key_release_event().connect(sigc::mem_fun(*this, &MarkedTextView::key_release_event_callback));
    Gtk::Widget::signal_key_press_event().connect(sigc::mem_fun(*this, &MarkedTextView::key_press_pre_event_callback), false);
    Gtk::Widget::signal_key_release_event().connect(sigc::mem_fun(*this, &MarkedTextView::key_release_pre_event_callback), false);
}

void
MarkedTextView::postinit()
{
    Widget::postinit();
    init_signals();
}

void
MarkedTextView::pos_to_rect(long pos, Rectangle &rect)
{
    Gtk::TextIter iter = get_buffer()->get_iter_at_offset(pos);
    Gdk::Rectangle grect;
    get_iter_location(iter, grect);
    rect.set_x(grect.get_x());
    rect.set_y(grect.get_y());
    rect.set_width(grect.get_width());
    rect.set_height(grect.get_height());
}

bool
MarkedTextView::on_expose_event(GdkEventExpose *event)
{
    Gtk::TextView::on_expose_event(event);
    std::list<GlyphMark *>::iterator iter;
    for (iter = marks.begin(); iter != marks.end(); iter++) {
	Glib::RefPtr<Gdk::Window> win = Gtk::TextView::get_window(Gtk::TEXT_WINDOW_TEXT);
	Glib::RefPtr<Gtk::Style> style = get_style();
	Glib::RefPtr<Gdk::GC> gc = style->get_light_gc(Gtk::STATE_NORMAL);
	Glib::RefPtr<Gdk::Pixbuf> glyph = (*iter)->glyph;
	int ix, iy;
	buffer_to_window_xy((*iter)->x, (*iter)->y, ix, iy);
	win->draw_pixbuf(gc, glyph, 0, 0, ix, iy,
			 glyph->get_width(), glyph->get_height(),
			 Gdk::RGB_DITHER_NONE, 0, 0);
    }
}

void
MarkedTextView::refresh_line(int y, int height)
{
    Glib::RefPtr<Gdk::Window> win = Gtk::TextView::get_window(Gtk::TEXT_WINDOW_TEXT);
    if (win) {
	Gdk::Rectangle rect;
	int x1, y1;
	buffer_to_window_xy(0, y, x1, y1);
	std::cerr << "this=" << this << " y=" << y << " y1=" << y1 << "\n";
	rect.set_x(x1);
	rect.set_y(y1);
	rect.set_width(get_width());
	rect.set_height(height);
	win->invalidate_rect(rect, false);
    }
}

GlyphMark *
MarkedTextView::map_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph, int x, int y)
{
    GlyphMark *new_mark = new GlyphMark(glyph, x, y);
    marks.push_back(new_mark);
    refresh_line(y, glyph->get_height());
    return new_mark;
}

void
MarkedTextView::pos_to_xy(long pos, int &x, int &y)
{
    Gtk::TextIter iter;
    iter = get_buffer()->get_iter_at_offset(pos);
    Gdk::Rectangle location;
    get_iter_location(iter, location);
    x = (int)location.get_x();
    y = (int)location.get_y();
}

GlyphMark *
MarkedTextView::map_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph, long pos)
{
    int x, y;
    pos_to_xy(pos, x, y);
    return map_glyph(glyph, x, y);
}

void
MarkedTextView::unmap_glyph(GlyphMark *mark)
{
    std::list<GlyphMark *>::iterator iter;
    for (iter = marks.begin(); iter != marks.end(); iter++) {
	if ((*iter) == mark) {
	    int x = mark->x;
	    int y = mark->y;
	    int height = mark->glyph->get_height();
	    marks.erase(iter);
	    refresh_line(y, height);
	    break;
	}
    }
}

void
MarkedTextView::unmap_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph)
{
    std::list<GlyphMark *>::iterator iter;
redo:
    for (iter = marks.begin(); iter != marks.end(); iter++) {
	if ((*iter)->glyph == glyph) {
	    int x = (*iter)->x;
	    int y = (*iter)->y;
	    int height = (*iter)->glyph->get_height();
	    marks.erase(iter);
	    refresh_line(y, height);
	    goto redo;
	}
    }
}

void
MarkedTextView::buffer_to_window_xy(int xin, int yin, int &xout, int &yout)
{
    Gtk::TextView::buffer_to_window_coords(Gtk::TEXT_WINDOW_TEXT, xin, yin,
					   xout, yout);
}

void
MarkedTextView::window_to_buffer_xy(int xin, int yin, int &xout, int &yout)
{
    Gtk::TextView::window_to_buffer_coords(Gtk::TEXT_WINDOW_TEXT, xin, yin,
					   xout, yout);
}

sigc::signal<bool, GtkX::EventKey *> &
MarkedTextView::signal_key_press_event()
{
    return signal_key_press_event_;
}

sigc::signal<bool, GtkX::EventKey *> &
MarkedTextView::signal_key_release_event()
{
    return signal_key_release_event_;
}

sigc::signal<bool, GtkX::EventKey *> &
MarkedTextView::signal_key_press_pre_event()
{
    return signal_key_press_pre_event_;
}

sigc::signal<bool, GtkX::EventKey *> &
MarkedTextView::signal_key_release_pre_event()
{
    return signal_key_release_pre_event_;
}

bool
MarkedTextView::key_press_event_callback(GdkEventKey *ev)
{
    RefPtr<Event> evx = translate_event((GdkEvent *)ev);
    if (!evx) return false;
    GtkX::EventKey *evxk = dynamic_cast<GtkX::EventKey *>(&*evx);
    if (!evxk) return false;
    return signal_key_press_event_(evxk);
}

bool
MarkedTextView::key_release_event_callback(GdkEventKey *ev)
{
    RefPtr<Event> evx = translate_event((GdkEvent *)ev);
    if (!evx) return false;
    GtkX::EventKey *evxk = dynamic_cast<GtkX::EventKey *>(&*evx);
    if (!evxk) return false;
    return signal_key_release_event_(evxk);
}

bool
MarkedTextView::key_press_pre_event_callback(GdkEventKey *ev)
{
    RefPtr<Event> evx = translate_event((GdkEvent *)ev);
    if (!evx) return false;
    GtkX::EventKey *evxk = dynamic_cast<GtkX::EventKey *>(&*evx);
    if (!evxk) return false;
    return signal_key_press_pre_event_(evxk);
}

bool
MarkedTextView::key_release_pre_event_callback(GdkEventKey *ev)
{
    RefPtr<Event> evx = translate_event((GdkEvent *)ev);
    if (!evx) return false;
    GtkX::EventKey *evxk = dynamic_cast<GtkX::EventKey *>(&*evx);
    if (!evxk) return false;
    return signal_key_release_pre_event_(evxk);
}

void
ScrolledText::init_signals()
{
    tb_->signal_changed().connect(sigc::mem_fun(*this, &ScrolledText::changed_callback));
}

void
ScrolledText::postinit()
{
    Widget::postinit();
    init_signals();
}

ScrolledText::ScrolledText(GtkX::Container &parent, PackOptions po,
			   const GtkX::String &name, const GtkX::String &label)
{
    sw_ = new Gtk::ScrolledWindow();
    tv_ = new MarkedTextView();
    tb_ = tv_->get_buffer();
    // tv_->modify_bg(Gtk::STATE_NORMAL, Gdk::Color("green"));
    // tv_->modify_fg(Gtk::STATE_NORMAL, Gdk::Color("red"));
    // tv_->modify_text(Gtk::STATE_NORMAL, Gdk::Color("blue"));
    // tv_->modify_base(Gtk::STATE_NORMAL, Gdk::Color("red"));
    sw_->add(*tv_->internal());
    tv_->show();
    Glib::RefPtr<Gtk::TextTagTable> tt = tb_->get_tag_table();
    if (tt) {
	uneditable_tag_ = tb_->create_tag(Glib::ustring("uneditable-tag"));
	uneditable_tag_->property_editable() = false;
    }
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}

ScrolledText::~ScrolledText()
{
    delete tv_;
    delete sw_;
}

Gtk::Widget *
ScrolledText::internal()
{
    return sw_;
}

const Gtk::Widget *
ScrolledText::internal() const
{
    return sw_;
}

Gtk::Widget *
ScrolledText::signals_from()
{
    return tv_->internal();
}

void
ScrolledText::window_to_buffer_xy(int xin, int yin, int &xout, int &yout)
{
    tv_->window_to_buffer_xy(xin, yin, xout, yout);
}

void
ScrolledText::buffer_to_window_xy(int xin, int yin, int &xout, int &yout)
{
    tv_->buffer_to_window_xy(xin, yin, xout, yout);
}

long
ScrolledText::xy_to_pos(int x, int y)
{
    int buf_x, buf_y;
    window_to_buffer_xy(x, y, buf_x, buf_y);
    Gtk::TextIter iter;
    tv_->get_iter_at_location(iter, buf_x, buf_y);
    return iter.get_offset();
}

String
ScrolledText::get_text()
{
    return tb_->get_text();
}

String
ScrolledText::get_text(long o1, long o2)
{
    Gtk::TextIter iter1 = tb_->get_iter_at_offset(o1);
    Gtk::TextIter iter2 = tb_->get_iter_at_offset(o2);
    return tb_->get_text(iter1, iter2);
}

void
ScrolledText::set_editable(bool set)
{
    tv_->set_editable(set);
}

bool
ScrolledText::get_editable() const
{
    return tv_->get_editable();
}

void
ScrolledText::insert(long pos, const String &repl)
{
    Gtk::TextIter iter = tb_->get_iter_at_offset(pos);
    tb_->insert(iter, repl.s());
}

void
ScrolledText::replace(long pos1, long pos2, const String &repl)
{
    Gtk::TextIter iter1 = tb_->get_iter_at_offset(pos1);
    Gtk::TextIter iter2 = tb_->get_iter_at_offset(pos2);
    tb_->erase(iter1, iter2);
    iter1 = tb_->get_iter_at_offset(pos1);
    tb_->insert(iter1, repl.s());
}

void
ScrolledText::show_position(long pos)
{
    Gtk::TextIter iter = tb_->get_iter_at_offset(pos);
    tv_->scroll_to(iter);
}

long
ScrolledText::get_last_position()
{
    return tb_->end().get_offset();
}

void
ScrolledText::set_insertion_position(long pos)
{
    Gtk::TextIter iter;
    if (pos == LONG_MAX)
	iter = tb_->end();
    else
	iter = tb_->get_iter_at_offset(pos);
    tb_->place_cursor(iter);
}

void
ScrolledText::set_text(const String &repl)
{
    tb_->set_text(repl.s());
}

MarkedTextView &
ScrolledText::view()
{
    return *tv_;
}

Glib::RefPtr<Gtk::TextBuffer>
ScrolledText::buffer()
{
    return tb_;
}

void
ScrolledText::set_highlight(long pos1, long pos2, HighlightMode mode)
{
    static int errcnt = 0;
    if (!errcnt++) std::cerr << "set_highlight(" << pos1 << "," << pos2 << "," << mode << ")\n";
}

long
ScrolledText::find_backward(String str, long start)
{
    Gtk::TextIter iter;
    Glib::ustring search = str.s();
    if (start == -1)
	iter = tb_->end();
    else
	iter = tb_->get_iter_at_offset(start);
    while (iter.backward_char()) {
	gunichar c = *iter;
	if (search.find(c) != (size_t)-1)
	    return iter.get_offset();
    }
    return -1;
}

long
ScrolledText::find_backward(UChar c, long start)
{
    Gtk::TextIter iter;
    if (start == -1)
	iter = tb_->end();
    else
	iter = tb_->get_iter_at_offset(start);
    while (iter.backward_char()) {
	if (c == *iter)
	    return iter.get_offset();
    }
    return -1;
}

long
ScrolledText::find_forward(String str, long start)
{
    Gtk::TextIter iter;
    Glib::ustring search = str.s();
    iter = tb_->get_iter_at_offset(start);
    while (iter.forward_char()) {
	gunichar c = *iter;
	if (search.find(c) != (size_t)-1)
	    return iter.get_offset();
    }
    return -1;
}

long
ScrolledText::find_forward(UChar c, long start)
{
    Gtk::TextIter iter;
    iter = tb_->get_iter_at_offset(start);
    while (iter.forward_char()) {
	if (c == *iter)
	    return iter.get_offset();
    }
    return -1;
}

bool
ScrolledText::get_selection_bounds(long &begin, long &end)
{
    Gtk::TextIter begin_iter, end_iter;
    bool status = tb_->get_selection_bounds(begin_iter, end_iter);
    if (!status)
	return status;
    begin = begin_iter.get_offset();
    end = end_iter.get_offset();
    return status;
}

bool
ScrolledText::pos_to_xy(long pos, int &x, int &y)
{
    Gtk::TextIter iter;
    iter = tb_->get_iter_at_offset(pos);
    Gdk::Rectangle location;
    tv_->get_iter_location(iter, location);
    tv_->buffer_to_window_xy(location.get_x(), location.get_y(), x, y);
    std::cerr << "pos_to_xy: what to return?\n";
    return true;
}

void
ScrolledText::clear_selection()
{
    // Glib::RefPtr<Gtk::TextMark> sel = tb_->get_selection_bound();
    // tb_->delete_mark(sel);
    Glib::RefPtr<Gtk::TextMark> ins = tb_->get_insert();
    Gtk::TextIter iter = tb_->get_iter_at_mark(ins);
    tb_->select_range(iter, iter);
}

long
ScrolledText::get_insertion_position()
{
    Glib::RefPtr<Gtk::TextMark> ins = tb_->get_insert();
    Gtk::TextIter iter = tb_->get_iter_at_mark(ins);
    return iter.get_offset();
}

int ScrolledText::get_rows()
{
    static int errcnt = 0;
    if (!errcnt++) std::cerr << "ScrolledText: ROWS\n";
    return 0;
}

int
ScrolledText::get_columns()
{
    static int errcnt = 0;
    if (!errcnt++) std::cerr << "ScrolledText: COLUMNS\n";
    return 0;
}

void
ScrolledText::protect(long pos1, long pos2)
{
    Gtk::TextIter iter1, iter2;
    iter1 = tb_->get_iter_at_offset(pos1);
    iter2 = (pos2 == LONG_MAX) ? tb_->end() : tb_->get_iter_at_offset(pos2);
    tb_->apply_tag(uneditable_tag_, iter1, iter2);
}

void
ScrolledText::changed_callback()
{
    signal_changed_();
}

sigc::signal<void> &
ScrolledText::signal_changed()
{
    return signal_changed_;
}

sigc::signal<bool, GtkX::EventKey *> &
ScrolledText::signal_key_press_event()
{
    return tv_->signal_key_press_event();
}

sigc::signal<bool, GtkX::EventKey *> &
ScrolledText::signal_key_release_event()
{
    return tv_->signal_key_release_event();
}

sigc::signal<bool, GtkX::EventKey *> &
ScrolledText::signal_key_press_pre_event()
{
    return tv_->signal_key_press_pre_event();
}

sigc::signal<bool, GtkX::EventKey *> &
ScrolledText::signal_key_release_pre_event()
{
    return tv_->signal_key_release_pre_event();
}
