#include <iostream>
#include <unistd.h>
#define LV_CONF_INCLUDE_SIMPLE
#include <lvgl.h>
#include "lv_drivers/display/fbdev.h"
#include "lv_drivers/indev/evdev.h"

// display buffer size - not sure if this size is really needed
#define DISP_BUF_SIZE 384000 // 800x480
// #define DISP_BUF_SIZE (128 * 1024)

const int screenWidth = 800;
const int screenHeight = 480;

int main(int argc, char *argv[])
{
	/*LittlevGL init*/
	lv_init();

	/*Linux frame buffer device init*/
	fbdev_init();

	// Touch pointer device init
	evdev_init();

	/*A small buffer for LittlevGL to draw the screen's content*/
	static lv_color_t buf[DISP_BUF_SIZE];
	// static lv_color_t buf1[DISP_BUF_SIZE];

	/*Initialize a descriptor for the buffer*/
	static lv_disp_draw_buf_t disp_buf;
	lv_disp_draw_buf_init(&disp_buf, buf, NULL, DISP_BUF_SIZE);

	/*Initialize and register a display driver*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.draw_buf = &disp_buf;
	disp_drv.flush_cb = fbdev_flush;
	disp_drv.hor_res = screenWidth;
	disp_drv.ver_res = screenHeight;
	lv_disp_drv_register(&disp_drv);

	while (1)
	{
		lv_task_handler();
		usleep(1000);
	}
	return 0;
}

uint32_t custom_tick_get(void)
{
	struct timespec period;
	clock_gettime(CLOCK_MONOTONIC, &period);
	uint64_t ticks_ms = period.tv_sec * 1000 + period.tv_nsec / 1000000;

	static uint64_t start_ms = 0;
	if (start_ms == 0)
	{
		start_ms = ticks_ms;
	}

	uint32_t time_ms = ticks_ms - start_ms;
	return time_ms;
}