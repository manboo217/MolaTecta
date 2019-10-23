/*
 * port.c
 *
 *  Created on: 2017/10/22
 *      Author: Blue
 */


#include "global.h"

// GPIO_TypeDef *gpio �̂Ƃ���Ōx�����o��̂ňꎞ�I�ɖ�����
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"

//+++++++++++++++++++++++++++++++++++++++++++++++
//_get_gpio_name
// �w�肳�ꂽ�s����GPIO�O���[�v�����擾����
// ����1�Fpin �c�c �s���ԍ�(PXn)
// �߂�l�FGPIO�O���[�v��
//+++++++++++++++++++++++++++++++++++++++++++++++
GPIO_TypeDef * _get_gpio_name(uint32_t pin){
	GPIO_TypeDef *gpio;
	switch(pin / 16){
		case 0:
			#ifdef GPIOA
			gpio = GPIOA;
			#endif
			break;
		case 1:
			#ifdef GPIOB
			gpio = GPIOB;
			#endif
			break;
		case 2:
			#ifdef GPIOC
			gpio = GPIOC;
			#endif
			break;
		case 3:
			#ifdef GPIOD
			gpio = GPIOD;
			#endif
			break;
		case 4:
			#ifdef GPIOE
			gpio = GPIOE;
			#endif
			break;
		case 5:
			#ifdef GPIOF
			gpio = GPIOF;
			#endif
			break;
	}
	return gpio;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_write
// �w�肳�ꂽ�s����High�܂���Low�ɂ���
// ����1�Fpin �c�c �s���ԍ�(PXn)
// ����2�Fdat �c�c High��Low��(0:Low,����ȊO:High)
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_write(uint32_t pin, uint8_t val){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	if(val){
		gpio->ODR |= 1 << (pin % 16);
	}else{
		gpio->ODR &= ~(1 << (pin % 16));
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_dir
// �w�肳�ꂽ�s���̓��o�͂�ݒ肷��
// ����1�Fpin �c�c �s���ԍ�(PXn)
// ����2�Fdir �c�c ���͂��o�͂�(0:����,����ȊO:�o��)
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_dir(uint32_t pin, uint8_t dir){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	if(dir){	// Output
		gpio->MODER &= ~(1 << (pin % 16 * 2 + 1));
		gpio->MODER |= 1 << (pin % 16 * 2);
		gpio->OTYPER &= ~(1 << pin % 16);
		gpio->OSPEEDR &= ~(0b11 << (pin % 16 * 2));
		gpio->PUPDR &= ~(0b11 << (pin % 16 * 2));
	}else{		// Input, Pullup
		gpio->MODER &= ~(0b11 << (pin % 16 * 2));
		gpio->PUPDR &= ~(1 << (pin % 16 * 2 + 1));
		gpio->PUPDR |= 1 << (pin % 16 * 2);
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_read
// �w�肳�ꂽ�s���̓��͒l���擾����
// ����1�Fpin �c�c �s���ԍ�(PXn)
// �߂�l�F0:Low,����ȊO:High
//+++++++++++++++++++++++++++++++++++++++++++++++
uint32_t pin_read(uint32_t pin){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	return gpio->IDR & 1 << (pin % 16);
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_set_alternate_function
// �w�肳�ꂽ�s����Alternate Function��ݒ肷��
// ����1�Fpin �c�c �s���ԍ�(PXn)
// ����2�Faf �c�c Alternate Function �ԍ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_set_alternate_function(uint32_t pin, uint8_t alternate_function){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	gpio->MODER |= 1 << (pin % 16 * 2 + 1);
	gpio->MODER &= ~(1 << (pin % 16 * 2));

	gpio->PUPDR &= ~(0b11 << (pin % 16 * 2));

	gpio->AFR[pin % 16 / 8] &= ~(0b1111 << (pin % 8) * 4);
	gpio->AFR[pin % 16 / 8] |= alternate_function << (pin % 8) * 4;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//pin_set_analog_mode
// �w�肳�ꂽ�s����Analog Mode�ɐݒ肷��
// ����1�Fpin �c�c �s���ԍ�(PXn)
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void pin_set_analog_mode(uint32_t pin){
	GPIO_TypeDef *gpio = _get_gpio_name(pin);

	gpio->MODER |= 0b11 << (pin % 16 * 2);
	gpio->PUPDR &= ~(0b11 << (pin % 16 * 2));
}

// �x�����ėL����
#pragma GCC diagnostic warning "-Wmaybe-uninitialized"
