#ifndef USER_APP_H
#define	USER_APP_H

#include "FreeRTOS.h"
#include "task.h"
#include "xc.h"
#include "FreeRTOSConfig.h"
#include "semphr.h"

void config_user_app();
//void task_1();
//void task_2();
//void task_3();

//void cozinheiro();
//void canibal_1();
//void canibal_2();
//void canibal_3();
//
//void teste_sem_contador();

void led_init();
void adc_init();
uint16_t adc_read();
void vTaskReadSensor(void *pvParameters);
void vTaskControl(void *pvParameters);

#endif	/* USER_APP_H */

