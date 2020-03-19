


/**
 * @section DESCRIPTION
 * MY9221
 * maker http://www.my-semi.com/content/products/product_mean.aspx?id=9
 * datasheet: http://www.my-semi.com/content/products/product_mean.aspx?id=28
 * shop  http://akizukidenshi.com/catalog/g/gI-09678/
 *
 * sample program schematic
 *
 *                VOUT(3.3V) --------- VOUT ------------------- VOUT
 *                 |                   |Vdd,Vled                 |Vdd,Vled
 *          --------------            --------------            ---------------
 *          mbed(LPC1768)|            |module(2)   |            |module(1)    |
 *                    p5 |------------|DI      DO  | ---------- |DI           |
 *                    p10|------------|CLKIN CLKOUT| ---------- |DCKIN        |
 *                       |            |            |            |             |
 *          -------------             --------------            --------------
 *                 |                        | GND                    | GND
 *                GND ------------------------------------------------ 
 *
 * 最新のmbed libraryでは動かない。
 * 2017/oct/22 リビジョン：153:b484a57bc302
 * を使う
 */

#include <mbed.h>
#include "MY9221.h"

DigitalOut myled(LED1);

#define ketaSuu (2) // 連結している透明7セグの数を設定する

MY9221 MY9221(p5, p10, ketaSuu); // di, clk, number(桁数)

float D_gray[ketaSuu * 12] = {0.0};

// segment pattern
//      a
//      -
//   f |g| b
//      -
//   e | | c  collm
//      -     .
//      d
uint8_t segData[12] = {
    //          f e d a   g b c cllom
    //          ----------------------
    0xf6, // 0:       1 1 1 1   0 1 1 0
    0x06, // 1:       0 0 0 0   0 1 1 0
    0x7c, // 2:       0 1 1 1   1 1 0 0
    0x3e, // 3:       0 0 1 1   1 1 1 0
    0x8e, // 4:       1 0 0 0   1 1 1 0
    0xba, // 5:       1 0 1 1   1 0 1 0
    0xfa, // 6:       1 1 1 1   1 0 1 0
    0x96, // 7:       1 0 0 1   0 1 1 0
    0xfe, // 8:       1 1 1 1   1 1 1 0
    0xbe, // 9:       1 0 1 1   1 1 1 0
    0x01, // collom:  0 0 0 0   0 0 0 1
    0x00  // brank    0 0 0 0   0 0 0 0
};

/** 表示バッファクリア
 *
 */
void clearSeg(void)
{
    for (int segNo = 0; segNo < (ketaSuu * 12); segNo++)
    {
        D_gray[segNo] = 0;
    }
}

/** main
 *
 */
int main(void)
{
    MY9221.commandSet(MY9221::fast, MY9221::bit8, MY9221::freq1, MY9221::waveApdm, MY9221::internal, MY9221::workLed, MY9221::free, MY9221::repeat);

    int display_number = 0; // 表示データ    0 ～ 99 で変化(この数字を２桁で表示する)

    while (1)
    {
        if(myled == 1){
            myled = 0;
        }
        else{
        myled = 1;
        }
        // 2桁数字表示
        uint8_t segMask = 0x01;

        clearSeg();
        for (int seg = 0; seg < 8; seg++)
        {
            if ((segMask & segData[display_number % 10]) == segMask)
            {
                D_gray[0 + seg] = 1.0;
            }

            if ((segMask & segData[display_number / 10]) == segMask)
            {
                D_gray[12 + seg] = 1.0;
            }

            segMask = segMask << 1;
        }

        MY9221.dataSet(&D_gray[0], 2);  // 一位桁
        MY9221.dataSet(&D_gray[12], 1); // 十位桁
        wait_ms(4);                     // Image refresh Rate [s] < led gray data update cycle [s]
        MY9221.refresh();

        wait(0.2);
        display_number++;
        if (display_number > 99)
        {
            display_number = 0;
        }
    }
}