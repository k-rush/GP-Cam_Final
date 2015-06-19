import java.net.*;
import java.io.*;

%cam = EVI_D70('/dev/ttyUSB0');

 camPoint = [0,0,0];
 targetPoint = [0,0,0];
% Communication
msg = '';
prompt = 'Enter coordinates';
while 1
 msg = input(prompt,'s');
 C = strsplit(char(msg));
 %C = cell2mat(D);
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
     cv = findCameraVector(camPoint,0);
     disp('the camera vector is ');
     disp(cv);
     %cam.moveRelative(0, -20);
 end
 
 %TARGET VECTOR
 if(char(msg(1)) == 'T')
     disp('TARGET');
     disp('Northing:');
     disp(C(2));
     disp('Easting:');
     disp(C(3));
     disp('Altitude:');
     disp(C(4));
     targetPoint = [str2double(C(3)),str2double(C(2)),str2double(C(4))];
     disp(targetPoint);
     tv = findTargetVector(targetPoint,camPoint);
     disp('CAMERA VECTOR:');
     disp(cv);
     disp('TARGET VECTOR:');
     disp(tv);
     disp(findPanAngle(tv,cv));
     disp(findTiltAngle(tv,cv));
     %cam.moveRelative(findPanAngle(tv,cv),findTiltAngle(tv,cv));

 end
end