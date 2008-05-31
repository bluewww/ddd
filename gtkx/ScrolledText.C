#include <iostream>

#include <GtkX/Container.h>
#include <GtkX/ScrolledText.h>

// #include "SourceView.h"

using namespace GtkX;

extern ScrolledText *gdb_w;

MarkedTextView::MarkedTextView(void)
{
    postinit();
}

Gtk::Widget *
MarkedTextView::internal(void)
{
    return this;
}

const Gtk::Widget *
MarkedTextView::internal(void) const
{
    return this;
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
	buffer_to_window_coords((*iter)->x, (*iter)->y,
				ix, iy);
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
	buffer_to_window_coords(0, y, x1, y1);
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
    std::cerr << "Mapped glyph " << glyph->gobj() << " at (" << x << "," << y
	      << ") = " << new_mark << "\n";
    marks.push_back(new_mark);
    refresh_line(y, glyph->get_height());
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
    std::cerr << "Mapped glyph " << glyph->gobj() << " at " << pos << "\n";
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
	    std::cerr << "Unmapped glyph " << mark->glyph->gobj()
		      << " at (" << x << "," << y
		      << ") = " << mark << "\n";
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
	    std::cerr << "Unmapped glyph " << glyph->gobj()
		      << " at (" << x << "," << y
		      << ") = " << *iter << "\n";
	    int height = (*iter)->glyph->get_height();
	    marks.erase(iter);
	    refresh_line(y, height);
	    goto redo;
	}
    }
}

void
MarkedTextView::buffer_to_window_coords(int xin, int yin, int &xout, int &yout)
{
    Gtk::TextView::buffer_to_window_coords(Gtk::TEXT_WINDOW_TEXT, xin, yin,
					   xout, yout);
}

ScrolledText::ScrolledText(void)
{
    tb_ = tv_.get_buffer();
    // tv_.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("green"));
    // tv_.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("red"));
    // tv_.modify_text(Gtk::STATE_NORMAL, Gdk::Color("blue"));
    // tv_.modify_base(Gtk::STATE_NORMAL, Gdk::Color("red"));
    add(tv_);
    tv_.show();
}

void
ScrolledText::init_signals(void)
{
    tb_->signal_changed().connect(sigc::mem_fun(*this, &ScrolledText::changed_callback));
}

void
ScrolledText::postinit(void)
{
    Widget::postinit();
    init_signals();
}

ScrolledText::ScrolledText(GtkX::Container &parent, PackOptions po,
			   const GtkX::String &name, const GtkX::String &label)
{
    tb_ = tv_.get_buffer();
    // tv_.modify_bg(Gtk::STATE_NORMAL, Gdk::Color("green"));
    // tv_.modify_fg(Gtk::STATE_NORMAL, Gdk::Color("red"));
    // tv_.modify_text(Gtk::STATE_NORMAL, Gdk::Color("blue"));
    // tv_.modify_base(Gtk::STATE_NORMAL, Gdk::Color("red"));
    add(tv_);
    tv_.show();
    set_name(name.s());
    parent.add_child(*this, po, 0);
    postinit();
}


Gtk::Widget *
ScrolledText::internal(void)
{
    return this;
}

const Gtk::Widget *
ScrolledText::internal(void) const
{
    return this;
}

Gtk::Widget *
ScrolledText::signals_from(void)
{
    return tv_.internal();
}

long
ScrolledText::xy_to_pos(double x, double y)
{
    int ix = (int)x;
    int iy = (int)y;
    int buf_x, buf_y;
    tv_.window_to_buffer_coords(Gtk::TEXT_WINDOW_TEXT, ix, iy, buf_x, buf_y);
    Gtk::TextIter iter;
    tv_.get_iter_at_location(iter, buf_x, buf_y);
    return iter.get_offset();
}

String
ScrolledText::get_text(void)
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
    tv_.set_editable(set);
}

bool
ScrolledText::get_editable(void) const
{
    return tv_.get_editable();
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
    tv_.scroll_to(iter);
}

long
ScrolledText::get_last_position(void)
{
    return tb_->end().get_offset();
}

void
ScrolledText::set_insertion_position(long pos)
{
    Gtk::TextIter iter = tb_->get_iter_at_offset(pos);
    tb_->place_cursor(iter);
}

void
ScrolledText::set_text(const String &repl)
{
    tb_->set_text(repl.s());
}

MarkedTextView &
ScrolledText::view(void)
{
    return tv_;
}

Glib::RefPtr<Gtk::TextBuffer>
ScrolledText::buffer(void)
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
    tv_.get_iter_location(iter, location);
    tv_.buffer_to_window_coords(location.get_x(), location.get_y(),
				x, y);
    std::cerr << "pos_to_xy: what to return?\n";
    return true;
}

void
ScrolledText::clear_selection(void)
{
    // Glib::RefPtr<Gtk::TextMark> sel = tb_->get_selection_bound();
    // tb_->delete_mark(sel);
    Glib::RefPtr<Gtk::TextMark> ins = tb_->get_insert();
    Gtk::TextIter iter = tb_->get_iter_at_mark(ins);
    tb_->select_range(iter, iter);
}

long
ScrolledText::get_insertion_position(void)
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
ScrolledText::changed_callback(void)
{
    signal_changed_();
}

sigc::signal<void> &
ScrolledText::signal_changed()
{
    return signal_changed_;
}

