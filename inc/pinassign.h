/*
 * pinassign.h
 *
 *  Created on: 2017/11/17
 *      Author: blue
 */

#ifndef PINASSIGN_H_
#define PINASSIGN_H_


/*============================================================
		�s���z�u�ݒ�
============================================================*/
/*------------------------------------------------------------
		���s�n
------------------------------------------------------------*/
//----�h���C�oIC�֘A----
enum {
	PIN_CLK_L = PB4,		//�N���b�N�[�q��
	PIN_CLK_R = PB5,		//�N���b�N�[�q�E
	PIN_CW_R = PB6,
	PIN_CW_L = PB3,
	PIN_REF = PB1,		//���t�@�����X
	//PIN_M1 = PXn,			//�㎥�؂�ւ����W�b�N1
	//PIN_M2 = PXn,			//�㎥�؂�ւ����W�b�N2
	PIN_M3 = PB7			//�㎥�؂�ւ����W�b�N3
};

/* �㎥�؂�ւ����W�b�N
�ESLA7078MPRT
 M1 | M2 | M3 |�㎥���[�h
  L |  L |  L | 2���㎥�iMode8�Œ�j
  L |  H |  L | 2���㎥�iModeF�Œ�j
  H |  L |  L | 1-2���㎥�i2�����j
  H |  H |  L | 1-2���㎥�iModeF�Œ�j
  L |  L |  H |  W1-2���i 4�����j
  L |  H |  H | 4W1-2���i16�����j
  H |  L |  H | 2W1-2���i 8�����j
  H |  H |  H | �X���[�v���[�h2

�ESLA7073MPRT
 M1 | M2 | �㎥���[�h
  L |  L | 4W1-2���i16�����j
  L |  H | 2W1-2���i 8�����j
  H |  L |  W1-2���i 4�����j
  H |  H |   1-2���i 2�����j
*/

//----��������֘A----
#define MT_FWD_L	HIGH		//CW/CCW�őO�ɐi�ޏo�́i���j
#define MT_BACK_L	LOW			//CW/CCW�Ō��ɐi�ޏo�́i���j
#define MT_FWD_R	LOW			//CW/CCW�őO�ɐi�ޏo�́i�E�j
#define MT_BACK_R	HIGH		//CW/CCW�Ō��ɐi�ޏo�́i�E�j


/*------------------------------------------------------------
		LED�E�X�C�b�`�n
------------------------------------------------------------*/
/*
 * SW_INC, SW_DEC, SW_RET : Input, Pullup
 */
enum {
	PIN_SW_INC = PA8,//11
	PIN_SW_DEC = PA15,//13
	PIN_SW_RET = PA11//14
};

/*
 * LED0~2 : Output
 */
enum {
	PIN_LED1 = PA13,//8
	PIN_LED2 = PA12,//12
	PIN_LED3 = PA14//15
};


/*------------------------------------------------------------
		�Z���T�n
------------------------------------------------------------*/
//----�Z���T�iPhotoTr�j�s���ԍ�----
enum {
	PIN_SENSOR_R = PA2,
	PIN_SENSOR_L = PA5,
	PIN_SENSOR_FR = PA0,
	PIN_SENSOR_FL = PA7,
	PIN_VOLTAGE_CHECK = PB1
};


//----�ԊO���i�ԐF�jLED�s���ԍ�----
enum {
	PIN_IR_R = PA3,
	PIN_IR_L = PA4,
	PIN_IR_FR = PA1,
	PIN_IR_FL = PA6
};

//----�Z���T�iPhotoTr�jAD�R���o�[�^�|�[�g�ԍ�----
enum {
	ADC_SENSOR_R = ADC1_CH3,
	ADC_SENSOR_L = ADC2_CH2,
	ADC_SENSOR_FR = ADC1_CH1,
	ADC_SENSOR_FL = ADC2_CH4,
	ADC_VOLTAGE_CHECK = ADC1_CH12
};

enum {
	BUZZER = PB0
};


#endif /* PINASSIGN_H_ */
