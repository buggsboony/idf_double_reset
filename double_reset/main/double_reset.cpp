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
TickType_t tickCount;

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
    ESP_LOGI(TAG,"mainJob STARTED\n");

    for(int i=0; i<20; i++)
    {
        ESP_LOGI(TAG,">%d",i);
        // Simuler un travail
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
    // Fin de la tâche
    ESP_LOGI(TAG,"Job ended.\n");

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
// string state = nvs_read_string(nvsHandle,"reset_state","DEFAULT_VALUE"); 
// cout<<"Read from func : "<<state<<endl;

//2023-07-12 23:24:59 - Write to NVS
void nvs_write_string(nvs_handle_t h,string key, string write_string)
{
   nvs_set_str(h, key.c_str(), write_string.c_str() );
   nvs_commit(h);
}

//2023-07-12 23:24:51 - Read From NVS
string nvs_read_string(nvs_handle_t h, string key, string defaultValue="")
{
   size_t required_size = 0;
   nvs_get_str(h,key.c_str(), NULL, &required_size);
//    cout<<"size to read : "<< required_size<<endl;
//    printf("req size = %d\n", required_size);
   if( required_size<=0) return defaultValue; //IN Case could not read
   char *buff = (char*) malloc(required_size);
   nvs_get_str(h, key.c_str(), buff, &required_size);
   return buff;
}

//Your configuration program (Simulation
TaskHandle_t configTaskHandle;
void configTask(void *pvParameters)
{
    cout<<"ENTER CONFIG MODE HERE"<<endl;
    vTaskDelay(pdMS_TO_TICKS(55100) );
    cout<<"exit config mode here"<<endl;
    // Send notification to waiting task
    xTaskNotifyGive(mainTaskHandle);
    //delete task
    vTaskDelete(NULL);  
}

//2023-07-14 09:21:29 - Wait a bit then lock state in NVS
const int WAIT_LOCK_MS = 10100; //10 secs
void waitAndLockTask(void *pvParameters)
{
    ESP_LOGI(TAG,"Inside waitAndLockTask() %d sec \n",WAIT_LOCK_MS);
    vTaskDelay( WAIT_LOCK_MS / portTICK_PERIOD_MS);
    ESP_LOGI(TAG,"Exit waitAndLockTask() %d \n",WAIT_LOCK_MS);
    //2023-07-14 09:17:57 - now lock !
    nvs_write_string(nvsHandle,"reset_state","JOB");
    //delete task
    vTaskDelete(NULL);  
}


extern "C" void app_main(void)
{    
    // Initialize logging
    esp_log_level_set(TAG, ESP_LOG_INFO);  // Set log level to INFO
  mainTaskHandle = xTaskGetCurrentTaskHandle();


nvs_flash_init();
nvs_open("storage", NVS_READWRITE, &nvsHandle);


//RunTask to Wait then Write Lock
xTaskCreate(waitAndLockTask, "waitAndLockTask", 2048, NULL, 5, NULL);

//2023-07-14 09:12:15 - read reset_state
string resetStateAtStart = nvs_read_string(nvsHandle,"reset_state","JOB");

cout<<xTaskGetTickCount()<<" => resetStateAtStart : "<<resetStateAtStart<<endl;

//2023-07-14 09:17:57 - write unlocked for now (config mode for a while)
nvs_write_string(nvsHandle,"reset_state","CONFIG");

if( resetStateAtStart == "CONFIG")
{
    //2023-07-14 09:25:09 - enter config mode (task)
    xTaskCreate(configTask, "configTask", 2048, NULL, 5, &configTaskHandle);
    nvs_write_string(nvsHandle,"reset_state","JOB"); //Set to locked state (run job state)
    //BlinkBlue //config mode
    cout<<"Wait till manual restart or config end!!!"<<endl;
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    cout<<"Done Waiting config, let's exit"<<endl;
}else// resetStateAtStart is "LOCKED" (JOB)
{
    //Start main job (Blocking)
    xTaskCreate(mainJob, "mainJob", 2048, NULL, 5, &jobHandle);
    cout<<"Wait main job to end !!!"<<endl;
    xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
    cout<<"Job ended, let's finish..."<<endl;
}




 puts("End main !!!\n");
}
