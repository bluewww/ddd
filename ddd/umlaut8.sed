# $Id$
# Convert overstrike umlauts ("\"\ba" etc.) to 8-bit umlauts

# Plain
s/'e/é/g
s/"a/ä/g
s/"o/ö/g
s/"u/ü/g
s/"A/Ä/g
s/"O/Ö/g
s/"U/Ü/g
s/sss\([^]\)/ß\1/g
s/sss$/ß/g

# Underline
s/'_e/_é/g
s/"_a/_ä/g
s/"_o/_ö/g
s/"_u/_ü/g
s/"_A/_Ä/g
s/"_O/_Ö/g
s/"_U/_Ü/g
s/_s_ss\([^]\)/_ß\1/g
s/_s_ss$/_ß/g

# Bold
s/''e/éé/g
s/""a/ää/g
s/""o/öö/g
s/""u/üü/g
s/""A/ÄÄ/g
s/""O/ÖÖ/g
s/""U/ÜÜ/g
s/ssssss\([^]\)/ßß\1/g
s/ssssss$/ßß/g

