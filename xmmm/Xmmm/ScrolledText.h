#ifndef SCROLLEDTEXT_H
#define SCROLLEDTEXT_H

#include <list>

#include <sigc++/signal.h>
#include <Xmmm/Container.h>

namespace Xmmm {

    enum HighlightMode {
	HIGHLIGHT_NORMAL,
	HIGHLIGHT_SELECTED,
	HIGHLIGHT_SECONDARY_SELECTED,
	SEE_DETAIL
    };

    struct GlyphMark
    {
	XImage *glyph;
	int x;
	int y;
	GlyphMark(XImage *g0, int x0, int y0) {
	    glyph = g0;
	    x = x0;
	    y = y0;
	}
    };

    class MarkedTextView: public Widget
    {
	bool on_expose_event(XEvent *event);
	std::list<GlyphMark *> marks;
    public:
	void pos_to_xy(long pos, int &x, int &y);
	GlyphMark *map_glyph(XImage *glyph, int x, int y);
	GlyphMark *map_glyph(XImage *glyph, long pos);
	// Unmark a particular glyph:
	void unmap_glyph(GlyphMark *mark);
	// Unmark all instances of a glyph:
	void unmap_glyph(XImage *glyph);
	void refresh_line(int y, int height);
    };

    class ScrolledText: public Widget
    {
    private:
	MarkedTextView tv_;
	// Glib::RefPtr<Gtk::TextBuffer> tb_;
    public:
	ScrolledText(void);
	sigc::signal<void> signal_changed(void); // From the TextBuffer
	bool get_editable(void) const;
	void set_editable(bool);
	long get_last_position(void) const;
	void set_insertion_position(long pos);
	long get_insertion_position(void) const;
	bool get_selection_bounds(long &begin, long &end) const;
	void clear_selection(void);
	void replace(long pos1, long pos2, const Xmmm::String &repl);
	void insert(long pos, const Xmmm::String &repl);
	void set_text(const Xmmm::String &repl);
	Xmmm::String get_text(void);
	Xmmm::String get_text(long, long);
	void show_position(long pos);
	MarkedTextView &view(void);
	// Glib::RefPtr<Gtk::TextBuffer> buffer(void);
	void set_highlight(long pos1, long pos2, HighlightMode);
	long find_forward(Xmmm::String str, long start=0);
	long find_backward(Xmmm::String str, long start=-1);
	long find_forward(char c, long start=0);
	long find_backward(char c, long start=-1);
	long xy_to_pos(double x, double y);
	bool pos_to_xy(long pos, int &x, int &y);
	int get_rows(void) const;
	int get_columns(void) const;
	bool is_mapped(void) const;
    };

}

#endif
