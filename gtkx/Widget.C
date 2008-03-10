#include <GtkX/Widget.h>
#include <gtkmm/container.h>

using namespace GtkX;

Glib::Quark GtkX::gtkx_super_quark("gtkx_super");

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
    return RefPtr<Display>(internal()->get_display());
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
Widget::postinit(void)
{
    internal()->set_data(gtkx_super_quark, this);
}

PropertyProxy<void *>
Widget::property_user_data(void)
{
    // N.B. This must be consistent with Gtk.
    return PropertyProxy<void *>(this, "user-data");
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

