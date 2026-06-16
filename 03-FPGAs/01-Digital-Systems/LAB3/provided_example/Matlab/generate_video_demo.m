% create Matlab video rgb_movie
% spiral demo
% export movie to MPEG4 local file
% LTL, 10.12.2020

clear all;

% -- simulation settings (change here) -------------------------------
CONVERT_TO_MPEG4  = true; % produce mp4 file
SHOW_RGB_FRAME    = true; % show each RGB frame after being processed
SLOW_MOTION       = false; % produce video at 10 (slomo)/72(real-time) fps

% suppress warning that image too large for screen
warning('off','images:initSize:adjustingMag'); 

% -- video parameters (don't touch) ----------------------------------
X_FULL = 1040; Y_FULL = 666;      % screen size in pixels incl. blank region
X_VISIBLE = 800; Y_VISIBLE = 600; % visable screen size in pixels
FRAME_RATE = 72; % frame rate in Hz
VIDEO_LEN = 10;   % video length in sec
xPos = 0; yPos = 0; % current pixel position of CRT beam 
currFrame = zeros(Y_FULL,X_FULL); % current video frame as matrix
visRGB8frame = zeros(Y_VISIBLE,X_VISIBLE,3); % frame in matlab 8 bit RGB format 

% -- constants of spiral demo (define your own here) -----------------
STEP_WIDTH = 32; % size of square and ste width
DIR_VEC = [-1,0; 0,-1; 1,0; 0,1]; % direction vectors
X_START = floor(X_VISIBLE/(2*STEP_WIDTH))*STEP_WIDTH;  % start x coord of square
Y_START = floor(Y_VISIBLE/(2*STEP_WIDTH))*STEP_WIDTH;  % start y coord of square 

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

% -- reset values of spiral demo  (define your own here) ------------------------
xSquare=X_START; ySquare=Y_START;  % current upper right corner of square 
segmLength = 1;       % number of successive steps to be taken in one direction ("segment")
segmIndex = 1;        % position of square in current segment
changeSegLength = 0;  % toggle value to change segment length only every 2nd time  
currDir = 0;          % 0:west, 1: north, 2:east, 3:south
indexColor = 0;       % color index 2: red, 1:green, 0:blue
currColor = 255;      % current color 255*256^indexColor
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
        
        % -- start of your algorithm  -----------------------------------
            
            % STATE: DISPLAY_PIXEL
            % create grid 
            if mod(yPos,STEP_WIDTH)==0 || mod(xPos,STEP_WIDTH)==0
                    currFrame(yPos+1,xPos+1) = 8947848; % dark gray
            end
            % create square
            if xPos >= xSquare && xPos < (xSquare + STEP_WIDTH),
                if yPos >= ySquare && yPos < (ySquare + STEP_WIDTH),
                    currFrame(yPos+1,xPos+1) = currColor; % R,G,B                    
                end
            end
            % update square position after frame is complete
            if (xPos == X_VISIBLE) && (yPos == Y_VISIBLE)
                % STATE: UPDATE_POSITION 
                if (ySquare >= STEP_WIDTH) && (ySquare <= Y_VISIBLE-STEP_WIDTH)
                    xSquare = xSquare + DIR_VEC(currDir+1,1)*STEP_WIDTH;
                    ySquare = ySquare + DIR_VEC(currDir+1,2)*STEP_WIDTH;
                else                    
                    xSquare=X_START; ySquare=Y_START;
                    segmIndex = 1;
                    segmLength = 1;
                    changeSegLength = 0;                    
                end
                %STATE: UPDATE_SEGMENT            
                if segmIndex < segmLength
                    segmIndex = segmIndex + 1;
                else
                    if changeSegLength
                        segmLength = segmLength + 1;
                        changeSegLength = 0;
                    else                        
                        changeSegLength = 1;
                    end
                    segmIndex = 1;
                    currDir = mod(currDir+1,4);
                end 
                %STATE: UPDATE_COLOR
                indexColor = mod(indexColor+1,3);
                currColor=255*256.^indexColor;
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

