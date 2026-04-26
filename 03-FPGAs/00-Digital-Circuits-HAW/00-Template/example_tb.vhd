library IEEE;
use IEEE.STD_LOGIC_1164.ALL;


entity example_tb is
end example_tb;

architecture Behavioral of example_tb is

component example is
    Port ( a : in std_logic_vector(0 downto 0);
           b : in std_logic_vector(0 downto 0);
           ci : in std_logic_vector(0 downto 0);
           co : out std_logic_vector(0 downto 0);
           s : out std_logic_vector(0 downto 0));
end component;

signal a_s, b_s, ci_s, co_s, s_s : std_logic_vector(0 downto 0);
constant DELAY : time := 10 ns;
constant OFFSET : time := 5 ns;

begin

i_example : example
    port map(
    a => a_s,
    b => b_s,
    ci => ci_s,
    co => co_s,
    s => s_s
    );

process
begin
    wait for OFFSET;
	
	-- first stimuli set: 0 0 0
    ci_s <= "0";
    b_s  <= "0";
    a_s  <= "0";
    wait for DELAY;

    -- second stimuli set: 0 0 1
    ci_s <= "0";
    b_s  <= "0";
    a_s  <= "1";
    wait for DELAY;

    -- third stimuli set: 0 1 0
    ci_s <= "0";
    b_s  <= "1";
    a_s  <= "0";
    wait for DELAY;

    -- fourth stimuli set: 0 1 1
    ci_s <= "0";
    b_s  <= "1";
    a_s  <= "1";
    wait for DELAY;

    -- fifth stimuli set: 1 0 0
    ci_s <= "1";
    b_s  <= "0";
    a_s  <= "0";
    wait for DELAY;

    -- sixth stimuli set: 1 0 1
    ci_s <= "1";
    b_s  <= "0";
    a_s  <= "1";
    wait for DELAY;

    -- seventh stimuli set: 1 1 0
    ci_s <= "1";
    b_s  <= "1";
    a_s  <= "0";
    wait for DELAY;

    -- eighth stimuli set: 1 1 1
    ci_s <= "1";
    b_s  <= "1";
    a_s  <= "1";
     
    wait for DELAY;
	assert false report "example finished" severity error;
    wait;
	
end process;


end Behavioral;
