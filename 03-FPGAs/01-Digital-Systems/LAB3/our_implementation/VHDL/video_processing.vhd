--
--  VIDEO_PROCESSING outputs a moving square moving in spiral
--  with changing colors, requires VIDEO_TIMING_GEN
--
--  13.12.2020 LTL
--

library ieee;
use ieee.std_logic_1164.ALL;
use ieee.numeric_std.all;
use work.video_stream_pkg.all;

entity VIDEO_PROCESSING is
    port(
        clk       		: in  std_logic;
        sresetn   		: in  std_logic;
        videoStreamIn   : in  VideoStream_t;
        videoStreamOut  : out VideoStream_t
    );

end entity;

architecture RTL of VIDEO_PROCESSING is

-- video parameters ----------------------------------
	-- screen size in pixels incl. blank region
	constant X_FULL : natural := 1040; 
	constant Y_FULL : natural :=  666;      

	-- visable screen size in pixels
	constant X_VISIBLE : natural := 800; 
	constant Y_VISIBLE : natural := 600;
 
-- spiral demo constants -------------------------------
	type dirMatrix_t is array (0 to 3, 0 to 1) of integer range -1 to 1;
	constant DIR_VEC : dirMatrix_t := ((-1,0), (0,-1), (1,0), (0,1));            -- direction vectors

	constant STEP_WIDTH : natural := 32; -- size of square and step width	

	constant X_START : natural := (X_VISIBLE/(2*STEP_WIDTH))*STEP_WIDTH;  -- start x coord of square
	constant Y_START : natural := (Y_VISIBLE/(2*STEP_WIDTH))*STEP_WIDTH;  -- start y coord of square 

-- spiral demo signals ---------------------------------
-- registers
	-- states
	type state_t is (displaySquare_s,updatePos_s,updateSegment_s,updateColor_s);
	signal state, stateNext : state_t := (displaySquare_s);
	-- temp. video stream for assembling components  
	signal videoStreamOutTemp, videoStreamOutTempNext : VideoStream_t := VIDEO_STREAM_IDLE;
	-- current upper right corner of square
	signal xSquare, xSquareNext 		: natural range 0 to X_VISIBLE := 0; 
	signal ySquare, ySquareNext 		: natural range 0 to Y_VISIBLE := 0;
	-- number of successive steps to be taken in one direction ("segment")	
	signal segmLength, segmLengthNext 	: natural range 1 to X_VISIBLE := 1;  
	-- position of square in current segment
	signal segmIndex, segmIndexNext  	: natural range 1 to X_VISIBLE := 1;
	-- toggle value to change segment length only every 2nd time	
	signal changeSegLength, changeSegLengthNext : natural range 0 to 1 := 0; 
	-- 0:west, 1: north, 2:east, 3:south	
	signal currDir, currDirNext 		: natural range 0 to 3 := 0; 
	-- color index 2: red, 1:green, 0:blue
	signal indexColor, indexColorNext 	: natural range 0 to 2 := 0;  
	-- current color 255*256^indexColor
	signal currColor, currColorNext 	: natural range 0 to 2**24-1 := 255;   

begin

-- forward video stream register to output 
videoStreamOut <= videoStreamOutTemp; 

-- sequential process with registers
spriralSeq_p : process (clk)
begin
	if rising_edge(clk) then    
		if sresetn = '0' then
			state 			<= displaySquare_s 	after 2 ns; 
			xSquare 			<= X_START 	after 2 ns; 
			ySquare 			<= Y_START 	after 2 ns;   
			segmLength 		<=   1 	after 2 ns;
			segmIndex 		<=   1 	after 2 ns;
			changeSegLength 	<=   0 	after 2 ns;    
			currDir 			<=   0 	after 2 ns;           
			indexColor 		<=   0 	after 2 ns;     
			currColor 		<= 255 	after 2 ns;      
			videoStreamOutTemp <= VIDEO_STREAM_IDLE after 2 ns;
		else
			state 			<= stateNext 			after 2 ns; 
			xSquare 			<= xSquareNext 		after 2 ns; 
			ySquare 			<= ySquareNext 		after 2 ns;   
			segmLength 		<= segmLengthNext 	after 2 ns;
			segmIndex 		<= segmIndexNext 		after 2 ns;
			changeSegLength 	<= changeSegLengthNext after 2 ns;    
			currDir 			<= currDirNext 		after 2 ns;           
			indexColor 		<= indexColorNext 	after 2 ns;     
			currColor 		<= currColorNext 		after 2 ns;      		
			videoStreamOutTemp <= videoStreamOutTempNext after 2 ns;
		end if;
	end if;
end process;

spriralComb_p : process (videoStreamIn, state, videoStreamOutTemp, xSquare,ySquare,segmLength,segmIndex,changeSegLength,currDir,indexColor,currColor)
	variable indexColorTemp : natural range 0 to 2 := 0;  
	-- current pixel position of CRT beam	
	variable xPos : natural range 0 to X_FULL-1 := 0; 
	variable yPos : natural range 0 to Y_FULL-1 := 0; 
begin
	yPos := to_integer(videoStreamIn.verticalPos);  			-- rename current x,y position for easier handling 
	xPos := to_integer(videoStreamIn.horizontalPos);

-- Default assignments
	videoStreamOutTempNext <= videoStreamIn after 2 ns; 		-- copy input video stream to temp register
	stateNext 		<= state 		after 2 ns;                    	-- keep state/reg value if not otherwise noted
	xSquareNext		<= xSquare 		after 2 ns; 
	ySquareNext 	   	<= ySquare 		after 2 ns;   
	indexColorNext  	<= indexColor 	after 2 ns; 
	currColorNext   	<= currColor  	after 2 ns;
	currDirNext        <= currDir      after 2 ns; 
	segmIndexNext 	<= segmIndex 		after 2 ns;
	segmLengthNext 	<= segmLength 	after 2 ns;
	changeSegLengthNext <= changeSegLength after 2 ns;                    
	segmIndexNext 	<= segmIndex 		after 2 ns;  

-- Finite State Machine with Data Path (FSMD)
	case state is
	when displaySquare_s 	=> 
			-- grid
			if (yPos mod STEP_WIDTH= 0) or (xPos mod STEP_WIDTH = 0)  then
				videoStreamOutTempNext.pixelRGBData <= to_unsigned(8947848,NUM_BITS_PIXEL_RGB);
			end if;			
			if (xPos >= xSquare) and (xPos < (xSquare + STEP_WIDTH)) then
				if (yPos >= ySquare) and (yPos < (ySquare + STEP_WIDTH)) then
					videoStreamOutTempNext.pixelRGBData <= to_unsigned(currColor,NUM_BITS_PIXEL_RGB);
				end if;
			end if;
			-- transition
			if xPos = X_VISIBLE-1 and yPos = Y_VISIBLE-1 then
				stateNext <= updatePos_s after 2 ns;
			end if; 
	when updatePos_s 		=> 
               if (ySquare >= STEP_WIDTH) and (ySquare <= Y_VISIBLE-STEP_WIDTH) then
				xSquareNext <= xSquare + DIR_VEC(currDir,0) * STEP_WIDTH after 2 ns;
				ySquareNext <= ySquare + DIR_VEC(currDir,1) * STEP_WIDTH after 2 ns;
			else                    
				xSquareNext 			<= X_START 	after 2 ns; 
				ySquareNext 			<= Y_START 	after 2 ns;
				segmIndexNext 		<= 1 	after 2 ns;
				segmLengthNext 		<= 1 	after 2 ns;
				changeSegLengthNext 	<= 0 	after 2 ns;                    
			end if;
			-- transition
			stateNext <= updateSegment_s after 2 ns;
			
	when updateSegment_s 	=>
			-- update segment every 2nd clock cycle and move on position segement
	          if segmIndex < segmLength then
				segmIndexNext <= segmIndex + 1;
               else
				if changeSegLength = 1 then
					segmLengthNext <= segmLength + 1;
					changeSegLengthNext <= 0 after 2 ns;
				else                        
					changeSegLengthNext <= 1 after 2 ns;
				end if;
			segmIndexNext <= 1 after 2 ns;
			currDirNext <= ((currDir+1) mod 4) after 2 ns;
			end if; 
			-- transition	
			stateNext <= updateColor_s after 2 ns;
			
	when updateColor_s 	=> 
			-- update color B->G->R
			indexColorTemp  := ((indexColor+1) mod 3); 	-- next color in variable to allow for tapping in same state, 
													-- long path! alternative: 2 states
			indexColorNext  <= indexColorTemp after 2 ns;                
			currColorNext   <= 255 * 2**(8*indexColorTemp) after 2 ns;  -- synthesizable since power of 2
			-- transition
			if xPos = X_FULL-1 and yPos = Y_FULL-1 then
				stateNext <= displaySquare_s after 2 ns;
			end if; 	
			
	end case;

end process;
 
end architecture RTL;
