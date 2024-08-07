#ifndef FLASH_DRIVER_ENABLED
#define FLASH_DRIVER_ENABLED

// flash address
#define FIRST_BOOT_ID     0
#define X_FLASH_ADDR_ID  4
#define Y_FLASH_ADDR_ID  8
#define Z_FLASH_ADDR_ID  12

#include <Arduino.h>

struct parameters_t{
  uint8_t kp;
  float ti;
  float td;
};


/******************************************************************************
 * @brief Escreve os parâmetros de controlo do motor x na memória flash, é resetada no primeiro arranque
 *
 * @param[in] parametersX parâmetros para gravar da estrutura parameters_t
 ******************************************************************************/
void write_parameters(parameters_t parametersX, uint8_t id);

/******************************************************************************
 * @brief Lê os parâmetros de controlo do motor x na memória flash
 * 
 * @return parametersFromFlashOut - parâmetros lidos da flash
 ******************************************************************************/
parameters_t read_parameters(uint8_t id);

/******************************************************************************
 * @brief Verifica se é o primeiro arranque
 *
 * @return boot flag - 5 = first boot
 ******************************************************************************/
uint8_t read_first_boot(void);

/******************************************************************************
 * @brief Define o primeiro arranque ativando a flag
 ******************************************************************************/
void set_first_boot(void);

#endif