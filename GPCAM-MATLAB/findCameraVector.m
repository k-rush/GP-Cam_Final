function outVector = findCameraVector(C, heading)
    heading = (heading+90)*(pi/180);
    ychange = sin(heading);
    xchange = cos(heading);
    outVector = [xchange,ychange,C(3)];
end