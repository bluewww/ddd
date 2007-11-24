#include <iostream>

#include <Xmmm/Box.h>
#include <Xmmm/Notebook.h>
#include <Xmmm/XmmmNotebook.h>

using namespace Xmmm;

// ***

void
VBox::init(::Widget parent, const Xmmm::String &name)
{
    rc_ = XmCreateRowColumn(parent, (char *)name.c(), NULL, 0);
    postinit();
}

VBox::VBox(Xmmm::Widget &parent, const Xmmm::String &name)
{
    init(parent.xt_container(), name);
    Xmmm::Notebook *nb = dynamic_cast<Xmmm::Notebook *>(&parent);
    if (nb) {
	std::cerr << "VBox constructor calls XmmmNotebookInsertPage\n";
	XmmmNotebookInsertChild(parent.xt_container(), (char *)name.c(), rc_);
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
    postinit();
}

HBox::HBox(Xmmm::Widget &parent, const Xmmm::String &name)
{
    init(parent.xt_container(), name);
    Xmmm::Notebook *nb = dynamic_cast<Xmmm::Notebook *>(&parent);
    if (nb) {
	std::cerr << "HBox constructor calls XmmmNotebookInsertPage\n";
	XmmmNotebookInsertChild(parent.xt_container(), (char *)name.c(), rc_);
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

