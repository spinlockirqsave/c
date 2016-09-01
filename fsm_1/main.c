/*
 * @file    main.c
 * @author  Piotr Gregor <piotrgregor@rsyncme.org>
 * @brief   Finite state machine example.   
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "fsm.h"


struct keyboard_event {
    struct fsm_event    super_; /* derive from fsm_event */
    char                code;   /* extend fsm_event */
};

/* Signals used by keyboard FSM */
enum {
    KEYBOARD_SHIFT_PRESSED_SIG,
    KEYBOARD_SHIFT_RELEASED_SIG,
    KEYBOARD_ANY_KEY_SIG
};

/* Our state machine */
struct keyboard {
    struct fsm  super_;     /* derive from struct fsm */
    uint8_t     color;
    /* ...other features */
};

/* Override fsm methods.
 * Necessary for proper start of FSM: */
void keyboard_ctor(struct keyboard* me);
void keyboard_initial(struct keyboard* me, struct fsm_event* e);
/* Optional: */
void keyboard_default(struct keyboard* me, struct fsm_event* e);
/* Other actions... */
void keyboard_shifted(struct keyboard* me, struct fsm_event* e);
void keyboard_ctrl(struct keyboard* me, struct fsm_event* e);
void keyboard_alt_ctrl(struct keyboard* me, struct fsm_event* e);
/* other actions... */


void keyboard_ctor(struct keyboard* me) {
    if (me == NULL) {
        return;
    }
    FSM_CTOR_(&me->super_, &keyboard_initial);
}

void keyboard_initial(struct keyboard* me, struct fsm_event* e) {
    if (me == NULL) {
        return;
    }
    /* initialization... */
    fprintf(stdout, "Keyboard initialized\n");
    FSM_TRANSITION_((struct fsm*) me, &keyboard_default);    /* note: passing a pointer to the base "class" */
}

int
main(void) {
    char c;
    struct keyboard k;
    keyboard_ctor(&k);
    for (;;) {
        struct keyboard_event ke;
        fprintf(stdout, "Signal <- ");
        ke.code = getc(stdin);
        getc(stdin);    /* discard \n */
        switch (ke.code) {
            case '.':
                exit(EXIT_SUCCESS); /* end program */
            case '^':
                ke.super_.signal = KEYBOARD_SHIFT_PRESSED_SIG;
                break;
            case '_':
                ke.super_.signal = KEYBOARD_SHIFT_RELEASED_SIG;
                    break;
            default:
                ke.super_.signal = KEYBOARD_ANY_KEY_SIG;
                break;
        }
        FSM_DISPATCH_((struct fsm*)&k, (struct fsm_event*)&ke);
    }
    return 0;
}

void keyboard_default(struct keyboard* me, struct fsm_event* e) {
    char c;
    if ((me == NULL) || (e == NULL)) {
        return;
    }
    switch (e->signal) {
        case KEYBOARD_SHIFT_PRESSED_SIG:
            fprintf(stdout, "[%s] Keyboard shift pressed\n", __func__);
            FSM_TRANSITION_((struct fsm*) me, &keyboard_shifted);
            break;
        case KEYBOARD_ANY_KEY_SIG:
            c = tolower(((struct keyboard_event*)e)->code);
            fprintf(stdout, "[%s] Any key [%c]\n", __func__, c);
            break;
        default:
            break;
    }
}

void keyboard_shifted(struct keyboard* me, struct fsm_event* e) {
    char c;
    if ((me == NULL) || (e == NULL)) {
        return;
    }
    switch (e->signal) {
        case KEYBOARD_SHIFT_RELEASED_SIG:
            fprintf(stdout, "[%s] Keyboard shift released\n", __func__);
            FSM_TRANSITION_((struct fsm*) me, &keyboard_default);
            break;
        case KEYBOARD_ANY_KEY_SIG:
            c = toupper(((struct keyboard_event*)e)->code);
            fprintf(stdout, "[%s] Any key [%c]\n", __func__, c);
            break;
        default:
            break;
    }
}

void keyboard_ctrl(struct keyboard* me, struct fsm_event* e) {
}

void keyboard_alt_ctrl(struct keyboard* me, struct fsm_event* e) {
}
