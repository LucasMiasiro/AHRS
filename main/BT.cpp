#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"
#include "config.h"
 
#define SPP_TAG             "INS"
#define SPP_SERVER_NAME     "INS_SERVER"
#define BT_INIT_MSG         "Welcome\r"
#define BT_RECEIVED_MSG     "Received: "
#define BT_DEVICE_NAME      "Embedded Navigation System"

namespace serialBTLogger{

 
static bool bWriteAfterOpenEvt = false;
static bool bWriteAfterWriteEvt = false;
static bool bWriteAfterSvrOpenEvt = true;
static bool bWriteAfterDataReceived = true;
 
static const esp_spp_mode_t esp_spp_mode = ESP_SPP_MODE_CB;
 
static const esp_spp_sec_t sec_mask = ESP_SPP_SEC_AUTHENTICATE;
static const esp_spp_role_t role_slave = ESP_SPP_ROLE_SLAVE;
 
 
static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
    //Used in app_main() to setup the BT configuration in the ESP32 and used for communication with device

    switch (event) {
    case ESP_SPP_INIT_EVT:
        esp_bt_dev_set_device_name(BT_DEVICE_NAME);
        esp_bt_gap_set_scan_mode(ESP_BT_CONNECTABLE, ESP_BT_GENERAL_DISCOVERABLE);
        esp_spp_start_srv(sec_mask,role_slave, 0, SPP_SERVER_NAME);
        break;
    case ESP_SPP_DISCOVERY_COMP_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_DISCOVERY_COMP_EVT");
        break;
    case ESP_SPP_OPEN_EVT:
        //When SPP Client connection open, the event comes

        ESP_LOGI(SPP_TAG, "ESP_SPP_OPEN_EVT");
       
        ESP_LOGI(SPP_TAG, "bWriteAfterOpenEvt: %d: ", bWriteAfterOpenEvt);

        if (bWriteAfterOpenEvt){
            ESP_LOGI(SPP_TAG, "bWriteAfterOpenEvt = true");
        }
        else{
            ESP_LOGI(SPP_TAG, "bWriteAfterOpenEvt = false");
        }

        break;
    case ESP_SPP_CLOSE_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CLOSE_EVT");
        break;
    case ESP_SPP_START_EVT:                                         //Short before connection is established
        ESP_LOGI(SPP_TAG, "ESP_SPP_START_EVT");
        break;
    case ESP_SPP_CL_INIT_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CL_INIT_EVT");
        break;
    case ESP_SPP_DATA_IND_EVT:                                      //When SPP connection received data, the event comes, only for ESP_SPP_MODE_CB
        ESP_LOGI(SPP_TAG, "ESP_SPP_DATA_IND_EVT len=%d handle=%d", param->data_ind.len, param->data_ind.handle);
 
        //ESP_LOG_BUFFER_HEX(tag, buffer, buff_len)
        //tag: description tag
        //buffer: Pointer to the buffer array
        //buff_len: length of buffer in bytes
 
        esp_log_buffer_hex("Received HEX Data",param->data_ind.data,param->data_ind.len);
        esp_log_buffer_char("Received String Data",param->data_ind.data,param->data_ind.len);
 
        if (bWriteAfterDataReceived){
            const char c[] = BT_RECEIVED_MSG;
            esp_spp_write(param->srv_open.handle, sizeof(c)/sizeof(c[0]), (uint8_t*) c);
            esp_spp_write(param->write.handle, param->data_ind.len, param->data_ind.data);
        }
 
        break;
    case ESP_SPP_CONG_EVT:
        ESP_LOGI(SPP_TAG, "ESP_SPP_CONG_EVT");
        break;
    case ESP_SPP_WRITE_EVT:
        //When SPP write operation completes, the event comes, only for ESP_SPP_MODE_CB
 
        ESP_LOGI(SPP_TAG, "ESP_SPP_WRITE_EVT len=%d cong=%d", param->write.len , param->write.cong);
 
        ESP_LOGI(SPP_TAG, "if param->write.cong ...");
        if (param->write.cong == 0) {
            ESP_LOGI(SPP_TAG, "param->write.cong == 0");
            if (bWriteAfterWriteEvt){
                ESP_LOGI(SPP_TAG, "bWriteAfterWriteEvt = true");
            }
            else{
                ESP_LOGI(SPP_TAG, "bWriteAfterWriteEvt = false");
            }
        }
        else {
            ESP_LOGI(SPP_TAG, "param->write.cong <> 0");
        }
        //Code copied from Initiator - End
 
        break;
    case ESP_SPP_SRV_OPEN_EVT:                                      //After connection is established, short before data is received
        //When SPP Server connection open, the event comes
        //In use in Acceptor
        ESP_LOGI(SPP_TAG, "ESP_SPP_SRV_OPEN_EVT");
 
        ESP_LOGI(SPP_TAG, "bWriteAfterOpenEvt: %d: ", bWriteAfterSvrOpenEvt);
        //Added code from Initiator - Start
        if (bWriteAfterSvrOpenEvt){
            ESP_LOGI(SPP_TAG, "bWriteAfterSvrOpenEvt = true");

            const char c[] = BT_INIT_MSG;
            esp_spp_write(param->srv_open.handle, sizeof(c)/sizeof(c[0]), (uint8_t*) c);
 
        }
        else{
            ESP_LOGI(SPP_TAG, "bWriteAfterSvrOpenEvt = false");
        }
 
        break;
    default:
        break;
    }
    ESP_LOGI(SPP_TAG, "End of: static void esp_spp_cb(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)");
}
 
void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param){
    //Used in app_main() to setup the BT configuration in the ESP32
    ESP_LOGI(SPP_TAG, "Start of: void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)");
    switch (event) {
    case ESP_BT_GAP_AUTH_CMPL_EVT:{
        if (param->auth_cmpl.stat == ESP_BT_STATUS_SUCCESS) {
            ESP_LOGI(SPP_TAG, "authentication success: %s", param->auth_cmpl.device_name);
            esp_log_buffer_hex(SPP_TAG, param->auth_cmpl.bda, ESP_BD_ADDR_LEN);
        } else {
            ESP_LOGE(SPP_TAG, "authentication failed, status:%d", param->auth_cmpl.stat);
        }
        break;
    }
    case ESP_BT_GAP_PIN_REQ_EVT:{
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_PIN_REQ_EVT min_16_digit:%d", param->pin_req.min_16_digit);
        if (param->pin_req.min_16_digit) {
            ESP_LOGI(SPP_TAG, "Input pin code: 0000 0000 0000 0000");
            esp_bt_pin_code_t pin_code = {0};
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 16, pin_code);
        } else {
            ESP_LOGI(SPP_TAG, "Input pin code: 1234");
            esp_bt_pin_code_t pin_code;
            pin_code[0] = '1';
            pin_code[1] = '2';
            pin_code[2] = '3';
            pin_code[3] = '4';
            esp_bt_gap_pin_reply(param->pin_req.bda, true, 4, pin_code);
        }
        break;
    }
 
    //Must be set in sdkconfig.h: CONFIG_BT_SSP_ENABLED == true
    //This enables the Secure Simple Pairing.
    case ESP_BT_GAP_CFM_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_CFM_REQ_EVT Please compare the numeric value: %d", param->cfm_req.num_val);
        esp_bt_gap_ssp_confirm_reply(param->cfm_req.bda, true);
        break;
    case ESP_BT_GAP_KEY_NOTIF_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_NOTIF_EVT passkey:%d", param->key_notif.passkey);
        break;
    case ESP_BT_GAP_KEY_REQ_EVT:
        ESP_LOGI(SPP_TAG, "ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!");
        break;
 
 
    default: {
        ESP_LOGI(SPP_TAG, "event: %d", event);
        //  0 ESP_BT_GAP_DISC_RES_EVT
        //  1 ESP_BT_GAP_DISC_STATE_CHANGED_EVT
        //  2 ESP_BT_GAP_RMT_SRVCS_EVT
        //  3 ESP_BT_GAP_RMT_SRVC_REC_EVT
        //  4 ESP_BT_GAP_AUTH_CMPL_EVT
        //  5 ESP_BT_GAP_PIN_REQ_EVT
        //  6 ESP_BT_GAP_CFM_REQ_EVT
        //  7 ESP_BT_GAP_KEY_NOTIF_EVT
        //  8 ESP_BT_GAP_KEY_REQ_EVT
        //  9 ESP_BT_GAP_READ_RSSI_DELTA_EVT
        // 10 ESP_BT_GAP_CONFIG_EIR_DATA_EVT
        // 11 ESP_BT_GAP_EVT_MAX
        break;
    }
    }
    return;
    ESP_LOGI(SPP_TAG, "End of: void esp_bt_gap_cb(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param)");
}
 
void startBT(){
 
    //Non-volatile storage (NVS) library is designed to store key-value pairs in flash.
    esp_err_t ret = nvs_flash_init();   //Initialize the default NVS partition.
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
 
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_BLE));        //release the controller memory as per the mode
 
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ESP_LOGI(SPP_TAG, "Call esp_bt_controller_init(&bt_cfg)");
    if ((ret = esp_bt_controller_init(&bt_cfg)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    else{
        ESP_LOGI(SPP_TAG, "Initialize controller ok");
    }
   
    ESP_LOGI(SPP_TAG, "Call esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)");
    if ((ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable controller failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    else{
        ESP_LOGI(SPP_TAG, "Enable controller ok");
    }
   
    ESP_LOGI(SPP_TAG, "Call esp_bluedroid_init()");
    if ((ret = esp_bluedroid_init()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s initialize bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    else{
        ESP_LOGI(SPP_TAG, "Initialize bluedroid ok");
    }
   
    ESP_LOGI(SPP_TAG, "Call esp_bluedroid_enable()");
    if ((ret = esp_bluedroid_enable()) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s enable bluedroid failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    else{
        ESP_LOGI(SPP_TAG, "Enable bluedroid ok");
    }
   
    ESP_LOGI(SPP_TAG, "Call esp_bt_gap_register_callback(esp_bt_gap_cb)");
    if ((ret = esp_bt_gap_register_callback(esp_bt_gap_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s gap register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    else{
        ESP_LOGI(SPP_TAG, "Gap register ok");
    }
   
    ESP_LOGI(SPP_TAG, "Call esp_spp_register_callback(esp_spp_cb)");
    if ((ret = esp_spp_register_callback(esp_spp_cb)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp register failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    else{
        ESP_LOGI(SPP_TAG, "spp register ok");
    }
   
    ESP_LOGI(SPP_TAG, "Call esp_spp_init(esp_spp_mode)");
    if ((ret = esp_spp_init(esp_spp_mode)) != ESP_OK) {
        ESP_LOGE(SPP_TAG, "%s spp init failed: %s\n", __func__, esp_err_to_name(ret));
        return;
    }
    else{
        ESP_LOGI(SPP_TAG, "spp init ok");
    }
   
    //Must be set in sdkconfig.h: CONFIG_BT_SSP_ENABLED == true
    //This enables the Secure Simple Pairing.
    ESP_LOGI(SPP_TAG, "CONFIG_BT_SSP_ENABLED == true");
    /* Set default parameters for Secure Simple Pairing */
    ESP_LOGI(SPP_TAG, "Set default parameters for Secure Simple Pairing");
    esp_bt_sp_param_t param_type = ESP_BT_SP_IOCAP_MODE;
    esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_IO;
    esp_bt_gap_set_security_param(param_type, &iocap, sizeof(uint8_t));
 
    /*
     * Set default parameters for Legacy Pairing
     * Use variable pin, input pin code when pairing
     */
    ESP_LOGI(SPP_TAG, "Set default parameters");
    esp_bt_pin_type_t pin_type = ESP_BT_PIN_TYPE_VARIABLE;
    esp_bt_pin_code_t pin_code;
    esp_bt_gap_set_pin(pin_type, 0, pin_code);
}

};
