------------------------------------------------------------------
--
-- [IE3-DI] Digital Circuits Winter Term 2025
--			Exercise 2
--
-- @name:   adder.vhd
-- @author: Luciano Carricart
-- @description: Ripple-Carry-Adder Unit
--				 DESIGN FILE
--
-- (c) 2025 HAW Hamburg
--
------------------------------------------------------------------
-- Use the following commands to open and close simulations
-- close_sim
-- launch_simulation
---------------------------------
-- libraries / packages 
library IEEE;
use IEEE.std_logic_1164.all;

---------------------------------
-- entity
entity adder is
	port( 	
		A  	 : IN  std_logic_vector(7 downto 0);	-- Operand A (bitwidth 8)
     	B  	 : IN  std_logic_vector(7 downto 0);    -- Operand B (bitwidth 8)
        sel  : IN  std_logic;                       -- Multiplexer sel (bitwidth 1)

        C    : OUT std_logic;                       -- Carry flag
        V    : OUT std_logic;                       -- Overflow flag
        N    : OUT std_logic;                       -- Negative flag denoting if result is negative
        Z    : OUT std_logic;                       -- Negative flag denoting whether result is zero

		S 	 : OUT std_logic_vector(7 downto 0)	-- Sum (bitwidth 8)
		);
end entity;

---------------------------------
-- architecture
architecture rtl of adder is
begin

---------------------------------
-- combinatorial process
adding: process(A, B, sel)

-- variable declarations 
variable sel_v : std_logic;
variable S_v : std_logic_vector(7 downto 0);
variable A_v, B_v : std_logic_vector(7 downto 0);
variable C_v, V_v, N_v, Z_v : std_logic;
variable c7_v : std_logic;      -- Prev state of co, for ovverflow flag calculation (bitwidth 1)

begin
	-- 1. signal to variable assignment
	A_v := A;
	sel_v := sel;
	
	if sel = '0' then  -- Adder case
        B_v := B;
        c_v := '0';
    else               -- Substractor case (first apply 2's complement, then add normally)
        B_v := NOT(B_v);
        c_v := '1';
    end if;
    
    -- 2. data processing: ripple-carry adder
    for i in 0 to 7 loop
       S_v(i) := A_v(i) XOR B_v(i) XOR c_v;
       c_v := (A_v(i) AND B_v(i) AND (NOT c_v)) OR 
              (A_v(i) AND (NOT B_v(i)) AND c_v) OR 
              ((NOT A_v(i)) AND B_v(i) AND c_v) OR 
              (A_v(i) AND B_v(i) AND c_v);
              
       if i = 6 then
            c7_v := c_v;
       end if;
    end loop;
	
	C_v := c_v;
	V_v := c_v XOR c7_v;
	N_v := S_v(7);
	
	if S_v = "0000000" then
	   Z_v := '1';
	else
	   Z_v := '0';
	end if;
	
	-- 3. variable to signal re-assignment
	C <= C_v;
	V <= V_v;
	N <= N_v;
	Z <= Z_v;
	
	S <= S_v;
	
end process;

end architecture rtl;
