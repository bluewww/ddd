#include <GtkX/Widget.h>

GtkX::String::String(const Glib::ustring &s0)
{
    s_ = s0;
}

GtkX::String::String(const char *s0)
{
    s_ = Glib::ustring(s0);
}

GtkX::String::~String(void)
{
}

Glib::ustring &
GtkX::String::s(void)
{
    return s_;
}

const Glib::ustring &
GtkX::String::s(void) const
{
    return s_;
}

GtkX::Widget::Widget(void)
{
}

GtkX::Widget::~Widget(void)
{
}

#if 0
Gtk::Widget *
GtkX::Widget::gtk_widget(void)
{
    return NULL;
}
#endif
