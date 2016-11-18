#pragma config(Sensor, in1,    lift_potentiometer, sensorPotentiometer)
#pragma config(Sensor, in2,    claw_potentiometer, sensorPotentiometer)
#pragma config(Motor,  port1,           inner_left_lift, tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           right_claw,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           left_claw,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port4,           right_lift,    tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,           back_left_drive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port6,           front_right_drive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           front_left_drive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           back_right_drive, tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           left_lift,     tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          inner_right_lift, tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// TODO: get the potentiometers working
// TODO: refine autonomous

#pragma platform(VEX2);
#pragma competitionControl(Competition);
#include "Vex_Competition_Includes.c";

const int MAX_MOTOR_POWER = 127;
const int DRIVE_THRESHOLD = 15;
const int POTENTIOMETER_THRESHOLD = 2;

const int EFFECTIVE_DIVISOR_DIFF = 300;

int target_lift_pos = 0;
int past_diff = 0;
int lift_velocity = 0;
const int MAX_LIFT_POSITION = 2400; //?
const int MIN_LIFT_POSITION = 190; //?

int target_hug_pos = 0;
const int CLAW_CLOSED = 0; //?
const int CLAW_HORIZ = 0; //?


int lowerTo127(int val)
{
	if (abs(val) > MAX_MOTOR_POWER)
		if (val < 0) return -MAX_MOTOR_POWER;
		else         return MAX_MOTOR_POWER;
	return val;
}

int above_threshold(int value, int threshold){
	if (abs(value) > threshold)
		return value;
	return 0;
}

// *********************   DRIVE  ***********************///

int x;
int y;
int z;
void drive(int xDrive, int yDrive, int turn)
{
	x = above_threshold(xDrive, DRIVE_THRESHOLD);
	y = above_threshold(yDrive, DRIVE_THRESHOLD);
	z = above_threshold(turn, DRIVE_THRESHOLD);

	motor[front_left_drive]  = lowerTo127(x+y-z);
	motor[front_right_drive] = lowerTo127(x-y-z);
	motor[back_left_drive]  = -lowerTo127(x-y+z);
	motor[back_right_drive] = -lowerTo127(x+y+z);
}

task userDriveTask(){
	while(true)	{
		drive(vexRT[Ch4], vexRT[Ch3], vexRT[Ch1]);
		wait1Msec(50); // don't hog cpu!
	}
}  // X, Y, Turn (power)

// **********************  HUG  *************************///

void hug(int in)
{
	motor[left_claw] = in;
	motor[right_claw] = -in;
}

task move_to_hug_pos_task() {   // if higher potentiometer values correspond with 'positive' rotation
	while(true) {
		if(vexRT[Btn6U])
			target_hug_pos += 20;
		else if(vexRT[Btn6D])
			target_hug_pos -= 20;
		else if(vexRT[Btn8U])
			target_hug_pos = CLAW_CLOSED;
		else if(vexRT[Btn8D])
			target_hug_pos = CLAW_HORIZ;
		if (target_hug_pos > CLAW_HORIZ)
			target_hug_pos = CLAW_HORIZ;
		if (target_hug_pos < CLAW_CLOSED)
			target_hug_pos = CLAW_CLOSED;

		int current_pos = SensorValue[lift_potentiometer];
		int diff = above_threshold((target_lift_pos - current_pos), POTENTIOMETER_THRESHOLD);

		int ratio_diff = diff / EFFECTIVE_DIVISOR_DIFF;

		int set_speed = lowerTo127(ratio_diff * MAX_MOTOR_POWER);
		hug(set_speed);
		wait1Msec(50);
	}
}

// *********************  LIFT  **************************//

void lift(int power)
{
	motor[left_lift] = -power;
	motor[inner_left_lift] = power;
	motor[right_lift] = power;
	motor[inner_right_lift] = -power;
}

task move_to_lift_pos_task() {   // if higher potentiometer values correspond with 'positive' rotation
	while(true) {
		if(vexRT[Btn5U])
			target_lift_pos += 20;
		else if(vexRT[Btn5D])
			target_lift_pos -= 20;
		else if(vexRT[Btn7U])
			target_lift_pos = MAX_LIFT_POSITION;
		else if(vexRT[Btn7D])
			target_lift_pos = MIN_LIFT_POSITION;
		if (target_lift_pos > MAX_LIFT_POSITION)
			target_lift_pos = MAX_LIFT_POSITION;
		if (target_lift_pos < MIN_LIFT_POSITION)
			target_lift_pos = MIN_LIFT_POSITION;

		int current_pos = SensorValue[lift_potentiometer];
		int diff = above_threshold((target_lift_pos - current_pos), POTENTIOMETER_THRESHOLD);

		lift_velocity = diff - past_diff;
		past_diff = diff;

		int ratio_diff = diff / EFFECTIVE_DIVISOR_DIFF;

		int set_speed = lowerTo127(ratio_diff * MAX_MOTOR_POWER);
		lift(set_speed);

		wait1Msec(50);
	}
}

//********************  AUTON FUNCTIONS   *******************************
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
void set_lift_pos(int target) {
	target_lift_pos = target;
}

void set_hug_pos(int target) {
	target_hug_pos = target;
}


// **********************  AUTON  *************************///
void pre_auton()
{
  bStopTasksBetweenModes = true;
}

const int isRight = -1;  // -1 if intially should go to left

void startTasks() {
	startTask(move_to_hug_pos_task);
	startTask(move_to_lift_pos_task);
	startTask(userDriveTask);
}

void stopTasks() {
	stopTask(move_to_hug_pos_task);
	stopTask(move_to_lift_pos_task);
	stopTask(userDriveTask);
}

void set_init_values() {
	target_lift_pos = SensorValue[lift_potentiometer];
	target_hug_pos = SensorValue[claw_potentiometer];
}

void w(int time) {
	wait1Msec(time);
}

void mimicAuton() {
	set_init_values();
	stopTasks();
	startTask(move_to_lift_pos_task);
	startTask(move_to_hug_pos_task);

	asyncAutonDrive(isRight*60,120,0,1200);
	set_lift_pos(MAX_LIFT_POSITION);
	w(800);
	set_hug_pos(CLAW_HORIZ);
	w(400);
	asyncAutonDrive(0, 127, 0, 500);
	w(500);


	stopTask(move_to_hug_pos_task);
	stopTask(move_to_hug_pos_task);
}

// 15 seconds long; time var always last
task autonomous()
{
	mimicAuton();
}

task usercontrol()
{
	/*  */
	set_init_values();
  startTasks();
	while(true) {
		lift( (vexRT[Btn5U] - vexRT[Btn5D]) * MAX_MOTOR_POWER);
		hug ( (vexRT[Btn6U] - vexRT[Btn6D]) * MAX_MOTOR_POWER);

		if(vexRT[Btn7U]) {
			mimicAuton();
		}
			//drive(vexRT[Ch4], vexRT[Ch3], vexRT[Ch1]);
			//constHugCheck();
			//hug  (CLAW_SPEED * (vexRT[Btn6U] - vexRT[Btn6D]));
			//lift (MAX_MOTOR_POWER*(vexRT[Btn5U] - vexRT[Btn5D]));
	}
	/* */
}
