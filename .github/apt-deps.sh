#!/bin/bash
apt update
apt install libmotif-dev libxt-dev netpbm imagemagick texlive-font-utils fig2dev texlive-latex-base
# allow imagemagick converts
sed -i'/coder/d' /etc/ImageMagick-6/policy.xml
