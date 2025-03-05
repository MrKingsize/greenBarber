/** @file main.cpp
 * 
 * @brief Main file. 
 * After compiling run the correct target 
 * pio run -e megaatmega2560
 * pio run -e leonardo
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


/*******************************************************************
 * Prototypes functions
 ******************************************************************/



// put function declarations here:
int myFunction(int, int);


/******************************************************************************
 * @brief initial setup system's function
 ******************************************************************************/
void setup() {
  // put your setup code here, to run once:
  int result = myFunction(2, 3);
}


/******************************************************************************
 * @brief main loop function
 ******************************************************************************/
void loop() {
  
  /*
  get ID

  if mcuId == dir right
  {
    get command 
  }

  else if mcuId == dir left


  */
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}