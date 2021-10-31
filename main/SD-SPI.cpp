/* SD card and FAT filesystem example.
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

// This example uses SPI peripheral to communicate with SD card.

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "dirent.h"
#include "config.h"

#if CONFIG_IDF_TARGET_ESP32S2
#define SPI_DMA_CHAN    host.slot
#elif CONFIG_IDF_TARGET_ESP32C3
#define SPI_DMA_CHAN    SPI_DMA_CH_AUTO
#else
#define SPI_DMA_CHAN    1
#endif

namespace SD {
static const char *TAG = "SD";
static sdmmc_card_t *card;
static char filename[SD_BUFFERSIZE] = SD_MOUNT_POINT "/0.csv";
static FILE *f;

bool getNum(char *a, uint8_t* out, const char end){
    char buffer[SD_MAX_SEARCH_LEN] = {0};
    for(int i = 0; i < SD_MAX_SEARCH_LEN; i++){
        if(*(a+i) == end){
            *out = atoi(buffer);
            return 1;
        } else {
            buffer[i] = *(a+i);
        }
    }
    return 0;
}

void initializeFileHandle(){
    printf("Writing to %s\n", filename);
    f = fopen(filename, "a");
    if (f == NULL) {
        ESP_LOGE(TAG, "Failed to open file for writing");
    }
}

void write(float *x, const uint8_t len){
    f = fopen(filename, "a");
    for (int i = 0; i < len - 1; i++){
        fprintf(f, "%f,", x[i]);
    }
    fprintf(f, "%f\n", x[len - 1]);
    fclose(f);
}


bool generateFileName(){
    uint8_t logNum = 0, fileNum = 0;
    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(SD_MOUNT_POINT)) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            /* printf ("%s\n", ent->d_name); */
            if (getNum(ent->d_name, &fileNum, '.')){
                if (fileNum >= logNum){
                    logNum = fileNum + 1;
                }
            }
        }
    } else {
        return 0;
    }
    sprintf(filename, SD_MOUNT_POINT "/%i.csv", logNum);
    closedir(dir);
    return 1;
}

bool startSD(void){
    esp_err_t ret;
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    const char mount_point[] = SD_MOUNT_POINT;
    ESP_LOGI(TAG, "Initializing SD card");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.max_freq_khz = SD_MAX_FREQ_HZ;
    spi_bus_config_t bus_cfg = {
        .mosi_io_num = SD_MOSI_IO,
        .miso_io_num = SD_MISO_IO,
        .sclk_io_num = SD_CLK_IO,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .data4_io_num = 0,
        .data5_io_num = 0,
        .data6_io_num = 0,
        .data7_io_num = 0,
        .max_transfer_sz = 4000,
        .flags = 0,
        .intr_flags = 0,
    };
    ret = spi_bus_initialize((spi_host_device_t)host.slot, &bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize bus.");
        return 0;
    }

    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = (gpio_num_t) SD_CS_IO;
    slot_config.host_id = (spi_host_device_t) host.slot;

    ESP_LOGI(TAG, "Mounting filesystem");
    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(TAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(TAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return 0;
    }
    ESP_LOGI(TAG, "Filesystem mounted");

    sdmmc_card_print_info(stdout, card);

    if(!generateFileName()){
        return 0;
    }
    initializeFileHandle();
    return 1;
}


}
