## $Id$
## DDD Makefile template.
## Process this file with `automake' to produce a `Makefile.in'.

# $Id$
# DDD top-level Makefile.

# Copyright (C) 2000 Universitaet Passau, Germany.
# Written by Andreas Zeller <zeller@gnu.org>.
# 
# This file is part of DDD.
# 
# DDD is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
# 
# DDD is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public
# License along with DDD -- see the file COPYING.
# If not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
# 
# DDD is the data display debugger.
# For details, see the DDD World-Wide-Web page, 
# `http://www.gnu.org/software/ddd/',
# or send a mail to the DDD developers <ddd@gnu.org>.


# Shared data installation
dddrootdir   = $(pkgdatadir)-$(VERSION)

# -----------------------------------------------------------------------------
# Data.
# -----------------------------------------------------------------------------

dddroot_DATA = NEWS COPYING


# -----------------------------------------------------------------------------
# Subdirectories.
# -----------------------------------------------------------------------------

SUBDIRS = @subdirs@


# -----------------------------------------------------------------------------
# Distribution (via etc/make-final).
# -----------------------------------------------------------------------------

# Build a DDD distribution from CVS archive.
.PHONY: cvsdist
cvsdist: $(srcdir)/ChangeLog diststuff disttouch distclean \
	snapshot htmldist picsdist

# Create architecture-independent files in DDD subdirectory.
.PHONY: diststuff progstuff
diststuff progstuff:
	@cd $(srcdir); \
	target=$@; \
	rootme=`pwd`; export rootme; \
	srcroot=`cd $(srcdir); pwd`; export srcroot; \
	echo "==> Making $@ in ddd..."; \
	( cd ./ddd; $(MAKE) $@ )

# Touch `configure' files such that they don't get re-created.
.PHONY: disttouch
disttouch:
	@cd $(srcdir); \
	for i in $(SUBDIRS); do \
	  if test -f ./$$i/Makefile && test -f ./$$i/configure; then \
	    echo "==> Making configure in $$i..."; \
	    if (cd ./$$i; $(MAKE) configure); then true; \
	    else exit 1; fi; \
	    if (cd ./$$i; touch configure); then true; \
	    else exit 1; fi; \
          else true; fi; \
        done

# Create a snapshot distribution from the current DDD development tree.
.PHONY: snapshot
snapshot:
	rm -fr proto-toplev; mkdir proto-toplev
	( \
	    srcroot=`cd $(srcdir); pwd`; export srcroot; \
	    ddd=`sed -n < $(srcdir)/ddd/acinclude.m4 's/^ddd *= *//p'`; \
	    VER=`sed -n < $(srcdir)/ddd/acinclude.m4 's/^VERSION *= *//p'`; \
	    VER=`echo $$VER | sed 's/[^0-9]*\([0-9][^ ]*\) .*/\1/'`; \
	    echo "==> Making $$ddd-$$VER.tar.gz"; \
	    rm -f $$ddd-$$VER.tar.gz; \
	    ln -s $$srcroot proto-toplev/$$ddd-$$VER; \
	    find $(srcdir) -name RCS -prune \
		-o -name CVS -prune \
		-o -name PICS -prune \
		-o -name html -prune \
		-o -name proto-toplev -prune \
		-o -name etc -prune \
		-o -name tests -prune \
		-o -name '.gdbinit.in' -print \
		-o -name '.[a-zA-Z0-9]*' -prune \
		-o -name '*.o' -prune \
		-o -name '*.a' -prune \
		-o -name '*~' -prune \
		-o -name '*#' -prune \
		-o \( -name "$$ddd" -type l \) -prune \
		-o -name "$$ddd-$$VER-*" -prune \
		-o -name "userinfo" -prune \
		-o -name "huffencode" -prune \
		-o -name "ctest" -prune \
		-o -name "cxxtest" -prune \
		-o -name "stringify" -prune \
		-o -name "sun" -prune \
		-o -name "linux" -prune \
		-o -name "config.cache" -prune \
		-o -type f -print -o -type l -print | \
	        sed "s,^$(srcdir)/,$$ddd-$$VER/," | sort \
		> proto-toplev/FILES; \
	    ( cd proto-toplev; gtar --create --dereference \
		--verbose --files-from FILES \
		--file - ) | gzip -v -9 > $$ddd-$$VER.tar.gz; \
	)
	rm -fr proto-toplev


# Create a HTML manual distribution.
.PHONY: htmldist
htmldist:
	rm -fr proto-toplev; mkdir proto-toplev
	( \
	    srcroot=`cd $(srcdir); pwd`; export srcroot; \
	    ddd=`sed -n < ddd/acinclude.m4 's/^ddd *= *//p'`; \
	    VER=`sed -n < ddd/acinclude.m4 's/^VERSION *= *//p'`; \
	    VER=`echo $$VER | sed 's/[^0-9]*\([0-9][^ ]*\) .*/\1/'`; \
	    echo "==> Making $$ddd-$$VER-html-manual.tar.gz"; \
	    rm -f $$ddd-$$VER-pics.tar.gz; \
	    ln -s $$srcroot proto-toplev/$$ddd-$$VER; \
	    find $(srcdir)/doc/html -name RCS -prune \
		-o -name CVS -prune \
		-o -name proto-toplev -prune \
		-o -name '.[a-zA-Z0-9]*' -prune \
		-o -name '*.o' -prune \
		-o -name '*.a' -prune \
		-o -name '*~' -prune \
		-o -name '*#' -prune \
		-o -type f -print -o -type l -print | \
	        sed "s,^$(srcdir)/,$$ddd-$$VER/," | sort \
		> proto-toplev/FILES; \
	    ( cd proto-toplev; gtar --create --dereference \
		--verbose --files-from FILES \
		--file - ) | gzip -v -9 > $$ddd-$$VER-html-manual.tar.gz; \
	)
	rm -fr proto-toplev


# Create a picture distribution.
.PHONY: picsdist
picsdist:
	rm -fr proto-toplev; mkdir proto-toplev
	( \
	    srcroot=`cd $(srcdir); pwd`; export srcroot; \
	    ddd=`sed -n < ddd/acinclude.m4 's/^ddd *= *//p'`; \
	    VER=`sed -n < ddd/acinclude.m4 's/^VERSION *= *//p'`; \
	    VER=`echo $$VER | sed 's/[^0-9]*\([0-9][^ ]*\) .*/\1/'`; \
	    echo "==> Making $$ddd-$$VER-pics.tar.gz"; \
	    rm -f $$ddd-$$VER-pics.tar.gz; \
	    ln -s $$srcroot proto-toplev/$$ddd-$$VER; \
	    find $(srcdir)/ddd/PICS -name RCS -prune \
		-o -name CVS -prune \
		-o -name proto-toplev -prune \
		-o -name '.[a-zA-Z0-9]*' -prune \
		-o -name '*.o' -prune \
		-o -name '*.a' -prune \
		-o -name '*~' -prune \
		-o -name '*#' -prune \
		-o -type f -print -o -type l -print | \
	        sed "s,^$(srcdir)/,$$ddd-$$VER/," | sort \
		> proto-toplev/FILES; \
	    ( cd proto-toplev; gtar --create --dereference \
		--verbose --files-from FILES \
		--file - ) | gzip -v -9 > $$ddd-$$VER-pics.tar.gz; \
	)
	rm -fr proto-toplev

# -----------------------------------------------------------------------------
# ChangeLog.
# -----------------------------------------------------------------------------

# We create the ChangeLog file directly from the CVS archive.
CVS2CL = etc/cvs2cl.pl
USERMAP = etc/users
$(srcdir)/ChangeLog: $(srcdir)/Makefile.am
	cd $(srcdir); $(CVS2CL) --separate-header --no-wrap --revisions --utc --usermap $(USERMAP)


# -----------------------------------------------------------------------------
# Autoconf.
# -----------------------------------------------------------------------------

# Automake invokes `autoconf' without passing the `-l' option :-(
AUTOCONF = autoconf -l ddd
