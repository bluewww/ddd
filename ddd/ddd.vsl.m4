// $Id$ -*- c++ -*-
// VSL functions for DDD graph display
// Generated automatically from ddd.vsl.m4 and colors.m4 by M4.
include(ifdef(`srcdir',srcdir()/colors.m4,colors.m4))dnl

// Copyright (C) 1995-1998 Technische Universitaet Braunschweig, Germany.
// Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
// 
// This file is part of DDD.
// 
// DDD is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
// 
// DDD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public
// License along with DDD -- see the file COPYING.
// If not, write to the Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// 
// DDD is the data display debugger.
// For details, see the DDD World-Wide-Web page, 
// `http://www.cs.tu-bs.de/softech/ddd/',
// or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

// Includes
#include <std.vsl>
#include <tab.vsl>
#include <fonts.vsl>
#include <colors.vsl>

// Font settings
#pragma replace stdfontfamily
stdfontfamily() = family_typewriter();
small(...) = rm(...);

// Shadow effects
shadow(box, thickness) =
  box & (square(thickness) | vrule(thickness))
| square(thickness) & hrule(thickness);
shadow(box) = shadow(box, 1);

// Colors
display_color(box)   = color(box, "FOREGROUND_COLOR", "DISPLAY_COLOR");
title_color(box)     = color(box, "FOREGROUND_COLOR");
disabled_color(box)  = color(box, "DISPLAY_COLOR", "DISABLED_COLOR");
simple_color(box)    = color(box, "FOREGROUND_COLOR");
text_color(box)      = color(box, "FOREGROUND_COLOR");
pointer_color(box)   = color(box, "DATA_COLOR");
struct_color(box)    = color(box, "FOREGROUND_COLOR");
list_color(box)      = color(box, "FOREGROUND_COLOR");
array_color(box)     = color(box, "DATA_COLOR");
reference_color(box) = color(box, "DATA_COLOR");
changed_color(box)   = color(box, "FOREGROUND_COLOR", "CHANGED_COLOR");

// Non-expanding alignments
fixed_hlist(_) = hnull();
fixed_hlist(_, head) = hfix(head);
fixed_hlist(sep, head, ...) = hfix(head) & sep & fixed_hlist(sep, ...);

fixed_vlist(_) = vnull();
fixed_vlist(_, head) = vfix(head);
fixed_vlist(sep, head, ...) = vfix(head) | sep | fixed_vlist(sep, ...);

// The title
title (disp_nr, name) -> 
  title_color(rm(disp_nr & ": ") & bf(name) & hfill());
title (name) -> 
  title_color(bf(name) & hfill());

// The annotation
annotation (name) ->
  small(name);

// The "disabled" string
disabled () -> 
  disabled_color(vcenter(it("(Disabled)") & hfill()));

// Ordinary values
simple_value (value) -> 
  simple_color(vcenter(value & hfill()));
numeric_value (value) -> 
  simple_color(vcenter(hfill() & value));

// Collapsed ordinary values
collapsed_simple_value () -> 
  simple_color(vcenter(rm("...") & hfill()));

// Info texts: single lines
text_line (line) -> 
  text_color(rm(line) & hfill());

// Multiple lines
text_value (...) -> valign(...);

// Collapsed ordinary values
collapsed_text_value () -> 
  text_color(rm("...") & hfill());

// Ordinary pointers
pointer_value (value) -> 
  pointer_color(vcenter(value & hfill()));

// Collapsed pointers
collapsed_pointer_value () -> 
  pointer_color(vcenter(rm("...") & hfill()));

// Dereferenced pointers
dereferenced_pointer_value (value) -> 
  pointer_color(vcenter(bf(value)) & hfill());

// Collapsed array
collapsed_array () -> 
  array_color(vcenter(rm("\133...\135") & hfill()));

// Empty array
empty_array () -> 
  array_color(vcenter(rm("\133\135") & hfill()));

// Vertical array
vertical_array (...) -> 
  array_color(frame(indent(vlist(hwhite() | hrule() | hwhite(), ...))));

// Horizontal array
horizontal_array (...) -> 
  array_color(frame(indent(fixed_hlist(vwhite() & vrule() & vwhite(), ...))) 
	      & hfill());

// Two-dimensional arrays
twodim_array (...) -> 
  array_color(dtab(...));
twodim_array_elem (...) -> 
  rm(...);

// Struct value
struct_value (...) -> 
  struct_color(frame(indent(valign(...))));

// Collapsed struct
collapsed_struct_value () -> 
  struct_color(vcenter(rm("{...}") & hfill()));

// Empty struct
empty_struct_value () ->
  struct_color(vcenter(rm("{}") & hfill()));

// Struct member name
struct_member_name (name) -> 
  struct_color(rm(name));

// Struct member
struct_member (name, value, name_width) -> 
  vcenter(rm(name) | hspace(name_width)) 
  & vcenter(rm(" = ")) & rm(value);

// List value
list_value (...) -> 
  list_color(valign(...));

// Collapsed list
collapsed_list_value () -> 
  list_color(vcenter(rm("...") & hfill()));

// Empty list
empty_list_value () -> 
  list_color(vcenter(rm("") & hfill()));

// List member name
list_member_name (name) -> 
  list_color(rm(name));

// List member
list_member (name, value, name_width) -> 
  vcenter(rm(name) | hspace(name_width)) 
  & vcenter(rm(" = ")) & rm(value) & hfill();

// Sequence
sequence_value (...) -> 
  simple_color(fixed_hlist(vwhite(), ...) & hfill());

// Collapsed sequence
collapsed_sequence_value () -> 
  collapsed_simple_value();

// Reference
reference_value (ref, value) -> 
  reference_color(vcenter(rm(ref & ": ")) & value & hfill());

// Collapsed reference
collapsed_reference_value () -> 
  reference_color(vcenter(rm("...") & hfill()));

// Changed value
changed_value (value) -> 
  changed_color(it(value));

// A value that is repeated N times
repeated_value (value, n) ->
  value & vcenter(rm(" <" & dec(n) & "\327>"));

// The entire box
display_box (title, value) -> 
  shadow(display_color(frame(title | hrule() | hwhite () | rm(value))));
