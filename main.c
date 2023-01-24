#include <msp430.h> 

#define kPeriod  3000      // 12000/3000 = 4Hz

unsigned short flg;

void initializePort1(void){

    //端子設定
    P1SEL &= ~(BIT0 + BIT6 + BIT3); //3系統の端子をIOに指定
    P1DIR |= BIT0 + BIT6;           //2系統を出力端子に指定
    P1DIR &= ~BIT3;                 //1系統を入力端子に指定

    P1REN |= BIT3;                  //１系統のみPullup,Pulldown有効化
    P1OUT |= BIT3;                  //Pullupに指定

    //P1.3のGPIO割り込み設定
    P1IES |= BIT3;                  //Edge High->Low検出に設定
    P1IE |= BIT3;                   //割り込み許可

    P1OUT |= BIT0;
    P1OUT &= ~BIT6;

}

void initializeTimerA(){
    //Setup Clock
    BCSCTL3 |= LFXT1S_2;    //Select VLO for ACLK

    //Setup Timer_A
    TACTL = TASSEL_1 + ID_0 + MC_1;
    TACCR0 = kPeriod;                   // 周期設定
    TACCTL0 |= CCIE;                    // enable interrupt
    //_BIS_SR(SCG0 | SCG0 | CPUOFF);  //goes in LPM3
}

//GPIO割り込みイベント関数
#pragma vector = PORT1_VECTOR
__interrupt void interruputPort1Test(void)
{
    //unsigned short flg;
    //flg = TACTL && MC_1;

    if (flg != 0){
        TACTL = TASSEL_1 + ID_0 + MC_0;     //Timer停止
        //_BIS_SR(SCG1 | SCG0 | CPUOFF | OSCOFF);     //goes into LPM4
        flg = 0;
    }else{
        initializeTimerA();
        flg = 1;
    }
    P1IFG &= ~BIT3;         //clear flag.
}

//Timer割り込みイベント関数
#pragma vector = TIMER0_A0_VECTOR
__interrupt void interruputTimerATest(void)
{
    P1OUT ^= (BIT0 + BIT6);
}


/**
 * main.c
 */
void main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    flg = 0;
    initializePort1();          //　P1.3の割り込みを許可

    _BIS_SR(GIE);
    //_BIS_SR(SCG1 | SCG0 | CPUOFF | OSCOFF);     //goes into LPM4
    _BIS_SR(SCG0 | SCG0 | CPUOFF);  //goes in LPM3
    for (;;);

}

