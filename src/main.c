//LAB 7 - CAN Bus

#include <zephyr.h>
#include <kernel.h>
#include <sys/printk.h>
#include <device.h>
#include <drivers/can.h>
#include <drivers/gpio.h>
#include <sys/byteorder.h>
#include <stdio.h>


#define LED0_NODE DT_ALIAS(led0)
#define LED0	DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN0	DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS0	DT_GPIO_FLAGS(LED0_NODE, gpios)

#define LED1_NODE DT_ALIAS(led1)
#define LED1	DT_GPIO_LABEL(LED1_NODE, gpios)
#define PIN1	DT_GPIO_PIN(LED1_NODE, gpios)
#define FLAGS1	DT_GPIO_FLAGS(LED1_NODE, gpios)

#define LED2_NODE DT_ALIAS(led2)
#define LED2	DT_GPIO_LABEL(LED2_NODE, gpios)
#define PIN2	DT_GPIO_PIN(LED2_NODE, gpios)
#define FLAGS2	DT_GPIO_FLAGS(LED2_NODE, gpios)

#define LED3_NODE DT_ALIAS(led3)
#define LED3	DT_GPIO_LABEL(LED3_NODE, gpios)
#define PIN3	DT_GPIO_PIN(LED3_NODE, gpios)
#define FLAGS3	DT_GPIO_FLAGS(LED3_NODE, gpios)

CAN_DEFINE_MSGQ(receive_q, 50);

//message -1
struct zcan_frame frame1 = 
        {
                .id_type = CAN_STANDARD_IDENTIFIER,
                .rtr = CAN_DATAFRAME,
                .id = 0x123,
                .dlc = 8,
                .data = {1,2,3,4,5,6,7,8}
        };
//message -2
struct zcan_frame frame2 = 
        {
                .id_type = CAN_STANDARD_IDENTIFIER,
                .rtr = CAN_DATAFRAME,
                .id = 0x456,
                .dlc = 8,
                .data = {1,2,3}

        };

//filter-id=0x123
const struct zcan_filter single_filter_1 =
 {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x123,
        .rtr_mask = 1,
        .id_mask = CAN_STD_ID_MASK
};

//filter-id=0x456
const struct zcan_filter single_filter_2 =
 {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x456,
        .rtr_mask = 1,
        .id_mask = CAN_STD_ID_MASK
};

//filter-mask =0 (essentially ignores the id)
const struct zcan_filter all_filter =
 {
        .id_type = CAN_STANDARD_IDENTIFIER,
        .rtr = CAN_DATAFRAME,
        .id = 0x123,
        .rtr_mask = 0,
        .id_mask = CAN_STD_ID_MASK
};


//CAN messages
const struct device *can_dev;
int ret;
int ret1;
int filter_id1;
int filter_id2;

//LED
const struct device *led_dev;
const struct device *led_dev2;
const struct device *led_dev3;
const struct device *led_dev4;
bool led_status;


//send a single message to bus
void send_message(struct zcan_frame *frame)
{
        ret = can_send(can_dev, frame, K_MSEC(150), NULL, NULL); // send message 1
        if (ret != CAN_TX_OK) {
                printk("Sending failed [%d]", ret);
                }         
}

//send 2 messages to bus
void send_message_2(void)
{
        ret = can_send(can_dev, &frame1, K_MSEC(100), NULL, NULL); // send message 1
        if (ret != CAN_TX_OK) {
                printk("Sending failed [%d]", ret);
                }     
        k_sleep(K_MSEC(250)); //wait before sending again

        ret1 = can_send(can_dev, &frame2, K_MSEC(100), NULL, NULL); //send message 2
        if (ret1 != CAN_TX_OK) {
                printk("Sending failed [%d]", ret);
                }     

}

//recieves all messages using filter with ID mask = 0
void rx_callback_function_no_filter(struct zcan_frame *frame, void *arg)
{       
        //turn on Red LED when messages are recived using this callback function
        gpio_pin_set(led_dev, PIN0, (int)true);
}

//recieves 1 message using filter for message id and blink LED if received
void rx_callback_function_single_filter(struct zcan_frame *frame, void *arg)
{
        //turn on Orange LED when message received matches filter criteria
        gpio_pin_set(led_dev2, PIN1, (int)true);
}


//receives messages using a message queue for all messages
void rx_callback_functionq(struct k_msgq *message_q) //Activity 3
{
        //turn on  LED when message received matches filter criteria
        gpio_pin_set(led_dev3, PIN2, (int)true);
}


//Receiver for worker queue
void receiver_q(struct zcan_filter filter)
{
        struct zcan_frame message;
        int filter_id = can_attach_msgq(can_dev, &receive_q, &filter);
        while(1)
        {
                k_msgq_get(&receive_q, &message, K_FOREVER);

                if (message.dlc != 2U) {
			printk("Wrong data length: %u\n", message.dlc);
			continue;
		}
                //turn on  LED when message received matches filter criteria
                gpio_pin_set(led_dev4, PIN3, (int)true);
        }
}

//Setup LED for recieve
void led_setup()
{
    led_dev = device_get_binding(LED0);
    led_dev2 = device_get_binding(LED1);
    led_dev3 = device_get_binding(LED2);
    led_dev4 = device_get_binding(LED2);
    led_status = false;
    gpio_pin_configure(led_dev, PIN0, GPIO_OUTPUT_INACTIVE| FLAGS0);
    gpio_pin_configure(led_dev2, PIN1, GPIO_OUTPUT_INACTIVE | FLAGS1);
    gpio_pin_configure(led_dev3, PIN2, GPIO_OUTPUT_INACTIVE | FLAGS2);
    gpio_pin_configure(led_dev4, PIN3, GPIO_OUTPUT_INACTIVE | FLAGS3);

}


//main run
void main(void)
{
        //Activity 0
        can_dev = device_get_binding("CAN_1");
        //can_set_mode(can_dev, CAN_LOOPBACK_MODE); //-Activity 0 + 1
        can_set_mode(can_dev, CAN_NORMAL_MODE); //Activity 2 + 3

        led_setup();
        
        int message1;  //integer ID of message -Activity 3
        
        //filter_id1 = can_attach_isr(can_dev, (can_rx_callback_t)  rx_callback_function_no_filter, NULL, &all_filter); //Activity 0
        
        filter_id2 = can_attach_isr(can_dev, (can_rx_callback_t)  rx_callback_function_single_filter, NULL, &single_filter_1); //id=0x123

        while(1)
        {
                //send_message(&frame1); //Activity 0
                //send_message_2(); //ACtivity 1

                //Activity 2: Two boards both sending 2 messages but filtering for one message
                //send_message_2();

                //Activity 3-receive queue
                //send_message(&frame1); //send message-high priority
                send_message(&frame2); //send message-low priority
                receiver_q(all_filter);

                //printf("recieved: %d", message1);

        }

}
