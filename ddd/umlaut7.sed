# $Id$
# Convert 8-bit umlauts to overstrike umlauts ("\"\ba" etc.)

s/�/\\o'"a'/g
s/�/\\o'"o'/g
s/�/\\o'"u'/g
s/�/\\o'"A'/g
s/�/\\o'"O'/g
s/�/\\o'"U'/g
s/�/\\o'ss's/g
