
#include "ustr.h"

ustr_t ustrcmp (ucstr s1, ucstr s2)
{
        while (*s1 && *s2 && (*s1++ == *s2++)) ;
        return *s1 - *s2;
}
