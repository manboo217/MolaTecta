/*
 * sensor.c
 *
 *  Created on: 2017/10/24
 *      Author: Blue
 */


#include "global.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//sensor_init
// �Z���T�n�̕ϐ��̏������CAD�R���o�[�^�̐ݒ�ƃZ���T�l�擾�Ɏg�p����^�C�}�̐ݒ������
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void sensor_init(void){

	//====�Z���T�n�̕ϐ��̏�����====
	tp = 0;
	ad_l = ad_r = ad_fr = ad_fl = 0;
	base_l = base_r = 0;
	error = 0;


	//====AD�R���o�[�^�̐ݒ�====
	__HAL_RCC_ADC12_CLK_ENABLE();	//AD�R���o�[�^�ւ̃N���b�N������L����

	ADC12_COMMON->CCR = ADC_CCR_VREFEN | ADC_CCR_CKMODE_1;	//VREFINT�`�����l����L����, Clock���FAHB�C2����

	/*----------------
		ADC1�̐ݒ�
	----------------*/
	ADC1->CR = 0;					//ADVREGEN = 0b00, ADEN = 0;
	ADC1->CR = ADC_CR_ADVREGEN_0;	//ADVREGEN = 0b01, ADEN = 0;

	ms_wait(1);

	ADC1->CR = ADC_CR_ADCAL | ADC_CR_ADVREGEN_0;	//ADVREGEN = 0b01, ADEN = 0, ADCAL = 1;
	while(ADC1->CR & ADC_CR_ADCAL);					//wait until ADCAL == 0

	ADC1->CR = ADC_CR_ADVREGEN_0 | ADC_CR_ADEN;		//ADVREGEN = 0b01, ADEN = 0;
	while( !(ADC1->ISR & ADC_ISR_ADRDY) );			//wait until ADRDY == 1
	ADC1->ISR |= ADC_ISR_ADRDY;

	ADC1->CFGR = 0;

	ADC1->SMPR1 = ADC_SPEED << ADC_SMPR1_SMP1_Pos | ADC_SPEED << ADC_SMPR1_SMP2_Pos | ADC_SPEED << ADC_SMPR1_SMP3_Pos | ADC_SPEED << ADC_SMPR1_SMP4_Pos;
	ADC1->SMPR2 = 0b111  << ADC_SMPR2_SMP11_Pos | 0b111 << ADC_SMPR2_SMP12_Pos | 0b111 << ADC_SMPR2_SMP18_Pos;	//601.5 clk cycle

	//ADC1->SQR1 = ch << ADC_SQR1_SQ1_Pos;			//SQ1 = ch, L = 0 (1 conversion)
	ADC1->SQR2 = 0;
	ADC1->SQR3 = 0;
	ADC1->SQR4 = 0;

	ADC1->DIFSEL = 0;	//single end mode


	/*----------------
		ADC2�̐ݒ�
	----------------*/
	ADC2->CR = 0;					//ADVREGEN = 0b00, ADEN = 0;
	ADC2->CR = ADC_CR_ADVREGEN_0;	//ADVREGEN = 0b01, ADEN = 0;

	ms_wait(1);

	ADC2->CR = ADC_CR_ADCAL | ADC_CR_ADVREGEN_0;	//ADVREGEN = 0b01, ADEN = 0, ADCAL = 1;
	while(ADC2->CR & ADC_CR_ADCAL);					//wait until ADCAL == 0

	ADC2->CR = ADC_CR_ADVREGEN_0 | ADC_CR_ADEN;		//ADVREGEN = 0b01, ADEN = 0;
	while( !(ADC2->ISR & ADC_ISR_ADRDY) );			//wait until ADRDY == 1
	ADC2->ISR |= ADC_ISR_ADRDY;

	ADC2->CFGR = 0;

	ADC2->SMPR1 = ADC_SPEED << ADC_SMPR1_SMP1_Pos | ADC_SPEED << ADC_SMPR1_SMP2_Pos | ADC_SPEED << ADC_SMPR1_SMP3_Pos | ADC_SPEED << ADC_SMPR1_SMP4_Pos;
	ADC2->SMPR2 = 0;	//1.5 clk cycle

	ADC2->SQR2 = 0;
	ADC2->SQR3 = 0;
	ADC2->SQR4 = 0;

	ADC2->DIFSEL = 0;	//single end mode

	/*----------------------------
		ADC GPIO Initialization
	----------------------------*/
	pin_set_analog_mode(PIN_VOLTAGE_CHECK);		//PB1 : ADC1_CH12

	pin_set_analog_mode(PIN_SENSOR_FR);			//PA1 : ADC1_CH2
	pin_set_analog_mode(PIN_SENSOR_R);			//PA2 : ADC1_CH3
	pin_set_analog_mode(PIN_SENSOR_L);			//PA5 : ADC2_CH2
	pin_set_analog_mode(PIN_SENSOR_FL);			//PA7 : ADC2_CH4


	/*--------------------------------
		IR LED GPIO Initialization
	--------------------------------*/
	pin_dir(PIN_IR_FR, OUT);
	pin_dir(PIN_IR_R, OUT);
	pin_dir(PIN_IR_L, OUT);
	pin_dir(PIN_IR_FL, OUT);


	/*------------------------------------
		TIM6 Interrupt Initialization
	------------------------------------*/
	__HAL_RCC_TIM6_CLK_ENABLE();

	TIM6->CR1 = TIM_CR1_CEN;		//�^�C�}�L��
	TIM6->CR2 = 0;
	TIM6->DIER = TIM_DIER_UIE;		//�^�C�}�X�V���荞�݂�L����
	TIM6->CNT = 0;					//�^�C�}�J�E���^�l��0�Ƀ��Z�b�g
	TIM6->PSC = 63;					//�^�C�}�̃N���b�N���g�����V�X�e���N���b�N/64=1MHz�ɐݒ�
	TIM6->ARR = 4000;				//�^�C�}�J�E���^�̏���l�B4000�ɐݒ�B�䂦��4ms���ƂɊ��荞�ݔ���

	TIM6->EGR = TIM_EGR_UG;			//�^�C�}�ݒ�𔽉f�����邽�߂Ƀ^�C�}�X�V�C�x���g���N����

	NVIC_EnableIRQ(TIM6_DAC1_IRQn);			//�^�C�}�X�V���荞�݃n���h����L����
	NVIC_SetPriority(TIM6_DAC1_IRQn, 1);	//�^�C�}�X�V���荞�݂̊��荞�ݗD��x��ݒ�

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//sensor_wait_us
// 1us���ɃJ�E���g�A�b�v����TIM6���g����us�}�C�N���b�������~�߂�֐��B
// �iwhile���[�v���ɃI�[�o�[�t���[���N����Ƌ@�\���Ȃ��̂�TIM6�^�C�}�X�V���荞�݃n���h�����݂̂Ŏg�p���邱�Ƃ𐄏�����j
// �����Fus �c�c �������~�߂����}�C�N���b
// �߂�l�F����
//+++++++++++++++++++++++++++++++++++++++++++++++
void sensor_wait_us(uint32_t us){
	uint32_t dest = TIM6->CNT + us;
	while(TIM6->CNT < dest);
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//TIM6_DAC1_IRQHandler
// 16�r�b�g�^�C�}�[TIM6�̊��荞�݊֐��C�Z���T�l�̎擾�C������l�̌v�Z�������Ȃ�
// �����F����
// �߂�l�F����
//+++++++++++++++++++++++++++++++++++++++++++++++
void TIM6_DAC1_IRQHandler(void){

	if( !(TIM6->SR & TIM_SR_UIF) ){
		return;
	}

	switch(tp){

		//----�Z���T����----
		case 0:
			//�E�Z���T�l�̎擾
			pin_write(PIN_IR_R, HIGH);													//������LED��ON
			sensor_wait_us(IR_WAIT_US);													//�������܂�܂ŏ����ҋ@
			ad_r = get_adc_value(ADC_SENSOR_R);											//������̃A�i���O�l���L�^
			pin_write(PIN_IR_R, LOW);													//������LED��OFF
			//���Z���T�l�̎擾
			pin_write(PIN_IR_L, HIGH);													//������LED��ON
			sensor_wait_us(IR_WAIT_US);													//�������܂�܂ŏ����ҋ@
			ad_l = get_adc_value(ADC_SENSOR_L);											//������̃A�i���O�l���L�^
			pin_write(PIN_IR_L, LOW);													//������LED��OFF
			break;

		case 1:
			//���ʃZ���T�l�̎擾
			pin_write(PIN_IR_FR, HIGH);													//������LED��ON
			sensor_wait_us(IR_WAIT_US);													//�������܂�܂ŏ����ҋ@
			ad_fr = get_adc_value(ADC_SENSOR_FR);										//������̃A�i���O�l���L�^
			pin_write(PIN_IR_FR, LOW);													//������LED��OFF
			pin_write(PIN_IR_FL, HIGH);													//������LED��ON
			sensor_wait_us(IR_WAIT_US);													//�������܂�܂ŏ����ҋ@
			ad_fl = get_adc_value(ADC_SENSOR_FL);										//������̃A�i���O�l���L�^
			pin_write(PIN_IR_FL, LOW);													//������LED��OFF
			break;


		case 2://Mice����
			//����t���O������ΐ���

			if(MF.FLAG.CTRL){
				//��ᐧ��l���ꎟ�ۑ�����ϐ���錾��0�ŏ�����
				int16_t dl_tmp = 0, dr_tmp = 0;
				//��l����̍�������

	/*			if(MF.FLAG.SPEED)	Kp = Kp1;
				else				Kp = Kp2;
*/
				switch(sp_mode){
				case 0: Kp = Kp1; break;
				case 1: Kp = Kp4; break;
				case 2: Kp = Kp4; break;
				case 3: Kp = Kp4; break;
				case 4: Kp = Kp4; break;
				case 5: Kp = Kp4; break;
				default: Kp = Kp4; break;
				}
				old_r = ad_r;
				old_l = ad_l;

				if( abs(ad_r - old_r)> DIFF_THRE_R){
					THRE_R = CTRL_BASE_R + UP_R;
				}else{
					THRE_R = CTRL_BASE_R;
				}

				if( abs(ad_l - old_l)> DIFF_THRE_L){
					THRE_L = CTRL_BASE_L + UP_L;
 				}else{
					THRE_L = CTRL_BASE_L;
				}


				if((ad_r > THRE_R)&&(ad_l > THRE_L)){
					error =  ((ad_l - base_l)-(ad_r - base_r));
				}
				else if((ad_r <= THRE_R)&&(ad_l <= THRE_L)){
					error = 0;
				}
				else if(ad_r > THRE_R){
					error = -2*(ad_r - base_r);
				}
				else {
					error = 2*(ad_l - base_l);
				}

				dr_tmp -= Kp * error;		//��ᐧ��l������
				dl_tmp += Kp * error;	//��ᐧ��l������

				dr = dr_tmp;
				dl = dl_tmp;

			}
			else{
				//����t���O���Ȃ���ΐ���l0
				dl = dr = 0;
			}
			break;
			}
	//====�^�X�N�|�C���^��i�߂�====
	tp = (tp+1) % 3;

	TIM6->SR &= ~TIM_SR_UIF;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_adc_value
// �w�肳�ꂽ�`�����l���̃A�i���O�d���l�����o��
// ����1�Fch �c�c �d���l�����o���`�����l��
// �߂�l�F�d���l�i12bit����\�j
//+++++++++++++++++++++++++++++++++++++++++++++++
int get_adc_value(int ch){
	ADC_TypeDef *adc;
	if(ch / 32){
		adc = ADC2;
	}else{
		adc = ADC1;
	}

	adc->SQR1 = (ch % 32) << ADC_SQR1_SQ1_Pos;		//SQ1 = ch % 32, L = 0 (1 conversion)
	adc->CR |= ADC_CR_ADSTART;
	while( !(adc->ISR & ADC_ISR_EOC) );
	adc->ISR |= ADC_ISR_EOS;
	return adc->DR;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_base
// ����p�̊�l���擾����
// �����F�Ȃ�
// �߂�l�F���z�I�Ȓl���擾�ł������@1:�ł����@0:�ł��Ȃ�����
//+++++++++++++++++++++++++++++++++++++++++++++++
uint8_t get_base(){
	uint8_t res = 1;									//���z�I�Ȓl���擾�ł�����

	//----����p�̊���擾----
	base_l = ad_l;										//���݂̍����̃Z���T�l�Ō���
	base_r = ad_r;										//���݂̉E���̃Z���T�l�Ō���

	return res;											//���z�I�Ȓl���擾�ł�������Ԃ�

}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_wall_info
// �Ǐ����擾����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void get_wall_info(){

	//----�Ǐ��̏�����----
	wall_info = 0x00;									//�Ǐ���������
	//----�O�ǂ�����----
	if(ad_fl > WALL_BASE_FL){//ad_fr > WALL_BASE_FR || ad_fl > WALL_BASE_FL
		//AD�l��臒l���傫���i=�ǂ������Č������˕Ԃ��Ă��Ă���j�ꍇ
		wall_info |= 0x88;								//�Ǐ����X�V
		pin_write(PIN_LED2,1);

	}
	else{
		pin_write(PIN_LED2,0);
	}

	//----�E�ǂ�����----
	if(ad_r > WALL_BASE_R){
		//AD�l��臒l���傫���i=�ǂ������Č������˕Ԃ��Ă��Ă���j�ꍇ
		wall_info |= 0x44;								//�Ǐ����X�V
		pin_write(PIN_LED3,1);
	}
	else{
		pin_write(PIN_LED3,0);
	}
	//----���ǂ�����----
	if(ad_l > WALL_BASE_L){
		//AD�l��臒l���傫���i=�ǂ������Č������˕Ԃ��Ă��Ă���j�ꍇ
		wall_info |= 0x11;								//�Ǐ����X�V
		pin_write(PIN_LED1,1);
	}
	else{
		pin_write(PIN_LED1,0);
	}
/*
	MF.FLAG.CTRL_R = 0;
	MF.FLAG.CTRL_L = 0;
	if( (wall_info & 0x44) && ! (wall_info & 0X11) ){
		MF.FLAG.CTRL_R = 1;
	}
	if( ! (wall_info & 0x44) && (wall_info & 0X11) ){
		MF.FLAG.CTRL_L = 1;
	}
	*/
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//get_battery_voltage
// �o�b�e���[�̓d�����擾����
// �����F�Ȃ�
// �߂�l�F�o�b�e���[�d���l * 100
//+++++++++++++++++++++++++++++++++++++++++++++++
int get_battery_voltage(void){

	int vrefint_data = get_adc_value(ADC1_VREFINT);
	int value = get_adc_value(ADC_VOLTAGE_CHECK);

	float voltage = 3.3 * VREFINT_CAL * value / vrefint_data / 4095 * 11;
	return (int)(voltage * 100);
}
/*
//+++++++++++++++++++++++++++++++++++++++++++++++
//store_ssADC_in_eeprom
// map�f�[�^��eeprom�Ɋi�[����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void store_ssinfo_in_eeprom(void){
	eeprom_enable_write();
	int i;
	for(i = 0; i < SS_CNT_MAX; i++){
		int j;
		for(j = 0; j < 4; j++){
			eeprom_write_halfword(i*4 + j + 256, (uint16_t) ss_info[i][j]);//256=�Ǐ��i�[���̃I�t�Z�b�g
		}
	}
	eeprom_disable_write();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//load_map_in_eeprom
// map�f�[�^��eeprom����擾����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void load_ssinfo_from_eeprom(void){
	int i;
	for(i = 0; i < SS_CNT_MAX; i++){
		int j;
		for(j = 0; j < 4; j++){
			ss_info[i][j] = (uint8_t) eeprom_read_halfword(i*16 + j);
		}
	}
}
*/
