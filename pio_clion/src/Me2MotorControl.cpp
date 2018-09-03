#include "Me2MotorControl.h"
#include "MeBuzzer.h"
#include "Me2Buzzer.h"
#include "MeDCMotor.h"
#include "MeRGBLed.h"

namespace Me2Motor {

    MeRGBLed rgb;
    MeBuzzer buzzer;
    MeDCMotor MotorL(M1);
    MeDCMotor MotorR(M2);

    void ChangeSpeed(MotorState motor, int spd) {
        buzzer.tone(NTD1 + motor.factor * spd, 200);
        motor.moveSpeed = motor.factor * spd + motor.minSpeed;
    }
    void Forward(MotorState state)
    {
        MotorL.run(-state.moveSpeed);
        MotorR.run( state.moveSpeed);
    }
    void Backward(MotorState state)
    {
        MotorL.run( state.moveSpeed);
        MotorR.run(-state.moveSpeed);
    }
    void TurnLeft(MotorState state)
    {
        MotorL.run(-state.moveSpeed/10);
        MotorR.run( state.moveSpeed);
    }
    void TurnRight(MotorState state)
    {
        MotorL.run(-state.moveSpeed);
        MotorR.run( state.moveSpeed/10);
    }
    void Stop()
    {
        rgb.clear();
        MotorL.run(0);
        MotorR.run(0);
    }
}