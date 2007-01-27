#include <Xmmm/Box.h>

using namespace Xmmm;

// ***

VBox::VBox(::Widget parent, const char *name)
{
    rc_ = XmCreateRowColumn(parent, (char *)name, NULL, 0);
}

VBox::~VBox(void)
{
    XtDestroyWidget(rc_);
}

::Widget VBox::xt(void)
{
    return rc_;
}

// ***

HBox::HBox(::Widget parent, const char *name)
{
    Arg args[10];
    int arg;
    arg = 0;
    XtSetArg(args[arg], XmNorientation,  XmHORIZONTAL); arg++;
    rc_ = XmCreateRowColumn(parent, (char *)name, NULL, 0);
}

HBox::~HBox(void)
{
    XtDestroyWidget(rc_);
}

::Widget HBox::xt(void)
{
    return rc_;
}

// ***

