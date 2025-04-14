#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "mpu6050.h"

const int I2C_CHIP_ADDRESS = 0x68;
const int I2C_SDA_GPIO = 20;
const int I2C_SCL_GPIO = 21;

void i2c_task(void *p) {
    i2c_init(i2c_default, 400 * 1000);
    gpio_set_function(I2C_SDA_GPIO, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_GPIO, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_GPIO);
    gpio_pull_up(I2C_SCL_GPIO);

    // reset device to its default state
    // colocando 1 no bit 7 do registrador 0x6B
    uint8_t buf_write[2];
    buf_write[0] = MPUREG_PWR_MGMT_1; // registrador
    buf_write[1] = 1 << 7;            // valor
    i2c_write_blocking(i2c_default, I2C_CHIP_ADDRESS, buf_write, 2, false);
    vTaskDelay(pdMS_TO_TICKS(100));  // Aguarda tempo para reset
    // TODO
    
    uint8_t reg = 0x1C;
    uint8_t current_value;

    // Zera bits 4 e 3 e configura para 4G
    
    current_value &= 0xE7;
    current_value |= (1 << 3);


    // Escreve o novo valor
    buf_write[0] = reg;
    buf_write[1] = 0x08;
    i2c_write_blocking(i2c_default, I2C_CHIP_ADDRESS, buf_write, 2, false);

    vTaskDelay(pdMS_TO_TICKS(10));
    
    i2c_write_blocking(i2c_default, I2C_CHIP_ADDRESS, &reg, 1, true);
    i2c_read_blocking(i2c_default, I2C_CHIP_ADDRESS, &current_value, 1, false);

    printf("Após configuração, ACCEL_CONFIG = 0x%02X\n", current_value);

    while (1) {
        
        //i2c_write_blocking(i2c_default, I2C_CHIP_ADDRESS, &reg, 1, true);
        //i2c_read_blocking(i2c_default, I2C_CHIP_ADDRESS, &current_value, 1, false);

        //printf("O Valor atual da configuração, ACCEL_CONFIG = 0x%02X\n", current_value);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

int main() {
    stdio_init_all();
    printf("Start RTOS \n");

    xTaskCreate(i2c_task, "i2c task", 4095, NULL, 1, NULL);
    vTaskStartScheduler();

    while (true) {
    }
}
