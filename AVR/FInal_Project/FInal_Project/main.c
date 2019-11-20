/*
 * FInal_Project.c
 *
 * Created: 11/18/2019 1:15:36 PM
 * Author : Anaya
 */ 

#include <avr/io.h>
#include "SPI.h"
#include "timer.h"

enum MotorState{OFF, ON, RIGHT, LEFT}motor_state;
	
// SPI Commands from PI:
// 	0x01	Turn ON motors
// 	0x02	Turn RIGHT
// 	0x03	Turn LEFT
	
void MotorControl(){
	static unsigned short cnt = 0;
	switch(motor_state){  //State Transitions
		case OFF:
			if(receivedData == 0x01){
				motor_state = ON;
			}
			else{
				motor_state = OFF;
			}
		break;
		
		case ON:
			if(receivedData == 0x02){
				motor_state = RIGHT;
			}
			else if(receivedData == 0x03){
				motor_state = LEFT;
			}
			else{
				motor_state = ON;
			}
		break;
		
		case RIGHT: 
			if(cnt >= 5){
				motor_state = ON;
			}
			else{
				motor_state = RIGHT;
			}
		
		break;
		
		case LEFT:
			if(cnt >= 5){
				motor_state = ON;
			}
			else{
				motor_state = LEFT;
			}
		
		break;
		
		default:
			motor_state = OFF;
		break;
	}
	switch(motor_state){  //State Actions
		case OFF:
			PORTA = 0x00;
		break;
		
		case ON:
			PORTA = 0x03;
		break;
		
		case RIGHT:
			PORTA = 0x01;
		break;
		
		case LEFT:
			PORTA = 0x02;
		break;
	}
	
	
	
}

int main(void){
	DDRA = 0xFF; PORTA = 0x00;
	
	motor_state = OFF;
	
	TimerSet(50);
	TimerOn();
	SPI_SlaveInit();
	
    while (1){
		SPI_ISR();
		MotorControl();
		while(!TimerFlag);
	}
}

