#define motor1ForwardDir HIGH
#define motor1BackwardDir LOW
#define motor2ForwardDir LOW
#define motor2BackwardDir HIGH
const int maxSpeed = 100;

#define ENCODERS_I2C_ADDRESS 0x29
#define ENCODERS_I2C_MSG_SIZE 2
char encodersMsg[ENCODERS_I2C_MSG_SIZE];
long leftMotorPosition  = 0;
long rightMotorPosition = 0;
union u1 {
  long leftMotorPosition;
  unsigned char read_byte[4];
} 
leftMotorPositionUnion;
union u2 {
  long rightMotorPosition;
  unsigned char read_byte[4];
} 
rightMotorPositionUnion;


//Define Variables we'll be connecting to
double leftMotorPIDSetpoint, leftMotorPIDInput, leftMotorPIDOutput;
double rightMotorPIDSetpoint, rightMotorPIDInput, rightMotorPIDOutput;

//Specify the links and initial tuning parameters
PID leftMotorPID(&leftMotorPIDInput, &leftMotorPIDOutput, &leftMotorPIDSetpoint, 0,0,0, DIRECT);
PID rightMotorPID(&rightMotorPIDInput, &rightMotorPIDOutput, &rightMotorPIDSetpoint, 0,0,0, DIRECT);

void updateMotorStatuses() {
  Wire.beginTransmission(ENCODERS_I2C_ADDRESS);
  Wire.requestFrom(ENCODERS_I2C_ADDRESS,ENCODERS_I2C_MSG_SIZE);
  Wire.endTransmission();
  for (int i=0; i<=ENCODERS_I2C_MSG_SIZE; i++) {
    encodersMsg[i] = Wire.read();
  }

  leftMotorSpeed = (float)encodersMsg[0]*-1;
  rightMotorSpeed = (float)encodersMsg[1];

  motorSpeed = (leftMotorSpeed+rightMotorSpeed)/20;
  //speedPIDInput = smooth(speedFIR.process(motorSpeed));
  speedPIDInput = speedMovingAvarageFilter.process(speedFIR.process(motorSpeed));

  /*leftMotorPIDInput = leftMotorSpeed;
  rightMotorPIDInput = rightMotorSpeed;*/
}
void setMotorPIDSetpoint(int motor, double motorSpeed) {
  if (motor == 2)
    leftMotorPIDSetpoint = motorSpeed;
  else if (motor == 1)
    rightMotorPIDSetpoint = motorSpeed;

}
/*void initMotorPIDs() {
 Serial.println("initMotorPIDs");
 leftMotorPID.SetOutputLimits(-100, 100);
 rightMotorPID.SetOutputLimits(-100, 100);
 leftMotorPID.SetMode(AUTOMATIC);
 rightMotorPID.SetMode(AUTOMATIC);
 leftMotorPID.SetSampleTime(configuration.motorsPIDSampling);
 rightMotorPID.SetSampleTime(configuration.motorsPIDSampling);
 
 leftMotorPID.SetTunings((float)configuration.motorPIDKp / 100, (float)configuration.motorPIDKi / 100, (float)configuration.motorPIDKd / 100);
 rightMotorPID.SetTunings((float)configuration.motorPIDKp / 100, configuration.motorPIDKi / 100, (float)configuration.motorPIDKd / 100);
 }*/
void stopMotors() {
  setMotorPIDSetpoint(1, 0);
  setMotorPIDSetpoint(2, 0);
  moveMotor(1, 0); // höger
  moveMotor(2, 0); // vänster
  leftMotorPID.SetMode(MANUAL);
  rightMotorPID.SetMode(MANUAL);
}
void updateMotorPIDs() {
  leftMotorPID.Compute();
  rightMotorPID.Compute();
  moveMotor(1, rightMotorPIDOutput); // höger
  moveMotor(2, leftMotorPIDOutput); // vänster
  //Serial.println("updateMotorPIDs");
}
void moveMotor(int motor, double speed) { // speed is a value in percentage 0-100%
  // motor1 is right
  // motor2 is left

  if(speed > maxSpeed)
    speed = maxSpeed;
  else if (speed < -maxSpeed)
    speed = -maxSpeed;

  int pwmToMotor = 0;
  if (motor == 1)
    pwmToMotor = (abs(speed)*255/100)*motor1Calibration;
  else
    pwmToMotor = (abs(speed)*255/100)*motor2Calibration;
  if (pwmToMotor > 255)
    pwmToMotor = 255;

  if (motor == 1) {
    if (speed == 0) {
      digitalWrite(dir_a, motor1ForwardDir);
      analogWrite(pwm_a, 0);
    }
    else if (speed < 0) {
      digitalWrite(dir_a, motor1ForwardDir);
      analogWrite(pwm_a, pwmToMotor);
    }
    else {
      digitalWrite(dir_a, motor1BackwardDir);
      analogWrite(pwm_a, pwmToMotor);             
    }
  }
  else {
    if (speed == 0) {
      digitalWrite(dir_b, motor2ForwardDir);
      analogWrite(pwm_b, 0);
    }
    else if (speed < 0) {
      digitalWrite(dir_b, motor2ForwardDir);
      analogWrite(pwm_b, pwmToMotor);
    }
    else {
      digitalWrite(dir_b, motor2BackwardDir);
      analogWrite(pwm_b, pwmToMotor);
    }
  }
}













