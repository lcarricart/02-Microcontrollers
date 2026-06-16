% read video frame data from ModelSim simulation
% convert to 8-bit RGB Matlab image format
% create Matlab video rgb_movie
% export movie to MPEG4 local file
% LTL, 13.12.2020

clear all;

SLOW_MOTION = false; % produce video at 10 (slomo)/72(real-time) fps


warning('off','images:initSize:adjustingMag'); % suppress warning that image too large for screen

v = VideoWriter('rgb_movie.mp4','MPEG-4');
if SLOW_MOTION
    v.FrameRate = 10;
else
    v.FrameRate = 72;
end
v.Quality = 100;

open(v)

fileNotFound = 0;

% load the modelsim video output
tic
for N = [1:720]
    fprintf('frame: %03d \n',N);
    try
        eval(sprintf('VIDEO_OUT_%04d',N))
    catch EXCEPTID
        fprintf('VIDEO_OUT_%03d.m not found. Retry in 30s.\n',N);
        pause(30);
        try 
            eval(sprintf('VIDEO_OUT_%04d',N))
        catch EXCEPTID2
            fprintf('Giving up, check whether and where Modelsim has placed the frames.\n',N);
            break;
        end
    end
    
    % convert modelsim file to movie frame
    rgb_frame(:,:,1) = bitshift(frame_mds(:,:),-16);
    rgb_frame(:,:,2) = bitand(bitshift(frame_mds(:,:),-8),255);
    rgb_frame(:,:,3) = bitand(frame_mds(:,:),255);
    
    rgb_pic = im2uint8(double(rgb_frame)/255);
    imshow(rgb_pic);
    drawnow;

    writeVideo(v,im2frame(rgb_pic));
    toc
end
fprintf('Closing video stream.\n',N);
close(v)


