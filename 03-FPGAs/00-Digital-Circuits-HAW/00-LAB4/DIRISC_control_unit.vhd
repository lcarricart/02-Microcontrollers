------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 4
--
-- @name:   control_unit.vhd
-- @author: 
-- @description: DIRISC control unit
--				 DESIGN FILE
--
-- (c) 2025 HAW Hamburg
--
------------------------------------------------------------------

---------------------------------
-- libraries / packages 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
use work.global.all;

---------------------------------
-- entity
entity control_unit is
    port(
        clk : in std_logic;
        rst : in std_logic;
        instruction : in  std_logic_vector(15 downto 0);
        SREG        : in  std_logic_vector(3 downto 0);
        addr        : out std_logic_vector(10 downto 0);
        cs          : out std_logic_vector(1 downto 0);
        wren        : out std_logic;
        opgroup     : out std_logic_vector(1 downto 0);
        opcode      : out std_logic_vector(3 downto 0);
        src0        : out std_logic_vector(2 downto 0);
        src1        : out std_logic_vector(2 downto 0);
        dst         : out std_logic_vector(2 downto 0);
        PC          : out std_logic_vector(9 downto 0)
        );
end control_unit;

---------------------------------
-- architecture
architecture rtl of control_unit is

-- signal declaration
signal opcode_s, cond_s : std_logic_vector(3 downto 0);
signal src0_s, src1_s, dst_s : std_logic_vector(2 downto 0);
signal setflag_s : std_logic;
signal address_s : std_logic_vector(10 downto 0);
signal offset_s : std_logic_vector(9 downto 0);
signal cs_s : std_logic_vector(1 downto 0);
signal wren_s : std_logic;
signal opgroup_s : std_logic_vector(1 downto 0);
signal PC_cs, PC_ns : unsigned(9 downto 0);
signal stall_cs, stall_ns : std_logic;
signal instruction_s : std_logic_vector(15 downto 0);

begin
-- output port assignments
PC <= std_logic_vector(PC_cs);
addr <= address_s;
src0 <= src0_s;
src1 <= src1_s;
dst <= dst_s;
opgroup <= opgroup_s;
cs <= cs_s;
opcode <= opcode_s;
wren <= wren_s;

-- signal assignment/instruction
instruction_s <= instruction when stall_cs = '0' else
                 x"0000";

---------------------------------
-- sequential process
process(clk, rst)
begin
  if rising_edge(clk) then
    if (rst='1') then    
      PC_cs <= (others =>'0');
      stall_cs <= '1';
    else
      PC_cs <= PC_ns;
      stall_cs <= stall_ns;
    end if;
  end if;
end process;


---------------------------------
-- instruction decoder
id: process(instruction_s)
begin
-- TODO: insert instruction decoder logic for PART 1
-- ...
end process;

---------------------------------
-- address decoder
ad: process(opgroup_s, address_s)
begin
-- bad coding style, but works for Part 1
-- TODO: extend/modify/improve the given address decoder logic fpr Part 2
	cs_s <= (others =>'0');
	wren_s <= '0';

	if (opgroup_s(1) = '1') then
	  -- Info: cs_s(1) is DMEM chipselect, cs_s(0) is IO chipselect
	  cs_s <= '1' & '0';
	  wren_s <= opgroup_s(0);
    end if;
	
	
end process;

---------------------------------
-- branch unit
bu: process(opgroup_s, cond_s, offset_s, PC_cs, SREG)
variable PC_v : unsigned(9 downto 0);
variable offset_v : unsigned(9 downto 0);
variable stall_v : std_logic;
begin
    -- 1. variable assignment
    PC_v := PC_cs;
    offset_v := unsigned(offset_s);
    stall_v := '0';
    
    -- 2. data processing
    PC_v := PC_cs + 1;
    if (opgroup_s = BRANCH) then
        if (cond_s = ALWAYS or
            cond_s = SREG) then
            PC_v := offset_v;
            stall_v := '1';
        end if;
    end if;
    
    -- 3. signal assignment
    PC_ns <= PC_v;
    stall_ns <= stall_v;
end process;

end rtl;
