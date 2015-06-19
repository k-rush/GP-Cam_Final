/*
  C++ mex to control the EVI-D70 camera

	Compile with: mex eviCam.cpp ./EVILib/EVILib.a -I./EVILib
*/
#include "mex.h"
#include <eviDriver.h>

using namespace std;

eviDriver* driver = NULL;
bool on;
bool initialRun = true;

/* Helper method for getting the input string from prhs
 * Make sure to pass in in a index, and the
 * buffer by reference
 */
bool getInput(const mxArray *temp, char* &buf)
{
    int buflen = mxGetN(temp)*sizeof(mxChar)+1;
    buf = (char *)mxMalloc(buflen);
   /* Get the input string */
    if ( mxGetString(temp, buf, mxGetN(temp) + 1) != 0 ) { //FIX THIS LINE, make sure you transfer memory correctly.
       return 0;
    }

    return 1;
}

static void mexExit()
{
    mexPrintf(" -- Cleaning up driver --\n");
    if(driver != NULL && driver->isOpen())
    {
        mexPrintf(" -- Deleting things --\n");
        delete driver;
    }
}

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {	
    
    /* Anything that only need to be run once */
    if(initialRun)
    {
        mexPrintf("Setting up initial settings...\n");
        mexAtExit(mexExit);
        initialRun = false;
    }
    
	char *buf;
	if(nrhs > 0 && getInput(prhs[0], buf))
	{
        //Initiate driver if it hasn't been initiated yet.
        if(driver == NULL)
        {
            if(DEBUG) mexPrintf("Got input: %s\n", buf);
            driver = new eviDriver(buf); //Did it open? Or did it fail...
            if(!driver->isOpen())
            {
                delete driver;
                driver = NULL;
                mexErrMsgTxt("Could not open, did you give the right port?");
            }
        }

    	//Power on camera
    	else if (strcmp (buf, "On") == 0)
		{		
			driver->powerOn();
			on = true;
		}
    	//Power off camera
		else if (strcmp (buf, "Off") == 0)
		{
			driver->powerOff();
		}
		//Set AutoExposure
		/*else if (strcmp (buf, "AutoExposure") == 0)
		{
			if (nrhs != 2){
				mexPrintf("invalid parameter count: eviCam('AutoExposure',<exposure>");
				return;
			}
			if (!on){
				mexPrintf("error : must first call eviCam('On')\n");
				return;
			}

			double *ae = mxGetPr(prhs[1]);
			driver.autoExposure(*ae);
		}
		//Set Brightness
		else if (strcmp (buf, "GainBrightness") == 0)
		{
			if (nrhs != 3){
				mexPrintf("invalid parameter count: eviCam('Brightness',<type>,<gain>");
				return;
			}
			if (!on){
				mexPrintf("error : must first call eviCam('On')\n");
				return;
			}

			double *type = mxGetPr(prhs[1]);
			double *g = mxGetPr(prhs[2]);
			driver.gainBrightness((int)*type,(int)*g);
		}
		//Set Exposure
		else if (strcmp (buf, "ExposureComp") == 0)
		{
			if (nrhs != 3){
				mexPrintf("invalid parameter count: eviCam('ExposureComp',<on/off>,<ec>");
				return;
			}
			if (!on){
				mexPrintf("error : must first call eviCam('On')\n");
				return;
			}

			double *on = mxGetPr(prhs[1]);
			double *ec = mxGetPr(prhs[2]);
			driver.exposureComp(*on, (int)*ec);
		}
		//Set Focus
		else if (strcmp (buf, "Focus") == 0)
		{
			if (nrhs != 3){
				mexPrintf("invalid parameter count: eviCam('Focus',<auto>,<closer>");
				return;
			}
			if (!on){
				mexPrintf("error : must first call eviCam('On')\n");
				return;
			}

			double *autoF = mxGetPr(prhs[1]);
			double *closer = mxGetPr(prhs[2]);
			driver.focus(*autoF, *closer);
		}*/
    
    	//Move Relative
		else if (strcmp (buf, "MoveRelative") == 0)
		{
        	if (nrhs != 3 || (!mxIsDouble(prhs[1]) || !mxIsDouble(prhs[2]))){
				mexPrintf("Incorrect parameter setup: eviCam('MoveRelative', <Scalar>, <Scalar>\n");
				return;
			}
			if (!on){
				mexPrintf("error : must first call eviCam('On')\n");
				return;
			}
			double mx = mxGetScalar(prhs[1]);
			double my = mxGetScalar(prhs[2]);
			driver->moveRelative(mx, my);
    	}
		//Set Location
    	else if (strcmp (buf, "MoveAbsolute") == 0)
		{
			if (nrhs != 3 || (!mxIsDouble(prhs[1]) || !mxIsDouble(prhs[2]))){
				mexPrintf("Incorrect parameter setup: eviCam('SetLocation', <Scalar>, <Scalar>\n");
				return;
			}
			if (!on){
				mexPrintf("error : must first call eviCam('On')\n");
				return;
			}
			double mx = mxGetScalar(prhs[1]);
    		double my = mxGetScalar(prhs[2]);
			driver->moveAbsolute(mx, my);
		}
        //Continuous Panning
        else if (strcmp (buf, "PanLeft") == 0)
        {
            if(nrhs != 2 || !mxIsDouble(prhs[1])) {
                mexPrintf("Incorrect parameter setup: eviCam('PanRight', <scalar>)\n");
                return;
            }
            if(!on) {
                mexPrintf("error : must first call eviCam('On')\n");
                return;
            }
            double speed = mxGetScalar(prhs[1]);
            driver->panLeft(speed);
        }
        else if (strcmp (buf, "PanRight") == 0)
        {
            if(nrhs != 2 || !mxIsDouble(prhs[1])) {
                mexPrintf("Incorrect parameter setup: eviCam('PanRight', <scalar>)\n");
                return;
            }
            if(!on) {
                mexPrintf("error : must first call eviCam('On')\n");
                return;
            }
            double speed = mxGetScalar(prhs[1]);
            driver->panRight(speed);
        }
        else if (strcmp (buf, "STOP") == 0)
        {
            if(!on) {
                mexPrintf("error : must first call eviCam('On')\n");
                return;
            }
            driver->stop();
        }
		//Zoom Relative
		else if (strcmp (buf, "Zoom") == 0)
		{
			if (nrhs != 2 || (!mxIsDouble(prhs[1]))){
				mexPrintf("Incorrect parameter setup: eviCam('Zoom', <Scalar>\n");
				return;
			}
			if (!on){
				mexPrintf("error : must first call eviCam('On')\n");
				return;
			}
			int mz = (int)mxGetScalar(prhs[1]);
			driver->zoom(mz);
		}
		//Clear Commands
		else if (strcmp (buf, "ClearCommands") == 0)
		{
			driver->clearCommands();
		}
		else if (strcmp (buf, "IsRunning") == 0)
    	{
			plhs[0] = mxCreateLogicalScalar(driver->isRunning());
    	}
		else{
			mexPrintf("Unknown command.\n");
		}
	}
	else
	{
		mexErrMsgTxt("Could not get command, please start with a command.");
	}
    
    if(DEBUG) mexPrintf("Freeing input buffer...\n");
    mxFree(buf);
    
    plhs[0] = mxCreateDoubleScalar(1);
} 
