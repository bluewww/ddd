# $Id$
# Convert 8-bit umlauts to overstrike umlauts ("\"\ba" etc.)

s/é/\\o"'e"/g
s/ä/\\o'"a'/g
s/ö/\\o'"o'/g
s/ü/\\o'"u'/g
s/Ä/\\o'"A'/g
s/Ö/\\o'"O'/g
s/Ü/\\o'"U'/g
s/ß/\\o'ss's/g
