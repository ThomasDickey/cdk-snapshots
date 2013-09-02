/*
 * $Id: scroller.h,v 1.3 2013/09/01 21:56:04 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef SCROLLER_H
#define SCROLLER_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Copyright 2013 Thomas E.Dickey and Corentin Delorme
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgment:
 * 	This product includes software developed by Thomas E. Dickey
 * 	and contributors.
 * 4. Neither the name of Thomas E. Dickey, nor the names of contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THOMAS DICKEY AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THOMAS DICKEY OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#define SCROLLER_FIELDS \
   CDKOBJS  obj; \
   WINDOW * parent; \
   WINDOW * win; \
   WINDOW * scrollbarWin; \
   WINDOW * shadowWin; \
   int      titleAdj;   /* unused */ \
   chtype **    item; \
   int *    itemLen; \
   int *    itemPos; \
\
   int      currentTop; \
   int      currentItem; \
   int      currentHigh; \
\
   int      maxTopItem; \
   int      maxLeftChar; \
   int      maxchoicelen; \
   int      leftChar; \
   int      lastItem; \
   int      listSize; \
   int      boxWidth; \
   int      boxHeight; \
   int      viewSize; \
\
   int      scrollbarPlacement; \
   boolean  scrollbar; \
   int      toggleSize; /* size of scrollbar thumb/toggle */ \
   int      togglePos; /* position of scrollbar thumb/toggle */ \
   float    step; /* increment for scrollbar */ \
\
   EExitType    exitType; \
   boolean  shadow; \
   chtype   highlight

struct SScroller {
   /* This field must stay on top */
   SCROLLER_FIELDS;
};

typedef struct SScroller CDKSCROLLER;

void scroller_KEY_UP (CDKSCROLLER * /* scroller */);

void scroller_KEY_DOWN (CDKSCROLLER * /* scroller */);

void scroller_KEY_LEFT (CDKSCROLLER * /* scroller */);

void scroller_KEY_RIGHT (CDKSCROLLER * /* scroller */);

void scroller_KEY_PPAGE (CDKSCROLLER * /* scroller */);

void scroller_KEY_NPAGE (CDKSCROLLER * /* scroller */);

void scroller_KEY_HOME (CDKSCROLLER * /* scroller */);

void scroller_KEY_END (CDKSCROLLER * /* scroller */);

void scroller_FixCursorPosition (CDKSCROLLER * /* scroller */);

void scroller_SetPosition (CDKSCROLLER * /* scroller */, int /* item */);

int scroller_MaxViewSize (CDKSCROLLER * /* scroller */);

void scroller_SetViewSize (CDKSCROLLER * /* scroller */, int /* size */);

#ifdef __cplusplus
}
#endif

#endif /* SCROLLER_H */
#endif /* CDKINCLUDES */
