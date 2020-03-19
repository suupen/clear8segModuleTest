#ifndef MBED_MY9221_H
#define MBED_MY9221_H

/**
 * MY9221 Library
 * IC name : MY9221
 * maker : http://www.my-semi.com/content/products/product_mean.aspx?id=4
 * 12-Channel LED Driver With Grayscale Adaptive Pulse Density Modulation Control
 * data sheet : http://akizukidenshi.com/download/ds/mysemi/MY9221_DS1_0.pdf
 */

/** Example
 * @code
*  #include "mbed.h"
*#include "MY9221.h"
*MY9221 MY9221(p5, p10, 2);  // di, clk, number
*
*uint16_t D_gray[12] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
*float D_gray2[12] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
*
*int main()
*{
*
*    MY9221.commandSet(MY9221::fast, MY9221::bit16, MY9221::freq1, MY9221::waveApdm, MY9221::internal, MY9221::workLed, MY9221::free, MY9221::repeat);
*
*
*   while(1) {
*
*        for(int i=0; i < 12; i++) {
*            D_gray[i] += 0x100;
*            D_gray2[i] += 0.01;
*            if(D_gray2[i] > 1)D_gray2[i] = 0;
*        }
*        MY9221.dataRawSet(D_gray, 1);
*        MY9221.dataSet(D_gray2, 2);
*        wait_ms(20);                            // Image refresh Rate [s] < led gray data update cycle [s]
*        MY9221.refresh();
*
*    }
*}
 * @endcode
*/


#include "mbed.h"
#include "MY9221.h"



/**
 *  @class MY9221
 *  @brief nothine
 */
class MY9221
{

public:

    /** Create a MY9221 port, connected to the specified IC pins
     *  @param DI pin
     *  @param DCKI pin
     *  @param IC serial connection number :min = 1(default)
     *  @note  nothing
     */
    MY9221(PinName di, PinName dcki, uint8_t number);


//=========================
// command data description
//=========================
// 0x0000にクリアした上で、下記の定義をorしていく

// CMD[10] 0000 0x00 0000 0000  lout Tr/Tf select
#define LOUT_SLOW_MODE (0x0000)
#define LOUT_FAST_MODE (0x0400)

    enum hspd_t {
        slow = 0,
        fast
    };

// CMD[9:8] 0000 00xx 0000 0000  Gryascale resolution select
#define GRAYSCALE_8BIT  (0x0000)
#define GRAYSCALE_12BIT (0x0100)
#define GRAYSCALE_14BIT (0x0200)
#define GRAYSCALE_16BIT (0x0300)

    enum bs_t {
        bit8 = 0,
        bit12,
        bit14,
        bit16
    };

// CMD[7:5] 0000 0000 xxx0 0000  Internal oscillator freq select
#define ORIGINAL_FREQ1      (0x0000)
#define ORIGINAL_FREQ2      (0x0020)
#define ORIGINAL_FREQ4      (0x0040)
#define ORIGINAL_FREQ8      (0x0060)
#define ORIGINAL_FREQ16     (0x0080)
#define ORIGINAL_FREQ64     (0x00a0)
#define ORIGINAL_FREQ128    (0x00c0)
#define ORIGINAL_FREQ256    (0x00e0)

    enum gck_t {
        freq1 = 0,
        freq2,
        freq4,
        freq8,
        freq16,
        freq64,
        freq128,
        freq256
    };

// CMD[4] 0000 0000 000x 0000  Output waveform select
#define WAVE_MYPWM    (0x0000)   // Only Grayscale = 8bit
#define WAVE_APDM     (0x0010)

    enum sep_t {
        waveMypwm = 0,
        waveApdm
    };


// CMD[3] 0000 0000 0000 x000  Grayscale clock source select
#define GCLOCK_INTERNAL   (0x0000)
#define GCLOCK_EXTERNAL   (0x0008)

    enum osc_t {
        internal = 0,
        external
    };

// CMD[2] 0000 0000 0000 0x00  Output polarity select
#define WORK_LED   (0x0000)
#define WORK_MYPWM (0x0004)

    enum pol_t {
        workLed = 0,
        workMypwm
    };

// CMD[1] 0000 0000 0000 00x0  Counter reset select
#define COUNTER_FREE   (0x0000)
#define COUNTER_RESET  (0x0002)

    enum cntset_t {
        free = 0,
        reset
    };

// CMD[0] 0000 0000 0000 000x  One-shot select
#define SHOT_REPEAT   (0x0000)
#define SHOT_SHOT     (0x0001)

    enum onest_t {
        repeat = 0,
        shot
    };




// commandData_t D[]の配列番号
// MY9221のOUTxの番号と配列番号をあわせるための定義
#define OUT3 (0)
#define OUT2 (1)
#define OUT1 (2)
#define OUT0 (3)

//=========================
// MY9221 command data 定義
//=========================
    typedef struct {
        uint16_t DA;
        uint16_t DB;
        uint16_t DC;
    } dataSet_t;

    typedef struct {
        uint16_t CMD;
        dataSet_t D[4]; // [0]:OUT3 [1]:OUT2 [2]:OUT1 [3]:OUT0
    } commandData_t;

    /** command raw set
     * @param uint16_t hardware manual 16-bit COmmand Data Description CMD[15:0] = D[207:192]
     */
    void commandRawSet(uint16_t command);

    /** command nomalization set
     * @param hspd_t    hspd    fast,   slow
     * @param bs_t      bs      bit8,   bit12,  bit14,  bit16
     * @param gck_t     gck     freq1,  freq2,  freq4,  freq8, freq16,  freq64,   freq128,  freq256
     * @param sep_t     sep     waveMypwm,  waveApdm
     * @param osc_t     osc     internal,   external
     * @param pol_t     pol     workLed,    workMypwm
     * @param cntset_t  cntset  free,   reset
     * @param onest_t   onest   repeat, shot
     */
    void commandSet(hspd_t hspd, bs_t bs, gck_t gck, sep_t sep, osc_t osc, pol_t pol, cntset_t cntset, onest_t onest);

    /** gray data set
     * @param uint16_t *data [0]=OUTA[0], [1]=OUTB[0], [2]=OUTC[0], ... [9]=OUTA[3], [10]=OUTB[3], [11]=OUTC[3]
     * @param uint8_t number MY9221 number (1 to 10)
     */
    void dataRawSet(uint16_t *data, uint8_t number);

    /** gray data set
     * @param float *data (0.0 to 1.0) [0]=OUTA[0], [1]=OUTB[0], [2]=OUTC[0], ... [9]=OUTA[3], [10]=OUTB[3], [11]=OUTC[3]
     * @param uint8_t number MY9221 number (1 to 10)
     */
    void dataSet(float *data, uint8_t number);

    /** MY9221 send data and latch
     *
     */
    void refresh(void);

//----------------------------------------------------------
private:


    /** send one word data
     * @param uint16_t data : send data
     * @param return none
     */
    void sendWord(uint16_t data);

    /** data latch & display
     * @param none
     * @param return none
     */
    void latch(void);

    /** send brock data
     * @param commandData_t *data : MY9221 Data
     * @param uint8_t number : connection number (min=1)
     * @param none
     */
    void sendBrock(commandData_t *data);






    DigitalOut _di;
    DigitalOut _dcki;

    int8_t _number;
    uint16_t command;

    commandData_t MY9221_data[10];//_number];



};
#endif  //  MBED_MY9221_H