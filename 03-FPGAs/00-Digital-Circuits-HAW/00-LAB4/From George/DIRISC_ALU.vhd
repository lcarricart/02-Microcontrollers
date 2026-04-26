------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 4
--
-- @name:   ALU.vhd
-- @author: 
-- @description: DIRISC arithmetic, logic unit
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
entity ALU is
    Port ( data_src0 : in STD_LOGIC_VECTOR (15 downto 0);
           data_src1 : in STD_LOGIC_VECTOR (15 downto 0);
           opcode : in STD_LOGIC_VECTOR(3 downto 0);
           
           data_dst : out STD_LOGIC_VECTOR (15 downto 0)
           );
end ALU;

---------------------------------
-- architecture
architecture rtl of ALU is

-- signal declaration
signal data_dst_s : std_logic_vector(15 downto 0);

begin

-- output assignment
data_dst <= data_dst_s;

---------------------------------
-- combinatorial process
comb: process(data_src0, data_src1, opcode)
    variable data_dst_v : signed(15 downto 0);
    variable data_src0_v, data_src1_v : signed(15 downto 0);
    variable data_multiply_v : signed(31 downto 0);
begin
    -- 1. variable assignments
    data_src0_v := signed(data_src0);
    data_src1_v := signed(data_src1);
    data_dst_v := to_signed(0,16);
    
    -- 2. data manipulation: arithmetic/logic operation
    case opcode is
    when OPCODE_ADD => data_dst_v := data_src0_v + data_src1_v;
    when OPCODE_SUB => data_dst_v := data_src0_v - data_src1_v;
    when OPCODE_SHL => data_dst_v := data_src0_v(14 downto 0) & '0';
    when OPCODE_SHR => data_dst_v := data_src0_v(15) & data_src0_v(15 downto 1);
    when OPCODE_AND => data_dst_v := data_src0_v AND data_src1_v;
    when OPCODE_OR  => data_dst_v := data_src0_v OR data_src1_v;
    when OPCODE_NOT => data_dst_v := NOT data_src0_v;
    when OPCODE_XOR => data_dst_v := data_src0_v XOR data_src1_v;
    when OPCODE_MOV => data_dst_v := data_src0_v;
    when OPCODE_MUL => 
    data_multiply_v := data_src0_v * data_src1_v;
    data_dst_v := data_multiply_v(15 downto 0);
    when others => null;
    end case;

    -- 3. signal assignment
    data_dst_s <= std_logic_vector(data_dst_v);
            
end process comb;

end rtl;
