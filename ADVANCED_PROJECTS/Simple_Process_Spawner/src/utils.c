#include <stdio.h>
#include <string.h>
#include "utils.h"

void usage(char* argv)
{
    // Extract the program name from path.
    char *filename = strrchr(argv, '\\');
    
    printf("Usage: %s <\"path\\to\\executable\"> <window behaviour>\nWindow Behaviour (Optional): {\"hidden\", \"minimized\"}\n", (filename+1));
}
