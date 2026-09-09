/* ************************************************************************** */
#include "../codexion.h"

static int	is_dongle_ready(t_coder *coder, t_dongle *dongle, long long now)
{
	return (dongle->in_use == 0 && dongle->queue.size > 0
		&& dongle->queue.nodes[0].coder->id == coder->id
		&& now >= dongle->next_availability);
}

static int	check_cooldown(t_coder *coder, t_dongle *d1, t_dongle *d2,
		long long now)
{
	long long	wait;

	wait = 0;
	if (d1->in_use == 0 && d1->queue.size > 0
		&& d1->queue.nodes[0].coder->id == coder->id
		&& now < d1->next_availability)
		wait = d1->next_availability - now;
	else if (d2->in_use == 0 && d2->queue.size > 0
		&& d2->queue.nodes[0].coder->id == coder->id
		&& now < d2->next_availability)
		wait = d2->next_availability - now;
	if (wait > 0)
	{
		pthread_mutex_unlock(&d2->key);
		pthread_mutex_unlock(&d1->key);
		ft_usleep(wait, coder->simulation);
		return (1);
	}
	return (0);
}

static void	wait_dongle_signal(t_coder *coder, t_dongle *d1, t_dongle *d2,
		long long now)
{

	if (!is_dongle_ready(coder, d1, now))
	{
		pthread_mutex_unlock(&d2->key);
		pthread_cond_wait(&d1->signal, &d1->key);
		pthread_mutex_unlock(&d1->key);
	}
	else
	{
		pthread_mutex_unlock(&d1->key);
		pthread_cond_wait(&d2->signal, &d2->key);
		pthread_mutex_unlock(&d2->key);
	}
}

void	request_and_grab_dongles(t_coder *coder)
{
	t_dongle	*d1;
	t_dongle	*d2;
	long long	now;

	d1 = coder->left_dongle;
	d2 = coder->right_dongle;
	if (coder->left_dongle->id > coder->right_dongle->id)
	{
		d1 = coder->right_dongle;
		d2 = coder->left_dongle;
	}
	create_dongle_request(coder, d1);
	create_dongle_request(coder, d2);
	while (!check_stop(coder->simulation))
	{
		pthread_mutex_lock(&d1->key);
		pthread_mutex_lock(&d2->key);
		now = get_current_time_ms();
		if (is_dongle_ready(coder, d1, now) && is_dongle_ready(coder, d2, now))
		{
			d1->in_use = 1;
			d2->in_use = 1;
			pop_coder(&d1->queue);
			pop_coder(&d2->queue);
			pthread_mutex_unlock(&d2->key);
			pthread_mutex_unlock(&d1->key);
			print_status("has taken a dongle", coder);
			print_status("has taken a dongle", coder);
			return ;
		}
		if (check_cooldown(coder, d1, d2, now))
			continue ;
		wait_dongle_signal(coder, d1, d2, now);
	}
}

void	drop_dongle(t_dongle *dongle, long long dongle_cooldown)
{
	pthread_mutex_lock(&dongle->key);
	dongle->in_use = 0;
	dongle->next_availability = get_current_time_ms() + dongle_cooldown;
	pthread_cond_broadcast(&(dongle->signal));
	pthread_mutex_unlock(&dongle->key);
}
