
#include "ustr.h"

ustr ustrcpy (ustr d, ucstr s)
{
	ustr ret = d;
	while ((*d++ = (char)*s++)) ;
	return ret;
}
