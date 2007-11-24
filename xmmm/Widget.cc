#include <iostream>

#include <ext/hash_map>

#include <Xm/Protocols.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>

#include <Xmmm/Widget.h>

using namespace Xmmm;

std::vector<Arg> Xmmm::NO_ARGS;


Xmmm::String::String(const std::string &s0)
{
    s_ = s0;
}

Xmmm::String::String(const char *s0)
{
    if (s0)
	s_ = std::string(s0);
    else
	s_ = std::string("");

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
    return String(s()+str.s());
}

bool
Xmmm::String::operator==(const Xmmm::String &str) const
{
    return (s() == str.s());
}

Xmmm::String::operator bool(void) const
{
    return (s_.length() > 0);
}

// ***************************************************************************

Boolean
WM_set_close_callback(::Widget shell,
		      void (*callback)(::Widget, XtPointer, XtPointer),
		      XtPointer client_data)
{
    Display *display  = (Display *) 0 ;
    Atom     property = (Atom) 0 ;
    Atom     protocol = (Atom) 0 ;

    if (shell == (::Widget) 0) {
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

::Widget Xmmm::Widget::internal(void)
{
    return NULL;
}

::Widget Xmmm::Widget::xt_container(void)
{
    return internal();
}

void
Xmmm::Widget::show(void)
{
    ::Widget w = internal();
    if (XtIsShell(w)) {
	ShellWidget sw = (ShellWidget)w;
	XtWidgetProc resize;
	XtPopup(w, XtGrabNone);
	// FIXME: Fudge
#if 0
	if (sw->core.width < 2 && sw->core.height < 2
	    && sw->composite.num_children > 0) {
	    XtWidgetGeometry preferred;
	    XtWidgetGeometry geom;
	    int x, y, w, h, bw;
	    // Get current preferred values for shell and override
	    // width and height with values for child.
	    XtQueryGeometry((::Widget)sw, NULL, &geom);
	    x = (geom.request_mode&CWX)?geom.x:sw->core.x;
	    y = (geom.request_mode&CWY)?geom.y:sw->core.y;
	    w = (geom.request_mode&CWWidth)?geom.width:sw->core.width;
	    h = (geom.request_mode&CWHeight)?geom.height:sw->core.height;
	    bw = (geom.request_mode&CWBorderWidth)?geom.border_width:sw->core.border_width;
	    XtQueryGeometry(sw->composite.children[0], NULL, &preferred);
	    std::cerr << "Popup shell w=" << preferred.width << " h=" << preferred.height
		      << "\n";
	    if (preferred.request_mode & CWHeight)
		h = preferred.height;
	    if (preferred.request_mode & CWWidth)
		w = preferred.width;
	    XtConfigureWidget((::Widget)sw, x, y, w, h, bw);
	    resize = XtClass(sw)->core_class.resize;
	    if (resize)
		(*resize)((::Widget)sw);
	}
#endif
    }
    else {
	XtManageChild(w);
    }
}

void
Xmmm::Widget::hide(void)
{
    ::Widget w = internal();
    if (XtIsShell(w)) {
	XtPopdown(w);
    }
    else {
	XtUnmanageChild(internal());
    }
}

Xmmm::String
Xmmm::Widget::get_name(void)
{
    return String(XtName(internal()));
}

struct Widget_Hash {
    size_t operator()( ::Widget const &w) const {
	return (size_t)w;
    }
};

typedef __gnu_cxx::hash_map< ::Widget, Xmmm::Widget *, Widget_Hash> WrapperMap;
WrapperMap wrapper_map;

void
Xmmm::Widget::postinit(void)
{
    ::Widget w = internal();
    wrapper_map.insert(std::pair< ::Widget, Xmmm::Widget *>(w, this));
}

Xmmm::Widget *
Xmmm::Widget::lookup(::Widget w)
{
    WrapperMap::iterator iter = wrapper_map.find(w);
    if (iter != wrapper_map.end()) {
	return (*iter).second;
    }
    return NULL;
}

void
Xmmm::Widget::set_sensitive(bool state)
{
    XtSetSensitive(internal(), state?True:False);

    // Move flattening code from MakeMenu.C
#if 0
    if (!state && w == active_button)
    {
	// We won't get the LeaveWindow event, since W is now
	// insensitive.  Flatten button explicitly.
	flatten_button(w);
    }
#endif
}
