#include "philo.h"

//======================== FUNCTION: elapsed ===========================
//
// PURPOSE:
//    Returns ms elapsed since simulation start.
//    Used as the timestamp in every printed line.
//
// RETURN:
//    uint64_t — ms since data->start_time
//
// PARAMETERS:
//    t_data *data — contains start_time
//
// ALGORITHM:
//    1. Return time_since(data->start_time)

uint64_t	elapsed(t_data *data)

//======================== FUNCTION: get_sim_stop ======================
//
// PURPOSE:
//    Thread-safe read of sim_stop flag.
//
// RETURN:
//    int — 1 if stopped, 0 if running
//
// PARAMETERS:
//    t_data *data — shared simulation state
//
// ALGORITHM:
//    1. Lock state_mutex
//    2. Read sim_stop into local var
//    3. Unlock state_mutex
//    4. Return value

int	get_sim_stop(t_data *data)

//======================== FUNCTION: set_sim_stop ======================
//
// PURPOSE:
//    Thread-safe write of sim_stop = 1.
//    Called by monitor when death or meal limit is reached.
//
// RETURN:
//    void
//
// PARAMETERS:
//    t_data *data — shared simulation state
//
// ALGORITHM:
//    1. Lock state_mutex
//    2. Set sim_stop = 1
//    3. Unlock state_mutex

void	set_sim_stop(t_data *data)

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
