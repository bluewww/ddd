# $Id$
#
# ad2c : Convert app-defaults file to C strings decls.
#
# George Ferguson, ferguson@cs.rcohester.edu, 12 Nov 1990.
# Updated 19 Mar 1991 - Made it self-contained.

: read
# remove comments
/^!/d
# remove blanks
/^$/d
# escape backslash
s/\\/\\\\/g
# except the line continuation ones
s/\\$//g
# escape quotes
s/"/\\"/g
s/'/\\'/g
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
# s/\\\\/\\/g
s/\\n/\\\\n/g
s/\\t/\\\\t/g
s/\\f/\\\\f/g
s/\\b/\\\\b/g
b test
