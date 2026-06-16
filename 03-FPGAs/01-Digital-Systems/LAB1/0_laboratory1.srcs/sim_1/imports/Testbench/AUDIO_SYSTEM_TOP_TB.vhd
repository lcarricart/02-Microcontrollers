
library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use STD.textio.all;
use ieee.std_logic_textio.all;

entity AUDIO_SYSTEM_TOP_TB is
end AUDIO_SYSTEM_TOP_TB;

architecture BEHAVIORAL of AUDIO_SYSTEM_TOP_TB is

-- component under test
component AUDIO_SYSTEM_TOP is
	generic(W : integer := 16);
	port(
		CLK, SRESETN : in std_logic;
		-- clock signals (to audio codec)
		SYSCLK, BCK, LRC: out std_logic;
		-- serial data in
		DIN: in std_logic;
		-- serial data out
		DOUT: out std_logic
		);
end component AUDIO_SYSTEM_TOP;

-- Testbench Internal Signals

constant CLK_PERIOD: time := 10 ns; -- clock period (1/100 MHz)
constant SYSCTL_PERIOD: time := 80 ns; -- bit clock (8/100 MHz)
constant BCK_PERIOD: time := 640 ns; -- bit clock (8*8/100 MHz)
constant LRC_PERIOD: time := 20480 ns; -- LRC period (32*8*8/100MHz = approx 1/48kHz, 20.08333... would be accurate)
constant CMB_DLY : time :=  2 ns;    -- combinational delay time
constant RESET_TIME : time := 55 ns; --  reset time
constant W : integer := 16; -- size of a mono audio sample

signal CLK : std_logic :='1';
signal SRESETN 	: std_logic;
signal BCK, LRC, SYSCLK : std_logic;
signal DIN, DOUT : std_logic;

begin

-- CLK and SRESETN generation by unconditional assignement 
	CLK <= not CLK after CLK_PERIOD / 2 ;     -- actually a combinational loop -> oscillator 
	SRESETN <= '0', '1' after RESET_TIME;     -- non periodic, giving values directly like in force instruction
    
dinStimulus_p : process
constant dinStimulusLength : integer := 2*W;                                -- 0       8       0       8       
constant dinStimulusArray : std_logic_vector(dinStimulusLength-1 downto 0) := "10011100111100110101101010011110";
begin
    DIN <= '0';
    wait for RESET_TIME + CMB_DLY;              -- DIN is zero while in active-low reset
    for I in 0 to dinStimulusLength-1 loop      -- input DIN array bit by bit
        DIN <= dinStimulusArray(I);
        wait for BCK_PERIOD;
    end loop;
    wait;
end process;       
            

 -- Instantiate and Map DUT
 -- (output signals not used in TB are to be declared open, otherwise cannot be waveform viewed  
DUT : AUDIO_SYSTEM_TOP 
      port map(CLK, SRESETN, SYSCLK, BCK, LRC, DIN, DOUT 
			  );
  

end BEHAVIORAL;

