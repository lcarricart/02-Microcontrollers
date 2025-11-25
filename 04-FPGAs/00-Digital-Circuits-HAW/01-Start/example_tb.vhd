library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity example_tb is
end example_tb;

architecture Behavioral of example_tb is

component example is
    Port ( din0 : in std_logic_vector (15 downto 0);
           din1 : in std_logic_vector (15 downto 0);
           dout0 : out std_logic_vector (15 downto 0);
           dout1 : out std_logic_vector (15 downto 0);
           dout2 : out std_logic_vector (15 downto 0);
           dout3 : out std_logic_vector (15 downto 0));
end component;

signal din0_s, din1_s, dout0_s, dout1_s, dout2_s, dout3_s : std_logic_vector (15 downto 0);
constant DELAY : time := 10 ns;
constant OFFSET : time := 5 ns;

begin

i_example : example
    port map(
    din0 => din0_s,
    din1 => din1_s,
    dout0 => dout0_s,
    dout1 => dout1_s,
    dout2 => dout2_s,
    dout3 => dout3_s
    );

process
begin
    wait for OFFSET;
	
	-- first stimuli set: 0 0
    din1_s <= x"0000";
    din0_s <= x"0000";
    wait for DELAY;

	-- second stimuli set: 0 1
	din0_s <= x"FFFF";
	din1_s <= x"0000";
    wait for DELAY;

	-- third stimuli set: 1 0
	din0_s <= x"0000";
	din1_s <= x"FFFF";
    wait for DELAY;

	-- forth stimuli set: 1 1
	din0_s <= x"FFFF";
	din1_s <= x"FFFF";
     
    wait for DELAY;
	assert false report "example finished" severity error;
    wait;
	
end process;


end Behavioral;
