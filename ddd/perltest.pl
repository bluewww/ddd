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

sub print_me {
  print @_;
}

&print_me('one ', 'two ', 'three ', "\n");

1;
