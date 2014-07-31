/*
 * follow_segment.c
 *
 * This file just contains the follow_segment() function, which causes
 * 3pi to follow a segment of the maze until it detects an
 * intersection, a dead end, or the finish.
 *
 */

#include <pololu/3pi.h>

void follow_segment(int velocidade)
{
	int last_proportional = 0;
	long integral=0;

	// set up the 3pi
    int kp;
    float kd;
    
    //setting constants kp, kd 
    if(velocidade <= 120){
        kp = 20;
        kd = 3/2;
    }else if(velocidade <= 150){
        kp = 15;
        kd = 4/3;
    }else if(velocidade <= 200){
        kp = 10;
        kd = 4/3;
    }else if(velocidade <= 220){
        kp = 6;
        kd = 2;
    }else{
        kp = 5;
        kd = 9/4;
    }

	while(1)
	{
		// Normally, we will be following a line.  The code below is
		// similar to the 3pi-linefollower-pid example, but the maximum
		// speed is turned down to 60 for reliability.

		// Get the position of the line.
		unsigned int sensors[5];
		unsigned int position = read_line(sensors,IR_EMITTERS_ON);

		// The "proportional" term should be 0 when we are on the line.
		int proportional = ((int)position) - 2000;

		// Compute the derivative (change) and integral (sum) of the
		// position.
		int derivative = proportional - last_proportional;
		integral += proportional;

		// Remember the last position.
		last_proportional = proportional;

		// Compute the difference between the two motor power settings,
		// m1 - m2.  If this is a positive number the robot will turn
		// to the left.  If it is a negative number, the robot will
		// turn to the right, and the magnitude of the number determines
		// the sharpness of the turn.0
		int power_difference = proportional/kp + integral/15000 + derivative*kd;

		// Compute the actual motor settings.  We never set either motor
		// to a negative value.
		const int max = velocidade; // the maximum speed
		if(power_difference > max)
			power_difference = max;
		if(power_difference < -max)
			power_difference = -max;
		
		if(power_difference < 0)
			set_motors(max+power_difference,max);
		else
			set_motors(max,max-power_difference);

		// We use the inner three sensors (1, 2, and 3) for
		// determining whether there is a line straight ahead, and the
		// sensors 0 and 4 for detecting lines going to the left and
		// right.

		if(sensors[1] < 100 && sensors[2] < 100 && sensors[3] < 100)
		{
			// There is no line visible ahead, and we didn't see any
			// intersection.  Must be a dead end.
			return;
		}
		else if(sensors[0] > 200 || sensors[4] > 200)
		{
			// Found an intersection.
			return;
		}

	}
}

// Local Variables: **
// mode: C **
// c-basic-offset: 4 **
// tab-width: 4 **
// indent-tabs-mode: t **
// end: **
