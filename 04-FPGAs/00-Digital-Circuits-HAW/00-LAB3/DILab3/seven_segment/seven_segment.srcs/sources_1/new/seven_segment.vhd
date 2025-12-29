------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 3
--
-- @name:   seven_segment.vhd
-- @author: 
-- @description: control unit for the seven segment display of the 
--               basys 3 board
--				 DESIGN FILE
--
-- (c) 2025 HAW Hamburg
--
------------------------------------------------------------------

---------------------------------------------
-- libraries
library ieee;
use ieee.std_logic_1164.all;
use IEEE.NUMERIC_STD.all;

---------------------------------------------
-- entity
entity seven_segment is
    Port ( clk : in STD_LOGIC;
           rst : in STD_LOGIC;
           
           LEDs : out STD_LOGIC_VECTOR(7 downto 0);
           DIGIT_select : out STD_LOGIC_VECTOR(3 downto 0);
           SW : in STD_LOGIC_VECTOR(7 downto 0)     -- Switch (they don't seem like enough switches but I can use them as binary numbers)      
           );
end seven_segment;


---------------------------------------------
-- architecture
architecture rtl of seven_segment is


---------------------------------------------
-- signal declaration

-- Preparation: Update the bit patterns of the constants for the 7-segment LEDs. Follows the order dp, g, f, e, d, c, b, a
constant ZERO  : std_logic_vector(7 downto 0) := "11000000";
constant ONE   : std_logic_vector(7 downto 0) := "11111001";
constant TWO   : std_logic_vector(7 downto 0) := "10100100";
constant THREE : std_logic_vector(7 downto 0) := "10110000";
constant FOUR  : std_logic_vector(7 downto 0) := "10011001";
constant FIVE  : std_logic_vector(7 downto 0) := "10010010";
constant SIX   : std_logic_vector(7 downto 0) := "10000010";
constant SEVEN : std_logic_vector(7 downto 0) := "11111000";
constant EIGHT : std_logic_vector(7 downto 0) := "10000000";
constant NINE  : std_logic_vector(7 downto 0) := "10010000";

-- Define my counter signal here
-- I can define intermediate variables for DIGIT_select temp, LEDs temp. At the end, I modify the actual signals that are mapped as "out" in the port declaration
signal counter_cs, counter_ns : std_logic_vector(29 downto 0);

begin

---------------------------------
-- sequential process
sync : process(clk, rst)
begin

if rst = '1' then
    counter_cs <= (others => '0');
elsif rising_edge(clk) then
    counter_cs <= counter_ns;
end if;

end process;

---------------------------------
-- combinatorial process delta. Initial set / data preparation / preprocessing. Decoding input values.
-- Based on the switches input (binary), choose which LED pattern do we wanna use or which digit do we wanna turn.
-- The switch directly acts as an input without making my code wait for changes in the switch, it happens automatically
-- Output doesnt need a separate variable, I can directly assign the output variable  with my calculation
delta : process(counter_cs, SW)
variable counter_v: std_logic_vector(29 downto 0);
begin
    counter_v := counter_cs;

    if (counter_cs(29 downto 26) = "1010") OR (unsigned(counter_cs(29 downto 26)) > unsigned(SW(3 downto 0))) then 
        counter_v := (others => '0');
    else
        counter_v := std_logic_vector(unsigned(counter_v) + 1);
    end if;
    
    counter_ns <= counter_v;
end process;


---------------------------------
-- combinatorial process lambda
-- Depending on my saved DIGIT_select and LEDs pattern, I want to output the correct thing
-- Once assigning a value to the output signal, the LEDs will light up apparently
lambda : process(counter_cs, SW)
begin
    DIGIT_select <= "0000";
    
    case SW (3 downto 0) is
        when "0000"  => LEDs <= ZERO;
        when "0001" =>
            case counter_cs (29 downto 26) is
                when "0000" => LEDs <= ZERO;
                when "0001" => LEDs <= ONE;
                when others => LEDs <= ZERO;
            end case;
        when "0010" =>
            case counter_cs (29 downto 26) is
                when "0000" => LEDs <= ZERO;
                when "0001" => LEDs <= ONE;
                when "0010" => LEDs <= TWO;
                when others => LEDs <= ZERO;
            end case;
        when "0011" =>
            case counter_cs (29 downto 26) is
                when "0000" => LEDs <= ZERO;
                when "0001" => LEDs <= ONE;
                when "0010" => LEDs <= TWO;
                when "0011" => LEDs <= THREE;
                when others => LEDs <= ZERO;
            end case;   
        when "0100" =>
            case counter_cs (29 downto 26) is
                when "0000" => LEDs <= ZERO;
                when "0001" => LEDs <= ONE;
                when "0010" => LEDs <= TWO;
                when "0011" => LEDs <= THREE;
                when "0100" => LEDs <= FOUR;
                when others => LEDs <= ZERO;
            end case;
        when "0101" =>
            case counter_cs (29 downto 26) is
                when "0000" => LEDs <= ZERO;
                when "0001" => LEDs <= ONE;
                when "0010" => LEDs <= TWO;
                when "0011" => LEDs <= THREE;
                when "0100" => LEDs <= FOUR;
                when "0101" => LEDs <= FIVE;
                when others => LEDs <= ZERO;
            end case;
        when "0110" =>
            case counter_cs (29 downto 26) is
                when "0000" => LEDs <= ZERO;
                when "0001" => LEDs <= ONE;
                when "0010" => LEDs <= TWO;
                when "0011" => LEDs <= THREE;
                when "0100" => LEDs <= FOUR;
                when "0101" => LEDs <= FIVE;
                when "0110" => LEDs <= SIX;
                when others => LEDs <= ZERO;
            end case;
        when "0111" =>
            case counter_cs (29 downto 26) is
                when "0000" => LEDs <= ZERO;
                when "0001" => LEDs <= ONE;
                when "0010" => LEDs <= TWO;
                when "0011" => LEDs <= THREE;
                when "0100" => LEDs <= FOUR;
                when "0101" => LEDs <= FIVE;
                when "0110" => LEDs <= SIX;
                when "0111" => LEDs <= SEVEN;
                when others => LEDs <= ZERO;
            end case;
        when "1000" =>
            case counter_cs (29 downto 26) is
                when "0000" => LEDs <= ZERO;
                when "0001" => LEDs <= ONE;
                when "0010" => LEDs <= TWO;
                when "0011" => LEDs <= THREE;
                when "0100" => LEDs <= FOUR;
                when "0101" => LEDs <= FIVE;
                when "0110" => LEDs <= SIX;
                when "0111" => LEDs <= SEVEN;
                when "1000" => LEDs <= EIGHT;
                when others => LEDs <= ZERO;
            end case;
        when  "1001" =>
            case counter_cs (29 downto 26) is
                when "0000" => LEDs <= ZERO;
                when "0001" => LEDs <= ONE;
                when "0010" => LEDs <= TWO;
                when "0011" => LEDs <= THREE;
                when "0100" => LEDs <= FOUR;
                when "0101" => LEDs <= FIVE;
                when "0110" => LEDs <= SIX;
                when "0111" => LEDs <= SEVEN;
                when "1000" => LEDs <= EIGHT;
                when "1001" => LEDs <= NINE;
                when others => LEDs <= ZERO;
            end case;
        when others => LEDs <= ZERO;
    end case;
        
end process;
end rtl;
