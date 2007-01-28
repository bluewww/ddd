#include <X11/Intrinsic.h>
#include <sigc++/sigc++.h>

#ifndef XMMM_WIDGET_H
#define XMMM_WIDGET_H

namespace Xmmm {

    class Widget {
    public:
	Widget(void);
	~Widget(void);
	virtual ::Widget xt(void); // TEMPORARY
	void show(void);
	void hide(void);
    };

}

#endif // XMMM_WIDGET_H
