#pragma once

#define ANSII_RGB_FG(r, g, b) "\x1b[38;2;" #r ";" #g ";" #b "m"
#define ANSII_RGB_BG(r, g, b) "\x1b[48;2;" #r ";" #g ";" #b "m"

#define ANSII_RED_FG    "\e[0;31m"
#define ANSII_GRAY_FG   "\033[90m"
#define ANSII_BLUE_FG   "\e[0;34m"

#define ANSII_RESET     "\e[0m"


