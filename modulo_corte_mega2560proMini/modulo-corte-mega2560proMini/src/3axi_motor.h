/** @file 3axi_motor.h
 * 
 * @brief Header de controlo dos motores da ponte de 3 eixos. 
 *
 */ 
#ifndef AXI_MOTOR_ENABLED
#define AXI_MOTOR_ENABLED

#include <Arduino.h>
#include <TimerOne.h>

/*******************************************************************
 * Constants
 ******************************************************************/

// velocidade de rotação - escolher só múltiplos de PERIOD_CONTROL.

#define PERIOD_BASE             40 // 40 us
#define PERIOD_CALIB_TAMPA      20*PERIOD_BASE // 800 us
#define PERIOD_TAMPA            3*PERIOD_BASE
#define PERIOD_CALIB_VERTICAL   PERIOD_BASE
#define PERIOD_VERTICAL         PERIOD_BASE
#define PERIOD_ROTATIVO_F       4*PERIOD_BASE
#define PERIOD_ROTATIVO_T       2*PERIOD_BASE
#define TAMPA_LIMIT             6700
#define VERTICAL_LIMIT          26730

// motor pins idx               {MOTOR_TAMPA,   MOTOR_VERTICAL, MOTOR_ROTATIVO}
#define MOTOR_EN_PIN            {24,            18,             12}
#define MOTOR_DIR_PIN           {26,            20,             14}
#define MOTOR_PUL_PIN           {28,            22,             16}

#define CALIB_VER_PIN           10  // ativa a 0
#define SENSOR_LEV_FRENTE_PIN   8   // ativa a 0
#define SENSOR_LEV_ALTO_PIN     6   // ativa a 0
#define SENSOR_LEV_BAIXO_PIN    4   // ativa a 0
#define CALIB_TAMPA_PIN         2   // ativa a 0
#define FAN_EN_PIN              36 
#define CORTE_EN_PIN            38 
#define BUTTON_DIR_PIN          40
#define BUTTON_ESQ_PIN          42
#define BUTTON_SIDE_PIN         44
#define BUTTON_MINI_PIN         46


typedef enum{
    MOTOR_STOP_CMD = 0,
    MOTOR_PLUS_CMD = 1,
    MOTOR_MINUS_CMD = 2,
}MOTOR_CONTROL;

typedef enum{
    MOTOR_TAMPA = 0,
    MOTOR_VERTICAL,
    MOTOR_ROTATIVO,
    MOTOR_LAST,
}MOTOR_ENUM;

struct motor_control_t
{
    volatile uint32_t pos;      // posição atual do motor
    uint8_t dir;                // direção atual do motor 1 -> incrementa ticks, 0 -> decrementa ticks
    int vel;                    // velocidade do motor (período dos "ticks")
    uint8_t motorRunFlag;       // flag a indicar o estado de operação do motor
    uint8_t cmdAnterior;        // comando anterior dado ao controlador do motor
    uint8_t orientation;        // valor da dir para que os pulsos incrementam 
    uint32_t limit;             // limit do curso do motor 0 = infinito
    volatile uint8_t toggle;    // flag para controlo dos pulsos no controlador
    uint8_t calibFlag;          // flag para indicar se o motor está calibrado, também pode ser usado para controlar o motor sem pulsos
    int counterHandler;         // contador de handler para controlo da velocidade
};


void printValuesMotor(MOTOR_ENUM motor);
void motors_init(void);
void motor_control(MOTOR_ENUM motor, MOTOR_CONTROL cmd, uint32_t period);
void disable_motor(MOTOR_ENUM motor);
void enable_motor(MOTOR_ENUM motor);
void calibrateTampa(void);
void calibrateVertical(void);
void goto_pos(MOTOR_ENUM motor, uint32_t targetPos, uint32_t period);
void debugMotor(MOTOR_ENUM motor, uint32_t period);

#endif