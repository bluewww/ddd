# $Id$
# Fix DDD HTML Table of Contents

# Add a separator between title and table
\!<P>!a\
<HR>\
<H1>Table of Contents</H1>

# Set up a header
\!<BODY!a\
<ADDRESS>\
<SMALL CLASS=header>\
<A HREF="http://www.gnu.org/software/ddd/">DDD Home Page</A>\
</SMALL>\
</ADDRESS>

# Set up the title
s!<H2>\([^ ]* Edition[^<]*\)</H2>!<SPAN CLASS=caption>\1</SPAN><BR>!
s!<H2>\(Last updated[^<]*\)</H2>!<SPAN CLASS=caption>\1</SPAN><BR>!
s!<ADDRESS>Andreas Zeller</ADDRESS>!<P><ADDRESS CLASS=subtitle><A HREF="http://www.fmi.uni-passau.de/st/staff/zeller/">Andreas Zeller</A></ADDRESS>!
