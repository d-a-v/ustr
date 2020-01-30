
#include "ustr.h"

ustr_t ustrlen(ucstr str)
{
    ucstr p = str;
    while (*p++) ;
    return p - str - 1;
}
