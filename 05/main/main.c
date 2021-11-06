/*********************************************************
* Exemplo de passagem de parametro para Tasks na criação* 
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
#define LED1 2
#define LED2 5

/* Variáveis para Armazenar o handle das Tasks */
TaskHandle_t xTask1Handle, xTask2Handle, xTask3Handle;

/* Protótipo das Tasks*/
void vTask1(void *pvParameters ); 
void vTask2(void *pvParameters ); 

int valor = 100;

void app_main(void)
{
 
  xTaskCreate(
     vTask1                     /* função da task*/   
    ,  "Task1"                  /* Nome da Task */
    ,  configMINIMAL_STACK_SIZE +1024 /* Stack Size */
    ,  (void*)100                 /* parametro passado para a task*/
    ,  1                        /* Prioridade da task*/
    ,  &xTask1Handle          /* handle da task*/
    );    

  xTaskCreate(
     vTask1                     /* função da task*/   
    ,  "Task2"                  /* Nome da Task */
    ,  configMINIMAL_STACK_SIZE +1024 /* Stack Size */
    ,  (void*)500                 /* parametro passado para a task*/
    ,  1                        /* Prioridade da task*/
    ,  NULL         /* handle da task*/
    ); 

  xTaskCreate(
     vTask2                             /* função da task*/
    ,  "BLINK2"                          /* Nome da Task */
    ,  configMINIMAL_STACK_SIZE + 1024  /* Stack Size,*/
    ,  (void*)LED1                              /* parametro passado para a task*/
    ,  2                                /* Prioridade da task*/
    ,  &xTask2Handle                    /* handle da task*/
    );       

  xTaskCreate(
     vTask2                             /* função da task*/
    ,  "BLINK5"                          /* Nome da Task */
    ,  configMINIMAL_STACK_SIZE + 1024  /* Stack Size,*/
    ,  (void*)LED2                      /* parametro passado para a task*/
    ,  2                                /* Prioridade da task*/
    ,  &xTask3Handle                    /* handle da task*/
    );       
}


/* Task 1 - contador*/
void vTask1(void *pvParameters )
{
  int count = (int)pvParameters;

  while(1)
  {
    printf("Task %s: %d \n",__func__,count++);
    vTaskDelay(pdMS_TO_TICKS(500));    /* Delay de 1 segundos */
  }
}


/* Task 2 - Imprime valor na serial*/
void vTask2(void *pvParameters )
{
  
  int pin = (int)pvParameters; /*lê o parametro passado*/
  //gpio_reset_pin(pin);
    /* Set the GPIO as a push/pull output */
  gpio_set_direction(pin, GPIO_MODE_OUTPUT);

  while(1)
  {
    /* Blink off (output low) */
    printf("Turning off the LED %d\n",pin);
    gpio_set_level(pin, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    /* Blink on (output high) */
    printf("Turning on the LED %d\n",pin);
    gpio_set_level(pin, 1);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}
