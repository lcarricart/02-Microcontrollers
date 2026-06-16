function      visRGB8frame = matrix2RGB(currFrame,Y_VISIBLE,X_VISIBLE,SHOW_RGB_FRAME)
    
    visRGBframe = zeros(Y_VISIBLE,X_VISIBLE,3);
    
    visRGBframe(1:Y_VISIBLE,1:X_VISIBLE,1) = floor(currFrame(1:Y_VISIBLE,1:X_VISIBLE)/2^16); % R
    
    visRGBframe(1:Y_VISIBLE,1:X_VISIBLE,2) = floor((currFrame(1:Y_VISIBLE,1:X_VISIBLE)...
                                - visRGBframe(1:Y_VISIBLE,1:X_VISIBLE,1)*2^16)/2^8); % G
    
    visRGBframe(1:Y_VISIBLE,1:X_VISIBLE,3) = floor((currFrame(1:Y_VISIBLE,1:X_VISIBLE)...
                                - visRGBframe(1:Y_VISIBLE,1:X_VISIBLE,1)*2^16 ...
                                - visRGBframe(1:Y_VISIBLE,1:X_VISIBLE,2)*2^8)); % B
    
    visRGB8frame = im2uint8(visRGBframe/255);
    
    if SHOW_RGB_FRAME
        figure(1);
        imshow(visRGB8frame);
        drawnow;
    end
end

