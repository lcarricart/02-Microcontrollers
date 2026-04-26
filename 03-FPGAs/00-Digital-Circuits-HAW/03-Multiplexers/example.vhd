library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity example is
    Port ( sel  : in std_logic_vector (1 downto 0);
           din0 : in std_logic_vector (15 downto 0);
           din1 : in std_logic_vector (15 downto 0);
           dout : out std_logic_vector (15 downto 0));
end example;

architecture rtl of example is

begin

dout <= din0 AND din1 when sel = "00" else
        din0 OR din1 when sel = "01" else
        din0 XOR din1 when sel = "10" else
        din0 XNOR din1;

end rtl;
