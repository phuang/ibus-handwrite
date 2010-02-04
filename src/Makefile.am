# vim:set noet ts=4:
#
# ibus-handwrite - The handwrite engine for IBus
#
# Copyright (c) 2010 microcai <microcai@fedoraproject.org>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

TABLEFILE = $(pkgdatadir)/data/table.txt


AM_CFLAGS=  @IBUS_CFLAGS@ -DPKGDATADIR=\"$(pkgdatadir)\"  \
			-DMODEFILE=\"$(MODEFILE)\" -DTABLEFILE=\"$(TABLEFILE)\"

AM_LDFLAGS = @IBUS_LIBS@  @ZINNIA_LIBS@ 


if WITH_GTK
UI_FRONTEND = UI_gtk.c
AM_CFLAGS += @GTK2_CFLAGS@
AM_LDFLAGS += @GTK2_LIBS@
else
UI_FRONTEND = UI_X11.c
AM_CFLAGS += -DWITH_X11
AM_LDFLAGS += -lX11
endif

if WITH_DEBIAN
MODEFILE = /usr/share/tegaki/models/zinnia/handwriting-zh_CN.model
else
MODEFILE = $(pkgdatadir)/data/handwriting-zh_CN.model

test: ibus-engine-handwrite
	$(builddir)/ibus-engine-handwrite \
		--icon $(top_srcdir)/icons/ibus-handwrite.svg \
		--model $(top_srcdir)/data/handwriting-zh_CN.model
		--table $(top_srcdir)/data/table.txt
endif



libexecdir = @datadir@/ibus/engine/

libexec_PROGRAMS = ibus-engine-handwrite

ibus_engine_handwrite_SOURCES =  main.c 	engine.c 	engine.h	prase.c prase.h \
					  $(UI_FRONTEND) \
					  handrecog_zinnia.c \
					  handrecog.c \
					  handrecog.h \
					  UI.h \
					  $(NULL)

component_DATA = handwrite.xml 

componentdir = @datadir@/ibus/component

EXTRA_DIST = handwrite.xml.in 

CLEANFILES = \
	handwrite.xml \
	$(NULL)

handwrite.xml: handwrite.xml.in
	( \
		libexecdir=${libexecdir}; \
		pkgdatadir=${pkgdatadir}; \
		s=`cat $<`; \
		eval "echo \"$${s}\""; \
	) > $@

