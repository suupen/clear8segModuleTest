#include "mbed.h"
#include "MY9221.h"

/**
 *
 */
MY9221::MY9221(PinName di, PinName dcki, uint8_t number
              ) : _di(di), _dcki(dcki), _number(number)
{

    // initialize of variable and sfr
    _di = 0;
    _dcki = 0;
    
    command = 0x0000;
    commandRawSet(command);


}

/** send one word data
 * @param uint16_t data : send data
 * @param return none
 */
void MY9221::sendWord(uint16_t data)
{
    uint16_t temp = data;
    uint8_t count = 8;

    __disable_irq(); // 禁止
    _dcki = 0;
    wait_us(1);

    do {

        _di = ((temp & 0x8000) == 0x8000) ? 1 : 0;
        temp <<= 1;
        wait_us(1);

        _dcki = 1;
        wait_us(1);

        _di = ((temp & 0x8000) == 0x8000) ? 1 : 0;
        temp <<= 1;
        wait_us(1);

        _dcki = 0;
        wait_us(1);

    } while(--count > 0);

    _di = 0;
    _dcki = 0;
    __enable_irq(); // 許可

}

/** data latch & display
 * @param none
 * @param return none
 */
void MY9221::latch(void)
{
    __disable_irq(); // 禁止

    _dcki = 0;
    wait_us(1);

    _di = 1;
    wait_us(1);
    _di = 0;
    wait_us(1);

    _di = 1;
    wait_us(1);
    _di = 0;
    wait_us(1);

    _di = 1;
    wait_us(1);
    _di = 0;
    wait_us(1);

    _di = 1;
    wait_us(1);
    _di = 0;
    wait_us(1);


    _dcki = 0;
    __enable_irq(); // 許可

}

/** send brock data
 * @param commandData_t *data : MY9221 Data
 * @param uint8_t number : connection number (min=1)
 * @param none
 */
void MY9221::sendBrock(commandData_t *data)
{
    for(uint8_t i = 0; i < _number; i++) {

        sendWord((data + i)->CMD);

        sendWord((data + i)->D[OUT3].DA);
        sendWord((data + i)->D[OUT3].DB);
        sendWord((data + i)->D[OUT3].DC);

        sendWord((data + i)->D[OUT2].DA);
        sendWord((data + i)->D[OUT2].DB);
        sendWord((data + i)->D[OUT2].DC);

        sendWord((data + i)->D[OUT1].DA);
        sendWord((data + i)->D[OUT1].DB);
        sendWord((data + i)->D[OUT1].DC);

        sendWord((data + i)->D[OUT0].DA);
        sendWord((data + i)->D[OUT0].DB);
        sendWord((data + i)->D[OUT0].DC);
    }

    latch();
}


void MY9221::commandRawSet(uint16_t command)
{
    for(int i = 0; i < _number; i++) {
        MY9221_data[i].CMD = command;
    }
}


void MY9221::commandSet(hspd_t hspd, bs_t bs, gck_t gck, sep_t sep, osc_t osc, pol_t pol, cntset_t cntset, onest_t onest)
{

    command = 
        ((hspd   << 11)  & 0x0400) |
        ((bs     << 8)   & 0x0300) | 
        ((gck    << 5)   & 0x0e00) | 
        ((sep    << 4)   & 0x0010) | 
        ((osc    << 3)   & 0x0008) | 
        ((pol    << 2)   & 0x0004) | 
        ((cntset << 1)   & 0x0002) | 
        ((onest)         & 0x0001); 
                    
    for(int i = 0; i < _number; i++) {
        MY9221_data[i].CMD = command;
    }
}

void MY9221::dataRawSet(uint16_t *data, uint8_t number)
{

    uint8_t num = 0;

    if((number > 0) && (number < 11)) {
        num = number - 1;
    }

    MY9221_data[num].D[OUT0].DA = *(data + 0);
    MY9221_data[num].D[OUT0].DB = *(data + 1);
    MY9221_data[num].D[OUT0].DC = *(data + 2);

    MY9221_data[num].D[OUT1].DA = *(data + 3);
    MY9221_data[num].D[OUT1].DB = *(data + 4);
    MY9221_data[num].D[OUT1].DC = *(data + 5);

    MY9221_data[num].D[OUT2].DA = *(data + 6);
    MY9221_data[num].D[OUT2].DB = *(data + 7);
    MY9221_data[num].D[OUT2].DC = *(data + 8);

    MY9221_data[num].D[OUT3].DA = *(data + 9);
    MY9221_data[num].D[OUT3].DB = *(data + 10);
    MY9221_data[num].D[OUT3].DC = *(data + 11);

//   printf("%04x  %04x %04x %04x        ",MY9221_data[num].CMD, 0,  *(data + 0), MY9221_data[num].D[OUT0].DA);

}

void MY9221::dataSet(float *data, uint8_t number)
{
    uint16_t bit = (command >> 8 ) & 0x0003;
    uint16_t bias; 

    uint8_t num = 0;

    if((number > 0) && (number < 11)) {
        num = number - 1;
    }

    switch(bit){
        case MY9221::bit8:
        bias = 0x00ff;
        break;
        case MY9221::bit12:
        bias = 0x0fff;
        break;
        case MY9221::bit14:
        bias = 0x3fff;
        break;
        case MY9221::bit16:
        bias = 0xffff;
        break;
        default:
        bias = 0x00ff;
        break;
        }

    MY9221_data[num].D[OUT0].DA = (uint16_t)(*(data + 0) * bias);
    MY9221_data[num].D[OUT0].DB = (uint16_t)(*(data + 1) * bias);
    MY9221_data[num].D[OUT0].DC = (uint16_t)(*(data + 2) * bias);

    MY9221_data[num].D[OUT1].DA = (uint16_t)(*(data + 3) * bias);
    MY9221_data[num].D[OUT1].DB = (uint16_t)(*(data + 4) * bias);
    MY9221_data[num].D[OUT1].DC = (uint16_t)(*(data + 5) * bias);

    MY9221_data[num].D[OUT2].DA = (uint16_t)(*(data + 6) * bias);
    MY9221_data[num].D[OUT2].DB = (uint16_t)(*(data + 7) * bias);
    MY9221_data[num].D[OUT2].DC = (uint16_t)(*(data + 8) * bias);

    MY9221_data[num].D[OUT3].DA = (uint16_t)(*(data + 9) * bias);
    MY9221_data[num].D[OUT3].DB = (uint16_t)(*(data + 10) * bias);
    MY9221_data[num].D[OUT3].DC = (uint16_t)(*(data + 11) * bias);

//   printf("%04x   %04x %f %04x\r\n",MY9221_data[num].CMD, bias,  *(data + 0), MY9221_data[num].D[OUT0].DA);

}

void MY9221::refresh(void){

    sendBrock(MY9221_data);
}
