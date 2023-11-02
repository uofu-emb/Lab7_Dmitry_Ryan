//LAB 7 - CAN Bus

#include <zephyr.h>
#include <kernel.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/can.h>
#include <drivers/gpio.h>
#include <sys/byteorder.h>
#include <stdio.h>

CAN_DEFINE_MSGQ(receive_q, 50);

//messsage
struct zcan_frame frame1 = 
        {
                .id_type = CAN_STANDARD_IDENTIFIER,
                .rtr = CAN_DATAFRAME,
                .id = 0x123,
                .dlc = 8,
                .data = {1,2,3,4,5,6,7,8}
				

        };

		struct zcan_frame frame2 = {
                .id_type = CAN_STANDARD_IDENTIFIER,
                .rtr = CAN_DATAFRAME,
                .id = 0x456,
                .dlc = 8,
                .data = {1,2,3}

        };

//filter
const struct zcan_filter my_filter =
 {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x123,
        .rtr_mask = 0,
        .id_mask = CAN_STD_ID_MASK
};

const struct device *can_dev;
int ret;
int filter_id1;

void send_message(void)
{
        ret = can_send(can_dev, &frame1, K_MSEC(100), NULL, NULL); // send message 1
        if (ret != CAN_TX_OK) {
                        printk("Sending failed [%d]", ret);
                }     

		 ret = can_send(can_dev, &frame2, K_MSEC(100), NULL, NULL); //send message 2
		 if (ret != CAN_TX_OK) {
                        printk("Sending failed [%d]", ret);
                }     

}



void rx_callback_function(struct zcan_frame *frame, void *arg)
{
        filter_id1 = can_attach_isr(can_dev, rx_callback_function, arg, &my_filter);
        if (filter_id1 < 0) {
                printk("Unable to attach isr [%d]", filter_id1);
        }
}

void rx_callback_functionq(struct k_msgq *message_q)
{
      //  filter_id1 = can_attach_isr(can_dev, rx_callback_function, arg, &my_filter);
        filter_id1 = can_attach_msgq(can_dev, &receive_q, &my_filter); //message queue
        if (filter_id1 < 0) {
                printk("Unable to attach isr [%d]", filter_id1);
        }
}

void main(void)
{
   //Activity 0
        can_dev = device_get_binding("CAN_1");
        //can_set_mode(can_dev, CAN_LOOPBACK_MODE);
		can_set_mode(can_dev, CAN_NORMAL_MODE);
        
        int message1;  //integer ID of message
        while(1)
        {
                send_message();
                message1 = k_msgq_get (&receive_q, &frame1, K_FOREVER); //check if babbling message is blocking low priority msg = nothing in queue
                //printk(message1);
                printf("recieved: %d", message1);
        }
        
        rx_callback_function(&frame1, NULL);
	rx_callback_function(&frame2, NULL);
        rx_callback_functionq(&receive_q);
        
        //printk("Print data[%d]", can_dev->data);
        
}
