#include <GtkX/Widget.h>
#include <gtkmm/container.h>

using namespace GtkX;

Glib::Quark GtkX::gtkx_super_quark("gtkx_super");

static sigc::signal<bool> *signal_idle_ptr;

String::String(const Glib::ustring &s0)
{
    s_ = s0;
}

String::String(const char *s0)
{
    s_ = Glib::ustring(s0);
}

String::~String(void)
{
}

Glib::ustring &
String::s(void)
{
    return s_;
}

const Glib::ustring &
String::s(void) const
{
    return s_;
}

String
String::operator+(const String &str) const
{
    return String(s()+str.s());
}

bool
String::operator==(const String &str) const
{
    return s() == str.s();
}

gunichar
String::operator[](int i) const
{
    return s()[i];
}

String::operator bool(void) const
{
    return (s().length() > 0);
}

const char *
String::c(void) const
{
  return s().c_str();
}

const char *
String::c_str(void) const
{
  return s().c_str();
}

Display::Display(Glib::RefPtr<Gdk::Display> d0)
{
    disp_ = d0;
}

Widget::Widget(void)
{
}

Widget::~Widget(void)
{
}

#if 0
Gtk::Widget *
Widget::internal(void)
{
    return NULL;
}
#endif

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

RefPtr<Display>
Widget::get_display(void)
{
    Glib::RefPtr<Gdk::Display> d0 = internal()->get_display();
    return RefPtr<Display>(new Display(d0));
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
    return internal()->is_visible();
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
    internal()->signal_button_press_event().connect(sigc::mem_fun(*this, &Widget::button_press_event_callback));
    internal()->signal_button_release_event().connect(sigc::mem_fun(*this, &Widget::button_release_event_callback));
    internal()->signal_map().connect(sigc::mem_fun(*this, &Widget::signal_map_callback));
    internal()->signal_unmap().connect(sigc::mem_fun(*this, &Widget::signal_unmap_callback));
}

void
Widget::postinit(void)
{
    init_signals();
    internal()->set_data(gtkx_super_quark, this);
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
    GtkX::EventButton evx;
    translate_event((GdkEvent *)ev, (GtkX::Event *)&evx);
    signal_button_press_event_(&evx);
}

bool
Widget::button_release_event_callback(GdkEventButton *ev)
{
    GtkX::EventButton evx;
    translate_event((GdkEvent *)ev, (GtkX::Event *)&evx);
    signal_button_release_event_(&evx);
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

static bool
idle_handler(void)
{
    if (signal_idle_ptr) {
	(*signal_idle_ptr)();
    }
}

sigc::signal<bool> &
GtkX::signal_idle()
{
    if (!signal_idle_ptr) {
	signal_idle_ptr = new sigc::signal<bool>;
	Glib::signal_idle().connect(sigc::ptr_fun(idle_handler));
    }
    return *signal_idle_ptr;
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


