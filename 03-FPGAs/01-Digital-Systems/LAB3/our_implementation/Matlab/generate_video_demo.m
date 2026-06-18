%===============================================================================
% Objective: create a Matlab video "rgb_movie" based on the assignment 9 of
% Laboratory 3, Digital Systems. Later, export the movie to a MPEG4 local file.
%===============================================================================
% Version 14/06/2026
% Authors:
%   - Luciano Carricart
%   - Imran Rizwan
%   - Georgii Molyboga
% 
% v2: frameWait fixed to 3 since the tolerances of the clock can be disregarded.
%===============================================================================

clear all;

% -- simulation settings (change here) -------------------------------
CONVERT_TO_MPEG4  = true;  % produce mp4 file
SHOW_RGB_FRAME    = true;  % show each RGB frame after being processed
SLOW_MOTION       = false; % produce video at 10 (slomo)/72(real-time) fps

% suppress warning that image too large for screen
warning('off','images:initSize:adjustingMag'); 

% -- video parameters (don't touch) ----------------------------------
X_FULL = 1040;   Y_FULL = 666;      % screen size in pixels incl. blank region
X_VISIBLE = 800; Y_VISIBLE = 600;   % visable screen size in pixels
FRAME_RATE = 72;                    % frame rate in Hz
VIDEO_LEN = 5;                      % video length in sec
xPos = 0; yPos = 0;                 % current pixel position of CRT beam 
currFrame = zeros(Y_FULL,X_FULL);   % current video frame as matrix
visRGB8frame = zeros(Y_VISIBLE,X_VISIBLE,3); % frame in matlab 8 bit RGB format 

% -- constants of Windows Screensaver demo (define your own here) ---------
STEP_WIDTH    = 25;  % size of square and ste width
BORDER_SQUARE = 5;   % border width
X_START       = 375; % start x coord of square (top left)
Y_START       = 275; % start y coord of square (top left)

% Allowed range of movement
X_MIN = 0;
Y_MIN = 0;
X_MAX = X_VISIBLE - STEP_WIDTH; % 775
Y_MAX = Y_VISIBLE - STEP_WIDTH; % 575

% color definitions (R*256*256 + G*256 + B)
COLOR_RED   = 255*256*256;
COLOR_GREEN = 255*256;
COLOR_BLUE  = 255;

% open mp4 stream for writing
if CONVERT_TO_MPEG4
    v = VideoWriter('rgb_movie.mp4','MPEG-4');
    if SLOW_MOTION
        v.FrameRate = 10;
    else
        v.FrameRate = 72;
    end
    v.Quality = 100;    
    open(v)
end

% -- reset values of Windows Screensaver demo (define your own here) -----------------
xSquare = X_START; ySquare = Y_START;  % current upper right corner of square 
dirX = STEP_WIDTH;      % horizontal step per block move (decrements to the left)
dirY = -STEP_WIDTH;     % vertical step per block move   (decrements to the top)
currColor = COLOR_BLUE; % color initialization

% Variables to slow the block move down from the 72 Hz frame rate
frameCounter = 0;       % frames drawn since the last move
frameWait    = 3;       % frames to wait before the next move (72/3 = 24 moves/s)
% -------------------------------------------------------------------------------
tic
for N = [1:VIDEO_LEN*FRAME_RATE] % number of frames to be generated (72fps @ 800x600)
    % N is not available in the real-time system, don't rely on it in your
    % algorithm and create your own frame counter
    disp(N);
    currFrame = zeros(Y_FULL,X_FULL); % new blank frame  
    for yPos = 0:Y_FULL-1      
        % yPos: vertical cathode ray pos, provided as input port in VHDL too
        for xPos = 0:X_FULL-1  
        % xPos: horizontal cathode ray pos, provided as input port in VHDL too

        % -- none of your code lines must be outside the region below ----
        % -- unless you set a reset value or constant definition above ---
        
        % -- Windows Screensaver algorithm  -------------------------------------------
            
            % STATE: DISPLAY_PIXEL
            % create grid
            if mod(yPos,STEP_WIDTH)==0 || mod(xPos,STEP_WIDTH)==0
                    currFrame(yPos+1,xPos+1) = 8947848; % dark gray
            end
            % create square (centre empty and colored border)
            % If X is within the square's horizontal boundary
            if (xPos >= xSquare) && (xPos < (xSquare + STEP_WIDTH))
                % If Y is within the square's vertical boundary
                if (yPos >= ySquare) && (yPos < (ySquare + STEP_WIDTH))
                    % If (X, Y) land within the inner border boundary
                    if ((xPos < xSquare+BORDER_SQUARE) || (xPos >= xSquare+STEP_WIDTH-BORDER_SQUARE) || ...
                       (yPos < ySquare+BORDER_SQUARE) || (yPos >= ySquare+STEP_WIDTH-BORDER_SQUARE))
                        currFrame(yPos+1,xPos+1) = currColor; % R,G,B
                    end
                end
            end
            % update square position after frame is complete
            % If the "cursor" is in the bottom-right of the active region
            if (xPos == X_VISIBLE) && (yPos == Y_VISIBLE)
                % STATE: UPDATE_MOVE_CYCLE (frameWait, frameCounter)
                frameCounter = frameCounter + 1;
                % If waited long enough, move a block (but first update the variables)
                if frameCounter >= frameWait
                    frameCounter = 0;

                    % STATE: UPDATE_POSITION (diagonal move and bounce logic)
                    bounced = 0;
                    % If X lands outside the outer boundaries, flip the horizontal direction and flag a bounce
                    if xSquare <= X_MIN || xSquare >= X_MAX
                        dirX = -dirX; 
                        bounced = 1;
                    end
                    % If Y lands outside the outer boundaries, flip the vertical direction and flag a bounce
                    if ySquare <= Y_MIN || ySquare >= Y_MAX
                        dirY = -dirY; 
                        bounced = 1;
                    end
                    xSquare = xSquare + dirX;
                    ySquare = ySquare + dirY;

                    % STATE: UPDATE_COLOR
                    % If a bounce happened in this move, change the colour
                    if bounced
                        if currColor == COLOR_RED
                            currColor = COLOR_GREEN;
                        else
                            currColor = COLOR_RED;
                        end
                    end
                end
            end
        % -- end of your algorithm  -----------------------------------
        end
    end
    
    % convert and display
    visRGB8frame = matrix2RGB(currFrame,Y_VISIBLE,X_VISIBLE,SHOW_RGB_FRAME);

    % write videoframe to mp4 stream
    if CONVERT_TO_MPEG4
        writeVideo(v,visRGB8frame);
    end
end
toc
% close mp4 file
if CONVERT_TO_MPEG4
    close(v)
end

