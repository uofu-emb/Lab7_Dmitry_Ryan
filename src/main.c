#include <zephyr.h>
#include <kernel.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/can.h>
#include <drivers/gpio.h>
#include <sys/byteorder.h>


const struct device *can_dev;

can_dev = device_get_binding("CAN_1");
can_set_mode(can_dev, CAN_LOOPBACK_MODE);
