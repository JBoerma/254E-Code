#pragma config(Motor,  port1,           D_BL,          tmotorVex393_HBridge, openLoop, reversed)
#pragma config(Motor,  port2,           D_TL,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           L_BL,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           L_TL,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           L_R,           tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           R_L,           tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           R_TR,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           R_BR,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           D_TR,          tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          D_BR,          tmotorVex393_HBridge, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX2)
#pragma competitionControl(Competition)
#include "Vex_Competition_Includes.c"

void setDrive(int L, int R){
	motor[D_TL] = L;
	motor[D_BL] = L;
	motor[D_TR] = R;
	motor[D_BR] = R;
}

void drive(int L, int R, int threshold) {
	if (abs(L)<threshold)
		L = 0;
	if (abs(R)<threshold)
		R = 0;
	setDrive(L, R);
}

void setLift(int power){
	motor[L_TL] = power;
	motor[L_BL] = power;
	motor[L_R] = -power;

	motor[R_TR] = -power;
	motor[R_BR] = -power;
	motor[R_L] = power;
}

/*************** AUTON funcs ***************/

void autonDrive(int L, int R, int time){
	setDrive(L, R);
	wait1Msec(time);
}

void autonLift(int power, int time){
	setLift(power);
	wait1Msec(time);
}

/*************** AUTON tasks ***************/

int startOnRight = 1;    // -1 if on left

void adjustToSide(){
	if(startOnRight == -1) {
		autonDrive(100,127,500);
		autonDrive(127,100,500);
	} else {
		autonDrive(127,100,500);
		autonDrive(100,127,500);
	}
}

task driveAutonTask(){
	adjustToSide(); // 1000
	autonDrive(127,127,250);
	///////

	stopTask(driveAutonTask);
}

//pos up or down?
task liftAutonTask(){
	autonLift(0,750);
	autonLift(127,500);
	/////////

	stopTask(liftAutonTask);
}

/******************** AUTON ****************/

void pre_auton()
{
  bStopTasksBetweenModes = true;
}

void mimicAuton() {
	startTask(driveAutonTask);
	startTask(liftAutonTask);
}

task autonomous()
{
	mimicAuton();
}

/**************** USER ****************/

task usercontrol()
{
  while (true)
  {
  	if (vexRT[Btn7U])
  		mimicAuton();

  	setLift(127 * (-vexRT[Btn6U] + vexRT[Btn6D]));
  	drive(vexRT[Ch3], -vexRT[Ch2],15);
  }
}
