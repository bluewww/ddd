# $Id$
# Fix DDD HTML code

# Set up a style
\!</HEAD!i\
<LINK REL=StyleSheet HREF="style.css" TYPE="text/css" MEDIA=screen>\
<LINK REV="made" HREF="mailto:ddd@gnu.org">


# Set up a suitable background
s!<BODY>!<BODY BACKGROUND="PICS/dddback@IMGEXT@" BGCOLOR="#ffffff">!

# Set up a header
\!^Go to the!i\
<ADDRESS>\
<SMALL CLASS=header>
\!Go to the!a\
</SMALL>\
</ADDRESS>
s!<P><HR><P>!!

# Add the WWW page to the `go to' line
s!^\(Go to the .*\)\.$!\1, <A HREF="http://www.gnu.org/software/ddd/">DDD home page</A>!

# Add a logo before first header
\!<H1>!i\
<HR>\
<IMG ALT="DDD - The Data Display Debugger" WIDTH=410 HEIGHT=140 SRC="PICS/dddlogo@IMGEXT@">\
<HR>

\!^This document was generated.*!i\
<HR><SMALL CLASS=footer>
\!</BODY>!i\
</SMALL>

# Now for the details.

# Set up umlauts and accents
s/@"a/\&auml;/g
s/@"o/\&ouml;/g
s/@"u/\&uuml;/g
s/@"A/\&Auml;/g
s/@"O/\&Ouml;/g
s/@"U/\&Uuml;/g
s/@ss/\&szlig;/g

# Trailing stuff
s/@ifnotinfo//
s/@ifnottex//
s/\\def\\.*//
