/*
 * search.c
 *
 *  Created on: 2017/10/25
 *      Author: Blue
 */

#include "global.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//search_init
// �T���n�̕ϐ��ƃ}�b�v�̏�����������
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void search_init(void){

	//----�T���n----
	goal_x = GOAL_X;        		//GOAL_X��global.h�Ƀ}�N����`����
	goal_y = GOAL_Y;        		//GOAL_Y��global.h�Ƀ}�N����`����
	map_Init();						//�}�b�v�̏�����
	mouse.x = 0;
	mouse.y = 0;					//���ݒn�̏�����
	mouse.dir = 0;					//�}�E�X�̌����̏�����
}


/*===========================================================
		�T���n�֐�
===========================================================*/
/*-----------------------------------------------------------
		�����@�T�����sB�i�A�����s�j
-----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//searchB
// �A�����s��goal���W�ɐi��
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void searchB(void){

	if(MF.FLAG.SCND){
		load_map_from_eeprom();
	}

	//====�X�^�[�g�ʒu�Ǐ��擾====
	get_wall_info();										//�Ǐ��̏�����, ��ǂ͂Ȃ��Ȃ�
	wall_info &= ~0x88;										//�O�ǂ͑��݂���͂����Ȃ��̂ō폜����
	write_map();											//�Ǐ���n�}�ɋL��

	//====�O�ɕǂ������z��Ŗⓚ���p�őO�i====
	half_sectionA();
	adv_pos();

	//====�����}�b�v�E�o�H�쐬====
	write_map();											//�Ǐ���n�}�ɋL��
	r_cnt = 0;												//�o�H�J�E���^�̏�����
	ketsuate_cnt = 0;
	make_smap();											//�����}�b�v�쐬
	make_route();											//�ŒZ�o�H�T���iroute�z��ɓ��삪�i�[�����j

	//====�T�����s====
	do{
		//----�i�s----
		switch(route[r_cnt++]){								//route�z��ɂ���Đi�s������B�o�H�J�E���^��i�߂�
			//----�O�i----
			case 0x88:
				one_sectionU();
				break;
			//----�E��----
			case 0x44:
				half_sectionD();
				rotate_R90();
				turn_dir(DIR_TURN_R90);
				half_sectionA();
				break;
			//----180��]----
			case 0x22:
				half_sectionD();
				turn_dir(DIR_TURN_180);
				if(wall_info & 0x88){
					if((wall_info & 0x44) && ketsuate_cnt > 8){
						rotate_L90();
						set_position(0);
						rotate_L90();
						ketsuate_cnt = 0;
					}
					else if((wall_info & 0x11) && ketsuate_cnt > 8){
						rotate_R90();
						set_position(0);
						rotate_R90();
						ketsuate_cnt = 0;
					}else{
						rotate_180();
						set_position(0);
					}
				}else{
					rotate_180();
				}
				half_sectionAA();
				break;

			//----����----
			case 0x11:
				half_sectionD();
				rotate_L90();
				turn_dir(DIR_TURN_L90);
				half_sectionA();
				break;
		}
		adv_pos();
		conf_route();
		ketsuate_cnt ++;
	}while((mouse.x != goal_x) || (mouse.y != goal_y));

	half_sectionD();

	ms_wait(2000);

	rotate_180();
	turn_dir(DIR_TURN_180);

	if( ! MF.FLAG.SCND){
		store_map_in_eeprom();
	}
}

/*-----------------------------------------------------------
		�����@�T�����sS�i�X�����[���A�����s�j
-----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//searchS
// �X�����[���A�����s��goal���W�ɐi��
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void searchS(void){

	if(MF.FLAG.SCND){
		load_map_from_eeprom();//2�����s�Ȃ�}�b�v��ǂݍ���
	}

	//====�X�^�[�g�ʒu�Ǐ��擾====
										//�Ǐ��̏�����, ��ǂ͂Ȃ��Ȃ�
	get_wall_info();										//�Ǐ��̏�����, ��ǂ͂Ȃ��Ȃ�
	wall_info &= ~0x88;										//�O�ǂ͑��݂���͂����Ȃ��̂ō폜����
	write_map();											//�Ǐ���n�}�ɋL��

	//====�O�ɕǂ������z��Ŗⓚ���p�őO�i====
	half_sectionA();
	adv_pos();

	//====�����}�b�v�E�o�H�쐬====
	write_map();											//�Ǐ���n�}�ɋL��
	r_cnt = 0;												//�o�H�J�E���^�̏�����
	ketsuate_cnt = 0;
	make_smap();											//�����}�b�v�쐬
	make_route();											//�ŒZ�o�H�T���iroute�z��ɓ��삪�i�[�����j

	//====�T�����s====
	do{
		//----�i�s----
		switch(route[r_cnt++]){								//route�z��ɂ���Đi�s������B�o�H�J�E���^��i�߂�
			//----�O�i----
			case 0x88:
				one_sectionU();
				break;
			//----�E��----
			case 0x44:
				SLL_R90();
				turn_dir(DIR_TURN_R90);
				break;
			//----180��]----
			case 0x22:
				half_sectionD();
				turn_dir(DIR_TURN_180);
				if((wall_info & 0x88)&& ketsuate_cnt >6){
					if((wall_info & 0x44)){
						rotate_L90();
						set_position(0);
						rotate_L90();
						set_position(0);
						ketsuate_cnt = 0;
					}
					else if((wall_info & 0x11) && ketsuate_cnt >6){
						rotate_R90();
						set_position(0);
						rotate_R90();
						set_position(0);
						ketsuate_cnt = 0;
					}
					else {
						rotate_180();
						set_position(0);
					}
				}
				else {
					rotate_180();
				}

				half_sectionAA();
				break;

			//----����----
			case 0x11:
				SLL_L90();
				turn_dir(DIR_TURN_L90);
				break;
		}
		adv_pos();
		conf_route();
		ketsuate_cnt ++;

	}while((mouse.x != goal_x) || (mouse.y != goal_y));

	half_sectionD();

	ms_wait(2000);

//	rotate_180();
	turn_dir(DIR_TURN_180);
	if(sp_mode == 2) sp_mode == 1;

	if(wall_info & 0x88){
				if((wall_info & 0x44)){
					rotate_L90();
					drive_wait();
					set_position(0);
					drive_wait();
					rotate_L90();
					}
				else if(wall_info & 0x11){
					rotate_R90();
					drive_wait();
					set_position(0);
					drive_wait();
					rotate_R90();
					}
				else {
					rotate_180();
				}
	}else{
			rotate_180();
		}
	if( ! MF.FLAG.SCND){
		store_map_in_eeprom();
	}

}

/*-----------------------------------------------------------
		�����@�T�����sSS�i�X�����[���E���m��ԉ������s�j
-----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//searchSS
// �X�����[���A�����s��goal���W�ɐi��
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void searchSS(void){

	if(MF.FLAG.SCND){
		load_map_from_eeprom();//2�����s�Ȃ�}�b�v��ǂݍ���
	}

	//====�X�^�[�g�ʒu�Ǐ��擾====
	get_wall_info();										//�Ǐ��̏�����, ��ǂ͂Ȃ��Ȃ�
	wall_info &= ~0x88;										//�O�ǂ͑��݂���͂����Ȃ��̂ō폜����
	write_map();											//�Ǐ���n�}�ɋL��

	//====�O�ɕǂ������z��Ŗⓚ���p�őO�i====
	half_sectionA();
	adv_pos();

	//====�����}�b�v�E�o�H�쐬====
	write_map();											//�Ǐ���n�}�ɋL��
	r_cnt = 0;												//�o�H�J�E���^�̏�����
	ketsuate_cnt = 0;
	make_smap();											//�����}�b�v�쐬
	make_route();											//�ŒZ�o�H�T���iroute�z��ɓ��삪�i�[�����j

	//====�T�����s====
	do{
		//----�i�s----
		switch(route[r_cnt++]){								//route�z��ɂ���Đi�s������B�o�H�J�E���^��i�߂�
			//----�O�i----
			case 0x88:
				one_sectionU();
				break;
			//----�E��----
			case 0x44:
				SLL_R90();
				turn_dir(DIR_TURN_R90);
				break;
			//----180��]----
			case 0x22:
				half_sectionD();
				turn_dir(DIR_TURN_180);
				if((wall_info & 0x88)&& ketsuate_cnt >8){
					if((wall_info & 0x44)){
						rotate_L90();
						set_position(0);
						rotate_L90();
						set_position(0);
						ketsuate_cnt = 0;
					}
					else if((wall_info & 0x11) && ketsuate_cnt >8){
						rotate_R90();
						set_position(0);
						rotate_R90();
						set_position(0);
						ketsuate_cnt = 0;
					}
					else {
						rotate_180();
						set_position(0);
					}
				}
				else {
					rotate_180();
				}
				half_sectionAA();
				break;

			//----����----
			case 0x11:
				SLL_L90();
				turn_dir(DIR_TURN_L90);
				break;
		}
		adv_pos();
		conf_route();
		ketsuate_cnt ++;

	}while((mouse.x != goal_x) || (mouse.y != goal_y));

	half_sectionD();

	ms_wait(2000);

	rotate_180();
	turn_dir(DIR_TURN_180);

	if( ! MF.FLAG.SCND){
		store_map_in_eeprom();
	}

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//adv_pos
// �}�C�N���}�E�X�����ʒu���őO�i������
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void adv_pos(){

	switch(mouse.dir){										//�}�C�N���}�E�X�����݌����Ă�������Ŕ���
	case 0x00:												//�k�����Ɍ����Ă���ꍇ
		mouse.y++;											//Y���W���C���N�������g
		break;
	case 0x01:												//�������Ɍ����Ă���ꍇ
		mouse.x++;											//X���W���C���N�������g
		break;
	case 0x02:												//������Ɍ����Ă���ꍇ
		mouse.y--;											//Y���W���f�N�������g
		break;
	case 0x03:												//�������Ɍ����Ă���ꍇ
		mouse.x--;											//X���W���f�N�������g
		break;
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//conf_route
// �i�H�𔻒肷��
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void conf_route(){

	//----�Ǐ�񏑂�����----
	write_map();

	//----�ŒZ�o�H��ɕǂ�����ΐi�H�ύX----
	if(wall_info & route[r_cnt]){
		make_smap();										//�����}�b�v���X�V
		make_route();										//�ŒZ�o�H���X�V
		r_cnt = 0;											//�o�H�J�E���^��0��
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//map_Init
// �}�b�v�i�[�z��map[][]�̏�����������
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void map_Init(){

	//====�ϐ��錾====
	uint8_t x, y;											//for���p�ϐ�

	//====�������J�n====
	//�}�b�v�̃N���A
	for(y = 0; y < 16; y++){								//�eY���W�Ŏ��s
		for(x = 0; x < 16; x++){							//�eX���W�Ŏ��s
			map[y][x] = 0xf0;								//���4�r�b�g�i2�����s���j��ǂ���C����4�r�b�g�i1�����s���j��ǂȂ��Ƃ���B
		}
	}

	//�m��ǂ̔z�u
	for(y = 0; y < 16; y++){								//�eY���W�Ŏ��s
		map[y][0] |= 0xf1;									//�Ő��ɕǂ�z�u
		map[y][15] |= 0xf4;									//�œ��ɕǂ�z�u
	}
	for(x = 0; x < 16; x++){								//�eX���W�Ŏ��s
		map[0][x] |= 0xf2;									//�œ�ɕǂ�z�u
		map[15][x] |= 0xf8;									//�Ŗk�ɕǂ�z�u
	}
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//write_map
// �}�b�v�f�[�^����������
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void write_map(){

	//====�ϐ��錾====
	uint8_t m_temp;											//������␳�����Ǐ��

	//====�Ǐ��̕␳�i�[====
	m_temp = (wall_info >> mouse.dir) & 0x0f;				//�Z���T�Ǐ���mouse.dir�Ō�����␳�����ĉ���4bit�����c��
	m_temp |= (m_temp << 4);								//���4bit�ɉ���4bit���R�s�[�B���̍�Ƃ�m_temp��NESW���ŕǂ��i�[

	//====�f�[�^�̏�������====
	map[mouse.y][mouse.x] = m_temp; 						//���ݒn�ɕǏ�񏑂�����
	//----���ӂɏ�������----
	//�k���ɂ���
	if(mouse.y != 15){										//���ݍŖk�[�łȂ��Ƃ�
		if(m_temp & 0x88){									//�k�ǂ�����ꍇ
			map[mouse.y + 1][mouse.x] |= 0x22;				//�k���̋�悩�猩�ē�ǂ������������
		}else{												//�k�ǂ��Ȃ��ꍇ
			map[mouse.y + 1][mouse.x] &= 0xDD;				//�k���̋�悩�猩�ē�ǂȂ�����������
		}
	}
	//�����ɂ���
	if(mouse.x != 15){										//���ݍœ��[�łȂ��Ƃ�
		if(m_temp & 0x44){									//���ǂ�����ꍇ
			map[mouse.y][mouse.x + 1] |= 0x11;				//�����̋�悩�猩�Đ��ǂ������������
		}else{												//�k�ǂ��Ȃ��ꍇ
			map[mouse.y][mouse.x + 1] &= 0xEE;				//�����̋�悩�猩�Đ��ǂȂ�����������
		}
	}
	//��ǂɂ���
	if(mouse.y != 0){										//���ݍœ�[�łȂ��Ƃ�
		if(m_temp & 0x22){									//��ǂ�����ꍇ
			map[mouse.y - 1][mouse.x] |= 0x88;				//�쑤�̋�悩�猩�Ėk�ǂ������������
		}else{												//��ǂ��Ȃ��ꍇ
			map[mouse.y - 1][mouse.x] &= 0x77;				//�쑤�̋�悩�猩�Ėk�ǂȂ�����������
		}
	}
	//�����ɂ���
	if(mouse.x != 0){										//���ݍŐ��[�łȂ��Ƃ�
		if(m_temp & 0x11){									//���ǂ�����ꍇ
			map[mouse.y][mouse.x - 1] |= 0x44;				//�����̋�悩�猩�ē��ǂ������������
		}else{												//���ǂ��Ȃ��ꍇ
			map[mouse.y][mouse.x - 1] &= 0xBB;				//�����̋�悩�猩�ē����Ȃ�����������
		}
	}
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//write_map
// �}�E�X�̕�����ύX����
// ����1�Ft_pat �c�c ��]����(drive.h�Ń}�N����`)
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void turn_dir(uint8_t t_pat){

	//====������ύX====
	mouse.dir = (mouse.dir + t_pat) & 0x03;					//�w�肳�ꂽ��mouse.dir����]������
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//make_smap
// �����}�b�v���쐬����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void make_smap(void){

	//====�ϐ��錾====
	uint8_t x, y;											//for���p�ϐ�

	//====�����}�b�v�̃N���A====
	for(y = 0; y <= 15; y++){								//�eY���W�Ŏ��s
		for( x = 0; x <= 15; x++){							//�eX���W�Ŏ��s
			smap[y][x] = 0xff;								//���L�������͕����ő�Ƃ���
		}
	}

	//====�S�[�����W��0�ɂ���====
	uint8_t m_step = 0;										//�����J�E���^��0�ɂ���
	smap[goal_y][goal_x] = 0;

	//====�����̍��W�ɂ��ǂ蒅���܂Ń��[�v====
	do{
		//----�}�b�v�S���{��----
		for( y = 0; y <= 15; y++){							//�eY���W�Ŏ��s
			for( x = 0; x <= 15; x++){						//�eX���W�Ŏ��s
				//----���ݍő�̕����𔭌������Ƃ�----
				if( smap[y][x] == m_step){					//�����J�E���^m_step�̒l�����ݍő�̕���
					uint8_t m_temp = map[y][x];				//map�z�񂩂�}�b�v�f�[�^�����o��
					if(MF.FLAG.SCND){						//�񎟑��s�p�̃}�b�v���쐬����ꍇ�i�񎟑��s����MF.FLAG.SCND�������Ă���j
						m_temp >>= 4;						//���4bit���g���̂�4bit���E�ɃV�t�g������
					}
					//----�k�ǂɂ��Ă̏���----
					if(!(m_temp & 0x08) && y != 15){		//�k�ǂ��Ȃ����ݍŖk�[�łȂ��Ƃ�
						if(smap[y+1][x] == 0xff){			//�k�������L���Ȃ�
							smap[y+1][x] = m_step + 1;		//���̕�������������
						}
					}
					//----���ǂɂ��Ă̏���----
					if(!(m_temp & 0x04) && x != 15){		//���ǂ��Ȃ����ݍœ��[�łȂ��Ƃ�
						if(smap[y][x+1] == 0xff){			//���������L���Ȃ�
							smap[y][x+1] = m_step + 1;		//���̕�������������
						}
					}
					//----��ǂɂ��Ă̏���----
					if(!(m_temp & 0x02) && y != 0){			//��ǂ��Ȃ����ݍœ�[�łȂ��Ƃ�
						if(smap[y-1][x] == 0xff){			//�쑤�����L���Ȃ�
							smap[y-1][x] = m_step + 1;		//���̕�������������
						}
					}
					//----���ǂɂ��Ă̏���----
					if(!(m_temp & 0x01) && x != 0){			//���ǂ��Ȃ����ݍŐ��[�łȂ��Ƃ�
						if(smap[y][x-1] == 0xff){			//���������L���Ȃ�
							smap[y][x-1] = m_step + 1;		//���̕�������������
						}
					}
				}
			}
		}
		//====�����J�E���^�̃C���N�������g====
		m_step++;
	}while(smap[mouse.y][mouse.x] == 0xff);					//���ݍ��W�����L���ł͂Ȃ��Ȃ�܂Ŏ��s
}



//+++++++++++++++++++++++++++++++++++++++++++++++
//make_route
// �ŒZ�o�H�𓱏o����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void make_route(){

	//====�ϐ��錾====
	uint8_t x, y;												//X�CY���W
	uint8_t dir_temp =  mouse.dir;								//�}�E�X�̕��p��\��mouse.dir�̒l��dir_temp�ϐ��ɑޔ�������

	//====�ŒZ�o�H��������====
	uint16_t i;
	for(i = 0; i < 256; i++){
		route[i] = 0xff;											//route��0xff�ŏ�����
	}

	//====�����J�E���^���Z�b�g====
	uint8_t m_step = smap[mouse.y][mouse.x];					//���ݍ��W�̕����}�b�v�l���擾

	//====x, y�Ɍ��ݍ��W����������====
	x = mouse.x;
	y = mouse.y;

	//====�ŒZ�o�H�𓱏o====
	i = 0;
	do{
		uint8_t m_temp = map[y][x];								//��r�p�}�b�v���̊i�[
		if(MF.FLAG.SCND){										//�񎟑��s�p�̃}�b�v���쐬����ꍇ�i�񎟑��s����MF.FLAG.SCND�������Ă���j
			m_temp >>= 4;										//���4bit���g���̂�4bit���E�ɃV�t�g������
		}

		//----�k������----
		if(!(m_temp & 0x08) && (smap[y+1][x] < m_step)){		//�k���ɕǂ������A���ݒn��菬���������}�b�v�l�ł����
			route[i] = (0x00 - mouse.dir) & 0x03;				//route�z��ɐi�s�������L�^
			m_step = smap[y+1][x];								//�ő�����}�b�v�l���X�V
			y++;												//�k�ɐi�񂾂̂�Y���W���C���N�������g
		}

		//----��������----
		else if(!(m_temp & 0x04) && (smap[y][x+1] < m_step)){	//�����ɕǂ������A���ݒn��菬���������}�b�v�l�ł����
			route[i] = (0x01 - mouse.dir) & 0x03;				//route�z��ɐi�s�������L�^
			m_step = smap[y][x+1];								//�ő�����}�b�v�l���X�V
			x++;												//���ɐi�񂾂̂�X���W���C���N�������g
		}

		//----�������----
		else if(!(m_temp & 0x02) && (smap[y-1][x] < m_step)){	//�쑤�ɕǂ������A���ݒn��菬���������}�b�v�l�ł����
			route[i] = (0x02 - mouse.dir) & 0x03;				//route�z��ɐi�s�������L�^
			m_step = smap[y-1][x];								//�ő�����}�b�v�l���X�V
			y--;												//��ɐi�񂾂̂�Y���W���f�N�������g
		}
		//----��������----
		else if(!(m_temp & 0x01) && (smap[y][x-1] < m_step)){	//�����ɕǂ������A���ݒn��菬���������}�b�v�l�ł����
			route[i] = (0x03 - mouse.dir) & 0x03;				//route�z��ɐi�s�������L�^
			m_step = smap[y][x-1];								//�ő�����}�b�v�l���X�V
			x--;												//���ɐi�񂾂̂�X���W���f�N�������g
		}

		//----�i�[�f�[�^�`���ύX----
		switch(route[i]){										//route�z��Ɋi�[�����v�f�l�ŕ���
		case 0x00:												//�O�i����ꍇ
			route[i] = 0x88;									//�i�[�f�[�^�`����ύX
			break;
		case 0x01:												//�E�܂���ꍇ
			turn_dir(DIR_TURN_R90);								//�������̕�����90�x�E��]
			route[i] = 0x44;									//�i�[�f�[�^�`����ύX
			break;
		case 0x02:												//U�^�[������ꍇ
			turn_dir(DIR_TURN_180);								//�������̕�����180�x��]
			route[i] = 0x22;									//�i�[�f�[�^�`����ύX
			break;
		case 0x03:												//���܂���ꍇ
			turn_dir(DIR_TURN_L90);								//�������̕�����90�x�E��]
			route[i] = 0x11;									//�i�[�f�[�^�`����ύX
			break;
		default:												//����ȊO�̏ꍇ
			route[i] = 0x00;									//�i�[�f�[�^�`����ύX
			break;
		}
		i++;													//�J�E���^���C���N�������g
	}while( smap[y][x] != 0);									//�i�񂾐�̕����}�b�v�l��0�i=�S�[���j�ɂȂ�܂Ŏ��s
	mouse.dir = dir_temp;										//dir_temp�ɑޔ��������l��mouse.dir�Ƀ��X�g�A
}




//+++++++++++++++++++++++++++++++++++++++++++++++
//store_map_in_eeprom
// map�f�[�^��eeprom�Ɋi�[����
// �����F�Ȃ�
// �߂�l�F�Ȃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
void store_map_in_eeprom(void){
	eeprom_enable_write();
	int i;
	for(i = 0; i < 16; i++){
		int j;
		for(j = 0; j < 16; j++){
			eeprom_write_halfword(i*16 + j, (uint16_t) map[i][j]);
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
void load_map_from_eeprom(void){
	int i;
	for(i = 0; i < 16; i++){
		int j;
		for(j = 0; j < 16; j++){
			map[i][j] = (uint8_t) eeprom_read_halfword(i*16 + j);
		}
	}
}
