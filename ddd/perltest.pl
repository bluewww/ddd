#!/usr/local/bin/perl
# $Id$
# Perl test

# Copyright (C) 1998 Technische Universitaet Braunschweig, Germany.
# Written by Andreas Zeller <zeller@ips.cs.tu-bs.de>.
# 
# This file is part of DDD.
# 
# DDD is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# DDD is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with DDD -- see the file COPYING.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 
# DDD is the data display debugger.
# For details, see the DDD World-Wide-Web page, 
# `http://www.cs.tu-bs.de/softech/ddd/',
# or send a mail to the DDD developers <ddd@ips.cs.tu-bs.de>.

$foo = 42;

@foo = (1, 2, 3, 4);

$foo[0] = 99;
$foo[1] = 300_000;

%foo = ('red' => 1, 'green' => 2, 'blue' => 3);

$foo{'red'}   = 5;
$foo{'green'} = 7;

# References
$ref = \%foo;
$anotherref = $ref;
$refref = \$ref;
$refrefref = \$refref;

# Lists of lists - from perllol(1)
@LoL = (
	[ "fred", "barney" ],
	[ "george", "jane", "elroy" ],
	[ "homer", "marge", "bart" ],
       );
print $LoL[2][2], "\n";

$ref_to_LoL = [
	       [ "fred", "barney", "pebbles", "bambam", "dino", ],
	       [ "homer", "bart", "marge", "maggie", ],
	       [ "george", "jane", "alroy", "judy", ],
	      ];
print $ref_to_LoL->[2][2], "\n";


# Closures
sub newprint {
  my $x = shift;
  return sub { my $y = shift; print "$x, $y!\n"; };
}
$h = newprint("Howdy");
$g = newprint("Greetings");

&$h("world");
&$g("earthlings");


# Globs
$fh = *STDOUT;



sub print_me {
  print @_;
}

&print_me('one ', 'two ', 'three ', "\n");

1;
