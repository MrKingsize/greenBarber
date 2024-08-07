/** @file 3axi_motor.h
 * 
 * @brief Header de controlo dos motores da ponte de 3 eixos. 
 *
 */ 
#ifndef AXI_MOTOR_ENABLED
#define AXI_MOTOR_ENABLED

#include <Arduino.h>
#include <DueTimer.h>
#include "flash_driver.h"

/*******************************************************************
 * Constants
 ******************************************************************/

#define MOTOR1_ALARM_PIN    40
#define MOTOR1_EN_PIN       42
#define MOTOR1_DIR_PIN      43
#define MOTOR1_PULSE_PIN    41

#define MOTOR2_ALARM_PIN    45
#define MOTOR2_EN_PIN       47
#define MOTOR2_DIR_PIN      46
#define MOTOR2_PULSE_PIN    44

#define MOTOR3_ALARM_PIN    48
#define MOTOR3_EN_PIN       50
#define MOTOR3_DIR_PIN      51
#define MOTOR3_PULSE_PIN    49

#define CALIB_SET_PIN       37
#define CALIB_IN1_PIN       36
#define CALIB_IN2_PIN       39
#define CALIB_IN3_PIN       38

#define AXIS_X_RANGE        16900 //redutor 1:7.5 75cm de curso. 
#define AXIS_Y_RANGE        15400 //redutor 1:5
#define AXIS_Z_RANGE        5000  //redutro 1:
#define ERRO_X              10    // +/-10 = 20, erro < 1mm
#define ERRO_Y              10
#define ERRO_Z              10

#define ANALOG_PIN          A1

#define NUM_MOTORS_SUPPORTED 2

#define CONTROL_LIMIT     10000

// Default Control parameters
#define KX_DEFAULT_VALUE  8
#define TIX_DEFAULT_VALUE 0.0003
#define TDX_DEFAULT_VALUE 1.6
#define KY_DEFAULT_VALUE  8
#define TIY_DEFAULT_VALUE 0.0003
#define TDY_DEFAULT_VALUE 1.6
#define KZ_DEFAULT_VALUE  8
#define TIZ_DEFAULT_VALUE 0.0003
#define TDZ_DEFAULT_VALUE 1.6


#define MOTORS_DISABLED 0x0000
#define MOTOR_X_MASK    0x0001
#define MOTOR_Y_MASK    0x0010
#define MOTOR_Z_MASK    0x0100

struct controlValues_t{
    int32_t ex;
    int32_t ux;
    int32_t rx;
    float ix;
    float dx;
    int32_t ey;
    int32_t uy;
    int32_t ry;
    float iy;
    float dy;
    int32_t ez;
    int32_t uz;
    int32_t rz;
    float iz;
    float dz;
};

struct axiThreadsStates_t{
    uint8_t handlerXState;
    uint8_t handlerYState;
    uint8_t handlerZState;
    uint8_t controlerState;
};

/*******************************************************************
 * Prototypes functions
 ******************************************************************/

uint8_t getPrintFlag(void);
void setPrintFlag(uint8_t flag);

/******************************************************************************
 * @brief retorna os estados das threads
 * 
 * @param axiThreadsStates estados na estrutura axiThreadsStates_t
 ******************************************************************************/
void reset_threads_flags(void);
axiThreadsStates_t get_3axi_threads_state(void);

/******************************************************************************
 * @brief Retorna o estado se está algum motor a correr
 * 
 * @return flag_motor_running
 ******************************************************************************/
uint8_t getMotorRunning(void);

/******************************************************************************
 * @brief controlo manual da velocidade dos motores nos eixos conforme no menu do lcd
 * 
 * @param[in] x   motor x
 * @param[in] y   motor y
 * @param[in] z   motor z
 * 0 -> disabled
 * 1 -> enabled
 ******************************************************************************/
void position_manual_motors(uint16_t motorMask);

/******************************************************************************
 * @brief Calibração dos motores x, y e z 
 * flags de ativação, 0: não calibra, 1: calibra
 * 
 * @param[in] x   motor x
 * @param[in] y   motor y
 * @param[in] z   motor z
 * 0 -> disabled
 * 1 -> enabled
 ******************************************************************************/
void calibrate (uint16_t motorMask);

/******************************************************************************
 * @brief Define e inicia o controlo automático do motor indicado em motorMask
 *
 * @param[in] inX  Posição absoluta target do motor x
 * @param[in] inY  Posição absoluta target do motor y
 * @param[in] inZ  Posição absoluta target do motor z
 * @param[in] motorMask motores a controlar
 ******************************************************************************/
uint8_t control_goto(int32_t inX, int32_t inY, int32_t inZ, uint16_t motorMask);

/******************************************************************************
 * @brief Inicia os motores
 ******************************************************************************/
void motors_init(void);

/*******************************************************************************
 * @brief Desativação dos motores
 *
 * @param[in] motorN  Motor a desativar. 1->motorX, 2->motorY, 3->motorZ, 4->todos 
 *******************************************************************************/
void disable_motor (uint16_t motorN);

/*******************************************************************************
 * @brief Ativação dos motores, só suporta 1 motor ativo de cada vez
 *
 * @param[in] motorN  Motor a ativar. 1->motorX, 2->motorY, 3->motorZ
 * 
 * @return 0 success
 * @return 1 failure - number of motors on not supported
 *******************************************************************************/
uint8_t enable_motor (uint16_t motorN);

/*******************************************************************************
 * @brief Retorna o número de motores ativos
 *
 * @return Número de motores ativos
 *******************************************************************************/
uint8_t active_motors_num (void);

/******************************************************************************
 * @brief Carrega os valores dos controladores nas variáveis globais
 ******************************************************************************/
void load_control_params(void);

/******************************************************************************
 * @brief Retorna os parâmetros de controlo
 * 
 * @return 
 ******************************************************************************/
void get_control_params(parameters_t *px, parameters_t *py, parameters_t *pz);

/******************************************************************************
 * @brief Retorna os valores de controlo
 * 
 * @return controlValues
 ******************************************************************************/
void get_control_values(controlValues_t *controlValues);

/******************************************************************************
 * @brief Retorna o valor da posição z
 * 
 * @return posz
 ******************************************************************************/
uint32_t get_posZ(void);

/******************************************************************************
 * @brief Retorna o valor da posição y
 * 
 * @return posy
 ******************************************************************************/
uint32_t get_posY(void);

/******************************************************************************
 * @brief Retorna o valor da posição x
 * 
 * @return posx
 ******************************************************************************/
uint32_t get_posX(void);

#endif