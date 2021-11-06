/*********************************************************
* Exemplo para demonstrar o uso de MUTEX para acesso a recursos
* e a inversão de prioridade
* Por: Fábio Souza
*********************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/semphr.h"

#include "driver/gpio.h"
#include "esp_log.h"

/*definição dos pinos*/
#define LED 2       /*LED ligado ao GPIO2*/

/*handle do Semaforo*/
SemaphoreHandle_t xMutex;

/* Variáveis para armazenar o handle da Task */
TaskHandle_t xTaskTrataISRBTHandle;

/* Protótipo das Tasks*/
void vTask1( void *pvParameter );
void vTask2( void *pvParameters );

void app_main (void)
{   
  /* Cria o Mutex */
  xMutex = xSemaphoreCreateMutex();
  
  xTaskCreate(vTask1, "vTask1",2048,NULL,1, NULL);
  xTaskCreate(vTask2, "vTask2",2048,NULL,4,NULL);
}


void vTask1( void * pvParameters )
{
  (void) pvParameters;
  while(1)
  { 
    
    xSemaphoreTake( xMutex, portMAX_DELAY );
    ESP_LOGI("Task 1 ","vTask1 pega o semáforo");
    ESP_LOGI("Task 1 ","Vtask1 em execução por 2 segundos");
    for(int i = 0;i<20000;i++){
        ets_delay_us(100);
    }
    ESP_LOGI("Task 1 ","vTask1 devolve o semáforo");
    xSemaphoreGive( xMutex );
  }
}

void vTask2( void * pvParameters )
{
  (void) pvParameters;

  while(1)
  {
    
    xSemaphoreTake( xMutex, portMAX_DELAY );
    ESP_LOGI("Task 2 ","vTask2 pega o semáforo");
    ESP_LOGI("Task 2 ","Vtask2 em execução");
    xSemaphoreGive( xMutex );
    ESP_LOGI("Task 2 ","vTask2 devolve o semáforo \r\n");
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}
