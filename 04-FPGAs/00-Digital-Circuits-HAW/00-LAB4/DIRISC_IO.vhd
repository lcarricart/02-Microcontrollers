------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 4
--
-- @name:   IO.vhd
-- @author: 
-- @description: DIRISC peripheral IO module
--				 DESIGN FILE
--
-- (c) 2025 HAW Hamburg
--
------------------------------------------------------------------

---------------------------------
-- libraries / packages 
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

---------------------------------
-- entity
entity IO is
    port(
    clk   : in  std_logic;
    rst   : in  std_logic;
    SW0   : in  std_logic_vector(7 downto 0);
    SW1   : in  std_logic_vector(7 downto 0);
    cs    : in  std_logic;
    wren  : in  std_logic;
    din   : in  std_logic_vector(15 downto 0);
    addr  : in  std_logic_vector(1 downto 0);
    
    LED0  : out std_logic_vector(7 downto 0);
    LED1  : out std_logic_vector(7 downto 0);
    dout  : out std_logic_vector(15 downto 0)
    );
end IO;

---------------------------------
-- architecture
architecture rtl of IO is

-- signal declaration
signal LED0_cs, LED1_cs, SW0_cs, SW1_cs : std_logic_vector(7 downto 0);
signal LED0_ns, LED1_ns, SW0_ns, SW1_ns : std_logic_vector(7 downto 0);
signal dout_s : std_logic_vector(15 downto 0);

begin
-- signal/output assignment
dout <= dout_s;
LED0 <= LED0_cs;
LED1 <= LED1_cs;
SW0_ns <= SW0;
SW1_ns <= SW1;

---------------------------------
-- sequential process
sync: process(clk)
begin
  if rising_edge(clk) then
    if rst = '1' then
      LED0_cs <= (others =>'0');
      LED1_cs <= (others =>'0');
      SW0_cs <= (others =>'0');  
      SW1_cs <= (others =>'0');
    else
      LED0_cs <= LED0_ns; 
      LED1_cs <= LED1_ns;
      SW0_cs <= SW0_ns;  
      SW1_cs <= SW1_ns;
    end if;
  end if;  
end process;

---------------------------------
-- combinatorial process

comb : process(addr, LED0_cs, LED1_cs, SW0_cs, SW1_cs, din, cs, wren)
variable dout_v : std_logic_vector(15 downto 0);
variable LED0_v, LED1_v : std_logic_vector(7 downto 0);
begin
  -- 1. variable assignment
  dout_v := (others =>'0');
  LED0_v := LED0_cs;
  LED1_v := LED1_cs;
  
  -- 2. data manipulation
  -- 2.1. switches to datapath
  case addr is
  when "10" => dout_v(7 downto 0) := SW0_cs;
  when "11" => dout_v(7 downto 0) := SW1_cs;
  when others => null;
  end case;

  -- 2.2. datapath to LEDs
  if (cs = '1' and wren = '1') then
    case addr is
    when "00" => LED0_v := din(7 downto 0);
    when "01" => LED1_v := din(7 downto 0);
    when others => null;
    end case;
   end if;

  -- 3 signal assignment
  dout_s <= dout_v;
  LED0_ns <= LED0_v;
  LED1_ns <= LED1_v;
end process;

end rtl;
