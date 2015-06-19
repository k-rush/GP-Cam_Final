classdef EVI_D70 < handle
    % EVI_D70 < handle
    % version 1.0 : Last Edited 04/19 -CAS
    %
    % DESCRIPTION:
    %
    % This gives you all functionality for the EVI-D70 camera in matlab.
    % It makes assumptions when setting up the EVI-D70 Unicap Image
    % capture.  Such as that the DFG1394 is device 0, in
    % Unicap.listDevices().
    %
    %
    % METHODS:
    %
    % - Evi_D70(init_info)	(constructor)
    % - img = getframe()
    % - [x, y] = getresolution()
    % - camera.moveRelative(dx, dy);
    % - camera.moveAbsolute(x, y);
    % - camera.zoom(level);
    % - delete()			(deconstructor)
    %
    %
    % USE:
    % Call the constructor. The constructor for EVI_D70 takes the USB port
    % which the camera is connected to (ie '/dev/ttyUSB0').  Use
    % moveRelative and moveAbsolute to pan/tilt the camera.  Use zoom to
    % obviously zoom the camera. Get images using the getframe
    % method (of the standard camera interface). Destroy the camera handle
    % and deinit the camera using the delete method (of the standard camera
    % interface).
    %
    % EXAMPLE:
    %
    % c = EVI_D70('/dev/ttyUSB0');
    % c.moveRelative(-90, 10);
    % c.zoom(5);
    % c.getframe();
    % delete(c);
 
    properties(SetAccess = private, GetAccess = public)
        imageAcq;
    end
    
    methods
        function cam = EVI_D70(port)
            if(~ischar(port))
                display('[EVI_D70]Error: Constructor variable needs to be a string.');
                delete(cam);
                return;
            end
            %cam.imageAcq = UnicapAPI();
            if(eviCam(port));
                eviCam('On');
            end
            
            %Set up Unicap
            %cam.imageAcq.setDevice(0); %Assuming DFG1394 is device 0
            %cam.imageAcq.setProperty('source', 5); %Auto detect source
            %cam.imageAcq.setProperty('video norm', 1); %Set NTSC video norm
            %cam.imageAcq.setVideoFormat(0, 1); %UYVY 640x480 setting
        end
        
        %Movement functions
        function moveRelative(cam, dx, dy)
            eviCam('MoveRelative', dx*(136/90), dy);
        end
        
        function moveAbsolute(cam, dx, dy)
            eviCam('MoveAbsolute', dx*(136/90), dy);
        end
        
        function panLeft(cam, speed)
            eviCam('PanLeft', speed);
        end
        
        function panRight(cam, speed)
            eviCam('PanRight', speed);
        end
        
        function zoom(cam, value)
            eviCam('Zoom', value);
        end
        
        function stop(cam)
            eviCam('STOP');
        end
        
        function status = isRunning(cam)
            status = eviCam('IsRunning');
        end
        
        %Image Functions
        function img = getframe(cam)
            img = unix('ffmpeg -f video4linux2 -i /dev/video1 out.mpeg');
        end
        
        %Clean up functions
        function clearCommands(cam)
            eviCam('ClearCommands');
        end
        
        %Camera control
        function On(cam)
            eviCam('On');
        end
        
        function Off(cam)
            eviCam('Off');
        end
        
        function delete(cam)
            delete(cam.imageAcq);
            clear eviCam;
        end
    end
end
