/** @file 3axi_motor.cpp
 * 
 * @brief Ficheiro de controlo dos motores da ponte de 3 eixos. 
 *
 */ 
#include "3axi_motor.h"

volatile uint32_t posx = 0;
volatile uint32_t posy = 0;
volatile uint32_t posz = 0;
uint8_t dirx = 0;
uint8_t diry = 0;
uint8_t dirz = 0;

volatile uint16_t motorActive = MOTORS_DISABLED; // referência #ENABLED_3AXI_MOTORS
uint8_t xCalibrated = 0, yCalibrated = 0, zCalibrated = 0; //0 não calibrado, 1 calibrado, 2 em calibração


//motors variables
volatile uint8_t toggleX = 0;
volatile uint8_t toggleY = 0;
volatile uint8_t toggleZ = 0;
uint8_t flag_motor_running = 0;
axiThreadsStates_t threadsState;

float h = 0.001;
uint16_t wd = 500;

//parâmetros de controlo motor x
volatile int32_t ex;
volatile int32_t ux;
volatile int32_t rx = 0;
volatile float ix = 0;
volatile float ix2 = 0;
volatile float dx;
uint8_t dxPeriodFlag = 0;
uint16_t controlXcounter = 0;
float s0x;

//parâmetros de controlo motor y
volatile int32_t ey;
volatile int32_t uy;
volatile int32_t ry = 0;
volatile float iy = 0;
volatile float iy2 = 0;
volatile float dy;
uint8_t dyPeriodFlag = 0;
uint16_t controlYcounter = 0;
float s0y;

//parâmetros de controlo motor z
volatile int32_t ez;
volatile int32_t uz;
volatile int32_t rz = 0;
volatile float iz = 0;
volatile float iz2 = 0;
volatile float dz;
uint8_t dzPeriodFlag = 0;
uint16_t controlZcounter = 0;
float s0z;

//Parâmetros de controlador
parameters_t paramX;
parameters_t paramY;
parameters_t paramZ;

uint8_t printControlFlag = 0;

uint8_t getPrintFlag(void){
  return printControlFlag;
}
void setPrintFlag(uint8_t flag){
  printControlFlag = flag;
}

/******************************************************************************
 * @brief retorna os estados das threads
 * 
 * @param axiThreadsStates estados na estrutura axiThreadsStates_t
 ******************************************************************************/
void reset_threads_flags(void){
  threadsState.handlerXState = 0;
  threadsState.handlerYState = 0;
  threadsState.handlerZState = 0;
  threadsState.controlerState = 0;
}
axiThreadsStates_t get_3axi_threads_state(void){
    return threadsState;
}

/******************************************************************************
 * @brief Retorna o estado se está algum motor a correr
 * 
 * @return flag_motor_running
 ******************************************************************************/
uint8_t getMotorRunning(void){
    return flag_motor_running;
}

/******************************************************************************
 * @brief Retorna o valor da posição x
 * 
 * @return posx
 ******************************************************************************/
uint32_t get_posX(void){
    return posx;
}

/******************************************************************************
 * @brief Retorna o valor da posição y
 * 
 * @return posy
 ******************************************************************************/
uint32_t get_posY(void){
    return posy;
}

/******************************************************************************
 * @brief Retorna o valor da posição z
 * 
 * @return posz
 ******************************************************************************/
uint32_t get_posZ(void){
    return posz;
}

/******************************************************************************
 * @brief Retorna os valores de controlo
 * 
 * @return controlValues
 ******************************************************************************/
void get_control_values(controlValues_t *controlValues){
    controlValues_t tmp;
    //printf("posZ: %d ez: %d uz: %d\n",posz, ez, uz);
    //printf("counter = %d\t%d\n",controlXcounter, controlYcounter);
    //printf("pos x = %d\ty = %d\tz = %d\n",posx,posy,posz);
    /*tmp.ex = ex;
    tmp.ux = ux;
    tmp.rx = rx;
    tmp.ix = ix;
    tmp.dx = dx;

    tmp.ey = ey;
    tmp.uy = uy;
    tmp.ry = ry;
    tmp.iy = iy;
    tmp.dy = dy;

    tmp.ez = ez;
    tmp.uz = uz;
    tmp.rz = rz;
    tmp.iz = iz;
    tmp.dz = dz;*/

    controlValues = &tmp;
}

/******************************************************************************
 * @brief Retorna os parâmetros de controlo
 * 
 * @return 
 ******************************************************************************/
void get_control_params(parameters_t *px, parameters_t *py, parameters_t *pz){
  px = &paramX;
  py = &paramY;
  pz = &paramZ;
}

/******************************************************************************
 * @brief Carrega os valores dos controladores nas variáveis globais
 ******************************************************************************/
void load_control_params(void){
  
  Serial.print("First time boot? ");
  if (read_first_boot() != 5){   
    delay(3000); 
    paramX.kp = KX_DEFAULT_VALUE;
    paramX.ti = TIX_DEFAULT_VALUE;
    paramX.td = TDX_DEFAULT_VALUE;
    write_parameters(paramX, X_FLASH_ADDR_ID);
    paramY.kp = KY_DEFAULT_VALUE;
    paramY.ti = TIY_DEFAULT_VALUE;
    paramY.td = TDY_DEFAULT_VALUE;
    write_parameters(paramY, Y_FLASH_ADDR_ID);
    paramZ.kp = KZ_DEFAULT_VALUE;
    paramZ.ti = TIZ_DEFAULT_VALUE;
    paramZ.td = TDZ_DEFAULT_VALUE;
    write_parameters(paramZ, Z_FLASH_ADDR_ID);
    Serial.print("yes, load default\n");
    set_first_boot();
  }
  else{    
    Serial.print("no, load saved\n");
    paramX = read_parameters(X_FLASH_ADDR_ID);
    paramY = read_parameters(Y_FLASH_ADDR_ID);
    paramZ = read_parameters(Z_FLASH_ADDR_ID);
  }

  write_parameters(paramX, X_FLASH_ADDR_ID);
  write_parameters(paramY, Y_FLASH_ADDR_ID);
  write_parameters(paramZ, Z_FLASH_ADDR_ID);
}

/*******************************************************************************
 * @brief Retorna o número de motores ativos
 *
 * @return Número de motores ativos
 *******************************************************************************/
uint8_t active_motors_num (void){
  return (motorActive & MOTOR_X_MASK) + ((motorActive & MOTOR_Y_MASK) >> 4) + ((motorActive & MOTOR_Z_MASK) >> 8);
}

/*******************************************************************************
 * @brief Ativação dos motores, só suporta 1 motor ativo de cada vez
 *
 * @param[in] motorN  Motor a ativar. 1->motorX, 2->motorY, 3->motorZ
 * 
 * @return 0 success
 * @return 1 failure - number of motors on not supported
 *******************************************************************************/
uint8_t enable_motor (uint16_t motorN){
  //check número de motores ativos
  if (active_motors_num() < NUM_MOTORS_SUPPORTED){
    switch(motorN)
    {
      case MOTOR_X_MASK:
        digitalWrite(MOTOR1_EN_PIN, LOW); // on
        //digitalWrite(MOTOR2_EN_PIN, LOW);
        motorActive |= MOTOR_X_MASK;
        //printf("enx\n");
      break;
      case MOTOR_Y_MASK:
        digitalWrite(MOTOR2_EN_PIN, LOW); // on
        motorActive |= MOTOR_Y_MASK;
        //printf("eny\n");
      break;
      case MOTOR_Z_MASK:
        digitalWrite(MOTOR3_EN_PIN, LOW); // on
        motorActive |= MOTOR_Z_MASK;
        //printf("enz\n");
      break;
    }
    //delay(100);
    //printf("enable %x\n", motorActive);
    return 0;
  }
  else{
    return 1;
  }
}


/*******************************************************************************
 * @brief Desativação dos motores
 *
 * @param[in] motorN  Motor a desativar. 1->motorX, 2->motorY, 3->motorZ, 4->todos 
 *******************************************************************************/
void disable_motor (uint16_t motorN){
  switch(motorN)
  {
    case MOTOR_X_MASK:
      digitalWrite(MOTOR1_EN_PIN, HIGH); // off
      //digitalWrite(MOTOR2_EN_PIN, HIGH); // off
      motorActive &= ~MOTOR_X_MASK;
      Timer2.stop();
      //printf("disx\n");
    break;
    case MOTOR_Y_MASK:
      digitalWrite(MOTOR2_EN_PIN, HIGH); // off
      motorActive &= ~MOTOR_Y_MASK;
      Timer4.stop();
      //printf("disy\n");
    break;
    case MOTOR_Z_MASK:
      digitalWrite(MOTOR3_EN_PIN, HIGH); // off
      motorActive &= ~MOTOR_Z_MASK;
      Timer5.stop();
      //printf("disz\n");
    break;
    case MOTORS_DISABLED:
      digitalWrite(MOTOR1_EN_PIN, HIGH); // off
      digitalWrite(MOTOR2_EN_PIN, HIGH); // off
      digitalWrite(MOTOR3_EN_PIN, HIGH);  // off
      Timer2.stop();
      Timer4.stop();
      Timer5.stop();
      motorActive = MOTORS_DISABLED;
    break;
  }
  //delay(100);
  //printf("disable %x\n",motorActive);
}


/******************************************************************************
 * @brief handler de controlo dos parâmetros do sistema de controlo do motor x
 ******************************************************************************/
static void control_handler(){
  //threadsState.controlerState = 1;
  if (motorActive & MOTOR_X_MASK){   
    //Serial.println("ctrl x");
    // cálculo do erro
    ex = rx - posx;
    
    // cálculo do parâmetro integral
    if (ex < wd && ex > -wd){    
      ix = ix2 + s0x*ex;
      ix2 = ix;
    }
    else
      ix = 0;
    
    ux = paramX.kp*ex + ix; //ux temporário
    if (ux > CONTROL_LIMIT){
      ux = CONTROL_LIMIT;
    }
    else if (ux < -CONTROL_LIMIT){
      ux = -CONTROL_LIMIT;
    }
    
    boolean dxCondition = 0;
    if (dxPeriodFlag == 1){
      dx = pow(controlXcounter,paramX.td);
      dxCondition = dx < abs(ux);
    }  
    
    if (dxCondition == 1){
      // cálculo do parâmetro derivativo/arranque suave
      if (ex > 0){
        ux = dx;
      }
      else if (ex < 0){
        ux = -dx;
      }
      else{
        ux = 0;
      }
    }
    else{
      dxPeriodFlag = 0;
      // cálculo do sinal de controlo
      ux = paramX.kp*ex + ix;
    }

    if (ux > CONTROL_LIMIT){
      ux = CONTROL_LIMIT;
    }
    else if (ux < -CONTROL_LIMIT){
      ux = -CONTROL_LIMIT;
    }

    if (ex < -ERRO_X || ex > ERRO_X || dxPeriodFlag){
      if (ux > 0){
        dirx = 1;
      }
      else{
        dirx = 0;
      }
      Timer2.setFrequency(abs(ux)).start(); 
    }
    else{
      //Serial.println("motor x reach");
      disable_motor(MOTOR_X_MASK);
      
      /*if (motorActive == MOTORS_DISABLED){
        //Serial.println("Timer3 stop");
        Timer3.stop();        
        flag_motor_running = 0;
      }*/
    }
    controlXcounter++;
  }

  if (motorActive & MOTOR_Y_MASK){
    // cálculo do erro
    ey = ry - posy;
    
    // cálculo do parâmetro integral
    if (ey < wd && ey > -wd){    
      iy = iy2 + s0y*ey;
      iy2 = iy;
    }
    else
      iy = 0;  
    
    uy = paramY.kp*ey + iy; //uy temporário
    if (uy > CONTROL_LIMIT){
      uy = CONTROL_LIMIT;
    }
    else if (uy < -CONTROL_LIMIT){
      uy = -CONTROL_LIMIT;
    }
    
    boolean dyCondition = 0;
    if (dyPeriodFlag == 1){
      dy = pow(controlYcounter,paramY.td);
      dyCondition = dy < abs(uy);
    }  
    
    if (dyCondition == 1){
      // cálculo do parâmetro derivativo/arranque suave
      if (ey > 0){
        uy = dy;
      }
      else if (ey < 0){
        uy = -dy;
      }
      else{
        uy = 0;
      }
    }
    else{
      dyPeriodFlag = 0;
      // cálculo do sinal de controlo
      uy = paramY.kp*ey + iy;
    }

    if (uy > CONTROL_LIMIT){
      uy = CONTROL_LIMIT;
    }
    else if (uy < -CONTROL_LIMIT){
      uy = -CONTROL_LIMIT;
    }

    if (ey < -ERRO_Y || ey > ERRO_Y || dyPeriodFlag){
      if (uy > 0){
        diry = 1;
      }
      else{
        diry = 0;
      }
      Timer4.setFrequency(abs(uy)).start();
    }
    else{
      disable_motor(MOTOR_Y_MASK);
      //printf("controlYcounter = %d\n",controlYcounter);
      /*if (motorActive == MOTORS_DISABLED){
        //Serial.println("Timer3 stop");
        Timer3.stop();        
        flag_motor_running = 0;
      }*/
    }
    controlYcounter++;
  }

  if (motorActive & MOTOR_Z_MASK){
    // cálculo do erro
    ez = rz - posz;
    
    // cálculo do parâmetro integral
    if (ez < wd && ez > -wd){
      iz = iz2 + s0z*ez;
      iz2 = iz;
    }
    else
      iz = 0;
    
    uz = paramZ.kp*ez + iz; //ux temporário
    if (uz > CONTROL_LIMIT){
      uz = CONTROL_LIMIT;
    }
    else if (uz < -CONTROL_LIMIT){
      uz = -CONTROL_LIMIT;
    }
    
    boolean dzCondition = 0;
    if (dzPeriodFlag == 1){
      dz = pow(controlZcounter, paramZ.td);
      dzCondition = dz < abs(uz);
    }  
    
    if (dzCondition == 1){
      // cálculo do parâmetro derivativo/arranque suave
      if (ez > 0){
        uz = dz;
      }
      else if (ez < 0){
        uz = -dz;
      }
      else{
        uz = 0;
      }
    }
    else{
      dzPeriodFlag = 0;
      // cálculo do sinal de controlo
      uz = paramZ.kp*ez + iz;
    }

    if (uz > CONTROL_LIMIT){
      uz = CONTROL_LIMIT;
    }
    else if (uz < -CONTROL_LIMIT){
      uz = -CONTROL_LIMIT;
    }

    if (ez < -ERRO_Z || ez > ERRO_Z || dzPeriodFlag){
      if (uz > 0){
        dirz = 1;
      }
      else{
        dirz = 0;
      }
      Timer5.setFrequency(abs(uz)).start(); 
    }
    else{
      disable_motor(MOTOR_Z_MASK);
      /*if (motorActive == MOTORS_DISABLED){
        //Serial.println("Timer3 stop");
        Timer3.stop();        
        flag_motor_running = 0;
      }*/
    }
    controlZcounter++;
  }
  
  if (motorActive == MOTORS_DISABLED){
    //Serial.println("Timer3 stop");
    Timer3.stop();        
    flag_motor_running = 0;
  }
  
  printControlFlag = 1; //flag para fazer prints
}

/******************************************************************************
 * @brief Gerador dos pulsos de controlo e ativação do motor x
 ******************************************************************************/
static void motorX_handler(){
  //threadsState.handlerXState = 1;
  if (digitalRead(MOTOR1_ALARM_PIN) == 1){ // validação de alarme
    if (dirx){
      digitalWrite(MOTOR1_DIR_PIN, HIGH);
      //digitalWrite(MOTOR2_DIR_PIN, HIGH);
    }
    else{
      digitalWrite(MOTOR1_DIR_PIN, LOW);
      //digitalWrite(MOTOR2_DIR_PIN, LOW);
    }

    if (toggleX == 1){
      toggleX = 0;
      digitalWrite(MOTOR1_PULSE_PIN,HIGH);
      //digitalWrite(MOTOR2_PULSE_PIN,HIGH);
      if (dirx){
        posx++;
      }
      else{
        if (posx != 0){
          posx--;
        }
      }
    }
    else{
      toggleX = 1;
      digitalWrite(MOTOR1_PULSE_PIN,LOW);
      //digitalWrite(MOTOR2_PULSE_PIN,LOW);
    }
  }
  else{
    flag_motor_running = 0;
    disable_motor(MOTOR_X_MASK);
    Timer3.stop();
    Timer2.stop();
  }

}

/******************************************************************************
 * @brief Gerador dos pulsos de controlo e ativação do motor y
 ******************************************************************************/
static void motorY_handler(){
  //threadsState.handlerYState = 1;
  if (digitalRead(MOTOR2_ALARM_PIN) == 1){ // validação de alarme
    if (diry)
      digitalWrite(MOTOR2_DIR_PIN, HIGH);
    else
      digitalWrite(MOTOR2_DIR_PIN, LOW);

    if (toggleY == 1){
      toggleY = 0;
      digitalWrite(MOTOR2_PULSE_PIN,HIGH);
      if (diry){
        posy++;
      }
      else{
        if (posy != 0){
          posy--;
        }
      }
    }
    else{
      toggleY = 1;
      digitalWrite(MOTOR2_PULSE_PIN,LOW);
    }
  }
  else{
    flag_motor_running = 0;
    disable_motor(MOTOR_Y_MASK);
    Timer3.stop();
    Timer4.stop();
  }
}

/******************************************************************************
 * @brief Gerador dos pulsos de controlo e ativação do motor z
 ******************************************************************************/
static void motorZ_handler(){
  //threadsState.handlerZState = 1;  
  if (digitalRead(MOTOR3_ALARM_PIN) == 1){
    if (dirz)
      digitalWrite(MOTOR3_DIR_PIN, LOW);
    else
      digitalWrite(MOTOR3_DIR_PIN, HIGH);

    if (toggleZ == 1){
      toggleZ = 0;
      digitalWrite(MOTOR3_PULSE_PIN,HIGH);
      if (dirz){
        posz++;
      }
      else{
        if (posz != 0){
          posz--;
        }
      }
    }
    else{
      toggleZ = 1;
      digitalWrite(MOTOR3_PULSE_PIN,LOW);
    }
  }
  else{
    flag_motor_running = 0;
    disable_motor(MOTOR_Z_MASK);
    Timer3.stop();
    Timer5.stop();
  }  
}

/******************************************************************************
 * @brief Inicia os motores
 ******************************************************************************/
void motors_init(void){
  pinMode(MOTOR1_ALARM_PIN,INPUT_PULLUP);
  pinMode(MOTOR1_EN_PIN,OUTPUT);
  pinMode(MOTOR1_DIR_PIN,OUTPUT);
  pinMode(MOTOR1_PULSE_PIN,OUTPUT);
  pinMode(MOTOR2_ALARM_PIN,INPUT_PULLUP);
  pinMode(MOTOR2_EN_PIN,OUTPUT);
  pinMode(MOTOR2_DIR_PIN,OUTPUT);
  pinMode(MOTOR2_PULSE_PIN,OUTPUT);
  pinMode(MOTOR3_ALARM_PIN,INPUT_PULLUP);
  pinMode(MOTOR3_EN_PIN,OUTPUT);
  pinMode(MOTOR3_DIR_PIN,OUTPUT);
  pinMode(MOTOR3_PULSE_PIN,OUTPUT);
  pinMode(CALIB_SET_PIN,OUTPUT);
  pinMode(CALIB_IN1_PIN,INPUT);
  pinMode(CALIB_IN2_PIN,INPUT);
  pinMode(CALIB_IN3_PIN,INPUT);

  pinMode(ANALOG_PIN, INPUT);

  Timer2.attachInterrupt(motorX_handler);
  Timer4.attachInterrupt(motorY_handler);
  Timer5.attachInterrupt(motorZ_handler);
  Timer3.attachInterrupt(control_handler);

  digitalWrite(MOTOR1_DIR_PIN, LOW);
  digitalWrite(MOTOR1_PULSE_PIN, LOW);
  digitalWrite(MOTOR1_EN_PIN, HIGH);

  digitalWrite(MOTOR2_DIR_PIN, LOW);
  digitalWrite(MOTOR2_PULSE_PIN, LOW);
  digitalWrite(MOTOR2_EN_PIN, HIGH);

  digitalWrite(MOTOR3_DIR_PIN, LOW);
  digitalWrite(MOTOR3_PULSE_PIN, LOW);
  digitalWrite(MOTOR3_EN_PIN, HIGH);

  digitalWrite(CALIB_SET_PIN,HIGH);
}


/******************************************************************************
 * @brief Define e inicia o controlo automático do motor indicado em motorMask
 *
 * @param[in] inX  Posição absoluta target do motor x
 * @param[in] inY  Posição absoluta target do motor y
 * @param[in] inZ  Posição absoluta target do motor z
 * @param[in] motorMask motores a controlar
 ******************************************************************************/
uint8_t control_goto(int32_t inX, int32_t inY, int32_t inZ, uint16_t motorMask) //iniciação do controlador
{
  uint32_t controlPeriod = h*1000000;
  if (motorMask != 0)
    flag_motor_running = 1;

  if (motorMask & MOTOR_X_MASK){
    // Validação do alvo
    if (inX < 0)
      inX = 0;
    else if (inX > AXIS_X_RANGE)
      inX = AXIS_X_RANGE;
    
    s0x = 0.1*paramX.ti/h;
    rx = inX;
    ix = 0;
    ix2 = 0;
    dxPeriodFlag = 1;
    ux = paramX.kp*ex + ix;  
    if (ux > CONTROL_LIMIT){
      ux = CONTROL_LIMIT;
    }
    else if (ux < -CONTROL_LIMIT){
      ux = -CONTROL_LIMIT;
    }
    
    controlXcounter = 2;

    enable_motor(MOTOR_X_MASK);
    //enable_motor(MOTOR_Y_MASK); 
    Serial.print("Motor x go to ");
    Serial.println(rx);
    //Serial.print(",\t s0x: ");
    //Serial.println(s0x);
  }
  if (motorMask & MOTOR_Y_MASK){
    // Validação do alvo
    if (inY < 0)
      inY = 0;
    else if (inY > AXIS_Y_RANGE)
      inY = AXIS_Y_RANGE;
    
    s0y = 0.1*paramY.ti/h;
    ry = inY;
    iy = 0;
    iy2 = 0;
    dyPeriodFlag = 1;
    uy = paramY.kp*ey + iy;  
    if (uy > CONTROL_LIMIT){
      uy = CONTROL_LIMIT;
    }
    else if (uy < -CONTROL_LIMIT){
      uy = -CONTROL_LIMIT;
    }
    
    controlYcounter = 2;

    enable_motor(MOTOR_Y_MASK);  
    Serial.print("Motor y go to ");
    Serial.print(ry);
    //Serial.print(",\t s0y: ");
    //Serial.println(s0y);
  }
  if (motorMask & MOTOR_Z_MASK){
    // Validação do alvo
    if (inZ < 0)
      inZ = 0;
    else if (inZ > AXIS_Z_RANGE)
      inZ = AXIS_Z_RANGE;
    
    s0z = 0.1*paramZ.ti/h;
    rz = inZ;
    iz = 0;
    iz2 = 0;
    dzPeriodFlag = 1;
    uz = paramZ.kp*ez + iz;  
    if (uz > CONTROL_LIMIT){
      uz = CONTROL_LIMIT;
    }
    else if (uz < -CONTROL_LIMIT){
      uz = -CONTROL_LIMIT;
    }
    
    controlZcounter = 2;

    enable_motor(MOTOR_Z_MASK);  
    Serial.print("Motor z go to ");
    Serial.print(rz);
    //Serial.print(",\t s0z: ");
    //Serial.println(s0z);
  }
  Timer3.setPeriod(controlPeriod).start();
  return 0;
}

/******************************************************************************
 * @brief Calibração dos motores x, y e z 
 * flags de ativação, 0: não calibra, 1: calibra
 * 
 * @param[in] motorMask   motors to be calibrated
 ******************************************************************************/
void calibrate (uint16_t motorMask){
  disable_motor(MOTORS_DISABLED);
  digitalWrite(CALIB_SET_PIN,LOW);
  delay(100);
 
  //verificar se há erro
  if (motorMask & MOTOR_X_MASK){
    Serial.println("Calibrate X");
    enable_motor(MOTOR_X_MASK);
    if (digitalRead(CALIB_IN1_PIN) == HIGH){
      Serial.println("x started in sensor");
      xCalibrated = 2;
      posx = 0;
      dirx = 1;
      Serial.println("go to x40");
      Timer2.setPeriod(1000).start(); 
      while(posx < 40); //goto x40
      Timer2.stop();
      Serial.println("reach x40");
    }
    if (digitalRead(CALIB_IN1_PIN) == HIGH && xCalibrated == 2){
      //TODO: alarme de erro no motor x
      Serial.println("alarm x");
    }
    else{
      xCalibrated = 2;
      // goto sensor
      dirx = 0;
      Serial.println("go to x sensor");
      Timer2.setPeriod(600).start();
      uint8_t sample1 = 0,sample2 = 0;
      int8_t sampleFiltered = 0;
      while (sampleFiltered == 0){
        sample1 = sample2;
        sample2 = digitalRead(CALIB_IN1_PIN);
        sampleFiltered = sample1 + sample2 - 1;
        if (sampleFiltered < 0){
          sampleFiltered = 0;
        }
      }
      Timer2.stop();
      Serial.println("reach x sensor");
      posx = 0;
      // goto x200
      dirx = 1;
      Serial.println("go to x3000");
      Timer2.setPeriod(600).start();
      while(posx < 1000);
      Timer2.stop();
      Serial.println("reach x200");
    }
    Serial.println("x calibrated");
    disable_motor(MOTOR_X_MASK);
  }

  
  if (motorMask & MOTOR_Y_MASK){
    Serial.println("Calibrate Y");
    enable_motor(MOTOR_Y_MASK);
    //verificar se há erro
    if (digitalRead(CALIB_IN2_PIN) == HIGH){
      Serial.println("y started in sensor");
      yCalibrated = 2;
      posy = 0;
      diry = 1;
      Serial.println("go to y40");
      Timer4.setPeriod(1000).start(); 
      while(posy < 40); //goto y40
      Timer4.stop();
      Serial.println("reach y40");
    }
    if (digitalRead(CALIB_IN2_PIN) == HIGH && yCalibrated == 2){
      //TODO: alarme de erro no motor y
      Serial.println("alarm y");
    }
    else{
      yCalibrated = 2;
      // goto sensor
      diry = 0;
      Serial.println("go to y sensor");
      Timer4.setPeriod(600).start();
      uint8_t sample1 = 0,sample2 = 0;
      int8_t sampleFiltered = 0;
      while (sampleFiltered == 0){
        sample1 = sample2;
        sample2 = digitalRead(CALIB_IN2_PIN);
        sampleFiltered = sample1 + sample2 - 1;
        if (sampleFiltered < 0){
          sampleFiltered = 0;
        }
      }
      Timer4.stop();
      Serial.println("reach y sensor");
      posy = 0;
      // goto y200
      diry = 1;
      Serial.println("go to y200");
      Timer4.setPeriod(600).start();
      while(posy < 1000);
      Timer4.stop();
      Serial.println("reach y200");
    }
    Serial.println("y calibrated");
    disable_motor(MOTOR_Y_MASK);
  }

  if (motorMask & MOTOR_Z_MASK){
    Serial.println("Calibrate Z");
    enable_motor(MOTOR_Z_MASK);
    //verificar se há erro
    if (digitalRead(CALIB_IN3_PIN) == HIGH){
      Serial.println("z started in sensor");
      zCalibrated = 2;
      posz = 0;
      dirz = 1;
      Serial.println("go to z40");
      Timer5.setPeriod(1000).start(); 
      while(posz < 40); //goto z40
      Timer5.stop();
      Serial.println("reach z40");
    }
    if (digitalRead(CALIB_IN3_PIN) == HIGH && zCalibrated == 2){
      //TODO: alarme de erro no motor z
      Serial.println("alarm z");
    }
    else{
      zCalibrated = 2;
      // goto sensor
      dirz = 0;
      Serial.println("go to z sensor");
      Timer5.setPeriod(600).start();
      uint8_t sample1 = 0,sample2 = 0;
      int8_t sampleFiltered = 0;
      while (sampleFiltered == 0){
        sample1 = sample2;
        sample2 = digitalRead(CALIB_IN3_PIN);
        sampleFiltered = sample1 + sample2 - 1;
        if (sampleFiltered < 0){
          sampleFiltered = 0;
        }
      }
      Timer5.stop();
      Serial.println("reach z sensor");
      posz = 0;
      // goto z200
      dirz = 1;
      Serial.println("go to z200");
      Timer5.setPeriod(600).start();
      while(posz < 2000);//AXIS_Z_RANGE);
      Timer5.stop();
      Serial.println("reach z200");
    }
    Serial.println("z calibrated");
    disable_motor(MOTOR_Z_MASK);
  }
  digitalWrite(CALIB_SET_PIN,LOW);
  disable_motor(MOTORS_DISABLED);
}

/******************************************************************************
 * @brief controlo manual da velocidade dos motores nos eixos conforme no menu do lcd
 * 
 * @param[in] motorMask   motor to be controlled throught the mask
 ******************************************************************************/
void position_manual_motors(uint16_t motorMask){
  uint16_t speedMotor = analogRead(ANALOG_PIN);
  uint16_t newSpeedMotor = speedMotor;
  uint16_t key;
  uint32_t count = 0;
  uint32_t motorPosition = 0;
  if(motorPosition);
  
  while(1){
    if (count == 1000000){
      /*lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("speed: " + String(newSpeedMotor));
      lcd.setCursor(0,1);
      lcd.print("pos: " + String(motorPosition));*/
      count = 0;
      newSpeedMotor = analogRead(ANALOG_PIN) + 50;
      if (newSpeedMotor < 50)
        newSpeedMotor = 50;
      
      //Serial.println(newSpeedMotor);
      key = analogRead(A0);
      if (key < 60){        
        if (motorMask & MOTOR_X_MASK){
          enable_motor(MOTOR_X_MASK);
          motorPosition = posx;
          dirx = 0;
          if (newSpeedMotor - speedMotor > 100 || speedMotor - newSpeedMotor > 100){
            Timer2.setPeriod(newSpeedMotor).start();
            speedMotor = newSpeedMotor;
          } 
        }
        else if (motorMask & MOTOR_Y_MASK){
          enable_motor(MOTOR_Y_MASK);
          digitalWrite(MOTOR2_DIR_PIN, HIGH);
          motorPosition = posy;
          diry = 1;
          if (newSpeedMotor - speedMotor > 100 || speedMotor - newSpeedMotor > 100){
            Timer4.setPeriod(newSpeedMotor).start();
            speedMotor = newSpeedMotor;
          }
        }
        else if (motorMask & MOTOR_Z_MASK){
          enable_motor(MOTOR_Z_MASK);
          digitalWrite(MOTOR3_DIR_PIN, HIGH);
          motorPosition = posz;
          dirz = 0;
          if (newSpeedMotor - speedMotor > 100 || speedMotor - newSpeedMotor > 100){
            Timer5.setPeriod(newSpeedMotor).start();
            speedMotor = newSpeedMotor;
          }
        }
      }
      else if (key < 200); //Up
      else if (key < 400); //Down
      else if (key < 800){
        if (motorMask & MOTOR_X_MASK){          
          enable_motor(MOTOR_X_MASK);
          motorPosition = posx;
          dirx = 1;
          if (newSpeedMotor - speedMotor > 100 || speedMotor - newSpeedMotor > 100){
            Timer2.setPeriod(newSpeedMotor).start();
            speedMotor = newSpeedMotor;
          }
        }
        else if (motorMask & MOTOR_Y_MASK){
          enable_motor(MOTOR_Y_MASK);
          motorPosition = posy;
          diry = 0;
          if (newSpeedMotor - speedMotor > 100 || speedMotor - newSpeedMotor > 100){
            Timer4.setPeriod(newSpeedMotor).start();
            speedMotor = newSpeedMotor;
          }
        }
        else if (motorMask & MOTOR_Z_MASK){
          enable_motor(MOTOR_Z_MASK);
          motorPosition = posz;
          dirz = 1;
          if (newSpeedMotor - speedMotor > 100 || speedMotor - newSpeedMotor > 100){
            Timer5.setPeriod(newSpeedMotor).start();
            speedMotor = newSpeedMotor;
          }
        }
      }
      else if (key < 1000){ //Select
        disable_motor(MOTORS_DISABLED);
        break;
      }
      else{
        disable_motor(MOTORS_DISABLED);
        speedMotor = 10000;
      }
    }
    else{
      count++;
    }
  }
}