/*
 * Title: FollowDigitalVelocityWithVariableTorque
 *
 * Objective:
 *    This example demonstrates control of the ClearPath-MC operational mode
 *    Follow Digital Velocity Command, Bipolar PWM Command with Variable Torque.
 *
 * Description:
 *    This example enables a ClearPath motor and executes a repeating pattern of
 *    bidirectional velocity moves and torque limits. During operation, various
 *    move statuses are written to the USB serial port.
 *    The resolution for PWM outputs is 8-bit, meaning only 256 discrete speeds
 *    and torque limits can be commanded. The motor's actual commanded speed
 *    and torque limit may differ slightly from what you input below because
 *    of this.
 *    Consider using Manual Velocity Control mode if greater velocity command
 *    resolution is required, or if HLFB is needed for "move done/at speed"
 *    status feedback.
 *
 * Requirements:
 * 1. A ClearPath motor must be connected to Connector M-0.
 * 2. The connected ClearPath motor must be configured through the MSP software
 *    for Follow Digital Velocity Command, Bipolar PWM Command with Variable
 *    Torque mode (In MSP select Mode>>Velocity>>Follow Digital Velocity
 *    Command, then with "Bipolar PWM Command w/ Variable Torque")
 * 3. The ClearPath must have a defined Max Speed configured through the MSP
 *    software (On the main MSP window fill in the "Max Speed (RPM)" box with
 *    your desired maximum speed). Ensure the value of maxSpeed below matches
 *    this Max Speed.
 * 4. Set the PWM Deadband in MSP to 2.
 * 5. In MSP, ensure the two checkboxes for "Invert Torque PWM Input" and
 *    "Invert Speed PWM Input" are unchecked.
 * 6. A primary Torque Limit and Alternate Torque Limit must be defined using
 *    the Torque Limit setup window through the MSP software (To configure,
 *    click the "Setup..." button found under the "Torque Limit" label. Then
 *    fill in the textbox labeled "Alt Torque Limit (% of max)" and hit the
 *    Apply button). Use only symmetric limits. These limits must match the
 *    "torqueLimit" and "torqueLimitAlternate" variables defined below.
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 * ** ClearPath Manual (DC Power): https://www.teknic.com/files/downloads/clearpath_user_manual.pdf
 * ** ClearPath Manual (AC Power): https://www.teknic.com/files/downloads/ac_clearpath-mc-sd_manual.pdf
 *
 * 
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include "ClearCore.h"

// Defines the motor's connector as ConnectorM1
#define motor ConnectorM3

// Defines the motor's connector as ConnectorM1
#define testOut ConnectorIO0

// Select the baud rate to match the target device.
#define baudRate 9600

// Specify which serial to use: ConnectorUsb, ConnectorCOM0, or ConnectorCOM1.
#define SerialPort ConnectorUsb

// This is the commanded speed limit (must match the MSP value). This speed
// cannot actually be commanded, so use something slightly higher than your real
// max speed here and in MSP.
double maxVelocity = 100;

// A PWM deadband of 2% prevents signal jitter from effecting a 0 RPM command
// (must match MSP value)
double pwmDeadBand = 0;

// Declares our user-defined helper functions, which are used to command
// velocity and limit torque. The definitions/implementations of these functions
// are at the bottom of the sketch.
bool CommandDuty(double commandedDuty);

int main() {
    // Sets all motor connectors to the correct mode for Follow Digital
    // Velocity, Bipolar PWM mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_M2M3,
                          Connector::SCREWDRIVER);
    motor.ScrewdriverMode(MotorDriver::SCREW_MODE_IMON);

    ConnectorIO0.Mode(Connector::INPUT_DIGITAL);

    // Sets up serial communication and waits up to 5 seconds for a port to open.
    // Serial communication is not required for this example to run.
    SerialPort.Mode(Connector::USB_CDC);
    SerialPort.Speed(baudRate);
    uint32_t timeout = 2000;
    uint32_t startTime = Milliseconds();
    SerialPort.PortOpen();
    while (!SerialPort && Milliseconds() - startTime < timeout) {
        continue;
    }

    // Waits for 5 seconds to allow the motor to come online
    SerialPort.SendLine("Waiting for screwdriver to come online...");
    startTime = Milliseconds();
    while (Milliseconds() - startTime < timeout) {
        continue;
    }
    SerialPort.SendLine("Screwdriver Ready");
     
    motor.SetScrewRampTime(200);
    motor.ScrewCalStart(.5);

    while (true) {
        
    }
}
//------------------------------------------------------------------------------