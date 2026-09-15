/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tmousnia <tmousnia@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/01 12:47:54 by tmousnia          #+#    #+#             */
/*   Updated: 2026/09/06 23:51:43 by tmousnia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <unistd.h>

// defining structs

// prototypes
typedef struct s_dongle	t_dongle;
typedef struct s_coder	t_coder;

// struct
typedef struct s_heap_node
{
	int					request_id;
	t_coder				*coder;
	long long			request_time;
}						t_heap_node;

typedef struct s_heap
{
	t_heap_node			*nodes;
	int					size;
	int					capacity;
	char				*scheduler;
}						t_heap;

typedef struct s_data
{
	int					nb_coders;
	long long			time_to_burnout;
	long long			time_to_compile;
	long long			time_to_debug;
	long long			time_to_refactor;
	int					number_of_compiles_required;
	long long			dongle_cooldown;
	char				*scheduler;
}						t_data;

typedef struct s_simulation
{
	t_data				*data;
	int					stop_simulation;
	long long			start_time;
	pthread_mutex_t		print_key;
	pthread_mutex_t		stop_simulation_key;
	t_coder				*coders;
}						t_simulation;

typedef struct s_coder
{
	int					id;
	int					nb_compiles;
	long long			last_compile_time;
	int					is_compiling;
	pthread_mutex_t		key;
	t_dongle			*left_dongle;
	t_dongle			*right_dongle;
	t_simulation		*simulation;
	pthread_t			thread_id;
}						t_coder;

typedef struct s_dongle
{
	int					id;
	long long			next_availability;
	int					nb_coder;
	int					in_use;
	t_heap				queue;
	pthread_mutex_t		key;
	pthread_cond_t		signal;
	long long			request_counter;
}						t_dongle;

// end defining structs

// start defining function prototypes

// parsing
t_data					*parse_data(char const **argv, int nb_args);
int						ft_isdigit(int c);
int						is_valid_positive_number(char const *str);

// initialization
int						init(t_data *data, t_simulation *simulation,
							t_dongle **dongles, t_coder **coders);
void					destroy(t_data *data, t_simulation *simulation,
							t_dongle *dongles, t_coder *coders);

// routines
void					*coder_routine(void *arg);
void					create_dongle_request(t_coder *coder, t_dongle *dongle);

// simulation
int						start_simulation(t_data *data, t_simulation *simulation,
							t_coder *coders);
long long				get_current_time_ms(void);
void					ft_usleep(int milliseconds_to_sleep,
							t_simulation *simulation);
int						check_stop(t_simulation *simulation);
void					grab_dongle(t_coder *coder, t_dongle *dongle);
void					drop_dongle(t_dongle *dongle,
							long long dongle_cooldown);
void					request_and_grab_dongles(t_coder *coder);

// logs
void					print_status(char *status, t_coder *coder);

// monitor
void					*monitor_routine(void *arg);
int						is_coder_finished(t_coder *coder);

// heap
t_heap					init_heap(int capacity, char *scheduler);
int						insert_to_heap(t_heap *queue, t_heap_node request);
int						is_fifo(char *scheduler);
int						is_edf(char *scheduler);
void					queue_swap(t_heap *queue, int current, int parent);
void					pop_coder(t_heap *queue);

// end defining function prototypes

#endif
