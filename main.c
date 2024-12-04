#include "user_app.h"

void main( void )
{
//    config_user_app();
    
    void led_init();
    void adc_init();

    /*
    xTaskCreate(task_1, "t1", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(task_2, "t2", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(task_3, "t3", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    
    
    xTaskCreate(task_1, "t1", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(task_2, "t2", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    
    
    xTaskCreate(cozinheiro, "co", configMINIMAL_STACK_SIZE, NULL, 5, NULL);
    xTaskCreate(canibal_1, "c1", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(canibal_2, "c2", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    xTaskCreate(canibal_3, "c3", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    
    */
    
//    xTaskCreate(teste_sem_contador, "T", configMINIMAL_STACK_SIZE, NULL, 4, NULL);
    
    xTaskCreate(vTaskReadSensor, "Read Sensor", 128, NULL, 1, NULL);
    xTaskCreate(vTaskControl, "Control", 128, NULL, 1, NULL);
    
    /* Start the scheduler. */
    vTaskStartScheduler();

    for( ; ; )
    {
        /* Should not reach here. */
    }
}
