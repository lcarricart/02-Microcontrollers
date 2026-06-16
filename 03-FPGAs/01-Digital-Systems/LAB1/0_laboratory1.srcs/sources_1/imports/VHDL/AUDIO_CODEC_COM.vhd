-- AUDIO CODEC COMMUNICATION module
-- generates clocks SYSCLK, BCK, LRC
-- and converts data streams serial to parallel and vice versa
-- LTL, 09.02.2018 

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
 
entity AUDIO_CODEC_COM is
generic(W : integer := 16);
port(
	CLK, SRESETN : in std_logic;
 	-- clock signals (to audio codec)
	SYSCLK, BCK, LRC: out std_logic;
	-- ADC path (from audio codec to signal processing)
	DIN: in std_logic;
	PAR_OUT_L: out std_logic_vector(15 downto 0);
	PAR_OUT_R: out std_logic_vector(15 downto 0);
	-- DAC path (from signal processing to audio codec)
	PAR_IN_L: in std_logic_vector(15 downto 0);
	PAR_IN_R: in std_logic_vector(15 downto 0);
	DOUT: out std_logic
	);
end AUDIO_CODEC_COM;

architecture BEHAVIORAL of AUDIO_CODEC_COM is

-- components declarations
component FRQ_PRE_DIV 
generic(P : integer := 2);
port(
    SRESETN: in  std_logic;
	CLK:     in  std_logic;
	EN_CLK : out std_logic
	);
end component FRQ_PRE_DIV;

component CODEC_CLK_GEN
generic(M : integer := 9);
port(
	CLK       : in std_logic;
	EN_CLK    : in  std_logic;
	SRESETN   : in std_logic;
	SYSCLK, BCK, LRC : out std_logic
	);
end component CODEC_CLK_GEN;

component AUDIO_CDC_FSM 
    port(SRESETN, CLK : in std_logic;
        BCK, LRC : in std_logic;
	 -- control signals for serial 2 parallel converter
        SHIFT_IN	: out std_logic; 
	 -- control signals for parallel 2 serial converter
	SHIFT_OUT	: out std_logic;
	 -- control signals for save to/ load from parallel register
        SAVE_R_LOAD_L, SAVE_L_LOAD_R   : out std_logic
        );
end component AUDIO_CDC_FSM;

component SER_TO_PAR_LR 
	generic(W : integer := 16);
	port(
		CLK, SRESETN: in std_logic;
	-- serial bit stream in
		SER_IN: in std_logic;
	-- control signals for shifting a bit in
        SHIFT_IN  : in std_logic; 
	-- control signals for save to parallel register
        SAVE_R_LOAD_L, SAVE_L_LOAD_R   : in std_logic;        
	-- parallel word out
		PAR_OUT_L, PAR_OUT_R: out std_logic_vector(W-1 downto 0)
		);
end component SER_TO_PAR_LR;

component PAR_TO_SER_LR 
generic(W : integer := 16);
port(
	CLK, SRESETN: in std_logic;
	PAR_IN_L, PAR_IN_R: in std_logic_vector(W-1 downto 0);
	 -- control signals for shifting bit out
	SHIFT_OUT 	: in std_logic;
	 -- control signals for load in parallel word
        SAVE_R_LOAD_L, SAVE_L_LOAD_R   : in std_logic;        
	SER_OUT: out std_logic
	);
end component PAR_TO_SER_LR;

-- internal signals connecting the components
signal INT_BCK, INT_LRC, EN_CLK : std_logic;
signal SHIFT_IN, SHIFT_OUT, SAVE_R_LOAD_L, SAVE_L_LOAD_R : std_logic;


-- structural VHDL model -----------------
begin
-- components declarations
INST_FRQ_PRE_DIV : FRQ_PRE_DIV 
port map(SRESETN,CLK,EN_CLK);

INST_CODEC_CLK_GEN : CODEC_CLK_GEN
	port map (CLK, EN_CLK, SRESETN, SYSCLK, INT_BCK, INT_LRC);

INST_AUDIO_CDC_FSM : AUDIO_CDC_FSM
    port map(SRESETN, CLK, INT_BCK, INT_LRC, SHIFT_IN, SHIFT_OUT, SAVE_R_LOAD_L, SAVE_L_LOAD_R);

INST_SER_TO_PAR_LR : SER_TO_PAR_LR 
	port map(CLK,SRESETN,DIN,SHIFT_IN,SAVE_R_LOAD_L, SAVE_L_LOAD_R,PAR_OUT_L, PAR_OUT_R);

INST_PAR_TO_SER_LR : PAR_TO_SER_LR 
	port map(CLK,SRESETN,PAR_IN_L,PAR_IN_R,SHIFT_OUT,SAVE_R_LOAD_L, SAVE_L_LOAD_R,DOUT);

BCK <= INT_BCK after 2 ns;
LRC <= INT_LRC after 2 ns;
	
end architecture BEHAVIORAL;

