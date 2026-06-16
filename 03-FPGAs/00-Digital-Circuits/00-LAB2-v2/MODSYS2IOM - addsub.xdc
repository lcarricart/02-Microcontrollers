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

set_property PACKAGE_PIN G16 [get_ports {sel}];
set_property PACKAGE_PIN H16 [get_ports {T[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {sel}];
set_property IOSTANDARD LVCMOS33 [get_ports {T[1]}];

# Switches
set_property PACKAGE_PIN G18 [get_ports {A[0]}];
set_property PACKAGE_PIN B11 [get_ports {A[1]}];
set_property PACKAGE_PIN A11 [get_ports {A[2]}];
set_property PACKAGE_PIN D15 [get_ports {A[3]}];
set_property PACKAGE_PIN C15 [get_ports {A[4]}];
set_property PACKAGE_PIN A13 [get_ports {A[5]}];
set_property PACKAGE_PIN A14 [get_ports {A[6]}];
set_property PACKAGE_PIN F15 [get_ports {A[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {A[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {A[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {A[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {A[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {A[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {A[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {A[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {A[7]}];

# LEDs
set_property PACKAGE_PIN H15 [get_ports {S[0]}];
set_property PACKAGE_PIN K13 [get_ports {S[1]}];
set_property PACKAGE_PIN J13 [get_ports {S[2]}];
set_property PACKAGE_PIN K15 [get_ports {S[3]}];
set_property PACKAGE_PIN J15 [get_ports {S[4]}];
set_property PACKAGE_PIN E16 [get_ports {S[5]}];
set_property PACKAGE_PIN E15 [get_ports {S[6]}];
set_property PACKAGE_PIN H17 [get_ports {S[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {S[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {S[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {S[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {S[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {S[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {S[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {S[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {S[7]}];

set_property PACKAGE_PIN D17 [get_ports {C}];
set_property PACKAGE_PIN C17 [get_ports {V}];
set_property IOSTANDARD LVCMOS33 [get_ports {C}];
set_property IOSTANDARD LVCMOS33 [get_ports {V}];


##############
# Connector 4
# Buttons

set_property PACKAGE_PIN R2 [get_ports {T[0]}];
set_property PACKAGE_PIN P2 [get_ports {T[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {T[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {T[1]}];

# Switches
set_property PACKAGE_PIN R7 [get_ports {B[0]}];
set_property PACKAGE_PIN R5 [get_ports {B[1]}];
set_property PACKAGE_PIN R6 [get_ports {B[2]}];
set_property PACKAGE_PIN N4 [get_ports {B[3]}];
set_property PACKAGE_PIN M4 [get_ports {B[4]}];
set_property PACKAGE_PIN T3 [get_ports {B[5]}];
set_property PACKAGE_PIN R3 [get_ports {B[6]}];
set_property PACKAGE_PIN U7 [get_ports {B[7]}];
set_property IOSTANDARD LVCMOS33 [get_ports {B[0]}];
set_property IOSTANDARD LVCMOS33 [get_ports {B[1]}];
set_property IOSTANDARD LVCMOS33 [get_ports {B[2]}];
set_property IOSTANDARD LVCMOS33 [get_ports {B[3]}];
set_property IOSTANDARD LVCMOS33 [get_ports {B[4]}];
set_property IOSTANDARD LVCMOS33 [get_ports {B[5]}];
set_property IOSTANDARD LVCMOS33 [get_ports {B[6]}];
set_property IOSTANDARD LVCMOS33 [get_ports {B[7]}];

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

set_property PACKAGE_PIN L5 [get_ports {N}];
set_property PACKAGE_PIN E18 [get_ports {Z}];
set_property IOSTANDARD LVCMOS33 [get_ports {N}];
set_property IOSTANDARD LVCMOS33 [get_ports {Z}];

