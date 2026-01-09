
#include <cstdlib>
#include "board.h"
#include "i2c.h"

using namespace MM;

int main(int argc, char* argv[])
{
    bsp_init();
    Board hw = get_board();

    uint8_t dev_addr = 0x76;  // BMP390 device address

    const uint8_t reg_addr = 0x00;  // reg addr for chip ID

    uint8_t read_val = 0;
    uint8_t* chip_id = &read_val;
    size_t chip_id_len = 1;

    while (1)
    {
        hw.i2c.mem_read(chip_id, chip_id_len, reg_addr, dev_addr);
        for (volatile size_t i = 0; i < 100000; i++);
    }

    return 0;
}