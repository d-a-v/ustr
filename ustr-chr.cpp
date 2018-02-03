
#include <ustr.h>

ucstr ustrchr (ucstr s, char c)
{
	--s;
	while (*++s)
		if (*s == c)
			return s;
	return NULL;
}
