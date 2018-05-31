/*
 * can.c
 *
 *  Created on: 28 mag 2018
 *      Author: Utente
 */
#include "can.h"


 int CAN_Send(int id, uint8_t dataTx[8], int size,CAN_HandleTypeDef hcan ){
 HAL_StatusTypeDef status;

  		CanTxMsgTypeDef TxMess;
  		TxMess.StdId = 0x600 + id; // 0x600 + Node ID
  		TxMess.DLC = size;
  		TxMess.IDE = CAN_ID_STD;
  		TxMess.RTR = CAN_RTR_DATA;
  		TxMess.Data[0] = dataTx[0];
  		TxMess.Data[1] = dataTx[1];
  		TxMess.Data[2] = dataTx[2];
  		TxMess.Data[3] = dataTx[3];
  		TxMess.Data[4] = dataTx[4];
  		TxMess.Data[5] = dataTx[5];
  		TxMess.Data[6] = dataTx[6];
  		TxMess.Data[7] = dataTx[7];
  		hcan.pTxMsg = &TxMess;

  		status = HAL_CAN_Transmit(&hcan, 1000);
 }
void Send2GUI(uint16_t data[12][9],int id,  int size,CAN_HandleTypeDef hcan,float adcV, float current){
	uint8_t dataTxVect[108];
	for(int i=0;i<12;i++){

		for(int j=0;j<9;j++){

			dataTxVect[i*9 +j ]=data[i][j] << 8;

			}
	}
	uint8_t dataTx[8];
	for(int i=0;i<27;i++){

		for(int j=0;j<8;j++){


		dataTx[j]=dataTxVect[i*8 +j];

			 if(j%8==0){
			 CAN_Send(id,dataTx,size,hcan);
			 }

		}

//		uint8_t dataOther[8];
//		dataOther[0]=(uint8_t)adcV;
//		dataOther[1]=(uint8_t)adcV<<8;
//		dataOther[2]=(uint8_t)adcV<<16;
//		dataOther[3]=(uint8_t)adcV<<24;
//
//		dataOther[4]=(uint8_t)current;
//		dataOther[5]=(uint8_t)current<<8;
//		dataOther[6]=(uint8_t)current<<16;
//		dataOther[7]=(uint8_t)current<<24;


		CAN_Send(id,dataTx,size,hcan);

}
