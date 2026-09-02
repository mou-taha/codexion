/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 06:58:10 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 14:34:06 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int main(int argc, char const *argv[])
{
    t_data      *data;
    t_simulation simulation;
    t_dongle    *dongles;
    t_coder     *coders;

    data = parse_data(argv, argc);
    if (!data)
        return (1);
    if (init(data, &simulation, &dongles, &coders) == 1)
    {
        //TODO: call start simulation
        start_simulation(data, &simulation, coders);
        destroy(data, &simulation, dongles, coders);
    }
    else
        printf("initialization failed \n");
    return (0);
}
