# Generated automatically from Makefile.in by configure.
#
# Makefile for the examples directory.
#
CC	= gcc
LIBS	= -lcdk -lncurses -lm  -lncurses -lcurses
CFLAGS	= -g -O2 -Wall -DHAVE_CHTYPE=1  -DHAVE_LIBM=1 -DHAVE_LIBNCURSES=1 -DHAVE_COLOR=1 -DHAVE_DIRENT_H=1 -DSTDC_HEADERS=1 -DHAVE_LIMITS_H=1 -DHAVE_UNISTD_H=1 -DRETSIGTYPE=void -DHAVE_GETCWD=1 -DHAVE_MKTIME=1 -DHAVE_STRDUP=1 -DHAVE_STRERROR=1 -DHAVE_USLEEP=1 
INCDIR	= -I.. -I../include
LIBDIR	= -L..  -L/usr/lib/
BINS	= alphalist_ex bind_ex buttonbox_ex calendar_ex cdkscreen_ex dialog_ex \
		entry_ex fselect_ex graph_ex hello_ex histogram_ex \
		itemlist_ex label_ex marquee_ex matrix_ex mentry_ex \
		menu_ex position_ex preprocess_ex radio_ex scale_ex \
		scroll_ex selection_ex slider_ex subwindow_ex \
		swindow_ex template_ex viewer_ex vinstall

#
# Default compile option is stripped.
#
default: stripped

#
# Strip each of the binaries then strip them.
#
stripped:: $(BINS) stripBinaries

#
# Unstripped option for those who want it.
#
unstripped:: $(BINS)

#
# Strip all the binaries.
#
stripBinaries::
	@for i in $(BINS); do \
	   printf "Stripping %s ... " $$i; \
	   strip $$i; \
	   printf "Done.\n"; \
	done

alphalist_ex: alphalist_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

bind_ex: bind_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

buttonbox_ex: buttonbox_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

calendar_ex: calendar_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

cdkscreen_ex: cdkscreen_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

dialog_ex: dialog_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

entry_ex: entry_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

fselect_ex: fselect_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

graph_ex: graph_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

hello_ex: hello_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

histogram_ex: histogram_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

itemlist_ex: itemlist_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

label_ex: label_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

marquee_ex: marquee_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

matrix_ex: matrix_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

mentry_ex: mentry_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

menu_ex: menu_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

position_ex: position_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

preprocess_ex: preprocess_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

radio_ex: radio_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

scale_ex: scale_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

scroll_ex: scroll_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

selection_ex: selection_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

slider_ex: slider_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

subwindow_ex: subwindow_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

swindow_ex: swindow_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

template_ex: template_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

viewer_ex: viewer_ex.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

vinstall: vinstall.c
	$(CC) $(CFLAGS) $(INCDIR) $? -o $@ $(LIBDIR) $(LIBS)

#
# Standard clean directives.
#
clean::
	$(RM) core *.o $(BINS)
distclean:: clean
	$(RM) Makefile
