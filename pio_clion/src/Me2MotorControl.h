#ifndef ME2MOTOR_H
#define ME2MOTOR_H

#include <stdint.h>

#define RUN_F 0x01
#define RUN_B 0x01<<1
#define RUN_L 0x01<<2
#define RUN_R 0x01<<3
#define STOP 0

namespace Me2Motor {

    struct MotorState {
        uint8_t prevState = STOP;
        uint8_t current = STOP;
        int moveSpeed = 200;
        int minSpeed = 48;
        int factor=23;
    } ;

    void Forward(MotorState motor);

    void Backward(MotorState motor);

    void TurnLeft(MotorState motor);

    void TurnRight(MotorState motor);

    void Stop();

    void ChangeSpeed(MotorState motor, int spd);


}
#endif //ME2MOTOR_H
