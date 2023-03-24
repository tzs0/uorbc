#include "mod_uorbc_topic.h"
#include "mod_uorbc_qos.h"


//------------------------------------------------
ORB_DEFINE(sys_state, UORBC_QOS_1, struct sys_state_s);
//------------------------------------------------
ORB_DEFINE(sysinfo, UORBC_QOS_1, struct sysinfo_s);
//------------------------------------------------
ORB_DEFINE(price_info, UORBC_QOS_1, struct price_info_s);
//------------------------------------------------



