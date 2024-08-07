/** @file flash_driver.cpp
 * 
 * @brief Ficheiro de gestão da memória flash do arduino. 
 *
 */ 

#include "flash_driver.h"
#include <DueFlashStorage.h>

DueFlashStorage dueFlashStorage;

parameters_t parameters;
uint8_t bootFlag;

void write_parameters(parameters_t parameters, uint8_t id){
  byte parameters2flash[sizeof(parameters_t)]; // create byte array to store the struct
  memcpy(parameters2flash, &parameters, sizeof(parameters_t)); // copy the struct to the byte array
  dueFlashStorage.write(id, parameters2flash, sizeof(parameters_t)); // write byte array to flash
}

parameters_t read_parameters(uint8_t id){
  byte* parametersFromFlash = dueFlashStorage.readAddress(id); // byte array which is read from flash at adress 4
  parameters_t parametersFromFlashOut; // create a temporary struct
  memcpy(&parametersFromFlashOut, parametersFromFlash, sizeof(parameters_t)); // copy byte array to temporary struct

  return parametersFromFlashOut;
}

uint8_t read_first_boot(void){
  return dueFlashStorage.read(FIRST_BOOT_ID);
}

void set_first_boot(void){
  dueFlashStorage.write(FIRST_BOOT_ID,5);
}