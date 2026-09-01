#include "../codexion.h"

t_data    *parse_data(char const **argv,int nb_args)
{
    int i;
    i = 1;
    if (nb_args != 9)
    {
        printf(msg_args_error());
        return NULL;
    }

    t_data *data = malloc(sizeof(data));
    validate_args(data, argv);
    return NULL;
}


int	validate_args(t_data *data, char **arg)
{
	int	i;
	int	parsed_arg[8];

	i = 1;
    
	if (parse_data(data, parsed_arg) == 1 || init_struct(data) == 1)
		return (1);
	if (strcmp("FIFO", arg[i]) == 0 || strcmp("EDF", arg[i]) == 0)
		data->scheduler = arg[i];
	else
		return (print_error(STR_ERR_INV_ARG, arg[i], data));
	return (0);
}

/**
 * Parse data provided by user into the data structure. Initialize memory for
 * coders and dongles.
 */

static int	parse_data(t_data *data, int *arg)
{
	data->nb_coders = arg[0];
	data->time_to_burnout = arg[1];
	data->time_to_compile = arg[2];
	data->time_to_debug = arg[3];
	data->time_to_refactor = arg[4];
	data->number_of_compiles_required = arg[5];
	data->dongle_cooldown = arg[6];
	return (0);
}