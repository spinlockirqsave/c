/*
 * @brief   Finite state machine.
 * @date    01 Sep 2016 04:26 PM
 * @author  Piotr Gregor <piotrgregor@rsyncme.org>
 * @details FSM is represented by state and events. State is implemented
 *          by callback functions, events are enumerated (state is not).
 */

#ifndef _FSM_H_
#define _FSM_H_


#include <stdint.h>


struct fsm;
struct fsm_event;


typedef void (*fsm_state_cb)(struct fsm*, struct fsm_event*);

/* Base class for events derivation */
struct fsm_event {
    uint8_t signal;
};

/* Base class for FSM machine */
struct fsm {
    fsm_state_cb    state;  /* and this is our current state */
};

#define FSM_CTOR_(me, init) ((me)->state = (fsm_state_cb)(init))
#define FSM_INIT_(me, e) (*(me)->state)((me), (e))
#define FSM_DISPATCH_(me, e) (*(me)->state)((me), (e))
#define FSM_TRANSITION_(me, new_state) ((me)->state = (fsm_state_cb)(new_state))

#endif  /* _FSM_H_ */
