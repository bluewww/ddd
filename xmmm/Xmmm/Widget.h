// -*- C++ -*-
#ifndef XMMM_WIDGET_H
#define XMMM_WIDGET_H


#include <X11/Intrinsic.h>

#include <string>
#include <vector>
#include <sigc++/sigc++.h>

#include <Xmmm/Events.h>

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
	const char *c_str(void) const;
	operator const char *(void) const {return c();}
	String operator+(const String &str) const;
	bool operator==(const String &str) const;
	bool operator!=(const String &str) const;
	operator bool(void) const;
    };

    template <class T>
    class RefPtr {
	T *p;
    public:
	RefPtr(void) {
	    p = NULL;
	}
	RefPtr(T *ptr) {
	    p = ptr;
	    // if (p) p->ref();
	}
	RefPtr(const RefPtr &rp) {
	    p = rp.p;
	    // if (p) p->ref();
	}
	RefPtr &operator=(const RefPtr &rp) {
	    // if (rp.p) rp.p->ref();
	    // if (p) p->unref();
	    p = rp.p;
	    return *this;
	}
	RefPtr &operator=(T *val) {
	    // if (val) val->ref();
	    // if (p) p->unref();
	    p = val;
	    return *this;
	}
	~RefPtr(void) {
	    // if (p) p->unref();
	}
	operator T *(void) const {
	    return p;
	}
	T &operator*(void) const {
	    return *p;
	}
	T *operator->() const {
	    return &(operator*());
	}
	void clear(void) {
	}
    };

    typedef ::Display Display;

    typedef ::XFontStruct Font;

    template<> void RefPtr<Font>::clear(void);

    class Widget: public sigc::trackable {
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
	void set_name(const String &name);
	static Widget *lookup(::Widget w);
	void *&property_user_data(void) {return user_data;}
	void set_sensitive(bool);
    };

    extern std::vector<Arg> NO_ARGS;

    // FIXME: Actions need to be translated to/from Xt.
    typedef sigc::slot<void, Xmmm::Widget*, Xmmm::Event*, char **, unsigned int*> ActionProc;

    struct ActionRec {
	String string;
	ActionProc proc;
    };
}

std::ostream &operator<<(std::ostream &f, const Xmmm::String &s);

extern "C" {
    Boolean
    WM_set_close_callback(Widget shell,
			  void (*callback)(Widget, XtPointer, XtPointer),
			  XtPointer client_data);
}

#endif // XMMM_WIDGET_H
