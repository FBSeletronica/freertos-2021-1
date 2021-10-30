/*********************************************************
* Exemplo para mostrar a escolha do núcleo de execução da task
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

/* Variáveis para Armazenar o handle das Tasks */
TaskHandle_t xTask1Handle, xTask2Handle;

/* Protótipo das Tasks*/
void vTask1(void *pvParameters ); 
void vTask2(void *pvParameters ); 

void app_main(void)
{
  xTaskCreatePinnedToCore(vTask1,"Task1",2048,NULL,1, &xTask1Handle,APP_CPU_NUM);
  xTaskCreatePinnedToCore(vTask2,"Task2",2048,NULL,1, &xTask2Handle,PRO_CPU_NUM);     
}


/* Task 1 - Blink LED */
void vTask1(void *pvParameters )
{
  
  (void)pvParameters;

  while(1)
  {
    printf("%s : ",__func__);
    printf("Nucleo = %d\n",xPortGetCoreID());
    vTaskDelay(pdMS_TO_TICKS(100));
    vTaskDelete(NULL);
  }
}


/* Task 2 - Imprime valor na serial*/
void vTask2(void *pvParameters )
{
  
  (void)pvParameters;

  while(1)
  {
    printf("%s : ",__func__);
    printf("Nucleo = %d\n",xPortGetCoreID());
    vTaskDelay(pdMS_TO_TICKS(100));
    vTaskDelete(NULL);
  }
}
