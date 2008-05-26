#ifndef SCROLLEDTEXT_H
#define SCROLLEDTEXT_H

#include <QImage>
#include <QTextEdit>

#include <QtX/Container.h>

namespace QtX {

    enum HighlightMode {
	HIGHLIGHT_NORMAL,
	HIGHLIGHT_SELECTED,
	HIGHLIGHT_SECONDARY_SELECTED,
	SEE_DETAIL
    };

    struct GlyphMark
    {
	QImage *glyph;
	int x;
	int y;
	GlyphMark(QImage *g0, int x0, int y0) {
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
      int get_x(void) const {return x_;}
      void set_x(int x0) {x_ = x0;}
      int get_y(void) const {return y_;}
      void set_y(int y0) {y_ = y0;}
      int get_width(void) const {return w_;}
      void set_width(int w0) {w_ = w0;}
      int get_height(void) const {return h_;}
      void set_height(int h0) {h_ = h0;}
    };

    // QTextEdit has its own scroll bars, so there is no need for
    // a separate class
#if 0
    class MarkedTextView: public QtX::Widget, public QTextEdit
    {
	// bool on_expose_event(GdkEventExpose *event);
	std::list<GlyphMark *> marks;
    public:
	MarkedTextView(void);
	QWidget *internal(void);
	const QWidget *internal(void) const;
	void pos_to_xy(long pos, int &x, int &y);
	void pos_to_rect(long pos, Rectangle &rect);
	GlyphMark *map_glyph(QImage *glyph, int x, int y);
	GlyphMark *map_glyph(QImage *glyph, long pos);
	// Unmark a particular glyph:
	void unmap_glyph(GlyphMark *mark);
	// Unmark all instances of a glyph:
	void unmap_glyph(QImage *glyph);
	void refresh_line(int y, int height);
	void buffer_to_window_coords(int xin, int yin, int &xout, int &yout);
    };
#endif

    class ScrolledText: public QtX::Widget, public QTextEdit
    {
	sigc::signal<void> signal_changed_;
	void changed_callback(void);
    public:
	ScrolledText(void);
	ScrolledText(QtX::Container &parent, PackOptions po=PACK_SHRINK,
		     const QtX::String &name="", const QtX::String &label="");
	QWidget *internal(void);
	const QWidget *internal(void) const;
	void init_signals(void);
	void postinit(void);
	QWidget *signals_from(void);
	bool get_editable(void) const;
	void set_editable(bool);
	long get_last_position(void);
	void set_insertion_position(long pos);
	long get_insertion_position(void);
	bool get_selection_bounds(long &begin, long &end);
	void clear_selection(void);
	void replace(long pos1, long pos2, const GUI::String &repl);
	void insert(long pos, const GUI::String &repl);
	void set_text(const GUI::String &repl);
	GUI::String get_text(void);
	GUI::String get_text(long, long);
	void show_position(long pos);
	// MarkedTextView &view(void);
	// Glib::RefPtr<Gtk::TextBuffer> buffer(void);
	void set_highlight(long pos1, long pos2, HighlightMode);
	long find_forward(GUI::String str, long start=0);
	long find_backward(GUI::String str, long start=-1);
	long find_forward(UChar c, long start=0);
	long find_backward(UChar c, long start=-1);
	long xy_to_pos(double x, double y);
	bool pos_to_xy(long pos, int &x, int &y);
	int get_rows();
	int get_columns();
	sigc::signal<void> &signal_changed();
    };

}

#endif
