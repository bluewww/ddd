# $Id$
#
# ad2c : Convert app-defaults file to C strings decls.
#
# George Ferguson, ferguson@cs.rcohester.edu, 12 Nov 1990.
# Updated 19 Mar 1991 - Made it self-contained.

: read
# remove comments
/^!/d
# remove blank lines
/^$/d
# compress blanks after `:'
s/:[	 ][	 ]*/: /g
# remove spaces at end of line
s/[	 ][	 ]*$/ /
s/[	 ][	 ]*\\n/\\n/g
s/set prompt \(.*\)\\n/set prompt \1 \\n/
# escape backslash
s/\\/\\\\/g
# except the line continuation ones
s/\\$//g
# escape quotes
s/"/\\"/g
s/'/\\'/g
# escape tabs
s/	/\\t/g
# add leading quote
s/^/"/
#
: test
/\\$/b slash
s/$/",/
p
n
b read
#
: slash
p
n
# just like "read" only does not add leading quote
/^!/d
/^$/d
s/:[	 ][	 ]*/: /g
s/[	 ][	 ]*$/ /
s/[	 ][	 ]*\\n$/ /
s/[	 ][	 ]*\\n/\\n/g
s/set prompt \(.*\)\\n/set prompt \1 \\n/
s/\\/\\\\/g
s/\\$//g
s/"/\\"/g
s/'/\\'/g
s/	/\\t/g
b test
