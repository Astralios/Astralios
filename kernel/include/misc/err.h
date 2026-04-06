#pragma once

typedef enum err_t {
    SUCCESS,
    TIMEOUT,
    UNIMPLEMENTED,
    
    NAME_LIMIT,
    FILE_NOT_FOUND,
    OUT_OF_BOUNDS,
    OUT_OF_MEMORY,

    OTHER,
    ERR_COUNT,
} err_t;

