// FreeRTOS Mutex Wrapper
#include "mutex.h"

#include "esp_log.h"
#include "freertos/FreeRTOS.h"

typedef struct MutexRecord
{
    SemaphoreHandle_t xSemaphore;
    StaticSemaphore_t xMutexBuffer;
} MutexRecord_t;

static MutexRecord_t _Records[kNumMutexKeys];
static const char* TAG = "mutex";

void Mutex_Init(void)
{
    for (MutexKey_t eKey = kMutexKey_FirstKey; eKey < kNumMutexKeys; eKey++)
    {
        _Records[eKey].xSemaphore = xSemaphoreCreateMutexStatic( &_Records[eKey].xMutexBuffer );

        if (_Records[eKey].xSemaphore == NULL)
        {
            ESP_LOGE(TAG, "Failed to create semaphore for key %d", eKey);
        }
    }
}

MutexResult_t Mutex_Take(const MutexKey_t eKey)
{
    if ((unsigned)eKey >= kNumMutexKeys)
    {
        ESP_LOGE(TAG, "Key is invalid %d", eKey);
        return kMutexResult_Err_InvalidKey;
    }

    if (_Records[eKey].xSemaphore == NULL)
    {
        return kMutexResult_Err_InvalidDataPtr;
    }

    if (xSemaphoreTake(_Records[eKey].xSemaphore, portMAX_DELAY) == pdTRUE)
    {
        return kMutexResult_Ok;
    }
    else
    {
        ESP_LOGE(TAG, "Time out");
        return kMutexResult_Err_Timeout;
    }
}

MutexResult_t Mutex_Give(const MutexKey_t eKey)
{
    if ((unsigned)eKey >= kNumMutexKeys)
    {
        ESP_LOGE(TAG, "Unknown key");
        return kMutexResult_Err_InvalidKey;
    }

    if (_Records[eKey].xSemaphore == NULL)
    {
        ESP_LOGE(TAG, "Null data pointer in give");
        return kMutexResult_Err_InvalidDataPtr;
    }

    if (xSemaphoreGive(_Records[eKey].xSemaphore) == pdTRUE)
    {
        return kMutexResult_Ok;
    }
    else
    {
        ESP_LOGE(TAG, "Resource wasn't acquired");
        return kMutexResult_Err_ResourceWasNotAcquired;
    }
}
