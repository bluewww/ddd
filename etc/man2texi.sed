# $Id$
# Convert the DDD man page to Texinfo format.

# At characters
s/@/@@/g

# Macros
s/\\s-1@@ddd@@\\s+1/@value{ddd}/g
s/\\s-1@@Ddd@@\\s+1/@value{Ddd}/g
s/\\s-1@@DDD@@\\s+1/@value{DDD}/g
s/\\s-1DDD\\s+1/@value{DDD}/g
s/@@ddd@@/@value{ddd}/g
s/@@Ddd@@/@value{Ddd}/g
s/@@DDD@@/@value{DDD}/g

# Abbrevs
s/\\s-1BSD\\s+1/BSD/g
s/\\s-1DBX\\s+1/DBX/g
s/\\s-1EOF\\s+1/EOF/g
s/\\s-1FIG2DEV\\s+1/FIG2DEV/g
s/\\s-1FIG\\s+1/FIG/g
s/\\s-1GDB\\s+1/GDB/g
s/\\s-1GNU\\s+1/GNU/g
s/\\s-1IBMGL\\s+1/IBMGL/g
s/\\s-1JDB\\s+1/JDB/g
s/\\s-1L\\s-1A\\s+1T\\dE\\uX\\s+1/@TeX{}/g
s/\\s-1PIC\\s+1/PIC/g
s/\\s-1QUIT\\s+1/QUIT/g
s/\\s-1TRANSFIG\\s+1/TRANSFIG/g
s/\\s-1TTY\\s+1/TTY/g
s/\\s-1UNIX\\s+1/UNIX/g
s/\\s-1WWW\\s+1/WWW/g
s/\\s-1XDB\\s+1/XDB/g
s/\\s-1XFIG\\s+1/XFIG/g
s/\\s-1XPM\\s+1/XPM/g

# Comments
s/^\.\\"[ -]*$//
s/^\.\\"/@c/

# Keys
s/`\\s-1\\fBTAB\\fR\\s+1'/@key{TAB}/g
s/`\\s-1\\fBESC\\fR\\s+1'/@key{ESC}/g
s/`\\fB\(Ctrl[^\\]*\)\\fR'/@key{\1}/g
s/`\\fB\(Alt[^\\]*\)\\fR'/@key{\1}/g
s/`\\fBRETURN\\fR'/@key{RET}/g

s/\\fBTAB\\fR/@key{TAB}/g
s/\\fBESC\\fR/@key{ESC}/g
s/\\fB\(Ctrl[^\\]*\)\\fR/@key{\1}/g
s/\\fB\(Alt[^\\]*\)\\fR/@key{\1}/g
s/\\fBRETURN\\fR/@key{RET}/g

# Titles
s/^.SH *"\([^"]*\)"/@chapter \1/
s/^.SH/@chapter/
s/^.SS *"\([^"]*\)"/@section \1/
s/^.SS/@section/

# Paragraph separators
s/^\.PP//

# Itemize
s/^\.IP \\(bu 2/@item/

# Examples
s/^\.sp/@example/
s/^\.br//

# Home
s/\$HOME/\~/g

# Resources
s/^\.B "\([^ ]*\) *(\\fPclass\\fB \([^)]*\))"/@defvr Resource \1 (class \2)/

# Table
s/^\.TP//
s/^\.B//
s/^\.I//

# Pictures
\!^\.PSPIC @@srcdir@@/PICS/\([^.]*\)\..*!i\
@ifnotinfo\
@sp 1
\!^\.PSPIC @@srcdir@@/PICS/\([^.]*\)\..*!a\
@sp 1\
@end ifnotinfo
s!^\.PSPIC @@srcdir@@/PICS/\([^.]*\)\..*!@center @image{PICS/\1, 16cm}!

# Font
# s/^\.B \(.*\)/@strong{\1}/
# s/^\.I \(.*\)/@var{\1}/

# Special characters
s/\\-/-/g
s/\\\*(lq/``/g
s/\\\*(rq/''/g
s/\\(em\\&/---/g
s/\\(em/---/g
s/\\(en/--/g
s/\\(co/@copyright{}/g
s/\\ / /g
s/\\s-1<<\\s+1/<</g
s/\\s-1>>\\s+1/>>/g
s/\\s-2<<\\s+2/<</g
s/\\s-2>>\\s+2/>>/g
s/\\(->/ @result{} /g

# Strip remaining controls
s/\\c//g
s/\\&//g
s/\\|//g
s/\\s-1//g
s/\\s+1//g
s/\\s-2//g
s/\\s+2//g

# Remaining font info
s/`\\fB/@samp{/g
s/\\fR'/}/g
s/\\fB/@strong{/g
s/\\fI/@emph{/g
s/\\fP/{/g
s/\\fR/}/g
s/\\\\/\\/g
