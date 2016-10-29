#pragma config(Sensor, in1,    cLeft,          sensorPotentiometer)
#pragma config(Sensor, in2,    cRight,         sensorPotentiometer)
#pragma config(Motor,  port1,           lBRight,       tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           dTRight,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,           rClaw,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,           dTLeft,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port5,           lTRight,       tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,           dBLeft,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           lTLeft,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           dBRight,       tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port9,           lClaw,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port10,          lBLeft,        tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

// d at beginning means drive
// l at beginning beans lift
// r & l at beginning are for claw

// TODO: get the potentiometers working
// TODO: refine autonomous

#pragma platform(VEX2);
#pragma competitionControl(Competition);
#include "Vex_Competition_Includes.c";

const int MAX_MOTOR_POWER = 127;
const int DEADZONE = 10;
const int CLAW_SPEED = 70;
const float COEF_REDUCE_LEFT_ARM = 0.90;
const float COEF_REDUCE_RIGHT_ARM = 0.90;

int lowerTo127(int val)
{
	if (abs(val) > MAX_MOTOR_POWER)
	{
		if(val < 0) return -MAX_MOTOR_POWER;
		else        return MAX_MOTOR_POWER;
	}
	else return val;
}

// *********************   DRIVE  ***********************///
int x;
int y;
int z;
void drive(int xDrive, int yDrive, int turn)
{
	x = abs(xDrive) > DEADZONE ? -xDrive : 0;
	y = abs(yDrive) > DEADZONE ? -yDrive : 0;
	z = abs(turn)   > DEADZONE ? turn    : 0;

	motor[dTLeft]  = lowerTo127(x+y-z);
	motor[dTRight] = lowerTo127(x-y-z);
	motor[dBLeft]  = lowerTo127(x-y+z);
	motor[dBRight] = lowerTo127(x+y+z);
}

// **********************  HUG  *************************///
bool constHugIn = false;
void constHugCheck(){
	if(vexRT[Btn8D])
		constHugIn = true;
	if(vexRT[Btn8L] || vexRT[Btn6U] || vexRT[Btn6D])
		constHugIn = false;
}

void hug(int in)
{
	if(constHugIn)
	{
		motor[lClaw] = 60;
		motor[rClaw] = 60;
		return;
	}
	else
	{
		motor[lClaw] = COEF_REDUCE_LEFT_ARM*in;
		motor[rClaw] = COEF_REDUCE_RIGHT_ARM*in;
	}
}

void sensorhug(){
    while(true){
        if(Btn6U==true){
            motor[rClaw]=127;
            motor[lClaw]=127;
        }
        while(motor[rClaw]==127){
            int val1 = SensorValue[cLeft];
            wait1Msec(500);
            int val2 = SensorValue[cLeft];
            if(abs(val1-val2)<5){
                motor[rClaw]=30;
                motor[lClaw]=30;
            }
        }

    }
}
// *********************  LIFT  **************************//

void lift(int power)
{
	motor[lTLeft]  =  power;
	motor[lBLeft]  =  power;
	motor[lTRight] = -power;
	motor[lBRight] = -power;
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
const int ROT_TIME_TURN_180 = 800;
const int ROT_TIME_90_DEGREES = 650;
const int ROT_IN = 1;
const int ROT_OUT = -1;

const int TIME_TO_RISE = 950;  // adjust this
const int TIME_TO_LOWER = 400;

const int isRight = 1;  // -1 if intially should go to left

// 15 seconds long; time var always last
task autonomous()
{
	asyncAutonHug(ROT_IN, CLAW_SPEED, ROT_TIME_90_DEGREES);  // 1 is inward rotation - 650 is for 90 rotation
	asyncAutonDrive(isRight*60,120,0,1200);
	wait1Msec(200);
	// t = .2
	asyncAutonLift(-MAX_MOTOR_POWER, TIME_TO_RISE);
	wait1Msec(800);
	// t = 1 - canLift
	asyncAutonHug(ROT_IN, CLAW_SPEED, 200);
	wait1Msec(200);
	// t = 1.2 - canDrive, canLift, canHug
	asyncAutonDrive(0,120,0,400);
	wait1Msec(400);
	// t = 1.6 - canDrive, canLift, canHug
	asyncAutonHug(ROT_OUT, CLAW_SPEED, 200);
	asyncAutonDrive(0,-120,0,200);
	wait1Msec(200);
	// t = 1.8 - canDrive, canLift, canHug
	asyncAutonDrive(-isRight*120, 0, 0, 1200);
	wait1Msec(1200);
	// t = 3 - canDrive, canLift, canHug
	asyncAutonDrive(0, 120, 0, 200);
	asyncAutonHug(ROT_IN, CLAW_SPEED, 200);
	asyncAutonLift(-MAX_MOTOR_POWER, 200);
	wait1Msec(200);
	// t = 3.2 - canDrive, canLift, canHug
	asyncAutonHug(ROT_OUT, CLAW_SPEED, 200 + ROT_TIME_90_DEGREES); // 850
	asyncAutonDrive(0,0,120, ROT_TIME_TURN_180); // 800
	wait1Msec(300);
	// t = 3.5
	asyncAutonLift(MAX_MOTOR_POWER, TIME_TO_LOWER); // 400
	wait1Msec(550);
	// t = 4.05
	asyncAutonHug(ROT_IN, CLAW_SPEED, ROT_TIME_90_DEGREES); // 850
	wait1Msec(850);
	// t = 4.9
	asyncAutonHug(ROT_IN, 40, 1000);
	asyncAutonLift(-MAX_MOTOR_POWER, TIME_TO_RISE);
	wait1Msec(200);
	// t = 5.1
	asyncAutonDrive(0,0,120, ROT_TIME_TURN_180);
	wait1Msec(800);
	// t = 5.9
	asyncAutonDrive(0,120,0,400);
	wait1Msec(400);
	// t = 6.3
	asyncAutonHug(ROT_OUT, CLAW_SPEED, ROT_TIME_90_DEGREES);
	wait1Msec(400);





}

task userDriveTask(){ while(true)	drive(vexRT[Ch4], vexRT[Ch3], vexRT[Ch1]);}  // X, Y, Turn (power)
task userHugTask()  { while(true) hug  (CLAW_SPEED * (vexRT[Btn6U] - vexRT[Btn6D]));}  // power
task userLiftTask() { while(true) lift (MAX_MOTOR_POWER*(vexRT[Btn5U] - vexRT[Btn5D]));} // power

task usercontrol()
{
	stopTask(autonHugHandler);
	stopTask(autonDriveHandler);
	stopTask(autonHugHandler);

  startTask(userDriveTask);
	//startTask(userHugTask);
	//startTask(userLiftTask);
	while(true) {
			//drive(vexRT[Ch4], vexRT[Ch3], vexRT[Ch1]);
			constHugCheck();
			hug  (CLAW_SPEED * (vexRT[Btn6U] - vexRT[Btn6D]));
			lift (MAX_MOTOR_POWER*(vexRT[Btn5U] - vexRT[Btn5D]));
	}
}
