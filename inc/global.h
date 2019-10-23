/*
 * global.h
 *
 *  Created on: 2017/10/25
 *      Author: Blue
 */

#ifndef GLOBAL_H_
#define GLOBAL_H_

#include "stm32f3xx.h"

#include "port.h"

#include "params.h"

/*------------------------------------------------------------
		���p�E�\���̂̒�`
------------------------------------------------------------*/
/**********
���p�E�\���̂Ƃ́C���p�̂ƍ\���̂�g�ݍ��킹�����̂ŁC
�����̈ꊇ������C�����o�P�ʂł̑�����\�Ȃ��̂ł���B
�Ⴆ�΁C�ȉ���mouse_flags���p�E�\���̂�MF�ł́C
MF.FLAGS = 0;�ƑS�̂��ꊇ�ύX�ł���ق��C
MF.FLAG.DECL = 1;�ƃ����o���w�肵�ĒP�̂ŕύX���邱�Ƃ��o����B
**********/

//----�t���O���p�E�\����----
typedef union {					//���p�̂̐錾
	uint16_t FLAGS;
	struct ms_flags{			//�\���̂̐錾
		uint16_t RSV0:1;		//�\���r�b�g(B0)		(:1��1�r�b�g���̈Ӗ��C�r�b�g�t�B�[���h)
		uint16_t SCND:1;		//�񎟑��s�t���O(B1)
		uint16_t RSV2:1;		//�\���r�b�g(B2)
		uint16_t CTRL:1;		//����t���O(B3)
		uint16_t ACCL:1;		//�����t���O(B4)
		uint16_t DECL:1;		//�����t���O(B5)
		uint16_t DEF:1;			//�f�t�H���g�C���^�[�o���t���O(B6)
		uint16_t KTKKN:1;		//���m��ԉ���(B7)
		uint16_t RSV8:1;		//�\���r�b�g(B8)
		uint16_t RSV9:1;		//�\���r�b�g(B9)
		uint16_t SLL_L_UP:1;		//�X�����[�����^�[�������t���O(B10)
		uint16_t SLL_L_DW:1;		//�X�����[�����^�[�������t���O(B11)
		uint16_t SLL_R_UP:1;		//�X�����[���E�^�[�������t���O(B12)
		uint16_t SLL_R_DW:1;		//�X�����\���E�^�[�������t���O(B13)
		uint16_t SLL_STR:1;		//�X�����[�������t���O(B14)
		uint16_t SPEED:1;		//�X�����[���X�s�[�h�ύX�t���O(B15)//�p�~
	}FLAG;
} mouse_flags;

#ifdef MAIN_C_							//main.c���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	/*�O���[�o���ϐ��̒�`*/
	volatile mouse_flags MF;			//�}�E�X�X�e�[�^�X���܂Ƃ߂����p�E�\����
#else									//main.c�ȊO���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	/*�O���[�o���ϐ��̐錾*/
	extern volatile mouse_flags MF;
#endif


#include "main.h"
#include "eeprom.h"

#include "drive.h"
#include "sensor.h"
#include "search.h"

#include "pinassign.h"

#endif /* GLOBAL_H_ */
