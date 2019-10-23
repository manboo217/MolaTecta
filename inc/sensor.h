/*
 * sensor.h
 *
 *  Created on: 2017/10/24
 *      Author: Blue
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#define VREFINT_CAL *((uint16_t*)0x1FFFF7BA)


enum {
	ADC1_CH1 = 1,
	ADC1_CH2 = 2,
	ADC1_CH3 = 3,
	ADC1_CH4 = 4,
	ADC1_CH11 = 11,
	ADC1_CH12 = 12,
	ADC1_VREFINT = 18,
	ADC2_CH1 = 32 + 1,
	ADC2_CH2 = 32 + 2,
	ADC2_CH3 = 32 + 3,
	ADC2_CH4 = 32 + 4
};



/*============================================================
		�e��萔�E�ϐ��錾
============================================================*/
//----LED��ON/OFF�o��----
#define IR_ON	1
#define IR_OFF	0

//====�ϐ�====
#ifdef MAIN_C_												//main.c���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	/*�O���[�o���ϐ��̒�`*/
	//----���̑�----
	uint8_t tp;												//�^�X�N�|�C���^
	uint32_t ad_r, ad_fr, ad_fl, ad_l;						//A-D�l�i�[
	uint32_t old_r, old_l;
	uint16_t base_l, base_r;								//��l���i�[
	int16_t dif_l, dif_r;									//AD�l�Ɗ�Ƃ̍�
	int32_t error;
	float Kp;
	int16_t THRE_R, THRE_L;
//	uint8_t ss_info[SS_CNT_MAX][4];								//�Z���T���i�[�z��

#else														//main.c�ȊO���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	extern uint8_t tp;
	extern uint32_t ad_r, ad_fr, ad_fl, ad_l;
	extern uint32_t old_r, old_l;

	extern uint16_t base_l,  base_r;
	extern int16_t dif_l, dif_r;
	extern int32_t error;
	extern float Kp;
	extern int16_t THRE_R, THRE_L;
//	extern uint8_t ss_info[SS_CNT_MAX][4];								//�Z���T���i�[�z��

#endif

/*============================================================
		�֐��v���g�^�C�v�錾
============================================================*/

void sensor_init(void);
int get_adc_value(int);
int get_battery_voltage(void);


uint8_t get_base();					//�Z���T��l���擾
void get_wall_info();				//�Ǐ���ǂ�
/*
void store_ssinfo_in_eeprom();
void load_ssinfo_from_eeprom();
*/
#endif /* SENSOR_H_ */
