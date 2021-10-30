/*********************************************************
* Exemplo Criação de Task
* Esse exemplo exibe como criar duas tasks com diferentes tamanhos de Stacks
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

/* Variáveis para Armazenar o handle da Task */
TaskHandle_t task1Handle = NULL;
TaskHandle_t task2Handle = NULL;

/* Protótipo das Tasks*/
void vTask1(void *pvParameters ); 
void vTask2(void *pvParameters ); 

void app_main(void)
{

  xTaskCreate(
     vTask1                     /* função da task*/   
    ,  "Task1"                  /* Nome da Task */
    ,  1024                     /* Stack Size */
    ,  NULL                     /* parametro passado para a task*/
    ,  1                        /* Prioridade da task*/
    ,  &task1Handle             /* handle da task*/
    );    

  xTaskCreate(
     vTask2                             /* função da task*/
    ,  "Task2"                          /* Nome da Task */
    ,  configMINIMAL_STACK_SIZE + 1024  /* Stack Size,*/
    ,  NULL                             /* parametro passado para a task*/
    ,  2                                /* Prioridade da task*/
    ,  &task2Handle                     /* handle da task*/
    );       

}

/* Task 1 - Blink LED */
void vTask1(void *pvParameters )
{
  (void) pvParameters;

    /* Set the GPIO as a push/pull output */
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

  while(1)
  {
    /* Blink off (output low) */
    printf("Turning off the LED\n");
    gpio_set_level(BLINK_GPIO, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    /* Blink on (output high) */
    printf("Turning on the LED\n");
    gpio_set_level(BLINK_GPIO, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

/* Task 2 - Imprime valor na serial*/
void vTask2(void *pvParameters )
{
  (void) pvParameters;  
  int count = 0;        /*variável auxiliar */

  while(1)
  {
    printf("Task 2: %d\n",count++);
    vTaskDelay(pdMS_TO_TICKS(1000));    /* Delay de 1 segundos */
  }
}