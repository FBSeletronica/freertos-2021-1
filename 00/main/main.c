/*********************************************************
* Exemplo Blink LED - Task
* Pisca o LED da placa em intervalos de 500 milisegundo
* Esse exemplo tem a finalidade de exibir a criação de uma task
* Por: Fábio Souza
*********************************************************/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

/* Can use project configuration menu (idf.py menuconfig) to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
#define BLINK_GPIO CONFIG_BLINK_GPIO

/* Protótipo da Task*/
void vTaskBlink(void *pvParameters ); 

void app_main(void)
{
    
    xTaskCreate (
     vTaskBlink
    ,  "TaskBlink"              /* Nome da Task */
    ,  configMINIMAL_STACK_SIZE /* Stack Size, não se preocupe com esse valor agora. Vamos estudar mais pra frente*/
    ,  NULL                     /* parametro passado para a task*/
    ,  2                        /* Prioridade da task*/
    ,  NULL);                   /* handle da task*/

   
}

/* Task Blink LED */
void vTaskBlink(void *pvParameters ){

  (void) pvParameters;
  
  /* Set the GPIO as a push/pull output */
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
  while(1){
    /* Blink off (output low) */
        printf("Turning off the LED\n");
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        printf("Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        
  }
}