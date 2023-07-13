#include <stdio.h>
#include <iostream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

using namespace std;

static const char *TAG = "MyTask";

TaskHandle_t mainTaskHandle;
nvs_handle_t nvsHandle;

// Fonction de la tâche
void task(void *pvParameters)
{
    // Tâche en cours d'exécution
    printf("Tâche en cours...\n");

    // Simuler un travail
    vTaskDelay(pdMS_TO_TICKS(5000));

    // Fin de la tâche
    printf("Tâche terminée.\n");

    // Supprimer la tâche
    vTaskDelete(NULL);
   //Faut rien mettre après ici, ce ne sera pas exécuté
}


//2023-07-12 19:37:40
TaskHandle_t jobHandle;
void mainJob(void *pvParameters)
{
    // Tâche en cours d'exécution
    printf("mainJob started\n");

    for(int i=0; i<9; i++)
    {
        ESP_LOGI(TAG,">%d",i);
        // Simuler un travail
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    // Fin de la tâche
    printf("Job ended.\n");

    // Send notification to waiting task
    xTaskNotifyGive(mainTaskHandle);

    // Supprimer la tâche
    vTaskDelete(NULL);  
}//mainJob





////2023-07-13 00:23:40 - Usage : 
// nvs_flash_init();
// nvs_open("storage", NVS_READWRITE, &nvsHandle);

// //2023-07-13 00:22:14 - Write
// nvs_write_string(nvsHandle,"reset_state","OFF");

// //2023-07-13 00:22:20 - read
// string state = nvs_read_string(nvsHandle,"reset_state"); 
// cout<<"Read from func : "<<state<<endl;




//2023-07-12 23:24:59 - Write to NVS
void nvs_write_string(nvs_handle_t h,string key, string write_string)
{
   nvs_set_str(h, key.c_str(), write_string.c_str() );
   nvs_commit(h);
}

//2023-07-12 23:24:51 - Read From NVS
string nvs_read_string(nvs_handle_t h, string key)
{
   size_t required_size = 0;
   nvs_get_str(h,key.c_str(), NULL, &required_size);
   cout<<"size to read : "<< required_size<<endl;
   printf("req size = %d\n", required_size);
   if( required_size<=0) return "";
   char *buff = (char*) malloc(required_size);
   nvs_get_str(h, key.c_str(), buff, &required_size);
   return buff;
}

  





extern "C" void app_main(void)
{    
    // Initialize logging
    esp_log_level_set(TAG, ESP_LOG_INFO);  // Set log level to INFO
  mainTaskHandle = xTaskGetCurrentTaskHandle();


nvs_flash_init();
nvs_open("storage", NVS_READWRITE, &nvsHandle);

//2023-07-13 00:22:14 - Write
nvs_write_string(nvsHandle,"reset_state","OFF");

//2023-07-13 00:22:20 - read
string state = nvs_read_string(nvsHandle,"reset_state"); 
cout<<"Read from func : "<<state<<endl;






      // Créer la tâche
//     xTaskCreate(task, "myTask", 2048, NULL, 5, NULL);
//     xTaskCreate(mainJob, "mainJob", 2048, NULL, 5, &jobHandle);










//     // Attendre la fin de la tâche
//    puts("Wait for job task to finish\n");
//    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
//     //vTaskDelete(NULL);    
//     puts("End main!\n");
}
