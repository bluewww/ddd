#include <Xmmm/Box.h>
#include <Xmmm/Notebook.h>

using namespace Xmmm;

// ***

void
VBox::init(::Widget parent, const Xmmm::String &name)
{
    rc_ = XmCreateRowColumn(parent, (char *)name.c(), NULL, 0);
}

VBox::VBox(Xmmm::Widget &parent, const Xmmm::String &name)
{
    Xmmm::Notebook *nb = dynamic_cast<Xmmm::Notebook *>(&parent);
    if (nb) {
	rc_ = XmmmNotebookInsertPage(parent.internal(), (char *)name.c(), NULL, 0);
    }
    else {
	init(parent.xt_container(), name);
    }
}

VBox::VBox(::Widget parent, const Xmmm::String &name)
{
    init(parent, name);
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

void
HBox::init(::Widget parent, const Xmmm::String &name)
{
    Arg args[1];
    XtSetArg(args[0], XmNorientation,  XmHORIZONTAL);
    rc_ = XmCreateRowColumn(parent, (char *)name.c(), args, 1);
}

HBox::HBox(Xmmm::Widget &parent, const Xmmm::String &name)
{
    Xmmm::Notebook *nb = dynamic_cast<Xmmm::Notebook *>(&parent);
    if (nb) {
	Arg args[1];
	XtSetArg(args[0], XmNorientation,  XmHORIZONTAL);
	rc_ = XmmmNotebookInsertPage(parent.internal(), (char *)name.c(), args, 1);
    }
    else {
	init(parent.xt_container(), name);
    }
}

HBox::HBox(::Widget parent, const Xmmm::String &name)
{
    init(parent, name);
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

