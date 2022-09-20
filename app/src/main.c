#include "user_handler.h"
#include "execute_regime.h"

#include <stdio.h>



int main (void)
{
    printf("Hello NIT");
    user_handler();

    for (int i=0; i<5;++i)
	    normal_regime();
}
