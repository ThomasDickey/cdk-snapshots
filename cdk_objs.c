#include <cdk_int.h>

/*
 * Default method-functions for CDK objects.
 *
 * $Author: tom $
 * $Date: 2003/11/16 21:08:13 $
 * $Revision: 1.3 $
 */
void
setCdkULchar(CDKOBJS * obj, chtype ch)
{
    obj->ULChar = ch;
}

void
setCdkURchar(CDKOBJS * obj, chtype ch)
{
    obj->URChar = ch;
}

void
setCdkLLchar(CDKOBJS * obj, chtype ch)
{
    obj->LLChar = ch;
}

void
setCdkLRchar(CDKOBJS * obj, chtype ch)
{
    obj->LRChar = ch;
}

void
setCdkHZchar(CDKOBJS * obj, chtype ch)
{
    obj->HZChar = ch;
}

void
setCdkVTchar(CDKOBJS * obj, chtype ch)
{
    obj->VTChar = ch;
}

void
setCdkBXattr(CDKOBJS * obj, chtype ch)
{
    obj->BXAttr = ch;
}
