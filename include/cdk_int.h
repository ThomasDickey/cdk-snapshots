/*
 * $Id: cdk_int.h,v 1.26 2013/09/01 18:06:41 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDK_INT_H
#define CDK_INT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <cdk.h>

/*
 * Copyright 2003-2011,2013 Thomas E. Dickey
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
 * 	This product includes software developed by Thomas Dickey
 * 	and contributors.
 * 4. Neither the name of Thomas Dickey, nor the names of contributors
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

#define typeCallocN(type,n)     (type*)calloc((size_t)(n), sizeof(type))
#define typeCalloc(type)        typeCallocN(type,1)

#define typeReallocN(type,p,n)  (type*)realloc(p, (size_t)(n) * sizeof(type))

#define typeMallocN(type,n)     (type*)malloc((size_t)(n) * sizeof(type))
#define typeMalloc(type)        typeMallocN(type,1)

#define freeChecked(p)          if ((p) != 0) free (p)
#define freeAndNull(p)          if ((p) != 0) { free (p); p = 0; }

#define isChar(c)               ((int)(c) >= 0 && (int)(c) < KEY_MIN)
#define CharOf(c)               ((unsigned char)(c))

#define SIZEOF(v)               (sizeof(v)/sizeof((v)[0]))

/*
 * Macros to check if caller is attempting to make the widget as high (or wide)
 * as the screen.
 */
#define isFullWidth(n)		((n) == FULL || (COLS != 0 && ((n) >= COLS)))
#define isFullHeight(n)		((n) == FULL || (LINES != 0 && ((n) >= LINES)))

/*
 * Hide details of modifying widget->exitType
 */
#define storeExitType(d)	ObjOf(d)->exitType = (d)->exitType
#define initExitType(d)		storeExitType(d) = vNEVER_ACTIVATED
#define setExitType(w,c)	setCdkExitType(ObjOf(w), &((w)->exitType), c)
#define copyExitType(d,s)	storeExitType(d) = ExitTypeOf(s)

/*
 * Use this if checkCDKObjectBind() returns true, use this function to
 * decide if the exitType should be set as a side-effect.
 */
#define checkEarlyExit(w)	if (EarlyExitOf(w) != vNEVER_ACTIVATED) \
				    storeExitType(w) = EarlyExitOf(w)

/*
 * Position within the data area of a widget, accounting for border and title.
 */
#define SCREEN_XPOS(w,n) ((n) + BorderOf(w))
#define SCREEN_YPOS(w,n) ((n) + BorderOf(w) + TitleLinesOf(w))

/*
 * Miscellaneous definitions.
 */
#define CDK_PATHMAX		256

extern char *GPasteBuffer;

#ifdef __cplusplus
}
#endif

#endif /* CDK_INT_H */
#endif /* CDKINCLUDES */
