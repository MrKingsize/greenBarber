/** @file main.cpp
 * 
 * @brief Ficheiro principal de controlo dos motores e display no lcd. 
 *
 */ 
#include <Arduino.h>

#include <DueTimer.h>
#include <LiquidCrystal.h>
#include <DueFlashStorage.h>

#include "flash_driver.h"
#include "3axi_motor.h"
#include "menu.h"

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

//volatile uint8_t printControlFlag = 0;

/*******************************************************************
 * Prototypes functions
 ******************************************************************/

/******************************************************************************
 * @brief Imprime os estados das threads da ponte de 3 eixos
 ******************************************************************************/
static void printTheadsStates(axiThreadsStates_t axi){
  Serial.print("x y z ctrl: ");
  Serial.print(axi.handlerXState);
  Serial.print("\t");
  Serial.print(axi.handlerYState);
  Serial.print("\t");
  Serial.print(axi.handlerZState);
  Serial.print("\t");
  Serial.println(axi.controlerState);
}
/******************************************************************************
 * @brief Imprime os valores de controlo de sinal
 ******************************************************************************/
static void printControlValues(uint8_t motorNum){
  
  controlValues_t controlValues;
  parameters_t paramX;
  parameters_t paramY;
  parameters_t paramZ;
  //get_control_params(&paramX, &paramY, &paramZ);
  get_control_values(&controlValues);

  if(motorNum == 1){
    Serial.print("posx: ");
    Serial.print(get_posX());
    //Serial.print("\trx: ");
    //Serial.print(controlValues.rx);
    Serial.print("\tex: ");
    Serial.print(controlValues.ex);
    Serial.print("\tkx: ");
    Serial.print(paramX.kp*controlValues.ex);
    Serial.print("\tix: ");
    Serial.print(controlValues.ix,2);
    Serial.print("\tux: ");
    Serial.print(controlValues.ux);
    Serial.print("\tdx: ");
    Serial.println(controlValues.dx);
  }
  else if(motorNum == 2){
    Serial.print("posy: ");
    Serial.print(get_posY());
    Serial.print("\tey: ");
    Serial.print(controlValues.ey);
    Serial.print("\tky: ");
    Serial.print(paramY.kp*controlValues.ey);
    Serial.print("\tiy: ");
    Serial.print(controlValues.iy,2);
    Serial.print("\tuy: ");
    Serial.print(controlValues.uy);
    Serial.print("\tdy: ");
    Serial.println(controlValues.dy);
  }
  else if(motorNum == 3){
    Serial.print("posz: ");
    Serial.print(get_posZ());
    Serial.print("\tez: ");
    Serial.print(controlValues.ez);
    Serial.print("\tkz: ");
    Serial.print(paramZ.kp*controlValues.ez);
    Serial.print("\tiz: ");
    Serial.print(controlValues.iz,2);
    Serial.print("\tuz: ");
    Serial.print(controlValues.uz);
    Serial.print("\tdz: ");
    Serial.println(controlValues.dz);
  }
  
}



/******************************************************************************
 * @brief função de setup inicial do sistema
 ******************************************************************************/
void setup() {
  Serial.begin(9600); //lcd.setCursor(10,1); lcd.print ("Right ");

  motors_init();
  lcd_menu_init();
     
  load_control_params();
}



/******************************************************************************
 * @brief Lê a tecla premida no momento com distinção se a tecla foi premida 1 vez ou está a ser premida continuamente para as teclas esquerda e direita
 * 
 * @return tecla premida
 *    1 -> Select
 *    2 -> Left
 *    22 -> Left continuamente
 *    3 -> Down
 *    4 -> Up
 *    5 -> Right
 *    55 -> Right continuamente
 *    0 -> none
 ******************************************************************************/
int readKey(void){
  int x, x1, x2;
  x1 = 0;
  x2 = 0;
  int mediaX = 0;
  do{
    x = analogRead (0);
    x2 = x1;
    x1 = x;
    mediaX = (x + x1 + x2)/3;
  }while(abs(mediaX - x) > 100 || abs(mediaX - x1) > 100 || abs(mediaX - x2) > 100);
  x = mediaX;

  if (lastKeyToggle == 0){
    lastKeyToggle = 1;
    if (x < 60){lastKey = 5; return 5;} //Right    
    else if (x < 200){lastKey = 4; return 4;} //Up
    else if (x < 400){lastKey = 3; return 3;} //Down
    else if (x < 800){lastKey = 2; return 2;} //Left
    else if (x < 1000){lastKey = 1; return 1;} //Select
    else{lastKeyToggle = 0; return 0;}  //None
  }
  else{
    if (x < 60){ //Right
      readInternalCounter++;
      if (readInternalCounter % 1000 == 0){ 
        lastKey = 55;
        readKeyCounter++;
        return 55;
      }
    }   
    else if (x < 200){} //Up
    else if (x < 400){} //Down
    else if (x < 800){ //Left
      readInternalCounter++;
      if (readInternalCounter % 1000 == 0){
        lastKey = 22;
        readKeyCounter++;
        return 22;
        }
      }
    else if (x < 1000){} //Select
    else if (x >= 1000){
      lastKeyToggle = 0;
      readInternalCounter = 0;
      readKeyCounter = 0;
    }
    return 0;
  }
}





uint16_t printCount = 0;
/******************************************************************************
 * @brief loop principal função main
 ******************************************************************************/
void loop(){

  if(0){//teste
     
  }
  
  display_menu();  

  while(1){

    //debug
    printCount++;
    if (0){
      if (printCount == 30000){
        axiThreadsStates_t axiThreads = get_3axi_threads_state();
        reset_threads_flags();
        printTheadsStates(axiThreads);
        printCount = 0;
      }        
    }
    else if (getPrintFlag() == 1 && printCount == 3000){
      //printControlValues(0);      
      setPrintFlag(0);
      printCount = 0;
    }
    
    if (readKey()){
      do_menu(lastKey, readKeyCounter);
    }
    else{
      do_menu_keyless();
    }  

  }  
}