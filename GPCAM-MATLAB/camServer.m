import java.net.*;
import java.io.*;
clearJava();
cam = EVI_D70('/dev/ttyUSB0');
 
% Create Socket
port = input('Enter port number: ');
Server = ServerSocket (port);
 
% Listen to connection
disp('Waiting for a connection...')
connected = Server.accept;
 
iStream = connected.getInputStream;
 
 

 camPoint = [0,0,0];
 targetPoint = [0,0,0];
% Communication
msg = '';
while isempty(strfind(msg,'!q'))
 % Waits for messages from client
 while ~(iStream.available)
       pause(.1);
 end
 
 msg = readS(iStream);
 disp(char(msg));
 C = strsplit(char(msg));
 %CALIBRATE
 if(char(msg(1)) == 'C')
     
     disp('CALIBRATE');
     disp('Northing:');
     disp(C(2));
     disp('Easting:');
     disp(C(3));
     disp('Altitude:');
     disp(C(4));
     camPoint = [str2double(C(3)),str2double(C(2)),str2double(C(4))];
     disp('the camera point is ');
     disp(camPoint);
     % the findCameraVector(camPoint, heading) function returns a vector
     % that is simply the the difference between the camera's calibrated point 
     % and a point 1 meter in the direction of heading
     cv = findCameraVector(camPoint,0);
     disp('the camera vector is ');
     disp(cv);
     cam.moveAbsolute(0, 0);
 end

 %TARGET
 %every time that a target point is received, we calibrate the needed pan
%and tilt angles along with zoom factor
 if(char(msg(1)) == 'T')
     disp('TARGET');
     disp('Northing:');
     disp(C(2));
     disp('Easting:');
     disp(C(3));
     disp('Altitude:');
     disp(C(4));
     targetPoint = [str2double(C(2)),str2double(C(3)),str2double(C(4))];
     disp(targetPoint);
     tv = findTargetVector(targetPoint,camPoint);
     disp('CAMERA VECTOR:');
     disp(cv);
     disp('TARGET VECTOR:');
     disp(tv);
     pan = findPanAngle(tv,cv);
     disp(pan);
     tilt = findTiltAngle(tv,cv);
     disp(tilt);
     cam.moveAbsolute(pan, tilt);
     distance = sqrt(tv(1)*tv(1)+tv(2)*tv(2)+tv(3)*tv(3));
     %determine the amount of zoom necessary using distance
     if (distance >90)
         cam.zoom(18);
     end
     if (distance <=90 && distance > 85)
         cam.zoom(17);
     end
     if (distance <=85 && distance > 80)
         cam.zoom(16);
     end
     if (distance <=80 && distance > 75)
         cam.zoom(15);
     end
     if (distance <=75 && distance > 70)
         cam.zoom(14);
     end
     if (distance <=70 && distance > 65)
         cam.zoom(13);
     end
     if (distance <=65 && distance > 60)
         cam.zoom(12);
     end
     if (distance <=60 && distance > 55)
         cam.zoom(11);
     end
     if (distance <=55 && distance > 50)
         cam.zoom(10);
     end
     if (distance <=50 && distance > 45)
         cam.zoom(9);
     end
     if (distance <=45 && distance > 40)
         cam.zoom(8);
     end
     if (distance <=40 && distance > 35)
         cam.zoom(7);
     end
     if (distance <=35 && distance > 30)
         cam.zoom(6);
     end
     if (distance <=30 && distance > 25)
         cam.zoom(5);
     end
     if (distance <=25 && distance > 20)
         cam.zoom(4);
     end
     if (distance <=20 && distance > 15)
         cam.zoom(3);
     end
     if (distance <=15 && distance > 10)
         cam.zoom(2);
     end
     if (distance <=10)
         cam.zoom(1);
     end
     disp('THIS IS THE DISTANCE (m)');
     disp(distance);

 end
 
end

 
pause(1)
connected.close;
disp (['Connection ended: ' datestr(now)]);
