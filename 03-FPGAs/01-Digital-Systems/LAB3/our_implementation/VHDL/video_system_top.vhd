--
--  "video_system_top" generates and initializes the pixel bitstream 
--  with correct timing of HSYNC, VSYNC and pixel coordinates X,Y
--  In the image processing pipeline: 
--  -   moving_sine_disp creates a moving sine from look-up table
--      with changing colors
--  - chess_board_disp overlays a chess board in the backgroud of 
--    the sine
--
--  1.5.2020 LTL
--


library ieee;
use ieee.std_logic_1164.all;
use IEEE.numeric_std.all;
use work.video_stream_pkg.all;

entity video_system_top is 
	port( 	clk	 				: in std_logic;
			sresetn 			: in std_logic;
			hSync				: out std_logic;
			vSync 				: out std_logic;
			dviClk  			: out std_logic;
			dviDataEn			: out std_logic;
			vgaClk  			: out std_logic;
			vgaNSync, vgaNBlank : out std_logic;
			pixelRGBData	 	: out unsigned(23 downto 0)
	);
end entity video_system_top;

architecture structural of video_system_top is

component VIDEO_CLK_GEN is
    generic (
		RATIO_CLK_TO_VGA_CLK : positive := 2 
	);
	port(
        clk       : in  std_logic;
        sresetn   : in  std_logic;
        clkEnable : out std_logic;
        vgaClk    : out std_logic;
        dviClk    : out std_logic
    );
end component;

component VIDEO_TIMING_GEN
    port(
        clk       		: in  std_logic;
        sresetn   		: in  std_logic;
        clkEnable 		: in  std_logic;
        videoStream   	: out VideoStream_t
    );
end component;

component VIDEO_PROCESSING is
    port(
        clk       		: in  std_logic;
        sresetn   		: in  std_logic;
        videoStreamIn   : in  VideoStream_t;
        videoStreamOut  : out VideoStream_t
    );
end component;

signal videoStream_Timing2spiralDemo, 
	   videoStream_spiralDemo2Encoder: VideoStream_t := VIDEO_STREAM_IDLE;
signal clkEnable : std_logic;

begin	

videoClkGen_i : VIDEO_CLK_GEN
	generic map (RATIO_CLK_TO_VGA_CLK => 2) -- 100 MHz
	port map(
        clk       => clk,
        sresetn   => sresetn,
        clkEnable => clkEnable,
        vgaClk    => vgaClk,
        dviClk    => dviClk
    );

videoTimingGen_i : VIDEO_TIMING_GEN
    port map(
        clk       	=> clk,
        sresetn   	=> sresetn,
        clkEnable 	=> clkEnable,
        videoStream => videoStream_Timing2spiralDemo
    );

spiralDemo_i : VIDEO_PROCESSING
    port map(
        clk       		=> clk,
        sresetn   		=> sresetn,
        videoStreamIn   => videoStream_Timing2spiralDemo,
        videoStreamOut  => videoStream_spiralDemo2Encoder
    );

pixelRGBData <= videoStream_spiralDemo2Encoder.pixelRGBData;
hSync        <=  videoStream_spiralDemo2Encoder.pulseHSync;
vSync        <=  videoStream_spiralDemo2Encoder.pulseVSync;
vgaNSync     <= '0';
vgaNBlank    <= '1';
dviDataEn    <=  videoStream_spiralDemo2Encoder.dviDataEn;

end architecture structural;