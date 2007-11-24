#include <GtkX/Widget.h>

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

