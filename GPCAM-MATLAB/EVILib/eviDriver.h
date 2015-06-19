#include "EVI-D70.h"

#include <math.h>
#include <queue>
#include <cstdarg>

#define DEBUG 0

#define LIMIT_X 170
#define LIMIT_Y 90
#define LIMIT_Y_MIN -30
#define LIMIT_Z 18
#define LIMIT_Z_MIN 0

#define EMPTY 0
#define PAN_TILT 1
#define ZOOM 2
#define FOCUS 3
#define AUTO_E 4
#define GAIN 5
#define BRIGHTNESS 6
#define EXPOSURE 7
#define STOP 8
#define CONTINUOUS_PAN 9

using namespace std;

struct Command
{
	int type;
	queue<double> theParams;
};

/**
	* Send commands we put into the buffer
	*/
void* sendCommands(void*);

class eviDriver{
	public:
		/**
			*  Create a new EVI Driver 
			*/
		eviDriver(char *port);

		/** 
			* Destroy an EVI Driver
			*/
		~eviDriver();

//On/off ---------------------------------------------------------------------------------------------------------
		/**
			* Turn the camera on
			* @param port The computer port the camera is plugged into, eg "/dev/ttyUSB0"
			* @return The result, -1 is a failure, 1 is a success
			*/
		int powerOn();

		/**
			* Turn off the camera
			* @return Result, -1 is a failure, 1 is success
			*/
		int powerOff();

//Image ----------------------------------------------------------------------------------------------------------

		/** Turn auto-exposure on or off
			* @param ae True if auto-exposure should be on
			*/
		void autoExposure(bool ae);

		/** 
			* Adjust the gain or brightness.
			* @param dir -1: down, 0: Reset, 1: up
			*/
		void gain(int dir);

		/**
			* Adjust the brightness.
			@param dir - 1: down, 0: Reset, 1: up
		*/
		//void brightness(int dir);

		/**
			* Adjust the exposure compensation.	
			* @param on True if 
			* @param ec -1: down, 0: Reset, 1: up
			*/
		void exposureComp(bool on, int ec);

		/**
			* Change the focus.
			* @param autoF True - auto focus on
			* @param closer True - focus closer, false, focus farther
			*/
		//void focus(bool autoF, bool closer);

//Moving ---------------------------------------------------------------------------------------------------------
		/**
			* Move the camera a distance of mx, my from its current location
			* @param mx The horizontal distance to move, degrees
			* @param my The vertical distance to move, degrees
			* @return Result, either an error code, or 1 on success
			*/
		void moveRelative(double, double);

		/**
			* Move the camera to (mx, my)
			* @param mx The x location to move to, degrees
			* @param my The y location to move to, degress
			* @return Result, either an error code, or 1 on success
			*/
		void moveAbsolute(double, double);
        
        /**
         * Pan the camera left at speed (speed)
         * @param speed The speed of the pan
         */
        void panLeft(double);
        
        /**
         * Pan the camera right at speed (speed)
         * @param speed The speed of the pan
         */
        void panRight(double);

        /**
         * Halt camera movement 
         */
        void stop();
        
		/**
			* Zoom the camera a relative amount
			* @param magnification The magnification
			*/
		void zoom(int magnification);

		/**
			* Update our location, note that further moves may be in the queue, and not yet implemented
			* @param type What type of values are these (absolute versus relative)
			* @param my Horizontal degrees
			* @param mz Vertical degrees 
			*/
		void updateLocation(int type, double mx, double my);

//Adding/Sending Commands ----------------------------------------------------------------------------------------		
		/**
			* Add a new command to the buffer, overwriting any waiting commands
			* @param type - The type of command we're adding
			* @param ... - Each command needs a different amount of parameters, the number of parameters 
			*		needed can be inferred from the command
			*/
		void addCommand(int type, int numargs, ...);

		/**
			* Clear all the commands in the buffer
			*/
		void clearCommands();

		/**
			* Are there more commands to send?
		*/
		bool hasCommands();

		/**
			Are we doing SOMETHING?
		*/
		bool isRunning();
        
        /** Is the camera open? */
        bool isOpen();

//----------------------------------------------------------------------------------------------------------------

		/** Our camera object */
		EVI_D70 *cam;

		/** Our command queue **/
		queue<Command> myBuffer;

		/** Message sending thread */
		pthread_t	tid;

		/** The current x location, degrees */
		double x;
		/** The current y location, degrees */
		double y;
		/** The current z location */
		double z;

		/** True if we are currently sending messages */
		bool running;
		/** True if a command is currently executing in the camera */
		bool executing;
        /** True if the camera is open **/
        bool open;
};
