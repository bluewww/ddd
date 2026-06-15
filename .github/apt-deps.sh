#!/bin/bash
set -euxo pipefail

apt-get update
apt-get install -y \
  libmotif-dev \
  libxt-dev \
  netpbm \
  imagemagick \
  texlive-font-utils \
  texlive-latex-base \
  fig2dev

# allow imagemagick converts by giving full permission
sed -i '/coder/d' /etc/ImageMagick-6/policy.xml || true

# HACK: we don't have pamarith on ubuntu, but pnmarith seems to work too.
if ! command -v pamarith >/dev/null 2>&1; then
  ln -s /usr/bin/pnmarith /usr/bin/pamarith
fi
