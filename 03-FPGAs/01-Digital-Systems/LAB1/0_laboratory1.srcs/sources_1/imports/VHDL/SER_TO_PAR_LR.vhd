library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
 
entity SER_TO_PAR_LR is
generic(W : integer := 16);
port(
	CLK, SRESETN: in std_logic;
	-- serial bit stream in
	SER_IN: in std_logic;
	 -- control signals for shifting DIN bit in
        SHIFT_IN : in std_logic; 
	 -- control signals for save to parallel register
        SAVE_R_LOAD_L, SAVE_L_LOAD_R   : in std_logic;        
	-- parallel word out for left and right channel
	PAR_OUT_L, PAR_OUT_R: out std_logic_vector(W-1 downto 0)
	);
end SER_TO_PAR_LR;

architecture BEHAVIORAL of SER_TO_PAR_LR is

-- shift reg 
signal SHIFT_REG_NEXT,SHIFT_REG : std_logic_vector( W-1 downto 0);
-- parallel regs for left + right channel
signal PAR_REG_L_NEXT,PAR_REG_L : std_logic_vector( W-1 downto 0);
signal PAR_REG_R_NEXT,PAR_REG_R : std_logic_vector( W-1 downto 0); 

begin

-- process modeling 1 shift reg and 2 parallel output regs
S2P_REGS: process(CLK)
begin
	if CLK = '1' and CLK'event then
		if SRESETN = '0' then
			SHIFT_REG <= (others=>'0') after 2 ns;
 			PAR_REG_L <= (others=>'0') after 2 ns;
			PAR_REG_R <= (others=>'0') after 2 ns;
        	else
			SHIFT_REG <= SHIFT_REG_NEXT after 2 ns;
			PAR_REG_L <= PAR_REG_L_NEXT   after 2 ns;
			PAR_REG_R <= PAR_REG_R_NEXT   after 2 ns;
		end if;
	end if;
end process;

S2P_COMBIN: process(SER_IN,SHIFT_REG,PAR_REG_L,PAR_REG_R, 
		    SHIFT_IN,SAVE_R_LOAD_L, SAVE_L_LOAD_R)


begin		
	-- default operation for all regs is "keep value"
	SHIFT_REG_NEXT <= SHIFT_REG after 2 ns;
	PAR_REG_L_NEXT   <= PAR_REG_L   after 2 ns;
	PAR_REG_R_NEXT   <= PAR_REG_R   after 2 ns;

        -- shift into shift register (MSB first)
	if SHIFT_IN = '1' then
	   SHIFT_REG_NEXT <= SHIFT_REG(W-2 downto 0) & SER_IN;
	end if;

	-- next values of parallel registers
	if SAVE_L_LOAD_R = '1' then
	   PAR_REG_L_NEXT <=  SHIFT_REG after 2 ns;
	end if;
	if SAVE_R_LOAD_L = '1' then
	   PAR_REG_R_NEXT <= SHIFT_REG after 2 ns;
	end if;

	-- output logic: forward parallel reg values to port outputs
	PAR_OUT_L <= PAR_REG_L  after 2 ns; 
	PAR_OUT_R <= PAR_REG_R  after 2 ns;

end process;



end BEHAVIORAL;