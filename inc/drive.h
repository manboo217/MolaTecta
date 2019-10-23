/*
 * drive.h
 *
 *  Created on: 2017/10/23
 *      Author: Blue
 */

#ifndef DRIVE_H_
#define DRIVE_H_

/*============================================================
		各種定数・変数宣言
============================================================*/

/* 励磁切り替えロジック
・SLA7078MPRT
 M1 | M2 | M3 |励磁モード
  L |  L |  L | 2相励磁（Mode8固定）
  L |  H |  L | 2相励磁（ModeF固定）
  H |  L |  L | 1-2相励磁（2分割）
  H |  H |  L | 1-2相励磁（ModeF固定）
  L |  L |  H |  W1-2相（ 4分割）
  L |  H |  H | 4W1-2相（16分割）
  H |  L |  H | 2W1-2相（ 8分割）
  H |  H |  H | スリープモード2

・SLA7073MPRT
 M1 | M2 | 励磁モード
  L |  L | 4W1-2相（16分割）
  L |  H | 2W1-2相（ 8分割）
  H |  L |  W1-2相（ 4分割）
  H |  H |   1-2相（ 2分割）
*/


#define FORWARD		0x00		//前進向き
#define BACK		0x11		//後退
#define ROTATE_L	0x01		//回転向き（左）
#define ROTATE_R	0x10		//回転向き（右）


//====変数====
#ifdef MAIN_C_										//main.cからこのファイルが呼ばれている場合
	/*グローバル変数の定義*/
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
	//table.hに貼り付けた値を保持する配列
	volatile int16_t t_cnt_l, t_cnt_r;				//テーブルカウンタ
	volatile int16_t min_t_cnt, max_t_cnt;			//テーブルカウンタの最低値・最大値
	volatile uint16_t pulse_l, pulse_r;				//左右パルスカウンタ
	volatile int16_t dl, dr;						//比例制御量
	//モード
	volatile uint8_t d_mode;
	volatile uint8_t sp_mode;

	/*slalom用*/										//table.hに貼り付けた値を保持する配列
	volatile int16_t sll_t_cnt_l, sll_t_cnt_r;				//テーブルカウンタ
	volatile int16_t s_min_t_cnt, s_max_t_cnt;			//テーブルカウンタの最低値・最大値
	volatile uint16_t s_pulse_l, s_pulse_r;				//左右パルスカウンタ

#else												//main.c以外からこのファイルが呼ばれている場合
	/*グローバル変数の宣言*/
	extern const uint16_t table1[];
	extern const uint16_t table2[];
	extern const uint16_t table3[];
//	extern const uint16_t table4[];
	extern const uint16_t table_ktkkn[];

	extern volatile int16_t t_cnt_l, t_cnt_r;		//符号付き整数型に変更
	extern volatile int16_t min_t_cnt, max_t_cnt;	//符号付き整数型に変更
	extern volatile uint16_t pulse_l, pulse_r;

	/*slalom用*/
	extern const uint16_t SLL_TABLE_IN1[];
	extern const uint16_t SLL_TABLE_OUT1[];
	extern const uint16_t SLL_TABLE_IN2[];
	extern const uint16_t SLL_TABLE_OUT2[];

	extern volatile int16_t sll_t_cnt_l, sll_t_cnt_r;		//符号付き整数型に変更
	extern volatile int16_t s_min_t_cnt, s_max_t_cnt;	//符号付き整数型に変更
	extern volatile uint16_t s_pulse_l, s_pulse_r;

	extern volatile int16_t	dl, dr;
	extern volatile uint8_t d_mode;
	extern volatile uint8_t sp_mode;


#endif


#define drive_wait()	ms_wait(50)


/*============================================================
		関数プロトタイプ宣言
============================================================*/
void drive_init(void);
void drive_enable_motor(void);
void drive_disable_motor(void);
void drive_start(void);
void drive_stop(void);
void drive_set_dir(uint8_t);	//進む方向の設定

//====走行系====
//----基幹関数----
void driveA(uint16_t);			//加速走行
void driveD(uint16_t);			//減速走行
void driveU(uint16_t);			//等速走行（前の速度を維持）
void driveC(uint16_t);			//デフォルトインターバルで走行
void driveC_SLL(uint16_t);

//----上位関数----
void half_sectionA(void);		//加速半区画
void half_sectionAA(void);		//加速半区画
void half_sectionD(void);		//減速半区画
void half_sectionDD(void);
void one_section(void);			//加減速一区画
void one_sectionU(void);		//等速一区画
void one_sectionUU(void);
void rotate_R90(void);			//右90回転
void rotate_L90(void);			//左90回転
void SLL_R90(void);
void SLL_L90(void);
void rotate_180(void);			//180度回転
void set_position(uint8_t);		//位置合わせ
void start_position_setting(void);
#endif /* DRIVE_H_ */
