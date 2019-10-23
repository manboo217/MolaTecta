/*
 * params.h
 *
 *  Created on: 2017/10/25
 *      Author: Blue
 */

#ifndef PARAMS_H_
#define PARAMS_H_

/*============================================================
  各種定数（パラメータ）設定
============================================================*/
/*------------------------------------------------------------
  走行系
------------------------------------------------------------*/
//----走行パルス関連----
#define PULSE_SEC_HALF  380  //387->391
//#define PULSE_SEC_HALF_R  380

//半区画走行用パルス。2倍すると1区画分に
#define PULSE_ROT_R90   252  //右90度回転用パルス数 259
#define PULSE_ROT_L90   252  //左90度回転用パルス数 259
#define PULSE_ROT_180   504  //180度回転用パルス数 517
#define PULSE_SETPOS_BACK  450  //後ろ壁に当てるために下がるパルス数250
#define PULSE_SETPOS_SET  217  //後ろ壁から中央までのパルス数235

//SLALOMパルス数
#define SLL_OUT_PULSE1	655  //スラローム外輪パルス 655
#define SLL_IN_PULSE1	145  //スラローム内輪パルス 145

#define SLL_OUT_PULSE2	655  //スラローム外輪パルス 764
#define SLL_IN_PULSE2	145  //スラローム内輪パルス 169


//SLALOM速　パルス数
#define SLL_IO1_L		72		//72
#define SLL_IO1_R		72
#define CIR_IN_PULSE_R1		110  //114
#define CIR_OUT_PULSE_R1	495  //513
#define CIR_IN_PULSE_L1		110	 //110
#define CIR_OUT_PULSE_L1	495  //495


//SLALOM遅　パルス数
#define SLL_IO2_L	55		//43->48->52->58
#define SLL_IO2_R	55
#define CIR_IN_PULSE_R2		131//53
#define CIR_OUT_PULSE_R2	597//クロソイドスラロームの加減速ターンの間に入る定速ターンパルス 53
#define CIR_IN_PULSE_L2		128	//53
#define CIR_OUT_PULSE_L2	582//クロソイドスラロームの加減速ターンの間に入る定速ターンパルス 53


//----テーブルカウンタ関連----
#define MAX_T_CNT PULSE_SEC_HALF //テーブルカウンタの最大値 PULSE_SEC_HALF
#define MIN_T_CNT 0    //テーブルカウンタの最小値
#define SLL_T_CNT1 122   //スラロ―ム可減速時カウンタ最大値 速い時->122
#define SLL_T_CNT2 201
//----タイマ関連----
#define DEFAULT_INTERVAL 1600 //デフォルトのインターバル→小さくするほど速い
#define SLL_INTERVAL1 471 //スラロームのインターバル→小さくするほど速い
#define SLL_INTERVAL2 714 //スラロームのインターバル→小さくするほど速い

/*------------------------------------------------------------
  センサ系
------------------------------------------------------------*/
//----壁判断閾値（しきい値）----
#define WALL_BASE_FL 120 //前壁左センサ120
#define WALL_BASE_L  170  //左壁センサ170
#define WALL_BASE_R  100  //右壁センサ100
#define WALL_BASE_FR 100 //前壁右センサ100


//----制御閾値（しきい値）----sensor.c→TIM6_DAC1_IRQHandler
#define CTRL_BASE_L	240	//左制御閾値 359
#define CTRL_BASE_R	240	//右制御閾値 479➡一区間+3mmの位置に壁を置いてその時のセンサ値を閾値に
#define UP_L 150         //150->130->180!! 160~190?
#define UP_R 150        //150->130->180!! 160~190?
//#define Kp 0.136F

#define Kp0	0.125F //遅くて速いSLL		0.135 OK
#define Kp1	0.127F //比例制御係数(SLL遅) 使わない	0.130
#define Kp2	0.130F //比例制御係数(速) 0.136!!!->>本日地下にて0.140
#define Kp3 0.133F		//最速　0.138!!!->>本日地下にて0.140
#define Kp4 0.136F

#define DIFF_THRE_L 150 //150
#define DIFF_THRE_R 350 //350
//----赤外線（赤色）LED発光待機時間（単位はマイクロ秒）
#define IR_WAIT_US 20

//----AD変換スピード
#define ADC_SPEED 0b100 //19.5 Clock Cycles

//#define SS_CNT_MAX 1000000


/*--------------------------------										get_base();
 * ----------------------------
  探索系
------------------------------------------------------------*/
//----ゴール座標----
#define GOAL_X 9 //7
#define GOAL_Y 6 //7


#endif /* PARAMS_H_ */
