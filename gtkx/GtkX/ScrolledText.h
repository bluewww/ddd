#ifndef SCROLLEDTEXT_H
#define SCROLLEDTEXT_H

#include <gtkmm/scrolledwindow.h>
#include <gtkmm/textview.h>
#include <gtkmm/textbuffer.h>

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

    class MarkedTextView: public Gtk::TextView
    {
	bool on_expose_event(GdkEventExpose *event);
	std::list<GlyphMark *> marks;
    public:
	void pos_to_xy(long pos, int &x, int &y);
	GlyphMark *map_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph, int x, int y);
	GlyphMark *map_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph, long pos);
	// Unmark a particular glyph:
	void unmap_glyph(GlyphMark *mark);
	// Unmark all instances of a glyph:
	void unmap_glyph(Glib::RefPtr<Gdk::Pixbuf> glyph);
	void refresh_line(int y, int height);
    };

    class ScrolledText: public Gtk::ScrolledWindow
    {
    private:
	MarkedTextView tv_;
	Glib::RefPtr<Gtk::TextBuffer> tb_;
    public:
	ScrolledText(void);
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
	MarkedTextView &view(void);
	Glib::RefPtr<Gtk::TextBuffer> buffer(void);
	void set_highlight(long pos1, long pos2, HighlightMode);
	long find_forward(Glib::ustring str, long start=0);
	long find_backward(Glib::ustring str, long start=-1);
	long find_forward(gunichar c, long start=0);
	long find_backward(gunichar c, long start=-1);
	long xy_to_pos(double x, double y);
	bool pos_to_xy(long pos, int &x, int &y);
	int get_rows();
	int get_columns();
    };

}

#endif
