# $Id$
# Fix DDD HTML code

# Set up a style
\!</head!i\
<link rel=StyleSheet HREF="style.css" type="text/css" media=screen>\
<link rel=author href="mailto:zeller@gnu.org">

# Set up a suitable background
s!<body>!<body bgcolor="#ffffff">!

# Set up a header
\!^Node:!i\
<small class=header>
s!^Up:<a rel=up href="#(dir)">(dir)</a>!Up:<a rel=up href="http://www.gnu.org/software/ddd/">DDD home page</a>!
\!^Up:!a\
</small>\

# Add a logo before first header
\!<h1>Debugging with DDD</h1>!i\
<img alt="DDD - The Data Display Debugger" width=410 height=140 src="PICS/dddlogo.jpg">
