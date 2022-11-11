/****************************************************************************
 *
 *   Copyright (c) 2018 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/


#include "mp2.hpp"

#include <px4_platform_common/getopt.h>
#include <px4_platform_common/px4_config.h>
#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>

#include <uORB/topics/parameter_update.h>
#include <uORB/topics/vehicle_local_position.h>
#include <uORB/topics/mission.h>
#include <math.h>



int MP2::print_status()
{
	PX4_INFO("Running");
	// TODO: print additional runtime information about the state of the module

	return 0;
}

int MP2::custom_command(int argc, char *argv[])
{
	/*
	if (!is_running()) {
		print_usage("not running");
		return 1;
	}

	// additional custom commands can be handled like this:
	if (!strcmp(argv[0], "do-something")) {
		get_instance()->do_something();
		return 0;
	}
	 */

	return print_usage("unknown command");
}


int MP2::task_spawn(int argc, char *argv[])
{
	_task_id = px4_task_spawn_cmd("mp2",
				      SCHED_DEFAULT, // SCHED_DEFAULT = SCHED_FIFO = 1
				      SCHED_PRIORITY_DEFAULT, // SCHED_PRIORITY_DEFAULT = 20
				      2048,
				      (px4_main_t)&run_trampoline,
				      (char *const *)argv);
	//PX4_INFO("MP2 Running at priority %d\n",SCHED_PRIORITY_DEFAULT);

	if (_task_id < 0) {
		_task_id = -1;
		return -errno;
	}

	return 0;
}

MP2 *MP2::instantiate(int argc, char *argv[])
{
	int example_param = 0;
	bool example_flag = false;
	bool error_flag = false;

	int myoptind = 1;
	int ch;
	const char *myoptarg = nullptr;

	// parse CLI arguments
	while ((ch = px4_getopt(argc, argv, "p:f", &myoptind, &myoptarg)) != EOF) {
		switch (ch) {
		case 'p':
			example_param = (int)strtol(myoptarg, nullptr, 10);
			break;

		case 'f':
			example_flag = true;
			break;

		case '?':
			error_flag = true;
			break;

		default:
			PX4_WARN("unrecognized flag");
			error_flag = true;
			break;
		}
	}

	if (error_flag) {
		return nullptr;
	}

	MP2 *instance = new MP2(example_param, example_flag);

	if (instance == nullptr) {
		PX4_ERR("alloc failed");
	}

	return instance;
}

MP2::MP2(int example_param, bool example_flag)
	: ModuleParams(nullptr)
{
	printf("Starting MP2 Module");
	mp2server = new Mp2Server();
}

void MP2::run()
{
	px4_usleep(500000);
	
	// TODO: Get subscription id 
	int SUB_ID = orb_subscribe(ORB_ID(vehicle_local_position_copy));
	px4_pollfd_struct_t fds[1];
	fds[0].fd = SUB_ID; // TODO: put the subscription id here
	fds[0].events = POLLIN;

	while (!should_exit()) {

		// wait for up to 1000ms for data
		int pret = px4_poll(fds, (sizeof(fds) / sizeof(fds[0])), 1000);

		if (pret == 0) {
			// Timeout: let the loop run anyway, don't do `continue` here

		} else if (pret < 0) {
			// this is undesirable but not much we can do
			PX4_ERR("poll error %d, %d", pret, errno);
			px4_usleep(50000);
			continue;

		} else if (fds[0].revents & POLLIN) {
			// TODO: Create struct to hold the message
			//
			struct vehicle_local_position_copy_s vlp;
			orb_copy(ORB_ID(vehicle_local_position_copy), SUB_ID, &vlp);
			// TODO: Copy the data to the struct
			//
			mp2server->exfiltrate(vlp);
			int offset[3];
			mp2server->recieve(offset);

			struct vehicle_local_position_copy_s vlp_pub;
			memcpy(&vlp, &vlp_pub, sizeof(vehicle_local_position_copy_s));

			orb_advert_t velpos_pub = orb_advertise(ORB_ID(vehicle_local_position_copy), &vlp_pub);
			vlp_pub.x += (float)offset[0];
			vlp_pub.y += (float)offset[1];
			vlp_pub.z += (float)offset[2];

			orb_publish(ORB_ID(vehicle_local_position_copy), velpos_pub, &vlp_pub);

			// TODO: Print the position data using PX4_INFO()
		}

		// parameters_update();
	}

	// TODO: Unsubscribe
	orb_unsubscribe(SUB_ID);
	
}


int MP2::print_usage(const char *reason)
{
	if (reason) {
		PX4_WARN("%s\n", reason);
	}

	PRINT_MODULE_DESCRIPTION(
		R"DESCR_STR(
### Description
Section that describes the provided module functionality.

This is a template for a module running as a task in the background with start/stop/status functionality.

### Implementation
Section describing the high-level implementation of this module.

### Examples
CLI usage example:
$ module start -f -p 42

)DESCR_STR");

	PRINT_MODULE_USAGE_NAME("module", "mp2");
	PRINT_MODULE_USAGE_COMMAND("start");
	PRINT_MODULE_USAGE_PARAM_FLAG('f', "Optional example flag", true);
	PRINT_MODULE_USAGE_PARAM_INT('p', 0, 0, 1000, "Optional example parameter", true);
	PRINT_MODULE_USAGE_DEFAULT_COMMANDS();

	return 0;
}

int mp2_main(int argc, char *argv[])
{
	PX4_INFO("Running MP2 Module");
	return MP2::main(argc, argv);
}
