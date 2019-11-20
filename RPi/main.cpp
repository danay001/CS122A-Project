#include <iostream>
#include <wiringPi.h>]
#include <wiringPiSPI.h>
#include <chrono>

using namespace std::chrono;

#define TRIG 4
#define ECHO 5

enum DetectSM{Wait, Bump, Wall} detect_state;

unsigned long time_us(){
    microseconds us = duration_cast< microseconds >(system_clock::now().time_since_epoch());
	return us.count();
}

double dist_from_wall(){
	double dist = 0.0;
	unsigned long pulse_start = 0;
	unsigned long pulse_end = 0;
	unsigned long pulse_duration = 0;

	digitalWrite(TRIG, 1);
	delayMicroseconds(10);
	digitalWrite(TRIG, 0);
		
	while(digitalRead(ECHO) == 0){
		pulse_start = time_us();  //Get timestap right before rising edge of ECHO
	}

	while(digitalRead(ECHO)){
		pulse_end = time_us();	//Get timestamp right before falling edge of ECHO
	}

	pulse_duration = pulse_end - pulse_start;
	dist = static_cast<double>(pulse_duration) / 58;  //Get distance in cm

	return dist;
}

void Obj_Detection(){
	static double curDist = 0.0;
	switch(detect_state){
		case Wait:
			curDist = dist_from_wall();
            if(digitalRead(6)){
				detect_state = Bump;
			}
			else if(curDist <= 5.0){
				detect_state = Wall;
			}
			else{
				detect_state = Wait;
			}
			break;

		case Bump:
			if(digitalRead(6)){
				detect_state = Bump;

			}
			else{
				detect_state = Wait;
			}

			break;
		case Wall:
			detect_state = Wait;
			break;


	}

	switch(detect_state){
		case Wait:
			digitalWrite(0, 0);
			break;

		case Bump:
			digitalWrite(0, 1);
			wiringPiSPIDataRW (0, 0x02, 8);
			break;

		case Wall:
			digitalWrite(0, 1);
			wiringPiSPIDataRW (0, 0x03, 8);
			break;

	}
}

int main(){
	wiringPiSetup();
	wiringPiSPISetup(0, 8000000);
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);

	digitalWrite(TRIG, 0);
	delay(2000);

	wiringPiSPIDataRW (0, 0x01, 8);

	//Initialize state variables
	detect_state = Wait;

	while(1){
		Obj_Detection();

		delay(50);

	}

	return 0;
}

