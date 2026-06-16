--------
-- TOP LEVEL ENTITY FOR VOLUME AND QUANTIZATION PROCESSING
-- LTL, 26.4.2018 
--------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;
 
entity AUDIO_PROCESSING is
	generic(W : integer := 16);
	port(
--		SRESETN				: in std_logic;
--		CLK					: in std_logic;
		AUDIO_IN_L			: in std_logic_vector(W-1 downto 0);
		AUDIO_IN_R			: in std_logic_vector(W-1 downto 0);
		AUDIO_OUT_L			: out std_logic_vector(W-1 downto 0);
		AUDIO_OUT_R			: out std_logic_vector(W-1 downto 0)--;
--		SW_AUDIO_ON			: in std_logic;
--		SW_MODE_VOL_nQNT	: in std_logic;
--		SW_PLUS				: in std_logic;
--		SW_MINUS			: in std_logic;
--		LED_OUT				: out std_logic_vector(9 downto 0)
	);
end AUDIO_PROCESSING;

architecture BEHAVIOUR of AUDIO_PROCESSING is

begin	
	AUDIO_OUT_L	<= AUDIO_IN_L after 2 ns;		
	AUDIO_OUT_R	<= AUDIO_IN_R after 2 ns;		
end BEHAVIOUR;