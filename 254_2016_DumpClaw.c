#pragma config(Sensor, in1,    lift_potentiometer, sensorPotentiometer)
#pragma config(Motor,  port1,           inner_left_lift, tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           right_claw,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           left_claw,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           left_lift,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           front_right_drive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           front_left_drive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           back_left_drive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           back_right_drive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           right_lift,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          inner_right_lift, tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// TODO: get the potentiometers working
// TODO: refine autonomous

#pragma platform(VEX2);
#pragma competitionControl(Competition);
#include "Vex_Competition_Includes.c";

const int MAX_MOTOR_POWER = 127;
const int DEADZONE = 15;
const int LIFT_THRESHOLD = 2;

int lowerTo127(int val)
{
	if (abs(val) > MAX_MOTOR_POWER)
	{
		if(val < 0) return -MAX_MOTOR_POWER;
		else        return MAX_MOTOR_POWER;
	}
	else return val;
}

int above_threshold(int value, int threshold){
	if (abs(value) > threshold){
		return value;
	}
	else
		return 0;
}

// *********************   DRIVE  ***********************///

int x;
int y;
int z;
void drive(int xDrive, int yDrive, int turn)
{
	x = above_threshold(xDrive, DEADZONE);
	y = above_threshold(yDrive, DEADZONE);
	z = above_threshold(turn, DEADZONE);

	motor[front_left_drive]  = lowerTo127(x+y-z);
	motor[front_right_drive] = lowerTo127(x-y-z);
	motor[back_left_drive]  = lowerTo127(x-y+z);
	motor[back_right_drive] = lowerTo127(x+y+z);
}

// **********************  HUG  *************************///

void hug(int in)
{
		motor[left_claw] = in;
		motor[right_claw] = -in;
}
// *********************  LIFT  **************************//
// replace with enum
const int MAX_LIFT_POSITION = 90; //?
const int MIN_LIFT_POSITION = 20; //?
const int DIFF_LIFT_POSITION = 70; //?

void lift(int power)
{
	motor[left_lift] = -power;
	motor[inner_left_lift] = power;
	motor[right_lift] = power;
	motor[inner_right_lift] = -power;
}

void move_to_lift_pos(int pos) {
	int current_pos = SensorValue[lift_potentiometer];
	int diff = above_threshold((pos - current_pos), LIFT_THRESHOLD);
	int ratio_diff = diff / DIFF_LIFT_POSITION;
	lift(ratio_diff * MAX_MOTOR_POWER);
}

//********************  AUTON FUNCTIONS   *******************************
// HUG
int autonHugTime;
int autonHugDir;
int autonClawSpeed;
task autonHugHandler()
{
	hug((autonHugDir*autonClawSpeed));
	wait1Msec(autonHugTime);
	hug(0);
	stopTask(autonHugHandler);
}

void asyncAutonHug(int dir, int clawSpeed, int time)
{
	autonHugDir = dir;
	autonHugTime = time;
	autonClawSpeed = clawSpeed;
	startTask(autonHugHandler);
}

// DRIVE
int autonXDrive;
int autonYDrive;
int autonTurn;
int autonDriveTime;
task autonDriveHandler()
{
	drive(autonXDrive, autonYDrive, autonTurn);
	wait1Msec(autonDriveTime);
	drive(0,0,0);
	stopTask(autonDriveHandler);
}

void asyncAutonDrive(int xDrive, int yDrive, int turn, int time)
{
	autonXDrive = xDrive;
	autonYDrive = yDrive;
	autonTurn = turn;
	autonDriveTime = time;
	startTask(autonDriveHandler);
}

// LIFT
int autonLiftPower;
int autonLiftTime;
task autonLiftHandler()
{
	lift(autonLiftPower);
	wait1Msec(autonLiftTime);
	lift(0);
	stopTask(autonLiftHandler);
}


void asyncAutonLift(int power, int time)
{
	autonLiftPower = power;
	autonLiftTime = time;
	startTask(autonLiftHandler);
}

// Actual Auton
void pre_auton()
{
  bStopTasksBetweenModes = true;
}

// USEFUL AUTON CONSTANTS
const int ROT_TIME_TURN_180 = 400;
const int ROT_TIME_90_DEGREES = 650/4;
const int ROT_IN = 1;
const int ROT_OUT = -1;

const int TIME_TO_RISE = 1000;  // adjust this
const int TIME_TO_LOWER = 400;

const int isRight = -1;  // -1 if intially should go to left

// 15 seconds long; time var always last
task autonomous()
{
	//asyncAutonHug(ROT_IN, CLAW_SPEED, ROT_TIME_90_DEGREES);  // 1 is inward rotation - 650 is for 90 rotation
	asyncAutonDrive(isRight*60,120,0,1200);
	wait1Msec(200);
}

task userDriveTask(){ while(true)	drive(vexRT[Ch4], vexRT[Ch3], vexRT[Ch1]);}  // X, Y, Turn (power)
task userHugTask()  { while(true) hug  (MAX_MOTOR_POWER * (vexRT[Btn6U] - vexRT[Btn6D]));}  // power
task userLiftTask() { while(true) lift (MAX_MOTOR_POWER*(vexRT[Btn5U] - vexRT[Btn5D]));} // power

task usercontrol()
{

  //startTask(userDriveTask);
	//startTask(userHugTask);
	//startTask(userLiftTask);
	while(true) {
			drive(vexRT[Ch4], vexRT[Ch3], vexRT[Ch1]);
			//constHugCheck();
			//hug  (CLAW_SPEED * (vexRT[Btn6U] - vexRT[Btn6D]));
			//lift (MAX_MOTOR_POWER*(vexRT[Btn5U] - vexRT[Btn5D]));
	}
}
