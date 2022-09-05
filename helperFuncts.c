#include "helperFuncts.h"

char *cast_to_string(int num, char *str) {
    if(str == NULL) {
        return NULL;
    }
    sprintf(str, "%d", num);
    return str;
}