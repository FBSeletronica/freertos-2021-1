/*********************************************************
* Exemplo que demonstra como enviar valores para uma fila 
* a partir de uma ISR
* Por: Fábio Souza
*********************************************************/

/*inclusão de Bibliotecas*/
#include <stdio.h>
#include "driver/gpio.h"
#include <driver/adc.h>
#include "sdkconfig.h"

/*inclusão das Biblioteca do FreeRTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/*log*/
#include "esp_system.h"
#include "esp_log.h"


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

  adc1_config_width(ADC_WIDTH_BIT_12);//Configura a resolucao
  adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);//Configura a atenuacao

  while(1)
  {

    uint32_t adc = 0;
    for (int i = 0; i < 100; i++)
    {
        adc += adc1_get_raw(ADC1_CHANNEL_0);//Obtem o valor RAW do ADC
        ets_delay_us(30);
    }
    adc /= 100;

    ESP_LOGI("ADC", "Valor enviado: %u", adc);//Mostra a leitura calibrada no Serial Monitor
    xQueueSend(xFila, &adc, portMAX_DELAY) ;/* envia valor atual de count para fila*/

    vTaskDelay(pdMS_TO_TICKS(1000));//Delay 1seg

  }
}

/* Task2  */
void vTask2(void *pvParameters )
{
  (void) pvParameters;  /* Apenas para o Compilador não retornar warnings */
  int valor_recebido = 0;

     /* Set the GPIO as a push/pull output */
  gpio_set_direction(LED, GPIO_MODE_OUTPUT);

  while(1)
  {
      if(xQueueReceive(xFila, &valor_recebido, portMAX_DELAY)) //verifica se há valor na fila para ser lido. Espera 1 segundo
      {
          ESP_LOGI("Queue", "Item recebido: %u", valor_recebido);//Mostra o valor recebido na tela 

          if(valor_recebido> 3000){
            gpio_set_level(LED ,1);        /*Liga LED*/
            ESP_LOGI("LED", "ACESO");
          }
          else{
            gpio_set_level(LED ,0);        /*apaga LED*/
            ESP_LOGI("LED", "APAGADO");
          }
          
      }
      else
      {
          ESP_LOGE("Queue", "TIMEOUT");
      }
      
  }
}
