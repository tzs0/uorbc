/* Auto-generated by genmsg_cpp from file /Volumes/SD/Git/PX4/Firmware_fmuv2/Firmware/msg/actuator_armed.msg */

#include "topic_test0.h"


const char *__orb_test0_fields = "uint64_t timestamp;bool armed;bool prearmed;bool ready_to_arm;bool lockdown;bool manual_lockdown;bool force_failsafe;bool in_esc_calibration_mode;uint8_t[1] _padding0;";

ORB_DEFINE(test0, struct test0_s, 15,
    __orb_test0_fields);

