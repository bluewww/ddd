#include <iostream>

#include "GtkScrolledText.h"

bool
GtkMarkedTextView::on_expose_event(GdkEventExpose *event)
{
    Gtk::TextView::on_expose_event(event);
}


GtkScrolledText::GtkScrolledText(void)
{
    tb_ = tv_.get_buffer();
    // tv_.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("green"));
    // tv_.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("red"));
    // tv_.modify_text(Gtk::STATE_NORMAL, Gdk::Color("blue"));
    // tv_.modify_base(Gtk::STATE_NORMAL, Gdk::Color("red"));
    add(tv_);
    tv_.show();
}

long
GtkScrolledText::xy_to_pos(double x, double y)
{
    int ix = (int)x;
    int iy = (int)y;
    int buf_x, buf_y;
    tv_.window_to_buffer_coords(Gtk::TEXT_WINDOW_TEXT, ix, iy, buf_x, buf_y);
    Gtk::TextIter iter;
    tv_.get_iter_at_location(iter, buf_x, buf_y);
    return iter.get_offset();
}

Glib::ustring
GtkScrolledText::get_text(void)
{
    return tb_->get_text();
}

Glib::ustring
GtkScrolledText::get_text(long o1, long o2)
{
    Gtk::TextIter iter1 = tb_->get_iter_at_offset(o1);
    Gtk::TextIter iter2 = tb_->get_iter_at_offset(o2);
    return tb_->get_text(iter1, iter2);
}

void
GtkScrolledText::set_editable(bool set)
{
    tv_.set_editable(set);
}

bool
GtkScrolledText::get_editable(void) const
{
    return tv_.get_editable();
}

void
GtkScrolledText::insert(long pos, const Glib::ustring &repl)
{
    Gtk::TextIter iter = tb_->get_iter_at_offset(pos);
    tb_->insert(iter, repl);
}

void
GtkScrolledText::replace(long pos1, long pos2, const Glib::ustring &repl)
{
    Gtk::TextIter iter1 = tb_->get_iter_at_offset(pos1);
    Gtk::TextIter iter2 = tb_->get_iter_at_offset(pos2);
    tb_->erase(iter1, iter2);
    iter1 = tb_->get_iter_at_offset(pos1);
    tb_->insert(iter1, repl);
}

void
GtkScrolledText::show_position(long pos)
{
    Gtk::TextIter iter = tb_->get_iter_at_offset(pos);
    tv_.scroll_to(iter);
}

long
GtkScrolledText::get_last_position(void)
{
    return tb_->end().get_offset();
}

void
GtkScrolledText::set_insertion_position(long pos)
{
    Gtk::TextIter iter = tb_->get_iter_at_offset(pos);
    tb_->place_cursor(iter);
}

void
GtkScrolledText::set_text(const Glib::ustring &repl)
{
    tb_->set_text(repl);
}

Glib::SignalProxy0<void>
GtkScrolledText::signal_changed(void)
{
    return tb_->signal_changed();
}

GtkMarkedTextView &
GtkScrolledText::view(void)
{
    return tv_;
}

Glib::RefPtr<Gtk::TextBuffer>
GtkScrolledText::buffer(void)
{
    return tb_;
}

void
GtkScrolledText::set_highlight(long pos1, long pos2, XmHighlightMode mode)
{
    std::cerr << "set_highlight(" << pos1 << "," << pos2 << "," << mode << ")\n";
}

long
GtkScrolledText::find_backward(Glib::ustring str, long start)
{
    Gtk::TextIter iter;
    if (start == -1)
	iter = tb_->end();
    else
	iter = tb_->get_iter_at_offset(start);
    while (iter.backward_char()) {
	gunichar c = *iter;
	if (str.find(c) != (size_t)-1)
	    return iter.get_offset();
    }
    return -1;
}

long
GtkScrolledText::find_backward(gunichar c, long start)
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
GtkScrolledText::find_forward(Glib::ustring str, long start)
{
    Gtk::TextIter iter;
    iter = tb_->get_iter_at_offset(start);
    while (iter.forward_char()) {
	gunichar c = *iter;
	if (str.find(c) != (size_t)-1)
	    return iter.get_offset();
    }
    return -1;
}

long
GtkScrolledText::find_forward(gunichar c, long start)
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
GtkScrolledText::get_selection_bounds(long &begin, long &end)
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
GtkScrolledText::pos_to_xy(long pos, double &x, double &y)
{
    Gtk::TextIter iter;
    iter = tb_->get_iter_at_offset(pos);
    Gdk::Rectangle location;
    tv_.get_iter_location(iter, location);
    int ix, iy;
    tv_.buffer_to_window_coords(Gtk::TEXT_WINDOW_TEXT,
				location.get_x(), location.get_y(),
				ix, iy);
    x = ix;
    y = iy;
    std::cerr << "pos_to_xy: what to return?\n";
    return true;
}

void
GtkScrolledText::clear_selection(void)
{
    // Glib::RefPtr<Gtk::TextMark> sel = tb_->get_selection_bound();
    // tb_->delete_mark(sel);
    Glib::RefPtr<Gtk::TextMark> ins = tb_->get_insert();
    Gtk::TextIter iter = tb_->get_iter_at_mark(ins);
    tb_->select_range(iter, iter);
}

long
GtkScrolledText::get_insertion_position(void)
{
    Glib::RefPtr<Gtk::TextMark> ins = tb_->get_insert();
    Gtk::TextIter iter = tb_->get_iter_at_mark(ins);
    return iter.get_offset();
}

