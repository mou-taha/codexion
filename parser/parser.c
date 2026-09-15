/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/06 22:03:54 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/15 10:53:56 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../codexion.h"

int		validate_args(t_data *data, char const **arg);
void	parse_numbers_data(t_data *data, int *arg);
int		validate_time_values(int *arg);

/**
 * @brief Parses the command-line arguments and validates them.
 * 
 * check if number of arguments is correct, then validate each argument to ensure they are positive numbers and the last argument is either "fifo" or "edf".
 * 
 * @param argv The command-line arguments.
 * @param nb_args The number of command-line arguments.
 * @return A pointer to the parsed data, or NULL if parsing fails.
 */
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

/**
 * @brief Validates the command-line arguments.
 * 
 * This function ensures that the first seven arguments are positive numbers and that the last argument is either "fifo" or "edf". If the arguments are valid, it populates the t_data structure with the parsed values.
 * 
 * @param data Pointer to the parsed data structure.
 * @param arg The command-line arguments.
 * @return 1 if the arguments are valid, 0 otherwise.
 */
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
		printf("Invalid arguments\n");
	return (0);
}
/**
 * @brief populate data structure with parsed numeric values.
 * 
 * @param data Pointer to the parsed data structure.
 * @param arg The numeric command-line arguments.
 */
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
/**
 * @brief Validates the time values.
 * 
 * check if the time values are positive and the number of compiles required is non-negative.
 * 
 * @param arg The numeric command-line arguments.
 * @return 1 if the time values are valid, 0 otherwise.
 */
int	validate_time_values(int *arg)
{
	if (arg[0] <= 0 || arg[1] <= 0 || arg[2] <= 0
		|| arg[3] <= 0 || arg[4] <= 0 || arg[5] <= 0 || arg[6] < 0)
	{
		return (0);
	}
	return (1);
}
