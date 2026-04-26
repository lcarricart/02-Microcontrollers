library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity example is
    Port ( din0 : in std_logic_vector (15 downto 0);
           din1 : in std_logic_vector (15 downto 0);
           dout0 : out std_logic_vector (15 downto 0);
           dout1 : out std_logic_vector (15 downto 0);
           dout2 : out std_logic_vector (15 downto 0);
           dout3 : out std_logic_vector (15 downto 0));
end example;

architecture rtl of example is

begin

dout0 <= din0 AND din1;
dout1 <= din0 OR din1;
dout2 <= din0 XOR din1;
dout3 <= din0 XNOR din1;

end rtl;
