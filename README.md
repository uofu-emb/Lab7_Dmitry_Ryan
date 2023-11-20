# Dmitry & Ryan Lab-7
Networking + CAN Bus

-Activity 0 : Sending and Recieving Messages
By sending a single message periodically establishes the basic functionality
of the CAN bus system with a recieve function to indicate message is received.

Observed: Single message is sent to CAN bus periodically. If Red LED is on;
          the message is received. If Red LED is off, message is not received.
          Red LED indicates receive callback for no filter is utilized.

-Activity 1
Creating system call to send 2 messages to the CAN bus and establishing 
receive call for filtering incoming message to match a specific filter.

Observed: Two messages are sent to CAN bus periodically. If Orange LED is on;
          the message is received with match to the filter. If Orange LED is 
          off, message received does not match the filter.
          Orange LED indicates receive callback for single filter 1 is utilized.

-ACtivity 2
Sending two messages on the bus line one with a high priority and one with a 
low priority using 2 stm32 MCU boards both running the same code.

Observed: When 2 messages are sent to the bus line using two microcontrollers
          with their respective CAN bus transceivers to both send two messages
          the controllers take turns sending the messages depending if the bus
          is not in use with sending a message or if the bus is clear. The 
          CAN signals observed show turns of sending a long message and a short
          message with some time in between sending the messages. Also there is
          sometimes a collision between sending two messages from the different
          MCUs but it is resolved as each one stops sending and allows one or
          the other to send a message first. Both low and high priority messages
          are able to be sent to the shared bus lines.

-Activity 3
Sending two messages on the bus line one with a high priority and one with a 
low priority using a 2 different microcontroller boards and CANBus transcievers.
One MCU sending the high priority message while another MCU sends the low 
priority message with addition of using a receiver with a worker queue.

Observed: 1 MCU was set to send a high priority message periodically while a 2nd
          MCU was set to set a low priority message and receive the messages using
          a queue. It was observed that the Bus line would stop allowing messages 
          to be sent on the high line. The culprit of this was the low priority
          sender/MCU. Due to how it interrrupts the high priority sender/MCU to 
          not allow any messages to be sent on the bus line.
