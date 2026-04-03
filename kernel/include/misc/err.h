#pragma once

typedef enum err_t {
    SUCCESS,
    TIMEOUT,
    UNIMPLEMENTED,
    
    NAME_LIMIT,
    FILE_NOT_FOUND,
    OUT_OF_BOUNDS,

    OTHER,
    ERR_COUNT,
} err_t;

