/** @file PS2X.h
 * 
 * @brief Header for the PS2X controller drivers. 
 *
 */ 


 #ifndef PS2X_ENABLED
 #define PS2X_ENABLED
 
 /****************************** Includes *************************************/
 
 
 
 /****************************** Constants *************************************/

#define pressures   false

#define rumble      false
#define PS2_DAT        32   //MISO  19
#define PS2_CMD        33   //MOSI  23
#define PS2_SEL        26   //SS     5
#define PS2_CLK        27   //SLK   18
 
 /****************************** Structures *************************************/
 
 
 /****************************** Function Prototypes *************************************/
 uint8_t controller_init(void);
 void get_controller_cmd(void);
 
 #endif