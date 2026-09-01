/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 12:47:54 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/01 14:31:06 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

// defining structs

// prototypes
typedef struct s_dongle     t_dongle;


// struct
typedef struct s_data
{
    int nb_coders;
    long time_to_burnout;
    long time_to_compile;
    long time_to_debug;
    long time_to_refactor ;
    long number_of_compiles_required;
    long dongle_cooldown;
    int scheduler;
} t_data;

typedef struct s_simulation
{
    t_data *data;
    int stop_simulation;
    pthread_mutex_t print_key;
    pthread_mutex_t stop_simulation_key;
}   t_simulation;

typedef struct s_coder
{
    int id;
    int nb_compiles;
    long last_compile_time;
    pthread_mutex_t key;
    t_dongle *left_dongle;
    t_dongle *right_dongle;
    t_simulation *simulation;
}   t_coder;


typedef struct s_dongle
{
    int id;
    long next_availability;
    int nb_coder;
    t_coder **heap;
    pthread_mutex_t key;
    pthread_cond_t signal;
} t_dongle;
// end defining structs


// start defining function prototypes

t_data    *parse_data(char const **argv,int nb_args);
char *msg_args_error();

// end defining function prototypes

#endif