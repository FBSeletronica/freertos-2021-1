
/*********************************************************
* Exemplo para Deletar uma Task
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
TaskHandle_t xTask1Handle = NULL;
TaskHandle_t xTask2Handle = NULL;

/* Protótipo das Tasks*/
void vTask1(void *pvParameters ); 
void vTask2(void *pvParameters ); 

void app_main(void)
{
  gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
  gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);

  xTaskCreate(
     vTask1                     /* função da task*/   
    ,  "Task1"                  /* Nome da Task */
    ,  configMINIMAL_STACK_SIZE /* Stack Size */
    ,  NULL                     /* parametro passado para a task*/
    ,  1                        /* Prioridade da task*/
    ,  &xTask1Handle             /* handle da task*/
    );    

  xTaskCreate(
     vTask2                             /* função da task*/
    ,  "Task2"                          /* Nome da Task */
    ,  configMINIMAL_STACK_SIZE + 1024  /* Stack Size,*/
    ,  NULL                             /* parametro passado para a task*/
    ,  2                                /* Prioridade da task*/
    ,  &xTask2Handle                    /* handle da task*/
    );       
}

/* Task 1 - Blink LED */
void vTask1(void *pvParameters )
{
  (void) pvParameters;

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

  (void) pvParameters;  /* Apenas para o Compilador não retornar warnings */
  uint8_t count = 0;      /* declara e inicializa variável contador com 0*/

  while(1)
  {
    /* Imprime valor do contador na Serial*/
    printf("Task 2: %d\n",count++);

    /*Verifica valor do contador*/
    if(count>=10) /*se maior ou igual a 10*/
    {
        if(xTask1Handle != NULL)    /*verifica se o handle da task é diferente de NULL*/
        {
          printf("Task 1 Deletada!\n");       /*Mensagem na serial*/
          vTaskDelete(xTask1Handle);          /*Deleta task 1*/  
          gpio_set_level(BLINK_GPIO, 0);      /*Garante que LED ficará apagado*/
          xTask1Handle = NULL;                /*atualiza valor da variável para NULL*/
        }
    }
    vTaskDelay(pdMS_TO_TICKS(500));           /* Delay de 0.5 segundos */
  }
}


