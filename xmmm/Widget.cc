#include <Xm/Protocols.h>

#include <Xmmm/Widget.h>

Xmmm::String::String(const std::string &s0)
{
    s_ = s0;
}

Xmmm::String::String(const char *s0)
{
    s_ = std::string(s0);
}

Xmmm::String::~String(void)
{
}

std::string &
Xmmm::String::s(void)
{
    return s_;
}

const std::string &
Xmmm::String::s(void) const
{
    return s_;
}

const char *
Xmmm::String::c(void) const
{
    return s_.c_str();
}

Xmmm::String
Xmmm::String::operator+(const Xmmm::String &str) const
{
    return Xmmm::String(s()+str.s());
}

// ***************************************************************************

Boolean
WM_set_close_callback(Widget shell,
		      void (*callback)(Widget, XtPointer, XtPointer),
		      XtPointer client_data)
{
    extern Atom XmInternAtom(Display *, char *, Boolean) ;

    Display *display  = (Display *) 0 ;
    Atom     property = (Atom) 0 ;
    Atom     protocol = (Atom) 0 ;

    if (shell == (Widget) 0) {
	return False ;
    }

    if ((display = XtDisplay(shell)) == (Display *) 0) {
	return False ;
    }

    /* Retrieve Window Manager Protocol Property */

    if ((property = XmInternAtom(display,
				 "WM_PROTOCOLS",
				 False)) == (Atom) 0) {
	return False ;
    }

    /* Retrieve Window Manager Delete Window Property */

    if ((protocol = XmInternAtom(display,
				 "WM_DELETE_WINDOW",
				 True)) == (Atom) 0) {
	return False ;
    }

    /* Ensure that Shell has the Delete Window Property */
    /* NB: Necessary since some Window managers are not */
    /* Fully XWM Compilant (olwm for instance is not)   */

    XmAddProtocols(shell, property, &protocol, 1) ;

    /* Now add our callback into the Protocol Callback List */

    XmAddProtocolCallback(shell,
			  property,
			  protocol,
			  callback,
			  client_data) ;

    return True ;
}


// ***************************************************************************


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

::Widget Xmmm::Widget::xt_container(void)
{
    return xt();
}

void
Xmmm::Widget::show(void)
{
    ::Widget w = xt();
    if (XtIsShell(w)) {
	XtPopup(w, XtGrabNone);
    }
    else {
	XtManageChild(xt());
    }
}

void
Xmmm::Widget::hide(void)
{
    XtUnmanageChild(xt());
}

