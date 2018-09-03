#include "MeDefaultPrograms.h"
#include "MeInfraredReceiver.h"
#include "MeUltrasonic.h"
#include "MeLineFollower.h"
#include "MePort.h"

Me2Motor::MotorState mmotor;
MeIR mirSensor;
MeUltrasonic multrasonicSensor(PORT_3);
MeLineFollower mlineFollower(PORT_2);

void injectDefaultProgramDependencies(Me2Motor::MotorState motorState, MeUltrasonic ultrasonicSensor, MeIR irSensor,
                                      MeLineFollower lineFollower) {
    mmotor = motorState;
    multrasonicSensor = ultrasonicSensor;
    mirSensor = irSensor;
    mlineFollower = lineFollower;
}

int LineFollowFlag=0;

void modeA()
{
    switch (mmotor.current)
    {
        case RUN_F:
            Me2Motor::Forward(mmotor);
            mmotor.prevState = mmotor.current;
            break;
        case RUN_B:
            Me2Motor::Backward(mmotor);
            mmotor.prevState = mmotor.current;
            break;
        case RUN_L:
            Me2Motor::TurnLeft(mmotor);
            mmotor.prevState = mmotor.current;
            break;
        case RUN_R:
            Me2Motor::TurnRight(mmotor);
            mmotor.prevState = mmotor.current;
            break;
        case STOP:
            if(mmotor.prevState!=mmotor.current){
                mmotor.prevState = mmotor.current;
                Me2Motor::Stop();
            }
            break;
    }

}

void modeB()
{
    uint8_t d = multrasonicSensor.distanceCm(50);
    randomSeed(analogRead(6));
    uint8_t randNumber = random(2);
    if (d > 15 || d == 0)Me2Motor::Forward(mmotor);
    else if (d > 10) {
        switch (randNumber)
        {
            case 0:
                Me2Motor::TurnLeft(mmotor);
                delay(200);
                break;
            case 1:
                Me2Motor::TurnRight(mmotor);
                delay(200);
                break;
        }
    }
    else
    {
        Me2Motor::Backward(mmotor);
        delay(400);
    }
    delay(100);
}

void modeC() {
    uint8_t lineFollowSensorState;
    lineFollowSensorState = mlineFollower.readSensors();
    if (mmotor.moveSpeed > 230)mmotor.moveSpeed = 230;
    switch (lineFollowSensorState) {
        case S1_IN_S2_IN:
            Me2Motor::Forward(mmotor);
            LineFollowFlag = 10;
            break;

        case S1_IN_S2_OUT:
            Me2Motor::Forward(mmotor);
            if (LineFollowFlag > 1) LineFollowFlag--;
            break;

        case S1_OUT_S2_IN:
            Me2Motor::Forward(mmotor);
            if (LineFollowFlag < 20) LineFollowFlag++;
            break;

        case S1_OUT_S2_OUT:
            if (LineFollowFlag == 10) Me2Motor::Backward(mmotor);
            if (LineFollowFlag < 10) Me2Motor::TurnLeft(mmotor);
            if (LineFollowFlag > 10) Me2Motor::TurnRight(mmotor);
            break;
    }
}
