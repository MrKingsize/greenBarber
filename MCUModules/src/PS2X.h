/** @file PS2X.h
 * 
 * @brief Header for the PS2X controller drivers. 
 *
 */ 


 #ifndef PS2X_ENABLED
 #define PS2X_ENABLED
 
 /****************************** Includes *************************************/
 
 
 
 /****************************** Constants *************************************/
 //#define pressures   true
#define pressures   false
//#define rumble      true
#define rumble      false
#define PS2_DAT_PIN        13  //14    
#define PS2_CMD_PIN        11  //15
#define PS2_SEL_PIN        10  //16
#define PS2_CLK_PIN        12  //17
    
 
 /****************************** Structures *************************************/
 
 
 
 /****************************** Function Prototypes *************************************/
 void controller_init(void);
 void get_controller_cmd(void);
 
 #endif