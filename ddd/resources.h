// $Id$ -*- C++ -*-
// DDD application resources

// Copyright (C) 1996 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@gnu.org>.
// Cross-platform interface by Peter Wainwright <prw@ceiriog.eclipse.co.uk>
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 3 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, see <http://www.gnu.org/licenses/>.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.gnu.org/software/ddd/',
// or send a mail to the DDD developers <ddd@gnu.org>.

#ifndef _DDD_resources_h
#define _DDD_resources_h

#if defined(IF_XM)
#include <X11/Intrinsic.h>
#else
#include <libxml/tree.h>
#include "gtk_wrapper.h"
#endif

// DDD resource definitions
#if defined(IF_XM)
extern XtResource ddd_resources[];
#else
struct DDDResource;
extern DDDResource ddd_resources[];
// For String type
#include <GUI/Widget.h>
#endif
extern const int ddd_resources_size;

#if defined(IF_XM)
// Resources to use if `Ddd' app-defaults are not found
extern const _XtString const ddd_fallback_resources[];
#else
extern const char *const ddd_fallback_resources[];
#endif

#if defined(IF_XM)
// Default resources
extern XrmDatabase app_defaults(Display *display);
#else

// Emulate Xt resource database.
struct DDDResource {
    const char 	*resource_name;	/* Resource name			    */
    const char	*resource_class;	/* Resource class			    */
    const char	*resource_type;	/* Representation type desired		    */
    unsigned int	resource_size;	/* Size in bytes of representation	    */
    unsigned int	resource_offset;/* Offset from base to put resource value   */
    const char	*default_type;	/* representation type of specified default */
    void	*default_addr;	/* Address of default resource		    */
};

#ifdef NAG_ME
#warning we need to define a generic value type
#endif
struct DDDValueBase {
    int foo;
public:
    GUI::String get(void) const;
    void set(const GUI::String &);
};

extern xmlDoc *get_string_database(const char *s);
extern void merge_databases(xmlDoc *source_db, xmlDoc *target_db);
extern bool get_resource(xmlDoc *database, const char *str_name, const char *str_class,
			 DDDValueBase &value_return);
extern bool put_resource(xmlDoc *database, const char *str_name, const char *str_class,
			 DDDValueBase &value);
extern xmlDoc *get_file_database(const char *f);

extern void get_application_resources(xmlDoc *db,
				      void *app_data,
				      DDDResource *resources,
				      int resources_size);

#endif

#endif // _DDD_resources_h
// DON'T ADD ANYTHING BEHIND THIS #endif
