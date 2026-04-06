#pragma once
#include "misc/ansii.h"
#include "misc/kabort.h"
#include "devices/serial.h"

typedef enum todo_level_t
{
    TODO_LEVEL_FATAL,
    TODO_LEVEL_INFO,
} todo_level_t;

#define todo_with_msg(in, message)  (srprintf("[   " ANSII_YELLOW "TODO" ANSII_RESET "    ] " ANSII_GRAY_FG "%s" ANSII_RESET ": " message "\n", #in), kabort())
#define todo_without_msg(in)        (srprintf("[   " ANSII_YELLOW "TODO" ANSII_RESET "    ] " ANSII_GRAY_FG "%s" ANSII_RESET ": " "Unimplemented!" "\n", #in), kabort())
#define choose_todo(first, second, ret, ...) ret
#define todo(...) choose_todo(__VA_ARGS__, todo_with_msg, todo_without_msg)(__VA_ARGS__)
