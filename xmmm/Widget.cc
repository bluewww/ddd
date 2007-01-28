#include <Xmmm/Widget.h>

Xmmm::Widget::Widget(void)
{
}

Xmmm::Widget::~Widget(void)
{
}

::Widget Xmmm::Widget::xt(void)
{
    return NULL;
}

void
Xmmm::Widget::show(void)
{
    XtManageChild(xt());
}

void
Xmmm::Widget::hide(void)
{
    XtUnmanageChild(xt());
}

