/*
 * $Id: cdk_objs.h,v 1.22 2003/11/15 19:21:26 tom Exp $
 */

#ifndef CDKINCLUDES
#ifndef CDK_OBJS_H
#define CDK_OBJS_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CDK_H
#define CDKINCLUDES
#include <cdk.h>
#undef CDKINCLUDES
#include <binding.h>
#include <cdkscreen.h>
#endif

/*
 * Copyright 1999-2002,2003, Thomas Dickey
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

typedef struct CDKBINDING {
   BINDFN       bindFunction;
   void *       bindData;
   PROCESSFN    callbackfn;
} CDKBINDING;

struct CDKOBJS;

/*
 * Types for CDKFUNCS.returnType
 */
typedef enum {
   DataTypeUnknown = 0
   , DataTypeString
   , DataTypeInt
   , DataTypeFloat
   , DataTypeDouble
} CDKDataType;

typedef union {
   char * valueString;
   int    valueInt;
   float  valueFloat;
   double valueDouble;
} CDKDataUnion;

#define unknownString  (char *)0
#define unknownInt     (-1)
#define unknownFloat   (0.0)
#define unknownDouble  (0.0)

/*
 * Methods common to all widgets.
 */
typedef struct CDKFUNCS {
   EObjectType  objectType;
   CDKDataType  returnType;
   void         (*drawObj)         (struct CDKOBJS *, boolean);
   void         (*eraseObj)        (struct CDKOBJS *);
   void         (*moveObj)         (struct CDKOBJS *, int, int, boolean, boolean);
   int          (*injectObj)       (struct CDKOBJS *, chtype);
   void         (*focusObj)        (struct CDKOBJS *);
   void         (*unfocusObj)      (struct CDKOBJS *);
   void         (*saveDataObj)     (struct CDKOBJS *);
   void         (*refreshDataObj)  (struct CDKOBJS *);
   void         (*destroyObj)      (struct CDKOBJS *);
   /* line-drawing */
   void         (*setULcharObj)    (struct CDKOBJS *, chtype);
   void         (*setURcharObj)    (struct CDKOBJS *, chtype);
   void         (*setLLcharObj)    (struct CDKOBJS *, chtype);
   void         (*setLRcharObj)    (struct CDKOBJS *, chtype);
   void         (*setVTcharObj)    (struct CDKOBJS *, chtype);
   void         (*setHZcharObj)    (struct CDKOBJS *, chtype);
   void         (*setBXattrObj)    (struct CDKOBJS *, chtype);
} CDKFUNCS;

/* The cast is needed because traverse.c wants to use CDKOBJS pointers */
#define ObjPtr(p)           ((CDKOBJS*)(p))

#define MethodPtr(p,m)      ((ObjPtr(p))->fn->m)

/* Use these when we're certain it is a CDKOBJS pointer */
#define ObjTypeOf(p)            MethodPtr(p,objectType)
#define DataTypeOf(p)           MethodPtr(p,returnType)
#define DrawObj(p)              MethodPtr(p,drawObj)         (p,p->box)
#define EraseObj(p)             MethodPtr(p,eraseObj)        (p)
#define DestroyObj(p)           MethodPtr(p,destroyObj)      (p)
#define InjectObj(p,k)          MethodPtr(p,injectObj)       (p,(k))
#define InputWindowObj(p)       MethodPtr(p,inputWindowObj)  (p)
#define FocusObj(p)             MethodPtr(p,focusObj)        (p)
#define UnfocusObj(p)           MethodPtr(p,unfocusObj)      (p)
#define SaveDataObj(p)          MethodPtr(p,saveDataObj)     (p)
#define RefreshDataObj(p)       MethodPtr(p,refreshDataObj)  (p)

#define AcceptsFocusObj(p)      (ObjPtr(p)->acceptsFocus)
#define HasFocusObj(p)          (ObjPtr(p)->hasFocus)
#define InputWindowOf(p)        (ObjPtr(p)->inputWindow)

/*
 * Data common to all objects (widget instances).  This appears first in
 * each widget's struct to allow us to use generic functions in binding.c,
 * cdkscreen.c, position.c, etc.
 */
typedef struct CDKOBJS {
   int          screenIndex;
   CDKSCREEN *  screen;
   const CDKFUNCS * fn;
   boolean      box;
   int          borderSize;
   boolean      acceptsFocus;
   boolean      hasFocus;
   WINDOW *     inputWindow;
   void *       dataPtr;
   CDKDataUnion resultData;
   int          bindingCount;
   CDKBINDING * bindingList;
   /* line-drawing (see 'box') */
   chtype       ULChar;
   chtype       URChar;
   chtype       LLChar;
   chtype       LRChar;
   chtype       VTChar;
   chtype       HZChar;
   chtype       BXAttr;
} CDKOBJS;

#define ObjOf(ptr)              (&(ptr)->obj)
#define MethodOf(ptr)           (ObjOf(ptr)->fn)
#define ScreenOf(ptr)           (ObjOf(ptr)->screen)
#define WindowOf(ptr)           (ScreenOf(ptr)->window)
#define BorderOf(p)             (ObjOf(p)->borderSize)
#define ResultOf(p)             (ObjOf(p)->resultData)

/* line-drawing characters */
#define ULCharOf(w)             ObjOf(w)->ULChar
#define URCharOf(w)             ObjOf(w)->URChar
#define LLCharOf(w)             ObjOf(w)->LLChar
#define LRCharOf(w)             ObjOf(w)->LRChar
#define VTCharOf(w)             ObjOf(w)->VTChar
#define HZCharOf(w)             ObjOf(w)->HZChar
#define BXAttrOf(w)             ObjOf(w)->BXAttr

#define setULCharOf(o,c)        MethodOf(o)->setULcharObj(ObjOf(o),c)
#define setURCharOf(o,c)        MethodOf(o)->setURcharObj(ObjOf(o),c)
#define setLLCharOf(o,c)        MethodOf(o)->setLLcharObj(ObjOf(o),c)
#define setLRCharOf(o,c)        MethodOf(o)->setLRcharObj(ObjOf(o),c)
#define setVTCharOf(o,c)        MethodOf(o)->setVTcharObj(ObjOf(o),c)
#define setHZCharOf(o,c)        MethodOf(o)->setHZcharObj(ObjOf(o),c)
#define setBXAttrOf(o,c)        MethodOf(o)->setBXattrObj(ObjOf(o),c)

/* FIXME - remove this */
#define ReturnOf(p)   (ObjPtr(p)->dataPtr)

bool validCDKObject (CDKOBJS *);

void *  _newCDKObject(unsigned, const CDKFUNCS *);
#define newCDKObject(type,funcs) (type *)_newCDKObject(sizeof(type),funcs)

void _destroyCDKObject (CDKOBJS *);
#define destroyCDKObject(o)            _destroyCDKObject(ObjOf(o))

/* Use these for widgets that have an obj member which is a CDKOBJS struct */
#define drawCDKObject(o,box)           MethodOf(o)->drawObj       (ObjOf(o),box)
#define eraseCDKObject(o)              MethodOf(o)->eraseObj      (ObjOf(o))
#define moveCDKObject(o,x,y,rel,ref)   MethodOf(o)->moveObj       (ObjOf(o),x,y,rel,ref)
#define injectCDKObject(o,c,type)      (MethodOf(o)->injectObj    (ObjOf(o),c) ? ResultOf(o).value ## type : unknown ## type)

/* functions to set line-drawing are bound to cdk_objs.c if the widget is
 * simple, but are built into the widget for complex widgets.
 */
#define DeclareSetXXchar(storage,line) \
storage void line ## ULchar(struct CDKOBJS *, chtype); \
storage void line ## URchar(struct CDKOBJS *, chtype); \
storage void line ## LLchar(struct CDKOBJS *, chtype); \
storage void line ## LRchar(struct CDKOBJS *, chtype); \
storage void line ## VTchar(struct CDKOBJS *, chtype); \
storage void line ## HZchar(struct CDKOBJS *, chtype); \
storage void line ## BXattr(struct CDKOBJS *, chtype)

DeclareSetXXchar(extern,setCdk);

#define DeclareCDKObjects(upper, mixed, line, type) \
static int  _injectCDK ## mixed        (struct CDKOBJS *, chtype); \
static void _destroyCDK ## mixed       (struct CDKOBJS *); \
static void _drawCDK ## mixed          (struct CDKOBJS *, boolean); \
static void _eraseCDK ## mixed         (struct CDKOBJS *); \
static void _focusCDK ## mixed         (struct CDKOBJS *); \
static void _moveCDK ## mixed          (struct CDKOBJS *, int, int, boolean, boolean); \
static void _refreshDataCDK ## mixed   (struct CDKOBJS *); \
static void _saveDataCDK ## mixed      (struct CDKOBJS *); \
static void _unfocusCDK ## mixed       (struct CDKOBJS *); \
static const CDKFUNCS my_funcs = { \
   v ## upper, \
   DataType ## type, \
   _drawCDK ## mixed, \
   _eraseCDK ## mixed, \
   _moveCDK ## mixed, \
   _injectCDK ## mixed, \
   _focusCDK ## mixed, \
   _unfocusCDK ## mixed, \
   _saveDataCDK ## mixed, \
   _refreshDataCDK ## mixed, \
   _destroyCDK ## mixed, \
   line ## ULchar, \
   line ## URchar, \
   line ## LLchar, \
   line ## LRchar, \
   line ## VTchar, \
   line ## HZchar, \
   line ## BXattr, \
}

extern int getcCDKObject (CDKOBJS *);
extern void positionCDKObject (CDKOBJS *, WINDOW *);

#ifdef __cplusplus
}
#endif

#endif /* CDK_OBJS_H */
#endif /* CDKINCLUDES */
