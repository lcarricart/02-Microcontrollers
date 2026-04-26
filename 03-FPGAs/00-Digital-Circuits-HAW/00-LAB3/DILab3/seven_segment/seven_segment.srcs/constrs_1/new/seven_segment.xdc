#####################################################
##
## [IE3-DI] Digital Circuits Winter Term 2025
##			Exercise 3
##
## @name:   seven_segment.xdc
## @author: 
## description: basys 3 constraint file for Exercise 3
## 				no modifications necessary
##
## (c) 2025 HAW Hamburg
##
#####################################################

# Clock signal
set_property PACKAGE_PIN W5 [get_ports clk]							
set_property IOSTANDARD LVCMOS33 [get_ports clk]
create_clock -add -name clk -period 10.00 -waveform {0 5} [get_ports clk]

# reset signal
set_property PACKAGE_PIN R2 [get_ports {rst}]					
set_property IOSTANDARD LVCMOS33 [get_ports {rst}]

#7 LEDs display
set_property PACKAGE_PIN W7 [get_ports {LEDs[0]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {LEDs[0]}]
set_property PACKAGE_PIN W6 [get_ports {LEDs[1]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {LEDs[1]}]
set_property PACKAGE_PIN U8 [get_ports {LEDs[2]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {LEDs[2]}]
set_property PACKAGE_PIN V8 [get_ports {LEDs[3]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {LEDs[3]}]
set_property PACKAGE_PIN U5 [get_ports {LEDs[4]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {LEDs[4]}]
set_property PACKAGE_PIN V5 [get_ports {LEDs[5]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {LEDs[5]}]
set_property PACKAGE_PIN U7 [get_ports {LEDs[6]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {LEDs[6]}]
set_property PACKAGE_PIN V7 [get_ports {LEDs[7]}]							
	set_property IOSTANDARD LVCMOS33 [get_ports {LEDs[7]}]

## Digit select	
set_property PACKAGE_PIN U2 [get_ports {DIGIT_select[0]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {DIGIT_select[0]}]
set_property PACKAGE_PIN U4 [get_ports {DIGIT_select[1]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {DIGIT_select[1]}]
set_property PACKAGE_PIN V4 [get_ports {DIGIT_select[2]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {DIGIT_select[2]}]
set_property PACKAGE_PIN W4 [get_ports {DIGIT_select[3]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {DIGIT_select[3]}]

# Switches
set_property PACKAGE_PIN V17 [get_ports {SW[0]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {SW[0]}]
set_property PACKAGE_PIN V16 [get_ports {SW[1]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {SW[1]}]
set_property PACKAGE_PIN W16 [get_ports {SW[2]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {SW[2]}]
set_property PACKAGE_PIN W17 [get_ports {SW[3]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {SW[3]}]	
set_property PACKAGE_PIN W15 [get_ports {SW[4]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {SW[4]}]
set_property PACKAGE_PIN V15 [get_ports {SW[5]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {SW[5]}]
set_property PACKAGE_PIN W14 [get_ports {SW[6]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {SW[6]}]
set_property PACKAGE_PIN W13 [get_ports {SW[7]}]					
	set_property IOSTANDARD LVCMOS33 [get_ports {SW[7]}]
