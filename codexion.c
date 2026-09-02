/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/02 06:58:10 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/02 06:58:11 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
        // TODO: fix     leak
        exit_free(data, &simulation, dongles, coders);
    }
    return 0;
}
