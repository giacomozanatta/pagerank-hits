#include "utils.h"
#include <string.h>

int starts_with(const char *prefix, const char *string)
{
    return strncmp(prefix, string, strlen(prefix)) == 0;
}
