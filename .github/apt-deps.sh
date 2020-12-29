#!/bin/bash
apt update
apt install libmotif-dev libxt-dev netpbm imagemagick texlive-font-utils texlive-latex-base

if [[ $(lsb_release --release --short) == 16.04 ]]; then
    apt install transfig
else
    apt install fig2dev
fi

# allow imagemagick converts by giving full permission
sed -i '/coder/d' /etc/ImageMagick-6/policy.xml

# HACK: we don't have pamarith on ubuntu (due to licensing problems afaik), but
# pnmarith seems to work too
ln -s /usr/bin/pnmarith /usr/bin/pamarith

