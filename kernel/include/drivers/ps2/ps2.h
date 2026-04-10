#pragma once

#include <stdint.h>
#include <stdbool.h>

#define PS2_DATA_PORT   0x60
#define PS2_STATUS_PORT 0x64
#define PS2_CMD_PORT    0x64

#define PS2_RES_ACK 0xFA

#define PS2_TEST_CONTROLLER_FAIL   0xFC 
#define PS2_TEST_CONTROLLER_SUCCESS 0x55

enum ps2_test_port_result_t : uint8_t
{
    PS2_TEST_PORT_SUCCESS                     = 0x00,
    PS2_TEST_PORT_FAIL_CLOCK_LINE_STUCK_LOW   = 0x01,
    PS2_TEST_PORT_FAIL_CLOCK_LINE_STUCK_HIGH  = 0x02,
    PS2_TEST_PORT_FAIL_DATA_LINE_STUCK_LOW    = 0x03,
    PS2_TEST_PORT_FAIL_DATA_LINE_STUCK_HIGH   = 0x04,
};

enum ps2_cmd_t : uint8_t
{
    PS2_READ_CONTROLLER_CONFIG  = 0x20,
    PS2_WRITE_CONTROLLER_CONFIG = 0x60,
    PS2_DISABLE_SECOND_PORT     = 0xA7,
    PS2_ENABLE_SECOND_PORT      = 0xA8,
    PS2_TEST_SECOND_PORT        = 0xA9,
    PS2_TEST_CONTROLLER         = 0xAA,
    PS2_TEST_FIRST_PORT         = 0xAB,
    PS2_DISABLE_FIRST_PORT      = 0xAD,
    PS2_ENABLE_FIRST_PORT       = 0xAE,
};

enum ps2_buffer_status_t : bool
{
    PS2_BUFFER_EMPTY    = 0,
    PS2_BUFFER_FULL     = 1,
};

enum ps2_input_buffer_target_t : bool
{
    PS2_DEVICE      = 0,
    PS2_CONTROLLER  = 1,
};

enum ps2_status_register_bits_t : uint8_t
{
    PS2_OUTPUT_BUFFER_STATUS    = 1 << 0,
    PS2_INPUT_BUFFER_STATUS     = 1 << 1,
    PS2_SYSTEM_FLAG_STATUS      = 1 << 2,
    PS2_COMMAND_OR_DATA_STATUS  = 1 << 3,
    PS2_UNKNOWN1_STATUS         = 1 << 4,
    PS2_UNKNOWN2_STATUS         = 1 << 5,
    PS2_TIMEOUT_ERROR_STATUS    = 1 << 6,
    PS2_PARITY_ERROR_STATUS     = 1 << 7,
};

enum ps2_controller_config_bits_t : uint8_t
{
    PS2_FIRST_PORT_INTERRUPT_CONFIG     = 1 << 0,
    PS2_SECOND_PORT_INTERRUPT_CONFIG    = 1 << 1,
    PS2_SYSTEM_FLAG_CONFIG              = 1 << 2,
    PS2_SHOULD_BE_ZERO_CONFIG           = 1 << 3,
    PS2_FIRST_PORT_CLOCK_CONFIG         = 1 << 4,
    PS2_SECOND_PORT_CLOCK_CONFIG        = 1 << 5,
    PS2_FIRST_PORT_TRANSLATION_CONFIG   = 1 << 6,
    PS2_MUST_BE_ZERO_CONFIG             = 1 << 7,
};

typedef uint8_t ps2_status_register_t;
typedef uint8_t ps2_controller_config_t;

void    ps2_init(void);
void    detect_ps2_device_type();


