#include "config.h"

#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    char *config_data = NULL;
    size_t config_size = 0;

    int result = config_read(&config_data, &config_size);
    if (result != 0)
    {
        fputs("Could not read the Battle.net config.\n", stderr);
        return result;
    }
    printf("Read %zu bytes successfully.\n", config_size);

    free(config_data);
    return 0;
}
