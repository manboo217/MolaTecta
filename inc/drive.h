/*
 * drive.h
 *
 *  Created on: 2017/10/23
 *      Author: Blue
 */

#ifndef DRIVE_H_
#define DRIVE_H_

/*============================================================
		�e��萔�E�ϐ��錾
============================================================*/

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


#define FORWARD		0x00		//�O�i����
#define BACK		0x11		//���
#define ROTATE_L	0x01		//��]�����i���j
#define ROTATE_R	0x10		//��]�����i�E�j


//====�ϐ�====
#ifdef MAIN_C_										//main.c���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	/*�O���[�o���ϐ��̒�`*/
	const uint16_t table1[] = {
		#include "table1.h"
	};
	const uint16_t table2[] = {
		#include "table2.h"
	};
	const uint16_t table3[] = {
		#include "table3.h"
	};
	/*const uint16_t table4[] = {
		#include "table4.h"
	};
	*/
	const uint16_t table_ktkkn[] = {
		#include "table_ktkkn.h"
	};



	const uint16_t SLL_TABLE_IN1[] = {
		#include "sll_table_in1.h"
	};
	const uint16_t SLL_TABLE_OUT1[] = {
			#include "sll_table_out1.h"
		};

	const uint16_t SLL_TABLE_IN2[] = {
		#include "sll_table_in2.h"
	};
	const uint16_t SLL_TABLE_OUT2[] = {
			#include "sll_table_out2.h"
		};
	//table.h�ɓ\��t�����l��ێ�����z��
	volatile int16_t t_cnt_l, t_cnt_r;				//�e�[�u���J�E���^
	volatile int16_t min_t_cnt, max_t_cnt;			//�e�[�u���J�E���^�̍Œ�l�E�ő�l
	volatile uint16_t pulse_l, pulse_r;				//���E�p���X�J�E���^
	volatile int16_t dl, dr;						//��ᐧ���
	//���[�h
	volatile uint8_t d_mode;
	volatile uint8_t sp_mode;

	/*slalom�p*/										//table.h�ɓ\��t�����l��ێ�����z��
	volatile int16_t sll_t_cnt_l, sll_t_cnt_r;				//�e�[�u���J�E���^
	volatile int16_t s_min_t_cnt, s_max_t_cnt;			//�e�[�u���J�E���^�̍Œ�l�E�ő�l
	volatile uint16_t s_pulse_l, s_pulse_r;				//���E�p���X�J�E���^

#else												//main.c�ȊO���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	/*�O���[�o���ϐ��̐錾*/
	extern const uint16_t table1[];
	extern const uint16_t table2[];
	extern const uint16_t table3[];
//	extern const uint16_t table4[];
	extern const uint16_t table_ktkkn[];

	extern volatile int16_t t_cnt_l, t_cnt_r;		//�����t�������^�ɕύX
	extern volatile int16_t min_t_cnt, max_t_cnt;	//�����t�������^�ɕύX
	extern volatile uint16_t pulse_l, pulse_r;

	/*slalom�p*/
	extern const uint16_t SLL_TABLE_IN1[];
	extern const uint16_t SLL_TABLE_OUT1[];
	extern const uint16_t SLL_TABLE_IN2[];
	extern const uint16_t SLL_TABLE_OUT2[];

	extern volatile int16_t sll_t_cnt_l, sll_t_cnt_r;		//�����t�������^�ɕύX
	extern volatile int16_t s_min_t_cnt, s_max_t_cnt;	//�����t�������^�ɕύX
	extern volatile uint16_t s_pulse_l, s_pulse_r;

	extern volatile int16_t	dl, dr;
	extern volatile uint8_t d_mode;
	extern volatile uint8_t sp_mode;


#endif


#define drive_wait()	ms_wait(50)


/*============================================================
		�֐��v���g�^�C�v�錾
============================================================*/
void drive_init(void);
void drive_enable_motor(void);
void drive_disable_motor(void);
void drive_start(void);
void drive_stop(void);
void drive_set_dir(uint8_t);	//�i�ޕ����̐ݒ�

//====���s�n====
//----��֐�----
void driveA(uint16_t);			//�������s
void driveD(uint16_t);			//�������s
void driveU(uint16_t);			//�������s�i�O�̑��x���ێ��j
void driveC(uint16_t);			//�f�t�H���g�C���^�[�o���ő��s
void driveC_SLL(uint16_t);

//----��ʊ֐�----
void half_sectionA(void);		//���������
void half_sectionAA(void);		//���������
void half_sectionD(void);		//���������
void half_sectionDD(void);
void one_section(void);			//����������
void one_sectionU(void);		//��������
void one_sectionUU(void);
void rotate_R90(void);			//�E90��]
void rotate_L90(void);			//��90��]
void SLL_R90(void);
void SLL_L90(void);
void rotate_180(void);			//180�x��]
void set_position(uint8_t);		//�ʒu���킹
void start_position_setting(void);
#endif /* DRIVE_H_ */
