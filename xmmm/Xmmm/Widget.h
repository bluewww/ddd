#ifndef XMMM_WIDGET_H
#define XMMM_WIDGET_H


#include <X11/Intrinsic.h>

#include <string>
#include <vector>
#include <sigc++/sigc++.h>

namespace Xmmm {

    // This class is used to simplify constructors which can take a
    // C++ or old-fashioned C string.
    class String {
	std::string s_;
    public:
	String(const std::string &s0);
	String(const char *s0);
	~String(void);
	std::string &s(void);
	const std::string &s(void) const;
	const char *c(void) const;
	char *c(void);
	operator const char *(void) const {return c();}
	String operator+(const String &str) const;
	bool operator==(const String &str) const;
	operator bool(void) const;
    };

    class Widget {
	void *user_data;
    public:
	Widget(void);
	~Widget(void);
	void postinit(void);
	virtual ::Widget internal(void);
	virtual ::Widget xt_container(void);
	void show(void);
	void hide(void);
	String get_name(void);
	static Widget *lookup(::Widget w);
	void *&property_user_data(void) {return user_data;}
	void set_sensitive(bool);
    };

    extern std::vector<Arg> NO_ARGS;
}

extern "C" {
    Boolean
    WM_set_close_callback(Widget shell,
			  void (*callback)(Widget, XtPointer, XtPointer),
			  XtPointer client_data);
}

#endif // XMMM_WIDGET_H
