#ifndef MeDefaultPrograms_H
#define MeDefaultPrograms_H

#include "Me2MotorControl.h"
#include "MeUltrasonic.h"
#include "MeIR.h"
#include "MeLineFollower.h"

typedef enum
{
    MODE_A,
    MODE_B,
    MODE_C
} ProgramMode;

void modeA();
void modeB();
void modeC();

void injectDefaultProgramDependencies(
        Me2Motor::MotorState motorState,
        MeUltrasonic ultrasonicSensor,
        MeIR irSensor,
        MeLineFollower lineFollower);
#endif //MeDefaultPrograms_H
