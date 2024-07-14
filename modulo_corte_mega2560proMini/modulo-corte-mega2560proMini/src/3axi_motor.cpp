/** @file 3axi_motor.cpp
 * 
 * @brief Ficheiro de controlo dos motores da ponte de 3 eixos. 
 *
 */ 

/****************************** Includes *************************************/

#include "3axi_motor.h"

/*************************** Global Variables ********************************/

motor_control_t motorCtrl[MOTOR_LAST];

uint32_t countPrintMotor = 0; // Debug variables

/****************************** Functions *************************************/

/*******************************************************************************
 * @brief print dos valores de controladores dos motores
 *******************************************************************************/
void printValuesMotor(MOTOR_ENUM motor)
{
  if (motor < MOTOR_LAST)
  {
    char buffer[50];
    sprintf (buffer,"MOTOR_ID: %d\tpos = %d\tdir = %d\tmotor_run = %d\n",(int)motor, (int)motorCtrl[motor].pos,motorCtrl[motor].dir,motorCtrl[motor].motorRunFlag);
    Serial.print(buffer);
  }
}

/*******************************************************************************
 * @brief Debug dos valores de controladores dos motores
 *******************************************************************************/
void debugMotor(MOTOR_ENUM motor, uint32_t period)
{
  if (motor < MOTOR_LAST)
  {
    while ((digitalRead(BUTTON_DIR_PIN) == 1) || (digitalRead(BUTTON_ESQ_PIN) == 1))
    {
      if (digitalRead(BUTTON_DIR_PIN) == 0)
      {
        motor_control(motor, MOTOR_PLUS_CMD, period);
      }
      else if (digitalRead(BUTTON_ESQ_PIN) == 0)
      {
        motor_control(motor, MOTOR_MINUS_CMD, period);
      }
      else
      {
        motor_control(motor, MOTOR_STOP_CMD, period);
      }

      if (++countPrintMotor >= 1000)
      {
        printValuesMotor(motor);
        countPrintMotor = 0;
      }
    }
    motor_control(motor, MOTOR_STOP_CMD, period);
    printValuesMotor(motor);
  } 
}

/*******************************************************************************
 * @brief Ativação do motor rotativo
 *******************************************************************************/
void enable_motor(MOTOR_ENUM motor)
{
  if (motor < MOTOR_LAST)
  {
    uint8_t motorEnableArr[] = MOTOR_EN_PIN;
    digitalWrite(motorEnableArr[motor], LOW);
  } 
  
}

/*******************************************************************************
 * @brief Desativação do motor rotativo
 *******************************************************************************/
void disable_motor(MOTOR_ENUM motor)
{
  if (motor < MOTOR_LAST)
  {
    uint8_t motorEnableArr[] = MOTOR_EN_PIN;
    digitalWrite(motorEnableArr[motor], HIGH);
  }  
}

void goto_pos(MOTOR_ENUM motor, uint32_t targetPos, uint32_t period)
{
  if (motor < MOTOR_LAST)
  {
    if (targetPos > motorCtrl[motor].pos)
    {
      while(motorCtrl[motor].pos < targetPos && motorCtrl[motor].pos < TAMPA_LIMIT)
      {
        motor_control(motor,MOTOR_PLUS_CMD,period);
      }
    }
    else
    {
      while(motorCtrl[motor].pos > targetPos && motorCtrl[motor].pos > 0)
      {
        motor_control(motor,MOTOR_MINUS_CMD,period);
      }
    }
    motorCtrl[motor].motorRunFlag = 0;
    //motor_control(motor,MOTOR_STOP_CMD,period);
  }
}

/******************************************************************************
 * @brief calibra o motor vertical
 ******************************************************************************/
void calibrateTampa(void)
{
  motorCtrl[MOTOR_TAMPA].calibFlag = 1;
  motorCtrl[MOTOR_TAMPA].pos = 0;
  
  /*
  while(digitalRead(CALIB_TAMPA_PIN) == 1){
    motor_control(MOTOR_TAMPA,MOTOR_MINUS_CMD,PERIOD_CALIB_TAMPA);
    delay(1);
  }
  motor_control(MOTOR_TAMPA,MOTOR_STOP_CMD,PERIOD_CALIB_TAMPA);
  //printf("Zero marcado!\n");
  motorCtrl[MOTOR_TAMPA].calibFlag = 1;
  motorCtrl[MOTOR_TAMPA].pos = 0;
  //while(motorCtrl[1].pos < 2000){
  //  motor_control(MOTOR_TAMPA,MOTOR_PLUS_CMD,PERIOD_CALIB_TAMPA);
  //}
  motor_control(MOTOR_TAMPA,MOTOR_STOP_CMD,PERIOD_CALIB_TAMPA);
  */
}

/******************************************************************************
 * @brief calibra o motor vertical
 ******************************************************************************/
void calibrateVertical(void)
{  
  
  while(digitalRead(CALIB_VER_PIN) == 1){
    motor_control(MOTOR_VERTICAL,MOTOR_MINUS_CMD,PERIOD_CALIB_VERTICAL);
    delay(1);
  }
  motor_control(MOTOR_VERTICAL,MOTOR_STOP_CMD,PERIOD_CALIB_VERTICAL);
  //printf("Zero marcado!\n");
  motorCtrl[MOTOR_VERTICAL].calibFlag = 1;
  motorCtrl[MOTOR_VERTICAL].pos = 0;
  goto_pos(MOTOR_VERTICAL, 2000, PERIOD_VERTICAL);
  motor_control(MOTOR_VERTICAL,MOTOR_STOP_CMD,PERIOD_CALIB_VERTICAL);
  
  
}

/******************************************************************************
 * @brief handler de controlo dos motores
 ******************************************************************************/
void motor_control(MOTOR_ENUM motor, MOTOR_CONTROL cmd, uint32_t period)
{
  if ((period % PERIOD_BASE) != 0)
  {
    return;
  }

  if (motor >= MOTOR_LAST)
    return;

  if (motorCtrl[motor].cmdAnterior == cmd)
    return;
  
  switch(cmd)
  {
    case MOTOR_STOP_CMD:
      motorCtrl[motor].motorRunFlag = 0;
      disable_motor(motor);   
      break;
    case MOTOR_PLUS_CMD:
      motorCtrl[motor].motorRunFlag = 1;
      motorCtrl[motor].dir = 1;
      enable_motor(motor);
      motorCtrl[motor].vel = period;
      break;
    case MOTOR_MINUS_CMD:
      motorCtrl[motor].motorRunFlag = 1;
      motorCtrl[motor].dir = 0;
      enable_motor(motor);
      motorCtrl[motor].vel = period;   
      break;
  }
  motorCtrl[motor].cmdAnterior = cmd;
}

/******************************************************************************
 * @brief Gerador dos pulsos de controlo e ativação do motor rotativo
 * o periodo é incrementado 20
 ******************************************************************************/
static void motor_handler()
{
  uint8_t motorDirArr[] = MOTOR_DIR_PIN;
  uint8_t motorPulseArr[] = MOTOR_PUL_PIN;
  
  for (uint8_t motorIdx = 0; motorIdx < MOTOR_LAST; motorIdx++)
  {
    if (motorCtrl[motorIdx].motorRunFlag == 1)
    {
      if (motorCtrl[motorIdx].counterHandler == motorCtrl[motorIdx].vel)
      {
        
        if (motorCtrl[motorIdx].dir == motorCtrl[motorIdx].orientation)
          digitalWrite(motorDirArr[motorIdx], LOW);
        else
          digitalWrite(motorDirArr[motorIdx], HIGH);
        
        
        if (motorCtrl[motorIdx].toggle == 1)
        {
          if (motorCtrl[motorIdx].calibFlag == 0)
          {
            motorCtrl[motorIdx].toggle = 0;
            digitalWrite(motorPulseArr[motorIdx], HIGH);
            if (motorCtrl[motorIdx].dir == 1)
            {
              motorCtrl[motorIdx].pos++;
            }
            else
            {
              motorCtrl[motorIdx].pos--;
            }
          }
          else
          {
            if (motorCtrl[motorIdx].dir == 1)
            {
              if (motorCtrl[motorIdx].pos < motorCtrl[motorIdx].limit || motorCtrl[motorIdx].limit == 0)
              {
                motorCtrl[motorIdx].pos++;
                motorCtrl[motorIdx].toggle = 0;
                digitalWrite(motorPulseArr[motorIdx],HIGH);
              }
            }
            else
            {
              if (motorCtrl[motorIdx].pos != 0)
              {
                motorCtrl[motorIdx].pos--;
                motorCtrl[motorIdx].toggle = 0;
                digitalWrite(motorPulseArr[motorIdx],HIGH);
              }
            }
          }
        }
        else
        {
          motorCtrl[motorIdx].toggle = 1;
          digitalWrite(motorPulseArr[motorIdx],LOW);
        }
        motorCtrl[motorIdx].counterHandler = 0;
      }

      motorCtrl[motorIdx].counterHandler += PERIOD_BASE;
    }
  }
}

/******************************************************************************
 * @brief Inicia os motores
 ******************************************************************************/
void motors_init(void){
  // pinout setup

  uint8_t motorEnableArr[] = MOTOR_EN_PIN;
  uint8_t motorDirArr[] = MOTOR_DIR_PIN;
  uint8_t motorPulseArr[] = MOTOR_PUL_PIN;
  
  for (uint8_t motorIdx = 0; motorIdx < MOTOR_LAST; motorIdx++)
  {
    pinMode(motorEnableArr[motorIdx],OUTPUT);
    pinMode(motorDirArr[motorIdx],OUTPUT);
    pinMode(motorPulseArr[motorIdx],OUTPUT);
  }

  pinMode(CALIB_VER_PIN,INPUT);  
  pinMode(SENSOR_LEV_BAIXO_PIN,INPUT);
  pinMode(SENSOR_LEV_ALTO_PIN,INPUT);
  pinMode(SENSOR_LEV_FRENTE_PIN,INPUT);
  pinMode(CALIB_TAMPA_PIN,INPUT_PULLUP);
  pinMode(BUTTON_DIR_PIN,INPUT_PULLUP);
  pinMode(BUTTON_ESQ_PIN,INPUT_PULLUP);
  pinMode(BUTTON_SIDE_PIN,INPUT_PULLUP);
  pinMode(BUTTON_MINI_PIN,INPUT_PULLUP);

  pinMode(FAN_EN_PIN,OUTPUT);
  digitalWrite(FAN_EN_PIN,HIGH);

  pinMode(CORTE_EN_PIN,OUTPUT);
  digitalWrite(CORTE_EN_PIN,HIGH);
  
  Timer1.initialize(PERIOD_BASE);
  Timer1.attachInterrupt(motor_handler);

  // Activatin default enables and setting defaults
  disable_motor(MOTOR_TAMPA);

  // motor controler setup
  motorCtrl[MOTOR_TAMPA].orientation = 0;
  motorCtrl[MOTOR_TAMPA].limit = TAMPA_LIMIT;
  motorCtrl[MOTOR_TAMPA].toggle = 0;
  motorCtrl[MOTOR_TAMPA].calibFlag = 0;

  motorCtrl[MOTOR_VERTICAL].orientation = 1;
  motorCtrl[MOTOR_VERTICAL].limit = VERTICAL_LIMIT;
  motorCtrl[MOTOR_VERTICAL].toggle = 0;
  motorCtrl[MOTOR_VERTICAL].calibFlag = 0;

  motorCtrl[MOTOR_ROTATIVO].orientation = 0;
  motorCtrl[MOTOR_ROTATIVO].limit = 0;
  motorCtrl[MOTOR_ROTATIVO].toggle = 0;
  motorCtrl[MOTOR_ROTATIVO].calibFlag = 0;

}