/*
 * @file
 * @brief a record interface in windows
 *
 * it encapsluate the windows API waveInxxx;
 * Common steps:
 *	create_recorder,
 *	open_recorder, 
 *	start_record, 
 *	stop_record, 
 *	close_recorder,
 *	destroy_recorder
 *
 * @author		taozhang9
 * @date		2016/05/27
 */

#ifndef __IFLY_WINREC_H__
#define __IFLY_WINREC_H__


/* error code */
enum {
	RECORD_ERR_BASE = 0,
	RECORD_ERR_GENERAL,
	RECORD_ERR_MEMFAIL,
	RECORD_ERR_INVAL,
	RECORD_ERR_NOT_READY
};

/* recorder object. */
struct recorder {
	void (*on_data_ind)(char *data, unsigned long len, void *user_para);
	void * user_cb_para;
	volatile int state;		/* internal record state */

	void * wavein_hdl;
	void * rec_thread_hdl;
	void * bufheader;
	unsigned int bufcount;
};

#ifdef __cplusplus
extern "C" {
#endif /* C++ */

/** 
 * @fn
 * @brief	Get the default input device ID
 *
 * @return	returns WAVE_MAPPER in windows.
 */
int get_default_input_dev();

/**
 * @fn 
 * @brief	Get the total number of active input devices.
 * @return	the number. 0 means no active device. 
 */
unsigned int get_input_dev_num();

/**
 * @fn 
 * @brief	Create a recorder object.
 * @return	int			- Return 0 in success, otherwise return error code.
 * @param	out_rec		- [out] recorder object holder
 * @param	on_data_ind	- [in]	callback. called when data coming.
 * @param	user_cb_para	- [in] user params for the callback.
 * @see
 */
int create_recorder(struct recorder ** out_rec, 
				void (*on_data_ind)(char *data, unsigned long len, void *user_para), 
				void* user_cb_para);

/**
 * @fn 
 * @brief	Destroy recorder object. free memory. 
 * @param	rec	- [in]recorder object
 */
void destroy_recorder(struct recorder *rec);

/**
 * @fn 
 * @brief	open the device.
 * @return	int			- Return 0 in success, otherwise return error code.
 * @param	rec			- [in] recorder object
 * @param	dev			- [in] device id, from 0.
 * @param	fmt			- [in] record format.
 * @see
 * 	get_default_input_dev()
 */
int open_recorder(struct recorder * rec, unsigned int dev, WAVEFORMATEX * fmt);

/**
 * @fn
 * @brief	close the device.
 * @param	rec			- [in] recorder object
 */

void close_recorder(struct recorder *rec);

/**
 * @fn
 * @brief	start record.
 * @return	int			- Return 0 in success, otherwise return error code.
 * @param	rec			- [in] recorder object
 */
int start_record(struct recorder * rec);

/**
 * @fn
 * @brief	stop record.
 * @return	int			- Return 0 in success, otherwise return error code.
 * @param	rec			- [in] recorder object
 */
int stop_record(struct recorder * rec);

/**
 * @fn
 * @brief	test if the recording has been stopped.
 * @return	int			- 1: stopped. 0 : recording.
 * @param	rec			- [in] recorder object
 */
int is_record_stopped(struct recorder *rec);

#ifdef __cplusplus
} /* extern "C" */	
#endif /* C++ */

#endif