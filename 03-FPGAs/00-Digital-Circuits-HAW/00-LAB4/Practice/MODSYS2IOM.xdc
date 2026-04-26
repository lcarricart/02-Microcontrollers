##
## file name: MODSYS2IOM.xdc
## project: DI WT 2025
## date: 01-01-2026
## 
## description: Exercise 4 - MODSYS 2.0 template constraint file
##							 please adapt the port names according to 
##							 the VHDL description
##


##############
# mainboard
# Clock signal
#set_property PACKAGE_PIN P17 [get_ports {clk}]							
set_property PACKAGE_PIN N5 [get_ports {clk}]							
set_property IOSTANDARD LVCMOS33 [get_ports {clk}]
create_clock -add -name clk -period 100.00 -waveform {0 50} [get_ports {clk}]

# reset signal
set_property PACKAGE_PIN P5 [get_ports {rstn}]					
set_property IOSTANDARD LVCMOS33 [get_ports {rstn}]

##############
# Connector 3

# Buttons
set_property PACKAGE_PIN G16 [get_ports {debug0[0]}];
set_property PACKAGE_PIN H16 [get_ports {debug0[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {debug0[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {debug0[1]}];

# Switches
set_property PACKAGE_PIN G18 [get_ports {SW0[0]}];
set_property PACKAGE_PIN B11 [get_ports {SW0[1]}];
set_property PACKAGE_PIN A11 [get_ports {SW0[2]}];
set_property PACKAGE_PIN D15 [get_ports {SW0[3]}];
set_property PACKAGE_PIN C15 [get_ports {SW0[4]}];
set_property PACKAGE_PIN A13 [get_ports {SW0[5]}];
set_property PACKAGE_PIN A14 [get_ports {SW0[6]}];
set_property PACKAGE_PIN F15 [get_ports {SW0[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW0[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW0[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW0[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW0[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW0[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW0[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW0[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW0[7]}];

# LEDs
set_property PACKAGE_PIN H15 [get_ports {LED0[0]}];
set_property PACKAGE_PIN K13 [get_ports {LED0[1]}];
set_property PACKAGE_PIN J13 [get_ports {LED0[2]}];
set_property PACKAGE_PIN K15 [get_ports {LED0[3]}];
set_property PACKAGE_PIN J15 [get_ports {LED0[4]}];
set_property PACKAGE_PIN E16 [get_ports {LED0[5]}];
set_property PACKAGE_PIN E15 [get_ports {LED0[6]}];
set_property PACKAGE_PIN H17 [get_ports {LED0[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED0[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED0[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED0[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED0[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED0[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED0[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED0[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED0[7]}];


##############
# Connector 4 (not used)

# Switches
set_property PACKAGE_PIN R7 [get_ports {SW1[0]}];
set_property PACKAGE_PIN R5 [get_ports {SW1[1]}];
set_property PACKAGE_PIN R6 [get_ports {SW1[2]}];
set_property PACKAGE_PIN N4 [get_ports {SW1[3]}];
set_property PACKAGE_PIN M4 [get_ports {SW1[4]}];
set_property PACKAGE_PIN T3 [get_ports {SW1[5]}];
set_property PACKAGE_PIN R3 [get_ports {SW1[6]}];
set_property PACKAGE_PIN U7 [get_ports {SW1[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW1[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW1[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW1[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW1[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW1[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW1[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW1[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW1[7]}];

# LEDs
set_property PACKAGE_PIN U9 [get_ports {LED1[0]}];
set_property PACKAGE_PIN V5 [get_ports {LED1[1]}];
set_property PACKAGE_PIN V4 [get_ports {LED1[2]}];
set_property PACKAGE_PIN L4 [get_ports {LED1[3]}];
set_property PACKAGE_PIN K5 [get_ports {LED1[4]}];
set_property PACKAGE_PIN N6 [get_ports {LED1[5]}];
set_property PACKAGE_PIN M6 [get_ports {LED1[6]}];
set_property PACKAGE_PIN T8 [get_ports {LED1[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED1[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED1[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED1[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED1[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED1[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED1[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED1[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED1[7]}];
