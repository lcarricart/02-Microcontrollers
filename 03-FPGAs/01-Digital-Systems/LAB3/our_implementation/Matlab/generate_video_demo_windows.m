%===============================================================================
% Objective: create a Matlab video "rgb_movie" based on the assignment 9 of
% Laboratory 3, Digital Systems. Later, export the movie to a MPEG4 local file.
% Variant: bouncing 4-pane Windows-logo flag (screensaver style).
%===============================================================================
% Version 14/06/2026
% Authors:
%   - Luciano Carricart
%   - Imran Rizwan
%   - Georgii Molyboga
%===============================================================================

clear all;

% -- simulation settings (change here) -------------------------------
CONVERT_TO_MPEG4  = true; % produce mp4 file
SHOW_RGB_FRAME    = true; % show each RGB frame after being processed
SLOW_MOTION       = true; % produce video at 10 (slomo)/72(real-time) fps

% suppress warning that image too large for screen
warning('off','images:initSize:adjustingMag'); 

% -- video parameters (don't touch) ----------------------------------
X_FULL = 1040; Y_FULL = 666;      % screen size in pixels incl. blank region
X_VISIBLE = 800; Y_VISIBLE = 600; % visable screen size in pixels
FRAME_RATE = 72; % frame rate in Hz
VIDEO_LEN = 5;   % video length in sec
xPos = 0; yPos = 0; % current pixel position of CRT beam 
currFrame = zeros(Y_FULL,X_FULL); % current video frame as matrix
visRGB8frame = zeros(Y_VISIBLE,X_VISIBLE,3); % frame in matlab 8 bit RGB format 

% -- constants of Task 9 demo (Windows-logo variant) -----------------
STEP_WIDTH = 25;  % movement step per move (pixels) - keeps grid alignment
LOGO       = 50;  % size of the bouncing logo (pixels), multiple of STEP_WIDTH
PANE       = 23;  % size of each colored pane (pixels)
GAP        = 4;   % cross gap between panes (pixels): PANE + GAP + PANE = LOGO
X_START    = 375; % start x coord of logo (top left) - centered, grid aligned
Y_START    = 275; % start y coord of logo (top left) - centered, grid aligned

% Allowed range of movement (no border: logo bounces off the visible screen edges)
X_MIN = 0;
Y_MIN = 0;
X_MAX = X_VISIBLE - LOGO; % 800-50 = 750
Y_MAX = Y_VISIBLE - LOGO; % 600-50 = 550
UPDATE_HZ = 21;   % 21 logo moves per second to meet the requirement

% Windows-flag pane colors (R*256*256 + G*256 + B)
COL_RED    = 255*256*256;          % top-left pane
COL_GREEN  = 255*256;              % top-right pane
COL_BLUE   = 255;                  % bottom-left pane
COL_YELLOW = 255*256*256 + 255*256;% bottom-right pane

% open mp4 stream for writing
if CONVERT_TO_MPEG4
    v = VideoWriter('windows_movie.mp4','MPEG-4');
    if SLOW_MOTION
        v.FrameRate = 10;
    else
        v.FrameRate = 72;
    end
    v.Quality = 100;    
    open(v)
end

% -- reset values of Task 9 demo (Windows-logo variant) -----------------
xSquare = X_START; ySquare = Y_START;  % current upper-left corner of logo
velX = STEP_WIDTH;    % horizontal step per move (+ = right)
velY = -STEP_WIDTH;   % vertical step per move   (- = up) -> start to top-right
moveAcc = 0;          % phase accumulator to derive the 21Hz logo move from 72fps
% -------------------------------------------------------------------------------
tic
for N = [1:VIDEO_LEN*FRAME_RATE] % number of frames to be generated (72fps @ 800x600)
    % N is not available in the real-time system, don't rely on it in your
    % algorithm and create your own frame counter
    disp(N);
    currFrame = zeros(Y_FULL,X_FULL); % new blank frame (black background)
    for yPos = 0:Y_FULL-1      
        % yPos: vertical cathode ray pos, provided as input port in VHDL too
        for xPos = 0:X_FULL-1  
        % xPos: horizontal cathode ray pos, provided as input port in VHDL too

        % -- none of your code lines must be outside the region below ----
        % -- unless you set a reset value or constant definition above ---
        
        % -- Task 9 algorithm (Windows-logo variant) ---------------------
            
            % STATE: DISPLAY_PIXEL
            % create the 4-pane Windows logo (red/green/blue/yellow with cross gap)
            if xPos >= xSquare && xPos < (xSquare + LOGO)
                if yPos >= ySquare && yPos < (ySquare + LOGO)
                    dx = xPos - xSquare; % 0 .. LOGO-1
                    dy = yPos - ySquare; % 0 .. LOGO-1
                    % skip the cross gap between the panes
                    if (dx < PANE || dx >= PANE+GAP) && (dy < PANE || dy >= PANE+GAP)
                        if dy < PANE          % top row
                            if dx < PANE
                                currFrame(yPos+1,xPos+1) = COL_RED;    % top-left
                            else
                                currFrame(yPos+1,xPos+1) = COL_GREEN;  % top-right
                            end
                        else                  % bottom row
                            if dx < PANE
                                currFrame(yPos+1,xPos+1) = COL_BLUE;   % bottom-left
                            else
                                currFrame(yPos+1,xPos+1) = COL_YELLOW; % bottom-right
                            end
                        end
                    end
                end
            end
            % update logo position after frame is complete
            if (xPos == X_VISIBLE) && (yPos == Y_VISIBLE)
                % STATE: UPDATE_TIMER (derive 21Hz logo move from 72fps)
                moveAcc = moveAcc + UPDATE_HZ;
                if moveAcc >= FRAME_RATE
                    moveAcc = moveAcc - FRAME_RATE;
                    % STATE: UPDATE_POSITION (diagonal move + bounce)
                    % logo always exactly on an edge -> reverse and step back
                    if xSquare <= X_MIN || xSquare >= X_MAX
                        velX = -velX;
                    end
                    if ySquare <= Y_MIN || ySquare >= Y_MAX
                        velY = -velY;
                    end
                    xSquare = xSquare + velX;
                    ySquare = ySquare + velY;
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
