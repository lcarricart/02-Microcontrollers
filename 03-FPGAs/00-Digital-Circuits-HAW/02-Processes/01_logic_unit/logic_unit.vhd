library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

entity logic_unit is
    Port ( din0  : in std_logic_vector(15 downto 0);
           din1  : in std_logic_vector(15 downto 0);
           dout0 : out std_logic_vector(15 downto 0);
           dout1 : out std_logic_vector(15 downto 0);
           dout2 : out std_logic_vector(15 downto 0);
           dout3 : out std_logic_vector(15 downto 0)
           );
end logic_unit;

architecture rtl of logic_unit is

begin

-- Defining all this for the same simple purpose is trading design time for error minimization
comb : process(din0, din1)
    variable din0_v, din1_v : std_logic_vector(15 downto 0);
    variable dout0_v, dout1_v, dout2_v, dout3_v : std_logic_vector(15 downto 0);
    begin
        -- 1 variable assignment
        din0_v := din0;
        din1_v := din1;
        -- 2 data processing
        dout0_v := din0_v AND din1_v;
        dout1_v := din0_v OR din1_v;
        dout2_v := din0_v XOR din1_v;
        dout3_v := din0_v XNOR din1_v;
        -- 3 process output
        dout0 <= dout0_v;
        dout1 <= dout1_v;
        dout2 <= dout2_v;
        dout3 <= dout3_v;
    end process;

end rtl;
