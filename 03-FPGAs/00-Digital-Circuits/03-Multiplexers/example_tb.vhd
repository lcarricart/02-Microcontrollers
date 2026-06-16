library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity example_tb is
end example_tb;

architecture Behavioral of example_tb is

component example is
    Port ( sel  : in std_logic_vector (1 downto 0);
           din0 : in std_logic_vector (15 downto 0);
           din1 : in std_logic_vector (15 downto 0);
           dout : out std_logic_vector (15 downto 0));
end component;

signal din0_s, din1_s, dout_s : std_logic_vector (15 downto 0);
signal sel_s : std_logic_vector (1 downto 0);
constant DELAY : time := 10 ns;
constant OFFSET : time := 5 ns;

begin

i_example : example
    port map(
    sel => sel_s,
    din0 => din0_s,
    din1 => din1_s,
    dout => dout_s
    );

process
begin
    wait for OFFSET;
	
	-- SEL = 00 AND gate
	-- first stimuli set: 0 0
    din1_s <= x"0000";      -- Dummy testing values
    din0_s <= x"0000";      -- We could also assign din0_s = "000A"
    sel_s <= "00";         --                      din1_s = "FFF0"
    wait for DELAY;

	-- second stimuli set: 0 1
	din0_s <= x"FFFF";
	din1_s <= x"0000";
	sel_s <= "00";
    wait for DELAY;

	-- third stimuli set: 1 0
	din0_s <= x"0000";
	din1_s <= x"FFFF";
	sel_s <= "00";
    wait for DELAY;

	-- forth stimuli set: 1 1
	din0_s <= x"FFFF";
	din1_s <= x"FFFF";
	sel_s <= "00";
	
	-- SEL = 01 OR gate
    -- first stimuli set: 0 0
    din1_s <= x"0000";
    din0_s <= x"0000";
    sel_s <= "01";
    wait for DELAY;

	-- second stimuli set: 0 1
	din0_s <= x"FFFF";
	din1_s <= x"0000";
	sel_s <= "01";
    wait for DELAY;

	-- third stimuli set: 1 0
	din0_s <= x"0000";
	din1_s <= x"FFFF";
	sel_s <= "01";
    wait for DELAY;

	-- forth stimuli set: 1 1
	din0_s <= x"FFFF";
	din1_s <= x"FFFF";
	sel_s <= "01";
	
	-- SEL = 10 XOR gate
    -- first stimuli set: 0 0
    din1_s <= x"0000";
    din0_s <= x"0000";
    sel_s <= "10";
    wait for DELAY;

	-- second stimuli set: 0 1
	din0_s <= x"FFFF";
	din1_s <= x"0000";
	sel_s <= "10";
    wait for DELAY;

	-- third stimuli set: 1 0
	din0_s <= x"0000";
	din1_s <= x"FFFF";
	sel_s <= "10";
    wait for DELAY;

	-- forth stimuli set: 1 1
	din0_s <= x"FFFF";
	din1_s <= x"FFFF";
	sel_s <= "10";
	
	-- SEL = 11 XNOR gate
    -- first stimuli set: 0 0
    din1_s <= x"0000";
    din0_s <= x"0000";
    sel_s <= "10";
    wait for DELAY;

	-- second stimuli set: 0 1
	din0_s <= x"FFFF";
	din1_s <= x"0000";
	sel_s <= "11";
    wait for DELAY;

	-- third stimuli set: 1 0
	din0_s <= x"0000";
	din1_s <= x"FFFF";
	sel_s <= "11";
    wait for DELAY;

	-- forth stimuli set: 1 1
	din0_s <= x"FFFF";
	din1_s <= x"FFFF";
	sel_s <= "11";
     
    wait for DELAY;
	assert false report "example finished" severity error;
    wait;
	
end process;


end Behavioral;
