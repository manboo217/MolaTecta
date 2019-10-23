/*
 * eeprom.c
 *
 *  Created on: 2017/11/01
 *      Author: Blue
 */
//�}�b�v�f�[�^�i�[


#include "global.h"

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_enable_write
// eeprom�Ƃ��Ďg��flash�������̈��erase���C�������݂�L���ɂ���
// �����F�Ȃ�
// �߂�l�F��肪�N����Ȃ����HAL_OK��Ԃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
HAL_StatusTypeDef eeprom_enable_write(void){
	HAL_StatusTypeDef status;
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError = 0;
	EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.PageAddress = EEPROM_START_ADDRESS;
	EraseInitStruct.NbPages = 1;
	status = HAL_FLASH_Unlock();
	if(status != HAL_OK) return status;
	status = HAL_FLASHEx_Erase(&EraseInitStruct, &PageError);
	return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_disable_write
// eeprom�Ƃ��Ďg��flash�������̈�ւ̏������݂𖳌��ɂ���
// �����F�Ȃ�
// �߂�l�F��肪�N����Ȃ����HAL_OK��Ԃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
HAL_StatusTypeDef eeprom_disable_write(void){
	return HAL_FLASH_Lock();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_write_halfword
// eeprom�Ƃ��Ďg��flash�������̈��2byte(16bit. 1byte=8bit=8��0/1)�̒l����������
// �����Faddress �c�c eeprom�Ƃ��Ďg��flash�������̈�̐擪�A�h���X����̃I�t�Z�b�g�Cdata �c�c �������݂���uin16_t�^�f�[�^
// �߂�l�F��肪�N����Ȃ����HAL_OK��Ԃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
HAL_StatusTypeDef eeprom_write_halfword(uint32_t address, uint16_t data){
	HAL_StatusTypeDef status;
	address = address*2 + EEPROM_START_ADDRESS;//2byte���I�t�Z�b�g���炷
	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, address, data);
	return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_write_word
// eeprom�Ƃ��Ďg��flash�������̈��4�o�C�g�̒l����������
// �����Faddress �c�c eeprom�Ƃ��Ďg��flash�������̈�̐擪�A�h���X����̃I�t�Z�b�g�Cdata �c�c �������݂���uin32_t�^�f�[�^
// �߂�l�F��肪�N����Ȃ����HAL_OK��Ԃ�
//+++++++++++++++++++++++++++++++++++++++++++++++
HAL_StatusTypeDef eeprom_write_word(uint32_t address, uint32_t data) {
	HAL_StatusTypeDef status;
	address = address*4 + EEPROM_START_ADDRESS;
	status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, address, data);
	return status;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_read_halfword
// eeprom�Ƃ��Ďg��flash�������̈悩��2�o�C�g�̒l��ǂݍ���
// �����Faddress �c�c eeprom�Ƃ��Ďg��flash�������̈�̐擪�A�h���X����̃I�t�Z�b�g
// �߂�l�F�ǂݍ���uin16_t�^�f�[�^
//+++++++++++++++++++++++++++++++++++++++++++++++
uint16_t eeprom_read_halfword(uint32_t address){
	uint16_t val = 0;
	address = address*2 + EEPROM_START_ADDRESS;
	val = *(__IO uint16_t *)address;
	return val;
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//eeprom_read_word
// eeprom�Ƃ��Ďg��flash�������̈悩��4�o�C�g�̒l��ǂݍ���
// �����Faddress �c�c eeprom�Ƃ��Ďg��flash�������̈�̐擪�A�h���X����̃I�t�Z�b�g
// �߂�l�F�ǂݍ���uin32_t�^�f�[�^
//+++++++++++++++++++++++++++++++++++++++++++++++
uint32_t eeprom_read_word(uint32_t address){
	uint32_t val = 0;
	address = address*4 + EEPROM_START_ADDRESS;
	val = *(__IO uint32_t*)address;
	return val;
}
