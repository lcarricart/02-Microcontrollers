#####################################################
##
## [IE3-DI] Digital Circuits Winter Term 2025
##			Exercise 2
##
## @name:   MODSYS2IOM.xdc
## @author: 
## description: MODSYS 2.0 constraint file
##				TEMPLATE - please replace the given port names 
##				by the corresponding VHDL ports. 
##				Note: - Bitvectors must be split up into single bits
##						i.e. T[0] refers to the first bit of the vector "T"
##					  - All unassigned pins have to be commented by #
##
## (c) 2025 HAW Hamburg
##
#####################################################

##############
# Connector 3
# Buttons

set_property PACKAGE_PIN G16 [get_ports {T[0]}];
set_property PACKAGE_PIN H16 [get_ports {T[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {T[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {T[1]}];

# Switches
set_property PACKAGE_PIN G18 [get_ports {SW[0]}];
set_property PACKAGE_PIN B11 [get_ports {SW[1]}];
set_property PACKAGE_PIN A11 [get_ports {SW[2]}];
set_property PACKAGE_PIN D15 [get_ports {SW[3]}];
set_property PACKAGE_PIN C15 [get_ports {SW[4]}];
set_property PACKAGE_PIN A13 [get_ports {SW[5]}];
set_property PACKAGE_PIN A14 [get_ports {SW[6]}];
set_property PACKAGE_PIN F15 [get_ports {SW[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[7]}];

# LEDs
set_property PACKAGE_PIN H15 [get_ports {LED[0]}];
set_property PACKAGE_PIN K13 [get_ports {LED[1]}];
set_property PACKAGE_PIN J13 [get_ports {LED[2]}];
set_property PACKAGE_PIN K15 [get_ports {LED[3]}];
set_property PACKAGE_PIN J15 [get_ports {LED[4]}];
set_property PACKAGE_PIN E16 [get_ports {LED[5]}];
set_property PACKAGE_PIN E15 [get_ports {LED[6]}];
set_property PACKAGE_PIN H17 [get_ports {LED[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[7]}];

set_property PACKAGE_PIN D17 [get_ports {LED[8]}];
set_property PACKAGE_PIN C17 [get_ports {LED[9]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[8]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[9]}];


##############
# Connector 4
# Buttons

set_property PACKAGE_PIN R2 [get_ports {T[0]}];
set_property PACKAGE_PIN P2 [get_ports {T[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {T[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {T[1]}];

# Switches
set_property PACKAGE_PIN R7 [get_ports {SW[0]}];
set_property PACKAGE_PIN R5 [get_ports {SW[1]}];
set_property PACKAGE_PIN R6 [get_ports {SW[2]}];
set_property PACKAGE_PIN N4 [get_ports {SW[3]}];
set_property PACKAGE_PIN M4 [get_ports {SW[4]}];
set_property PACKAGE_PIN T3 [get_ports {SW[5]}];
set_property PACKAGE_PIN R3 [get_ports {SW[6]}];
set_property PACKAGE_PIN U7 [get_ports {SW[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {SW[7]}];

# LEDs
set_property PACKAGE_PIN U9 [get_ports {LED[0]}];
set_property PACKAGE_PIN V5 [get_ports {LED[1]}];
set_property PACKAGE_PIN V4 [get_ports {LED[2]}];
set_property PACKAGE_PIN L4 [get_ports {LED[3]}];
set_property PACKAGE_PIN K5 [get_ports {LED[4]}];
set_property PACKAGE_PIN N6 [get_ports {LED[5]}];
set_property PACKAGE_PIN M6 [get_ports {LED[6]}];
set_property PACKAGE_PIN T8 [get_ports {LED[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports LED[0]];
set_property IOSTANDARD LVCMOS33 [get_ports LED[1]];
set_property IOSTANDARD LVCMOS33 [get_ports LED[2]];
set_property IOSTANDARD LVCMOS33 [get_ports LED[3]];
set_property IOSTANDARD LVCMOS33 [get_ports LED[4]];
set_property IOSTANDARD LVCMOS33 [get_ports LED[5]];
set_property IOSTANDARD LVCMOS33 [get_ports LED[6]];
set_property IOSTANDARD LVCMOS33 [get_ports LED[7]];

set_property PACKAGE_PIN L5 [get_ports {LED[8]}];
set_property PACKAGE_PIN E18 [get_ports {LED[9]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[8]}];
set_property IOSTANDARD LVCMOS33 [get_ports {LED[9]}];

