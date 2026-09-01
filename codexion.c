#include "codexion.h"

int main(int argc, char const *argv[])
{
    t_data *data;
    data = parse_data(argv, argc);
    t_simulation simulation;
    t_dongle *dongles;
    t_coder *coders;
    if (data)
    {
        if (init(data, &simulation, &dongles, &coders) == 1)
            printf("initialization success \n");
        else
            printf("initialization failed \n");
        //TODO: fix leak
        free(data);
        free(coders);
        free(dongles);
    }
    return 0;
}
