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
#define PS2_DAT        36
#define PS2_CMD        38
#define PS2_SEL        32
#define PS2_CLK        34
    
 
 /****************************** Structures *************************************/
 
 
 /****************************** Function Prototypes *************************************/
 void controller_init(void);
 void get_controller_cmd(void);
 uint8_t get_forward_speed(void);
 
 #endif