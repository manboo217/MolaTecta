/*
 * search.h
 *
 *  Created on: 2017/10/25
 *      Author: Blue
 */

#ifndef SEARCH_H_
#define SEARCH_H_


/*============================================================
		�e��萔�E�ϐ��錾
============================================================*/
/**********
���H�̐�΍��W�ɂ��āC�X�^�[�g�n�_�������ɂȂ�悤�Ȉʒu���猩���Ƃ��C
�������k�C�E�����𓌁C���������C�������𐼂Ƃ��Ē�`���Ă���B
m_dir�̏ꍇ�C�k��0x00�C����0x01�C���0x02�C����0x03�ŕ\�����B�iturn_dir�֐��Q�Ɓj
�܂��C�}�b�v�i�[�z��map[][]�ɂ��āC�v�f�͏��4nit�Ɖ���4bit�ɕ�����Ă��āC
�k�ǂ�3bit�ځC���ǂ�2bit�ځC��ǂ�1bit�ځC���ǂ�0bit�ڂɁC
�ǂ�����ꍇ��1�C�Ȃ��ꍇ��0���i�[����Ă���B
����4bit��1�����s�p(���T���ǂ͖����Ɣ���)�ŏ��4bit��2�����s�p�i���T���ǂ͗L��Ɣ���j
��\���Ă���B�iwrite_map�֐������map_Init�֐��Q�Ɓj
�Ō�ɁC�ŒZ�o�H�i�[�z��route[]�ɂ��āC�i�s���Ɉړ����L�^����Ă���B
�e�v�f�ɋ@�̂��O�i����ۂ�0x88���C�E�܂���ۂ�0x44���CU�^�[������ۂ�0x22���C���܂���ꍇ��0x11���C
����ȊO�̏ꍇ�ɂ�0x00���i�[�����imake_route�֐��Q�Ɓj�B�Ȃ��C�i�s�̌o�߂�r_cnt�ŊǗ�����Ă���B
**********/


//----���ݒn�E���p�i�[�\����----
struct coordinate_and_direction{
	uint8_t x;
	uint8_t y;
	uint8_t dir;
};

#ifdef MAIN_C_							//main.c���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	/*�O���[�o���ϐ��̒�`*/
	volatile struct coordinate_and_direction mouse;
#else									//main.c�ȊO���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	/*�O���[�o���ϐ��̐錾*/
	extern volatile struct coordinate_and_direction mouse;
#endif


//----�����]���p�萔----
#define DIR_TURN_R90	0x01	//�E90�x��]
#define DIR_TURN_L90	0xff	//��90�x��]
#define DIR_TURN_180	0x02	//180�x��]


//====�ϐ�====
#ifdef MAIN_C_											//main.c���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	/*�O���[�o���ϐ��̒�`*/
	uint8_t map[16][16];								//�}�b�v�i�[�z��
	uint8_t smap[16][16];								//�����}�b�v�i�[�z��
	uint8_t wall_info;									//�Ǐ��i�[�ϐ�
	uint8_t goal_x, goal_y;								//�S�[�����W
	uint8_t route[256];									//�ŒZ�o�H�i�[�z��
	uint8_t r_cnt;										//�o�H�J�E���^
	uint16_t ketsuate_cnt;

#else													//main.c�ȊO���炱�̃t�@�C�����Ă΂�Ă���ꍇ
	/*�O���[�o���ϐ��̐錾*/
	extern uint8_t map[16][16];							//�}�b�v�i�[�z��
	extern uint8_t smap[16][16];						//�����}�b�v�i�[�z��
	extern uint8_t wall_info;							//�Ǐ��i�[�ϐ�
	extern uint8_t goal_x, goal_y;						//�S�[�����W
	extern uint8_t route[256];							//�ŒZ�o�H�i�[�z��
	extern uint8_t r_cnt;								//�o�H�J�E���^
	extern uint16_t ketsuate_cnt;
#endif


/*============================================================
		�֐��v���g�^�C�v�錾
============================================================*/
//====�T���n====

void search_init(void);

void searchA();											//1����~�^�T�����s
void searchB();											//�A���T�����s
void searchS();
void adv_pos();											//�}�E�X�̈ʒu����O�i
void conf_route();										//�����[�g�̊m�F
void map_Init();										//�}�b�v�f�[�^������
void write_map();										//�}�b�v��������
void turn_dir(uint8_t);									//���@�������ύX
void make_smap();										//�����}�b�v�쐬
void make_route();										//�ŒZ�o�H����

void store_map_in_eeprom(void);
void load_map_from_eeprom(void);

#endif /* SEARCH_H_ */
