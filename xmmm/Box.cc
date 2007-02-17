#include <Xmmm/Box.h>

using namespace Xmmm;

// ***

VBox::VBox(Xmmm::Widget parent, const Xmmm::String &name)
{
    rc_ = XmCreateRowColumn(parent.xt_container(), (char *)name.c(), NULL, 0);
}

VBox::VBox(::Widget parent, const Xmmm::String &name)
{
    rc_ = XmCreateRowColumn(parent, (char *)name.c(), NULL, 0);
}

VBox::~VBox(void)
{
    XtDestroyWidget(rc_);
}

::Widget VBox::internal(void)
{
    return rc_;
}

// ***

HBox::HBox(Xmmm::Widget parent, const Xmmm::String &name)
{
    Arg args[1];
    XtSetArg(args[0], XmNorientation,  XmHORIZONTAL);
    rc_ = XmCreateRowColumn(parent.xt_container(), (char *)name.c(), args, 1);
}

HBox::HBox(::Widget parent, const Xmmm::String &name)
{
    Arg args[1];
    XtSetArg(args[0], XmNorientation,  XmHORIZONTAL);
    rc_ = XmCreateRowColumn(parent, (char *)name.c(), args, 1);
}

HBox::~HBox(void)
{
    XtDestroyWidget(rc_);
}

::Widget HBox::internal(void)
{
    return rc_;
}

// ***

