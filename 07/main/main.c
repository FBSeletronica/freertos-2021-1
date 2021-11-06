/*********************************************************
* Exemplo que demonstra como criar uma fila, adicionar e ler
* dados da fila através de duas tasks
* Por: Fábio Souza
*********************************************************/

/*inclusão de Bibliotecas*/
#include <stdio.h>
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_log.h"


/*inclusão das Biblioteca do FreeRTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

#define LED 2

QueueHandle_t xFila; /*cria objeto fila */

/* Variáveis para armazenar o handle da Task */
TaskHandle_t xTask1Handle;
TaskHandle_t xTask2Handle;

/* Protótipo das Tasks*/
void vTask1(void *pvParameters ); 
void vTask2(void *pvParameters ); 


void app_main(void)
{
  BaseType_t xReturned;

  xFila = xQueueCreate(5, sizeof(int));  //Cria a fila com 5 slots com tamanho de um int
  
  if( xFila == NULL )
  {
    ESP_LOGE( "xFila", "Não foi possivel criar a fila"); 
    while(1);
  }
  /* Cria a vTask1 */
  xReturned = xTaskCreate(vTask1,  "Task1",  configMINIMAL_STACK_SIZE + 1024,  NULL ,  1,  &xTask1Handle ); 

  if( xReturned == pdFAIL)
  {
      ESP_LOGE( "Task1", "Não foi possivel criar a Task 1"); 
      while(1);
  }

    /* Cria a vTask2 */
  xReturned = xTaskCreate(vTask2,  "Task2",  configMINIMAL_STACK_SIZE + 1024,  NULL ,  1,  &xTask2Handle ); 
  if( xReturned == pdFAIL)
  {
      ESP_LOGE( "Task2", "Não foi possivel criar a Task 2"); 
      while(1);
  }
}


/* Task1 */
void vTask1(void *pvParameters )
{
  (void) pvParameters;
  int count = 0;
  
  while(1)
  {

    if(count <10){
        xQueueSend(xFila, &count, portMAX_DELAY) ;/* envia valor atual de count para fila*/
        count++;    /*incrementa contador*/
    }
    else
    {
      count = 0; /* reinicia contador*/
      vTaskDelay(pdMS_TO_TICKS(5000)); /*aguarda 5 segundos */
    }
    vTaskDelay(pdMS_TO_TICKS(500)); /* Aguarda 500 ms antes de uma nova iteração*/
  }
}

/* Task2  */
void vTask2(void *pvParameters )
{
  (void) pvParameters;  /* Apenas para o Compilador não retornar warnings */
  int valor_recebido = 0;
  uint8_t x = 0;

  gpio_set_direction(LED, GPIO_MODE_OUTPUT);

  while(1)
  {
      if(xQueueReceive(xFila, &valor_recebido, pdMS_TO_TICKS(1000)) == pdTRUE) //verifica se há valor na fila para ser lido. Espera 1 segundo
      {
          ESP_LOGI("Queue", "Item recebido: %u", valor_recebido);//Mostra o valor recebido na tela 
      }
      else
      {
          ESP_LOGE("Queue", "TIMEOUT");
      }
      gpio_set_level(LED ,x^=1);        /*inverte estado do LED*/
  }
}
