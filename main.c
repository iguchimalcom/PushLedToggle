#include <msp430.h> 

#define kPeriod  3000      // 12000/3000 = 4Hz

unsigned short flg;

void initializePort1(void){

    //�[�q�ݒ�
    P1SEL &= ~(BIT0 + BIT6 + BIT3); //3�n���̒[�q��IO�Ɏw��
    P1DIR |= BIT0 + BIT6;           //2�n�����o�͒[�q�Ɏw��
    P1DIR &= ~BIT3;                 //1�n������͒[�q�Ɏw��

    P1REN |= BIT3;                  //�P�n���̂�Pullup,Pulldown�L����
    P1OUT |= BIT3;                  //Pullup�Ɏw��

    //P1.3��GPIO���荞�ݐݒ�
    P1IES |= BIT3;                  //Edge High->Low���o�ɐݒ�
    P1IE |= BIT3;                   //���荞�݋���

    P1OUT |= BIT0;
    P1OUT &= ~BIT6;

}

void initializeTimerA(){
    //Setup Clock
    BCSCTL3 |= LFXT1S_2;    //Select VLO for ACLK

    //Setup Timer_A
    TACTL = TASSEL_1 + ID_0 + MC_1;
    TACCR0 = kPeriod;                   // �����ݒ�
    TACCTL0 |= CCIE;                    // enable interrupt
    //_BIS_SR(SCG0 | SCG0 | CPUOFF);  //goes in LPM3
}

//GPIO���荞�݃C�x���g�֐�
#pragma vector = PORT1_VECTOR
__interrupt void interruputPort1Test(void)
{
    //unsigned short flg;
    //flg = TACTL && MC_1;

    if (flg != 0){
        TACTL = TASSEL_1 + ID_0 + MC_0;     //Timer��~
        //_BIS_SR(SCG1 | SCG0 | CPUOFF | OSCOFF);     //goes into LPM4
        flg = 0;
    }else{
        initializeTimerA();
        flg = 1;
    }
    P1IFG &= ~BIT3;         //clear flag.
}

//Timer���荞�݃C�x���g�֐�
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
    initializePort1();          //�@P1.3�̊��荞�݂�����

    _BIS_SR(GIE);
    //_BIS_SR(SCG1 | SCG0 | CPUOFF | OSCOFF);     //goes into LPM4
    _BIS_SR(SCG0 | SCG0 | CPUOFF);  //goes in LPM3
    for (;;);

}

