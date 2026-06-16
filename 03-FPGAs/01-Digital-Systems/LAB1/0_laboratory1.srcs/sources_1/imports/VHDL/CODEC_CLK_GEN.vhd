-- clock generation module for PCM3006 audio codec
library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
 
entity CODEC_CLK_GEN is
generic(M : integer := 9);
port(
	CLK       : in std_logic;
	EN_CLK    : in  std_logic;
	SRESETN   : in std_logic;
	SYSCLK, BCK, LRC : out std_logic
	);
end CODEC_CLK_GEN;

architecture BEHAVIORAL of CODEC_CLK_GEN is

-- counter state reg
signal Q, Q_NEXT: unsigned(M-1 downto 0);

begin
-- counter 
CLK_DIV_M_REG: process(CLK)
begin
	if CLK='1' and CLK'event then
		if SRESETN = '0' then
			Q <= (others =>'0') after 2 ns; 
		else
			Q <= Q_NEXT after 2 ns;
		end if;
	end if;
end process;

-- increment when enabled
CLK_DIV_M_CMB: process(Q,EN_CLK)
begin
	-- audio codec clocks (output logic)
	SYSCLK <= std_logic(Q(0)) after 2 ns;
	BCK    <= std_logic(Q(3)) after 2 ns;
	LRC    <= std_logic(Q(8)) after 2 ns;

	-- next state logic
	Q_NEXT <= Q after 2 ns;
	if EN_CLK = '1' then
		Q_NEXT <= Q + 1 after 2 ns;
	end if;		
end process;

end BEHAVIORAL;
