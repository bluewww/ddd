#!/usr/local/bin/perl
# $Id$
# Perl test

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
