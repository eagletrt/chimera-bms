/*
 * measurement.c
 *
 *  Created on: 28 mag 2018
 *      Author: Utente
 */


#include "measurement.h"

/* Private functions ---------------------------------------------------------*/
static int MEASUREMENT_ScaleAmps(int rawval);


/* Private variables ---------------------------------------------------------*/

// Some PT1 (IIR) filter variables
int prefilt = 0;
int prefilt_L = 0;

int dc_offset = 0;
int dc_offset_L = 0;

int slowfilt = 0;
int slowfilt_L = 0;

int zoomfilt = 0;
int zoomfilt_L = 0;

// Variables for scope 2 with variable time base
int zoomfilt_shift = 0;
int zoom_n = 1; // variable time base
int zoom_n_cnt = 0;	// Counter for variable time base


int zoom_min = 4096; // Min and max values
int zoom_max = 0; // Min and max values
int zoom_min_hold = SENSOR_OFFSET; // Min and max values
int zoom_max_hold = SENSOR_OFFSET; // Min and max values
int zoom[160];	// Array with zoomed scope

int trigger_pos = 0;	// Trigger position
int zoom_cnt = 0;	// Index in the zoom field
enTRIGGER trigger = TRG_PREPARE; // Trigger state machine
int triggercnt = 0; // Trigger counter
int trigger_prepare_cnt = 0; // Counter for pre-trigger

// Edge detection
int sign = 0;
int sign_old = 0;
int pos_edge = 0;
int neg_edge = 0;
int pos_edge_cnt = 0;

// Count the puls and period time
int period_cnt = 0;
int period = 0;
int pulse = 0;

// Auto zero offset calibration
int sensor_offset = SENSOR_OFFSET;
int offset_cnt = 0;

// stop or run
int measurement_run = RUN;
int stop_cnt = 0;

// Variables used to save the history
uint8_t zoom_tmp[160];
MEASUREMENT_tHistory history[HISTORY_ENTRIES];
int history_index = -1;

/**
 * Initialize the module
 */
void MEASUREMENT_Init(void) {
}

/**
 * Getter for run or stop state
 *
 * @return RUN or STOP
 *
 */
int MEASUREMENT_GetRun(void) {
	return measurement_run;
}

/**
 * Function to process a new ADC value.
 *
 * @param value
 * 	The new ADC value
 */
 void MEASUREMENT_NewSample(uint16_t value) {

	// Filter the stream with t= 80Âµs for edge detection (frequency calculation)
	prefilt_L += value - prefilt;
	prefilt = prefilt_L / T_PREFILT;

	// Filter the stream with t= 163ms for DC offset
	dc_offset_L += prefilt - dc_offset;
	dc_offset = dc_offset_L / T_DC_OFFSET;

	// Filter the stream with t= 163ms for 1st scope screen
	slowfilt_L += prefilt - slowfilt;
	slowfilt = slowfilt_L / T_SLOWFILT;

	// Filter the zoomed samples
	zoomfilt_L += prefilt - zoomfilt;
	zoomfilt = zoomfilt_L >> zoomfilt_shift;

	// Zero calibration
	if (offset_cnt <= OFFSET_TIME) {
		offset_cnt++;
		if (offset_cnt == OFFSET_TIME) {
			if ( ( slowfilt >= (SENSOR_OFFSET - OFFSET_THRESHOLD)) &&
				 ( slowfilt <= (SENSOR_OFFSET + OFFSET_THRESHOLD))) {
				sensor_offset = slowfilt;
				zoom_min_hold = sensor_offset;
				zoom_max_hold = sensor_offset;
			}
		}
	}

	// No welding current
	if ((     (slowfilt-sensor_offset) <  STOP_THRESHOLD ) &&
			( (slowfilt-sensor_offset) > -STOP_THRESHOLD )) {

		// Stop after a certain time
		if (stop_cnt < ONE_MEASUREMENT_SEC) {
			stop_cnt++;
		} else {
			measurement_run = STOP;
		}
	} else {
		stop_cnt = 0;
		measurement_run = RUN;
	}



	// use every nth sample for zoom
	zoom_n_cnt++;
	if (zoom_n_cnt >= zoom_n) {
		zoom_n_cnt = 0;

		// Generate the trigger event
		if (trigger != TRG_COMPLETE) {

			if (trigger_prepare_cnt <= 32) {
				trigger_prepare_cnt ++;
			} else if (trigger == TRG_PREPARE) {
				trigger = TRG_READY;
				zoom_min = 4096;
				zoom_max = 0;
			}

			zoom_cnt++;
			if (zoom_cnt >= 160) {
				zoom_cnt = 0;
			}
			zoom[zoom_cnt] = zoomfilt;
			if (zoomfilt < zoom_min)
				zoom_min = zoomfilt;
			if (zoomfilt > zoom_max)
				zoom_max = zoomfilt;

			// Finished
			if (trigger == TRG_TRIGGERED) {
				if (triggercnt > 0) {
					triggercnt--;
				} else {
					trigger = TRG_COMPLETE;
				}
			}
		}
	}


	// edge detection
	if (prefilt > (dc_offset+THRESHOLD))
		sign = 1;
	else if (prefilt < (dc_offset-THRESHOLD))
		sign = -1;
	pos_edge = ((sign==1) && (sign_old == -1));
	neg_edge = ((sign==-1) && (sign_old == 1));
	sign_old = sign;

	// Suppress a positive edge for 1ms (fmax = 1kHz)
	if (pos_edge_cnt < (POS_EDGE_TIMEOUT-(POS_EDGE_TIMEOUT/10))) {
		if (neg_edge) {
			pulse = period_cnt;
		}
	}

	// Suppress a positive edge for 1ms (fmax = 1kHz)
	if (pos_edge_cnt > 0) {
		pos_edge_cnt --;
	} else if (pos_edge) {
		pos_edge_cnt = POS_EDGE_TIMEOUT;
		period = period_cnt;
		period_cnt = 0;

		// Trigger (edge detection)
		if (trigger == TRG_READY) {
			// 128 samples after the trigger + 32 samples pre-trigger = 160
			triggercnt = 128;
			trigger = TRG_TRIGGERED;
			//Keep the trigger position
			trigger_pos = zoom_cnt;
		}
	}

	// Count the period ( measure the frequency)
	if (period_cnt < 1000000)
		period_cnt ++;

}

/**
 * Getter for frequency
 *
 * @return frequency in Hz
 *
 */
int MEASUREMENT_GetFrequency(void) {
	if (period == 0)
		return 0;
	return SAMPLING_FRQ / period;
}

/**
 * Getter for duty cycle
 *
 * @return duty cycle in %
 *
 */
int MEASUREMENT_GetRatio(void) {
	if (period == 0)
		return 100;
	if (pulse > period) {
		return 100;
	}
	return ((pulse * 100) / period);
}

/*
 * Scales the ADC values to amps
 *
 * The resistor divider 500R and 1k, or 3:2, so 5V -> 3.333V
 * Vref = 3.3V
 * ADCmax = 4096 = 4,95V
 *
 * Sensor is LEM  HTFS 200-P (Farnell 9135715)
 * SensorOffset = 2.5V = 2068 LSB
 * Vout_Sensor = Vref ± (1.25 * I / 200A )
 * Vout_Sensor = Vref ± 6.25mR * I
 * 1A = 5.1717... LSB
 * I(A) = ADC * 0,193359375 = ADC * 99 / 512
 *
 * @param rawval
 * 		ADC raw value
 * @return the valus in amps
 */
static int MEASUREMENT_ScaleAmps1(int rawval,uint16_t sensor_offset){

	return (((rawval-sensor_offset)*99)/512);
}
static int MEASUREMENT_ScaleAmps(int rawval){

	return (((rawval-sensor_offset)*99)/512);
}

/**
 * Getter for slow filtered current
 *
 * @return current in amps
 *
 */
int MEASUREMENT_GetSlowFilt(void) {
	return MEASUREMENT_ScaleAmps(slowfilt);
}

/**
 * Getter for minimum current
 *
 * @return current in amps
 *
 */
int MEASUREMENT_GetMin(void) {
	return MEASUREMENT_ScaleAmps(zoom_min_hold);
}

/**
 * Getter for maximum current
 *
 * @return current in amps
 *
 */
int MEASUREMENT_GetMax(void) {
	return MEASUREMENT_ScaleAmps(zoom_max_hold);
}

/**
 * Copies the zoomed filed
 *
 * @param target
 * 	destination buffer
 *
 */
void MEASUREMENT_CopyZoomField(uint8_t target[]) {
	int i,ii,iii;
	int scale, offset;
	int val;

	// do it only during RUN
	if (measurement_run == STOP)
		return;

	// Scale it to 80% of scope2 screen height and adjust it to the middle
	offset = (zoom_min + zoom_max) / 2;
	scale = (zoom_max - zoom_min);

	zoom_min_hold = zoom_min;
	zoom_max_hold = zoom_max;

	// 10A min
	if (scale <= 51)
		scale = 51;

	// Scale it to 80px
	scale /= 80;

	// calculate 1/x and scale it with 65536
	scale = 65536 / scale;

	if (trigger == TRG_COMPLETE) {
		ii = trigger_pos;
		iii = 32;
		for (i=0; i<160; i++) {
			val = (zoom[ii] - offset) * scale / 65536;

			val += 50;
			// Limit to 0..100
			if (val < 0)
				val = 0;
			if (val > 100)
				val = 100;

			// Copy
			target[iii] = (uint8_t)(val);
			zoom_tmp[iii] = (uint8_t)(val);

			ii++;
			if (ii >= 160) {
				ii = 0;
			}
			iii++;
			if (iii >= 160) {
				iii = 0;
			}
		}
		zoom_cnt = 0;
		trigger_prepare_cnt = 0;
		trigger = TRG_PREPARE;

		zoom_n = period / 50;
	}

}

/**
 * Save the current values for hitory
 */
void MEASUREMENT_SaveHistory(void) {
	int i;
	history_index++;
	if (history_index >= HISTORY_ENTRIES)
		history_index = 0;

	history[history_index].zoom_min = MEASUREMENT_GetMin();
	history[history_index].zoom_max = MEASUREMENT_GetMax();
	history[history_index].amps = MEASUREMENT_GetSlowFilt();
	history[history_index].frequency = MEASUREMENT_GetFrequency();
	history[history_index].ratio = MEASUREMENT_GetRatio();

	for (i=0; i<160; i++)
		history[history_index].zoom[i] = zoom_tmp[i];
}

/**
 * Gets one entry of the history buffer
 *
 * @index
 * 	index of the history
 */
MEASUREMENT_tHistory MEASUREMENT_GetHistory(int index) {
	int i;

	i = history_index - index;
	if (i < 0)
		i+= HISTORY_ENTRIES;
	return history[i];
}
float Current_Calibration_Offset(ADC_HandleTypeDef hadc1){
	uint32_t *adcBuffer;
	uint32_t sum=0;
	for(int i=0;i<10;i++){

<<<<<<< HEAD
	HAL_ADC_Start_DMA(&hadc1, adcBuffer[i], 1);
	sum=sum+adcBuffer[i];
	HAL_Delay(20);

	}
	float offset=(float)sum/10;
	return offset;
}
uint32_t Get_Amps_Value(uint32_t *Vout,uint16_t offset){
=======
uint32_t Get_Amps_Value(uint32_t *Vout){
>>>>>>> 594fe3abe34ee1125d96496891c9e0c9d3c856ae
		float VVoutV = (float)Vout[0] * 3.3 / 4095;
		return MEASUREMENT_ScaleAmps1((int)Vout[0], offset);

	}
