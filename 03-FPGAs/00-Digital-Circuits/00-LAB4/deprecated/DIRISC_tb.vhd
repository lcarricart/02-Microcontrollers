------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 4
--
-- @name:   DIRISC_tb.vhd
-- @author: 
-- @description: DIRISC programmable architecture
--				 TESTBENCH
--
-- (c) 2025 HAW Hamburg
--
------------------------------------------------------------------

---------------------------------
-- libraries / packages 
library IEEE;
use IEEE.std_logic_1164.all;

---------------------------------
-- entity
entity RISC_tb is
end entity;

---------------------------------
-- architecture
architecture behavioral of RISC_tb is

-- signal/constant declarations
signal clk		: std_logic :='0';
signal rst      : std_logic;
signal SW0_s, SW1_s	  : std_logic_vector(7 downto 0);
signal LED0_s, LED1_s : std_logic_vector(7 downto 0);

constant PERIOD		: time := 100 ns;
constant OFFSET		: time := 20 ns;

---------------------------------
-- component declaration for device under test (DUT)
component RISC_top
    port(
    clk : in std_logic;
    rst : in std_logic;
    SW0  : in std_logic_vector(7 downto 0);  
    SW1  : in std_logic_vector(7 downto 0);  
    LED0  : out std_logic_vector(7 downto 0);  
    LED1  : out std_logic_vector(7 downto 0)
    );  
end component;

begin
---------------------------------
-- simulation cycle
clk <= not clk after PERIOD/2;

---------------------------------
-- device under test (DUT) instantiation
i_RISC_top : RISC_top
	port map(
	   clk => clk,
	   rst => rst,
	   SW0 => SW0_s, 	
	   SW1 => SW1_s, 	
	   LED0 => LED0_s, 	
	   LED1 => LED1_s 	
	);


---------------------------------
-- stimuli process
stimuli: process
begin
   ---------------------------------
    rst <= '1';
   -- wait for some initialization time
	wait for PERIOD*3; 
	wait for OFFSET; 
    rst <= '0';
    
	-- The machine code in the program memory is processed automatially
	-- Nothing more to do in the testbench 
    wait;
end process;

end behavioral;
