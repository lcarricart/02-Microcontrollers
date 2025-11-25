library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity logic_unit_tb is
end logic_unit_tb;

architecture Behavioral of logic_unit_tb is

component example is
    Port ( din0  : in std_logic_vector(15 downto 0);
           din1  : in std_logic_vector(15 downto 0);
           dout0 : out std_logic_vector(15 downto 0);
           dout1 : out std_logic_vector(15 downto 0);
           dout2 : out std_logic_vector(15 downto 0);
           dout3 : out std_logic_vector(15 downto 0)
           );
end component;

signal din0_s, din1_s : STD_LOGIC_VECTOR(15 downto 0);
signal dout0_s, dout1_s, dout2_s, dout3_s : STD_LOGIC_VECTOR(15 downto 0);
constant DELAY : time := 10 ns;
constant OFFSET : time := 5 ns;

begin

i_logic_unit : logic_unit
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
    din0_s <= x"FFFF";
    wait for DELAY;

	-- second stimuli set: 0 1
	assert false report "example finished" severity error;
    wait;
	
end process;


end Behavioral;
