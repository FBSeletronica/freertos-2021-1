/*********************************************************
* Exemplo que demonstra como enviar valores para uma fila 
* a partir de uma ISR
* Por: Fábio Souza
*********************************************************/

/*inclusão de Bibliotecas*/
#include <stdio.h>
#include "driver/gpio.h"
#include "sdkconfig.h"

/*inclusão das Biblioteca do FreeRTOS*/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"

/*log*/
#include "esp_system.h"
#include "esp_log.h"

/*Definições*/
#define DEBUG 1
#define LED 2       /*LED ligado ao GPIO2*/
#define BT 12       /* Botão ligado ao pino 12*/

QueueHandle_t xFila; /*cria objeto fila */

/* Variáveis para armazenar o handle da Task */
TaskHandle_t xTask1Handle;

/* Protótipo das Tasks*/
void vTask1(void *pvParameters ); 


/* Funções auxiliares */
void vInitHW(void);

static void IRAM_ATTR gpio_isr_handler(void* arg) {
  static int val;
  BaseType_t xHigherPriorityTaskWoken;
  xHigherPriorityTaskWoken = pdTRUE;
  
  val++;
  xQueueSendFromISR(xFila, &val, &xHigherPriorityTaskWoken);
}

/* Função Init Harware */
void vInitHW(void)
{
    
    gpio_pad_select_gpio(LED );
    gpio_set_direction(LED , GPIO_MODE_OUTPUT);
    gpio_set_direction(BT,GPIO_MODE_INPUT);
    gpio_set_pull_mode( BT, GPIO_PULLUP_ONLY); 
    gpio_set_intr_type(BT, GPIO_INTR_NEGEDGE);
    gpio_install_isr_service(0);
    gpio_isr_handler_add(BT, gpio_isr_handler, (void*) BT);

}

/* Função app_main*/
void app_main()
{ 
  uint8_t x = 0;
    
  vInitHW();      /* Configuração do Hardware */

  xFila = xQueueCreate(5, sizeof(int));//Cria a fila com 5 slots com tamanho de um int
  
  if( xFila == NULL )
  {
    ESP_LOGE( "xFila", "Não foi possivel criar a fila"); 
    while(1);
  }
  /* Cria a vTask1 */
  xTaskCreate(vTask1,  "Task1",  configMINIMAL_STACK_SIZE + 1024,  NULL,  1,  &xTask1Handle);

  while(1) {

    gpio_set_level(LED , x^=1);        /*inverte estado do LED*/
    vTaskDelay(pdMS_TO_TICKS(500));    /* Delay de 500 segundos */
  }
}


/* Task1 */
void vTask1(void *pvParameters )
{
  (void) pvParameters;
  int valor_recebido = 0;

  while(1)
  {
      xQueueReceive(xFila, &valor_recebido, portMAX_DELAY); /* Aguarda chegar valor na Fila */ 
      ESP_LOGI("Queue", "Item recebido: %u", valor_recebido);//Mostra o valor recebido na tela 
  }
}
