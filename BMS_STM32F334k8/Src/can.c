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
