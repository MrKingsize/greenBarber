/** @file main.cpp
 * 
 * @brief Ficheiro principal de controlo dos motores e display no lcd. 
 *
 */ 
#include <Arduino.h>

#include <TimerOne.h>

#include "3axi_motor.h"

/*******************************************************************
 * Local Constants
 ******************************************************************/


/*******************************************************************
 * Local Variables
 ******************************************************************/ 

uint8_t lastKey = 0;
uint8_t lastKeyToggle = 0;
uint16_t readKeyCounter = 0;
uint16_t readInternalCounter = 0;
//extern menu_t menu;

uint32_t count = 0;
uint32_t countPrint = 0;

//volatile uint8_t printControlFlag = 0;

/*******************************************************************
 * Prototypes functions
 ******************************************************************/


/******************************************************************************
 * @brief função de setup inicial do sistema
 ******************************************************************************/
void setup() {
  Serial.begin(9600); //lcd.setCursor(10,1); lcd.print ("Right ");
  delay(100);
  motors_init();
}

static void printData(int state)
{
  static uint32_t countPrintD = 0;
  if (countPrintD++ >= 1000)
  {
    uint8_t levBaixo = digitalRead(SENSOR_LEV_BAIXO_PIN);
    uint8_t levAlto = digitalRead(SENSOR_LEV_ALTO_PIN);
    uint8_t levFrente = digitalRead(SENSOR_LEV_FRENTE_PIN);
    uint8_t calibV = digitalRead(CALIB_VER_PIN);
    uint8_t calibTampa = digitalRead(CALIB_TAMPA_PIN);

    char buffer[50];
    sprintf (buffer, "levBaixo = %d\tlevAlto = %d\tlevFrente = %d\n", levBaixo, levAlto, levFrente);
    Serial.print(buffer);
    sprintf (buffer, "calibV = %d\tcalivTampa = %d\tstate = %d\n", calibV, calibTampa, state);
    Serial.print(buffer);
    printValuesMotor(MOTOR_VERTICAL);
    countPrintD = 0;
  }
}

void pinout_debug()
{
  //pinMode(CALIB_VER_PIN,INPUT);
  //pinMode(SENSOR_LEV_ALTO_PIN,INPUT);
  //pinMode(SENSOR_LEV_BAIXO_PIN,INPUT);
  //pinMode(SENSOR_LEV_FRENTE_PIN,INPUT);

  while(1)
  {
    
    uint8_t levBaixo = digitalRead(SENSOR_LEV_BAIXO_PIN);
    uint8_t levAlto = digitalRead(SENSOR_LEV_ALTO_PIN);
    uint8_t levFrente = digitalRead(SENSOR_LEV_FRENTE_PIN);
    uint8_t calibV = digitalRead(CALIB_VER_PIN);
    uint8_t calibTampa = digitalRead(CALIB_TAMPA_PIN);



    if (countPrint++ >= 1000){
      char buffer[50];
      sprintf (buffer, "levBaixo = %d\tlevAlto = %d\tlevFrente = %d\n", levBaixo, levAlto, levFrente);
      Serial.print(buffer);
      sprintf (buffer, "calibV = %d\tcalivTampa = %d\n", calibV,calibTampa);
      Serial.print(buffer);
      countPrint = 0;
    }
    
    delay(1);
  };
}

void dc_debug()
{
  
    


  if (digitalRead(BUTTON_DIR_PIN) == 0)
  {
    digitalWrite(FAN_EN_PIN,LOW);
  }
  else
  {
    digitalWrite(FAN_EN_PIN,HIGH);
  }

  if(digitalRead(BUTTON_ESQ_PIN) == 0)
  {
    digitalWrite(CORTE_EN_PIN,LOW);
  }
  else
  {
    digitalWrite(CORTE_EN_PIN,HIGH);
  }

   
}

/******************************************************************************
 * @brief loop principal função main
 ******************************************************************************/
void loop()
{
  //pinout_debug();

  Serial.print("Init!\n");
  // calibração
  // iniciar calibração é carregar nos dois em simultâneo
  while((digitalRead(BUTTON_DIR_PIN) == 1) || (digitalRead(BUTTON_ESQ_PIN) == 1));
  Serial.print("Em calibração Vertical\n");
  calibrateVertical();
  Serial.print("calibrado!\n");

  // calibração
  while(digitalRead(BUTTON_MINI_PIN) == 1);
  Serial.print("Em calibração da Tampa\n");
  calibrateTampa();
  Serial.print("calibrado!\n");

  //printValuesTampa();
  int toggleOut = digitalRead(BUTTON_MINI_PIN);
  int fanOnFlag = 0;
  int state = 0;

  while(1){

   
    if (digitalRead(BUTTON_SIDE_PIN) == 1)
    {
      digitalWrite(CORTE_EN_PIN,HIGH);
    }
    else
    {
      digitalWrite(CORTE_EN_PIN,LOW);
    }


    uint8_t in_left = digitalRead(BUTTON_ESQ_PIN);
    uint8_t in_right = digitalRead(BUTTON_DIR_PIN);

    if(in_right == 0)
    {
      motor_control(MOTOR_ROTATIVO, MOTOR_PLUS_CMD, PERIOD_ROTATIVO_F);
    }
    else if (in_left == 0)
    {
      motor_control(MOTOR_ROTATIVO, MOTOR_MINUS_CMD, PERIOD_ROTATIVO_T);
    }
    else{
      motor_control(MOTOR_ROTATIVO, MOTOR_STOP_CMD, PERIOD_ROTATIVO_T);
    }

    uint8_t levAlto = digitalRead(SENSOR_LEV_ALTO_PIN);
    uint8_t levBaixo = digitalRead(SENSOR_LEV_BAIXO_PIN);
    uint8_t levFrente = digitalRead(SENSOR_LEV_FRENTE_PIN);

    if (levFrente == 0)
    {
      motor_control(MOTOR_VERTICAL, MOTOR_MINUS_CMD, PERIOD_VERTICAL);
      state = 1;
    }
    else if (levAlto == 0)
    {
      motor_control(MOTOR_VERTICAL, MOTOR_PLUS_CMD, PERIOD_VERTICAL);
      state = 2;
    }
    else if (levBaixo == 0)
    {
      motor_control(MOTOR_VERTICAL, MOTOR_MINUS_CMD, PERIOD_VERTICAL);
      state = 3;
    }
    else
    {
      motor_control(MOTOR_VERTICAL, MOTOR_STOP_CMD, PERIOD_VERTICAL);
      state = 4;
    }

    
    // FAN e tampa control
    if (toggleOut != digitalRead(BUTTON_MINI_PIN))
      count++;
    
    if(count > 1)
    {
      if (fanOnFlag)
      {
        digitalWrite(FAN_EN_PIN,HIGH);
        digitalWrite(CORTE_EN_PIN,HIGH);
        delay(500);
        goto_pos(MOTOR_TAMPA, 0, PERIOD_TAMPA);
        count = 0;
        fanOnFlag = 0;
      }
      else
      {
        goto_pos(MOTOR_TAMPA, TAMPA_LIMIT, PERIOD_TAMPA);
        delay(500);
        digitalWrite(FAN_EN_PIN,LOW);
        digitalWrite(CORTE_EN_PIN,LOW);
        fanOnFlag = 1;
        count = 0;
      }
    }
    toggleOut = digitalRead(BUTTON_MINI_PIN);

    printData(state);
    delay(1);    
  } 
}