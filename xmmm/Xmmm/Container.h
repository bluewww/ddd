#ifndef XMMM_CONTAINER_H
#define XMMM_CONTAINER_H

#include <X11/Intrinsic.h>

#include <Xmmm/Widget.h>

namespace Xmmm {

    class Container: public Widget {
    public:
	Container(void);
	~Container(void);
    };

}

#endif // XMMM_CONTAINER_H
