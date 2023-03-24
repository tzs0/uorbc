#include "mod_uorbc_utils.h"
#include "mod_uorbc_topic.h"

//----------------------------------------------------------------
const E_UORBC_ID orb_multi_list[UORB_MULTI_MAX_INSTANCES] = {0};
//----------------------------------------------------------------
int mod_uorbc_utils_get_orbid(const char* name)
{
	if(0 == strcmp(name, (ORB_ID(sys_state))->o_name)) {
		return ORBC_SYS_STATE;
	}
	else if(0 == strcmp(name, (ORB_ID(sysinfo))->o_name)) {
		return ORBC_SYS_INFO;
	}
	else if(0 == strcmp(name, (ORB_ID(price_info))->o_name)) {
		return ORBC_PRICE_INFO;
	}

	return UORBC_NONE;
}
//----------------------------------------------------------------
bool mod_uorbc_utils_ismulti(const int orb_id)
{
	for(int i = 0; i < UORB_MULTI_MAX_INSTANCES; ++i) {
		if(orb_id == (int) orb_multi_list[i]) {
			return true;
		}
	}

	return false;
}
//----------------------------------------------------------------
E_UORBC_ID mod_uorbc_utils_multi_getpos(const int orb_id)
{
	for(int i = 0; i < UORB_MULTI_MAX_INSTANCES; ++i) {
		if(orb_id == (int) orb_multi_list[i]) {
			return i;
		}
	}

	return UORBC_NONE;
}

