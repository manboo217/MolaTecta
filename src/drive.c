/*
 * drive.c
 *
 *  Created on: 2017/10/23
 *      Author: Blue
 */

#include "global.h"


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_init
// ���s�n�̕ϐ��̏������C���[�^�[�h���C�o�֌W��GPIO�ݒ��PWM�o�͂Ɏg���^�C�}�̐ݒ������
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_init(void){

	//====���s�n�̕ϐ��̏�����====
	max_t_cnt = MAX_T_CNT;			//�e�[�u���J�E���^�ō��l������     MAX_T_CNT��params.h�Ƀ}�N����`����
	min_t_cnt = MIN_T_CNT;			//�e�[�u���J�E���^�Œ�l������     MIN_T_CNT��params.h�Ƀ}�N����`����
	MF.FLAGS = 0;					//�t���O�N���A
	//====���[�^�[�h���C�o�֌W��GPIO�ݒ�====
	pin_dir(PIN_M3, OUT);
	pin_dir(PIN_CW_R, OUT);
	pin_dir(PIN_CW_L, OUT);
	pin_dir(PIN_REF, OUT);
	pin_write(PIN_M3, HIGH);		//�X�e�b�s���O���[�^�㎥OFF
	drive_set_dir(FORWARD);			//�O�i����悤�Ƀ��[�^�̉�]������ݒ�
	//====PWM�o�͂Ɏg���^�C�}�̐ݒ�====
	/*--------------------------------------------------------------------
		TIM16 : 16�r�b�g�^�C�}�B�����[�^�̐���Ɏg���B�o�͂�TIM16_CH1(PB4)
	--------------------------------------------------------------------*/
	__HAL_RCC_TIM16_CLK_ENABLE();

	TIM16->CR1 = 0;						//�^�C�}����
	TIM16->CR2 = 0;
	TIM16->DIER = TIM_DIER_UIE;			//�^�C�}�X�V���荞�݂�L����
	TIM16->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;	//PWM���[�h1
	TIM16->CCER = TIM_CCER_CC1E;		//TIM16_CH1�o�͂��A�N�e�B�uHigh��
	TIM16->BDTR = TIM_BDTR_MOE;			//PWM�o�͂�L����
	TIM16->CNT = 0;						//�^�C�}�J�E���^�l��0�Ƀ��Z�b�g
	TIM16->PSC = 63;					//�^�C�}�̃N���b�N���g�����V�X�e���N���b�N/64=1MHz�ɐݒ�
	TIM16->ARR = DEFAULT_INTERVAL;		//�^�C�}�J�E���^�̏���l�B��芸����DEFAULT_INTERVAL(params.h)�ɐݒ�
	TIM16->CCR1 = 25;					//�^�C�}�J�E���^�̔�r��v�l
	TIM16->EGR = TIM_EGR_UG;			//�^�C�}�ݒ�𔽉f�����邽�߂Ƀ^�C�}�X�V�C�x���g���N����

	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);			//�^�C�}�X�V���荞�݃n���h����L����
	NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 2);	//�^�C�}�X�V���荞�݂̊��荞�ݗD��x��ݒ�

	pin_set_alternate_function(PB4, 1);			//PB4 : TIM16_CH1��AF1�ɊY��

	/*--------------------------------------------------------------------
		TIM17 : 16�r�b�g�^�C�}�B�E���[�^�̐���Ɏg���B�o�͂�TIM17_CH1(PB5)
	--------------------------------------------------------------------*/
	__HAL_RCC_TIM17_CLK_ENABLE();

	TIM17->CR1 = 0;						//�^�C�}����
	TIM17->CR2 = 0;
	TIM17->DIER = TIM_DIER_UIE;			//�^�C�}�X�V���荞�݂�L����
	TIM17->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;	//PWM���[�h1
	TIM17->CCER = TIM_CCER_CC1E;		//TIM16_CH1�o�͂��A�N�e�B�uHigh��
	TIM17->BDTR = TIM_BDTR_MOE;			//PWM�o�͂�L����
	TIM17->CNT = 0;						//�^�C�}�J�E���^�l��0�Ƀ��Z�b�g
	TIM17->PSC = 63;					//�^�C�}�̃N���b�N���g�����V�X�e���N���b�N/64=1MHz�ɐݒ�
	TIM17->ARR = DEFAULT_INTERVAL;		//�^�C�}�J�E���^�̏���l�B��芸����DEFAULT_INTERVAL(params.h)�ɐݒ�
	TIM17->CCR1 = 25;					//�^�C�}�J�E���^�̔�r��v�l
	TIM17->EGR = TIM_EGR_UG;			//�^�C�}�ݒ�𔽉f�����邽�߂Ƀ^�C�}�X�V�C�x���g���N����

	NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);			//�^�C�}�X�V���荞�݃n���h����L����
	NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, 2);		//�^�C�}�X�V���荞�݂̊��荞�ݗD��x��ݒ�

	pin_set_alternate_function(PB5, 10);	//PB5 : TIM17_CH1��AF10�ɊY��
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_enable_motor
// �X�e�b�s���O���[�^��㎥����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_enable_motor(void){
	pin_write(PIN_REF, HIGH);
	pin_write(PIN_M3, LOW);			//�X�e�b�s���O���[�^�㎥ON
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_disable_motor
// �X�e�b�s���O���[�^�̗㎥��؂�
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_disable_motor(void){
	pin_write(PIN_M3, HIGH);		//�X�e�b�s���O���[�^�㎥OFF
}

/*----------------------------------------------------------
		16bit�^�C�}���荞��
----------------------------------------------------------*/
/**********
---���s�̎d�g��---
�X�e�b�s���O���[�^�̓��쐧���16bit�^�C�}�ōs���Ă���B�e16bit�^�C�}�̐ݒ�́C
�E�J�E���g�J�n����CCR1�܂ł̊Ԃ͏o�̓s����Low�ɂȂ�
�ECCR1�ɒB����Əo�̓s����High�ɂȂ�
�EARR�ɒB����Ɗ��荞�݂𐶐�+�^�C�}�J�E���^�����Z�b�g
�ƂȂ��Ă���idrive_init�����Q�Ɓj
���[�^�h���C�o�́iRef��High�ɂ�����ԂŁjClock��High�ɂ��邱�Ƃň��p�x��]���C
Low�ɖ߂�����܂�High�ɂ��邱�Ƃł܂����p�x��]����B
Clock�ɂ̓^�C�}�̏o�̓s�����q���ł���̂ŁC�^�C�}�̎����𑬂�����قǉ�]�������Ȃ�B
���̃v���O�����ł̓J�E���g�J�n����CCR1�̊Ԃ����ɂ��āi���[�^�h���C�o��High��F�����镪�������K�v�j�C
�J�E���g�J�n����ARR�̊Ԃ𒲐����邱�Ƃő��x��ω������Ă���B
���������̏�Ԃ�MF.FLAG�\���́iglobal.h�Œ�`�j�ŊǗ�����Ă���B
�������ɂ��āC���O��Excel�Ōv�Z�������ԁi���荞�݁j���Ƃ�ARR�̒l��table[]�z��ɋL�^���Ă����B
�i�z��̒l��table.h��Excel����R�s�[&�y�[�X�g�����Đݒ肷��Btable[]�̒�`��drive.h�Q�Ɓj
���������̂ǂ̒i�K�Ȃ̂��itable[]�̗v�f�ԍ��E�C���f�b�N�X�j��t_cnt_l,t_cnt_r�ŋL�^���Ă���B
**********/

//+++++++++++++++++++++++++++++++++++++++++++++++
//TIM1_UP_TIM16_IRQHandler
// 16�r�b�g�^�C�}�[TIM16�̊��荞�݊֐��C�����[�^�̊Ǘ����s��
// �����F����
// �߂�l�F����
//+++++++++++++++++++++++++++++++++++++++++++++++
void TIM1_UP_TIM16_IRQHandler(){

	if( !(TIM16->SR & TIM_SR_UIF) ){
		return;
	}
	pulse_l++;															//���p���X�̃J�E���^���C���N�������g
	//====����������====
	//----��������----
	if(sp_mode >= 1){
		if(MF.FLAG.DECL){													//�����t���O�������Ă���ꍇ
			t_cnt_l = max(t_cnt_l - 1, min_t_cnt);
		}
	//----��������----
		else if(MF.FLAG.ACCL){												//�����t���O�������Ă���ꍇ
			t_cnt_l = min(t_cnt_l + 1, max_t_cnt);
		}

	//----�f�t�H���g�C���^�[�o��----
		if(MF.FLAG.DEF){													//�f�t�H���g�C���^�[�o���t���O�������Ă���ꍇ
			TIM16->ARR = DEFAULT_INTERVAL - dl;								//�f�t�H���g�C���^�[�o���ɐ�����������l�ɐݒ�
		}

	/******SLALOM*******/
	//���^�[���ŏ�
		else if(MF.FLAG.SLL_L_UP){
			if(sll_t_cnt_l<SLL_T_CNT1){
				sll_t_cnt_l ++;
			}
			TIM16->ARR = SLL_TABLE_IN1[sll_t_cnt_l];
		}

	//���^�[���Ō�
		else if(MF.FLAG.SLL_L_DW){
			if(sll_t_cnt_l<SLL_T_CNT1){
				sll_t_cnt_l ++;
			}
			TIM16->ARR = SLL_TABLE_IN1[SLL_T_CNT1 - sll_t_cnt_l];
			}

	//�E�^�[���ŏ�
		else if(MF.FLAG.SLL_R_UP){
			if(sll_t_cnt_l<SLL_T_CNT1){
				sll_t_cnt_l ++;
			}
			TIM16->ARR = SLL_TABLE_OUT1[sll_t_cnt_l];
		}
	//�E�^�[���Ō�
		else if(MF.FLAG.SLL_R_DW){
			if(sll_t_cnt_l<SLL_T_CNT1){
				sll_t_cnt_l ++;
			}
			TIM16->ARR = SLL_TABLE_OUT1[SLL_T_CNT1 - sll_t_cnt_l];
		}
		else if(MF.FLAG.SLL_STR){
			TIM16->ARR = SLL_INTERVAL1;
		}

	//----����ȊO�̎��̓e�[�u���J�E���^�̎w�������C���^�[�o��----
		else {
			switch(sp_mode){
				case 0:		TIM16->ARR = table2[t_cnt_l] - dl;	break;					//�X�����[��2
				case 1:		TIM16->ARR = table1[t_cnt_l] - dl;	break;					//�ȉ��X�����[��1
				case 2:		TIM16->ARR = table3[t_cnt_l] - dl;	break;
				case 3:		TIM16->ARR = table1[t_cnt_l] - dl;	break;					//�ȉ��X�����[��1
				case 4:		TIM16->ARR = table3[t_cnt_l] - dl;  break;
				case 5:		TIM16->ARR = table1[t_cnt_l] - dl;	break;					//�ȉ��X�����[��1
				default:	TIM16->ARR = table2[t_cnt_l] - dl;	break;
				}
		}
	}

	if(sp_mode == 0){
			if(MF.FLAG.DECL){													//�����t���O�������Ă���ꍇ
				t_cnt_l = max(t_cnt_l - 1, min_t_cnt);
			}
		//----��������----
			else if(MF.FLAG.ACCL){												//�����t���O�������Ă���ꍇ
				t_cnt_l = min(t_cnt_l + 1, max_t_cnt);
			}

		//----�f�t�H���g�C���^�[�o��----
			if(MF.FLAG.DEF){													//�f�t�H���g�C���^�[�o���t���O�������Ă���ꍇ
				TIM16->ARR = DEFAULT_INTERVAL - dl;								//�f�t�H���g�C���^�[�o���ɐ�����������l�ɐݒ�
			}

		/******SLALOM*******/

		//���^�[���ŏ�
			else if(MF.FLAG.SLL_L_UP){
				if(sll_t_cnt_l<SLL_T_CNT2){
					sll_t_cnt_l ++;
				}
				TIM16->ARR = SLL_TABLE_IN2[sll_t_cnt_l];
			}

		//���^�[���Ō�
			else if(MF.FLAG.SLL_L_DW){
				if(sll_t_cnt_l<SLL_T_CNT2){
					sll_t_cnt_l ++;
				}
				TIM16->ARR = SLL_TABLE_IN2[SLL_T_CNT2 - sll_t_cnt_l];
				}

		//�E�^�[���ŏ�
			else if(MF.FLAG.SLL_R_UP){
				if(sll_t_cnt_l<SLL_T_CNT2){
					sll_t_cnt_l ++;
				}
				TIM16->ARR = SLL_TABLE_OUT2[sll_t_cnt_l];
			}
		//�E�^�[���Ō�
			else if(MF.FLAG.SLL_R_DW){
				if(sll_t_cnt_l<SLL_T_CNT2){
					sll_t_cnt_l ++;
				}
				TIM16->ARR = SLL_TABLE_OUT2[SLL_T_CNT2 - sll_t_cnt_l];
			}
			else if(MF.FLAG.SLL_STR){
				TIM16->ARR = SLL_INTERVAL2;
			}

		//----����ȊO�̎��̓e�[�u���J�E���^�̎w�������C���^�[�o��----
			else {
				TIM16->ARR = table2[t_cnt_l] - dl;								//�����[�^�C���^�[�o���ݒ�
			}
		}

		TIM16->SR &= ~TIM_SR_UIF;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//TIM1_TRG_CON_TIM17_IRQHandler
// 16�r�b�g�^�C�}�[TIM17�̊��荞�݊֐��C�E���[�^�̊Ǘ����s��
// �����F����
// �߂�l�F����
//+++++++++++++++++++++++++++++++++++++++++++++++
void TIM1_TRG_COM_TIM17_IRQHandler(){

	if( !(TIM17->SR & TIM_SR_UIF) ){
		return;
	}
	pulse_r++;															//�E�p���X�̃J�E���^���C���N�������g

	if(sp_mode >= 1){
	//====����������====
	//----��������----
		if(MF.FLAG.DECL){													//�����t���O�������Ă���ꍇ
			t_cnt_r = max(t_cnt_r - 1, min_t_cnt);
		}
	//----��������----
		else if(MF.FLAG.ACCL){												//�����t���O�������Ă���ꍇ
			t_cnt_r = min(t_cnt_r + 1, max_t_cnt);
		}

	//----�f�t�H���g�C���^�[�o��----
		if(MF.FLAG.DEF){													//�f�t�H���g�C���^�[�o���t���O�������Ă���ꍇ
			TIM17->ARR = DEFAULT_INTERVAL - dr;								//�f�t�H���g�C���^�[�o���ɐ�����������l�ɐݒ�
		}

	/******SLALOM*******/

	//���^�[�� �ŏ�
		else if(MF.FLAG.SLL_L_UP){
			if(sll_t_cnt_r<SLL_T_CNT1){
				sll_t_cnt_r ++;
			}
			TIM17->ARR = SLL_TABLE_OUT1[sll_t_cnt_r];
		}

	//���^�[���Ō�
		else if(MF.FLAG.SLL_L_DW){
			if(sll_t_cnt_r<SLL_T_CNT1){
				sll_t_cnt_r ++;
			}
			TIM17->ARR = SLL_TABLE_OUT1[SLL_T_CNT1 - sll_t_cnt_r];
		}

	//�E�^�[���ŏ�
		else if(MF.FLAG.SLL_R_UP){
			if(sll_t_cnt_r<SLL_T_CNT1){
				sll_t_cnt_r ++;
			}
			TIM17->ARR = SLL_TABLE_IN1[sll_t_cnt_r];
		}
	//�E�^�[���Ō�
		else if(MF.FLAG.SLL_R_DW){
			if(sll_t_cnt_r<SLL_T_CNT1){
				sll_t_cnt_r ++;
			}
			TIM17->ARR = SLL_TABLE_IN1[SLL_T_CNT1 - sll_t_cnt_r];
		}
		else if(MF.FLAG.SLL_STR){
			TIM17->ARR = SLL_INTERVAL1;
		}

	//----����ȊO�̎��̓e�[�u���J�E���^�̎w�������C���^�[�o��----
		else {
			switch(sp_mode){
			case 0:		TIM17->ARR = table2[t_cnt_r] - dr;	break;
			case 1:		TIM17->ARR = table1[t_cnt_r] - dr;	break;					//�����[�^�C���^�[�o���ݒ�
			case 2:		TIM17->ARR = table3[t_cnt_r] - dr;	break;
			case 3: 	TIM17->ARR = table1[t_cnt_r] - dr;	break;
			case 4:		TIM17->ARR = table3[t_cnt_r] - dr;  break;
			case 5: 	TIM17->ARR = table1[t_cnt_r] - dr;	break;
			default:	TIM17->ARR = table2[t_cnt_r] - dr;	break;
			}
		}
	}
		else if(sp_mode == 0){
			//====����������====
			//----��������----
				if(MF.FLAG.DECL){													//�����t���O�������Ă���ꍇ
					t_cnt_r = max(t_cnt_r - 1, min_t_cnt);
				}
			//----��������----
				else if(MF.FLAG.ACCL){												//�����t���O�������Ă���ꍇ
					t_cnt_r = min(t_cnt_r + 1, max_t_cnt);
				}

			//----�f�t�H���g�C���^�[�o��----
				if(MF.FLAG.DEF){													//�f�t�H���g�C���^�[�o���t���O�������Ă���ꍇ
					TIM17->ARR = DEFAULT_INTERVAL - dr;								//�f�t�H���g�C���^�[�o���ɐ�����������l�ɐݒ�
				}

			/******SLALOM*******/

			//���^�[�� �ŏ�
				else if(MF.FLAG.SLL_L_UP){
					if(sll_t_cnt_r<SLL_T_CNT2){
						sll_t_cnt_r ++;
					}
					TIM17->ARR = SLL_TABLE_OUT2[sll_t_cnt_r];
				}

			//���^�[���Ō�
				else if(MF.FLAG.SLL_L_DW){
					if(sll_t_cnt_r<SLL_T_CNT2){
						sll_t_cnt_r ++;
					}
					TIM17->ARR = SLL_TABLE_OUT2[SLL_T_CNT2 - sll_t_cnt_r];
				}

			//�E�^�[���ŏ�
				else if(MF.FLAG.SLL_R_UP){
					if(sll_t_cnt_r<SLL_T_CNT2){
						sll_t_cnt_r ++;
					}
					TIM17->ARR = SLL_TABLE_IN2[sll_t_cnt_r];
				}
			//�E�^�[���Ō�
				else if(MF.FLAG.SLL_R_DW){
					if(sll_t_cnt_r<SLL_T_CNT2){
						sll_t_cnt_r ++;
					}
					TIM17->ARR = SLL_TABLE_IN2[SLL_T_CNT2 - sll_t_cnt_r];
				}
				else if(MF.FLAG.SLL_STR){
					TIM17->ARR = SLL_INTERVAL2;
				}

			//----����ȊO�̎��̓e�[�u���J�E���^�̎w�������C���^�[�o��----
				else {
					TIM17->ARR = table2[t_cnt_r] - dr;
				}
		}

		TIM17->SR &= ~TIM_SR_UIF;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_reset_t_cnt
// �e�[�u���J�E���^�����Z�b�g�imin_t_cnt�̒l�ɃZ�b�g�j����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_reset_t_cnt(void){
	t_cnt_l = t_cnt_r = min_t_cnt;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_start
// ���s���J�n����
// �ipulse_l,pulse_r��0�Ƀ��Z�b�g���ă^�C�}��L���ɂ���j
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_start(void){

	pulse_l = pulse_r = 0;		//���s�����p���X���̏�����
	TIM16->CR1 |= TIM_CR1_CEN;	// Enable timer
	TIM17->CR1 |= TIM_CR1_CEN;	// Enable timer
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_stop
// ���s���I������
// �i�^�C�}���~�߂ă^�C�}�J�E���g�l��0�Ƀ��Z�b�g����j
// ����1�F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_stop(void){

	TIM16->CR1 &= ~TIM_CR1_CEN;	// Disable timer
	TIM17->CR1 &= ~TIM_CR1_CEN;	// Disable timer
	TIM16->CNT = 0;				// Reset Counter
	TIM17->CNT = 0;				// Reset Counter
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_set_dir
// �i�s������ݒ肷��
// ����1�Fd_dir �c�c �ǂ̕����ɐi�s���邩  0���ڂō��C1���ڂŉE�̕����ݒ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_set_dir(uint8_t d_dir){

	//====�����[�^====
	switch(d_dir & 0x0f){									//0~3�r�b�g�ڂ����o��
		//----����]----
		case 0x00:											//0x00�̏ꍇ
			pin_write(PIN_CW_L, MT_FWD_L);					//����O�i�����ɐݒ�
			break;
		//----�t��]----
		case 0x01:								 			//0x01�̏ꍇ
			pin_write(PIN_CW_L, MT_BACK_L);					//������i�����ɐݒ�
			break;
	}
	//====�E���[�^====
	switch(d_dir & 0xf0){									//4~7�r�b�g�ڂ����o��
		case 0x00:											//0x00�̏ꍇ
			pin_write(PIN_CW_R, MT_FWD_R);					//�E��O�i�����ɐݒ�
			break;
		case 0x10:											//0x10�̏ꍇ
			pin_write(PIN_CW_R, MT_BACK_R);					//�E����i�����ɐݒ�
			break;
	}
}




/*==========================================================
		���s�n�֐�
==========================================================*/
/*
		��{�d�l�Ƃ��āC
		��֐�		������:���s�p���X��

		�}�E�X�t���O(MF)
			6Bit:�f�t�H���g�C���^�[�o���t���O
			5Bit:�����t���O
			4Bit:�����t���O
			3Bit:����t���O
			1Bit:�񎟑��s�t���O
*/
/*----------------------------------------------------------
		��֐�
----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//driveA
// �w��p���X���������s����
// ����1�Fdist �c�c ���s����p���X
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveA(uint16_t dist){

	//====���s====
	//----���s�J�n----
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 1;										//�����E�f�t�H���g�C���^�[�o���t���O���N���A�C�����t���O���Z�b�g
	drive_reset_t_cnt();									//�e�[�u���J�E���^�����Z�b�g
	drive_start();											//���s�J�n

	//----���s----
	while((pulse_l < dist) || (pulse_r < dist));			//���E�̃��[�^���w��p���X�ȏ�i�ނ܂őҋ@

	drive_stop();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveD
// �w��p���X���������s���Ē�~����
// ����1�Fdist �c�c ���s����p���X
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveD(uint16_t dist){

	//====���s====
	//----���s�J�n----
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//�����E�����E�f�t�H���g�C���^�[�o���t���O���N���A
	drive_start();											//����J�n

	int16_t c_pulse = dist - (t_cnt_l - min_t_cnt);			//�������s���� = ������ - �����ɕK�v�ȋ���
	if(c_pulse > 0){
		//----�������s----
		while((pulse_l < c_pulse) || (pulse_r < c_pulse));	//���E�̃��[�^���������̃p���X�ȏ�i�ނ܂őҋ@
	}

	//----�������s----
	MF.FLAG.DECL = 1;										//�����t���O���Z�b�g
	while((pulse_l < dist) || (pulse_r < dist));			//���E�̃��[�^���������̃p���X�ȏ�i�ނ܂őҋ@

	//====���s�I��====
	drive_stop();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveU
// �w��p���X���������s���Ē�~����
// ����1�Fdist �c�c ���s����p���X
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveU(uint16_t dist){

	//====�������s�J�n====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//�����E�����E�f�t�H���g�C���^�[�o���t���O���N���A
	drive_start();											//���s�J�n

	//====���s====
	while((pulse_l < dist) || (pulse_r < dist));			//���E�̃��[�^���������̃p���X�ȏ�i�ނ܂őҋ@

	//====���s�I��====
	drive_stop();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveC
// �w��p���X���f�t�H���g�C���^�[�o���ő��s���Ē�~����
// ����1�Fdist �c�c ���s����p���X
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveC(uint16_t dist){

	//====��]�J�n====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 1;
	MF.FLAG.ACCL = 0;										//�����E�����t���O���N���A�C�f�t�H���g�C���^�[�o���t���O���Z�b�g
	drive_start();											//���s�J�n

	//====��]====
	while((pulse_l < dist) || (pulse_r < dist));			//���E�̃��[�^���葬���̃p���X�ȏ�i�ނ܂őҋ@

	drive_stop();
}


void driveC_SLL(uint16_t dist){

	//====��]�J�n====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//�����E�����t���O���N���A�C�f�t�H���g�C���^�[�o���t���O���Z�b�g
	MF.FLAG.SLL_STR = 1;
	drive_start();											//���s�J�n

	//====��]====
	while((pulse_l < dist) || (pulse_r < dist));			//���E�̃��[�^���葬���̃p���X�ȏ�i�ނ܂őҋ@
	MF.FLAG.SLL_STR = 0;

}


/*----------------------------------------------------------
		��ʊ֐�
----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionA
// ����敪�������Ȃ��瑖�s����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionA(void){

	MF.FLAG.CTRL = 1;										//�����L���ɂ���
	driveA(PULSE_SEC_HALF);									//�����̃p���X���������Ȃ��瑖�s�B���s��͒�~���Ȃ�
	get_wall_info();										//�Ǐ����擾�C�Еǐ���̗L���E�����̔��f
}

void half_sectionAA(void){

	MF.FLAG.CTRL = 0;										//����𖳌��ɂ���
	driveA(PULSE_SEC_HALF);									//�����̃p���X���������Ȃ��瑖�s�B���s��͒�~���Ȃ�
	get_wall_info();										//�Ǐ����擾�C�Еǐ���̗L���E�����̔��f
}



//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionD
// ����敪�������Ȃ��瑖�s����~����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionD(void){

	MF.FLAG.CTRL = 1;										//�����L���ɂ���
	driveD(PULSE_SEC_HALF);									//�����̃p���X���������Ȃ��瑖�s�B���s��͒�~����
}

void half_sectionDD(void){

	MF.FLAG.CTRL = 0;										//�����L���ɂ���
	driveD(PULSE_SEC_HALF);									//�����̃p���X���������Ȃ��瑖�s�B���s��͒�~����
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//one_section
// 1��敪�i��Œ�~����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_section(void){

	half_sectionA();										//����敪�������s
	half_sectionD();										//����敪�������s�̂���~
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionA(TEST)
// ����敪�������Ȃ��瑖�s����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionA_test(void){

	MF.FLAG.CTRL = 0;										//����𖳌��ɂ���
	driveA(PULSE_SEC_HALF);									//�����̃p���X���������Ȃ��瑖�s�B���s��͒�~���Ȃ�
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionD
// ����敪�������Ȃ��瑖�s����~����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionD_test(void){

	MF.FLAG.CTRL = 0;										//�����L���ɂ���
	driveD(PULSE_SEC_HALF);									//�����̃p���X���������Ȃ��瑖�s�B���s��͒�~����
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//one_section
// 1��敪�i��Œ�~����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_section_test(void){

	half_sectionA_test();										//����敪�������s
	half_sectionD_test();										//����敪�������s�̂���~
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//one_sectionU
// ������1��敪�i��
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_sectionU(void){

	MF.FLAG.CTRL = 1;										//�����L���ɂ���
	driveU(PULSE_SEC_HALF);									//�����̃p���X���������s�B���s��͒�~���Ȃ�
	/*MF.FLAG.CTRL_R = 0;
	MF.FLAG.CTRL_L = 0;										//�Еǐ��䖳��
	*/
	driveU(PULSE_SEC_HALF);									//�����̃p���X���������s�B���s��͒�~���Ȃ�
	get_wall_info();										//�Ǐ����擾
}

void one_sectionUU(void){

	MF.FLAG.CTRL = 0;										//�����L���ɂ���
	driveU(PULSE_SEC_HALF*2);									//�����̃p���X���������s�B���s��͒�~���Ȃ�
}



//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_R90
// �E��90�x��]����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++

void rotate_R90(void){

	MF.FLAG.CTRL = 0;										//���䖳��
	drive_set_dir(ROTATE_R);								//�E�ɐ��񂷂�悤���[�^�̉�]������ݒ�
	//drive_wait();
	driveC(PULSE_ROT_R90);									//�f�t�H���g�C���^�[�o���Ŏw��p���X����]�B��]��ɒ�~����
	//drive_wait();
	drive_set_dir(FORWARD);									//�O�i����悤�Ƀ��[�^�̉�]������ݒ�
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_L90
// ����90�x��]����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_L90(void){

	MF.FLAG.CTRL = 0;										//����𖳌��ɂ���
	drive_set_dir(ROTATE_L);								//���ɐ��񂷂�悤���[�^�̉�]������ݒ�
	//drive_wait();
	driveC(PULSE_ROT_L90);									//�f�t�H���g�C���^�[�o���Ŏw��p���X����]�B��]��ɒ�~����
	//drive_wait();
	drive_set_dir(FORWARD);									//�O�i����悤�Ƀ��[�^�̉�]������ݒ�
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//SLL_R90
// �X�����[���ŉE��90�x��]����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++

void SLL_R90(void){

	MF.FLAG.CTRL = 0;										//���䖳��
	drive_set_dir(FORWARD);
	//�������
	drive_start();

	if(sp_mode >= 1){
	driveC_SLL(SLL_IO1_R);

	//�N���\�C�h�`�~��
	MF.FLAG.SLL_R_UP = 1;
	sll_t_cnt_l = sll_t_cnt_r = 0;

	pulse_l = pulse_r = 0;
	while((pulse_l < SLL_OUT_PULSE1/2 + CIR_OUT_PULSE_R1) && (pulse_r < SLL_IN_PULSE1/2 + CIR_IN_PULSE_R1));			//���E�̃��[�^������p���X���ȏ�i�ނ܂�

	//�`�N���\�C�h
	MF.FLAG.SLL_R_UP = 0;
	MF.FLAG.SLL_R_DW = 1;
	sll_t_cnt_l = sll_t_cnt_r = 0;
	pulse_l = pulse_r = 0;

	while((pulse_l < SLL_OUT_PULSE1/2) && (pulse_r < SLL_IN_PULSE1/2));			//���E�̃��[�^���葬���̃p���X�ȏ�i�ނ܂őҋ@

	MF.FLAG.SLL_R_DW = 0;
	pulse_l = pulse_r = 0;
	//�������
	driveC_SLL(SLL_IO1_R);
	}

	else if(sp_mode == 0){
		driveC_SLL(SLL_IO2_R);

		//�N���\�C�h�`�~��
		MF.FLAG.SLL_R_UP = 1;
		sll_t_cnt_l = sll_t_cnt_r = 0;

		pulse_l = pulse_r = 0;
		while((pulse_l < SLL_OUT_PULSE2/2 + CIR_OUT_PULSE_R2) && (pulse_r < SLL_IN_PULSE2/2 + CIR_IN_PULSE_R2));			//���E�̃��[�^������p���X���ȏ�i�ނ܂�

		//�`�N���\�C�h
		MF.FLAG.SLL_R_UP = 0;
		MF.FLAG.SLL_R_DW = 1;
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;
	//	drive_set_dir(FORWARD);

		while((pulse_l < SLL_OUT_PULSE2/2) && (pulse_r < SLL_IN_PULSE2/2));			//���E�̃��[�^���葬���̃p���X�ȏ�i�ނ܂őҋ@

		MF.FLAG.SLL_R_DW = 0;
		pulse_l = pulse_r = 0;
		//�������
		driveC_SLL(SLL_IO2_R);

	}
	get_wall_info();										//�Ǐ����擾�C�Еǐ���̗L���E�����̔��f
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//SLL_L90
// �X�����[���ō���90�x��]����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void SLL_L90(void){

	MF.FLAG.CTRL = 0;										//���䖳��
	drive_set_dir(FORWARD);								//���ɐ��񂷂�悤���[�^�̉�]������ݒ�
	drive_start();
	if(sp_mode >= 1){
		driveC_SLL(SLL_IO1_L);
		//�t���O����
		MF.FLAG.SLL_L_UP = 1;
		//initialization
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;
		while((pulse_l < SLL_IN_PULSE1 / 2 + CIR_IN_PULSE_L1) && (pulse_r < SLL_OUT_PULSE1 / 2 + CIR_OUT_PULSE_L1));			//���E�̃��[�^���葬���̃p���X�ȏ�i�ނ܂őҋ@
		//�t���O����
		MF.FLAG.SLL_L_UP = 0;
		MF.FLAG.SLL_L_DW = 1;
		//initialization
		drive_set_dir(FORWARD);
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;

		while((pulse_l < SLL_IN_PULSE1/2) && (pulse_r < SLL_OUT_PULSE1/2));

		MF.FLAG.SLL_L_DW = 0;
		pulse_l = pulse_r = 0;

		drive_set_dir(FORWARD);									//�O�i����悤�Ƀ��[�^�̉�]������ݒ�
		driveC_SLL(SLL_IO1_L);
	}

	else if(sp_mode == 0){
		driveC_SLL(SLL_IO2_L);
		//�t���O����
		MF.FLAG.SLL_L_UP = 1;
		//initialization
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;
		while((pulse_l < SLL_IN_PULSE2 / 2 + CIR_IN_PULSE_L2) && (pulse_r < SLL_OUT_PULSE2 / 2 + CIR_OUT_PULSE_L2));			//���E�̃��[�^���葬���̃p���X�ȏ�i�ނ܂őҋ@

		//drive_stop();

		//�t���O����
		MF.FLAG.SLL_L_UP = 0;
		MF.FLAG.SLL_L_DW = 1;

		//initialization
		drive_set_dir(FORWARD);
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;

		while((pulse_l < SLL_IN_PULSE2/2) && (pulse_r < SLL_OUT_PULSE2/2));

		MF.FLAG.SLL_L_DW = 0;
		pulse_l = pulse_r = 0;

		drive_set_dir(FORWARD);									//�O�i����悤�Ƀ��[�^�̉�]������ݒ�
		driveC_SLL(SLL_IO2_L);
	}

	get_wall_info();										//�Ǐ����擾�C�Еǐ���̗L���E�����̔��f

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_180
// 180�x��]����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_180(void){

	MF.FLAG.CTRL = 0;										//����𖳌��ɂ���
	drive_set_dir(ROTATE_R);								//���ɐ��񂷂�悤���[�^�̉�]������ݒ�
	//drive_wait();
	driveC(PULSE_ROT_180);									//�f�t�H���g�C���^�[�o���Ŏw��p���X����]�B��]��ɒ�~����
	//drive_wait();
	drive_set_dir(FORWARD);									//�O�i����悤�Ƀ��[�^�̉�]������ݒ�
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//set_position
// �@�̂̐K��ǂɓ��Ăďꏊ����撆���ɍ��킹��
// �����Fsw �c�c 0�ȊO�Ȃ�get_base()����
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_position(uint8_t sw){

	MF.FLAG.CTRL = 0;										//����𖳌��ɂ���
	drive_set_dir(BACK);									//��ނ���悤���[�^�̉�]������ݒ�
	//drive_wait();
	driveC(PULSE_SETPOS_BACK);								//�K�𓖂Ă���x�Ɍ�ށB��]��ɒ�~����
	//drive_wait();
	if(sw){
		get_base();
	}
	drive_set_dir(FORWARD);									//�O�i����悤�Ƀ��[�^�̉�]������ݒ�
	//drive_wait();
	driveC(PULSE_SETPOS_SET);								//�f�t�H���g�C���^�[�o���Ŏw��p���X����]�B��]��ɒ�~����
	drive_wait();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_test
// �e�X�g���[�h
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++

void drive_test(){
	//while(1){
		int k;
		int a = 1;

		d_mode = 0;
		while(!is_sw_pushed(PIN_SW_RET)){
		if( is_sw_pushed(PIN_SW_INC) ){
					ms_wait(100);
					while( is_sw_pushed(PIN_SW_INC) );
					d_mode++;

					if(d_mode > 7)	d_mode = 0;
				}

		if( is_sw_pushed(PIN_SW_DEC) ){
					ms_wait(100);
					while( is_sw_pushed(PIN_SW_DEC) );
					d_mode--;
					if(d_mode < 0)	d_mode = 7;
				}

		led_write(d_mode & 0b100, d_mode & 0b010, d_mode & 0b001);
		ms_wait(50);
		}

	switch (d_mode){

	case 0:
			for(k=0;k<1;k++){
					led_write(1, 1, 1);
					ms_wait(300);
					led_write(0, 1, 1);
					ms_wait(300);
					led_write(0, 0, 1);
					ms_wait(300);
					}

			get_base();
			led_write(0, 1, 0);

			half_sectionAA();
			for(k=0;k<6;k++){
				one_sectionUU();
				}
			half_sectionDD();
				break;
	case 1:

		for(k=0;k<1;k++){
				led_write(1, 1, 1);
				ms_wait(300);
				led_write(0, 1, 1);
				ms_wait(300);
				led_write(0, 0, 1);
				ms_wait(300);
				}

		led_write(0, 0, 1);

		for(k=0;k<8;k++){
					rotate_R90();
					ms_wait(50);
					}
			break;

	case 2:

		for(k=0;k<1;k++){
				led_write(1, 1, 1);
				ms_wait(300);
				led_write(0, 1, 1);
				ms_wait(300);
				led_write(0, 0, 1);
				ms_wait(300);
				}

				led_write(1, 0, 0);

			for(k=0;k<16;k++){
					rotate_L90();
					ms_wait(50);
			}
			break;

	case 3:

			for(k=0;k<1;k++){
				led_write(1, 1, 1);
				ms_wait(200);
				led_write(0, 1, 1);
				ms_wait(200);
				led_write(0, 0, 1);
				ms_wait(200);
				}

				led_write(1, 0, 1);


				for(k=0;k<8;k++){
					rotate_180();
					ms_wait(50);
			}
			break;

	case 4:
				a = 1;
				while(a){
					if(ad_l > 1400 && ad_fl > 1400){
						sp_mode = 1;
						a = 0;
						}
					else if(ad_r > 1400 && ad_r > 1400){
						sp_mode = 0;
						a = 0;
						}
					}
			for(k=0;k<1;k++){
					led_write(1, 1, 1);
					ms_wait(200);
					led_write(0, 1, 1);
					ms_wait(200);
					led_write(0, 0, 1);
					ms_wait(200);
					}
			half_sectionA();
			for(k=0;k<8;k++){
				//half_sectionA();
				SLL_R90();
				driveC_SLL(PULSE_SEC_HALF);
			}
			driveD(PULSE_SEC_HALF);
			drive_stop();

				break;
	case 5:
			a = 1;
			while(a){
				if(ad_l > 1400 && ad_fl > 1400){
					sp_mode = 1;
					a = 0;
					}
				else if(ad_r > 1300 && ad_r > 1300){
					sp_mode = 0;
					a = 0;
					}
				}
			for(k=0;k<1;k++){
					led_write(1, 1, 1);
					ms_wait(200);
					led_write(0, 1, 1);
					ms_wait(200);
					led_write(0, 0, 1);
					ms_wait(200);
					}
				half_sectionA();
			for(k=0;k<8;k++){
				SLL_L90();
				driveC_SLL(PULSE_SEC_HALF);
			}
			driveD(PULSE_SEC_HALF);

				drive_stop();

	default:
			break;
	}
}

void start_position_setting(void){
	rotate_R90();
	set_position(0);
	drive_wait();
	rotate_L90();
	set_position(0);
	drive_wait();
}

