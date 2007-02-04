#include <GtkX/Widget.h>

using namespace GtkX;

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

Widget::Widget(void)
{
}

Widget::~Widget(void)
{
}

#if 0
Gtk::Widget *
Widget::gtk_widget(void)
{
    return NULL;
}
#endif

void
Widget::show(void)
{
    gtk_widget()->show();
}

void
Widget::hide(void)
{
    gtk_widget()->hide();
}

