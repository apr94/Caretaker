#include <pebble_worker.h>

#define FALL_DOWN 1

// background workers to detect fall

static void tap_handler(AccelAxisType axis, int32_t direction) {

    // mark events of interest

    switch (axis) {
	case ACCEL_AXIS_Z:
	    if (direction < 0 || direction > 0) {
		AppWorkerMessage msg_data = {
		    .data0 = 1
		};
		// Send the data to the foreground app
		app_worker_send_message(FALL_DOWN, &msg_data);
	    }
	    break;
	case ACCEL_AXIS_Y:
	    if (direction < 0 || direction > 0) {
		AppWorkerMessage msg_data = {
		    .data0 = 1
		};
		// Send the data to the foreground app
		app_worker_send_message(FALL_DOWN, &msg_data);
	    }
	    break;
	case ACCEL_AXIS_X:
	    if (direction < 0 || direction > 0) {
		AppWorkerMessage msg_data = {
		    .data0 = 1
		};
		// Send the data to the foreground app
		app_worker_send_message(FALL_DOWN, &msg_data);
	    }
	    break;
	default:
	    break;
    }
}

static void worker_init() {
    accel_tap_service_subscribe(tap_handler);
}

static void worker_deinit() {
    accel_tap_service_unsubscribe();
}

int main(void) {
    worker_init();
    worker_event_loop();
    worker_deinit();
}
