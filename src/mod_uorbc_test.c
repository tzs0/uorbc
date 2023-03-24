#include "mod_uorbc_topic.h"
#include "uORB.h"
#include "mod_uorbc_platform.h"
#include "mod_uorbc_utils.h"


static void* uorbc_test_thread_pub(void);
static void* uorbc_test_thread_sub(void);
static int uorbc_check_copy(const struct orb_metadata* meta, int handle, void* buffer, uint8_t* update);


static int uorbc_check_copy(const struct orb_metadata* meta, int handle, void* buffer, uint8_t* update)
{
	*update = 0;

	int ret = orb_copy(meta, handle, buffer);
	if(0 == ret) {
		uorbc_platform_log("orb_copy ok! meta->o_name:%s now all_msg is read", meta->o_name);
		*update = 1;
	}
	else {
		uorbc_platform_log_err("orb_copy fail:error or no msg in queue");
	}

	return ret;
}

static void* uorbc_test_thread_pub(void)
{
	uorbc_platform_log("uorbc_test_thread_pub");

	struct sysinfo_s sysinfo_new;

	uint32_t pub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(1500);

		uorbc_platform_log("0 pub_count:%d", pub_count);
		const char* pub = "0 pub";

		sysinfo_new.machine_number++;
		uorbc_platform_log("%s sysinfo_new.machine_number[%d] ++ %s", pub, sysinfo_new.machine_number, pub);
		orb_publish(ORB_ID(sysinfo), UORB_PUB_NORMAL, &sysinfo_new);

		pub_count++;
	}
}

static void* uorbc_test_thread_pub1(void)
{
	uorbc_platform_log("uorbc_test_thread_pub1");

	struct sysinfo_s sysinfo_new;

	uint32_t pub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(1500);
		const char* pub = "1 pub";

		uorbc_platform_log("1 pub_count:%d", pub_count);

		sysinfo_new.machine_number += 11;
		uorbc_platform_log("%s sysinfo_new.machine_number[%d] +=11 %s", pub, sysinfo_new.machine_number, pub);
		orb_publish(ORB_ID(sysinfo), UORB_PUB_NORMAL, &sysinfo_new);

		pub_count++;
	}
}

static void* uorbc_test_thread_pub2(void)
{
	uorbc_platform_log("uorbc_test_thread_pub2");

	struct sysinfo_s sysinfo_new;

	uint32_t pub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(1500);
		const char* pub = "2 pub";

		uorbc_platform_log("2 pub_count:%d", pub_count);

		sysinfo_new.machine_number += 22;
		uorbc_platform_log("%s sysinfo_new.machine_number[%d] +=22 %s", pub, sysinfo_new.machine_number, pub);
		orb_publish(ORB_ID(sysinfo), UORB_PUB_NORMAL, &sysinfo_new);

		pub_count++;
	}
}

static void* uorbc_test_thread_pub3(void)
{
	uorbc_platform_log("uorbc_test_thread_pub3");

	struct sysinfo_s sysinfo_new;

	uint32_t pub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(1500);
		const char* pub = "3 pub";

		uorbc_platform_log("3 pub_count:%d", pub_count);

		sysinfo_new.machine_number += 33;
		uorbc_platform_log("%s sysinfo_new.machine_number[%d] +=33 %s", pub, sysinfo_new.machine_number, pub);
		orb_publish(ORB_ID(sysinfo), UORB_PUB_NORMAL, &sysinfo_new);

		pub_count++;
	}
}

static void* uorbc_test_thread_pub4(void)
{
	uorbc_platform_log("uorbc_test_thread_pub4");

	struct sysinfo_s sysinfo_new;

	uint32_t pub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(1500);
		const char* pub = "4 pub";

		uorbc_platform_log("4 pub_count:%d", pub_count);

		sysinfo_new.machine_number += 53;
		uorbc_platform_log("%s sysinfo_new.machine_number[%d] +=53 %s", pub, sysinfo_new.machine_number, pub);
		orb_publish(ORB_ID(sysinfo), UORB_PUB_NORMAL, &sysinfo_new);

		pub_count++;
	}
}

static void* uorbc_test_thread_pub5(void)
{
	uorbc_platform_log("uorbc_test_thread_pub5");

	struct sysinfo_s sysinfo_new;

	uint32_t pub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(1500);
		const char* pub = "5 pub";

		uorbc_platform_log("5 pub_count:%d", pub_count);

		sysinfo_new.machine_number += 63;
		uorbc_platform_log("%s sysinfo_new.machine_number[%d] +=63 %s", pub, sysinfo_new.machine_number, pub);
		orb_publish(ORB_ID(sysinfo), UORB_PUB_NORMAL, &sysinfo_new);

		pub_count++;
	}
}

static void* uorbc_test_thread_pub6(void)
{
	uorbc_platform_log("uorbc_test_thread_pub6");

	struct sysinfo_s sysinfo_new;

	uint32_t pub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(1500);
		const char* pub = "6 pub";

		uorbc_platform_log("6 pub_count:%d", pub_count);

		sysinfo_new.machine_number += 73;
		uorbc_platform_log("%s sysinfo_new.machine_number[%d] +=73 %s", pub, sysinfo_new.machine_number, pub);
		orb_publish(ORB_ID(sysinfo), UORB_PUB_NORMAL, &sysinfo_new);

		pub_count++;
	}
}


static void* uorbc_test_thread_sub(void)
{
	uorbc_platform_log("uorbc_test_thread_sub");

	int handle_sysinfo = orb_subscribe(ORB_ID(sysinfo));
	int ret = -1;

	uint32_t sub_count = 0;

	while(1) {
		uorbc_platform_delay_ms(5500);
		const char* sub = "0 sub";
		uorbc_platform_log("%s sub_count:%d", sub, sub_count);
		sub_count++;

		struct _que_msg {
			uint32_t num;
			struct sysinfo_s sysinfo[0];
		};
		struct _que_msg* q = NULL;
		uint32_t pointer;

		uint8_t update = 0;

		q = NULL;
		ret = uorbc_check_copy(ORB_ID(sysinfo), handle_sysinfo, &pointer, &update);
		uorbc_platform_log("ret:%d update:%d %s", ret, update, sub);
		q = pointer;
		if(update&&q) {
			struct sysinfo_s* p_sysinfo = &(q->sysinfo);
			for(int i=0; i<q->num; i++) {
				uorbc_platform_log("%s machine_number[%d] %s total %d msg, cur:%d",
				                   sub, p_sysinfo->machine_number, sub, q->num, i);
				p_sysinfo++;
			}
			free(q);
		}
	}
}

static void* uorbc_test_thread_sub1(void)
{
	uorbc_platform_log("uorbc_test_thread_sub1");

	int handle_sysinfo = orb_subscribe(ORB_ID(sysinfo));
	int ret = -1;

	uint32_t sub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(5500);
		const char* sub = "1 sub";
		uorbc_platform_log("%s sub_count:%d", sub, sub_count);
		sub_count++;

		struct _que_msg {
			uint32_t num;
			struct sysinfo_s sysinfo[0];
		};
		struct _que_msg* q = NULL;
		uint32_t pointer;

		uint8_t update = 0;

		q = NULL;
		ret = uorbc_check_copy(ORB_ID(sysinfo), handle_sysinfo, &pointer, &update);
		uorbc_platform_log("ret:%d update:%d %s", ret, update, sub);
		q = pointer;
		if(update&&q) {
			struct sysinfo_s* p_sysinfo = &(q->sysinfo);
			for(int i=0; i<q->num; i++) {
				uorbc_platform_log("%s machine_number[%d] %s total %d msg, cur:%d",
				                   sub, p_sysinfo->machine_number, sub, q->num, i);
				p_sysinfo++;
			}
			free(q);
		}
	}
}

static void* uorbc_test_thread_sub2(void)
{
	uorbc_platform_log("uorbc_test_thread_sub2");

	int handle_sysinfo = orb_subscribe(ORB_ID(sysinfo));
	int ret = -1;

	uint32_t sub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(5500);
		const char* sub = "2 sub";
		uorbc_platform_log("%s sub_count:%d", sub, sub_count);
		sub_count++;

		struct _que_msg {
			uint32_t num;
			struct sysinfo_s sysinfo[0];
		};
		struct _que_msg* q = NULL;
		uint32_t pointer;

		uint8_t update = 0;

		q = NULL;
		ret = uorbc_check_copy(ORB_ID(sysinfo), handle_sysinfo, &pointer, &update);
		uorbc_platform_log("ret:%d update:%d %s", ret, update, sub);
		q = pointer;
		if(update&&q) {
			struct sysinfo_s* p_sysinfo = &(q->sysinfo);
			for(int i=0; i<q->num; i++) {
				uorbc_platform_log("%s machine_number[%d] %s total %d msg, cur:%d",
				                   sub, p_sysinfo->machine_number, sub, q->num, i);
				p_sysinfo++;
			}
			free(q);
		}
	}
}

static void* uorbc_test_thread_sub3(void)
{
	uorbc_platform_log("uorbc_test_thread_sub3");

	int handle_sysinfo = orb_subscribe(ORB_ID(sysinfo));
	int ret = -1;

	uint32_t sub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(5500);
		const char* sub = "3 sub";
		uorbc_platform_log("%s sub_count:%d", sub, sub_count);
		sub_count++;

		struct _que_msg {
			uint32_t num;
			struct sysinfo_s sysinfo[0];
		};
		struct _que_msg* q = NULL;
		uint32_t pointer;

		uint8_t update = 0;

		q = NULL;
		ret = uorbc_check_copy(ORB_ID(sysinfo), handle_sysinfo, &pointer, &update);
		uorbc_platform_log("ret:%d update:%d %s", ret, update, sub);
		q = pointer;
		if(update&&q) {
			struct sysinfo_s* p_sysinfo = &(q->sysinfo);
			for(int i=0; i<q->num; i++) {
				uorbc_platform_log("%s machine_number[%d] %s total %d msg, cur:%d",
				                   sub, p_sysinfo->machine_number, sub, q->num, i);
				p_sysinfo++;
			}
			free(q);
		}
	}
}

static void* uorbc_test_thread_sub4(void)
{
	uorbc_platform_log("uorbc_test_thread_sub4");

	int handle_sysinfo = orb_subscribe(ORB_ID(sysinfo));
	int ret = -1;

	uint32_t sub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(5500);
		const char* sub = "4 sub";
		uorbc_platform_log("%s sub_count:%d", sub, sub_count);
		sub_count++;

		struct _que_msg {
			uint32_t num;
			struct sysinfo_s sysinfo[0];
		};
		struct _que_msg* q = NULL;
		uint32_t pointer;

		uint8_t update = 0;

		q = NULL;
		ret = uorbc_check_copy(ORB_ID(sysinfo), handle_sysinfo, &pointer, &update);
		uorbc_platform_log("ret:%d update:%d %s", ret, update, sub);
		q = pointer;
		if(update&&q) {
			struct sysinfo_s* p_sysinfo = &(q->sysinfo);
			for(int i=0; i<q->num; i++) {
				uorbc_platform_log("%s machine_number[%d] %s total %d msg, cur:%d",
				                   sub, p_sysinfo->machine_number, sub, q->num, i);
				p_sysinfo++;
			}
			free(q);
		}
	}
}

static void* uorbc_test_thread_sub5(void)
{
	uorbc_platform_log("uorbc_test_thread_sub5");

	int handle_sysinfo = orb_subscribe(ORB_ID(sysinfo));
	int ret = -1;

	uint32_t sub_count = 0;
	while(1) {
		uorbc_platform_delay_ms(5500);
		const char* sub = "5 sub";
		uorbc_platform_log("%s sub_count:%d", sub, sub_count);
		sub_count++;

		struct _que_msg {
			uint32_t num;
			struct sysinfo_s sysinfo[0];
		};
		struct _que_msg* q = NULL;
		uint32_t pointer;

		uint8_t update = 0;

		q = NULL;
		ret = uorbc_check_copy(ORB_ID(sysinfo), handle_sysinfo, &pointer, &update);
		uorbc_platform_log("ret:%d update:%d %s", ret, update, sub);
		q = pointer;
		if(update&&q) {
			struct sysinfo_s* p_sysinfo = &(q->sysinfo);
			for(int i=0; i<q->num; i++) {
				uorbc_platform_log("%s machine_number[%d] %s total %d msg, cur:%d",
				                   sub, p_sysinfo->machine_number, sub, q->num, i);
				p_sysinfo++;
			}
			free(q);
		}
	}
}

static void* uorbc_test_thread_sub6(void)
{
	uorbc_platform_log("uorbc_test_thread_sub6");

	int handle_sysinfo = orb_subscribe(ORB_ID(sysinfo));
	int ret = -1;
	uint32_t sub_count = 0;

	while(1) {
		uorbc_platform_delay_ms(5500);
		const char* sub = "6 sub";
		uorbc_platform_log("%s sub_count:%d", sub, sub_count);
		sub_count++;

		struct _que_msg {
			uint32_t num;
			struct sysinfo_s sysinfo[0];
		};
		struct _que_msg* q = NULL;
		uint32_t pointer;

		uint8_t update = 0;

		q = NULL;
		ret = uorbc_check_copy(ORB_ID(sysinfo), handle_sysinfo, &pointer, &update);
		uorbc_platform_log("ret:%d update:%d %s", ret, update, sub);
		q = pointer;
		if(update&&q) {
			struct sysinfo_s* p_sysinfo = &(q->sysinfo);
			for(int i=0; i<q->num; i++) {
				uorbc_platform_log("%s machine_number[%d] %s total %d msg, cur:%d",
				                   sub, p_sysinfo->machine_number, sub, q->num, i);
				p_sysinfo++;
			}
			free(q);
		}
	}
}

void mod_uorbc_test(void)
{
	uorbc_platform_log("\n\n\ntest uorbc 0c\n");
	uorbc_platform_delay_ms(3000);

	orb_init();

	struct sysinfo_s init_sysinfo = {
		.machine_type = "Dryer",
		.machine_number = 0,
		.mbn = 0,
		.net = 0
	};
	orb_advertise(ORB_ID(sysinfo), &init_sysinfo);


	uorbc_platform_pthread_t thread_pub = 0;
	uorbc_platform_pthread_t thread_pub1 = 0;
	uorbc_platform_pthread_t thread_pub2 = 0;
	uorbc_platform_pthread_t thread_pub3 = 0;
	uorbc_platform_pthread_t thread_pub4 = 0;
	uorbc_platform_pthread_t thread_pub5 = 0;
	uorbc_platform_pthread_t thread_pub6 = 0;
	mod_uorbc_platform_thread_create(&thread_pub, uorbc_test_thread_pub, NULL);
	mod_uorbc_platform_thread_create(&thread_pub1, uorbc_test_thread_pub1, NULL);
	mod_uorbc_platform_thread_create(&thread_pub2, uorbc_test_thread_pub2, NULL);
	mod_uorbc_platform_thread_create(&thread_pub3, uorbc_test_thread_pub3, NULL);
	mod_uorbc_platform_thread_create(&thread_pub4, uorbc_test_thread_pub4, NULL);
	mod_uorbc_platform_thread_create(&thread_pub5, uorbc_test_thread_pub5, NULL);
	mod_uorbc_platform_thread_create(&thread_pub6, uorbc_test_thread_pub6, NULL);

	uorbc_platform_pthread_t thread_sub = 0;
	uorbc_platform_pthread_t thread_sub1 = 0;
	uorbc_platform_pthread_t thread_sub2 = 0;
	uorbc_platform_pthread_t thread_sub3 = 0;
	uorbc_platform_pthread_t thread_sub4 = 0;
	uorbc_platform_pthread_t thread_sub5 = 0;
	uorbc_platform_pthread_t thread_sub6 = 0;
	mod_uorbc_platform_thread_create(&thread_sub, uorbc_test_thread_sub, NULL);
	mod_uorbc_platform_thread_create(&thread_sub1, uorbc_test_thread_sub1, NULL);
	mod_uorbc_platform_thread_create(&thread_sub2, uorbc_test_thread_sub2, NULL);
	mod_uorbc_platform_thread_create(&thread_sub3, uorbc_test_thread_sub3, NULL);
	mod_uorbc_platform_thread_create(&thread_sub4, uorbc_test_thread_sub4, NULL);
	mod_uorbc_platform_thread_create(&thread_sub5, uorbc_test_thread_sub5, NULL);
	mod_uorbc_platform_thread_create(&thread_sub6, uorbc_test_thread_sub6, NULL);
}


