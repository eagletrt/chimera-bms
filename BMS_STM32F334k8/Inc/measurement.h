/*
 * measurement.h
 *
 *  Created on: 28 mag 2018
 *      Author: Utente
 */

#ifndef MEASUREMENT_H_
#define MEASUREMENT_H_
/* includes  ---------------------------------------------------------*/
#include <stdint.h>

#define SAMPLING_FRQ 100000 // ADC sampling frequency
#define SAMPLING_TIME 10 // ADC sampling time in 탎

#define T_PREFILT 8 // Filter time of the prefilter in 10탎 steps
#define T_DC_OFFSET 16384 // Filter time of the DC offset filter in 10탎 steps
#define T_SLOWFILT 16384 // Filter time of the slow filter for 1st scope screen in 10탎 steps
#define POS_EDGE_TIMEOUT 100 // in 10탎 steps. So 100 = 1000탎 -> fmax = 1kHz
#define THRESHOLD 20 // Threshold in ADC steps. 20 = +- 4A
#define STOP_THRESHOLD 25 // Threshold in ADC steps. 25 = +- 5A

#define SENSOR_ENTRIES 3

#define SENSOR_OFFSET 2068 // ADC at 0A - see static int MEASUREMENT_ScaleAmps(int rawval)
#define ONE_MEASUREMENT_SEC 100000
#define OFFSET_TIME (2*ONE_MEASUREMENT_SEC) // Sample count. 200000 @100KHz = 2sec
#define OFFSET_THRESHOLD 50 // Max deviation for zero offset calibration in ADC steps. 50 = +- 5A
#define AUTOSAVE_CYCLE 10 // in 0.256s steps
#define HISTORY_ENTRIES (250 / AUTOSAVE_CYCLE)

#define RUN 1
#define STOP 0

typedef enum {
  TRG_PREPARE,
  TRG_READY,
  TRG_TRIGGERED,
  TRG_COMPLETE
}enTRIGGER;

typedef struct {
	uint8_t zoom[160];
	int zoom_min;
	int zoom_max;
	int amps;
	int frequency;
	int ratio;
}MEASUREMENT_tHistory;


/* Global variables -------------------------------------------------*/

/* Function prototypes -------------------------------------------------*/
void MEASUREMENT_Init(void);
void MEASUREMENT_NewSample(uint16_t value);
int MEASUREMENT_GetFrequency(void);
int MEASUREMENT_GetSlowFilt(void);
void MEASUREMENT_CopyZoomField(uint8_t target[]);
int MEASUREMENT_GetMin(void);
int MEASUREMENT_GetMax(void);
int MEASUREMENT_GetRatio(void);
int MEASUREMENT_GetRun(void);
void MEASUREMENT_SaveHistory(void);
uint32_t Get_Amps_Value(uint32_t *Vout);
MEASUREMENT_tHistory MEASUREMENT_GetHistory(int index);


#endif /* MEASUREMENT_H_ */
