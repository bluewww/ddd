# $Id$
# Fix DDD HTML code

# Set up a style
\!</HEAD!i\
<LINK REL=StyleSheet HREF="style.css" TYPE="text/css" MEDIA=screen>\
<LINK REV="made" HREF="mailto:ddd@ips.cs.tu-bs.de">


# Set up a suitable background
s!<BODY>!<BODY BACKGROUND="PICS/dddback.gif" BGCOLOR="#f5f5f5">!

# Set up a header
\!<p>Go to!i\
<ADDRESS>\
<SMALL CLASS=header>
\!<p>Go to!a\
</SMALL>\
</ADDRESS>
s!<P><HR><P>!!

# Add the WWW page to the `go to' line
s!^<p>\(Go to .*\)\.$!\1, <A HREF="http://www.cs.tu-bs.de/softech/ddd/">DDD home page</A>!

# Add a logo before first header
\!<H1>!i\
<HR>\
<IMG ALT="DDD - The Data Display Debugger" WIDTH=410 HEIGHT=140 SRC="PICS/dddlogo.gif">\
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
