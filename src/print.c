#include "philo.h"

//======================== FUNCTION: get_sim_stop ======================
//
// PURPOSE:
//    Safely check if the simulation is over 
//    - is for asking "Is the simulation over?" (Read-only)
//    Every single loop! Philosophers use this constantly :
//    (while (!get_sim_stop(data))) to know if they should keep eating/sleeping
//    or if they should exit their threads.

int	get_sim_stop(t_data *data)
{
  int var;

  pthread_mutex_lock(&data->state_mutex);
  var = data->sim_stop;
  pthread_mutex_unlock(&data->state_mutex);
  return val;
}

//======================== FUNCTION: set_sim_stop ======================
//
// PURPOSE:
//    To trigger the end of the simulation.
//    - is for declaring "The simulation is now over!" (Write-only).
//    Only the Monitor thread uses this !
//    It calls this function the moment it detects a philosopher has starved,
//    or the moment it confirms everyone has eaten their max_meals

void	set_sim_stop(t_data *data)
{
  pthread_mutex_lock(&data->state_mutex);
  sim_stop = 1;
  pthread_mutex_unlock(&data->state_mutex);
}
//======================== FUNCTION: print_line ========================
//
// PURPOSE:
//    Formats and prints one status line to stdout.
//    Uses static msg array indexed by t_status enum.
//    Called only from within print_status (already holds print_mutex).
//
// RETURN:
//    void
//
// PARAMETERS:
//    t_data   *data   — needed for elapsed timestamp
//    int       id     — philosopher id (1-indexed)
//    t_status  status — enum value maps directly to msg string
//
// VARIABLES:
//    static const char *msgs[] — indexed by t_status enum values
//
// ALGORITHM:
//    1. Use status as index into msgs[]
//    2. printf: "%lu %d %s\n" with elapsed, id, msgs[status]

void	print_line(t_data *data, int id, t_status status)
{
  static char *msgs[5];

  msgs[0] = "null_opr";
  msgs[1] = "died";
  msgs[2] = "is eating";
  msgs[3] = "is sleeping";
  msgs[4] = "is thinking";
  msgs[5] = "has taken a fork";

  printf("%lu %d %s\n", time_since(data->start_time), id, msgs[status]);
}

//======================== FUNCTION: print_status ======================
//
// PURPOSE:
//    Thread-safe status printer with Pedro's double-check pattern.
//    Guarantees no message is printed after "died".
//
// RETURN:
//    void
//
// PARAMETERS:
//    t_philo  *philo  — philosopher printing the message
//    t_status  status — what to print
//
// ALGORITHM:
//    1. Lock state_mutex
//    2. If sim_stop AND status != DIED → unlock + return (early exit)
//    "(status != DIED) current message being printed is not a death message"  
//    3. Unlock state_mutex
//    4. Lock print_mutex
//    5. Lock state_mutex again (re-check — sim may have stopped
//       while we waited for print_mutex)
//    6. If !sim_stop OR status == DIED → print_line + unlock state
//    7. Else → unlock state
//    8. Unlock print_mutex
//
// EDGE CASES:
//    - Thread passes step 2, blocks on print_mutex, monitor prints
//      "died" meanwhile → re-check at step 5 catches this
//    - DIED status always prints regardless of sim_stop

void	print_status(t_philo *philo, t_status status)
{
  t_data *data;

  pthread_mutex_lock(&data->state_mutex);
  if (data->sim_stop && status != DIED)
  {
    pthread_mutex_unlock(&data->state_mutex);
    return ;
  }
  pthread_mutex_unlock(&data->state_mutex);
  pthread_mutex_lock(&data->print_mutex);
  pthread_mutex_lock(&data->state_mutex);
  if (!data->sim_stop || status == DIED)
  {
    pthread_mutex_unlock(&data->state_mutex);
    print_status(data, philo->id, status)
  }
  else 
    pthread_mutex_unlock(&data->state_mutex);
  pthread_mutex_unlock(&data->print_mutex);
}
