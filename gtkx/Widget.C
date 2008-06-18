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

#include <map>
#include <iostream>

#include <GtkX/Widget.h>
#include <gtkmm/container.h>
#include <gdk/gdkgc.h>

using namespace GtkX;

Glib::Quark GtkX::gtkx_super_quark("gtkx_super");

const GtkX::String GtkX::mklabel(const GtkX::String &name,
				 const GtkX::String &label)
{
    if (label.s().length() > 0)
	return label;
    return name;
}

typedef std::map<GdkDisplay *, RefPtr<Display> > DisplayMap;
DisplayMap display_map;

Display::Display(Glib::RefPtr<Gdk::Display> d0)
{
    nrefs_ = 0;
    disp_ = d0;
}

RefPtr<Display>
Display::wrap(Glib::RefPtr<Gdk::Display> d0)
{
    if (!d0) return NULL;
    DisplayMap::iterator iter = display_map.find(d0->gobj());
    if (iter != display_map.end()) {
	return iter->second;
    }
    RefPtr<Display> d = new Display(d0);
    display_map.insert(std::pair<GdkDisplay *, RefPtr<Display> >(d0->gobj(), d));
    return d;
}

String
Display::get_name(void) const
{
    return String(disp_->get_name().c_str());
}

void
Display::flush(void)
{
    disp_->flush();
}

RefPtr<Display>
Display::open(const String& display_name)
{
    return Display::wrap(Gdk::Display::open(display_name.c_str()));
}

void
Display::close(void)
{
    disp_->close();
}

RefPtr<Screen>
Display::get_default_screen(void)
{
    return Screen::wrap(disp_->get_default_screen());
}

void
Display::pointer_ungrab(unsigned int time_)
{
    disp_->pointer_ungrab(time_);
}

void
Display::keyboard_ungrab(unsigned int time_)
{
    disp_->keyboard_ungrab(time_);
}

typedef std::map<GdkScreen *, RefPtr<Screen> > ScreenMap;
ScreenMap screen_map;

RefPtr<Screen>
Screen::wrap(Glib::RefPtr<Gdk::Screen> s0)
{
    if (!s0) return NULL;
    ScreenMap::iterator iter = screen_map.find(s0->gobj());
    if (iter != screen_map.end()) {
	return iter->second;
    }
    RefPtr<Screen> s = new Screen(s0);
    screen_map.insert(std::pair<GdkScreen *, RefPtr<Screen> >(s0->gobj(), s));
    return s;
}

Screen::Screen(Glib::RefPtr<Gdk::Screen> s0)
{
    nrefs_ = 0;
    screen_ = s0;
}

RefPtr<XWindow>
Screen::get_root_window(void)
{
    return XWindow::wrap(screen_->get_root_window());
}

String
Screen::make_display_name(void) const
{
    return screen_->make_display_name();
}

typedef std::map<GdkWindow *, RefPtr<XWindow> > XWindowMap;
XWindowMap xwindow_map;

RefPtr<XWindow>
XWindow::wrap(Glib::RefPtr<Gdk::Window> w0)
{
    if (!w0) return NULL;
    XWindowMap::iterator iter = xwindow_map.find(w0->gobj());
    if (iter != xwindow_map.end()) {
	return iter->second;
    }
    RefPtr<XWindow> s = new XWindow(w0);
    xwindow_map.insert(std::pair<GdkWindow *, RefPtr<XWindow> >(w0->gobj(), s));
    return s;
}

RefPtr<XWindow>
XWindow::wrap(GdkWindow *w0)
{
    XWindowMap::iterator iter = xwindow_map.find(w0);
    if (iter != xwindow_map.end()) {
	return iter->second;
    }
    // N.B. GdkWindow is the same as GdkDrawable.
    // Make sure we have an actual GdkWindowObject here.

    // Note: the second arg ensures the refcount is bumped here.
    // Glib::wrap does not do this by default because usually the
    // pointer is obtained by calling a GTK routine which does it.
    Glib::RefPtr<Gdk::Window> w1 = Glib::wrap((GdkWindowObject *)w0, true);
    RefPtr<XWindow> s = new XWindow(w1);
    xwindow_map.insert(std::pair<GdkWindow *, RefPtr<XWindow> >(w0, s));
    return s;
}

XWindow::XWindow(Glib::RefPtr<Gdk::Window> w0)
{
    win_ = w0;
}

Glib::RefPtr<Gdk::Drawable>
XWindow::internal(void)
{
    return win_;
}

Glib::RefPtr<const Gdk::Drawable>
XWindow::internal(void) const
{
    return win_;
}

void
XWindow::set_back_pixmap(const RefPtr<Pixmap> &pixmap, bool parent_relative)
{
    win_->set_back_pixmap(pixmap->internal_pixmap(), parent_relative);
}

void
XWindow::clear(void)
{
    win_->clear();
}

void
XWindow::clear_area(int x, int y, int w, int h)
{
    win_->clear_area(x, y, w, h);
}

GrabStatus
XWindow::pointer_grab(bool owner_events, EventMask event_mask,
		      RefPtr<XWindow>& confwin,
		      Cursor &cursor, unsigned int time_)
{
    Gdk::EventMask mask;
    translate_event_mask(event_mask, mask);
    Glib::RefPtr<Gdk::Window> gconfwin;
    if (confwin) gconfwin = confwin->win_;
    Gdk::GrabStatus gs = win_->pointer_grab(owner_events, mask,
					    gconfwin,
					    cursor, time_);
}

GrabStatus
XWindow::pointer_grab(bool owner_events, EventMask event_mask,
		      unsigned int time_)
{
    Gdk::EventMask mask;
    translate_event_mask(event_mask, mask);
    Gdk::GrabStatus gs = win_->pointer_grab(owner_events, mask, time_);
}

void
XWindow::set_title(const String &s)
{
    win_->set_title(s.c_str());
}


Widget::Widget(void)
{
}

Widget::~Widget(void)
{
}

// Default
Gtk::Widget *
Widget::signals_from(void)
{
    return internal();
}

Container *
Widget::get_parent(void)
{
    Gtk::Container *par = internal()->get_parent();
    // Composite widget?
    while (par && !par->get_data(gtkx_super_quark)) {
	par = par->get_parent();
    }
    if (!par) {
	return NULL;
    }
    return (GtkX::Container *)par->get_data(gtkx_super_quark);
}

Widget *
GtkX::get_wrapper(Gtk::Widget *w)
{
    return (GtkX::Widget *)w->get_data(gtkx_super_quark);
}

void
Widget::show(void)
{
    internal()->show();
}

void
Widget::hide(void)
{
    internal()->hide();
}

bool
Widget::is_visible(void)
{
    return internal()->is_visible();
}

bool
Widget::is_realized(void)
{
    return internal()->is_realized();
}

String
Widget::get_name(void)
{
    return internal()->get_name();
}

void
Widget::set_name(const String &s)
{
    internal()->set_name(s.s());
}

void
Widget::set_sensitive(bool b)
{
    internal()->set_sensitive(b);
}

void
Widget::init_signals(void)
{
    // Must be called from postinit, because internal() must be set.
    Gtk::Widget *from = signals_from();
    from->signal_button_press_event().connect(sigc::mem_fun(*this, &Widget::button_press_event_callback));
    from->signal_button_release_event().connect(sigc::mem_fun(*this, &Widget::button_release_event_callback));
    from->signal_button_press_event().connect(sigc::mem_fun(*this, &Widget::button_press_pre_event_callback), false);
    from->signal_button_release_event().connect(sigc::mem_fun(*this, &Widget::button_release_pre_event_callback), false);
    from->signal_map().connect(sigc::mem_fun(*this, &Widget::signal_map_callback));
    from->signal_unmap().connect(sigc::mem_fun(*this, &Widget::signal_unmap_callback));
}

void
Widget::postinit(void)
{
    init_signals();
    internal()->set_data(gtkx_super_quark, this);
}

Requisition
Widget::size_request(void) const
{
    Gtk::Requisition greq = internal()->size_request();
    Requisition req = {greq.width, greq.height};
    return req;
}

static Gtk::StateType
translate_state_type(const StateType &in)
{
    switch(in) {
    case STATE_NORMAL: return Gtk::STATE_NORMAL;
    case STATE_ACTIVE: return Gtk::STATE_ACTIVE;
    case STATE_PRELIGHT: return Gtk::STATE_PRELIGHT;
    case STATE_SELECTED: return Gtk::STATE_SELECTED;
    case STATE_INSENSITIVE: return Gtk::STATE_INSENSITIVE;
    default: std::cerr << "Unknown state type\n"; return Gtk::STATE_NORMAL;
    }
}

static void
translate_color(const Color &c, GdkColor &gc)
{
    gc.red = (unsigned short)(USHRT_MAX*c.r);
    gc.green = (unsigned short)(USHRT_MAX*c.g);
    gc.blue = (unsigned short)(USHRT_MAX*c.b);
}

static void
translate_color(const Color &c, Gdk::Color &gc)
{
    gc.set_red((unsigned short)(USHRT_MAX*c.r));
    gc.set_green((unsigned short)(USHRT_MAX*c.g));
    gc.set_blue((unsigned short)(USHRT_MAX*c.b));
}

static void
translate_color(const GdkColor &gc, Color &c)
{
    c.r = (double)gc.red/(double)USHRT_MAX;
    c.g = (double)gc.green/(double)USHRT_MAX;
    c.b = (double)gc.blue/(double)USHRT_MAX;
}

Color
Widget::get_bg(void) const
{
    return get_bg(STATE_NORMAL);
}

Color
Widget::get_fg(void) const
{
    return get_fg(STATE_NORMAL);
}

Color
Widget::get_bg(StateType state) const
{
    Gdk::Color gc = internal()->get_style()->get_bg(translate_state_type(state));
    return Color((double)gc.get_red()/(double)USHRT_MAX,
		 (double)gc.get_green()/(double)USHRT_MAX,
		 (double)gc.get_blue()/(double)USHRT_MAX);
}

Color
Widget::get_fg(StateType state) const
{
    Gdk::Color gc = internal()->get_style()->get_fg(translate_state_type(state));
    return Color((double)gc.get_red()/(double)USHRT_MAX,
		 (double)gc.get_green()/(double)USHRT_MAX,
		 (double)gc.get_blue()/(double)USHRT_MAX);
}

void
Widget::set_bg(const Color &c)
{
    set_bg(STATE_NORMAL, c);
}

void
Widget::set_fg(const Color &c)
{
    set_fg(STATE_NORMAL, c);
}

void
Widget::set_bg(StateType state, const Color &c)
{
    Gdk::Color gc;
    translate_color(c, gc);
    internal()->modify_bg(translate_state_type(state), gc);
}

void
Widget::set_fg(StateType state, const Color &c)
{
    Gdk::Color gc;
    translate_color(c, gc);
    internal()->modify_fg(translate_state_type(state), gc);
}


Main *
Widget::get_main(void) const
{
    return default_main_loop;
}

RefPtr<Display>
Widget::get_display(void)
{
    return Display::wrap(internal()->get_display());
}

RefPtr<Screen>
Widget::get_screen(void)
{
    return Screen::wrap(internal()->get_screen());
}

RefPtr<XWindow>
Widget::get_window(void)
{
    Glib::RefPtr<Gdk::Window> rp = internal()->get_window();
    fprintf(stderr, "WINDOW %lx\n", ((long *)&rp)[0]);
    return XWindow::wrap(internal()->get_window());
}

PropertyProxy<void *>
Widget::property_user_data(void)
{
    // N.B. This must be consistent with Gtk.
    return PropertyProxy<void *>(this, "user-data");
}

PropertyProxy_RO<void *>
Widget::property_user_data(void) const
{
    // N.B. This must be consistent with Gtk.
    return PropertyProxy_RO<void *>(this, "user-data");
}

void
Widget::add_destroy_notify_callback(void *data, void *(*f)(void *)) {
    internal()->add_destroy_notify_callback(data, f);
}

void
Widget::remove_destroy_notify_callback(void *data) {
    internal()->remove_destroy_notify_callback(data);
}

bool
Widget::translate_coordinates(GtkX::Widget &dest_w, int x1, int y1, int &x2, int &y2)
{
    return internal()->translate_coordinates(*dest_w.internal(), x1, y1, x2, y2);
}

sigc::signal<bool, GtkX::EventButton *> &
Widget::signal_button_press_event()
{
    return signal_button_press_event_;
}

sigc::signal<bool, GtkX::EventButton *> &
Widget::signal_button_release_event()
{
    return signal_button_release_event_;
}

sigc::signal<bool, GtkX::EventButton *> &
Widget::signal_button_press_pre_event()
{
    return signal_button_press_pre_event_;
}

sigc::signal<bool, GtkX::EventButton *> &
Widget::signal_button_release_pre_event()
{
    return signal_button_release_pre_event_;
}

sigc::signal<void> &
Widget::signal_map()
{
    return signal_map_;
}

sigc::signal<void> &
Widget::signal_unmap()
{
    return signal_unmap_;
}

bool
Widget::button_press_event_callback(GdkEventButton *ev)
{
    RefPtr<Event> evx = translate_event((GdkEvent *)ev);
    if (!evx) return false;
    GtkX::EventButton *evxb = dynamic_cast<GtkX::EventButton *>(&*evx);
    if (!evxb) return false;
    return signal_button_press_event_(evxb);
}

bool
Widget::button_release_event_callback(GdkEventButton *ev)
{
    RefPtr<Event> evx = translate_event((GdkEvent *)ev);
    if (!evx) return false;
    GtkX::EventButton *evxb = dynamic_cast<GtkX::EventButton *>(&*evx);
    if (!evxb) return false;
    return signal_button_release_event_(evxb);
}

bool
Widget::button_press_pre_event_callback(GdkEventButton *ev)
{
    RefPtr<Event> evx = translate_event((GdkEvent *)ev);
    if (!evx) return false;
    GtkX::EventButton *evxb = dynamic_cast<GtkX::EventButton *>(&*evx);
    if (!evxb) return false;
    return signal_button_press_pre_event_(evxb);
}

bool
Widget::button_release_pre_event_callback(GdkEventButton *ev)
{
    RefPtr<Event> evx = translate_event((GdkEvent *)ev);
    if (!evx) return false;
    GtkX::EventButton *evxb = dynamic_cast<GtkX::EventButton *>(&*evx);
    if (!evxb) return false;
    return signal_button_release_pre_event_(evxb);
}

void
Widget::signal_map_callback(void)
{
    signal_map_();
}

void
Widget::signal_unmap_callback(void)
{
    signal_unmap_();
}

// FIXME: Should be const, but I cannot get it to work.

RefPtr<GC>
Widget::get_black_gc()
{
    return GC::wrap(internal()->get_style()->get_black_gc());
}

RefPtr<GC>
Widget::get_white_gc()
{
    return GC::wrap(internal()->get_style()->get_white_gc());
}

void
Widget::set_event_mask(EventMask events)
{
    Gdk::EventMask g_events;
    translate_event_mask(events, g_events);
    internal()->set_events(g_events);
}

void
Widget::add_event_mask(EventMask events)
{
    Gdk::EventMask g_events;
    translate_event_mask(events, g_events);
    internal()->add_events(g_events);
}

SignalIdle signal_idle_;

SignalIdle::SignalIdle(void)
{
}

sigc::connection
SignalIdle::connect(const sigc::slot<bool>& slot, int priority)
{
    return Glib::signal_idle().connect(slot, priority);
}

SignalIdle &
GtkX::signal_idle()
{
    return signal_idle_;
}

static Gdk::Color make_color(const Color &rgb)
{
    Gdk::Color c;
    c.set_rgb_p(rgb.r, rgb.g, rgb.b);
    return c;
}

Cursor::Cursor(const RefPtr<Pixmap> &source, const RefPtr<Pixmap> &mask,
	       const Color &fg, const Color &bg,
	       int x, int y):
    Gdk::Cursor(source->gdk_pixmap(), mask->gdk_pixmap(),
		make_color(fg), make_color(bg), x, y)
{
}


typedef std::map<GdkPixmap *, RefPtr<Pixmap> > PixmapMap;
PixmapMap pixmap_map;

RefPtr<Pixmap>
Pixmap::wrap(Glib::RefPtr<Gdk::Pixmap> p0)
{
    if (!p0) return NULL;
    PixmapMap::iterator iter = pixmap_map.find(p0->gobj());
    if (iter != pixmap_map.end()) {
	return iter->second;
    }
    RefPtr<Pixmap> s = new Pixmap(p0);
    pixmap_map.insert(std::pair<GdkPixmap *, RefPtr<Pixmap> >(p0->gobj(), s));
    return s;
}

RefPtr<Pixmap>
Pixmap::create_bitmap_from_data(const char *bits, int width, int height)
{
    Glib::RefPtr<Gdk::Pixmap> bm = Gdk::Bitmap::create(bits, width, height);
    return new Pixmap(bm);
}

RefPtr<Pixmap>
Pixmap::create_from_data(RefPtr<Drawable> drawable,
			 const char *data, int width, int height,
			 int depth, const Color &fg, const Color &bg)
{
    Gdk::Color gfg, gbg;
    gfg.set_rgb((unsigned short)(USHRT_MAX*fg.r),
		(unsigned short)(USHRT_MAX*fg.g),
		(unsigned short)(USHRT_MAX*fg.b));
    gbg.set_rgb((unsigned short)(USHRT_MAX*bg.r),
		(unsigned short)(USHRT_MAX*bg.g),
		(unsigned short)(USHRT_MAX*bg.b));
    Glib::RefPtr<Gdk::Pixmap> pm = Gdk::Pixmap::create_from_data(drawable->internal(),
								 data, width, height, 24,
								 gfg, gbg);
    return new Pixmap(pm);
}

Drawable::Drawable(void)
{
    nrefs_ = 0;
}

Pixmap::Pixmap(Glib::RefPtr<Gdk::Pixmap> p0)
{
    pixmap_ = p0;
}

Glib::RefPtr<Gdk::Pixmap>
Pixmap::gdk_pixmap(void)
{
    return pixmap_;
}

Glib::RefPtr<Gdk::Drawable>
Pixmap::internal(void)
{
    return pixmap_;
}

Glib::RefPtr<const Gdk::Drawable>
Pixmap::internal(void) const
{
    return pixmap_;
}

Glib::RefPtr<Gdk::Pixmap>
Pixmap::internal_pixmap(void)
{
    return pixmap_;
}

int
Drawable::get_depth(void) const
{
    return internal()->get_depth();
}

void
Drawable::draw_line(RefPtr<GC> gc,
		    int x1, int y1, int x2, int y2)
{
    internal()->draw_line(gc->internal(), x1, y1, x2, y2);
}

void
Drawable::draw_rectangle(RefPtr<GC> gc, bool filled,
			 int x, int y, int w, int h)
{
    internal()->draw_rectangle(gc->internal(), filled, x, y, w, h);
}

void
Drawable::draw_arc(RefPtr<GC> gc, bool filled,
		   int x, int y, int w, int h,
		   int angle1, int angle2)
{
    internal()->draw_arc(gc->internal(), filled, x, y, w, h, angle1, angle2);
}

void
Drawable::draw_polygon(RefPtr<GC> gc, bool filled,
		       std::vector<Point> &points)
{
    std::vector<Gdk::Point> gpoints;
    std::vector<Point>::iterator i;
    for (i = points.begin(); i != points.end(); i++) {
	gpoints.push_back(Gdk::Point((*i).x, (*i).y));
    }
    internal()->draw_polygon(gc->internal(), filled, gpoints);
}

typedef std::map<GdkGC *, RefPtr<GC> > GCMap;
GCMap gc_map;

GC::GC(Glib::RefPtr<Gdk::GC> g0)
{
    nrefs_ = 0;
    gc_ = g0;
}

RefPtr<GC>
GC::wrap(Glib::RefPtr<Gdk::GC> g0)
{
    if (!g0) return NULL;
    GCMap::iterator iter = gc_map.find(g0->gobj());
    if (iter != gc_map.end()) {
	return iter->second;
    }
    RefPtr<GC> g = new GC(g0);
    gc_map.insert(std::pair<GdkGC *, RefPtr<GC> >(g0->gobj(), g));
    return g;
}

Glib::RefPtr<Gdk::GC>
GC::internal(void)
{
    return gc_;
}

Glib::RefPtr<const Gdk::GC>
GC::internal(void) const
{
    return gc_;
}

GC::GC(const RefPtr<Drawable> &d)
{
    gc_ = Gdk::GC::create(d->internal());
}

void
GC::set_foreground(const Color &color)
{
    GdkColor c;
    translate_color(color, c);
    gdk_gc_set_rgb_fg_color(gc_->gobj(), &c);
}

Color
GC::get_foreground(void) const
{
    GdkGCValues val;
    gdk_gc_get_values(gc_->gobj(), &val);
    Color c;
    translate_color(val.foreground, c);
    return c;
}

void
GC::set_background(const Color &color)
{
    GdkColor c;
    translate_color(color, c);
    gdk_gc_set_rgb_bg_color(gc_->gobj(), &c);
}

Color
GC::get_background(void) const
{
    GdkGCValues val;
    gdk_gc_get_values(gc_->gobj(), &val);
    Color c;
    translate_color(val.background, c);
    return c;
}

static void
translate_line_style(const GdkLineStyle &in, LineStyle &out)
{
    switch(in) {
    case GDK_LINE_SOLID: out = LINE_SOLID; break;
    case GDK_LINE_ON_OFF_DASH: out = LINE_ON_OFF_DASH; break;
    case GDK_LINE_DOUBLE_DASH: out = LINE_DOUBLE_DASH; break;
    default: std::cerr << "Unknown line style\n"; out = LINE_SOLID; break;
    }
}

static void
translate_line_style(const LineStyle &in, GdkLineStyle &out)
{
    switch(in) {
    case LINE_SOLID: out = GDK_LINE_SOLID; break;
    case LINE_ON_OFF_DASH: out = GDK_LINE_ON_OFF_DASH; break;
    case LINE_DOUBLE_DASH: out = GDK_LINE_DOUBLE_DASH; break;
    default: std::cerr << "Unknown line style\n"; out = GDK_LINE_SOLID; break;
    }
}

static void
translate_cap_style(const GdkCapStyle &in, CapStyle &out)
{
    switch(in) {
    case GDK_CAP_NOT_LAST: out = CAP_NOT_LAST; break;
    case GDK_CAP_BUTT: out = CAP_BUTT; break;
    case GDK_CAP_ROUND: out = CAP_ROUND; break;
    case GDK_CAP_PROJECTING: out = CAP_PROJECTING; break;
    default: std::cerr << "Unknown cap style\n"; out = CAP_NOT_LAST; break;
    }
}

static void
translate_cap_style(const CapStyle &in, GdkCapStyle &out)
{
    switch(in) {
    case CAP_NOT_LAST: out = GDK_CAP_NOT_LAST; break;
    case CAP_BUTT: out = GDK_CAP_BUTT; break;
    case CAP_ROUND: out = GDK_CAP_ROUND; break;
    case CAP_PROJECTING: out = GDK_CAP_PROJECTING; break;
    default: std::cerr << "Unknown cap style\n"; out = GDK_CAP_NOT_LAST; break;
    }
}

static void
translate_join_style(const GdkJoinStyle &in, JoinStyle &out)
{
    switch(in) {
    case GDK_JOIN_MITER: out = JOIN_MITER; break;
    case GDK_JOIN_ROUND: out = JOIN_ROUND; break;
    case GDK_JOIN_BEVEL: out = JOIN_BEVEL; break;
    default: std::cerr << "Unknown cap style\n"; out = JOIN_MITER; break;
    }
}

static void
translate_join_style(const JoinStyle &in, GdkJoinStyle &out)
{
    switch(in) {
    case JOIN_MITER: out = GDK_JOIN_MITER; break;
    case JOIN_ROUND: out = GDK_JOIN_ROUND; break;
    case JOIN_BEVEL: out = GDK_JOIN_BEVEL; break;
    default: std::cerr << "Unknown join style\n"; out = GDK_JOIN_MITER; break;
    }
}

static void
translate_fill(const GdkFill &in, Fill &out)
{
    switch(in) {
    case GDK_SOLID: out = SOLID; break;
    case GDK_TILED: out = TILED; break;
    case GDK_STIPPLED: out = STIPPLED; break;
    case GDK_OPAQUE_STIPPLED: out = OPAQUE_STIPPLED; break;
    default: std::cerr << "Unknown fill style\n"; out = SOLID; break;
    }
}

static void
translate_fill(const Fill &in, GdkFill &out)
{
    switch(in) {
    case SOLID: out = GDK_SOLID; break;
    case TILED: out = GDK_TILED; break;
    case STIPPLED: out = GDK_STIPPLED; break;
    case OPAQUE_STIPPLED: out = GDK_OPAQUE_STIPPLED; break;
    default: std::cerr << "Unknown fill style\n"; out = GDK_SOLID; break;
    }
}

void
translate_function(const GdkFunction &in, Function &out)
{
    switch(in) {
    case GDK_COPY: out = COPY; break;
    case GDK_INVERT: out = INVERT; break;
    case GDK_XOR: out = XOR; break;
    case GDK_CLEAR: out = CLEAR; break;
    case GDK_AND: out = AND; break;
    case GDK_AND_REVERSE: out = AND_REVERSE; break;
    case GDK_AND_INVERT: out = AND_INVERT; break;
    case GDK_NOOP: out = NOOP; break;
    case GDK_OR: out = OR; break;
    case GDK_EQUIV: out = EQUIV; break;
    case GDK_OR_REVERSE: out = OR_REVERSE; break;
    case GDK_COPY_INVERT: out = COPY_INVERT; break;
    case GDK_OR_INVERT: out = OR_INVERT; break;
    case GDK_NAND: out = NAND; break;
    case GDK_NOR: out = NOR; break;
    case GDK_SET: out = SET; break;
    default: std::cerr << "Unknown function\n"; out = COPY; break;
    }
}

void
translate_function(const Function &in, GdkFunction &out)
{
    switch(in) {
    case COPY: out = GDK_COPY; break;
    case INVERT: out = GDK_INVERT; break;
    case XOR: out = GDK_XOR; break;
    case CLEAR: out = GDK_CLEAR; break;
    case AND: out = GDK_AND; break;
    case AND_REVERSE: out = GDK_AND_REVERSE; break;
    case AND_INVERT: out = GDK_AND_INVERT; break;
    case NOOP: out = GDK_NOOP; break;
    case OR: out = GDK_OR; break;
    case EQUIV: out = GDK_EQUIV; break;
    case OR_REVERSE: out = GDK_OR_REVERSE; break;
    case COPY_INVERT: out = GDK_COPY_INVERT; break;
    case OR_INVERT: out = GDK_OR_INVERT; break;
    case NAND: out = GDK_NAND; break;
    case NOR: out = GDK_NOR; break;
    case SET: out = GDK_SET; break;
    default: std::cerr << "Unknown function\n"; out = GDK_COPY; break;
    }
}

void
translate_gc_mask(const GCValuesMask &in, GdkGCValuesMask &out)
{
    int res = 0;
    if (in&GC_FOREGROUND) {res |= GDK_GC_FOREGROUND;}
    if (in&GC_BACKGROUND) {res |= GDK_GC_BACKGROUND;}
    if (in&GC_LINE_WIDTH) {res |= GDK_GC_LINE_WIDTH;}
    if (in&GC_LINE_STYLE) {res |= GDK_GC_LINE_STYLE;}
    if (in&GC_CAP_STYLE) {res |= GDK_GC_CAP_STYLE;}
    if (in&GC_JOIN_STYLE) {res |= GDK_GC_JOIN_STYLE;}
    if (in&GC_FILL) {res |= GDK_GC_FILL;}
    if (in&GC_STIPPLE) {res |= GDK_GC_STIPPLE;}
    if (in&GC_FUNCTION) {res |= GDK_GC_FUNCTION;}
    out = GdkGCValuesMask(res);
}

void
GC::set_values(const GCValues &values, GCValuesMask mask)
{
    GdkGCValues gval;
    GdkGCValuesMask gmask;
    translate_gc_mask(mask, gmask);
    if (mask&GC_FOREGROUND)
	translate_color(values.foreground, gval.foreground);
    if (mask&GC_BACKGROUND)
	translate_color(values.background, gval.background);
    if (mask&GC_LINE_WIDTH)
	gval.line_width = values.line_width;
    if (mask&GC_LINE_STYLE)
	translate_line_style(values.line_style, gval.line_style);
    if (mask&GC_CAP_STYLE)
	translate_cap_style(values.cap_style, gval.cap_style);
    if (mask&GC_JOIN_STYLE)
	translate_join_style(values.join_style, gval.join_style);
    if (mask&GC_FILL)
	translate_fill(values.fill, gval.fill);
    if (mask&GC_FUNCTION)
	translate_function(values.function, gval.function);
    // FIXME: We cannot cache these results with wrap().
    // gval.stipple = new Pixmap(values.stipple);
    gdk_gc_set_values(gc_->gobj(), &gval, gmask);
}

void
GC::get_values(GCValues &values)
{
    GdkGCValues gval;
    gdk_gc_get_values(gc_->gobj(), &gval);
    translate_color(gval.foreground, values.foreground);
    translate_color(gval.background, values.background);
    values.line_width = gval.line_width;
    translate_line_style(gval.line_style, values.line_style);
    translate_cap_style(gval.cap_style, values.cap_style);
    translate_join_style(gval.join_style, values.join_style);
    translate_fill(gval.fill, values.fill);
    translate_function(gval.function, values.function);
    // FIXME: We cannot cache these results with wrap().
    // values.stipple = new Pixmap(gval.stipple);
}

void
GC::set_line_attributes(int line_width, LineStyle line_style, CapStyle cap_style, JoinStyle join_style)
{
    GCValues val;
    val.line_width = line_width;
    val.line_style = line_style;
    val.cap_style = cap_style;
    val.join_style = join_style;
    set_values(val, GCValuesMask(GC_LINE_WIDTH|GC_LINE_STYLE|GC_CAP_STYLE|GC_JOIN_STYLE));
}

void
GC::set_fill(Fill fill)
{
    GCValues val;
    val.fill = fill;
    set_values(val, GC_FILL);
}

void
GC::set_stipple(const RefPtr<Pixmap> &stipple)
{
    std::cerr << "GC::set_stipple not implemented\n";
    //GCValues val;
    //val.stipple = stipple;
    //set_values(val, GC_STIPPLE);
}

void
GC::set_function(Function function)
{
    GCValues val;
    val.function = function;
    set_values(val, GC_FUNCTION);
}

Color::Color(const String &s)
{
    Gdk::Color c(Glib::ustring(s.c_str()));
    r = (double)c.get_red()/(double)USHRT_MAX;
    g = (double)c.get_green()/(double)USHRT_MAX;
    b = (double)c.get_blue()/(double)USHRT_MAX;
}

FontDescription::FontDescription(void)
{
}

FontDescription::FontDescription(const Pango::FontDescription &src):
    Pango::FontDescription(src)
{
}

typedef std::map<PangoFont *, RefPtr<Font> > FontMap;
FontMap font_map;

Font::Font(Glib::RefPtr<Pango::Font> f0)
{
    nrefs_ = 0;
    font_ = f0;
}

RefPtr<Font>
Font::wrap(Glib::RefPtr<Pango::Font> f0)
{
    if (!f0) return NULL;
    FontMap::iterator iter = font_map.find(f0->gobj());
    if (iter != font_map.end()) {
	return iter->second;
    }
    RefPtr<Font> f = new Font(f0);
    font_map.insert(std::pair<PangoFont *, RefPtr<Font> >(f0->gobj(), f));
    return f;
}

Glib::RefPtr<Pango::Font>
Font::internal(void)
{
    return font_;
}

Glib::RefPtr<const Pango::Font>
Font::internal(void) const
{
    return font_;
}

FontDescription
Font::describe(void) const
{
    return FontDescription(font_->describe());
}

SignalTimeout signal_timeout_;

SignalTimeout::SignalTimeout(void)
{
}

sigc::connection
SignalTimeout::connect(const sigc::slot<bool>& slot, unsigned int interval)
{
    return Glib::signal_timeout().connect(slot, interval);
}

SignalTimeout &
GtkX::signal_timeout()
{
    return signal_timeout_;
}

