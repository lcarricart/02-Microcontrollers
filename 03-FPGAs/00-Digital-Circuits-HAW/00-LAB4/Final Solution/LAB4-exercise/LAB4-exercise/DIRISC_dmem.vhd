------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 4
--
-- @name:   dmem.vhd
-- @author: 
-- @description: DIRISC data memory module
--				 DESIGN FILE
--
-- (c) 2025 HAW Hamburg
--
------------------------------------------------------------------

---------------------------------
-- libraries / packages 
library ieee;
use ieee.std_logic_1164.all;	-- std_logic
use ieee.numeric_std.all;		-- numeric

---------------------------------
-- entity
entity DMEM is
  port (
    -- primary memory port (a)
    clk   : in  std_logic;							-- clock
    wr    : in  std_logic;							-- write enable flag
    cs    : in  std_logic;                      -- chipselect
    addr  : in  std_logic_vector(9 downto 0);	-- address port
    din   : in  std_logic_vector(15 downto 0);	-- DWIDTH in
    dout  : out std_logic_vector(15 downto 0)	-- DWIDTH out
    );
end DMEM;

---------------------------------
-- architecture
architecture behavior of DMEM is

-- type declaration
type mem_t is array ( (2**10)-1 downto 0 ) of std_logic_vector(15 downto 0);	-- memory type
-- signal declaration
signal mem : mem_t;	-- memory datatype
signal dout_s : std_logic_vector(15 downto 0);  
begin

dout <= (x"000" & addr(3 downto 0)) when addr(9 downto 4) = "111111" else
        dout_s;

---------------------------------
-- combinatorial process
process(addr, mem)
begin
    dout_s <= mem(to_integer(unsigned(addr)));
end process;

---------------------------------
-- bram process
process(clk)
begin
  if rising_edge(clk) then
    if(wr='1' and cs = '1') then
	  mem(to_integer(unsigned(addr))) <= din;
	end if;
  end if;
end process;

end behavior;
