-- FRQ_PRE_DIV
-- frequency pre divider, creates every 2^(P-1) an enable pulse
-- LTL, 7.2.2018

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
 
entity FRQ_PRE_DIV is
generic(P : integer := 2);
port(
    SRESETN: in  std_logic;
	CLK:     in  std_logic;
	EN_CLK : out std_logic
	);
end FRQ_PRE_DIV;

architecture BEHAVIORAL of FRQ_PRE_DIV is
-- counter state reg
signal Q, Q_NEXT: unsigned(P-1 downto 0);

begin
-- counter 
FRQ_PRE_DIV_REG: process(CLK)
begin
	if CLK='1' and CLK'event then
		if SRESETN = '0' then
			Q <= (others =>'0'); 
		else
			Q <= Q_NEXT after 2 ns;
		end if;
	end if;
end process;

-- increment and create en enable pulse every 2^P cycle
FRQ_PRE_DIV_CMB: process(Q)
begin
    EN_CLK <= '0' after 2 ns;
	-- output logic
	if Q = 2**P-1 then 
		EN_CLK <= '1' after 2 ns;
	end if;
	-- next state logic
	Q_NEXT <= Q + 1 after 2 ns;
end process;

end BEHAVIORAL;