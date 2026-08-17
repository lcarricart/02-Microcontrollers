Pins to be configured to bring-up a CAN application:
- FlexCAN instance 0:
	- CAN0_TX > PTA7
	- CAN0_RX > PTA6
	- CAN0_EN > PTC21 > HIGH needed for Normal mode
	- CAN0_STB > PTC20 > HIGH needed for Normal mode
	
- RGB LED (D3). They are input and output
	- RGB_REDLED_PTA29
	- RGB_GREENLED_PTA30
	- RGB_BLUELED_PTA31
	
In order to appropiately debug this application, add the following variables to the Watch Window:
- rx_errors
- tx_errors
- init
- config
- start
- tx_status
- rxFrame