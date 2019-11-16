#include <iostream>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include <chrono>

using namespace std::chrono;

#define TRIG 4
#define ECHO 5
#define LEFT_BUMPER 6 
#define RIGHT_BUMPER 25

enum DetectSM{Wait, Bump_Left, Bump_Right, Wall} detect_state;

unsigned char buffer[100];

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
           	 if(digitalRead(LEFT_BUMPER)){
				detect_state = Bump_Left;
			}
			else if(digitalRead(RIGHT_BUMPER)){
				detect_state = Bump_Right;
			}
			else if(curDist <= 5.0){
				detect_state = Wall;
			}
			else{
				detect_state = Wait;
			}
			break;

		case Bump_Left:
			if(digitalRead(LEFT_BUMPER)){
				detect_state = Bump_Left;

			}
			else{
				detect_state = Wait;
			}

			break;
		case Bump_Right:
			if(digitalRead(RIGHT_BUMPER)){
				detect_state = Bump_Right;
			}
			else{
				detect_state = Wait;
			}
			break;
		case Wall:
			curDist = dist_from_wall();
			if(curDist <= 5.0){
				detect_state = Wall;
			}
			else{
				detect_state = Wait;
			}
			break;


	}

	switch(detect_state){
		case Wait:
			digitalWrite(0, 0);
			buffer[0] = 0x01;
			wiringPiSPIDataRW(0, buffer, 1);
			break;

		case Bump_Left:
			digitalWrite(0, 1);
			buffer[0] = 0x02;
			wiringPiSPIDataRW (0, buffer, 1);
			break;

		case Bump_Right:
			digitalWrite(0, 1);
			buffer[0] = 0x03;
			wiringPiSPIDataRW(0, buffer, 1);
			break;

		case Wall:
			digitalWrite(0, 1);
			buffer[0] = 0x03;
			wiringPiSPIDataRW (0, buffer, 1);
			break;

	}
}

int main(){
	wiringPiSetup();
	wiringPiSPISetup(0, 500000);
	pinMode(TRIG, OUTPUT);
	pinMode(ECHO, INPUT);
	pinMode(0, OUTPUT);
	pinMode(LEFT_BUMPER, INPUT);
	pinMode(RIGHT_BUMPER, INPUT);

	digitalWrite(TRIG, 0);
	delay(2000);

	buffer[0] = 0x01;
	wiringPiSPIDataRW (0, buffer, 1);

	//Initialize state variables
	detect_state = Wait;

	while(1){
		buffer[0] = 0x01;
		wiringPiSPIDataRW(0, buffer, 1);
		Obj_Detection();

		delay(50);

	}

	return 0;
}

