/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 22:03:54 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/06 22:04:01 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int		validate_args(t_data *data, char const **arg);
void	parse_numbers_data(t_data *data, int *arg);
int     validate_time_values(int *arg);

t_data	*parse_data(char const **argv, int nb_args)
{
	t_data	*data;

	if (nb_args != 9)
	{
		printf("\nInvalid number of arguments\n");
		return (NULL);
	}
	data = malloc(sizeof(t_data));
	if (validate_args(data, argv) == 0)
	{
		free(data);
		return (NULL);
	}
	return (data);
}

int	validate_args(t_data *data, char const **arg)
{
	int	i;
	int	parsed_arg[8];

	i = 1;
	while (i < 8)
	{
		if (!is_valid_positive_number(arg[i]))
		{
			printf("\nInvalid arguments\n");
			return (0);
		}
		parsed_arg[i - 1] = atoi(arg[i]);
		i++;
	}
	if ((strcmp(arg[i], "fifo") == 0 || strcmp(arg[i], "edf") == 0)
		&& validate_time_values(parsed_arg))
	{
		data->scheduler = (char *)arg[i];
		parse_numbers_data(data, parsed_arg);
		return (1);
	}
	else
		printf("\nInvalid arguments\n");
	return (0);
}

void	parse_numbers_data(t_data *data, int *arg)
{
	data->nb_coders = arg[0];
	data->time_to_burnout = arg[1];
	data->time_to_compile = arg[2];
	data->time_to_debug = arg[3];
	data->time_to_refactor = arg[4];
	data->number_of_compiles_required = arg[5];
	data->dongle_cooldown = arg[6];
}

int     validate_time_values(int *arg)
{
    if (arg[0] <= 0 || arg[1] <= 0 || arg[2] <= 0 || 
        arg[3] <= 0 || arg[4] <= 0 || arg[5] <= 0 || arg[6] < 0)
    {
        return (0);
    }
    return (1);
}