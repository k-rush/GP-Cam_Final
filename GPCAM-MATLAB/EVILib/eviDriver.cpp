#include "eviDriver.h"

/**
	*  Create a new EVI Driver 
	*/
eviDriver::eviDriver(char *port)
{
	cam = new EVI_D70();
  	if(cam == NULL)
 	{
		cout << "eviDriver::eviDriver() - Error creating the camera\n";
  	}
	
 	 if(cam->Init() != 1)
 	 {
		cout << "eviDriver::powerOn() - Error initializing the camera\n";
	 }
  
	// Open serial port
	if(cam->Open(1, port) != 1)
	{
		cout << "eviDriver::powerOn() - Error opening the port\n";
		open = false;
  	}
	else
	{
		open = true;
	}
    memset(&tid, 0, sizeof(pthread_t));
   if(DEBUG) cout << "eviDriver::eviDriver() - value of tid: " << tid << endl;
}

/** 
	* Destroy an EVI Driver
	*/
eviDriver::~eviDriver()
{   
    if(DEBUG) cout << "eviDriver::~eviDriver() - cleaning up\n";
    if(open)
	{
		void *status;
		clearCommands();
		running = false;
		if(DEBUG) cout << "eviDriver::~eviDriver - value of tid: " << tid << endl;
		if(tid)
    			pthread_join(tid, &status);
        cam->Close();
	}
    
	if(cam != NULL)
  {
  	delete cam;
    cam = NULL;
  }
}



//---------------------------On/Off----------------------------
/**
	* Turn the camera on
	* @param port The computer port the camera is plugged into, eg "/dev/ttyUSB0"
	* @return The result, -1 is a failure, 1 is a success
	*/
int eviDriver::powerOn()
{
	// We do a power ON only if the camera is not already ON.
  int status = cam->PowerInq();
  if(status == -1)
	{
		cout << "eviDriver::powerOn() - Error, could not get the camera's status\n";
  	return -1;
	}
  else if(status == EVILIB_OFF)
  {
		if(cam->Power(EVILIB_ON) != 1)
		{
			cout << "eviDriver::powerOn() - The camera failed to power on\n";  
			return -1;
		}
	}
 
	//Move camera to home position
	if(cam->Home() != 1)
		cout << "eviDriver::powerOn() - Couldn't set camera to home position\n";

	//initialize to location 0,0
	x = 0;
	y = 0;
	z = 0;
	
	//Testing this command out
	//cam->Display(EVILIB_ON, EVILIB_WAIT_COMP);
	//cam->Freeze(EVILIB_ON, EVILIB_NO_WAIT_COMP);

	cout << "eviDriver::powerOn() - Powered on successfully.\n";
	running = true;

	//start the message sending thread
	pthread_attr_t	attr;
	pthread_attr_init(&attr);	//Create and initialize a thread's attribute structure
	pthread_create(&tid, &attr, sendCommands, this);

	return 1;
}


/**
	* Turn off the camera
	* @return Result, -1 is a failure, 1 is success
	*/
int eviDriver::powerOff()
{
	int res;
	running = false;

	//clear all the commands, stop the current one
	if(hasCommands())
		clearCommands();

	if(cam->Power(EVILIB_OFF) != 1)
	{
		cout << "eviDriver::powerOff() - Failed to power off\n";
		res = -1;
	}
	
	return res;
}

//Image -------------------------------------------------------------------------------------------------------------------

/** Turn auto-exposure on or off
	* @param ae True if auto-exposure should be on
	*/
void eviDriver::autoExposure(bool ae)
{
	if(ae)
		addCommand(AUTO_E, 1, (double)EVILIB_AUTO);
	else
		addCommand(AUTO_E, 1, (double)EVILIB_MANUAL);
}


/** 
	* Adjust the gain or brightness.
	* @param g -1: down, 0: Reset, 1: up
	*/
void eviDriver::gain(int dir)
{
	if(dir < 0)
		addCommand(GAIN, 1, (double)EVILIB_DOWN);
	else if(dir > 0)
		addCommand(GAIN, 1, (double)EVILIB_UP);
	else
		addCommand(GAIN, 1, (double)EVILIB_RESET);
}

/*void eviDriver::brightness(int dir)
{
	if(dir < 0)
		addCommand(BRIGHTNESS, 1, (double)EVILIB_DOWN);
	else if(dir > 0)
		addCommand(BRIGHTNESS, 1, (double)EVILIB_UP);
	else
		addCommand(BRIGHTNESS, 1, (double)EVILIB_RESET);
}*/

/**
	* Adjust the exposure compensation.	
	* @param on True if 
	* @param ec -1: down, 0: Reset, 1: up
	*/
void eviDriver::exposureComp(bool on, int dir)
{

	if(on)
	{
		if(dir < 0)
			addCommand(EXPOSURE, 1, (double)EVILIB_DOWN);
		else if(dir > 0)
			addCommand(EXPOSURE, 1, (double)EVILIB_UP);
		else
			addCommand(EXPOSURE, 1, (double)EVILIB_RESET);
	}
	else
		addCommand(EXPOSURE, 1, (double)EVILIB_OFF);
}

//Moving -------------------------------------------------------------------------------------------------------------------
/**
	* Move the camera a distance of mx, my from its current location
	* @param deltaH - Change in horizontal degrees
	* @param deltaV - Change in vertical degrees
	*/
void eviDriver::moveRelative(double deltaH, double deltaV)
{
	addCommand(PAN_TILT, 3, (double)EVILIB_RELATIVE, deltaH, deltaV);
}


/**
	* Move the camera to (mx, my)
	* @param mx The x location to move to, degrees
	* @param my The y location to move to, degress
	* @return Result, either an error code, or 1 on success
	*/
void eviDriver::moveAbsolute(double finalX, double finalY)
{
	addCommand(PAN_TILT, 3, (double)EVILIB_ABSOLUTE, finalX, finalY);
}

void eviDriver::panLeft(double speed)
{
    if(speed < EVILIB_min_pspeed)
        speed = EVILIB_min_pspeed;
    else if(speed > EVILIB_max_pspeed)
        speed = EVILIB_max_pspeed;
    addCommand(CONTINUOUS_PAN, 2, (double)EVILIB_LEFT, speed);
}

void eviDriver::panRight(double speed)
{
    if(speed < EVILIB_min_pspeed)
        speed = EVILIB_min_pspeed;
    else if(speed > EVILIB_max_pspeed)
        speed = EVILIB_max_pspeed;
    addCommand(CONTINUOUS_PAN, 2, (double)EVILIB_RIGHT, speed);
}

/**
	* Zoom the camera to a specified magnification
	* @param magnification ... The magnification... really
	*/
void eviDriver::zoom(int magnification)
{
	addCommand(ZOOM, 2, (double)EVILIB_DIRECT, (double)magnification);
}

void eviDriver::updateLocation(int type, double mx, double my)
{
	if(DEBUG) cout << "eviDriver::updateLocation() - Got (" << type << ", " << mx << ", " << my << ")" <<endl;
	if(type == EVILIB_RELATIVE)
	{
		x += mx;
		y += my;
	}
	else if(type == EVILIB_ABSOLUTE)
	{
		x = mx;
		y = my;
	}

	(x > 170) ? x = 170 : x;
	(x < -170) ? x = -170 : x;
	(y > 90) ? y = 90 : y;
	(y < -30) ? y = -30 : y;
	if(DEBUG) cout << "eviDriver::updateLocation() - Camera \"at\" (" << x << ", " << y << ")\n";
}

void eviDriver::stop()
{
    addCommand(STOP, 1, (double)EVILIB_STOP);
}
//Adding/Sending Commands --------------------------------------------------------------------------------------------------
void eviDriver::addCommand(int type, int numargs, ...)
{
	Command currentCom;
	currentCom.type = type;
	double temp;	

	if(DEBUG) cout << "eviDriver::addCommand() - Adding command: " << currentCom.type << "\n";

	//Variable parameters code
	va_list curPar;
	va_start(curPar, numargs);
	for(int i = 0; i < numargs; i++)
	{
		temp = va_arg(curPar, double);
		if(DEBUG) cout << "eviDriver::addCommand() -  parameter #" << i + 1 << ": " << temp << "\n";
		currentCom.theParams.push(temp);
	}

	va_end(curPar);
	//----------------------

	// Our command is ready
	myBuffer.push(currentCom);
}

/**
	* Send commands we put into the buffer
	*/
void* sendCommands(void *arg)
{
  eviDriver *cd;
  cd = (eviDriver*)arg;

	int res;
	int value;
	double tempX, tempY, magnification;

	while(cd !=NULL && cd->running)
	{
		if(cd->cam->PanTiltInMove())
			sched_yield();

		if(cd->hasCommands())
		{
			cd->executing = true;
			Command currentCommand = cd->myBuffer.front();
			cd->myBuffer.pop();

			//send the next command
			switch(currentCommand.type)
			{
			case PAN_TILT: //EVILIB::Pan_TiltDrive
				//Get our values
				value = (int)currentCommand.theParams.front(); currentCommand.theParams.pop();
				tempX = currentCommand.theParams.front(); currentCommand.theParams.pop();
				tempY = currentCommand.theParams.front(); currentCommand.theParams.pop();
				
				//Call the cam
				res = cd->cam->Pan_TiltDrive(value, EVILIB_max_pspeed, EVILIB_max_tspeed, tempX, tempY, EVILIB_WAIT_COMP);

				if(res)
				{
					cd->updateLocation(value, tempX, tempY);
					if(DEBUG) {
						float mx, my;
						cd->cam->Pan_TiltPosInq(mx, my);
						cout << "eviDriver::sendCommands() - Camera at (" << mx << ", " << my << ")\n";
					}
				}
				else
					cout << "eviDriver::sendCommands() - Failed move.\n";

				break;
            		case CONTINUOUS_PAN:
                		value = (int)currentCommand.theParams.front(); currentCommand.theParams.pop();
                		tempX = currentCommand.theParams.front(); currentCommand.theParams.pop();
              			if(DEBUG) cout << "eviDriver::sendCommands - Attempting to continuously pan " << value << " with speed " << tempX << ".\n";
		                res = cd->cam->Pan_TiltDrive(value, tempX, EVILIB_max_tspeed, 0, 0, EVILIB_WAIT_COMP);
                		if (DEBUG) cout << "eviDriver::sendCommands - Result: " << res << endl;
                		break;
		        case STOP:
				if(DEBUG) cout << "eviDriver::sendCommands - About to stop a move command." << endl;
               			value = (int)currentCommand.theParams.front(); currentCommand.theParams.pop();
		                res = cd->cam->Pan_TiltDrive(value, 1, 1, 1, 1, EVILIB_WAIT_COMP);
                		break;
			case ZOOM:
				//Get our values
				value = (int)currentCommand.theParams.front(); currentCommand.theParams.pop();
				magnification = currentCommand.theParams.front(); currentCommand.theParams.pop();
				
				res = cd->cam->Zoom(EVILIB_DIRECT, EVILIB_max_zspeed, magnification, EVILIB_WAIT_COMP);
				if(DEBUG) cout << "eviDriver::sendCommands() - Zoomed to: " << magnification << "\n";
				break;

			/*case FOCUS:
				res = cd->cam->Focus(currentCommand.x, 0, EVILIB_NO_WAIT_COMP);
				cd->updateLocBuff();
				printf( "eviDriver::sendCommands() - Changed the focus\n");
				break;*/
			case AUTO_E:
				//Pull out our values
				value = (int)currentCommand.theParams.front(); currentCommand.theParams.pop();

				//Call cam command
				res = cd->cam->AE(value, EVILIB_WAIT_COMP);
				if(res)
					cout << "eviDriver::sendCommands() - Auto Exposure changed to " << res << ".\n";
				else
					cout << "eviDriver::sendCommands() - Auto exposure change failed.\n";

				break;
			case GAIN:
				//Pull out our values
				value = (int)currentCommand.theParams.front(); currentCommand.theParams.pop();

				//Call cam command

				res = cd->cam->Gain(value, 0, EVILIB_WAIT_COMP);
				if(res)
					cout << "eviDriver::sendCommands() - Gain adjusted.\n";
				else
					cout << "eviDriver::sendCommands() - Gain adjust failed.\n";

				break;
			/*case BRIGHTNESS:
				//Pull out our values
				value = (int)currentCommand.theParams.front(); currentCommand.theParams.pop();

				//Call cam command
				res = cd->cam->Bright(value, 0, EVILIB_WAIT_COMP);
				if(res)
					printf( "eviDriver::sendCommands() - Brightness adjusted.\n");
				else
					printf(  "eviDriver::sendCommands() - Brightness adjust failed.\n");

				break;*/
			case EXPOSURE:
				//Pull out our values
				value = (int)currentCommand.theParams.front(); currentCommand.theParams.pop();

				if(value != EVILIB_OFF && cd->cam->ExpCompModeInq() == EVILIB_OFF)
					cd->cam->ExpComp(EVILIB_ON, 0, EVILIB_WAIT_COMP);

				//Call cam command
				res = cd->cam->ExpComp(value, 0, EVILIB_WAIT_COMP);
				if(res)
					cout << "eviDriver::sendCommands() - Exposure adjusted\n";
				else
					cout << "eviDriver::sendCommands() - Exposure adjust failed.\n";

				break;

			case EMPTY:	break;

			default:
				cout << "eviDriver::sendCommands() - ERROR: Unknown command type: "<< currentCommand.type << "\n";
				break;
			}

			cd->executing = false;
		}
	}

	pthread_exit(0);	//Terminate the thread
}

/**
	* Clear all the commands in the buffer
	*/
void eviDriver::clearCommands()
{
	if(DEBUG) cout << "eviDriver()::clearCommands() - clearing the buffer\n";
	while(!myBuffer.empty())
		myBuffer.pop();
}

bool eviDriver::hasCommands()
{
	return !myBuffer.empty();
}

bool eviDriver::isRunning()
{
	return (executing || hasCommands());
}

bool eviDriver::isOpen()
{
    return open;
}

