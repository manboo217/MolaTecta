/*
 * drive.c
 *
 *  Created on: 2017/10/23
 *      Author: Blue
 */

#include "global.h"


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_init
// 走行系の変数の初期化，モータードライバ関係のGPIO設定とPWM出力に使うタイマの設定をする
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_init(void){

	//====走行系の変数の初期化====
	max_t_cnt = MAX_T_CNT;			//テーブルカウンタ最高値初期化     MAX_T_CNTはparams.hにマクロ定義あり
	min_t_cnt = MIN_T_CNT;			//テーブルカウンタ最低値初期化     MIN_T_CNTはparams.hにマクロ定義あり
	MF.FLAGS = 0;					//フラグクリア
	//====モータードライバ関係のGPIO設定====
	pin_dir(PIN_M3, OUT);
	pin_dir(PIN_CW_R, OUT);
	pin_dir(PIN_CW_L, OUT);
	pin_dir(PIN_REF, OUT);
	pin_write(PIN_M3, HIGH);		//ステッピングモータ励磁OFF
	drive_set_dir(FORWARD);			//前進するようにモータの回転方向を設定
	//====PWM出力に使うタイマの設定====
	/*--------------------------------------------------------------------
		TIM16 : 16ビットタイマ。左モータの制御に使う。出力はTIM16_CH1(PB4)
	--------------------------------------------------------------------*/
	__HAL_RCC_TIM16_CLK_ENABLE();

	TIM16->CR1 = 0;						//タイマ無効
	TIM16->CR2 = 0;
	TIM16->DIER = TIM_DIER_UIE;			//タイマ更新割り込みを有効に
	TIM16->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;	//PWMモード1
	TIM16->CCER = TIM_CCER_CC1E;		//TIM16_CH1出力をアクティブHighに
	TIM16->BDTR = TIM_BDTR_MOE;			//PWM出力を有効に
	TIM16->CNT = 0;						//タイマカウンタ値を0にリセット
	TIM16->PSC = 63;					//タイマのクロック周波数をシステムクロック/64=1MHzに設定
	TIM16->ARR = DEFAULT_INTERVAL;		//タイマカウンタの上限値。取り敢えずDEFAULT_INTERVAL(params.h)に設定
	TIM16->CCR1 = 25;					//タイマカウンタの比較一致値
	TIM16->EGR = TIM_EGR_UG;			//タイマ設定を反映させるためにタイマ更新イベントを起こす

	NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);			//タイマ更新割り込みハンドラを有効に
	NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 2);	//タイマ更新割り込みの割り込み優先度を設定

	pin_set_alternate_function(PB4, 1);			//PB4 : TIM16_CH1はAF1に該当

	/*--------------------------------------------------------------------
		TIM17 : 16ビットタイマ。右モータの制御に使う。出力はTIM17_CH1(PB5)
	--------------------------------------------------------------------*/
	__HAL_RCC_TIM17_CLK_ENABLE();

	TIM17->CR1 = 0;						//タイマ無効
	TIM17->CR2 = 0;
	TIM17->DIER = TIM_DIER_UIE;			//タイマ更新割り込みを有効に
	TIM17->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1PE;	//PWMモード1
	TIM17->CCER = TIM_CCER_CC1E;		//TIM16_CH1出力をアクティブHighに
	TIM17->BDTR = TIM_BDTR_MOE;			//PWM出力を有効に
	TIM17->CNT = 0;						//タイマカウンタ値を0にリセット
	TIM17->PSC = 63;					//タイマのクロック周波数をシステムクロック/64=1MHzに設定
	TIM17->ARR = DEFAULT_INTERVAL;		//タイマカウンタの上限値。取り敢えずDEFAULT_INTERVAL(params.h)に設定
	TIM17->CCR1 = 25;					//タイマカウンタの比較一致値
	TIM17->EGR = TIM_EGR_UG;			//タイマ設定を反映させるためにタイマ更新イベントを起こす

	NVIC_EnableIRQ(TIM1_TRG_COM_TIM17_IRQn);			//タイマ更新割り込みハンドラを有効に
	NVIC_SetPriority(TIM1_TRG_COM_TIM17_IRQn, 2);		//タイマ更新割り込みの割り込み優先度を設定

	pin_set_alternate_function(PB5, 10);	//PB5 : TIM17_CH1はAF10に該当
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_enable_motor
// ステッピングモータを励磁する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_enable_motor(void){
	pin_write(PIN_REF, HIGH);
	pin_write(PIN_M3, LOW);			//ステッピングモータ励磁ON
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_disable_motor
// ステッピングモータの励磁を切る
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_disable_motor(void){
	pin_write(PIN_M3, HIGH);		//ステッピングモータ励磁OFF
}

/*----------------------------------------------------------
		16bitタイマ割り込み
----------------------------------------------------------*/
/**********
---走行の仕組み---
ステッピングモータの動作制御は16bitタイマで行われている。各16bitタイマの設定は，
・カウント開始からCCR1までの間は出力ピンがLowになる
・CCR1に達すると出力ピンがHighになる
・ARRに達すると割り込みを生成+タイマカウンタをリセット
となっている（drive_init函数参照）
モータドライバの（RefをHighにした状態で）ClockをHighにすることで一定角度回転し，
Lowに戻した後またHighにすることでまた一定角度回転する。
Clockにはタイマの出力ピンを繋いであるので，タイマの周期を速くするほど回転も速くなる。
このプログラムではカウント開始からCCR1の間を一定にして（モータドライバがHighを認識する分長さが必要），
カウント開始からARRの間を調整することで速度を変化させている。
加減速等の状態はMF.FLAG構造体（global.hで定義）で管理されている。
加減速について，事前にExcelで計算した時間（割り込み）ごとのARRの値をtable[]配列に記録しておく。
（配列の値はtable.hにExcelからコピー&ペーストをして設定する。table[]の定義はdrive.h参照）
今加減速のどの段階なのか（table[]の要素番号・インデックス）はt_cnt_l,t_cnt_rで記録している。
**********/

//+++++++++++++++++++++++++++++++++++++++++++++++
//TIM1_UP_TIM16_IRQHandler
// 16ビットタイマーTIM16の割り込み関数，左モータの管理を行う
// 引数：無し
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void TIM1_UP_TIM16_IRQHandler(){

	if( !(TIM16->SR & TIM_SR_UIF) ){
		return;
	}
	pulse_l++;															//左パルスのカウンタをインクリメント
	//====加減速処理====
	//----減速処理----
	if(sp_mode >= 1){
		if(MF.FLAG.DECL){													//減速フラグが立っている場合
			t_cnt_l = max(t_cnt_l - 1, min_t_cnt);
		}
	//----加速処理----
		else if(MF.FLAG.ACCL){												//加速フラグが立っている場合
			t_cnt_l = min(t_cnt_l + 1, max_t_cnt);
		}

	//----デフォルトインターバル----
		if(MF.FLAG.DEF){													//デフォルトインターバルフラグが立っている場合
			TIM16->ARR = DEFAULT_INTERVAL - dl;								//デフォルトインターバルに制御を加えた値に設定
		}

	/******SLALOM*******/
	//左ターン最初
		else if(MF.FLAG.SLL_L_UP){
			if(sll_t_cnt_l<SLL_T_CNT1){
				sll_t_cnt_l ++;
			}
			TIM16->ARR = SLL_TABLE_IN1[sll_t_cnt_l];
		}

	//左ターン最後
		else if(MF.FLAG.SLL_L_DW){
			if(sll_t_cnt_l<SLL_T_CNT1){
				sll_t_cnt_l ++;
			}
			TIM16->ARR = SLL_TABLE_IN1[SLL_T_CNT1 - sll_t_cnt_l];
			}

	//右ターン最初
		else if(MF.FLAG.SLL_R_UP){
			if(sll_t_cnt_l<SLL_T_CNT1){
				sll_t_cnt_l ++;
			}
			TIM16->ARR = SLL_TABLE_OUT1[sll_t_cnt_l];
		}
	//右ターン最後
		else if(MF.FLAG.SLL_R_DW){
			if(sll_t_cnt_l<SLL_T_CNT1){
				sll_t_cnt_l ++;
			}
			TIM16->ARR = SLL_TABLE_OUT1[SLL_T_CNT1 - sll_t_cnt_l];
		}
		else if(MF.FLAG.SLL_STR){
			TIM16->ARR = SLL_INTERVAL1;
		}

	//----それ以外の時はテーブルカウンタの指し示すインターバル----
		else {
			switch(sp_mode){
				case 0:		TIM16->ARR = table2[t_cnt_l] - dl;	break;					//スラローム2
				case 1:		TIM16->ARR = table1[t_cnt_l] - dl;	break;					//以下スラローム1
				case 2:		TIM16->ARR = table3[t_cnt_l] - dl;	break;
				case 3:		TIM16->ARR = table1[t_cnt_l] - dl;	break;					//以下スラローム1
				case 4:		TIM16->ARR = table3[t_cnt_l] - dl;  break;
				case 5:		TIM16->ARR = table1[t_cnt_l] - dl;	break;					//以下スラローム1
				default:	TIM16->ARR = table2[t_cnt_l] - dl;	break;
				}
		}
	}

	if(sp_mode == 0){
			if(MF.FLAG.DECL){													//減速フラグが立っている場合
				t_cnt_l = max(t_cnt_l - 1, min_t_cnt);
			}
		//----加速処理----
			else if(MF.FLAG.ACCL){												//加速フラグが立っている場合
				t_cnt_l = min(t_cnt_l + 1, max_t_cnt);
			}

		//----デフォルトインターバル----
			if(MF.FLAG.DEF){													//デフォルトインターバルフラグが立っている場合
				TIM16->ARR = DEFAULT_INTERVAL - dl;								//デフォルトインターバルに制御を加えた値に設定
			}

		/******SLALOM*******/

		//左ターン最初
			else if(MF.FLAG.SLL_L_UP){
				if(sll_t_cnt_l<SLL_T_CNT2){
					sll_t_cnt_l ++;
				}
				TIM16->ARR = SLL_TABLE_IN2[sll_t_cnt_l];
			}

		//左ターン最後
			else if(MF.FLAG.SLL_L_DW){
				if(sll_t_cnt_l<SLL_T_CNT2){
					sll_t_cnt_l ++;
				}
				TIM16->ARR = SLL_TABLE_IN2[SLL_T_CNT2 - sll_t_cnt_l];
				}

		//右ターン最初
			else if(MF.FLAG.SLL_R_UP){
				if(sll_t_cnt_l<SLL_T_CNT2){
					sll_t_cnt_l ++;
				}
				TIM16->ARR = SLL_TABLE_OUT2[sll_t_cnt_l];
			}
		//右ターン最後
			else if(MF.FLAG.SLL_R_DW){
				if(sll_t_cnt_l<SLL_T_CNT2){
					sll_t_cnt_l ++;
				}
				TIM16->ARR = SLL_TABLE_OUT2[SLL_T_CNT2 - sll_t_cnt_l];
			}
			else if(MF.FLAG.SLL_STR){
				TIM16->ARR = SLL_INTERVAL2;
			}

		//----それ以外の時はテーブルカウンタの指し示すインターバル----
			else {
				TIM16->ARR = table2[t_cnt_l] - dl;								//左モータインターバル設定
			}
		}

		TIM16->SR &= ~TIM_SR_UIF;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//TIM1_TRG_CON_TIM17_IRQHandler
// 16ビットタイマーTIM17の割り込み関数，右モータの管理を行う
// 引数：無し
// 戻り値：無し
//+++++++++++++++++++++++++++++++++++++++++++++++
void TIM1_TRG_COM_TIM17_IRQHandler(){

	if( !(TIM17->SR & TIM_SR_UIF) ){
		return;
	}
	pulse_r++;															//右パルスのカウンタをインクリメント

	if(sp_mode >= 1){
	//====加減速処理====
	//----減速処理----
		if(MF.FLAG.DECL){													//減速フラグが立っている場合
			t_cnt_r = max(t_cnt_r - 1, min_t_cnt);
		}
	//----加速処理----
		else if(MF.FLAG.ACCL){												//加速フラグが立っている場合
			t_cnt_r = min(t_cnt_r + 1, max_t_cnt);
		}

	//----デフォルトインターバル----
		if(MF.FLAG.DEF){													//デフォルトインターバルフラグが立っている場合
			TIM17->ARR = DEFAULT_INTERVAL - dr;								//デフォルトインターバルに制御を加えた値に設定
		}

	/******SLALOM*******/

	//左ターン 最初
		else if(MF.FLAG.SLL_L_UP){
			if(sll_t_cnt_r<SLL_T_CNT1){
				sll_t_cnt_r ++;
			}
			TIM17->ARR = SLL_TABLE_OUT1[sll_t_cnt_r];
		}

	//左ターン最後
		else if(MF.FLAG.SLL_L_DW){
			if(sll_t_cnt_r<SLL_T_CNT1){
				sll_t_cnt_r ++;
			}
			TIM17->ARR = SLL_TABLE_OUT1[SLL_T_CNT1 - sll_t_cnt_r];
		}

	//右ターン最初
		else if(MF.FLAG.SLL_R_UP){
			if(sll_t_cnt_r<SLL_T_CNT1){
				sll_t_cnt_r ++;
			}
			TIM17->ARR = SLL_TABLE_IN1[sll_t_cnt_r];
		}
	//右ターン最後
		else if(MF.FLAG.SLL_R_DW){
			if(sll_t_cnt_r<SLL_T_CNT1){
				sll_t_cnt_r ++;
			}
			TIM17->ARR = SLL_TABLE_IN1[SLL_T_CNT1 - sll_t_cnt_r];
		}
		else if(MF.FLAG.SLL_STR){
			TIM17->ARR = SLL_INTERVAL1;
		}

	//----それ以外の時はテーブルカウンタの指し示すインターバル----
		else {
			switch(sp_mode){
			case 0:		TIM17->ARR = table2[t_cnt_r] - dr;	break;
			case 1:		TIM17->ARR = table1[t_cnt_r] - dr;	break;					//左モータインターバル設定
			case 2:		TIM17->ARR = table3[t_cnt_r] - dr;	break;
			case 3: 	TIM17->ARR = table1[t_cnt_r] - dr;	break;
			case 4:		TIM17->ARR = table3[t_cnt_r] - dr;  break;
			case 5: 	TIM17->ARR = table1[t_cnt_r] - dr;	break;
			default:	TIM17->ARR = table2[t_cnt_r] - dr;	break;
			}
		}
	}
		else if(sp_mode == 0){
			//====加減速処理====
			//----減速処理----
				if(MF.FLAG.DECL){													//減速フラグが立っている場合
					t_cnt_r = max(t_cnt_r - 1, min_t_cnt);
				}
			//----加速処理----
				else if(MF.FLAG.ACCL){												//加速フラグが立っている場合
					t_cnt_r = min(t_cnt_r + 1, max_t_cnt);
				}

			//----デフォルトインターバル----
				if(MF.FLAG.DEF){													//デフォルトインターバルフラグが立っている場合
					TIM17->ARR = DEFAULT_INTERVAL - dr;								//デフォルトインターバルに制御を加えた値に設定
				}

			/******SLALOM*******/

			//左ターン 最初
				else if(MF.FLAG.SLL_L_UP){
					if(sll_t_cnt_r<SLL_T_CNT2){
						sll_t_cnt_r ++;
					}
					TIM17->ARR = SLL_TABLE_OUT2[sll_t_cnt_r];
				}

			//左ターン最後
				else if(MF.FLAG.SLL_L_DW){
					if(sll_t_cnt_r<SLL_T_CNT2){
						sll_t_cnt_r ++;
					}
					TIM17->ARR = SLL_TABLE_OUT2[SLL_T_CNT2 - sll_t_cnt_r];
				}

			//右ターン最初
				else if(MF.FLAG.SLL_R_UP){
					if(sll_t_cnt_r<SLL_T_CNT2){
						sll_t_cnt_r ++;
					}
					TIM17->ARR = SLL_TABLE_IN2[sll_t_cnt_r];
				}
			//右ターン最後
				else if(MF.FLAG.SLL_R_DW){
					if(sll_t_cnt_r<SLL_T_CNT2){
						sll_t_cnt_r ++;
					}
					TIM17->ARR = SLL_TABLE_IN2[SLL_T_CNT2 - sll_t_cnt_r];
				}
				else if(MF.FLAG.SLL_STR){
					TIM17->ARR = SLL_INTERVAL2;
				}

			//----それ以外の時はテーブルカウンタの指し示すインターバル----
				else {
					TIM17->ARR = table2[t_cnt_r] - dr;
				}
		}

		TIM17->SR &= ~TIM_SR_UIF;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_reset_t_cnt
// テーブルカウンタをリセット（min_t_cntの値にセット）する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_reset_t_cnt(void){
	t_cnt_l = t_cnt_r = min_t_cnt;
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_start
// 走行を開始する
// （pulse_l,pulse_rを0にリセットしてタイマを有効にする）
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_start(void){

	pulse_l = pulse_r = 0;		//走行したパルス数の初期化
	TIM16->CR1 |= TIM_CR1_CEN;	// Enable timer
	TIM17->CR1 |= TIM_CR1_CEN;	// Enable timer
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_stop
// 走行を終了する
// （タイマを止めてタイマカウント値を0にリセットする）
// 引数1：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_stop(void){

	TIM16->CR1 &= ~TIM_CR1_CEN;	// Disable timer
	TIM17->CR1 &= ~TIM_CR1_CEN;	// Disable timer
	TIM16->CNT = 0;				// Reset Counter
	TIM17->CNT = 0;				// Reset Counter
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_set_dir
// 進行方向を設定する
// 引数1：d_dir …… どの方向に進行するか  0桁目で左，1桁目で右の方向設定
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void drive_set_dir(uint8_t d_dir){

	//====左モータ====
	switch(d_dir & 0x0f){									//0~3ビット目を取り出す
		//----正回転----
		case 0x00:											//0x00の場合
			pin_write(PIN_CW_L, MT_FWD_L);					//左を前進方向に設定
			break;
		//----逆回転----
		case 0x01:								 			//0x01の場合
			pin_write(PIN_CW_L, MT_BACK_L);					//左を後進方向に設定
			break;
	}
	//====右モータ====
	switch(d_dir & 0xf0){									//4~7ビット目を取り出す
		case 0x00:											//0x00の場合
			pin_write(PIN_CW_R, MT_FWD_R);					//右を前進方向に設定
			break;
		case 0x10:											//0x10の場合
			pin_write(PIN_CW_R, MT_BACK_R);					//右を後進方向に設定
			break;
	}
}




/*==========================================================
		走行系関数
==========================================================*/
/*
		基本仕様として，
		基幹関数		第一引数:走行パルス数

		マウスフラグ(MF)
			6Bit:デフォルトインターバルフラグ
			5Bit:減速フラグ
			4Bit:加速フラグ
			3Bit:制御フラグ
			1Bit:二次走行フラグ
*/
/*----------------------------------------------------------
		基幹関数
----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//driveA
// 指定パルス分加速走行する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveA(uint16_t dist){

	//====走行====
	//----走行開始----
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 1;										//減速・デフォルトインターバルフラグをクリア，加速フラグをセット
	drive_reset_t_cnt();									//テーブルカウンタをリセット
	drive_start();											//走行開始

	//----走行----
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが指定パルス以上進むまで待機

	drive_stop();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveD
// 指定パルス分減速走行して停止する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveD(uint16_t dist){

	//====走行====
	//----走行開始----
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//加速・減速・デフォルトインターバルフラグをクリア
	drive_start();											//痩躯開始

	int16_t c_pulse = dist - (t_cnt_l - min_t_cnt);			//等速走行距離 = 総距離 - 減速に必要な距離
	if(c_pulse > 0){
		//----等速走行----
		while((pulse_l < c_pulse) || (pulse_r < c_pulse));	//左右のモータが等速分のパルス以上進むまで待機
	}

	//----減速走行----
	MF.FLAG.DECL = 1;										//減速フラグをセット
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//====走行終了====
	drive_stop();
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//driveU
// 指定パルス分等速走行して停止する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveU(uint16_t dist){

	//====等速走行開始====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//加速・減速・デフォルトインターバルフラグをクリア
	drive_start();											//走行開始

	//====走行====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが減速分のパルス以上進むまで待機

	//====走行終了====
	drive_stop();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//driveC
// 指定パルス分デフォルトインターバルで走行して停止する
// 引数1：dist …… 走行するパルス
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void driveC(uint16_t dist){

	//====回転開始====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 1;
	MF.FLAG.ACCL = 0;										//加速・減速フラグをクリア，デフォルトインターバルフラグをセット
	drive_start();											//走行開始

	//====回転====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが定速分のパルス以上進むまで待機

	drive_stop();
}


void driveC_SLL(uint16_t dist){

	//====回転開始====
	MF.FLAG.DECL = 0;
	MF.FLAG.DEF = 0;
	MF.FLAG.ACCL = 0;										//加速・減速フラグをクリア，デフォルトインターバルフラグをセット
	MF.FLAG.SLL_STR = 1;
	drive_start();											//走行開始

	//====回転====
	while((pulse_l < dist) || (pulse_r < dist));			//左右のモータが定速分のパルス以上進むまで待機
	MF.FLAG.SLL_STR = 0;

}


/*----------------------------------------------------------
		上位関数
----------------------------------------------------------*/
//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionA
// 半区画分加速しながら走行する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionA(void){

	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveA(PULSE_SEC_HALF);									//半区画のパルス分加速しながら走行。走行後は停止しない
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断
}

void half_sectionAA(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	driveA(PULSE_SEC_HALF);									//半区画のパルス分加速しながら走行。走行後は停止しない
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断
}



//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionD
// 半区画分減速しながら走行し停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionD(void){

	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveD(PULSE_SEC_HALF);									//半区画のパルス分減速しながら走行。走行後は停止する
}

void half_sectionDD(void){

	MF.FLAG.CTRL = 0;										//制御を有効にする
	driveD(PULSE_SEC_HALF);									//半区画のパルス分減速しながら走行。走行後は停止する
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//one_section
// 1区画分進んで停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_section(void){

	half_sectionA();										//半区画分加速走行
	half_sectionD();										//半区画分減速走行のち停止
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionA(TEST)
// 半区画分加速しながら走行する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionA_test(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	driveA(PULSE_SEC_HALF);									//半区画のパルス分加速しながら走行。走行後は停止しない
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//half_sectionD
// 半区画分減速しながら走行し停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void half_sectionD_test(void){

	MF.FLAG.CTRL = 0;										//制御を有効にする
	driveD(PULSE_SEC_HALF);									//半区画のパルス分減速しながら走行。走行後は停止する
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//one_section
// 1区画分進んで停止する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_section_test(void){

	half_sectionA_test();										//半区画分加速走行
	half_sectionD_test();										//半区画分減速走行のち停止
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//one_sectionU
// 等速で1区画分進む
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void one_sectionU(void){

	MF.FLAG.CTRL = 1;										//制御を有効にする
	driveU(PULSE_SEC_HALF);									//半区画のパルス分等速走行。走行後は停止しない
	/*MF.FLAG.CTRL_R = 0;
	MF.FLAG.CTRL_L = 0;										//片壁制御無効
	*/
	driveU(PULSE_SEC_HALF);									//半区画のパルス分等速走行。走行後は停止しない
	get_wall_info();										//壁情報を取得
}

void one_sectionUU(void){

	MF.FLAG.CTRL = 0;										//制御を有効にする
	driveU(PULSE_SEC_HALF*2);									//半区画のパルス分等速走行。走行後は停止しない
}



//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_R90
// 右に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++

void rotate_R90(void){

	MF.FLAG.CTRL = 0;										//制御無効
	drive_set_dir(ROTATE_R);								//右に旋回するようモータの回転方向を設定
	//drive_wait();
	driveC(PULSE_ROT_R90);									//デフォルトインターバルで指定パルス分回転。回転後に停止する
	//drive_wait();
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_L90
// 左に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_L90(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(ROTATE_L);								//左に旋回するようモータの回転方向を設定
	//drive_wait();
	driveC(PULSE_ROT_L90);									//デフォルトインターバルで指定パルス分回転。回転後に停止する
	//drive_wait();
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//SLL_R90
// スラロームで右に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++

void SLL_R90(void){

	MF.FLAG.CTRL = 0;										//制御無効
	drive_set_dir(FORWARD);
	//直線区間
	drive_start();

	if(sp_mode >= 1){
	driveC_SLL(SLL_IO1_R);

	//クロソイド～円弧
	MF.FLAG.SLL_R_UP = 1;
	sll_t_cnt_l = sll_t_cnt_r = 0;

	pulse_l = pulse_r = 0;
	while((pulse_l < SLL_OUT_PULSE1/2 + CIR_OUT_PULSE_R1) && (pulse_r < SLL_IN_PULSE1/2 + CIR_IN_PULSE_R1));			//左右のモータがあるパルス数以上進むまで

	//～クロソイド
	MF.FLAG.SLL_R_UP = 0;
	MF.FLAG.SLL_R_DW = 1;
	sll_t_cnt_l = sll_t_cnt_r = 0;
	pulse_l = pulse_r = 0;

	while((pulse_l < SLL_OUT_PULSE1/2) && (pulse_r < SLL_IN_PULSE1/2));			//左右のモータが定速分のパルス以上進むまで待機

	MF.FLAG.SLL_R_DW = 0;
	pulse_l = pulse_r = 0;
	//直線区間
	driveC_SLL(SLL_IO1_R);
	}

	else if(sp_mode == 0){
		driveC_SLL(SLL_IO2_R);

		//クロソイド～円弧
		MF.FLAG.SLL_R_UP = 1;
		sll_t_cnt_l = sll_t_cnt_r = 0;

		pulse_l = pulse_r = 0;
		while((pulse_l < SLL_OUT_PULSE2/2 + CIR_OUT_PULSE_R2) && (pulse_r < SLL_IN_PULSE2/2 + CIR_IN_PULSE_R2));			//左右のモータがあるパルス数以上進むまで

		//～クロソイド
		MF.FLAG.SLL_R_UP = 0;
		MF.FLAG.SLL_R_DW = 1;
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;
	//	drive_set_dir(FORWARD);

		while((pulse_l < SLL_OUT_PULSE2/2) && (pulse_r < SLL_IN_PULSE2/2));			//左右のモータが定速分のパルス以上進むまで待機

		MF.FLAG.SLL_R_DW = 0;
		pulse_l = pulse_r = 0;
		//直線区間
		driveC_SLL(SLL_IO2_R);

	}
	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//SLL_L90
// スラロームで左に90度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void SLL_L90(void){

	MF.FLAG.CTRL = 0;										//制御無効
	drive_set_dir(FORWARD);								//左に旋回するようモータの回転方向を設定
	drive_start();
	if(sp_mode >= 1){
		driveC_SLL(SLL_IO1_L);
		//フラグ処理
		MF.FLAG.SLL_L_UP = 1;
		//initialization
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;
		while((pulse_l < SLL_IN_PULSE1 / 2 + CIR_IN_PULSE_L1) && (pulse_r < SLL_OUT_PULSE1 / 2 + CIR_OUT_PULSE_L1));			//左右のモータが定速分のパルス以上進むまで待機
		//フラグ処理
		MF.FLAG.SLL_L_UP = 0;
		MF.FLAG.SLL_L_DW = 1;
		//initialization
		drive_set_dir(FORWARD);
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;

		while((pulse_l < SLL_IN_PULSE1/2) && (pulse_r < SLL_OUT_PULSE1/2));

		MF.FLAG.SLL_L_DW = 0;
		pulse_l = pulse_r = 0;

		drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
		driveC_SLL(SLL_IO1_L);
	}

	else if(sp_mode == 0){
		driveC_SLL(SLL_IO2_L);
		//フラグ処理
		MF.FLAG.SLL_L_UP = 1;
		//initialization
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;
		while((pulse_l < SLL_IN_PULSE2 / 2 + CIR_IN_PULSE_L2) && (pulse_r < SLL_OUT_PULSE2 / 2 + CIR_OUT_PULSE_L2));			//左右のモータが定速分のパルス以上進むまで待機

		//drive_stop();

		//フラグ処理
		MF.FLAG.SLL_L_UP = 0;
		MF.FLAG.SLL_L_DW = 1;

		//initialization
		drive_set_dir(FORWARD);
		sll_t_cnt_l = sll_t_cnt_r = 0;
		pulse_l = pulse_r = 0;

		while((pulse_l < SLL_IN_PULSE2/2) && (pulse_r < SLL_OUT_PULSE2/2));

		MF.FLAG.SLL_L_DW = 0;
		pulse_l = pulse_r = 0;

		drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
		driveC_SLL(SLL_IO2_L);
	}

	get_wall_info();										//壁情報を取得，片壁制御の有効・無効の判断

}

//+++++++++++++++++++++++++++++++++++++++++++++++
//rotate_180
// 180度回転する
// 引数：なし
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void rotate_180(void){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(ROTATE_R);								//左に旋回するようモータの回転方向を設定
	//drive_wait();
	driveC(PULSE_ROT_180);									//デフォルトインターバルで指定パルス分回転。回転後に停止する
	//drive_wait();
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
}


//+++++++++++++++++++++++++++++++++++++++++++++++
//set_position
// 機体の尻を壁に当てて場所を区画中央に合わせる
// 引数：sw …… 0以外ならget_base()する
// 戻り値：なし
//+++++++++++++++++++++++++++++++++++++++++++++++
void set_position(uint8_t sw){

	MF.FLAG.CTRL = 0;										//制御を無効にする
	drive_set_dir(BACK);									//後退するようモータの回転方向を設定
	//drive_wait();
	driveC(PULSE_SETPOS_BACK);								//尻を当てる程度に後退。回転後に停止する
	//drive_wait();
	if(sw){
		get_base();
	}
	drive_set_dir(FORWARD);									//前進するようにモータの回転方向を設定
	//drive_wait();
	driveC(PULSE_SETPOS_SET);								//デフォルトインターバルで指定パルス分回転。回転後に停止する
	drive_wait();
}

//+++++++++++++++++++++++++++++++++++++++++++++++
//drive_test
// テストモード
// 戻り値：なし
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

