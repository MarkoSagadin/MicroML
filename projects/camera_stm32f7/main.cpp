#include "sys_init.h"
#include "utility.h"
#include "flir.h"

int main() 
{
    clock_setup();
    systick_setup();
    usart_setup();
    gpio_setup();
    i2c_setup();

    printf("System setup done!\n");

    uint16_t serial_num[4] = {0,0,0,0};
    uint16_t shutter_position[2] = {0,0};

    if(!get_flir_command(command_code(LEP_CID_SYS_FLIR_SERIAL_NUMBER, LEP_I2C_COMMAND_TYPE_GET), serial_num, 4))
        printf("Fail\n");

    printf("SYS Flir Serial Number: %04X, %04X, %04X, %04X\n", 
                                                serial_num[0],
                                                serial_num[1],
                                                serial_num[2],
                                                serial_num[3]);

    if(!get_flir_command(command_code(LEP_CID_SYS_SHUTTER_POSITION, LEP_I2C_COMMAND_TYPE_GET), shutter_position , 2))
        printf("Fail\n");

    printf("Got shutter position: %04X, %04X\n", shutter_position[0], shutter_position[1]);

    delay(1000);

    printf("Change shutter position to close\n");
    shutter_position[0] = 2;
    if(!set_flir_command(command_code(LEP_CID_SYS_SHUTTER_POSITION, LEP_I2C_COMMAND_TYPE_SET), shutter_position, 2))
        printf("Fail\n");

    delay(1000);

    if(!get_flir_command(command_code(LEP_CID_SYS_SHUTTER_POSITION, LEP_I2C_COMMAND_TYPE_GET), shutter_position, 2))
        printf("Fail\n");

    printf("Got shutter position: %04X, %04X\n", shutter_position[0], shutter_position[1]);

    delay(1000);

    printf("Change shutter position to open\n");
    shutter_position[0]= 1;
    if(!set_flir_command(command_code(LEP_CID_SYS_SHUTTER_POSITION, LEP_I2C_COMMAND_TYPE_SET), shutter_position, 2))
        printf("Fail\n");

    delay(1000);

    if(!get_flir_command(command_code(LEP_CID_SYS_SHUTTER_POSITION, LEP_I2C_COMMAND_TYPE_GET), shutter_position, 2))
        printf("Fail\n");

    printf("Got shutter position: %04X, %04X\n", shutter_position[0], shutter_position[1]);



    while (1) 
    {
        gpio_set(GPIOB, GPIO14);
        delay(1000);
        gpio_clear(GPIOB, GPIO14);
        delay(1000);
    }

    return 0;
}

