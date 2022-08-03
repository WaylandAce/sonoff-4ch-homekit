#include <stdio.h>
#include <espressif/esp_wifi.h>
#include <espressif/esp_sta.h>
#include <espressif/esp_common.h>
#include <esp/uart.h>
#include <esp8266.h>
#include <FreeRTOS.h>
#include <task.h>

#include <homekit/homekit.h>
#include <homekit/characteristics.h>
#include "wifi.h"
#include "button.h"

// The GPIO pin that is connected to the relay 1 on the Sonoff Device
// Both IS_SONOFF_MINIR2 and BUTTON2_GPIO
const int RELAY1_GPIO = 12;
#ifdef IS_SONOFF_4CH
const int RELAY2_GPIO = 05;
const int RELAY3_GPIO = 04;
const int RELAY4_GPIO = 15;
#endif
// The GPIO pin that is connected to the blue LED on the Sonoff Device
const int LED1_GPIO = 13;

// Both IS_SONOFF_MINIR2 and BUTTON2_GPIO
#define BUTTON1_GPIO 0

#ifdef IS_SONOFF_MINIR2
#define SWITCH1_GPIO 4
#endif

#ifdef IS_SONOFF_4CH
#define BUTTON2_GPIO 9
#define BUTTON3_GPIO 10
#define BUTTON4_GPIO 14
#endif

#define VERSION "0.1.6"

void switch_on_callback1(homekit_characteristic_t *_ch, homekit_value_t on, void *context);

#ifdef IS_SONOFF_4CH
void switch_on_callback2(homekit_characteristic_t *_ch, homekit_value_t on, void *context);
void switch_on_callback3(homekit_characteristic_t *_ch, homekit_value_t on, void *context);
void switch_on_callback4(homekit_characteristic_t *_ch, homekit_value_t on, void *context);
#endif

void button_callback(uint8_t gpio, button_event_t event);

void relay1_write(bool on) {
    gpio_write(RELAY1_GPIO, on ? 1 : 0);
}

#ifdef IS_SONOFF_4CH
void relay2_write(bool on) {
    gpio_write(RELAY2_GPIO, on ? 1 : 0);
}

void relay3_write(bool on) {
    gpio_write(RELAY3_GPIO, on ? 1 : 0);
}

void relay4_write(bool on) {
    gpio_write(RELAY4_GPIO, on ? 1 : 0);
}
#endif

void led_write(bool on) {
    gpio_write(LED1_GPIO, on ? 0 : 1);
}

void reset_configuration_task() {
    //Flash the LED first before we start the reset
    for (int i=0; i<3; i++) {
        led_write(true);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        led_write(false);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    printf("Resetting Wifi Config\n");

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Resetting HomeKit Config\n");

    homekit_server_reset();

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    printf("Restarting\n");

    sdk_system_restart();

    vTaskDelete(NULL);
}

void reset_configuration() {
    printf("Resetting Sonoff configuration\n");
    xTaskCreate(reset_configuration_task, "Reset configuration", 256, NULL, 2, NULL);
}

homekit_characteristic_t switch_on1 = HOMEKIT_CHARACTERISTIC_(
    ON, false, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(switch_on_callback1)
);

#ifdef IS_SONOFF_4CH
homekit_characteristic_t switch_on2 = HOMEKIT_CHARACTERISTIC_(
    ON, false, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(switch_on_callback2)
);

homekit_characteristic_t switch_on3 = HOMEKIT_CHARACTERISTIC_(
    ON, false, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(switch_on_callback3)
);

homekit_characteristic_t switch_on4 = HOMEKIT_CHARACTERISTIC_(
    ON, false, .callback=HOMEKIT_CHARACTERISTIC_CALLBACK(switch_on_callback4)
);
#endif

void gpio_init() {
    gpio_enable(LED1_GPIO, GPIO_OUTPUT);
    led_write(false);

    gpio_enable(RELAY1_GPIO, GPIO_OUTPUT);
    gpio_enable(BUTTON1_GPIO, GPIO_INPUT);
    relay1_write(switch_on1.value.bool_value);

#ifdef IS_SONOFF_4CH
    gpio_enable(RELAY2_GPIO, GPIO_OUTPUT);
    gpio_enable(RELAY3_GPIO, GPIO_OUTPUT);
    gpio_enable(RELAY4_GPIO, GPIO_OUTPUT);

    gpio_enable(BUTTON2_GPIO, GPIO_INPUT);
    gpio_enable(BUTTON3_GPIO, GPIO_INPUT);
    gpio_enable(BUTTON4_GPIO, GPIO_INPUT);

    relay2_write(switch_on2.value.bool_value);
    relay3_write(switch_on3.value.bool_value);
    relay4_write(switch_on4.value.bool_value);
#endif
}

void switch_on_callback1(homekit_characteristic_t *_ch, homekit_value_t on, void *context) {
    relay1_write(switch_on1.value.bool_value);
}

#ifdef IS_SONOFF_4CH
void switch_on_callback2(homekit_characteristic_t *_ch, homekit_value_t on, void *context) {
    relay2_write(switch_on2.value.bool_value);
}

void switch_on_callback3(homekit_characteristic_t *_ch, homekit_value_t on, void *context) {
    relay3_write(switch_on3.value.bool_value);
}

void switch_on_callback4(homekit_characteristic_t *_ch, homekit_value_t on, void *context) {
    relay4_write(switch_on4.value.bool_value);
}
#endif

void button_callback(uint8_t gpio, button_event_t event) {

    switch (event) {
        case button_event_single_press:
        switch (gpio) {
            case BUTTON1_GPIO:
#ifdef IS_SONOFF_MINIR2
            case SWITCH1_GPIO:
#endif
                printf("Toggling relay 1\n");
                switch_on1.value.bool_value = !switch_on1.value.bool_value;
                relay1_write(switch_on1.value.bool_value);
                homekit_characteristic_notify(&switch_on1, switch_on1.value);
            break;
#ifdef IS_SONOFF_4CH
            case BUTTON2_GPIO:
                printf("Toggling relay 2\n");
                switch_on2.value.bool_value = !switch_on2.value.bool_value;
                relay2_write(switch_on2.value.bool_value);
                homekit_characteristic_notify(&switch_on2, switch_on2.value);
            break;
            case BUTTON3_GPIO:
                printf("Toggling relay 3\n");
                switch_on3.value.bool_value = !switch_on3.value.bool_value;
                relay3_write(switch_on3.value.bool_value);
                homekit_characteristic_notify(&switch_on3, switch_on3.value);
            break;
            case BUTTON4_GPIO:
                printf("Toggling relay 4\n");
                switch_on4.value.bool_value = !switch_on4.value.bool_value;
                relay4_write(switch_on4.value.bool_value);
                homekit_characteristic_notify(&switch_on4, switch_on4.value);
            break;
#endif
        }

            break;
        case button_event_long_press:
            if (gpio == BUTTON1_GPIO) {
                reset_configuration();
            }
            break;
        default:
            printf("Unknown button event: %d\n", event);
    }
}

void switch_identify_task(void *_args) {
    // We identify the Sonoff by Flashing it's LED.
    for (int i=0; i<3; i++) {
        for (int j=0; j<2; j++) {
            led_write(true);
            vTaskDelay(100 / portTICK_PERIOD_MS);
            led_write(false);
            vTaskDelay(100 / portTICK_PERIOD_MS);
        }

        vTaskDelay(250 / portTICK_PERIOD_MS);
    }

    led_write(false);

    vTaskDelete(NULL);
}

void switch_identify(homekit_value_t _value) {
    printf("Switch identify\n");
    xTaskCreate(switch_identify_task, "Switch identify", 128, NULL, 2, NULL);
}

homekit_characteristic_t name = HOMEKIT_CHARACTERISTIC_(NAME, "Sonoff");

homekit_accessory_t *accessories[] = {
    HOMEKIT_ACCESSORY(.id=1, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]){
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
            &name,
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "iTEAD"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, DEVICE_SERIAL_NUMBER),
            HOMEKIT_CHARACTERISTIC(MODEL, DEVICE_MODEL),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, VERSION),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, switch_identify),
            NULL
        }),
            HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Sonoff switch 1"),
            &switch_on1,
            NULL
        }),
        NULL
    }),
#ifdef IS_SONOFF_4CH
    HOMEKIT_ACCESSORY(.id=2, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]){
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
            &name,
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "iTEAD"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, DEVICE_SERIAL_NUMBER),
            HOMEKIT_CHARACTERISTIC(MODEL, DEVICE_MODEL),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, VERSION),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, switch_identify),
            NULL
        }),
            HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Sonoff switch 2"),
            &switch_on2,
            NULL
        }),
        NULL
    }),
    HOMEKIT_ACCESSORY(.id=3, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]){
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
            &name,
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "iTEAD"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, DEVICE_SERIAL_NUMBER),
            HOMEKIT_CHARACTERISTIC(MODEL, DEVICE_MODEL),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, VERSION),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, switch_identify),
            NULL
        }),
            HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Sonoff switch 3"),
            &switch_on3,
            NULL
        }),
        NULL
    }),
    HOMEKIT_ACCESSORY(.id=4, .category=homekit_accessory_category_switch, .services=(homekit_service_t*[]){
        HOMEKIT_SERVICE(ACCESSORY_INFORMATION, .characteristics=(homekit_characteristic_t*[]){
            &name,
            HOMEKIT_CHARACTERISTIC(MANUFACTURER, "iTEAD"),
            HOMEKIT_CHARACTERISTIC(SERIAL_NUMBER, DEVICE_SERIAL_NUMBER),
            HOMEKIT_CHARACTERISTIC(MODEL, DEVICE_MODEL),
            HOMEKIT_CHARACTERISTIC(FIRMWARE_REVISION, VERSION),
            HOMEKIT_CHARACTERISTIC(IDENTIFY, switch_identify),
            NULL
        }),
            HOMEKIT_SERVICE(SWITCH, .primary=true, .characteristics=(homekit_characteristic_t*[]){
            HOMEKIT_CHARACTERISTIC(NAME, "Sonoff switch 4"),
            &switch_on4,
            NULL
        }),
        NULL
    }),
#endif
    NULL
};


homekit_server_config_t config = {
    .accessories = accessories,
    .password = "111-11-111"
};

void create_accessory_name() {
    uint8_t macaddr[6];
    sdk_wifi_get_macaddr(STATION_IF, macaddr);

    int name_len = snprintf(NULL, 0, "Sonoff Switch-%02X%02X%02X",
                            macaddr[3], macaddr[4], macaddr[5]);
    char *name_value = malloc(name_len+1);
    snprintf(name_value, name_len+1, "Sonoff Switch-%02X%02X%02X",
             macaddr[3], macaddr[4], macaddr[5]);

    name.value = HOMEKIT_STRING(name_value);
}

static void wifi_init() {
    struct sdk_station_config wifi_config = {
        .ssid = WIFI_SSID,
        .password = WIFI_PASSWORD,
    };

    sdk_wifi_station_set_config(&wifi_config);
    sdk_wifi_station_connect();
    sdk_wifi_station_set_auto_connect(true);
}

void user_init(void) {
    uart_set_baud(0, 115200);

    create_accessory_name();

    wifi_init();
    homekit_server_init(&config);

    gpio_init();

    if (button_create(BUTTON1_GPIO, 0, 4000, button_callback)) {
        printf("Failed to initialize button 1\n");
    }

#ifdef IS_SONOFF_MINIR2
    if (button_create(SWITCH1_GPIO, 0, 4000, button_callback)) {
        printf("Failed to initialize switch 1\n");
    }
#endif

#ifdef IS_SONOFF_4CH
    if (button_create(BUTTON2_GPIO, 0, 4000, button_callback)) {
        printf("Failed to initialize button 2\n");
    }

    if (button_create(BUTTON3_GPIO, 0, 4000, button_callback)) {
        printf("Failed to initialize button 3\n");
    }

    if (button_create(BUTTON4_GPIO, 0, 4000, button_callback)) {
        printf("Failed to initialize button 4\n");
    }
#endif
}
