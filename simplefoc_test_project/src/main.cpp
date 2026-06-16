#include <Arduino.h>
#include "encoder.h"

// Example bit-bang pins for ESP32 (change to your wiring)
const int MA704_CLK_1 = 18;   // clock output
const int MA704_DATA_1 = 19;  // data input

const int MA704_CLK_2 = 5;   // clock output
const int MA704_DATA_2 = 23;  // data input

MA704Sensor sensor(MA704_CLK_1, MA704_DATA_1);
MA704Sensor sensor2(MA704_CLK_2, MA704_DATA_2);

//Motor parameters
BLDCMotor motor = BLDCMotor(7);
BLDCDriver3PWM driver = BLDCDriver3PWM(32, 33, 25, 12);

BLDCMotor motor1 = BLDCMotor(7);
BLDCDriver3PWM driver1 = BLDCDriver3PWM(26, 27, 14, 12);

//Command settings
float target_velocity = 0;
uint32_t prev_millis;

//Setting the alarm voltage
#define UNDERVOLTAGE_THRES 11.1

void board_check();
float get_vin_Volt();
void board_init();
bool flag_under_voltage = false;

Commander command = Commander(Serial);
void doMotor(char* cmd) { command.motor(&motor, cmd); }

// Current Sensing
// Sampling Resistor Value Gain ADC Pin
InlineCurrentSense current_sense0 = InlineCurrentSense(0.01f, 50.0f, 39, 36);
InlineCurrentSense current_sense1 = InlineCurrentSense(0.01f, 50.0f, 35, 34);

void setup() {

  Serial.begin(115200);
  board_init();

  //Sensor configuration
  sensor.init();
  sensor2.init();
  motor.linkSensor(&sensor);
  motor1.linkSensor(&sensor2);

  //Supply voltage setting [V]
  driver.voltage_power_supply = get_vin_Volt();
  driver.init();
  driver1.voltage_power_supply = get_vin_Volt();
  driver1.init();  
  
  //Connect the motor and driver objects
  motor.linkDriver(&driver);
  motor1.linkDriver(&driver1);
  current_sense0.linkDriver(&driver);
  current_sense1.linkDriver(&driver1);

  // Current Sensing
  current_sense0.init();
  current_sense1.init();

  //Link current sensing
  motor.linkCurrentSense(&current_sense0);
  motor1.linkCurrentSense(&current_sense1);

  // FOC config (still using voltage control)
  motor.voltage_sensor_align = 0.5;
  motor.torque_controller = TorqueControlType::voltage;
  motor.controller = MotionControlType::torque;
  
  //Maximum motor limiting voltage
  motor.voltage_limit = 0.5;    // [V] Please modify and check this value carefully, excessive voltage and current may cause the driver board to burn out!!!
  motor1.voltage_limit = 0.5;    // [V] Please modify and check this value carefully, excessive voltage and current may cause the driver board to burn out!!!
  
  //Speed ​​low pass filter time constant
  motor.LPF_velocity.Tf = 0.01;
  motor1.LPF_velocity.Tf = 0.01;
  
  //Initialize the motor
  motor.init();
  // motor1.init();

  //Initialize FOC
  motor.initFOC();
  // motor1.initFOC();

  motor.target = 0;

  
  char motor_id = 'M';
  command.add(motor_id, doMotor, "motor");
  motor.useMonitoring(Serial);

  // configuring the monitoring to be well parsed by the webcontroller
  motor.monitor_start_char = motor_id;
  motor.monitor_end_char = motor_id;

  command.verbose = VerboseMode::machine_readable;

  Serial.println(F("Motor ready."));
  Serial.println(F("Set the target velocity using serial terminal:"));
  
}

void loop() {
  motor.loopFOC();
  // motor1.loopFOC();

  motor.move();
  // motor1.move();

  motor.monitor();
  // motor1.monitor();

  //When the voltage is lower than the set value, the motor will be disabled.
  board_check();

  //User Communications
  if (!flag_under_voltage)
    command.run();
}


void board_init() {
  pinMode(32, INPUT_PULLUP);
  pinMode(33, INPUT_PULLUP);
  pinMode(25, INPUT_PULLUP);
  pinMode(26, INPUT_PULLUP);
  pinMode(27, INPUT_PULLUP);
  pinMode(14, INPUT_PULLUP);

  analogReadResolution(12);  //12bit

  float VIN_Volt = get_vin_Volt();
  while (VIN_Volt <= UNDERVOLTAGE_THRES) {
    VIN_Volt = get_vin_Volt();
    delay(500);
    Serial.printf("Waiting for power on, current voltage%.2f\n", VIN_Volt);
  }
  Serial.printf("Calibrating motor...Current voltage%.2f\n", VIN_Volt);
}

float get_vin_Volt() {
  return analogReadMilliVolts(13) * 8.5 / 1000;
}

void board_check() {

  uint32_t curr_millis = millis();
  static uint8_t enableState = 0;

  if (curr_millis - prev_millis >= 1000) {
    float vin_Volt = get_vin_Volt();

    if (vin_Volt < UNDERVOLTAGE_THRES) {
      flag_under_voltage = true;
      enableState = 0;
      uint8_t count = 5;
      while (count--) {
        vin_Volt = get_vin_Volt();
        if (vin_Volt > UNDERVOLTAGE_THRES) {
          flag_under_voltage = false;
          break;
        }
      }
    } else {
      flag_under_voltage = false;
    }
    if (flag_under_voltage) {
      motor.disable();
      motor1.disable();
    } else if (0 == enableState && flag_under_voltage == false) {
      enableState = 1;
      motor.enable();
      motor1.enable();
    }
    prev_millis = curr_millis;
  }
}