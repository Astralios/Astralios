#pragma once
#include <stdint.h>

typedef enum : uint8_t 
{
    LEFT_CLICK   = 1 << 0,
    RIGHT_CLICK  = 1 << 1, 
    MIDDLE_CLICK = 1 << 2
} mse_action_t;

typedef struct {
    int          dx;
    int          dy;
    mse_action_t action;
} mse_ev_t;
