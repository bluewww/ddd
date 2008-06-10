#ifndef GTK_WRAPPER_H
#define GTK_WRAPPER_H

#if defined(HAVE_CONFIG_H)
#include "config.h"
#endif

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#if 0
typedef bool Boolean;
typedef bool Bool;

#define False false
#define True true
#endif
#endif

#endif // GTK_WRAPPER_H
