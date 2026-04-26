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
set_property PACKAGE_PIN P17 [get_ports clk]							
set_property IOSTANDARD LVCMOS33 [get_ports clk]
create_clock -add -name clk -period 10.00 -waveform {0 5} [get_ports clk]

# reset signal
set_property PACKAGE_PIN P5 [get_ports {rst}]					
set_property IOSTANDARD LVCMOS33 [get_ports {rst}]

##############
# Connector 3

# Buttons
set_property PACKAGE_PIN G16 [get_ports {debug0}];
set_property PACKAGE_PIN H16 [get_ports {debug1}];
set_property IOSTANDARD LVCMOS33 [get_ports {debug0}];
set_property IOSTANDARD LVCMOS33 [get_ports {debug1}];

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
