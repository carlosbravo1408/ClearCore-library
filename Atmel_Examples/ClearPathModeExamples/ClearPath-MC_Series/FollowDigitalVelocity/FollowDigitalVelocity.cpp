/*
 * Title: FollowDigitalVelocity
 *
 * Objective:
 *    This example demonstrates control of the ClearPath-MC operational mode
 *    Follow Digital Velocity Command, Unipolar PWM Command.
 *
 * Description:
 *    This example enables a ClearPath motor and executes velocity moves based
 *    on the state of an analog input sensor. During operation, various move
 *    statuses are written to the USB serial port.
 *    This example does not use HLFB for motor feedback. It is possible your
 *    commanded velocity is not reached before a new velocity is commanded.
 *    The resolution for PWM outputs is 8-bit, meaning 256 discrete speeds
 *    can be commanded in each direction. The motor's actual commanded speed may
 *    differ from what is printed to the serial port because of this.
 *    Consider using Manual Velocity Control mode if greater velocity command
 *    resolution is required, or if HLFB is needed for "move done" or "at speed"
 *    status feedback.
 *
 * Requirements:
 * 1. A ClearPath motor must be connected to Connector M-0.
 * 2. The connected ClearPath motor must be configured through the MSP software
 *    for Follow Digital Velocity Command, Unipolar PWM Command mode (In MSP
 *    select Mode>>Velocity>>Follow Digital Velocity Command, then with
 *    "Unipolar PWM Command" selected hit the OK button).
 * 3. The ClearPath must have a defined Max Speed configured through the MSP
 *    software (On the main MSP window fill in the "Max Speed (RPM)" box with
 *    your desired maximum speed). Ensure the value of maxSpeed below matches
 *    this Max Speed.
 * 4. Ensure the "Invert PWM Input" checkbox found on the MSP's main window is
 *    unchecked.
 * 5. Ensure the Input A filter in MSP is set to 20ms, (In MSP select
 *    Advanced>>Input A, B Filtering... then in the Settings box fill in
 *    the textbox labeled "Input A Filter Time Constant (msec)" then hit the OK
 *    button).
 * 6. An analog sensor connected to one of the analog inputs (A-9 through A-12)
 *    to control motor velocity. Define the appropriate connector below.
 *
 *
 * Links:
 * ** ClearCore Documentation: https://teknic-inc.github.io/ClearCore-library/
 * ** ClearCore Manual: https://www.teknic.com/files/downloads/clearcore_user_manual.pdf
 * ** ClearPath Manual (DC Power): https://www.teknic.com/files/downloads/clearpath_user_manual.pdf
 * ** ClearPath Manual (AC Power): https://www.teknic.com/files/downloads/ac_clearpath-mc-sd_manual.pdf
 * ** ClearPath Mode Informational Video: https://www.teknic.com/watch-video/#OpMode8
 *
 * 
 * Copyright (c) 2020 Teknic Inc. This work is free to use, copy and distribute under the terms of
 * the standard MIT permissive software license which can be found at https://opensource.org/licenses/MIT
 */

#include "ClearCore.h"

// Defines the motor's connector as ConnectorM0
#define motor ConnectorM0

// Defines the analog input to control commanded position
#define AnalogSensor ConnectorA9

// The INPUT_A_FILTER must match the Input A filter setting in MSP
// (Advanced >> Input A, B Filtering...)
#define INPUT_A_FILTER 20

// Select the baud rate to match the target device.
#define baudRate 9600

// Specify which serial to use: ConnectorUsb, ConnectorCOM0, or ConnectorCOM1.
#define SerialPort ConnectorUsb

// This is the commanded speed limit (must match the MSP value). This speed
// cannot actually be commanded, so use something slightly higher than your real
// max speed here and in MSP
double maxSpeed = 510;

// Declares our user-defined helper function, which is used to command speed 
// The definition/implementation of this function is at the bottom of
// this example.
bool CommandSpeed(int32_t commandedSpeed);

int main() {
    // Set up an analog sensor to control commanded position.
    AnalogSensor.Mode(Connector::INPUT_ANALOG);

    // Set the clock rate to output the highest resolution PWM.
    MotorMgr.MotorInputClocking(MotorManager::CLOCK_RATE_HIGH);

    // Sets all motor connectors to the correct mode for Follow Digital
    // Velocity, Unipolar PWM mode.
    MotorMgr.MotorModeSet(MotorManager::MOTOR_ALL,
                          Connector::CPM_MODE_A_DIRECT_B_PWM);                

    // Sets up serial communication and waits up to 5 seconds for a port to open.
    // Serial communication is not required for this example to run.
    SerialPort.Mode(Connector::USB_CDC);
    SerialPort.Speed(baudRate);
    uint32_t timeout = 5000;
    uint32_t startTime = Milliseconds();
    SerialPort.PortOpen();
    while (!SerialPort && Milliseconds() - startTime < timeout) {
        continue;
    }

    // Enables the motor
    motor.EnableRequest(true);
    SerialPort.SendLine("Motor Enabled");

    // Change ClearPath's Input A state to change direction.
    motor.MotorInAState(false);
    //motor.MotorInAState(true);
    
    // Delays to send the correct filtered direction.
    Delay_ms(2 + INPUT_A_FILTER);
    
    
    while (true) {
        // Read the voltage on the analog sensor (0-10V).
        float analogVoltage = AnalogSensor.AnalogVoltage();
        // Convert the voltage measured to a position within the valid range.
        int32_t commandedSpeed =
            static_cast<int32_t>(round(analogVoltage / 10 * maxSpeed));

        // Move at the commanded velocity.
        CommandSpeed(commandedSpeed);    // See below for the detailed function definition.
    }
}

/*------------------------------------------------------------------------------
 * CommandSpeed
 *
 *    Command the motor to move using a speed of commandedSpeed
 *    Prints the move status to the USB serial port
 *
 * Parameters:
 *    int commandedSpeed  - The speed to command
 *
 * Returns: True/False depending on whether the velocity was successfully
 * commanded.
 */
bool CommandSpeed(int32_t commandedSpeed) {
  
    if (abs(commandedSpeed) >= abs(maxSpeed)) {
        SerialPort.SendLine("Move rejected, requested velocity at or over the limit.");
        return false;
    }

//  Uncomment the lines below if you would like to pass this function signed velocity values directly
//    // Change ClearPath's Input A state to change direction.
//    if (commandedSpeed > 0) {
//        motor.MotorInAState(false);
//    }
//    else {
//        motor.MotorInAState(true);
//    }
//    // Delays to send the correct filtered direction.
//    Delay_ms(2 + INPUT_A_FILTER);

    // Scale the velocity command to our duty cycle range.
    uint8_t dutyRequest = abs(commandedSpeed) / maxSpeed * 255;

    // Print the new speed command
    SerialPort.Send("Commanding speed: ");
    SerialPort.SendLine(commandedSpeed);

    // Command the move.
    motor.MotorInBDuty(dutyRequest);

    return true;
}
//------------------------------------------------------------------------------
