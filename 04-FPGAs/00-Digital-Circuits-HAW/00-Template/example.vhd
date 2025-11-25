library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity example is
    Port ( a : in std_logic_vector(0 downto 0);
           b : in std_logic_vector(0 downto 0);
           ci : in std_logic_vector(0 downto 0);
           co : out std_logic_vector(0 downto 0);
           s : out std_logic_vector(0 downto 0));
end example;

architecture rtl of example is

begin

-- This variable can be easily represented by XOR means
s <= a XOR b XOR ci;
-- This variable can't and therefore requires a sum-of-products representation
co <= (a AND b AND (NOT ci)) OR (a AND (NOT b) AND ci) OR ((NOT a) AND b AND ci) OR (a AND b AND ci);

end rtl;
