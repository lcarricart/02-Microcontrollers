------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 4
--
-- @name:   pmem.vhd
-- @author: 
-- @description: DIRISC program memory module
--				 DESIGN FILE
--
-- (c) 2025 HAW Hamburg
--
-- DIRISC instruction set architecture
-- 
-- REGREG -------------------------------------------------------------------------
-- 15 - 14 - 13 - 12 - 11 - 10 - 09 - 08 - 07 - 06 - 05 - 04 - 03 - 02 - 01 - 00 --
--  0 -  0 - opcode            - X  - src0         - src1         - dst          --
--
-- LOAD ---------------------------------------------------------------------------
-- 15 - 14 - 13 - 12 - 11 - 10 - 09 - 08 - 07 - 06 - 05 - 04 - 03 - 02 - 01 - 00 --
--  1 -  0 - address                                              - dst          --
--
-- STORE --------------------------------------------------------------------------
-- 15 - 14 - 13 - 12 - 11 - 10 - 09 - 08 - 07 - 06 - 05 - 04 - 03 - 02 - 01 - 00 --
--  1 -  1 - address                                              - src0         --
--
-- BRANCH -------------------------------------------------------------------------
-- 15 - 14 - 13 - 12 - 11 - 10 - 09 - 08 - 07 - 06 - 05 - 04 - 03 - 02 - 01 - 00 --
--  0 -  1 - offset                                          - cond              --
--
------------------------------------------------------------------

---------------------------------
-- libraries / packages 
library ieee;
use ieee.std_logic_1164.all;	-- std_logic
use ieee.numeric_std.all;		-- numeric

---------------------------------
-- entity
entity PMEM is
  port (
    -- primary memory port (a)
    clk   : in  std_logic;							-- clock
    PC          : in  std_logic_vector(9 downto 0);	-- PC in
    instruction : out std_logic_vector(15 downto 0)	-- instruction out
    );
end PMEM;

---------------------------------
-- architecture
architecture behavior of PMEM is

-- type declaration
type rom_t is array (natural range <>) of std_logic_vector(15 downto 0);
 
---------------------------------
-- definition of machine code instructions
constant pmem: rom_t := (
--  ld r1, x3F1:    10 011_1111_0001 001 (load 1 to r1)
	("1001111110001001"),           
--  ld r2, x3F7:    10 011_1111_0111 010 (load 7 to r2)
	("1001111110001001"),          
--  add r0, r2, r1: 00 0001 0 000 001 000 (r0 = r1 + r2 = 8)
	("0000010000001000"),
--  mul r0, r0, r1: 00 0011 0 000 010 000 (r0 = r0 * r2 = 56)
	("0000110000010000"),          

-- insert your machine code here
-- ...

--  st r0, x301:    11 011_0000_0001 000 (stores r0 to DMEM[1])
	("1110000000001000"),         	
--  b 0000:     01 0000000000 1010 (branch back to first instruction (address 0))
	("0100000000011010"),          

-- last instruction: nop (mov r0, r0)
	(x"0000")           
    );
  
begin

---------------------------------
-- bram process
process(clk)
begin
  if rising_edge(clk) then
	if (to_integer(unsigned(PC)) < pmem'length) then
      instruction <= pmem(to_integer(unsigned(PC)));
    else
      instruction <= (others =>'0');
    end if;
  end if;
end process;

end behavior;