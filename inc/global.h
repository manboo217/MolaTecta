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
		共用・構造体の定義
------------------------------------------------------------*/
/**********
共用・構造体とは，共用体と構造体を組み合わせたもので，
内部の一括操作も，メンバ単位での操作も可能なものである。
例えば，以下のmouse_flags共用・構造体のMFでは，
MF.FLAGS = 0;と全体を一括変更できるほか，
MF.FLAG.DECL = 1;とメンバを指定して単体で変更することも出来る。
**********/

//----フラグ共用・構造体----
typedef union {					//共用体の宣言
	uint16_t FLAGS;
	struct ms_flags{			//構造体の宣言
		uint16_t RSV0:1;		//予備ビット(B0)		(:1は1ビット分の意味，ビットフィールド)
		uint16_t SCND:1;		//二次走行フラグ(B1)
		uint16_t RSV2:1;		//予備ビット(B2)
		uint16_t CTRL:1;		//制御フラグ(B3)
		uint16_t ACCL:1;		//加速フラグ(B4)
		uint16_t DECL:1;		//減速フラグ(B5)
		uint16_t DEF:1;			//デフォルトインターバルフラグ(B6)
		uint16_t KTKKN:1;		//既知区間加速(B7)
		uint16_t RSV8:1;		//予備ビット(B8)
		uint16_t RSV9:1;		//予備ビット(B9)
		uint16_t SLL_L_UP:1;		//スラローム左ターン加速フラグ(B10)
		uint16_t SLL_L_DW:1;		//スラローム左ターン減速フラグ(B11)
		uint16_t SLL_R_UP:1;		//スラローム右ターン加速フラグ(B12)
		uint16_t SLL_R_DW:1;		//スラロ―ム右ターン減速フラグ(B13)
		uint16_t SLL_STR:1;		//スラローム直線フラグ(B14)
		uint16_t SPEED:1;		//スラロームスピード変更フラグ(B15)//廃止
	}FLAG;
} mouse_flags;

#ifdef MAIN_C_							//main.cからこのファイルが呼ばれている場合
	/*グローバル変数の定義*/
	volatile mouse_flags MF;			//マウスステータスをまとめた共用・構造体
#else									//main.c以外からこのファイルが呼ばれている場合
	/*グローバル変数の宣言*/
	extern volatile mouse_flags MF;
#endif


#include "main.h"
#include "eeprom.h"

#include "drive.h"
#include "sensor.h"
#include "search.h"

#include "pinassign.h"

#endif /* GLOBAL_H_ */
