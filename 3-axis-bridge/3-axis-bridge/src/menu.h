/** @file 3axi_motor.h
 * 
 * @brief Header de menu de navegação no lcd. 
 *
 */ 

#ifndef MENU_ENABLED
#define MENU_ENABLED

#include <Arduino.h>
#include <LiquidCrystal.h>

#include "3axi_motor.h"
#include "flash_driver.h"

/*******************************************************************
 * Constants
 ******************************************************************/

#define FALSE 0
#define TRUE 1
#define NUM_MAIN_MENU_ITEMS 5
#define NUM_CALIBRATE_MENU_ITEMS 5
#define NUM_POSITION_MANUAL_MENU_ITEMS 5
#define NUM_POSITION_AUTO_MENU_ITEMS 5
#define NUM_CONFIG_MENU_ITEMS 4
#define NUM_CONFIG_MENU_X_ITEMS 5
#define NUM_CONFIG_MENU_Y_ITEMS 5
#define NUM_CONFIG_MENU_Z_ITEMS 5
#define NUM_CHECK_STATS_ITEMS 6

/*******************************************************************
 * Structures
 ******************************************************************/
struct menuItem_t{
  int8_t posy;
  String itemNameCursor;
  String itemName;
  uint8_t sizeName;
};

struct menu_t{
  menuItem_t mainMenu[NUM_MAIN_MENU_ITEMS];
  menuItem_t calibrateMenu[NUM_CALIBRATE_MENU_ITEMS];
  menuItem_t positionManualMenu[NUM_POSITION_MANUAL_MENU_ITEMS];
  menuItem_t positionAutoMenu[NUM_POSITION_AUTO_MENU_ITEMS];
  menuItem_t configMenu[NUM_CONFIG_MENU_ITEMS];
  menuItem_t configXmenu[NUM_CONFIG_MENU_X_ITEMS];
  menuItem_t configYmenu[NUM_CONFIG_MENU_Y_ITEMS];
  menuItem_t configZmenu[NUM_CONFIG_MENU_Z_ITEMS];
  menuItem_t checkStatsMenu[NUM_CHECK_STATS_ITEMS];
  uint8_t activeMenu; //1 -> main menu
  uint8_t posicaoCursor;
};

/*******************************************************************
 * Prototypes functions
 ******************************************************************/

/******************************************************************************
 * @brief executa o menu
 ******************************************************************************/
void do_menu(uint8_t lastKey, uint16_t readKeyCounter);

/******************************************************************************
 * @brief Inicia o LCD
 ******************************************************************************/
void lcd_menu_init(void);

/******************************************************************************
 * @brief Mostra os items do menu no lcd
 ******************************************************************************/
void display_menu(void);

/******************************************************************************
 * @brief executa a parte do menu sem teclas
 ******************************************************************************/
void do_menu_keyless(void);

#endif