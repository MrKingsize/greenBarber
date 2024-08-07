/** @file menu.cpp
 * 
 * @brief Ficheiro de navegação do menu no lcd 
 *
 */ 

#include "menu.h"

/*******************************************************************
 * Local Variables
 ******************************************************************/ 
//LCD pin to Arduino
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

menu_t menu;
uint8_t lcdUpdate = 0; //sempre que houver alterações esta flag tem que ser colocada a 1

uint8_t endX = 0;
uint8_t endY = 0;
uint8_t endZ = 0;
uint8_t endXanterior = 0;
uint8_t endYanterior = 0;
uint8_t endZanterior = 0;
uint16_t valuePostionAutoX = 0;
uint16_t valuePostionAutoY = 0;
uint16_t valuePostionAutoZ = 0;
uint16_t valuePostionAutoXanterior = 0;
uint16_t valuePostionAutoYanterior = 0;
uint16_t valuePostionAutoZanterior = 0;

/******************************************************************************
 * @brief Construção da estrutura do menu 
 * Menu principal - activeMenu = 1
 *    > Calibrar - activeMenu = 2
 *        > Voltar
 *        > Calib xyz
 *        > Calib x
 *        > Calib y
 *        > Calib z
 *    > Posição manual - activeMenu = 3
 *        > Voltar
 *        > Pos x:
 *        > Pos x:
 *        > Pos x:
 *        > Executar
 *    > Posição auto - activeMenu = 4
 *        > Voltar
 *        > x:
 *        > x:
 *        > x:
 *        > Executar
 *    > Config - activeMenu = 5
 *        > Config x - activeMenu = 6
 *            > Voltar
 *            > Kx:
 *            > Tix:
 *            > tdx:
 *            > Set 
 *        > Config y - activeMenu = 7
 *            > Voltar
 *            > Ky:
 *            > Tiy:
 *            > tdy:
 *            > Set 
 *        > Config z - activeMenu = 8
 *            > Voltar
 *            > Kz:
 *            > Tiz:
 *            > tdz:
 *            > Set 
 *    > Check stats - activeMenu = 9
 *        > Voltar
 *        > Status end x, y, y
 *        > Bateria
 *        > Posicionamento
 *        > Luminosidade
 *        > Contador
 ******************************************************************************/
static void menuInit(void){
  menu.activeMenu = 1;
  menu.posicaoCursor = 0; 
  
  menu.mainMenu[0].posy = 0;
  menu.mainMenu[0].itemName = " Calibrar ";
  menu.mainMenu[0].sizeName = 10;
  menu.mainMenu[1].posy = 1;
  menu.mainMenu[1].itemName = " Posicao manual ";
  menu.mainMenu[1].sizeName = 16;
  menu.mainMenu[2].posy = 2;
  menu.mainMenu[2].itemName = " Posicao auto ";
  menu.mainMenu[2].sizeName = 14;
  menu.mainMenu[3].posy = 3;
  menu.mainMenu[3].itemName = " Config ";
  menu.mainMenu[3].sizeName = 8; 
  menu.mainMenu[4].posy = 4;
  menu.mainMenu[4].itemName = " Check stats ";
  menu.mainMenu[4].sizeName = 13; 

  menu.calibrateMenu[0].posy = 0;
  menu.calibrateMenu[0].itemName = " Voltar ";
  menu.calibrateMenu[0].sizeName = 8;
  menu.calibrateMenu[1].posy = 1;
  menu.calibrateMenu[1].itemName = " Calib xyz ";
  menu.calibrateMenu[1].sizeName = 11;
  menu.calibrateMenu[2].posy = 2;
  menu.calibrateMenu[2].itemName = " Calib x ";
  menu.calibrateMenu[2].sizeName = 9;
  menu.calibrateMenu[3].posy = 3;
  menu.calibrateMenu[3].itemName = " Calib y ";
  menu.calibrateMenu[3].sizeName = 9;
  menu.calibrateMenu[4].posy = 4;
  menu.calibrateMenu[4].itemName = " Calib z ";
  menu.calibrateMenu[4].sizeName = 9;

  menu.positionManualMenu[0].posy = 0;
  menu.positionManualMenu[0].itemName = " Voltar ";
  menu.positionManualMenu[0].sizeName = 8;
  menu.positionManualMenu[1].posy = 1;
  menu.positionManualMenu[1].itemName = " Pos x: ";
  menu.positionManualMenu[1].sizeName = 8;
  menu.positionManualMenu[2].posy = 2;
  menu.positionManualMenu[2].itemName = " Pos y: ";
  menu.positionManualMenu[2].sizeName = 8;
  menu.positionManualMenu[3].posy = 3;
  menu.positionManualMenu[3].itemName = " Pos z: ";
  menu.positionManualMenu[3].sizeName = 8;
  menu.positionManualMenu[4].posy = 4;
  menu.positionManualMenu[4].itemName = " Executar ";
  menu.positionManualMenu[4].sizeName = 10;

  menu.positionAutoMenu[0].posy = 0;
  menu.positionAutoMenu[0].itemName = " Voltar ";
  menu.positionAutoMenu[0].sizeName = 8;
  menu.positionAutoMenu[1].posy = 1;
  menu.positionAutoMenu[1].itemName = " x: ";
  menu.positionAutoMenu[1].sizeName = 4;
  menu.positionAutoMenu[2].posy = 2;
  menu.positionAutoMenu[2].itemName = " y: ";
  menu.positionAutoMenu[2].sizeName = 4;
  menu.positionAutoMenu[3].posy = 3;
  menu.positionAutoMenu[3].itemName = " z: ";
  menu.positionAutoMenu[3].sizeName = 4;
  menu.positionAutoMenu[4].posy = 4;
  menu.positionAutoMenu[4].itemName = " Executar ";
  menu.positionAutoMenu[4].sizeName = 10;

  menu.configMenu[0].posy = 0;
  menu.configMenu[0].itemName = " Voltar ";
  menu.configMenu[0].sizeName = 8;
  menu.configMenu[1].posy = 1;
  menu.configMenu[1].itemName = " Config x ";
  menu.configMenu[1].sizeName = 10;
  menu.configMenu[2].posy = 2;
  menu.configMenu[2].itemName = " Config y ";
  menu.configMenu[2].sizeName = 10;
  menu.configMenu[3].posy = 3;
  menu.configMenu[3].itemName = " Config z ";
  menu.configMenu[3].sizeName = 10;

  menu.configXmenu[0].posy = 0;
  menu.configXmenu[0].itemName = " Voltar ";
  menu.configXmenu[0].sizeName = 8;
  menu.configXmenu[1].posy = 1;
  menu.configXmenu[1].itemName = " Kx: ";
  menu.configXmenu[1].sizeName = 5;
  menu.configXmenu[2].posy = 2;
  menu.configXmenu[2].itemName = " Tix: ";
  menu.configXmenu[2].sizeName = 6;
  menu.configXmenu[3].posy = 3;
  menu.configXmenu[3].itemName = " Tdx: ";
  menu.configXmenu[3].sizeName = 6;
  menu.configXmenu[4].posy = 4;
  menu.configXmenu[4].itemName = " Set ";
  menu.configXmenu[4].sizeName = 5;

  menu.configYmenu[0].posy = 0;
  menu.configYmenu[0].itemName = " Voltar ";
  menu.configYmenu[0].sizeName = 8;
  menu.configYmenu[1].posy = 1;
  menu.configYmenu[1].itemName = " Ky: ";
  menu.configYmenu[1].sizeName = 5;
  menu.configYmenu[2].posy = 2;
  menu.configYmenu[2].itemName = " Tiy: ";
  menu.configYmenu[2].sizeName = 6;
  menu.configYmenu[3].posy = 3;
  menu.configYmenu[3].itemName = " Tdy: ";
  menu.configYmenu[3].sizeName = 6;
  menu.configYmenu[4].posy = 4;
  menu.configYmenu[4].itemName = " Set ";
  menu.configYmenu[4].sizeName = 5;

  menu.configZmenu[0].posy = 0;
  menu.configZmenu[0].itemName = " Voltar ";
  menu.configZmenu[0].sizeName = 8;
  menu.configZmenu[1].posy = 1;
  menu.configZmenu[1].itemName = " Kz: ";
  menu.configZmenu[1].sizeName = 5;
  menu.configZmenu[2].posy = 2;
  menu.configZmenu[2].itemName = " Tiz: ";
  menu.configZmenu[2].sizeName = 6;
  menu.configZmenu[3].posy = 3;
  menu.configZmenu[3].itemName = " Tdz: ";
  menu.configZmenu[3].sizeName = 6;
  menu.configZmenu[4].posy = 4;
  menu.configZmenu[4].itemName = " Set ";
  menu.configZmenu[4].sizeName = 5;

  menu.checkStatsMenu[0].posy = 0;
  menu.checkStatsMenu[0].itemName = " Voltar ";
  menu.checkStatsMenu[0].sizeName = 8;
  menu.checkStatsMenu[1].posy = 1;
  menu.checkStatsMenu[1].itemName = " End ";
  menu.checkStatsMenu[1].sizeName = 5;
  menu.checkStatsMenu[2].posy = 2;
  menu.checkStatsMenu[2].itemName = " Bat: ";
  menu.checkStatsMenu[2].sizeName = 6;
  menu.checkStatsMenu[3].posy = 3;
  menu.checkStatsMenu[3].itemName = " Posicionamento ";
  menu.checkStatsMenu[3].sizeName = 16;
  menu.checkStatsMenu[4].posy = 4;
  menu.checkStatsMenu[4].itemName = " Luminosidade ";
  menu.checkStatsMenu[4].sizeName = 14;
  menu.checkStatsMenu[5].posy = 5;
  menu.checkStatsMenu[5].itemName = " Contador ";
  menu.checkStatsMenu[5].sizeName = 10;
}

/******************************************************************************
 * @brief Retorna o menu
 ******************************************************************************/
/*void get_menu(menu_t *menuS){
    menuS = menu;
}*/

/******************************************************************************
 * @brief Inicia o LCD
 ******************************************************************************/
void lcd_menu_init(void){
    lcd.begin(16, 2);
    lcd.clear();
    
    menuInit();
}

/******************************************************************************
 * @brief coloca um parênteses [] o item onde está o cursor no menu
 ******************************************************************************/
static void showSelect(menuItem_t menuItems, uint8_t select){
  lcd.setCursor(0,  menuItems.posy);
  if (select) lcd.print("[");
  else  lcd.print(" ");
  lcd.setCursor(menuItems.sizeName - 1,  menuItems.posy);
  if (select) lcd.print("]");
  else  lcd.print(" ");
}

/******************************************************************************
 * @brief Mostra os items do menu no lcd
 ******************************************************************************/
void display_menu(void){
  lcd.clear();
  uint8_t menuSize;
  menuItem_t display_menu[NUM_CHECK_STATS_ITEMS]; //porque é a maior
  parameters_t paramX;
  parameters_t paramY;
  parameters_t paramZ;
  get_control_params(&paramX, &paramY, &paramZ);


  if (menu.activeMenu == 1){
    menuSize = NUM_MAIN_MENU_ITEMS;
    for (uint8_t i = 0; i < NUM_MAIN_MENU_ITEMS; i++){
      display_menu[i] = menu.mainMenu[i];
    }
  }
  else if (menu.activeMenu == 2){  
    menuSize = NUM_CALIBRATE_MENU_ITEMS;
    for (uint8_t i = 0; i < NUM_CALIBRATE_MENU_ITEMS; i++){
      display_menu[i] = menu.calibrateMenu[i];
    }
  }
  else if (menu.activeMenu == 3){
    menuSize = NUM_POSITION_MANUAL_MENU_ITEMS;
    for (uint8_t i = 0; i < NUM_POSITION_MANUAL_MENU_ITEMS; i++){
      display_menu[i] = menu.positionManualMenu[i];
    }
  }
  else if (menu.activeMenu == 4){
    menuSize = NUM_POSITION_AUTO_MENU_ITEMS;
    for (uint8_t i = 0; i < NUM_POSITION_AUTO_MENU_ITEMS; i++){
      display_menu[i] = menu.positionAutoMenu[i];
      if (i == 1){
        display_menu[i].itemName = menu.positionAutoMenu[i].itemName + String(valuePostionAutoX) + "/" + String(AXIS_X_RANGE);
      }
      else if (i == 2){
        display_menu[i].itemName = menu.positionAutoMenu[i].itemName + String(valuePostionAutoY) + "/" + String(AXIS_Y_RANGE);
      }
      else if (i == 3){
        display_menu[i].itemName = menu.positionAutoMenu[i].itemName + String(valuePostionAutoZ) + "/" + String(AXIS_Z_RANGE);
      }
    }
  }
  else if(menu.activeMenu == 5){
    menuSize = NUM_CONFIG_MENU_ITEMS;
    for(uint8_t i = 0; i < NUM_CONFIG_MENU_ITEMS; i++){
      display_menu[i] = menu.configMenu[i];      
    }
  }
  else if(menu.activeMenu == 6){
    menuSize = NUM_CONFIG_MENU_X_ITEMS;
    for(uint8_t i = 0; i < NUM_CONFIG_MENU_X_ITEMS; i++){
      display_menu[i] = menu.configXmenu[i];
      if (i == 1){
        display_menu[i].itemName = menu.configXmenu[i].itemName + String(paramX.kp);
      }
      else if (i == 2){
        display_menu[i].itemName = menu.configXmenu[i].itemName + String(paramX.ti,5);
      }
      else if (i == 3){
        display_menu[i].itemName = menu.configXmenu[i].itemName + String(paramX.td,3);
      }
    }
  }
  else if(menu.activeMenu == 7){
    menuSize = NUM_CONFIG_MENU_Y_ITEMS;
    for(uint8_t i = 0; i < NUM_CONFIG_MENU_Y_ITEMS; i++){
      display_menu[i] = menu.configYmenu[i];
      if (i == 1){
        display_menu[i].itemName = menu.configYmenu[i].itemName + String(paramY.kp);
      }
      else if (i == 2){
        display_menu[i].itemName = menu.configYmenu[i].itemName + String(paramY.ti,5);
      }
      else if (i == 3){
        display_menu[i].itemName = menu.configYmenu[i].itemName + String(paramY.td,3);
      }
    }
  }
  else if(menu.activeMenu == 8){
    menuSize = NUM_CONFIG_MENU_Z_ITEMS;
    for(uint8_t i = 0; i < NUM_CONFIG_MENU_Z_ITEMS; i++){
      display_menu[i] = menu.configZmenu[i];
      if (i == 1){
        display_menu[i].itemName = menu.configZmenu[i].itemName + String(paramZ.kp);
      }
      else if (i == 2){
        display_menu[i].itemName = menu.configZmenu[i].itemName + String(paramZ.ti,5);
      }
      else if (i == 3){
        display_menu[i].itemName = menu.configZmenu[i].itemName + String(paramZ.td,3);
      }
    }
  }
  else if(menu.activeMenu == 9){
    menuSize = NUM_CHECK_STATS_ITEMS;
    for(uint8_t i = 0; i < NUM_CHECK_STATS_ITEMS; i++){
      display_menu[i] = menu.checkStatsMenu[i];
      if (i == 1){
        display_menu[i].itemName = menu.checkStatsMenu[i].itemName + "x:" + String(endX) + " y:" + String(endY) + " z:" + String(endZ);
      }
      else if (i == 2){
        display_menu[i].itemName = menu.checkStatsMenu[i].itemName + "24V 100%";
      }
    }
  }
  else{
    menuSize = 0;
  }
    
  for (uint8_t i = 0; i < menuSize; i++){
    if (display_menu[i].posy == 0 || display_menu[i].posy == 1){
      lcd.setCursor(0,  display_menu[i].posy);
      lcd.print(display_menu[i].itemName);        
      if (display_menu[i].posy == menu.posicaoCursor){
        showSelect(display_menu[i], 1);
      }
      else{
        showSelect(display_menu[i], 0);
      }
    }    
  }
}

/******************************************************************************
 * @brief processa a navegação do menu e o que acontece em cada item
 ******************************************************************************/
static void select_current_item(uint8_t lastKey, uint16_t readKeyCounter){
  parameters_t paramX;
  parameters_t paramY;
  parameters_t paramZ;
  get_control_params(&paramX, &paramY, &paramZ);
  if (menu.activeMenu == 1){ //menu principal
    for (uint8_t i = 0; i < NUM_MAIN_MENU_ITEMS; i++){
      if (menu.mainMenu[i].posy == menu.posicaoCursor){ //encontrou o item
        if (i == 0){
          menu.activeMenu = 2; //avançar para o menu de calibração  
        }
        else if (i == 1){
          menu.activeMenu = 3; //avançar para o menu de posicionamento manual
        }
        else if (i == 2){
          menu.activeMenu = 4; //avançar para o menu de posicionamento automatico
          valuePostionAutoX = get_posX();
          valuePostionAutoY = get_posY();
          valuePostionAutoZ = get_posZ();
          valuePostionAutoXanterior = valuePostionAutoX;
          valuePostionAutoYanterior = valuePostionAutoY;
          valuePostionAutoZanterior = valuePostionAutoZ;
        }
        else if (i == 3){
          menu.activeMenu = 5;  //avançar para o menu config
        }
        else if (i == 4){
          menu.activeMenu = 9; //avançar para o menu stats
        }
      }
    }
  }
  else if(menu.activeMenu == 2){    // menu de calibração
    for (uint8_t i = 0; i < NUM_CALIBRATE_MENU_ITEMS; i++){
      if (menu.calibrateMenu[i].posy == menu.posicaoCursor){ //encontrou o item
        if (i == 0){
          menu.activeMenu = 1; //voltar para o menu principal  
        }
        else if (i == 1){
          calibrate(MOTOR_X_MASK | MOTOR_Y_MASK | MOTOR_Z_MASK);
        }
        else if (i == 2){
          calibrate(MOTOR_X_MASK);
        }
        else if (i == 3){
          calibrate(MOTOR_Y_MASK);
        }
        else if (i == 4){
          calibrate(MOTOR_Z_MASK);
        }
      }
    }    
  }
  else if(menu.activeMenu == 3){    // menu de posicionamento
    for (uint8_t i = 0; i < NUM_POSITION_MANUAL_MENU_ITEMS; i++){
      if (menu.positionManualMenu[i].posy == menu.posicaoCursor){ //encontrou o item
        if (i == 0){
          menu.activeMenu = 1; //voltar para o menu principal  
        }
        else if (i == 1){
          //set x
          position_manual_motors(MOTOR_X_MASK);
        }
        else if (i == 2){
          //set y
          position_manual_motors(MOTOR_Y_MASK);
        }
        else if (i == 3){
          //set z
          position_manual_motors(MOTOR_Z_MASK);
        }
        else if (i == 4){
          //execute
        }
      }
    }
  }
  else if(menu.activeMenu == 4){    // menu de posicionamento automatico
    for (uint8_t i = 0; i < NUM_POSITION_AUTO_MENU_ITEMS; i++){
      if (menu.positionAutoMenu[i].posy == menu.posicaoCursor){ //encontrou o item
        if (i == 0){
          if (lastKey == 1){
            lcdUpdate = 1;
            menu.activeMenu = 1; //voltar para o menu principal 
          }
        }
        else if (i == 1){ //set x
          if (lastKey == 5){ // aumenta o valor x
            if (valuePostionAutoX <= AXIS_X_RANGE - 50){
              lcdUpdate = 1;        
              valuePostionAutoX += 50;
            }
          }
          else if (lastKey == 2){ // diminui valor x
            if (valuePostionAutoX > 0){
              lcdUpdate = 1;
              valuePostionAutoX -= 50;
            }
          }
          else if (lastKey == 55){
            if (valuePostionAutoX <= AXIS_X_RANGE - 50){
              if(readKeyCounter > 100){
                lcdUpdate = 1;
                valuePostionAutoX += 50;
              }
            }
          }
          else if (lastKey == 22){
            if (valuePostionAutoX > 0){
              if(readKeyCounter > 100){
                lcdUpdate = 1;
                valuePostionAutoX -= 50;
              }
            }
          }
        }
        else if (i == 2){ //set y
          if (lastKey == 5){ // aumenta o valor y
            if (valuePostionAutoY <= AXIS_Y_RANGE - 50){
              lcdUpdate = 1;        
              valuePostionAutoY += 50;
            }
          }
          else if (lastKey == 2){ // diminui valor y
            if (valuePostionAutoY > 0){
              lcdUpdate = 1;
              valuePostionAutoY -= 50;
            }
          }
          else if (lastKey == 55){
            if (valuePostionAutoY <= AXIS_Y_RANGE - 50){
              if(readKeyCounter > 100){
                lcdUpdate = 1;
                valuePostionAutoY += 50;
              }
            }
          }
          else if (lastKey == 22){
            if (valuePostionAutoY > 0){
              if(readKeyCounter > 100){
                lcdUpdate = 1;
                valuePostionAutoY -= 50;
              }
            }
          }
        }
        else if (i == 3){ //set z
          if (lastKey == 5){ // aumenta o valor z
            if (valuePostionAutoZ <= AXIS_Z_RANGE - 50){
              lcdUpdate = 1;        
              valuePostionAutoZ += 50;
            }
          }
          else if (lastKey == 2){ // diminui valor x
            if (valuePostionAutoZ > 0){
              lcdUpdate = 1;
              valuePostionAutoZ -= 50;
            }
          }
          else if (lastKey == 55){
            if (valuePostionAutoZ <= AXIS_Z_RANGE - 50){
              if(readKeyCounter > 100){
                lcdUpdate = 1;
                valuePostionAutoZ += 50;
              }
            }
          }
          else if (lastKey == 22){
            if (valuePostionAutoZ > 0){
              if(readKeyCounter > 100){
                lcdUpdate = 1;
                valuePostionAutoZ -= 50;
              }
            }
          }
        }
        else if (i == 4){ //execute
          
          uint16_t motorMask = 0;
          if (valuePostionAutoX != valuePostionAutoXanterior)
            motorMask |= MOTOR_X_MASK;
          if (valuePostionAutoY != valuePostionAutoYanterior)
            motorMask |= MOTOR_Y_MASK;
          if (valuePostionAutoZ != valuePostionAutoZanterior)
            motorMask |= MOTOR_Z_MASK;
          //printf("motorMask = %x\n",motorMask);
          if (getMotorRunning() == 0){
            control_goto(valuePostionAutoX, valuePostionAutoY, valuePostionAutoZ, motorMask);
          }
          
          valuePostionAutoXanterior = valuePostionAutoX;
          valuePostionAutoYanterior = valuePostionAutoY;
          valuePostionAutoZanterior = valuePostionAutoZ;
        }
      }
    }
  }
  else if(menu.activeMenu == 5){    // menu de configuração dos parâmetros
    for (uint8_t i = 0; i < NUM_CONFIG_MENU_ITEMS; i++){
      if (menu.configMenu[i].posy == menu.posicaoCursor){ //encontrou o item
        if (i == 0){
          menu.activeMenu = 1; //avançar para o menu principal          
        }
        else if (i == 1){
          menu.activeMenu = 6; //avançar para o menu config x
        }
        else if (i == 2){
          menu.activeMenu = 7; //avançar para o menu config y
        }
        else if (i == 3){
          menu.activeMenu = 8;  //avançar para o menu config z
        }
      }
    }    
  }
  else if(menu.activeMenu == 6){ //Config X
    for (uint8_t i = 0; i < NUM_CONFIG_MENU_X_ITEMS; i++){
      if (menu.configXmenu[i].posy == menu.posicaoCursor){ //encontrou o item
        if (i == 0){
          if (lastKey == 1){
            lcdUpdate = 1;
            paramX = read_parameters(X_FLASH_ADDR_ID);
            menu.activeMenu = 5; //voltar para o menu Config
          }
        }
        else if (i == 1){ //set Kx
          if (lastKey == 5){ // aumenta o valor Kx
            lcdUpdate = 1;        
            paramX.kp += 1;            
          }
          else if (lastKey == 2){ // diminui valor Kx
            if (paramX.kp > 1){
              lcdUpdate = 1;
              paramX.kp -= 1;
            }
          }
        }
        else if (i == 2){ //set Tix
          if (lastKey == 5){ // aumenta o valor Tix
            lcdUpdate = 1;
            if (paramX.ti <= 0.0001){
              paramX.ti += 0.00001;
            }
            else if (paramX.ti <= 0.001){
              paramX.ti += 0.0001;
            }
            else if (paramX.ti <= 0.01){
              paramX.ti += 0.001;
            }
            else{
              paramX.ti += 0.01;
            }
          }
          else if (lastKey == 2){ // diminui valor Tix
            lcdUpdate = 1;
            if (paramX.ti >= 0.02){
              paramX.ti -= 0.01;
            }
            else if (paramX.ti >= 0.002){
              paramX.ti -= 0.001;
            }
            else if(paramX.ti >= 0.0002){
              paramX.ti -= 0.0001;
            }
            else if(paramX.ti >= 0.00002){              
              paramX.ti -= 0.00001;
            }
          }
        }
        else if (i == 3){ //set Tdx
          if (lastKey == 5){ // aumenta o valor Tdx
            lcdUpdate = 1;
            paramX.td += 0.1;
          }
          else if (lastKey == 2){ // diminui valor Tdx
            lcdUpdate = 1;
            paramX.td -= 0.1;
          }
        }
        else if (i == 4){  
          lcdUpdate = 1;        
          write_parameters(paramX, X_FLASH_ADDR_ID);
          menu.activeMenu = 5; //voltar para o menu Config 
        }
      }
    }
  }
  else if(menu.activeMenu == 7){ //Config Y
    for (uint8_t i = 0; i < NUM_CONFIG_MENU_Y_ITEMS; i++){
      if (menu.configYmenu[i].posy == menu.posicaoCursor){ //encontrou o item
        if (i == 0){
          if (lastKey == 1){
            lcdUpdate = 1;
            paramY = read_parameters(Y_FLASH_ADDR_ID);
            menu.activeMenu = 5; //voltar para o menu Config
          }
        }
        else if (i == 1){ //set Ky
          if (lastKey == 5){ // aumenta o valor Ky
            lcdUpdate = 1;        
            paramY.kp += 1;            
          }
          else if (lastKey == 2){ // diminui valor Ky
            if (paramY.kp > 1){
              lcdUpdate = 1;
              paramY.kp -= 1;
            }
          }
        }
        else if (i == 2){ //set Tiy
          if (lastKey == 5){ // aumenta o valor Tiy
            lcdUpdate = 1;
            if (paramY.ti <= 0.0001){
              paramY.ti += 0.00001;
            }
            else if (paramY.ti <= 0.001){
              paramY.ti += 0.0001;
            }
            else if (paramY.ti <= 0.01){
              paramY.ti += 0.001;
            }
            else{
              paramY.ti += 0.01;
            }
          }
          else if (lastKey == 2){ // diminui valor Tiy
            lcdUpdate = 1;
            if (paramY.ti >= 0.02){
              paramY.ti -= 0.01;
            }
            else if (paramY.ti >= 0.002){
              paramY.ti -= 0.001;
            }
            else if(paramY.ti >= 0.0002){
              paramY.ti -= 0.0001;
            }
            else if(paramY.ti >= 0.00002){              
              paramY.ti -= 0.00001;
            }
          }
        }
        else if (i == 3){ //set Tdy
          if (lastKey == 5){ // aumenta o valor Tdy
            lcdUpdate = 1;
            paramY.td += 0.1;
          }
          else if (lastKey == 2){ // diminui valor Tdy
            lcdUpdate = 1;
            paramY.td -= 0.1;
          }
        }
        else if (i == 4){  
          lcdUpdate = 1;        
          write_parameters(paramY,Y_FLASH_ADDR_ID);
          menu.activeMenu = 5; //voltar para o menu Config 
        }
      }
    }
  }
  else if(menu.activeMenu == 8){ //Config Z
    for (uint8_t i = 0; i < NUM_CONFIG_MENU_Z_ITEMS; i++){
      if (menu.configZmenu[i].posy == menu.posicaoCursor){ //encontrou o item
        if (i == 0){
          if (lastKey == 1){
            lcdUpdate = 1;
            paramZ = read_parameters(Z_FLASH_ADDR_ID);
            menu.activeMenu = 5; //voltar para o menu Config
          }
        }
        else if (i == 1){ //set Kz
          if (lastKey == 5){ // aumenta o valor Kx
            lcdUpdate = 1;        
            paramZ.kp += 1;            
          }
          else if (lastKey == 2){ // diminui valor Kx
            if (paramZ.kp > 1){
              lcdUpdate = 1;
              paramZ.kp -= 1;
            }
          }
        }
        else if (i == 2){ //set Tiz
          if (lastKey == 5){ // aumenta o valor Tiz
            lcdUpdate = 1;
            if (paramZ.ti <= 0.0001){
              paramZ.ti += 0.00001;
            }
            else if (paramZ.ti <= 0.001){
              paramZ.ti += 0.0001;
            }
            else if (paramZ.ti <= 0.01){
              paramZ.ti += 0.001;
            }
            else{
              paramZ.ti += 0.01;
            }
          }
          else if (lastKey == 2){ // diminui valor Tiz
            lcdUpdate = 1;
            if (paramZ.ti >= 0.02){
              paramZ.ti -= 0.01;
            }
            else if (paramZ.ti >= 0.002){
              paramZ.ti -= 0.001;
            }
            else if(paramZ.ti >= 0.0002){
              paramZ.ti -= 0.0001;
            }
            else if(paramZ.ti >= 0.00002){              
              paramZ.ti -= 0.00001;
            }
          }
        }
        else if (i == 3){ //set Tdz
          if (lastKey == 5){ // aumenta o valor Tdz
            lcdUpdate = 1;
            paramZ.td += 0.1;
          }
          else if (lastKey == 2){ // diminui valor Tdz
            lcdUpdate = 1;
            paramZ.td -= 0.1;                          
          }
        }
        else if (i == 4){  
          lcdUpdate = 1;        
          write_parameters(paramZ,Z_FLASH_ADDR_ID);
          menu.activeMenu = 5; //voltar para o menu Config 
        }
      }
    }
  }
  else if(menu.activeMenu == 9){    // menu de stats
    for (uint8_t i = 0; i < NUM_CHECK_STATS_ITEMS; i++){
      if (menu.checkStatsMenu[i].posy == menu.posicaoCursor){ //encontrou o item
        if (i == 0){
          menu.activeMenu = 1; //voltar para o menu principal  
          digitalWrite(CALIB_SET_PIN,HIGH);
        }
        else if (i == 1){ // Status end          
          digitalWrite(CALIB_SET_PIN,LOW);
          endX = digitalRead(CALIB_IN1_PIN);
          endY = digitalRead(CALIB_IN2_PIN);
          endZ = digitalRead(CALIB_IN3_PIN);          
          if (endXanterior != endX || endYanterior != endY || endZanterior != endZ){
            lcdUpdate = 1;
          }
          endXanterior = endX;
          endYanterior = endY;
          endZanterior = endZ;
          
        }
        else if (i == 2){ // Bateria
          //lcdUpdate = 1;
        }
        else if (i == 3){ // Posicionamento
          
        }
        else if (i == 4){ // Lumiosidade
          
        }
        else if (i == 5){ // Contador
          
        }
      }
    }
  }
}

/******************************************************************************
 * @brief executa o menu
 ******************************************************************************/
void do_menu(uint8_t lastKey, uint16_t readKeyCounter){
  if (menu.activeMenu == 1){
    if (lastKey == 4){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 1){
        menu.posicaoCursor = 0;
      }
      else{
        if (menu.mainMenu[0].posy != 0){ //não está no início da lista
          for (uint8_t i = 0; i < NUM_MAIN_MENU_ITEMS; i++){  //shift up items
            menu.mainMenu[i].posy++;
          }             
        }
      }
    }
    else if (lastKey == 3){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 0){
        menu.posicaoCursor = 1;
      }
      else{
        if (menu.mainMenu[NUM_MAIN_MENU_ITEMS - 1].posy != 1){ //não está no fim da lista
          for (uint8_t i = 0; i < NUM_MAIN_MENU_ITEMS; i++){  //shift down items
            menu.mainMenu[i].posy--;
          }
        }
      }     
    }
    else if (lastKey == 1){
      lcdUpdate = 1;
      select_current_item(lastKey, readKeyCounter);  
    }
  }
  else if(menu.activeMenu == 2){
    if (lastKey == 4){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 1){
        menu.posicaoCursor = 0;
      }
      else{
        if (menu.calibrateMenu[0].posy != 0){ //não está no início da lista
          for (uint8_t i = 0; i < NUM_CALIBRATE_MENU_ITEMS; i++){  //shift up items
            menu.calibrateMenu[i].posy++;
          }             
        }
      }
    }
    else if (lastKey == 3){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 0){
        menu.posicaoCursor = 1;
      }
      else{
        if (menu.calibrateMenu[NUM_CALIBRATE_MENU_ITEMS - 1].posy != 1){ //não está no fim da lista
          for (uint8_t i = 0; i < NUM_CALIBRATE_MENU_ITEMS; i++){  //shift down items
            menu.calibrateMenu[i].posy--;
          }             
        }
      }     
    }
    else if (lastKey == 1){
      lcdUpdate = 1;
      select_current_item(lastKey, readKeyCounter);  
    }
  }
  else if(menu.activeMenu == 3){
    lcdUpdate = 1;
    if (lastKey == 4){
      if (menu.posicaoCursor == 1){
        menu.posicaoCursor = 0;
      }
      else{
        if (menu.positionManualMenu[0].posy != 0){ //não está no início da lista
          for (uint8_t i = 0; i < NUM_POSITION_MANUAL_MENU_ITEMS; i++){  //shift up items
            menu.positionManualMenu[i].posy++;
          }             
        }
      }
    }
    else if (lastKey == 3){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 0){
        menu.posicaoCursor = 1;
      }
      else{
        if (menu.positionManualMenu[NUM_POSITION_MANUAL_MENU_ITEMS - 1].posy != 1){ //não está no fim da lista
          for (uint8_t i = 0; i < NUM_POSITION_MANUAL_MENU_ITEMS; i++){  //shift down items
            menu.positionManualMenu[i].posy--;
          }             
        }
      }     
    }
    else if (lastKey == 1){
      lcdUpdate = 1;
      select_current_item(lastKey, readKeyCounter);  
    }
  }else if(menu.activeMenu == 4){ // cursor move up
    if (lastKey == 4){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 1){
        menu.posicaoCursor = 0;
      }
      else{
        if (menu.positionAutoMenu[0].posy != 0){ //não está no início da lista
          for (uint8_t i = 0; i < NUM_POSITION_AUTO_MENU_ITEMS; i++){  //shift up items
            menu.positionAutoMenu[i].posy++;
          }             
        }
      }
    }
    else if (lastKey == 3){ // cursor move down
      lcdUpdate = 1;
      if (menu.posicaoCursor == 0){
        menu.posicaoCursor = 1;
      }
      else{
        if (menu.positionAutoMenu[NUM_POSITION_AUTO_MENU_ITEMS - 1].posy != 1){ //não está no fim da lista
          for (uint8_t i = 0; i < NUM_POSITION_AUTO_MENU_ITEMS; i++){  //shift down items
            menu.positionAutoMenu[i].posy--;
          }             
        }
      }     
    }
    else if (lastKey == 1 || lastKey == 2 || lastKey == 22 || lastKey == 5 || lastKey == 55){ // select, direita, esquerda
      select_current_item(lastKey, readKeyCounter);  
    }
  }
  else if(menu.activeMenu == 5){ // cursor move up
    if (lastKey == 4){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 1){
        menu.posicaoCursor = 0;
      }
      else{
        if (menu.configMenu[0].posy != 0){ //não está no início da lista
          for (uint8_t i = 0; i < NUM_CONFIG_MENU_ITEMS; i++){  //shift up items
            menu.configMenu[i].posy++;
          }             
        }
      }
    }
    else if (lastKey == 3){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 0){
        menu.posicaoCursor = 1;
      }
      else{
        if (menu.configMenu[NUM_CONFIG_MENU_ITEMS - 1].posy != 1){ //não está no fim da lista
          for (uint8_t i = 0; i < NUM_CONFIG_MENU_ITEMS; i++){  //shift down items
            menu.configMenu[i].posy--;
          }
        }
      }     
    }
    else if (lastKey == 1){
      lcdUpdate = 1;
      select_current_item(lastKey, readKeyCounter);  
    }
  }
  else if(menu.activeMenu == 6){
    if (lastKey == 4){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 1){
        menu.posicaoCursor = 0;
      }
      else{
        if (menu.configXmenu[0].posy != 0){ //não está no início da lista
          for (uint8_t i = 0; i < NUM_CONFIG_MENU_X_ITEMS; i++){  //shift up items
            menu.configXmenu[i].posy++;
          }             
        }
      }
    }
    else if (lastKey == 3){ // cursor move down
      lcdUpdate = 1;
      if (menu.posicaoCursor == 0){
        menu.posicaoCursor = 1;
      }
      else{
        if (menu.configXmenu[NUM_CONFIG_MENU_X_ITEMS - 1].posy != 1){ //não está no fim da lista
          for (uint8_t i = 0; i < NUM_CONFIG_MENU_X_ITEMS; i++){  //shift down items
            menu.configXmenu[i].posy--;
          }             
        }
      }     
    }
    else if (lastKey == 1 || lastKey == 2 || lastKey == 5){ // select, direita, esquerda
      select_current_item(lastKey, readKeyCounter);  
    }
  }
  else if(menu.activeMenu == 7){
    if (lastKey == 4){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 1){
        menu.posicaoCursor = 0;
      }
      else{
        if (menu.configYmenu[0].posy != 0){ //não está no início da lista
          for (uint8_t i = 0; i < NUM_CONFIG_MENU_Y_ITEMS; i++){  //shift up items
            menu.configYmenu[i].posy++;
          }             
        }
      }
    }
    else if (lastKey == 3){ // cursor move down
      lcdUpdate = 1;
      if (menu.posicaoCursor == 0){
        menu.posicaoCursor = 1;
      }
      else{
        if (menu.configYmenu[NUM_CONFIG_MENU_Y_ITEMS - 1].posy != 1){ //não está no fim da lista
          for (uint8_t i = 0; i < NUM_CONFIG_MENU_Y_ITEMS; i++){  //shift down items
            menu.configYmenu[i].posy--;
          }             
        }
      }     
    }
    else if (lastKey == 1 || lastKey == 2 || lastKey == 5){ // select, direita, esquerda
      select_current_item(lastKey, readKeyCounter);  
    }
  }
  else if(menu.activeMenu == 8){
    if (lastKey == 4){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 1){
        menu.posicaoCursor = 0;
      }
      else{
        if (menu.configZmenu[0].posy != 0){ //não está no início da lista
          for (uint8_t i = 0; i < NUM_CONFIG_MENU_Z_ITEMS; i++){  //shift up items
            menu.configZmenu[i].posy++;
          }             
        }
      }
    }
    else if (lastKey == 3){ // cursor move down
      lcdUpdate = 1;
      if (menu.posicaoCursor == 0){
        menu.posicaoCursor = 1;
      }
      else{
        if (menu.configZmenu[NUM_CONFIG_MENU_Z_ITEMS - 1].posy != 1){ //não está no fim da lista
          for (uint8_t i = 0; i < NUM_CONFIG_MENU_Z_ITEMS; i++){  //shift down items
            menu.configZmenu[i].posy--;
          }             
        }
      }     
    }
    else if (lastKey == 1 || lastKey == 2 || lastKey == 5){ // select, direita, esquerda
      select_current_item(lastKey, readKeyCounter);  
    }
  }
  else if(menu.activeMenu == 9){ // cursor move up
    if (lastKey == 4){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 1){
        menu.posicaoCursor = 0;
      }
      else{
        if (menu.checkStatsMenu[0].posy != 0){ //não está no início da lista
          for (uint8_t i = 0; i < NUM_CHECK_STATS_ITEMS; i++){  //shift up items
            menu.checkStatsMenu[i].posy++;
          }             
        }
      }
    }
    else if (lastKey == 3){
      lcdUpdate = 1;
      if (menu.posicaoCursor == 0){
        menu.posicaoCursor = 1;
      }
      else{
        if (menu.checkStatsMenu[NUM_CHECK_STATS_ITEMS - 1].posy != 1){ //não está no fim da lista
          for (uint8_t i = 0; i < NUM_CHECK_STATS_ITEMS; i++){  //shift down items
            menu.checkStatsMenu[i].posy--;
          }
        }
      }     
    }
    else if (lastKey == 1){          
      select_current_item(lastKey, readKeyCounter);  
    }    
  }
  if (lcdUpdate){
    display_menu();
    lcdUpdate = 0;
  }
}

void do_menu_keyless(void){
  if(menu.activeMenu == 9){
    for (uint8_t i = 0; i < NUM_CHECK_STATS_ITEMS; i++){
      if (menu.checkStatsMenu[i].posy == menu.posicaoCursor){
        if (i == 1 || i == 2){
          select_current_item(0, 0); 
          if (lcdUpdate){
            display_menu();
            lcdUpdate = 0;
          }
        }
      }
    }
  }
}
