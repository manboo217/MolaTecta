/************************************************************
		WMMC Micromouse 標準プログラム 2018
		2007年~2012年	H8/3694f版が運用される
		2013/03/01		LPC向けに移植 by 長谷川 健人
		2013/09/20		標準化のため色々弄る by 高橋 有礼
		2017/11/12		STM32向けに移植
 ************************************************************/
/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#define MAIN_C_
#include "global.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>


#define BUFSIZE 50
static char buf[BUFSIZE];
#undef BUFSIZE

void sysclk_init(void){						// set SystemClock to 64MHz
	RCC->CFGR |= RCC_CFGR_PLLMUL16;			// PLL source & multiplication x 16
	RCC->CR |= RCC_CR_PLLON;				// set PLL on
	while( !(RCC->CR & RCC_CR_PLLRDY) );	// wait until PLL is ready

	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;		// APB1 clock is 32MHz (up to 36MHz)
	FLASH->ACR |= FLASH_ACR_LATENCY_1;
	RCC->CFGR |= RCC_CFGR_SW_PLL;			// switch sysclk to PLL
	while( (RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL );	// wait until the setting is applied

	SystemCoreClockUpdate();	// Updates the variable SystemCoreClock and must be called whenever the core clock is changed during program execution.
}

//ms_wait
void ms_wait(uint32_t ms){		// SystemClock is prescaled by 8 in SysTick timer
	SysTick->LOAD = 8000 - 1;	// reset value for count-down timer
	SysTick->VAL = 0;			// initial value
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;	// count start

	uint32_t i;
	for(i = 0; i < ms; i++){
		while( !(SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) );
	}
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;	// count stop
}

//usart
static void usart_io_init(void) {
	// GPIO as Tx and Rx
	// NOTE: The index for AFR is vailable only in 0 or 1.
	//       In detail, you should refer in the programming reference guide.
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	GPIOA->MODER |= (GPIO_MODE_AF_PP << GPIO_MODER_MODER9_Pos); // Alternate function in PA9
	GPIOA->AFR[1] |= (GPIO_AF7_USART1 << (4 * (9 - 8))); // alternate function select in high resistor
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR9_Pos);
	GPIOA->OSPEEDR |= (GPIO_SPEED_FREQ_HIGH << GPIO_PUPDR_PUPDR9_Pos);

	GPIOA->MODER |= (GPIO_MODE_AF_PP << GPIO_MODER_MODER10_Pos); // Alternate function in PA10
	GPIOA->AFR[1] |= (GPIO_AF7_USART1 << (4 * (10 - 8))); // anternate function select in high resistor
	GPIOA->PUPDR |= (GPIO_PULLUP << GPIO_PUPDR_PUPDR10_Pos);
	GPIOA->OSPEEDR |= (GPIO_SPEED_FREQ_HIGH << GPIO_PUPDR_PUPDR10_Pos);
}

// Tx/Rx Baud rate = f_ck / USARTDIV (16x over sampling)
// SET_BIT() and CLEAR_BIT() macros are available in stm32f3xx.h .
void usart_init(uint32_t baudrate) {
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;
	RCC->CFGR3 |= RCC_CFGR3_USART1SW_SYSCLK; // system clock == usart1 clock

	// In this project, word length is set to 8bit.
	USART1->CR1 &= ~USART_CR1_OVER8; // not OVER8
	USART1->BRR |= (uint32_t)(64e6 / baudrate);
	usart_io_init();
	USART1->CR1 |= USART_CR1_TE | USART_CR1_RE; // transmitter & receiver enable

	USART1->CR1 |= USART_CR1_UE; // enable USART1
}

void send_char(char c) {
	// Do not fail to set TE bit ==> Write data to USART_TDR ==> Wait until TDR_TC is set.
	while( !(USART1->ISR & USART_ISR_TXE) );
	USART1->TDR = c;
	while( !(USART1->ISR & USART_ISR_TC) );
}

void send_str(const char *str) {
	uint8_t p = 0;
	while( str[p] != '\0' ) {
		send_char(str[p]);
		p++;
	}
}

char get_char(void) {
	// Do not fail to set CR1_RE bit ==> Wait until RXNE bit is set. ==> Read from RDR
	while( !(USART1->ISR & USART_ISR_RXNE) );
	char buf = USART1->RDR;
	return buf;
}

void get_str(char *str) {
	char buf;
	uint8_t p = 0;
	do {
		buf = get_char();
		str[p++] = buf;
		if( buf == 127 ) p -= 2; // back space
	} while( buf != '\n' && buf != '\r' );
	str[p-1] = '\0';
}

void uart_printf(const char* format, ...) {
	va_list arg;
	va_start(arg, format);

	vsprintf(buf, format, arg);
	send_str(buf);
	va_end(arg);
}

void gpio_init(void){
	__HAL_RCC_GPIOA_CLK_ENABLE();	// Peripheral clock enable
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

	/*
	 * SW_INC, SW_DEC, SW_RET : Input, Pullup
	 */
	pin_dir(PIN_SW_INC, IN);
	pin_dir(PIN_SW_DEC, IN);
	pin_dir(PIN_SW_RET, IN);

	/*
	 * LED0~2 : Output
	 */
	pin_dir(PIN_LED1, OUT);
	pin_dir(PIN_LED2, OUT);
	pin_dir(PIN_LED3, OUT);

}

void led_write(uint8_t led1, uint8_t led2, uint8_t led3){
	pin_write(PIN_LED1, led1);
	pin_write(PIN_LED2, led2);
	pin_write(PIN_LED3, led3);
}
void led_light(int);

void speed_switcher(void);

UART_HandleTypeDef uart_handler;
HAL_StatusTypeDef uart_init(void){

	/*USART1 GPIO Configuration
		PA9     ------> USART1_TX
		PA10    ------> USART1_RX
	*/
	GPIO_InitTypeDef gpio_init_struct;
	gpio_init_struct.Pin = GPIO_PIN_9|GPIO_PIN_10;
	gpio_init_struct.Mode = GPIO_MODE_AF_PP;
	gpio_init_struct.Pull = GPIO_PULLUP;
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio_init_struct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);

	__HAL_RCC_USART1_CLK_ENABLE();	// Peripheral clock enable

	uart_handler.Instance = USART1;
	uart_handler.Init.BaudRate = 38400;
	uart_handler.Init.WordLength = UART_WORDLENGTH_8B;
	uart_handler.Init.StopBits = UART_STOPBITS_1;
	uart_handler.Init.Parity = UART_PARITY_NONE;
	uart_handler.Init.Mode = UART_MODE_TX_RX;
	uart_handler.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	uart_handler.Init.OverSampling = UART_OVERSAMPLING_16;
	uart_handler.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	uart_handler.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;

	return HAL_UART_Init(&uart_handler);
}

/*
int __io_putchar(int ch) {
    if( ch == '\n' ) {
        uart_send('\r');
    }
    uart_send(ch);
    return 0;
}
*/

int main(void){

	ms_wait(12);	// 12*8 = 96 ms
	sysclk_init();
	gpio_init();
	//uart_init();

	ms_wait(100);
	usart_io_init();
	usart_init(115200);
	ms_wait(100);

	sensor_init();
	drive_init();
	search_init();
	int mode = 0;
	int k = 0;
	int a = 1;
	MF.FLAG.SPEED= 0;

	while(1){

		if( is_sw_pushed(PIN_SW_INC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_INC) );
			mode++;

			if(mode > 7){
				mode = 0;
			}
		}
		if( is_sw_pushed(PIN_SW_DEC) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_DEC) );
			mode--;
			if(mode < 0){
				mode = 7;
			}
		}

		led_write(mode & 0b100, mode & 0b010, mode & 0b001);
		ms_wait(50);

		if( is_sw_pushed(PIN_SW_RET) ){
			ms_wait(100);
			while( is_sw_pushed(PIN_SW_RET) );

			switch(mode){

			case 0:
				;
				uart_printf("\nMODE0 First Search MODE\n\r");
				//----一次探索スラローム走行----
				led_light(0);
				speed_switcher();

				while(ad_fl <1400 && ad_l < 1400){
							drive_disable_motor();
						}
				drive_enable_motor();
				led_light(1);

				MF.FLAG.SCND = 0;
				goal_x = GOAL_X;
				goal_y = GOAL_Y;

				start_position_setting();
				//----基準値を取る----
				get_base();
				uart_printf("base_L=%d, base_R=%d\n\r",base_l, base_r);

				searchS();
				ms_wait(2000);

				goal_x = goal_y = 0;
				MF.FLAG.SPEED = 0;
				searchS();

				goal_x = GOAL_X;
				goal_y = GOAL_Y;

				drive_disable_motor();

				break;


			case 1:
				uart_printf("\nMODE1 Second Search MODE\n\r");

				led_light(2);
				speed_switcher();
				while(ad_fl <1400 && ad_l < 1400){
							drive_disable_motor();
						}
				drive_enable_motor();
				led_light(1);


				MF.FLAG.SCND = 1;
				goal_x = GOAL_X;
				goal_y = GOAL_Y;

				drive_enable_motor();
				start_position_setting();
				get_base();

				searchS();
				ms_wait(500);

				goal_x = goal_y = 0;
				MF.FLAG.SPEED = 0;
				searchS();

				goal_x = GOAL_X;
				goal_y = GOAL_Y;

				drive_disable_motor();

				break;

				case 2:
					//----一次探索連続走行----
					sp_mode = 0;
					led_light(0);
					a = 1;

					speed_switcher();
					drive_enable_motor();
					led_light(1);

					MF.FLAG.SCND = 0;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					start_position_setting();
					get_base();
					uart_printf("base_L=%d, base_R=%d\n\r",base_l, base_r);

					searchB();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					drive_disable_motor();
					break;

				case 3:
					;
					//----二次探索走行----

					uart_printf("\nMODE3 Second Search MODE\n\r");
					led_light(2);

					speed_switcher();
					drive_enable_motor();

					led_light(1);
					MF.FLAG.SCND = 1;
					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					drive_enable_motor();

					start_position_setting();
					get_base();

					searchB();
					ms_wait(500);

					goal_x = goal_y = 0;
					searchB();

					goal_x = GOAL_X;
					goal_y = GOAL_Y;

					drive_disable_motor();
					break;



				case 4:
					led_light(3);
					drive_enable_motor();

					start_position_setting();
					get_base();
					led_write(1, 0, 0);
					drive_disable_motor();

					while(1){
						for(k=0;k<2;k++){
								led_write(0, 1, 0);
								ms_wait(80);
								led_write(0, 0, 0);
								ms_wait(80);
							}

					uart_printf("error:%d, dl:%d, dr:%d\n\r",(ad_l - base_l)-(ad_r - base_r), ad_l-old_l, ad_r-old_r);

					old_r = ad_r;
					old_l = ad_l;
					led_write(1, 0, 0);
					}

					break;

				case 5:
					;
					uart_printf("\nMODE5 SENSOR CHECK MODE\n\r");
					for(k=0;k<2;k++){
						led_write(0, 1, 0);
						ms_wait(80);
						led_write(0, 0, 0);
						ms_wait(80);
					}
					drive_enable_motor();

					start_position_setting();
					get_base();
					led_write(1, 0, 0);
					drive_disable_motor();

					while(1){
						for(k=0;k<2;k++){
							led_write(0, 1, 0);
							ms_wait(80);
							led_write(0, 0, 0);
							ms_wait(80);
							}

				uart_printf("FL:%d, SL:%d, SR:%d, FR:%d, diff_L:%d, diff_R:%d, error:%d\n\r",ad_fl, ad_l, ad_r, ad_fr, ad_l - base_l, ad_r - base_r, (ad_l - base_l)-(ad_r - base_r));

				old_r = ad_r;
				old_l = ad_l;
				led_write(1, 0, 0);
				}
				break;

				case 6:
					;
				uart_printf("\nMODE6 DRIVE TEST MODE\n\r");
				led_light(5);
				drive_enable_motor();
				while(1)	drive_test();
				break;
			}
		}
		ms_wait(100);
	}
}

void led_light(int num){
	int k = 0;
	switch(num){
	case 0:
		for(k=0;k<2;k++){
			led_write(1, 1, 1);
			ms_wait(80);
			led_write(0, 0, 0);
			ms_wait(80);
			}
		break;

	case 1:
		for(k=0;k<2;k++){
			led_write(1, 0, 0);
			ms_wait(80);
			led_write(0, 1, 0);
			ms_wait(80);
			led_write(0, 0, 1);
			ms_wait(80);
			}
		break;
	case 2:
		for(k=0;k<5;k++){
			led_write(0, 1, 0);
			ms_wait(100);
			led_write(1, 0, 1);
			ms_wait(100);
			}
			led_write(0, 1, 0);
		break;
	case 3:
		for(k=0;k<2;k++){
			led_write(0, 1, 0);
			ms_wait(80);
			led_write(0, 0, 0);
			ms_wait(80);
			}
		break;
	case 5:
		for(k=0;k<3;k++){
				led_write(1, 0, 0);
				ms_wait(80);
				led_write(0, 1, 0);
				ms_wait(80);
			}
		break;
	}
}

void speed_switcher(void){
	int a = 1;
	sp_mode = 0;
	while(a){
			if( is_sw_pushed(PIN_SW_INC) ){
					ms_wait(100);
					while( is_sw_pushed(PIN_SW_INC) );
					sp_mode++;
					if(sp_mode > 7){
						sp_mode = 0;
						}
				}
			if( is_sw_pushed(PIN_SW_DEC) ){
					ms_wait(100);
					while( is_sw_pushed(PIN_SW_DEC) );
					sp_mode--;
					if(sp_mode < 0){
						sp_mode = 7;
						}
			}
			led_write(sp_mode & 0b100, sp_mode & 0b010, sp_mode & 0b001);
			ms_wait(50);

			if( is_sw_pushed(PIN_SW_RET) ){
					ms_wait(100);
					while( is_sw_pushed(PIN_SW_RET) );
					a = 0;
			}
		}
}
