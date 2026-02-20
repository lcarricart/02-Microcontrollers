------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 4
--
-- @name:   RF.vhd
-- @author: 
-- @description: DIRISC register file
--				 DESIGN FILE
--
-- (c) 2025 HAW Hamburg
--
------------------------------------------------------------------

---------------------------------
-- libraries / packages 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;
use work.global.all;

---------------------------------
-- entity
entity RF is
    Port ( clk : in STD_LOGIC;
           rst : in STD_LOGIC;
           
           opgroup : in STD_LOGIC_VECTOR(1 downto 0);
           src0 : in STD_LOGIC_VECTOR(2 downto 0);
           src1 : in STD_LOGIC_VECTOR(2 downto 0);
           dst : in STD_LOGIC_VECTOR(2 downto 0); 

           data_dst : in STD_LOGIC_VECTOR(15 downto 0);
          
           data_src0 : out STD_LOGIC_VECTOR (15 downto 0);
           data_src1 : out STD_LOGIC_VECTOR (15 downto 0) 
           );
end RF;

---------------------------------
-- architecture
architecture rtl of RF is

-- type declaration
type rf_t is array (7 downto 0) of std_logic_vector(15 downto 0); 
-- signal declaration
signal rf_cs, rf_ns : rf_t;
signal data_src0_s, data_src1_s : std_logic_vector(15 downto 0);

begin
-- output assignments
data_src0 <= data_src0_s;
data_src1 <= data_src1_s;


-- synchronous process
sync: process(clk)
begin
if (clk = '1' and clk'event) then
    if (rst ='1') then
        rf_cs <= (others => (others =>'0'));
    else
        rf_cs <= rf_ns;
    end if;
end if;    
end process sync;


---------------------------------
-- combinatorial process
comb: process(data_dst, rf_cs, dst, src0, src1, opgroup)
variable data_dst_v : std_logic_vector(15 downto 0);
variable data_src0_v, data_src1_v : std_logic_vector(15 downto 0);
variable rf_v : rf_t;
begin

    -- 1. variable assignment
    data_dst_v := data_dst;
    rf_v := rf_cs;
    
    -- 2. data manipulation
    -- 2.1. read data from register file
    data_src0_v := rf_cs(to_integer(unsigned(src0)));
    data_src1_v := rf_cs(to_integer(unsigned(src1)));

    -- 2.2. write data to register file
    if (opgroup = REGREG or opgroup = LOAD) then
        rf_v(to_integer(unsigned(dst))) := data_dst;      
    end if;

    -- 3. signal re-assignment
    data_src0_s <= data_src0_v; 
    data_src1_s <= data_src1_v; 
    rf_ns <= rf_v;
    
end process comb;

end rtl;
