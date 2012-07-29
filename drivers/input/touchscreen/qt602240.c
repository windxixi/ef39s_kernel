/* file_name: qt602240.c
 *
 * description: Quantum TSP driver.
 *
 * Copyright (C) 2008-2010 Atmel & Pantech Co. Ltd.
 *
 */

#include <linux/module.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/earlysuspend.h>
#include <asm/io.h>
#include <linux/gpio.h>
#include <mach/vreg.h>
#include <linux/regulator/consumer.h>
#include <mach/gpio.h>
#include <linux/miscdevice.h>
#include <linux/hrtimer.h>
#include <linux/pm.h>
#include <linux/slab.h>
#include <asm/mach-types.h>

//ADC
#include <linux/pmic8058-xoadc.h>
#include <linux/msm_adc.h>
#include <mach/mpp.h>                                

#include "qt602240.h"

#if defined(CONFIG_MACH_MSM8X60_EF39S)
#include "qt602240_cfg_ef39s.h"
#elif defined(CONFIG_MACH_MSM8X60_EF40K) || defined(CONFIG_MACH_MSM8X60_EF40S)	
#if (BOARD_REV >= TP10)
#include "qt602240_cfg_ef40k.h"
#elif (BOARD_REV <= WS20)
#include "qt602240_cfg_ef40k_ws20.h"
#endif
#elif defined(CONFIG_MACH_MSM8X60_PRESTO)
#include "qt602240_cfg_presto.h"
#elif defined(CONFIG_MACH_MSM8X60_EF65L)
#include "qt602240_cfg_ef65l.h"
#else
#include "qt602240_cfg.h"
#endif


/* -------------------------------------------------------------------- */
/* debug option */
/* -------------------------------------------------------------------- */

static int DebugON = 0;
#define dbg(fmt, args...) if(DebugON)printk("[QT602240]" fmt, ##args)
#define dbg_raw(fmt, args...) if(DebugON)printk("" fmt, ##args)

#define dbg_func_in()		dbg("[+] %s\n", __func__)
#define dbg_func_out()		dbg("[-] %s\n", __func__)
#define dbg_line()		dbg("line : %d | func : %s\n", __LINE__, __func__)
/* -------------------------------------------------------------------- */

#define IRQ_TOUCH_INT		gpio_to_irq(GPIO_TOUCH_CHG)
/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* qt602240 i2c slave address */
/* -------------------------------------------------------------------- */
#define QT602240_I2C_ADDR       0x4A
#define QT602240_I2C_BOOT_ADDR  0x24
/* -------------------------------------------------------------------- */

#ifdef QT_FIRMUP_ENABLE
/* -------------------------------------------------------------------- */
/* qt602240 chipset firmware data to update */
/* -------------------------------------------------------------------- */

unsigned char QT602240_firmware[] = {
#include "mXT224e__APP_v1_0_AA.h"
};

void	QT_reprogram(void);
uint8_t	QT_Boot(bool withReset);
/* -------------------------------------------------------------------- */
#endif // QT_FIRMUP_ENABLE


/* -------------------------------------------------------------------- */
/* function proto type & variable for driver							*/
/* -------------------------------------------------------------------- */
static int __devinit qt602240_probe(struct i2c_client *client, const struct i2c_device_id *id);
static int __devexit qt602240_remove(struct i2c_client *client);
#if defined(CONFIG_PM) && defined(CONFIG_HAS_EARLYSUSPEND)
static int qt602240_late_resume(struct early_suspend *h);
static int qt602240_early_suspend(struct early_suspend *h);
#endif //CONFIG_PM && CONFIG_HAS_EARLYSUSPEND

static struct class *touch_atmel_class;
struct device *ts_dev;

static const struct i2c_device_id qt602240_id[] = {
	{ "qt602240-i2c", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, qt602240_id);

static struct i2c_driver qt602240_driver = {
	.driver = {
		.name	= "qt602240-i2c",
		.owner	= THIS_MODULE,
	},
	.probe		= qt602240_probe,
	.remove		= __devexit_p(qt602240_remove),
	.id_table	= qt602240_id,
//#if defined(CONFIG_PM) 
//	.suspend    	= qt602240_suspend,
//	.resume		= qt602240_resume,
//#endif //CONFIG_PM
};

struct workqueue_struct *qt602240_wq;

struct qt602240_data_t
{
	struct i2c_client *client;
	struct input_dev *input_dev;
	struct work_struct work;
#ifdef LOCKSCREEN_MODE
	struct work_struct work_lockscreen_mode_enable;
	struct work_struct work_lockscreen_mode_disable;
#endif
	struct early_suspend es;
	struct mutex    lock;
};
struct qt602240_data_t *qt602240_data = NULL;


/* -------------------------------------------------------------------- */
/* function proto type & variable for attribute				*/
/* -------------------------------------------------------------------- */
/* for attribute */
static ssize_t i2c_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t i2c_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t size);
static ssize_t gpio_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t gpio_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t size);
static ssize_t setup_show(struct device *dev, struct device_attribute *attr, char *buf);
static ssize_t setup_store(struct device *dev, struct device_attribute *attr,const char *buf, size_t size);
static DEVICE_ATTR(gpio, S_IRUGO | S_IWUSR, gpio_show, gpio_store);
static DEVICE_ATTR(i2c, S_IRUGO | S_IWUSR, i2c_show, i2c_store);
static DEVICE_ATTR(setup, S_IRUGO | S_IWUSR, setup_show, setup_store);
/* -------------------------------------------------------------------- */

typedef struct
{
	info_id_t info_id;          /*! Info ID struct. */
	object_t *objects;          /*! Pointer to an array of objects. */
	uint16_t CRC;               /*! CRC field, low bytes. */
	uint8_t CRC_hi;             /*! CRC field, high byte. */
} info_block_t;

typedef struct
{
	uint8_t object_type;     /*!< Object type. */
	uint8_t instance;        /*!< Instance number. */
} report_id_map_t;

#ifdef QT_MULTITOUCH_ENABLE
typedef struct
{
	uint8_t id;                     /*!< id */
	int8_t status;          	/*!< dn=1, up=0, none=-1 */
	int16_t x;                      /*!< X */
	int16_t y;                      /*!< Y */

	int8_t mode;
	int16_t realx;                     
	int16_t realy;
	int area;
} report_finger_info_t;

typedef struct
{
	uint16_t code;                      /*!< key code */
	uint8_t status;                      /*!< key status (press/release) */
	bool update;     
} report_key_info_t;

static report_finger_info_t fingerInfo[MAX_NUM_FINGER];
static report_key_info_t keyInfo[MAX_NUM_FINGER];

#endif //QT_MULTITOUCH_ENABLE

typedef struct 
{
	uint8_t tch_ch; /*number of touch channel*/
	uint8_t atch_ch; /*number of anti-touch channel*/
} debug_info_t;
static debug_info_t debugInfo;

static info_block_t *info_block;
static report_id_map_t *report_id_map;
volatile uint8_t read_pending;
static int max_report_id = 0;
uint8_t max_message_length;
uint16_t message_processor_address;

/*! Command processor address. */
static uint16_t command_processor_address;

/*! Flag indicating if driver setup is OK. */
static enum driver_setup_t driver_setup = DRIVER_SETUP_INCOMPLETE;

/*! Message buffer holding the latest message received. */
uint8_t *quantum_msg;

/*! \brief The current address pointer. */
static U16 address_pointer;

#ifdef ITO_TYPE_CHECK
static int tsp_ito_type = -1;
#endif
/* flag to indicate if last calibration has been confirmed good */
static uint8_t cal_check_flag = 0u;

#ifdef CHIP_NOINIT
static bool Chip_NoInit = false;
#endif

/* Calibration checking routine calling code - in interrupt handler */
#ifdef RECHECK_AFTER_CALIBRATION
static uint8_t calibration_message = 0;
#endif

static uint8_t timer_tick = 0;
static struct timer_list timer;
#ifdef LOCKSCREEN_MODE
static struct timer_list lockscreen_mode_timer;
#endif
static bool timer_enabled = false;
static bool active_event = true;
static bool is_cal_success = false;

typedef enum
{
	TSC_EVENT_NONE,
	TSC_EVENT_WINDOW,
	TSC_EVENT_MENU,
	TSC_EVENT_HOME,
	TSC_EVENT_BACK,  
	TSC_EVENT_FACEPRESS, 
} tsc_key_mode_type;

typedef enum
{
	TSC_CLEAR_ALL,
	TSC_CLEAR_EVENT,
} tsc_clear_type;

#ifdef TOUCH_IO
typedef enum 
{
	APPLY_TOUCH_CONFIG = 501,
	DIAG_DEBUG = 502,
	RESET_TOUCH_CONFIG = 503,
	GET_TOUCH_CONFIG = 504,
	SET_TOUCH_CONFIG = 505,
	READ_ITO_TYPE = 506,
	TOUCH_IOCTL_DEBUG = 507,

	TOUCH_CHARGER_MODE = 701,

	TOUCH_LOCKSCREEN_MODE_ENABLE = 1013,
	TOUCH_LOCKSCREEN_MODE_DISABLE = 1014
}CONFIG_CMD;
#endif //TOUCH_IO

#ifdef CHARGER_MODE
unsigned int tsp_power_status=1;

unsigned long charger_mode;
unsigned long previous_charger_mode;
#endif //CHARGER_MODE

static int nCalCount=0;

/*------------------------------ functions prototype -----------------------------------*/
uint8_t init_touch_driver(void);
uint8_t reset_chip(void);
uint8_t calibrate_chip(void);
uint8_t diagnostic_chip(uint8_t mode);
uint8_t backup_config(void);
uint8_t write_power_config(gen_powerconfig_t7_config_t power_config);
uint8_t write_acquisition_config(gen_acquisitionconfig_t8_config_t acq_config);
uint8_t write_multitouchscreen_config(uint8_t screen_number, touch_multitouchscreen_t9_config_t cfg);
uint8_t write_keyarray_config(uint8_t key_array_number, touch_keyarray_t15_config_t cfg);
uint8_t write_comc_config(uint8_t instance, spt_comcconfig_t18_config_t cfg);
uint8_t write_gpio_config(uint8_t instance, spt_gpiopwm_t19_config_t cfg);
uint8_t write_proximity_config(uint8_t instance, touch_proximity_t23_config_t cfg);
uint8_t write_onetouchgesture_config(uint8_t instance, proci_onetouchgestureprocessor_t24_config_t cfg);
uint8_t write_selftest_config(uint8_t instance, spt_selftest_t25_config_t cfg);
uint8_t write_twotouchgesture_config(uint8_t instance, proci_twotouchgestureprocessor_t27_config_t cfg);
uint8_t write_simple_config(uint8_t object_type, uint8_t instance, void *cfg);
uint8_t get_object_size(uint8_t object_type);
uint8_t type_to_report_id(uint8_t object_type, uint8_t instance);
uint8_t report_id_to_type(uint8_t report_id, uint8_t *instance);
uint8_t read_id_block(info_id_t *id);
uint16_t get_object_address(uint8_t object_type, uint8_t instance);
uint32_t get_stored_infoblock_crc(void);
uint8_t calculate_infoblock_crc(uint32_t *crc_pointer);
uint32_t CRC_24(uint32_t crc, uint8_t byte1, uint8_t byte2);
void write_message_to_usart(uint8_t msg[], uint8_t length);
int init_hw_setting(void);
void TSP_Restart(void);
int8_t check_chip_calibration(void);
int8_t get_touch_antitouch_info(void);

uint8_t write_gripsuppression_T40_config(proci_gripsuppression_t40_config_t cfg);
uint8_t write_palmsuppression_T41_config(proci_palmsuppression_t41_config_t cfg);
uint8_t write_touchsuppression_t42_config(proci_touchsuppression_t42_config_t cfg);
uint8_t write_digitizer_t43_config(spt_digitizer_t43_config_t cfg);
uint8_t write_CTE_T46_config(spt_cteconfig_t46_config_t cfg);
uint8_t write_stylus_t47_config(proci_stylus_t47_config_t cfg);
uint8_t write_noisesuppression_t48_config(procg_noisesuppression_t48_config_t cfg);

U8 read_mem(U16 start, U8 size, U8 *mem);
U8 read_U16(U16 start, U16 *mem);
U8 write_mem(U16 start, U8 size, U8 *mem);

void  clear_event(uint8_t clear);


#ifdef SKY_PROCESS_CMD_KEY 
// To be depreciated.
static int ts_fops_ioctl(struct inode *inode, struct file *filp,unsigned int cmd, unsigned long arg);
static int ts_fops_open(struct inode *inode, struct file *filp);
#endif

#ifdef TOUCH_IO
/* File IO */
static int open(struct inode *inode, struct file *file);
static int release(struct inode *inode, struct file *file);
static ssize_t read(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t write(struct file *file, const char *buf, size_t count, loff_t *ppos);
static int ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg);
static int ioctl_debug(unsigned long arg);

static void apply_touch_config(void);
static void reset_touch_config(void);
#ifdef CHARGER_MODE
static uint8_t qt_charger_mode_config(unsigned long mode);
#endif // CHARGER_MODE
#endif // TOUCH_IO

void  qt602240_front_test_init(void);
void off_hw_setting(void);

/*------------------------------ for tunning ATmel - start ----------------------------*/
struct class *touch_class;
EXPORT_SYMBOL(touch_class);

struct device *switch_test;
EXPORT_SYMBOL(switch_test);


/******************************************************************************
 * 
 *       QT602240 Object table init
 *                                                             
 * *****************************************************************************/
//General Object
gen_powerconfig_t7_config_t power_config = {0};                 //Power config settings.
gen_acquisitionconfig_t8_config_t acquisition_config = {0};     // Acquisition config. 

//Touch Object
touch_multitouchscreen_t9_config_t touchscreen_config = {0};    //Multitouch screen config.
touch_keyarray_t15_config_t keyarray_config = {0};              //Key array config.
touch_proximity_t23_config_t proximity_config = {0};        //Proximity config. 

//Signal Processing Objects
proci_onetouchgestureprocessor_t24_config_t onetouch_gesture_config = {0};  //One-touch gesture config. 
proci_twotouchgestureprocessor_t27_config_t twotouch_gesture_config = {0};  //Two-touch gesture config.

//Support Objects
spt_comcconfig_t18_config_t   comc_config = {0};            //Communication config settings.
spt_gpiopwm_t19_config_t  gpiopwm_config = {0};             //GPIO/PWM config
spt_selftest_t25_config_t selftest_config = {0};            //Selftest config.

//MXT224E Objects
proci_gripsuppression_t40_config_t              gripsuppression_t40_config = {0};       
proci_touchsuppression_t42_config_t             touchsuppression_t42_config = {0};
spt_cteconfig_t46_config_t                      cte_t46_config = {0};
proci_stylus_t47_config_t                       stylus_t47_config = {0};
procg_noisesuppression_t48_config_t             noisesuppression_t48_config = {0};

/*------------------------------ for tunning ATmel - end ----------------------------*/


#ifdef SKY_PROCESS_CMD_KEY
// To be depreciated.
static struct file_operations ts_fops = {
	.owner = THIS_MODULE,
	.open = ts_fops_open,
	//	.release = ts_fops_close,
	.ioctl = ts_fops_ioctl,
};

static struct miscdevice touch_event = {
	.minor = MISC_DYNAMIC_MINOR,
	.name = "touch_fops",
	.fops = &ts_fops,
};

static int ts_fops_open(struct inode *inode, struct file *filp)
{
	filp->private_data = qt602240_data;
	return 0;
}

typedef enum {	
	TOUCH_IOCTL_READ_LASTKEY=1001,	
	TOUCH_IOCTL_DO_KEY,	
	TOUCH_IOCTL_RELEASE_KEY, 
	TOUCH_IOCTL_CLEAN,
	TOUCH_IOCTL_DEBUG_,
	TOUCH_IOCTL_RESTART,
	TOUCH_IOCTL_PRESS_TOUCH,
	TOUCH_IOCTL_RELEASE_TOUCH,
	TOUCH_IOCTL_CHARGER_MODE,
	POWER_OFF,

	TOUCH_IOCTL_DELETE_ACTAREA = 2001,
	TOUCH_IOCTL_RECOVERY_ACTAREA,
	TOUCH_IOCTL_INIT = 3001,	
	TOUCH_IOCTL_OFF  = 3002,
} TOUCH_IOCTL_CMD;

// To be depreciated.
static int ts_fops_ioctl(struct inode *inode, struct file *filp,
		unsigned int cmd, unsigned long arg)
{
	void __user *argp = (void __user *)arg;
	printk("[QT602240]ts_fops_ioctl(%d, %d) \n",(int)cmd,(int)arg);
	switch (cmd) 
	{
		case TOUCH_IOCTL_READ_LASTKEY:
			break;
		case TOUCH_IOCTL_DO_KEY:
			dbg("TOUCH_IOCTL_DO_KEY  = %d\n",(int)argp);			
			if ( (int)argp == 0x20a )
				input_report_key(qt602240_data->input_dev, 0xe3, 1);
			else if ( (int)argp == 0x20b )
				input_report_key(qt602240_data->input_dev, 0xe4, 1);
			else
				input_report_key(qt602240_data->input_dev, (int)argp, 1);
			break;
		case TOUCH_IOCTL_RELEASE_KEY:		
			dbg("TOUCH_IOCTL_RELEASE_KEY  = %d\n",(int)argp);
			if ( (int)argp == 0x20a )
				input_report_key(qt602240_data->input_dev, 0xe3, 0);
			else if ( (int)argp == 0x20b )
				input_report_key(qt602240_data->input_dev, 0xe4, 0);
			else
				input_report_key(qt602240_data->input_dev, (int)argp, 0);
			break;		
		case TOUCH_IOCTL_DEBUG_:
			dbg("Touch Screen Read Queue ~!!\n");	
			queue_work(qt602240_wq, &qt602240_data->work);
			break;
		case TOUCH_IOCTL_CLEAN:
			dbg("Touch Screen Previous Data Clean ~!!\n");
			clear_event(TSC_CLEAR_ALL);
			break;
		case TOUCH_IOCTL_RESTART:
			dbg("Touch Screen Calibration Restart ~!!\n");			
			calibrate_chip();
			break;
		case TOUCH_IOCTL_PRESS_TOUCH:
			input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_X, (int)(arg&0x0000FFFF));
			input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_Y, (int)((arg >> 16) & 0x0000FFFF));
			input_report_abs(qt602240_data->input_dev, ABS_MT_TOUCH_MAJOR, 255);
			input_report_abs(qt602240_data->input_dev, ABS_MT_WIDTH_MAJOR, 1);
			input_mt_sync(qt602240_data->input_dev);
			input_sync(qt602240_data->input_dev);
			break;
		case TOUCH_IOCTL_RELEASE_TOUCH:		
			input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_X, (int)(arg&0x0000FFFF));
			input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_Y, (int)((arg >> 16) & 0x0000FFFF));
			input_report_abs(qt602240_data->input_dev, ABS_MT_TOUCH_MAJOR, 0);
			input_report_abs(qt602240_data->input_dev, ABS_MT_WIDTH_MAJOR, 1);
			input_mt_sync(qt602240_data->input_dev);
			input_sync(qt602240_data->input_dev); 
			break;			
		case TOUCH_IOCTL_CHARGER_MODE:
			qt_charger_mode_config(arg);
			break;
		case POWER_OFF:
			pm_power_off();
			break;
		case TOUCH_IOCTL_DELETE_ACTAREA:
			touchscreen_config.yloclip = 0;		// Change Active area
			touchscreen_config.yhiclip = 0;
			if (write_multitouchscreen_config(0, touchscreen_config) != CFG_WRITE_OK)
			{
				dbg("qt_Multitouchscreen_config Error!!!\n");
			}
			break;
		case TOUCH_IOCTL_RECOVERY_ACTAREA:
			touchscreen_config.yloclip = 15;	// Change Active area
			touchscreen_config.yhiclip = 15;
			if (write_multitouchscreen_config(0, touchscreen_config) != CFG_WRITE_OK)
			{
				dbg("qt_Multitouchscreen_config Error!!!\n");
			}
			break;
    case TOUCH_IOCTL_INIT:
			printk("Touch init \n");
			qt602240_front_test_init();
			break;
		default:
			break;
	}

	return true;
}
#endif

#ifdef TOUCH_IO
void print_touch_info(void)
{
	info_id_t *id;
	id = (info_id_t *) kmalloc(sizeof(info_id_t), GFP_KERNEL | GFP_ATOMIC);
	read_id_block(id);
}
void print_touch_reg(void)
{
	dbg("touchscreen_config tchthr : %d\n",  touchscreen_config.tchthr);
	dbg("touchscreen_config tchdi : %d\n",  touchscreen_config.tchdi);
	dbg("touchscreen_config blen : %d\n",  touchscreen_config.blen);
	dbg("touchscreen_config orient : %d\n",  touchscreen_config.orient);
	dbg("touchscreen_config tchhyst : %d\n",  touchscreen_config.tchhyst);
	dbg("noisesuppression_t48_config blen : %d\n",  noisesuppression_t48_config.blen);
	dbg("noisesuppression_t48_config calcfg : %d\n",  noisesuppression_t48_config.calcfg);
	dbg("noisesuppression_t48_config cfg : %d\n",  noisesuppression_t48_config.cfg);
	dbg("noisesuppression_t48_config tchthr : %d\n",  noisesuppression_t48_config.tchthr);
	dbg("noisesuppression_t48_config tchdi : %d\n",  noisesuppression_t48_config.tchdi);
	dbg("noisesuppression_t48_config tchhyst : %d\n",  noisesuppression_t48_config.tchhyst);	
}
void reset_touch_info(void)
{
}

int reference_data[224] = { 0 };
int diag_debug(int command) 
{
	/*command 0x10: Delta, 0x11: Reference*/
	uint8_t data_buffer[130] = { 0 };
	uint8_t try_ctr = 0;
	uint8_t data_byte = 0; /* dianostic command to get touch refs*/
	uint16_t diag_address;

	uint8_t page;
	uint8_t i;
	uint8_t j;
	uint16_t value;
	int16_t signed_value;

	dbg_func_in();
	disable_irq(qt602240_data->client->irq);

	if (driver_setup != DRIVER_SETUP_OK)
		return -1;

	/* read touch flags from the diagnostic object - clear buffer so the while loop can run first time */
	diagnostic_chip(command);
	msleep(20); 

	/* read touch flags from the diagnostic object - clear buffer so the while loop can run first time */
	memset( data_buffer , 0xFF, sizeof( data_buffer ) );
	diag_address = get_object_address(DEBUG_DIAGNOSTIC_T37,0);
	

	/* data array is 20 x 16 bits for each set of flags, 2 byte header, 40 bytes for touch flags 40 bytes for antitouch flags*/

	/* count up the channels/bits if we recived the data properly */
	//printk("[TSP] DIAG! \n");
		j = 0;
	// ceil (224 / (128/2) )
	for (page = 0; page < 4; page++) {
		/* wait for diagnostic object to update */
		while(!((data_buffer[0] == command) && (data_buffer[1] == page)))
		{
			/* wait for data to be valid  */
			if(try_ctr > 100)
			{
				/* Failed! */
				//dbg("[TSP] Diagnostic Data did not update!!\n");
				break;
			}
			msleep(5); 
			try_ctr++; /* timeout counter */
			read_mem(diag_address, 2,data_buffer);
			//printk("[TSP] Waiting for diagnostic data to update, try %d\n", try_ctr);
		}
		/* data is ready - read the detection flags */
		read_mem(diag_address, 130,data_buffer);
		//printk("[TSP] CMD: %x Page: %x \n", data_buffer[0],data_buffer[1]);
		if((data_buffer[0] == command) && (data_buffer[1] == page))
		{

			/* count the channels and print the flags to the log */
			for(i = 0; i < 128; i+=2) /* check X lines - data is in words so increment 2 at a time */
			{
				
				value =  (data_buffer[3+i]<<8) + data_buffer[2+i];
			        if (command == 0x11)
				{
					value = value - 0x4000;
					reference_data[j] = value;
				}
				else if (command == 0x10)
				{
					signed_value = value;
					reference_data[j] = signed_value;
				}
				else 
				{
					reference_data[j] = value;
				}
				j++;
				
				/* print the flags to the log - only really needed for debugging */
				//printk("%05d " , value);
				if(j % touchscreen_config.xsize == 0)
				{
					//printk("\n");
				}

			}

		}
		else 
		{
			printk("[TSP]ERROR!!");
		}
		/* send page up command so we can detect when data updates next time,
		 * page byte will sit at 1 until we next send F3 command */
		data_byte = 0x01;
		write_mem(command_processor_address + DIAGNOSTIC_OFFSET, 1, &data_byte);

	}

	enable_irq(qt602240_data->client->irq);
	return 0;
}

static struct file_operations fops = 
{
	.owner =    THIS_MODULE,
	.ioctl =    ioctl,
	.read =     read,
	.write =    write,
	.open =     open,
	.release =  release
};

static struct miscdevice touch_io = 
{
	.minor =    MISC_DYNAMIC_MINOR,
	.name =     "qt602240",
	.fops =     &fops
};

static int open(struct inode *inode, struct file *file) 
{
	return 0; 
}

static int release(struct inode *inode, struct file *file) 
{
	return 0; 
}
static ssize_t write(struct file *file, const char *buf, size_t count, loff_t *ppos)
{
	int nBufSize=0;
	if((size_t)(*ppos) > 0) return 0;
	if(buf!=NULL)
	{
		nBufSize=strlen(buf);
		if(strncmp(buf, "queue", 5)==0)
		{
			queue_work(qt602240_wq, &qt602240_data->work);
		}
		if(strncmp(buf, "debug", 5)==0)
		{			
			DebugON=1;	 
		}
		if(strncmp(buf, "debugoff", 8)==0)
		{			
			DebugON=0;	    
		}
		if(strncmp(buf, "checkcal", 8)==0)
		{			
			check_chip_calibration();
		}
		if(strncmp(buf, "cal", 3)==0)
		{			
			calibrate_chip();
		}
		if(strncmp(buf, "resettouch", 10)==0)
		{			
			reset_touch_info();
		}
		if(strncmp(buf, "printtouch", 10)==0)
		{			
			print_touch_reg();
		}
		if(strncmp(buf, "infotouch", 9)==0)
		{			
			print_touch_info();
		}
		if(strncmp(buf, "save", 4)==0)
		{			
			backup_config();	    
		}
		if(strncmp(buf, "reference", 9)==0)
		{			
			diag_debug(0x11);	    
		}
		if(strncmp(buf, "delta", 5)==0)
		{			
			diag_debug(0x10);	    
		}
	}
	*ppos +=nBufSize;
	return nBufSize;
}
static ssize_t read(struct file *file, char *buf, size_t count, loff_t *ppos)
{
	return 0; 
}


/*
 * Touch Config Table
 */
typedef enum {
	UINT8 = 0,
	UINT16 = 1,
	INT8 = 2,
	INT16 = 3,
}enum_size;

typedef struct {
	int* value;
	uint8_t size;
}config_table_element;

config_table_element t7_power_config_table[] = { 
	{(int*)&power_config.idleacqint, UINT8},
	{(int*)&power_config.actvacqint, UINT8},
	{(int*)&power_config.actv2idleto, UINT8}
};

config_table_element t8_acquisition_config_table[] = { 
	{(int*)&acquisition_config.chrgtime, UINT8},
	{(int*)&acquisition_config.reserved, UINT8},
	{(int*)&acquisition_config.tchdrift, UINT8},
	{(int*)&acquisition_config.driftst, UINT8},
	{(int*)&acquisition_config.tchautocal, UINT8},
	{(int*)&acquisition_config.sync, UINT8},
	{(int*)&acquisition_config.atchcalst, UINT8},
	{(int*)&acquisition_config.atchcalsthr, UINT8},
	{(int*)&acquisition_config.atchfrccalthr, UINT8},
	{(int*)&acquisition_config.atchfrccalratio, INT8},
};

config_table_element t9_touchscreen_config_table[] = {
	{(int*)&touchscreen_config.ctrl, UINT8},
	{(int*)&touchscreen_config.xorigin, UINT8},
	{(int*)&touchscreen_config.yorigin, UINT8},
	{(int*)&touchscreen_config.xsize, UINT8},
	{(int*)&touchscreen_config.ysize, UINT8},
	{(int*)&touchscreen_config.akscfg, UINT8},
	{(int*)&touchscreen_config.blen, UINT8},
	{(int*)&touchscreen_config.tchthr, UINT8},
	{(int*)&touchscreen_config.tchdi, UINT8},
	{(int*)&touchscreen_config.orient, UINT8},
	{(int*)&touchscreen_config.mrgtimeout, UINT8},
	{(int*)&touchscreen_config.movhysti, UINT8},
	{(int*)&touchscreen_config.movhystn, UINT8},
	{(int*)&touchscreen_config.movfilter, UINT8},
	{(int*)&touchscreen_config.numtouch, UINT8},
	{(int*)&touchscreen_config.mrghyst, UINT8},
	{(int*)&touchscreen_config.mrgthr, UINT8},
	{(int*)&touchscreen_config.amphyst, UINT8},
	{(int*)&touchscreen_config.xrange, UINT16},
	{0},
	{(int*)&touchscreen_config.yrange, UINT16},
	{0},
	{(int*)&touchscreen_config.xloclip, UINT8},
	{(int*)&touchscreen_config.xhiclip, UINT8},
	{(int*)&touchscreen_config.yloclip, UINT8},
	{(int*)&touchscreen_config.yhiclip, UINT8},
	{(int*)&touchscreen_config.xedgectrl, UINT8},
	{(int*)&touchscreen_config.xedgedist, UINT8},
	{(int*)&touchscreen_config.yedgectrl, UINT8},
	{(int*)&touchscreen_config.yedgedist, UINT8},
	{(int*)&touchscreen_config.jumplimit, UINT8},
	{(int*)&touchscreen_config.tchhyst, UINT8},
	{(int*)&touchscreen_config.xpitch, UINT8},
	{(int*)&touchscreen_config.ypitch, UINT8},
	{(int*)&touchscreen_config.nexttchdi, UINT8},
};

config_table_element t46_cteconfig_config_table[] = {
	{(int*)&cte_t46_config.ctrl, UINT8},
	{(int*)&cte_t46_config.mode, UINT8},
	{(int*)&cte_t46_config.idlesyncsperx, UINT8},
	{(int*)&cte_t46_config.actvsyncsperx, UINT8},
	{(int*)&cte_t46_config.adcspersync, UINT8},
	{(int*)&cte_t46_config.pulsesperadc, UINT8}, 
	{(int*)&cte_t46_config.xslew, UINT8},  
	{(int*)&cte_t46_config.syncdelay, UINT16},
	{0},
};

config_table_element t48_noisesuppression_config_table[] = {
	{(int*)&noisesuppression_t48_config.ctrl, UINT8},
	{(int*)&noisesuppression_t48_config.cfg, UINT8},
	{(int*)&noisesuppression_t48_config.calcfg, UINT8},
	{(int*)&noisesuppression_t48_config.basefreq, UINT8},
	{(int*)&noisesuppression_t48_config.freq_0, UINT8},
	{(int*)&noisesuppression_t48_config.freq_1, UINT8},
	{(int*)&noisesuppression_t48_config.freq_2, UINT8},
	{(int*)&noisesuppression_t48_config.freq_3, UINT8},
	{(int*)&noisesuppression_t48_config.mffreq_2, UINT8},
	{(int*)&noisesuppression_t48_config.mffreq_3, UINT8},
	{(int*)&noisesuppression_t48_config.nlgain, UINT8},
	{(int*)&noisesuppression_t48_config.nlthr, UINT8},
	{(int*)&noisesuppression_t48_config.gclimit, UINT8},
	{(int*)&noisesuppression_t48_config.gcactvinvldadcs, UINT8},
	{(int*)&noisesuppression_t48_config.gcidleinvldadcs, UINT8},
	{(int*)&noisesuppression_t48_config.gcinvalidthr, UINT16},
	{0},
	{(int*)&noisesuppression_t48_config.gcmaxadcsperx, UINT8},
	{(int*)&noisesuppression_t48_config.gclimitmin, UINT8},
	{(int*)&noisesuppression_t48_config.gclimitmax, UINT8},
	{(int*)&noisesuppression_t48_config.gccountmintgt, UINT16},
	{0},
	{(int*)&noisesuppression_t48_config.mfinvlddiffthr, UINT8},
	{(int*)&noisesuppression_t48_config.mfincadcspxthr, UINT16},
	{0},
	{(int*)&noisesuppression_t48_config.mferrorthr, UINT16},
	{0},
	{(int*)&noisesuppression_t48_config.selfreqmax, UINT8},
	{(int*)&noisesuppression_t48_config.reserved9, UINT8},
	{(int*)&noisesuppression_t48_config.reserved10, UINT8},
	{(int*)&noisesuppression_t48_config.reserved11, UINT8},
	{(int*)&noisesuppression_t48_config.reserved12, UINT8},
	{(int*)&noisesuppression_t48_config.reserved13, UINT8},
	{(int*)&noisesuppression_t48_config.reserved14, UINT8},
	{(int*)&noisesuppression_t48_config.blen, UINT8},
	{(int*)&noisesuppression_t48_config.tchthr, UINT8},
	{(int*)&noisesuppression_t48_config.tchdi, UINT8},
	{(int*)&noisesuppression_t48_config.movhysti, UINT8},
	{(int*)&noisesuppression_t48_config.movhystn, UINT8},
	{(int*)&noisesuppression_t48_config.movfilter, UINT8},
	{(int*)&noisesuppression_t48_config.numtouch, UINT8},
	{(int*)&noisesuppression_t48_config.mrghyst, UINT8},
	{(int*)&noisesuppression_t48_config.mrgthr, UINT8},
	{(int*)&noisesuppression_t48_config.xloclip, UINT8},
	{(int*)&noisesuppression_t48_config.xhiclip, UINT8},
	{(int*)&noisesuppression_t48_config.yloclip, UINT8},
	{(int*)&noisesuppression_t48_config.yhiclip, UINT8},
	{(int*)&noisesuppression_t48_config.xedgectrl, UINT8},
	{(int*)&noisesuppression_t48_config.xedgedist, UINT8},
	{(int*)&noisesuppression_t48_config.yedgectrl, UINT8},
	{(int*)&noisesuppression_t48_config.yedgedist, UINT8},
	{(int*)&noisesuppression_t48_config.jumplimit, UINT8},
	{(int*)&noisesuppression_t48_config.tchhyst, UINT8},
	{(int*)&noisesuppression_t48_config.nexttchdi, UINT8},
};

config_table_element* config_table[50] = {
	/* 0~9 */
	0,0,0,0,0,0,0, 
	(config_table_element*)&t7_power_config_table,
	(config_table_element*)&t8_acquisition_config_table,
	(config_table_element*)&t9_touchscreen_config_table,
	/* 10~19 */
	0,0,0,0,0,0,0,0,0,0,
	/* 20~29 */
	0,0,0,0,0,0,0,0,0,0,
	/* 30~39 */
	0,0,0,0,0,0,0,0,0,0,
	/* 40~49 */
	0,0,0,0,0,0,
	(config_table_element*)&t46_cteconfig_config_table,
	0,
	(config_table_element*)&t48_noisesuppression_config_table,
	0,
};


static int ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	config_table_element config;
	int return_value = -1;
	int data, object_type, field_index;
	// Read Command 
	// Write, Etc.
	switch (cmd)
	{
		case SET_TOUCH_CONFIG:
			data		= (int)((arg & 0xFFFF0000) >> 16);
			object_type 	= (int)((arg & 0x0000FF00) >> 8);
			field_index 	= (int)((arg & 0x000000FF) >> 0);
			if (config_table[object_type] == 0) {
				printk("[TSP] Error! undefined object type! %d\n", object_type);
				break;
			}
			config = config_table[object_type][field_index];
			if (config.size == UINT8) {
				*((uint8_t*)config_table[object_type][field_index].value) = data;
			}
			else if (config.size == UINT16) {
				*((uint16_t*)config_table[object_type][field_index].value) = data;
			}
			else if (config.size == INT8) {
				*((int8_t*)config_table[object_type][field_index].value) = data;
			}
			else if (config.size == INT16) {
				*((int16_t*)config_table[object_type][field_index].value) = data;
			}
			else {
				// Error
			}
			printk("[TSP] set %d-%d with %d\n", object_type, field_index, data);
			break;
		case GET_TOUCH_CONFIG:
			object_type 	= (int)((arg & 0x0000FF00) >> 8);
			field_index 	= (int)((arg & 0x000000FF) >> 0);
			if (config_table[object_type] == 0) {
				//printk("[TSP] Error! undefined object type! %d\n", object_type);
				break;
			}
			config = config_table[object_type][field_index];
			if (config.size == UINT8) {
				return_value = (uint8_t)*(config.value);
			}
			else if (config.size == UINT16) {
				return_value = (uint16_t)*(config.value);
			}
			else if (config.size == INT8) {
				return_value = (int8_t)*(config.value);
			}
			else if (config.size == INT16) {
				return_value = (int16_t)*(config.value);
			}
			else {
				// Error
			}
			return return_value;
			break;
		case APPLY_TOUCH_CONFIG:
			apply_touch_config();
			break;
		case RESET_TOUCH_CONFIG:
			reset_touch_config();
			break;
#ifdef ITO_TYPE_CHECK
		case READ_ITO_TYPE:
			return tsp_ito_type;
			break;
#endif
		case TOUCH_IOCTL_DEBUG:
			return ioctl_debug(arg);
			break;
		case TOUCH_CHARGER_MODE:
			dbg("Touch charger mode change \n");
			qt_charger_mode_config(arg);
			break;
#ifdef LOCKSCREEN_MODE
		case TOUCH_LOCKSCREEN_MODE_ENABLE:
			queue_work(qt602240_wq, &qt602240_data->work_lockscreen_mode_enable);
			printk("[TSP]Touch Lockscreen Mode Enable.\n");
			break;
		case TOUCH_LOCKSCREEN_MODE_DISABLE:
			printk("[TSP]Touch Lockscreen Mode Disable.\n");
			queue_work(qt602240_wq, &qt602240_data->work_lockscreen_mode_disable);
			break;
#endif
			//Factory Cmd
		case TOUCH_IOCTL_READ_LASTKEY:
			break;
		case TOUCH_IOCTL_DO_KEY:
			input_report_key(qt602240_data->input_dev, (int)arg, 1);
			break;
		case TOUCH_IOCTL_RELEASE_KEY:		
			input_report_key(qt602240_data->input_dev, (int)arg, 0);
			break;
		case TOUCH_IOCTL_INIT:
			printk("Touch init \n");
			qt602240_front_test_init();
			break;
		case TOUCH_IOCTL_OFF:
			printk("Touch off \n");
			off_hw_setting();
			break;    
		case DIAG_DEBUG:
			/*
			 * Run Diag and save result into reference_data array when arg. is 5010 or 5011. 
			 * Returns diag result when the arg. is in range of 0~223. 
			 */
			if (arg == 5010) 
			{
				diag_debug(0x10);
				return 0;
			}
			if (arg == 5011) 
			{
				diag_debug(0x11);
				return 0;
			}
			else if (arg > 224-1)
			{
				printk("[TSP] ERROR!");
				return 0;
			}
			//printk("[TSP] get ref: %d \n", reference_data[arg]);
			return reference_data[arg];

		default:
			break;
	}
	return 0;
}

typedef enum  {
	IOCTL_DEBUG_SUSPEND = 0,	
	IOCTL_DEBUG_RESUME = 1,	
	IOCTL_DEBUG_GET_TOUCH_ANTITOUCH_INFO = 2,
	IOCTL_DEBUG_TCH_CH= 3,	
	IOCTL_DEBUG_ATCH_CH= 4,	
} ioctl_debug_cmd;
static int ioctl_debug(unsigned long arg) 
{
	switch (arg)
	{
		case IOCTL_DEBUG_SUSPEND:
			qt602240_early_suspend(NULL);
			break;
		case IOCTL_DEBUG_RESUME:
			qt602240_late_resume(NULL);
			break;
		case IOCTL_DEBUG_GET_TOUCH_ANTITOUCH_INFO:
			check_chip_calibration();
			return get_touch_antitouch_info();
			break;
		case IOCTL_DEBUG_TCH_CH:
			return debugInfo.tch_ch;
			break;
		case IOCTL_DEBUG_ATCH_CH:
			return debugInfo.atch_ch;
			break;
		default:
			break;
	}
	return 0;
}
static void apply_touch_config(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	if (write_power_config(power_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}


	dbg("xxxx acquisition config\n");
	if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg("xxxx tchthr : %d",  touchscreen_config.tchthr);
	dbg("xxxx multitouchscreen config\n");
	if (write_multitouchscreen_config(0, touchscreen_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg("xxxx keyarray config \n");
	if (write_keyarray_config(0, keyarray_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg("xxxx comc config \n");
	if (get_object_address(SPT_COMCONFIG_T18, 0) != OBJECT_NOT_FOUND)
	{
		if (write_comc_config(0, comc_config) != CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	dbg("xxxx gpio pwm config \n");
	if (write_gpio_config(0, gpiopwm_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}	

	dbg("xxxx proximity config \n");
	if (get_object_address(TOUCH_PROXIMITY_T23, 0) != OBJECT_NOT_FOUND)
	{
		if (write_proximity_config(0, proximity_config) != CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	/*
	   dbg("xxxx one touch gesture config \n");
	   if (get_object_address(PROCI_ONETOUCHGESTUREPROCESSOR_T24, 0) !=OBJECT_NOT_FOUND)
	   {
	   if (write_onetouchgesture_config(0, onetouch_gesture_config) !=CFG_WRITE_OK)
	   {
	   dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	   }
	   }
	   */
	dbg("xxxx self test config \n");
	if (get_object_address(SPT_SELFTEST_T25, 0) != OBJECT_NOT_FOUND)
	{
		if (write_selftest_config(0,selftest_config) != CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	/*
	   dbg("xxxx two touch config \n");
	   if (get_object_address(PROCI_TWOTOUCHGESTUREPROCESSOR_T27, 0) !=OBJECT_NOT_FOUND)
	   {
	   if (write_twotouchgesture_config(0, twotouch_gesture_config) !=CFG_WRITE_OK)
	   {
	   dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	   }
	   }
	   */
#if 0
	if(info_block->info_id.version < 0x20)  //V2.0
	{
		dbg("xxxx two touch config \n");
		if (get_object_address(PROCI_TWOTOUCHGESTUREPROCESSOR_T27, 0) !=OBJECT_NOT_FOUND)
		{
			if (write_twotouchgesture_config(0, twotouch_gesture_config) !=CFG_WRITE_OK)
			{
				dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
			}
		}
	}
#endif        

	if (get_object_address(PROCI_GRIPSUPPRESSION_T40, 0) != OBJECT_NOT_FOUND)
	{
		if (write_gripsuppression_T40_config(gripsuppression_t40_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	if (get_object_address(PROCI_TOUCHSUPPRESSION_T42, 0) != OBJECT_NOT_FOUND)
	{
		if (write_touchsuppression_t42_config(touchsuppression_t42_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	if (get_object_address(SPT_CTECONFIG_T46, 0) != OBJECT_NOT_FOUND)
	{
		if (write_CTE_T46_config(cte_t46_config) != CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	if (get_object_address(PROCI_STYLUS_T47, 0) != OBJECT_NOT_FOUND)
	{
		if (write_stylus_t47_config(stylus_t47_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	if (get_object_address(PROCG_NOISESUPPRESSION_T48, 0) != OBJECT_NOT_FOUND)
	{
		if (write_noisesuppression_t48_config(noisesuppression_t48_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	dbg_func_out();
}

#ifdef CHARGER_MODE
static uint8_t qt_charger_mode_config(unsigned long mode)
{
	uint8_t status = 0;
	uint16_t object_address_t7, object_address_t48= 0;

	dbg_func_in();
	switch (mode)
	{
		case BATTERY_PLUGGED_NONE:
		case BATTERY_PLUGGED_AC:
		case BATTERY_PLUGGED_USB:
			break;
		default:
			return 0;
			break;
	}

	dbg("[TSP] qt_charger_mode_config [charger_mode=%d, previous_charger_mode=%d] \n", (int)mode, (int)previous_charger_mode);
	if(tsp_power_status == 0)
		return 0;

	if (driver_setup != DRIVER_SETUP_OK)
		return 0;

	if(previous_charger_mode == mode)
		return 0;
	previous_charger_mode = mode;

	switch (mode)
	{
		case BATTERY_PLUGGED_NONE:
			dbg("Touch charger mode BATTERY_PLUGGED_NONE \n");
			noisesuppression_t48_config.calcfg = T48_CALCFG;
			power_config.idleacqint = T7_IDLEACQINT;
			break;

		case BATTERY_PLUGGED_AC:
		case BATTERY_PLUGGED_USB:
			dbg("Touch charger mode BATTERY_PLUGGED \n");
			noisesuppression_t48_config.calcfg = T48_CALCFG_PLUG;
			dbg("noisesuppression_t48_config.calcfg = %d\n",noisesuppression_t48_config.calcfg);
			power_config.idleacqint = T7_IDLEACQINT_PLUG;
			break;
		default:
			break;
	}

	object_address_t7 = get_object_address(GEN_POWERCONFIG_T7, 0);
	dbg("object_address_t7 = %d\n",object_address_t7);
	if (object_address_t7 == 0)
		return(CFG_WRITE_FAILED);

	object_address_t48 = get_object_address(PROCG_NOISESUPPRESSION_T48, 0);
	dbg("object_address_t48 = %d\n",object_address_t48);
	if (object_address_t48 == 0)
		return(CFG_WRITE_FAILED);
	mutex_lock(&qt602240_data->lock);

	status |= write_mem(object_address_t7, 1, &power_config.idleacqint);

	if (write_noisesuppression_t48_config(noisesuppression_t48_config) !=
				CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg("noisesuppression_t48_config.calcfg = %d\n",noisesuppression_t48_config.calcfg);
		
	mutex_unlock(&qt602240_data->lock);

	dbg_func_out();
	return status;
}

#endif // CHARGER_MODE
#endif //TOUCH_IO



/*****************************************************************************
 *
 *       QT602240  Configuration Block
 *
 * ***************************************************************************/

void qt_Power_Sleep(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	/* Set Idle Acquisition Interval to 32 ms. */
	power_config.idleacqint = 0;

	/* Set Active Acquisition Interval to 16 ms. */
	power_config.actvacqint = 0;

	/* Set Active to Idle Timeout to 4 s (one unit = 200ms). */
	power_config.actv2idleto = 0;

	/* Write power config to chip. */
	if (write_power_config(power_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg_func_out();
}

void qt_Power_Config_Init(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	power_config.idleacqint = T7_IDLEACQINT;
	power_config.actvacqint = T7_ACTVACQINT;
	power_config.actv2idleto = T7_ACTV2IDLETO;

	/* Write power config to chip. */
	if (write_power_config(power_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg_func_out();
}

void qt_Acquisition_Config_Init(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	acquisition_config.chrgtime = T8_CHRGTIME; // 2.25us
	acquisition_config.reserved = 0;
	acquisition_config.tchdrift = T8_TCHDRIFT; // 4s
	acquisition_config.driftst = T8_DRIFTST; // 1s
	acquisition_config.tchautocal = T8_TCHAUTOCAL; // infinite
	acquisition_config.sync = T8_SYNC; // disabled
	acquisition_config.atchcalst = T8_ATCHCALST/*2*/;
	acquisition_config.atchcalsthr = T8_ATCHCALSTHR/*50*/;

	acquisition_config.atchfrccalthr = T8_ATCHFRCCALTHR;     /*!< Anti-touch force calibration threshold */
	acquisition_config.atchfrccalratio = T8_ATCHFRCCALRATIO;  /*!< Anti-touch force calibration ratio */  

	if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg_func_out();
}

void qt_Acquisition_Config_Init_locked(struct work_struct * p) {
	mutex_lock(&qt602240_data->lock);
	qt_Acquisition_Config_Init();
	mutex_unlock(&qt602240_data->lock);
}

void qt_Multitouchscreen_Init(void)
{
	dbg_func_in();
	if (driver_setup != DRIVER_SETUP_OK)
		return;

	touchscreen_config.ctrl = T9_CTRL;
	touchscreen_config.movhysti = T9_MOVHYSTI;
	touchscreen_config.movhystn = T9_MOVHYSTN;
	touchscreen_config.mrghyst = T9_MRGHYST;
	touchscreen_config.mrgthr = T9_MRGTHR;
	touchscreen_config.amphyst = T9_AMPHYST;
	touchscreen_config.yloclip = T9_YLOCLIP;
	touchscreen_config.yhiclip = T9_YHICLIP;
	touchscreen_config.jumplimit = T9_JUMPLIMIT;
	touchscreen_config.xpitch = T9_XPITCH;
	touchscreen_config.ypitch =  T9_YPITCH;
	touchscreen_config.nexttchdi =  T9_NEXTTCHDI;
	touchscreen_config.blen = T9_BLEN;
#ifdef ITO_TYPE_CHECK
	touchscreen_config.tchthr = T9_TCHTHR[tsp_ito_type];
#else
	touchscreen_config.tchthr = T9_TCHTHR;
#endif
	touchscreen_config.movfilter = T9_MOVFILTER;
	touchscreen_config.tchdi = T9_TCHDI;
#ifdef ITO_TYPE_CHECK
	touchscreen_config.tchhyst = T9_TCHHYST[tsp_ito_type];   // 25% of tchthr
#else
	touchscreen_config.tchhyst = T9_TCHHYST;   // 25% of tchthr
#endif
	touchscreen_config.xorigin = T9_XORIGIN;
	touchscreen_config.yorigin = T9_YORIGIN;
	touchscreen_config.xsize = T9_XSIZE;
	touchscreen_config.ysize = T9_YSIZE;
	touchscreen_config.akscfg = T9_AKSCFG;
	touchscreen_config.orient = T9_ORIENT;
	touchscreen_config.mrgtimeout = T9_MRGTIMEOUT;
	touchscreen_config.numtouch= T9_NUMTOUCH;
	touchscreen_config.xrange = T9_XRANGE;
	touchscreen_config.yrange = T9_YRANGE;
	touchscreen_config.xloclip = T9_XLOCLIP;
	touchscreen_config.xhiclip = T9_XHICLIP;
	touchscreen_config.xedgectrl = T9_XEDGECTRL;
	touchscreen_config.xedgedist = T9_XEDGEDIST;
	touchscreen_config.yedgectrl = T9_YEDGECTRL;
	touchscreen_config.yedgedist = T9_YEDGEDIST;


	if (write_multitouchscreen_config(0, touchscreen_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg_func_out();
}

void qt_KeyArray_Init(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	keyarray_config.ctrl = T15_CTRL;
	keyarray_config.xorigin = T15_XORIGIN;
	keyarray_config.yorigin = T15_YORIGIN;
	keyarray_config.xsize = T15_XSIZE;
	keyarray_config.ysize = T15_YSIZE;
	keyarray_config.akscfg = T15_AKSCFG;
	keyarray_config.blen = T15_BLEN;
	keyarray_config.tchthr = T15_TCHTHR;
	keyarray_config.tchdi = T15_TCHDI;
	keyarray_config.reserved[0] = T15_RESERVED_0;
	keyarray_config.reserved[1] = T15_RESERVED_1;

	if (write_keyarray_config(0, keyarray_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg_func_out();
}

void qt_ComcConfig_Init(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	comc_config.ctrl = T18_CTRL;
	comc_config.cmd = NO_COMMAND;

	if (get_object_address(SPT_COMCONFIG_T18, 0) != OBJECT_NOT_FOUND)
	{
		if (write_comc_config(0, comc_config) != CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	dbg_func_out();
}

void qt_Gpio_Pwm_Init(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	gpiopwm_config.ctrl = T19_CTRL;
	gpiopwm_config.reportmask = T19_REPORTMASK;
	gpiopwm_config.dir = T19_DIR;
	gpiopwm_config.intpullup = T19_INTPULLUP;
	gpiopwm_config.out = T19_OUT;
	gpiopwm_config.wake = T19_WAKE;
	gpiopwm_config.pwm = T19_PWM;
	gpiopwm_config.period = T19_PERIOD;
	gpiopwm_config.duty[0] = T19_DUTY_0;
	gpiopwm_config.duty[1] = T19_DUTY_1;
	gpiopwm_config.duty[2] = T19_DUTY_2;
	gpiopwm_config.duty[3] = T19_DUTY_3;
	gpiopwm_config.trigger[0] = T19_TRIGGER_0;
	gpiopwm_config.trigger[1] = T19_TRIGGER_1;
	gpiopwm_config.trigger[2] = T19_TRIGGER_2;
	gpiopwm_config.trigger[3] = T19_TRIGGER_3;

	if (write_gpio_config(0, gpiopwm_config) != CFG_WRITE_OK)
	{
		dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
	}

	dbg_func_out();
}

void qt_Proximity_Config_Init(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	proximity_config.ctrl = T23_CTRL;
	proximity_config.xorigin = T23_XORIGIN;
	proximity_config.yorigin = T23_YORIGIN;    
	proximity_config.xsize = T23_XSIZE;
	proximity_config.ysize = T23_YSIZE;
	proximity_config.reserved_for_future_aks_usage = T23_RESERVED;
	proximity_config.blen = T23_BLEN;
	proximity_config.fxddthr = T23_FXDDTHR; 
	proximity_config.fxddi = T23_FXDDI; 
	proximity_config.average = T23_AVERAGE;     
	proximity_config.mvnullrate = T23_MVNULLRATE; 
	proximity_config.mvdthr = T23_MVDTHR; 

	if (get_object_address(TOUCH_PROXIMITY_T23, 0) != OBJECT_NOT_FOUND)
	{
		if (write_proximity_config(0, proximity_config) != CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	dbg_func_out();
}

void qt_One_Touch_Gesture_Config_Init(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	/* Disable one touch gestures. */
	onetouch_gesture_config.ctrl = T24_CTRL;
	onetouch_gesture_config.numgest = T24_NUMGEST;

	onetouch_gesture_config.gesten = T24_GESTEN;
	onetouch_gesture_config.pressproc = T24_PROCESS;
	onetouch_gesture_config.tapto = T24_TAPTO;
	onetouch_gesture_config.flickto = T24_FLICKTO;
	onetouch_gesture_config.dragto = T24_DRAGTO;
	onetouch_gesture_config.spressto = T24_SPRESSTO;
	onetouch_gesture_config.lpressto = T24_LPRESSTO;
	onetouch_gesture_config.reppressto = T24_REPPRESSTO;
	onetouch_gesture_config.flickthr = T24_FLICKTHR;
	onetouch_gesture_config.dragthr = T24_DRAGTHR;
	onetouch_gesture_config.tapthr = T24_TAPTHR;
	onetouch_gesture_config.throwthr = T24_THROWTHR;

	if (get_object_address(PROCI_ONETOUCHGESTUREPROCESSOR_T24, 0) !=
			OBJECT_NOT_FOUND)
	{
		if (write_onetouchgesture_config(0, onetouch_gesture_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	dbg_func_out();
}

void qt_Selftest_Init(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	selftest_config.ctrl = T25_CTRL;
	selftest_config.cmd = T25_CMD;

	if (get_object_address(SPT_SELFTEST_T25, 0) != OBJECT_NOT_FOUND)
	{
		if (write_selftest_config(0,selftest_config) != CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	dbg_func_out();
}

void qt_Two_touch_Gesture_Config_Init(void)
{
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	/* Disable two touch gestures. */
	twotouch_gesture_config.ctrl = T27_CTRL;
	twotouch_gesture_config.numgest = T27_NUMGEST;
	twotouch_gesture_config.reserved2 = T27_RESERVED_0;
	twotouch_gesture_config.gesten = T27_GESTEN;
	twotouch_gesture_config.rotatethr = T27_ROTATETHR;
	twotouch_gesture_config.zoomthr = T27_ZOOMTHR;

	if (get_object_address(PROCI_TWOTOUCHGESTUREPROCESSOR_T27, 0) !=
			OBJECT_NOT_FOUND)
	{
		if (write_twotouchgesture_config(0, twotouch_gesture_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}

	dbg_func_out();
}

void qt_Grip_Suppression_T40_Config_Init(void)
{
	gripsuppression_t40_config.ctrl     = T40_CTRL;
	gripsuppression_t40_config.xlogrip  = T40_XLOGRIP;
	gripsuppression_t40_config.xhigrip  = T40_XHIGRIP;
	gripsuppression_t40_config.ylogrip  = T40_YLOGRIP;
	gripsuppression_t40_config.yhigrip  = T40_YHIGRIP;

	/* Write grip suppression config to chip. */
	if (get_object_address(PROCI_GRIPSUPPRESSION_T40, 0) != OBJECT_NOT_FOUND)
	{
		if (write_gripsuppression_T40_config(gripsuppression_t40_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}
}


void qt_Proci_palmsuppression_T41_config_Init(void)
{
#if 0
	palmsuppression_t41_config.ctrl         = T41_CTRL;
	palmsuppression_t41_config.reserved_0   = T41_RESERVED_0;
	palmsuppression_t41_config.reserved_1   = T41_RESERVED_1;
	palmsuppression_t41_config.largeobjthr  = T41_LARGEOBJTHR;
	palmsuppression_t41_config.distancethr  = T41_DISTANCETHR;
	palmsuppression_t41_config.supextto     = T41_SUPEXTTO;

	/* Write grip suppression config to chip. */
	if (get_object_address(PROCI_PALMSUPPRESSION_T41, 0) != OBJECT_NOT_FOUND)
	{
		if (write_palmsuppression_T41_config(palmsuppression_t41_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}
#endif
}


void qt_Touch_Suppression_T42_Config_Init(void)
{
#if 0
	touchsuppression_t42_config.ctrl             = T42_CTRL;         
	touchsuppression_t42_config.apprthr          = T42_APPRTHR;      
	touchsuppression_t42_config.maxapprarea      = T42_MAXAPPRAREA;  
	touchsuppression_t42_config.maxtcharea       = T42_MAXTCHAREA;   
	touchsuppression_t42_config.supstrength      = T42_SUPSTRENGTH;  
	touchsuppression_t42_config.supextto         = T42_SUPEXTTO;     
	touchsuppression_t42_config.maxnumtchs       = T42_MAXNUMTCHS;   
	touchsuppression_t42_config.shapestrength    = T42_SHAPESTRENGTH;

	/* Write grip suppression config to chip. */
	if (get_object_address(PROCI_TOUCHSUPPRESSION_T42, 0) != OBJECT_NOT_FOUND)
	{
		if (write_touchsuppression_t42_config(touchsuppression_t42_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}
#endif
}


void qt_DIGITIZER_T43_Config_Init(void)
{
#if 0
	digitizer_t43_config.ctrl               = T42_CTRL;         
	digitizer_t43_config.hididlerate        = T43_HIDIDLERATE;      
	digitizer_t43_config.xlength      	= T43_XLENGTH;  
	digitizer_t43_config.ylength            = T43_YLENGTH;   

	/* Write spt digitizer t43 config to chip. */
	if (get_object_address(SPT_DIGITIZER_T43, 0) != OBJECT_NOT_FOUND)
	{
		if (write_digitizer_t43_config(digitizer_t43_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}
#endif
}


void qt_CTE_T46_Config_Init(void)
{
	/* Set CTE config */
	cte_t46_config.ctrl                 = T46_CTRL;
	cte_t46_config.mode                 = T46_MODE;
	cte_t46_config.idlesyncsperx        = T46_IDLESYNCSPERX;
	cte_t46_config.actvsyncsperx        = T46_ACTVSYNCSPERX;
	cte_t46_config.adcspersync          = T46_ADCSPERSYNC;
	cte_t46_config.pulsesperadc         = T46_PULSESPERADC;
	cte_t46_config.xslew                = T46_XSLEW;
	cte_t46_config.syncdelay            = T46_SYNCDELAY;

	/* Write CTE config to chip. */
	if (get_object_address(SPT_CTECONFIG_T46, 0) != OBJECT_NOT_FOUND)
	{
		if (write_CTE_T46_config(cte_t46_config) != CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}
}


void qt_Stylus_T47_Config_Init(void)
{
	stylus_t47_config.ctrl          = T47_CTRL; 
	stylus_t47_config.contmin       = T47_CONTMIN; 
	stylus_t47_config.contmax       = T47_CONTMAX; 
	stylus_t47_config.stability     = T47_STABILITY; 
	stylus_t47_config.maxtcharea    = T47_MAXTCHAREA;  
	stylus_t47_config.amplthr       = T47_AMPLTHR; 
	stylus_t47_config.styshape      = T47_STYSHAPE; 
	stylus_t47_config.hoversup      = T47_HOVERSUP; 
	stylus_t47_config.confthr       = T47_CONFTHR; 
	stylus_t47_config.syncsperx     = T47_SYNCSPERX;


	/* Write grip suppression config to chip. */
	if (get_object_address(PROCI_STYLUS_T47, 0) != OBJECT_NOT_FOUND)
	{
		if (write_stylus_t47_config(stylus_t47_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}
}


void qt_Noisesuppression_T48_config_Init(void)
{
	noisesuppression_t48_config.ctrl  			= T48_CTRL ;	
	noisesuppression_t48_config.cfg  			= T48_CFG ;
	noisesuppression_t48_config.calcfg  			= T48_CALCFG ;
	noisesuppression_t48_config.basefreq  			= T48_BASEFREQ ;
	noisesuppression_t48_config.freq_0  			= T48_RESERVED0 ;
	noisesuppression_t48_config.freq_1  			= T48_RESERVED1 ;
	noisesuppression_t48_config.freq_2  			= T48_RESERVED2 ;
	noisesuppression_t48_config.freq_3  			= T48_RESERVED3 ;
	noisesuppression_t48_config.mffreq_2  			= T48_MFFREQ_2 ;
	noisesuppression_t48_config.mffreq_3  			= T48_MFFREQ_3 ;
	noisesuppression_t48_config.nlgain  			= T48_RESERVED4 ;
	noisesuppression_t48_config.nlthr  			= T48_RESERVED5 ;
	noisesuppression_t48_config.gclimit  			= T48_RESERVED6 ;
	noisesuppression_t48_config.gcactvinvldadcs  		= T48_GCACTVINVLDADCS ;
	noisesuppression_t48_config.gcidleinvldadcs  		= T48_GCIDLEINVLDADCS ;
	noisesuppression_t48_config.gcinvalidthr  		= T48_RESERVED7 ;
	/* noisesuppression_t48_config.reserved8  		= T48_RESERVED8 ; */
	noisesuppression_t48_config.gcmaxadcsperx  		= T48_GCMAXADCSPERX ;
	noisesuppression_t48_config.gclimitmin  		= T48_GCLIMITMIN ;
	noisesuppression_t48_config.gclimitmax  		= T48_GCLIMITMAX ;
	noisesuppression_t48_config.gccountmintgt  		= T48_GCCOUNTMINTGT ;
	noisesuppression_t48_config.mfinvlddiffthr  		= T48_MFINVLDDIFFTHR ;
	noisesuppression_t48_config.mfincadcspxthr  		= T48_MFINCADCSPXTHR ;
	noisesuppression_t48_config.mferrorthr  		= T48_MFERRORTHR ;
	noisesuppression_t48_config.selfreqmax  		= T48_SELFREQMAX ;
	noisesuppression_t48_config.reserved9  			= T48_RESERVED9 ;
	noisesuppression_t48_config.reserved10  		= T48_RESERVED10 ;
	noisesuppression_t48_config.reserved11  		= T48_RESERVED11 ;
	noisesuppression_t48_config.reserved12  		= T48_RESERVED12 ;
	noisesuppression_t48_config.reserved13  		= T48_RESERVED13 ;
	noisesuppression_t48_config.reserved14 		 	= T48_RESERVED14 ;
	noisesuppression_t48_config.blen  			= T48_BLEN ;
	noisesuppression_t48_config.tchthr  			= T48_TCHTHR ;
	noisesuppression_t48_config.tchdi  			= T48_TCHDI ;
	noisesuppression_t48_config.movhysti  			= T48_MOVHYSTI ;
	noisesuppression_t48_config.movhystn  			= T48_MOVHYSTN ;
	noisesuppression_t48_config.movfilter  			= T48_MOVFILTER ;
	noisesuppression_t48_config.numtouch  			= T48_NUMTOUCH ;
	noisesuppression_t48_config.mrghyst  			= T48_MRGHYST ;
	noisesuppression_t48_config.mrgthr  			= T48_MRGTHR ;
	noisesuppression_t48_config.xloclip  			= T48_XLOCLIP ;
	noisesuppression_t48_config.xhiclip  			= T48_XHICLIP ;
	noisesuppression_t48_config.yloclip  			= T48_YLOCLIP ;
	noisesuppression_t48_config.yhiclip  			= T48_YHICLIP ;
	noisesuppression_t48_config.xedgectrl  			= T48_XEDGECTRL ;
	noisesuppression_t48_config.xedgedist  			= T48_XEDGEDIST ;
	noisesuppression_t48_config.yedgectrl  			= T48_YEDGECTRL ;
	noisesuppression_t48_config.yedgedist  			= T48_YEDGEDIST ;
	noisesuppression_t48_config.jumplimit  			= T48_JUMPLIMIT ;
	noisesuppression_t48_config.tchhyst  			= T48_TCHHYST ;
	noisesuppression_t48_config.nexttchdi  			= T48_NEXTTCHDI ;

	/* Write grip suppression config to chip. */
	if (get_object_address(PROCG_NOISESUPPRESSION_T48, 0) != OBJECT_NOT_FOUND)
	{
		if (write_noisesuppression_t48_config(noisesuppression_t48_config) !=
				CFG_WRITE_OK)
		{
			dbg("[TSP] Configuration Fail!!! , Line %d \n", __LINE__);
		}
	}
}

static void reset_touch_config(void)
{
	qt_Power_Config_Init();
	qt_Acquisition_Config_Init();
	qt_Multitouchscreen_Init();
	qt_KeyArray_Init();
	qt_ComcConfig_Init();
	qt_Gpio_Pwm_Init();
	qt_Proximity_Config_Init();
	qt_One_Touch_Gesture_Config_Init();
	qt_Selftest_Init();
	qt_Two_touch_Gesture_Config_Init();
	qt_Grip_Suppression_T40_Config_Init();
	qt_Proci_palmsuppression_T41_config_Init();
	qt_Touch_Suppression_T42_Config_Init();
	qt_CTE_T46_Config_Init();
	qt_Stylus_T47_Config_Init();
	qt_Noisesuppression_T48_config_Init();
}

unsigned char Comm_Config_Process(unsigned char change_en)
{
	dbg_func_in();

	change_en = 0;

	dbg_func_out();

	return change_en;
}

uint8_t reset_chip(void)
{
	uint8_t data = 1u;
	uint8_t rc;

	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return WRITE_MEM_FAILED;

	rc = write_mem(command_processor_address + RESET_OFFSET, 1, &data);

	dbg_func_out();

	return rc;
}

#ifdef LOCKSCREEN_MODE
void  qt_enable_lockscreen_mode(struct work_struct * p)
{
	int ret;
	
	mutex_lock(&qt602240_data->lock);

	acquisition_config.tchautocal= T8_TCHAUTOCAL_LOCK;
	acquisition_config.atchcalst = T8_ATCHCALST_LOCK;
	acquisition_config.atchcalsthr = T8_ATCHCALSTHR_LOCK;
	acquisition_config.atchfrccalthr = T8_ATCHFRCCALTHR_LOCK;
	acquisition_config.atchfrccalratio = T8_ATCHFRCCALRATIO_LOCK; 
	

	if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
	{
		/* "Acquisition config write failed!\n" */
		dbg("\n[TSP][ERROR] line : %d\n", __LINE__);
		ret = WRITE_MEM_FAILED; /* calling function should retry calibration call */
	}
	
	mutex_unlock(&qt602240_data->lock);
}
#endif


/*****************************************************************************
 *
 *  FUNCTION
 *  PURPOSE
 * \brief Calibrates the chip.
 * 
 * This function will send a calibrate command to touch chip.
 * Whilst calibration has not been confirmed as good, this function will set
 * the ATCHCALST and ATCHCALSTHR to zero to allow a bad cal to always recover
 * 
 * @return WRITE_MEM_OK if writing the command to touch chip was successful.
 * 
 *  INPUT
 *  OUTPUT
 *
 * ***************************************************************************/

uint8_t calibrate_chip(void)
{
	uint8_t data = 1u;
	int ret = WRITE_MEM_OK;
	uint8_t atchcalst, atchcalsthr;

	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return WRITE_MEM_FAILED;

	/* change calibration suspend settings to zero until calibration confirmed good */
	/* store normal settings */
	atchcalst = acquisition_config.atchcalst;
	atchcalsthr = acquisition_config.atchcalsthr;

	/* resume calibration must be performed with zero settings */
	acquisition_config.atchcalst = 0;
	acquisition_config.atchcalsthr = 0; 

	dbg("[TSP] reset acq atchcalst=%d, atchcalsthr=%d\n", acquisition_config.atchcalst, acquisition_config.atchcalsthr );

	/* Write temporary acquisition config to chip. */
	if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
	{
		/* "Acquisition config write failed!\n" */
		dbg("\n[TSP][ERROR] line : %d\n", __LINE__);
		ret = WRITE_MEM_FAILED; /* calling function should retry calibration call */
	}

	/* send calibration command to the chip */
	if(ret == WRITE_MEM_OK)
	{
		/* change calibration suspend settings to zero until calibration confirmed good */
		ret = write_mem(command_processor_address + CALIBRATE_OFFSET, 1, &data);

		/* set flag for calibration lockup recovery if cal command was successful */
		if(ret == WRITE_MEM_OK)
		{ 
			/* set flag to show we must still confirm if calibration was good or bad */
			cal_check_flag = 1u;
		}
	}

	msleep(120);
	
	
	/* restore settings to the local structure so that when we confirm the 
	 * cal is good we can correct them in the chip */
	/* this must be done before returning */
	
	acquisition_config.atchcalst = atchcalst;
	acquisition_config.atchcalsthr = atchcalsthr;

	dbg("[TSP] write acq atchcalst=%d, atchcalsthr=%d\n", acquisition_config.atchcalst, acquisition_config.atchcalsthr );
	/* Write temporary acquisition config to chip. */
	if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
	{
		/* "Acquisition config write failed!\n" */
		dbg("\n[TSP][ERROR] line : %d\n", __LINE__);
		ret = WRITE_MEM_FAILED; /* calling function should retry calibration call */
	}

	dbg_func_out();
	return ret;
}

uint8_t diagnostic_chip(uint8_t mode)
{
	uint8_t status;
	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return WRITE_MEM_FAILED;

	status = write_mem(command_processor_address + DIAGNOSTIC_OFFSET, 1, &mode);

	dbg_func_out();
	return(status);
}

uint8_t backup_config(void)
{
	/* Write 0x55 to BACKUPNV register to initiate the backup. */
	uint8_t data = 0x55u;
	uint8_t rc;

	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return WRITE_MEM_FAILED;

	rc = write_mem(command_processor_address + BACKUP_OFFSET, 1, &data);

	dbg_func_out();

	return rc;
}

uint8_t write_power_config(gen_powerconfig_t7_config_t cfg)
{
	uint8_t rc;

	dbg_func_in();

	rc = write_simple_config(GEN_POWERCONFIG_T7, 0, (void *) &cfg);

	dbg_func_out();
	return rc;
}

uint8_t write_acquisition_config(gen_acquisitionconfig_t8_config_t cfg)
{
	uint8_t rc;

	dbg_func_in();

	rc = write_simple_config(GEN_ACQUISITIONCONFIG_T8, 0, (void *) &cfg);

	dbg_func_out();
	return rc;
}

uint8_t write_multitouchscreen_config(uint8_t instance, touch_multitouchscreen_t9_config_t cfg)
{
	uint16_t object_address;
	uint8_t *tmp;
	uint8_t status;
	uint8_t object_size;

	dbg_func_in();

	object_size = get_object_size(TOUCH_MULTITOUCHSCREEN_T9);
	if (object_size == 0)
	{
		return(CFG_WRITE_FAILED);
	}
	tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);
	if (tmp == NULL)
	{
		return(CFG_WRITE_FAILED);
	}

	memset(tmp,0,object_size);


	/* 18 elements at beginning are 1 byte. */
	memcpy(tmp, &cfg, 18);

	/* Next two are 2 bytes. */

	*(tmp + 18) = (uint8_t) (cfg.xrange &  0xFF);
	*(tmp + 19) = (uint8_t) (cfg.xrange >> 8);

	*(tmp + 20) = (uint8_t) (cfg.yrange &  0xFF);
	*(tmp + 21) = (uint8_t) (cfg.yrange >> 8);

	/* And the last 4(8) 1 bytes each again. */

	*(tmp + 22) = cfg.xloclip;
	*(tmp + 23) = cfg.xhiclip;
	*(tmp + 24) = cfg.yloclip;
	*(tmp + 25) = cfg.yhiclip;

	*(tmp + 26) = cfg.xedgectrl;
	*(tmp + 27) = cfg.xedgedist;
	*(tmp + 28) = cfg.yedgectrl;
	*(tmp + 29) = cfg.yedgedist;

	//if(info_block->info_id.version >= 0x16)
	//{
		*(tmp + 30) = cfg.jumplimit;
	//}

	*(tmp + 31) = cfg.tchhyst;
	*(tmp + 32) = cfg.xpitch;
	*(tmp + 33) = cfg.ypitch;
	*(tmp + 34) = cfg.nexttchdi;

	object_address = get_object_address(TOUCH_MULTITOUCHSCREEN_T9,
			instance);

	if (object_address == 0)
	{
		return(CFG_WRITE_FAILED);
	}

	status = write_mem(object_address, object_size, tmp);

	kfree(tmp);

	dbg_func_out();
	return(status);

}

uint8_t write_keyarray_config(uint8_t instance, touch_keyarray_t15_config_t cfg)
{
	uint8_t rc;
	dbg_func_in();

	rc = write_simple_config(TOUCH_KEYARRAY_T15, instance, (void *) &cfg);

	dbg_func_out();
	return rc;
}

uint8_t write_comc_config(uint8_t instance, spt_comcconfig_t18_config_t cfg)
{
	uint8_t rc;
	dbg_func_in();

	rc = write_simple_config(SPT_COMCONFIG_T18, instance, (void *) &cfg);

	dbg_func_out();
	return rc;
}

uint8_t write_gpio_config(uint8_t instance, spt_gpiopwm_t19_config_t cfg)
{
	uint8_t rc;
	dbg_func_in();
	rc = write_simple_config(SPT_GPIOPWM_T19, instance, (void *) &cfg);
	dbg_func_out();
	return rc;
}

uint8_t write_proximity_config(uint8_t instance, touch_proximity_t23_config_t cfg)
{
	uint16_t object_address;
	uint8_t *tmp;
	uint8_t status;
	uint8_t object_size;

	dbg_func_in();

	object_size = get_object_size(TOUCH_PROXIMITY_T23);
	if (object_size == 0)
	{
		return(CFG_WRITE_FAILED);
	}
	tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);
	if (tmp == NULL)
	{
		return(CFG_WRITE_FAILED);
	}

	memset(tmp,0,object_size);

	*(tmp + 0) = cfg.ctrl;
	*(tmp + 1) = cfg.xorigin;
	*(tmp + 2) = cfg.yorigin;
	*(tmp + 3) = cfg.xsize;
	*(tmp + 4) = cfg.ysize;
	*(tmp + 5) = cfg.reserved_for_future_aks_usage;
	*(tmp + 6) = cfg.blen;

	*(tmp + 7) = (uint8_t) (cfg.fxddthr & 0x00FF);
	*(tmp + 8) = (uint8_t) (cfg.fxddthr >> 8);

	*(tmp + 9) = cfg.fxddi;
	*(tmp + 10) = cfg.average;

	*(tmp + 11) = (uint8_t) (cfg.mvnullrate & 0x00FF);
	*(tmp + 12) = (uint8_t) (cfg.mvnullrate >> 8);

	*(tmp + 13) = (uint8_t) (cfg.mvdthr & 0x00FF);
	*(tmp + 14) = (uint8_t) (cfg.mvdthr >> 8);
#if 0
	*(tmp + 7) = (uint8_t) (cfg.tchthr & 0x00FF);
	*(tmp + 8) = (uint8_t) (cfg.tchthr >> 8);

	*(tmp + 9) = cfg.tchdi;
	*(tmp + 10) = cfg.average;

	*(tmp + 11) = (uint8_t) (cfg.rate & 0x00FF);
	*(tmp + 12) = (uint8_t) (cfg.rate >> 8);
#endif

	object_address = get_object_address(TOUCH_PROXIMITY_T23,
			instance);

	if (object_address == 0)
	{
		return(CFG_WRITE_FAILED);
	}

	status = write_mem(object_address, object_size, tmp);

	kfree(tmp);
	dbg_func_out();
	return(status);
}

uint8_t write_onetouchgesture_config(uint8_t instance, proci_onetouchgestureprocessor_t24_config_t cfg)
{
	uint16_t object_address;
	uint8_t *tmp;
	uint8_t status;
	uint8_t object_size;

	dbg_func_in();

	object_size = get_object_size(PROCI_ONETOUCHGESTUREPROCESSOR_T24);
	if (object_size == 0)
	{
		return(CFG_WRITE_FAILED);
	}
	tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);
	if (tmp == NULL)
	{
		return(CFG_WRITE_FAILED);
	}

	memset(tmp,0,object_size);

	*(tmp + 0) = cfg.ctrl;
	*(tmp + 1) = cfg.numgest;

	*(tmp + 2) = (uint8_t) (cfg.gesten & 0x00FF);
	*(tmp + 3) = (uint8_t) (cfg.gesten >> 8);

	memcpy((tmp+4), &cfg.pressproc, 7);

	*(tmp + 11) = (uint8_t) (cfg.flickthr & 0x00FF);
	*(tmp + 12) = (uint8_t) (cfg.flickthr >> 8);

	*(tmp + 13) = (uint8_t) (cfg.dragthr & 0x00FF);
	*(tmp + 14) = (uint8_t) (cfg.dragthr >> 8);

	*(tmp + 15) = (uint8_t) (cfg.tapthr & 0x00FF);
	*(tmp + 16) = (uint8_t) (cfg.tapthr >> 8);

	*(tmp + 17) = (uint8_t) (cfg.throwthr & 0x00FF);
	*(tmp + 18) = (uint8_t) (cfg.throwthr >> 8);

	object_address = get_object_address(PROCI_ONETOUCHGESTUREPROCESSOR_T24,
			instance);

	if (object_address == 0)
	{
		return(CFG_WRITE_FAILED);
	}

	status = write_mem(object_address, object_size, tmp);
	kfree(tmp);
	dbg_func_out();
	return(status);
}

uint8_t write_selftest_config(uint8_t instance, spt_selftest_t25_config_t cfg)
{

	uint16_t object_address;
	uint8_t *tmp;
	uint8_t status;
	uint8_t object_size;

	dbg_func_in();

	object_size = get_object_size(SPT_SELFTEST_T25);
	if (object_size == 0)
	{
		return(CFG_WRITE_FAILED);
	}
	tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);


	if (tmp == NULL)
	{
		return(CFG_WRITE_FAILED);
	}

	memset(tmp,0,object_size);

	*(tmp + 0) = cfg.ctrl;
	*(tmp + 1) = cfg.cmd;
	object_address = get_object_address(SPT_SELFTEST_T25,
			instance);

	if (object_address == 0)
	{
		return(CFG_WRITE_FAILED);
	}

	status = write_mem(object_address, object_size, tmp);

	kfree(tmp);
	dbg_func_out();
	return(status);
}

uint8_t write_twotouchgesture_config(uint8_t instance, proci_twotouchgestureprocessor_t27_config_t cfg)
{

	uint16_t object_address;
	uint8_t *tmp;
	uint8_t status;
	uint8_t object_size;

	dbg_func_in();

	object_size = get_object_size(PROCI_TWOTOUCHGESTUREPROCESSOR_T27);
	if (object_size == 0)
	{
		return(CFG_WRITE_FAILED);
	}
	tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);

	if (tmp == NULL)
	{
		return(CFG_WRITE_FAILED);
	}

	memset(tmp,0,object_size);

	*(tmp + 0) = cfg.ctrl;
	*(tmp + 1) = cfg.numgest;
	*(tmp + 2) = 0;
	*(tmp + 3) = cfg.gesten;
	*(tmp + 4) = cfg.rotatethr;
	*(tmp + 5) = (uint8_t) (cfg.zoomthr & 0x00FF);
	*(tmp + 6) = (uint8_t) (cfg.zoomthr >> 8);

	object_address = get_object_address(PROCI_TWOTOUCHGESTUREPROCESSOR_T27,
			instance);

	if (object_address == 0)
	{
		return(CFG_WRITE_FAILED);
	}

	status = write_mem(object_address, object_size, tmp);
	kfree(tmp);
	dbg_func_out();
	return(status);
}

uint8_t write_gripsuppression_T40_config(proci_gripsuppression_t40_config_t cfg)
{
	return(write_simple_config(PROCI_GRIPSUPPRESSION_T40, 0, (void *) &cfg));
}

uint8_t write_palmsuppression_T41_config(proci_palmsuppression_t41_config_t cfg)
{
	return(write_simple_config(PROCI_PALMSUPPRESSION_T41, 0, (void *) &cfg));
}

uint8_t write_touchsuppression_t42_config(proci_touchsuppression_t42_config_t cfg)
{
	return(write_simple_config(PROCI_TOUCHSUPPRESSION_T42, 0, (void *) &cfg));
}

uint8_t write_digitizer_t43_config(spt_digitizer_t43_config_t cfg)
{
	return(write_simple_config(SPT_DIGITIZER_T43, 0, (void *) &cfg));
}

uint8_t write_CTE_T46_config(spt_cteconfig_t46_config_t cfg)
{

	return(write_simple_config(SPT_CTECONFIG_T46, 0, (void *) &cfg));
}

uint8_t  write_stylus_t47_config(proci_stylus_t47_config_t cfg)
{
	return(write_simple_config(PROCI_STYLUS_T47, 0, (void *) &cfg));
}

uint8_t  write_noisesuppression_t48_config(procg_noisesuppression_t48_config_t cfg)
{
	uint16_t object_address;
	uint8_t *tmp;
	uint8_t status;
	uint8_t object_size;

	object_size = get_object_size(PROCG_NOISESUPPRESSION_T48);
	if (object_size == 0)
	{
		return(CFG_WRITE_FAILED);
	}
	tmp = (uint8_t *) kmalloc(object_size, GFP_KERNEL | GFP_ATOMIC);
	if (tmp == NULL)
	{
		return(CFG_WRITE_FAILED);
	}

	memset(tmp,0,object_size);


	/* 18 elements at beginning are 1 byte. */
	memcpy(tmp, &cfg, 15);

	/* Next two are 2 bytes. */

	*(tmp + 15) = (uint8_t) (cfg.gcinvalidthr &  0xFF);
	*(tmp + 16) = (uint8_t) (cfg.gcinvalidthr >> 8);
	*(tmp + 17) = cfg.gcmaxadcsperx;
	*(tmp + 18) = cfg.gclimitmin;
	*(tmp + 19) = cfg.gclimitmax;
	*(tmp + 20) = (uint8_t) (cfg.gccountmintgt &  0xFF);
	*(tmp + 21) = (uint8_t) (cfg.gccountmintgt >> 8);
	*(tmp + 22) = (uint8_t) cfg.mfinvlddiffthr;
	*(tmp + 23) = (uint8_t) (cfg.mfincadcspxthr &  0xFF);
	*(tmp + 24) = (uint8_t) (cfg.mfincadcspxthr >> 8);
	*(tmp + 25) = (uint8_t) (cfg.mferrorthr &  0xFF);
	*(tmp + 26) = (uint8_t) (cfg.mferrorthr >> 8);
	*(tmp + 27) = cfg.selfreqmax;
	*(tmp + 28) = cfg.reserved9;
	*(tmp + 29) = cfg.reserved10;
	*(tmp + 30) = cfg.reserved11;
	*(tmp + 31) = cfg.reserved12;
	*(tmp + 32) = cfg.reserved13;
	*(tmp + 33) = cfg.reserved14;
	*(tmp + 34) = cfg.blen;
	*(tmp + 35) = cfg.tchthr; 
	*(tmp + 36) = cfg.tchdi;
	*(tmp + 37) = cfg.movhysti;
	*(tmp + 38) = cfg.movhystn;
	*(tmp + 39) = cfg.movfilter;
	*(tmp + 40) = cfg.numtouch;
	*(tmp + 41) = cfg.mrghyst;
	*(tmp + 42) = cfg.mrgthr;
	*(tmp + 43) = cfg.xloclip;
	*(tmp + 44) = cfg.xhiclip;
	*(tmp + 45) = cfg.yloclip;
	*(tmp + 46) = cfg.yhiclip;
	*(tmp + 47) = cfg.xedgectrl;
	*(tmp + 48) = cfg.xedgedist;
	*(tmp + 49) = cfg.yedgectrl;
	*(tmp + 50) = cfg.yedgedist;
	*(tmp + 51) = cfg.jumplimit;
	*(tmp + 52) = cfg.tchhyst; 
	*(tmp + 53) = cfg.nexttchdi;

	object_address = get_object_address(PROCG_NOISESUPPRESSION_T48, 0);

	if (object_address == 0)
	{
		return(CFG_WRITE_FAILED);
	}

	status = write_mem(object_address, object_size, tmp);
	kfree(tmp);
	return(status);
}

uint8_t write_simple_config(uint8_t object_type, uint8_t instance, void *cfg)
{
	uint16_t object_address;
	uint8_t object_size;
	uint8_t rc;

	dbg_func_in();

	object_address = get_object_address(object_type, instance);
	object_size = get_object_size(object_type);

	if ((object_size == 0) || (object_address == 0))
	{
		rc = CFG_WRITE_FAILED;
	}
	else
	{
		rc = write_mem(object_address, object_size, cfg);
	}

	dbg_func_out();
	return rc; 
}

uint8_t get_object_size(uint8_t object_type)
{
	uint8_t object_table_index = 0;
	uint8_t object_found = 0;
	uint16_t size = OBJECT_NOT_FOUND;
	object_t *object_table;
	object_t obj;

	//dbg_func_in();

	if(info_block == NULL)		
		return 0;

	object_table = info_block->objects;
	while ((object_table_index < info_block->info_id.num_declared_objects) &&
			!object_found)
	{
		obj = object_table[object_table_index];
		/* Does object type match? */
		if (obj.object_type == object_type)
		{
			object_found = 1;
			size = obj.size + 1;
		}
		object_table_index++;
	}

	//dbg_func_out();
	return(size);
}

uint8_t type_to_report_id(uint8_t object_type, uint8_t instance)
{

	uint8_t report_id = 1;
	int8_t report_id_found = 0;
	uint8_t rc;

	dbg_func_in();

	while((report_id <= max_report_id) && (report_id_found == 0))
	{
		if((report_id_map[report_id].object_type == object_type) &&
				(report_id_map[report_id].instance == instance))
		{
			report_id_found = 1;
		}
		else
		{
			report_id++;
		}
	}
	if (report_id_found)
	{
		rc = report_id;
	}
	else
	{
		rc = ID_MAPPING_FAILED;
	}

	dbg_func_out();

	return rc;
}

uint8_t report_id_to_type(uint8_t report_id, uint8_t *instance)
{
	uint8_t rc;

	//dbg_func_in();

	if (report_id <= max_report_id)
	{
		*instance = report_id_map[report_id].instance;
		rc = report_id_map[report_id].object_type;
	}
	else
	{
		rc = ID_MAPPING_FAILED;
	}

	//dbg_func_out();

	return rc;
}

uint8_t read_id_block(info_id_t *id)
{
	uint8_t status;

	dbg_func_in();

	status = read_mem(0, 1, (void *) &id->family_id);
	if (status != READ_MEM_OK)
	{
#if 1 //#if EF30S_BDVER_GE(EF30S_PT10) 
#else
		return(status);
#endif
	}
	dbg("family_id = 	0x%x\n",id->family_id);

	status = read_mem(1, 1, (void *) &id->variant_id);
	if (status != READ_MEM_OK) goto read_id_block_exit;
	dbg("variant_id = 	0x%x\n",id->variant_id);

	status = read_mem(2, 1, (void *) &id->version);
	if (status != READ_MEM_OK) goto read_id_block_exit;
	dbg("version = 		0x%x\n",id->version);

	status = read_mem(3, 1, (void *) &id->build);
	if (status != READ_MEM_OK) goto read_id_block_exit;
	dbg("familybuild_id = 	0x%x\n",id->build);

	status = read_mem(4, 1, (void *) &id->matrix_x_size);
	if (status != READ_MEM_OK) goto read_id_block_exit;
	dbg("matrix_x_size = 	%d\n",id->matrix_x_size);

	status = read_mem(5, 1, (void *) &id->matrix_y_size);
	if (status != READ_MEM_OK) goto read_id_block_exit;
	dbg("matrix_y_size = 	%d\n",id->matrix_y_size);

	status = read_mem(6, 1, (void *) &id->num_declared_objects);

	dbg_func_out();

	return status;

read_id_block_exit:

	dbg("error : read_id_block_exit\n");
	dbg_func_out();
	return status;
}

uint16_t get_object_address(uint8_t object_type, uint8_t instance)
{
	uint8_t object_table_index = 0;
	uint8_t address_found = 0;
	uint16_t address = OBJECT_NOT_FOUND;
	object_t *object_table;
	object_t obj;

	//dbg_func_in();

	if(info_block == NULL)		
		return 0;

	object_table = info_block->objects;
	while ((object_table_index < info_block->info_id.num_declared_objects) &&
			!address_found)
	{
		obj = object_table[object_table_index];
		/* Does object type match? */
		if (obj.object_type == object_type)
		{

			address_found = 1;

			/* Are there enough instances defined in the FW? */
			if (obj.instances >= instance)
			{
				address = obj.i2c_address + (obj.size + 1) * instance;
			}
		}
		object_table_index++;
	}

	//dbg_func_out();
	return(address);
}

uint32_t get_stored_infoblock_crc()
{
	uint32_t crc;

	dbg_func_in();

	crc = (uint32_t) (((uint32_t) info_block->CRC_hi) << 16);
	crc = crc | info_block->CRC;

	dbg_func_out();
	return(crc);
}

uint8_t calculate_infoblock_crc(uint32_t *crc_pointer)
{

	uint32_t crc = 0;
	uint16_t crc_area_size;
	uint8_t *mem;
	uint8_t i;
	uint8_t rc;

	uint8_t status;

	dbg_func_in();

	rc = CRC_CALCULATION_OK;

	/* 7 bytes of version data, 6 * NUM_OF_OBJECTS bytes of object table. */
	crc_area_size = 7 + info_block->info_id.num_declared_objects * 6;

	mem = (uint8_t *) kmalloc(crc_area_size, GFP_KERNEL | GFP_ATOMIC);
	if (mem == NULL)
	{
		rc = CRC_CALCULATION_FAILED;
		goto calculate_infoblock_crc_exit;
	}

	status = read_mem(0, crc_area_size, mem);
	if (status != READ_MEM_OK)
	{
		rc = CRC_CALCULATION_FAILED;
		goto calculate_infoblock_crc_exit;
	}

	i = 0;
	while (i < (crc_area_size - 1))
	{
		crc = CRC_24(crc, *(mem + i), *(mem + i + 1));
		i += 2;
	}

	crc = CRC_24(crc, *(mem + i), 0);

	kfree(mem);

	/* Return only 24 bit CRC. */
	*crc_pointer = (crc & 0x00FFFFFF);

	dbg_func_out();
	return rc;

calculate_infoblock_crc_exit:
	dbg("error : calculate_infoblock_crc\n");
	dbg_func_out();
	return rc;
}

uint32_t CRC_24(uint32_t crc, uint8_t byte1, uint8_t byte2)
{
	static const uint32_t crcpoly = 0x80001B;
	uint32_t result;
	uint16_t data_word;

	//dbg_func_in();

	data_word = (uint16_t) ((uint16_t) (byte2 << 8u) | byte1);
	result = ((crc << 1u) ^ (uint32_t) data_word);

	if (result & 0x1000000)
	{
		result ^= crcpoly;
	}

	//dbg_func_out();

	return(result);
}

void touch_data_init(void)
{
	int i = 0;
	dbg_func_in();

	for (i = 0; i<MAX_NUM_FINGER; i++ )
	{
		fingerInfo[i].mode = TSC_EVENT_NONE;
		fingerInfo[i].status = -1;
		fingerInfo[i].area = 0;
		keyInfo[i].update = false;
	}
	active_event = true;

	calibration_message = 0; 
	timer_tick = 0;
	timer_enabled = false;
	is_cal_success = false;

	dbg_func_out();
}

/*------------------------------ main block -----------------------------------*/
void quantum_touch_probe(void)
{
	U8 report_id;
	U8 object_type, instance;
	uint32_t crc, stored_crc;
#ifdef CHIP_NOINIT
	uint16_t object_address;
	uint16_t xres, yres;
	U8 xysize[3],  status;
#endif
	dbg_func_in();

	if (init_touch_driver() == DRIVER_SETUP_OK)
	{
		dbg("\n[TSP] Touch device found\n");
	}
	else
	{
		dbg("\n[TSP][ERROR] Touch device NOT found\n");
		return ;
	}

	/* Get and show the version information. */

	if(calculate_infoblock_crc(&crc) != CRC_CALCULATION_OK)
	{
		dbg("Calculating CRC failed, skipping check!\n\r");
	}
	else
	{
		dbg("Calculated CRC:\t\n");
		write_message_to_usart((uint8_t *) &crc, 4);
		dbg("\n\r");
	}

	stored_crc = get_stored_infoblock_crc();
	dbg("Stored CRC:\t");
	write_message_to_usart((uint8_t *) &stored_crc, 4);
	dbg("\n\r");

	if (stored_crc != crc)
	{
		dbg("Warning: info block CRC value doesn't match the calculated!\n\r");
	}
	else
	{
		dbg("Info block CRC value OK.\n\r");
	}

	/* Test the report id to object type / instance mapping: get the maximum
	 * report id and print the report id map. */

	dbg("Report ID to Object type / instance mapping:\n\r");

	for (report_id = 0; report_id <= max_report_id; report_id++)
	{
		object_type = report_id_to_type(report_id, &instance);
		dbg("Report ID : %d, Object Type : T%d, Instance : %d\n",report_id, object_type, instance);
	}

#ifdef CHIP_NOINIT
	object_address = get_object_address(TOUCH_MULTITOUCHSCREEN_T9,
			0);

	status = read_U16(object_address+18, &xres);
	status = read_U16(object_address+20, &yres);
	status = read_mem(object_address+3, 2, (U8 *) xysize);
	dbg("[TSP] Read Chip xyInfo : xres(%d), yres(%d), xsize(%d), ysize(%d)\n", xres, yres, xysize[0], xysize[1]);

	if(Chip_NoInit == false){
		if(!((yres == (SCREEN_RESOLUTION_X-1) || yres == SCREEN_RESOLUTION_X) && xysize[0] == T9_XSIZE && xysize[1] == T9_YSIZE))
			Chip_NoInit = true;
	}    
#endif

	reset_touch_config();

	/* Backup settings to NVM. */
	if (backup_config() != WRITE_MEM_OK)
	{
		dbg("Failed to backup, exiting...\n");
		return;
	}
	else
	{
		dbg("Backed up the config to non-volatile memory!\n");
	}

	/* Calibrate the touch IC. */
	if (calibrate_chip() != WRITE_MEM_OK)
	{
		dbg("Failed to calibrate, exiting...\n");
		return;
	}
	else
	{
		dbg("Chip calibrated!\n");
	}

	touch_data_init();

#ifdef CHARGER_MODE
	charger_mode = -1;
	previous_charger_mode = -1;
#endif  

	dbg_func_out();
	dbg("Waiting for touch chip messages...\n");
}

/*------------------------------ Sub functions -----------------------------------*/
/*!
  \brief Initializes touch driver.

  This function initializes the touch driver: tries to connect to given 
  address, sets the message handler pointer, reads the info block and object
  table, sets the message processor address etc.

  @param I2C_address is the address where to connect.
  @param (*handler) is a pointer to message handler function.
  @return DRIVER_SETUP_OK if init successful, DRIVER_SETUP_FAILED 
  otherwise.
  */
uint8_t init_touch_driver(void)
{
	int i;
	int current_report_id = 0;
	uint8_t tmp;
	uint16_t current_address;
	uint16_t crc_address;
	object_t *object_table;
	info_id_t *id;
	uint32_t *CRC;
	uint8_t status;

	dbg_func_in();

	/* Read the info block data. */
	id = (info_id_t *) kmalloc(sizeof(info_id_t), GFP_KERNEL | GFP_ATOMIC);
	if (id == NULL)
	{
		return(DRIVER_SETUP_INCOMPLETE);
	}

	if (read_id_block(id) != 1)
	{
		dbg("[TSP][ERROR] can't read info block data.\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}  

	/* Read object table. */
	object_table = (object_t *) kmalloc(id->num_declared_objects * sizeof(object_t), GFP_KERNEL | GFP_ATOMIC);
	if (object_table == NULL)
	{
		dbg("[TSP][ERROR] 3\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}

	/* Reading the whole object table block to memory directly doesn't work cause sizeof object_t isn't necessarily the same on every compiler/platform due to alignment issues. Endianness can also cause trouble. */
	current_address = OBJECT_TABLE_START_ADDRESS;

	for (i = 0; i < id->num_declared_objects; i++)
	{
		status = read_mem(current_address, 1, &(object_table[i]).object_type);
		if (status != READ_MEM_OK)
		{
			dbg("[TSP][ERROR] 4\n");
			return(DRIVER_SETUP_INCOMPLETE);
		}
		current_address++;

		status = read_U16(current_address, &object_table[i].i2c_address);
		if (status != READ_MEM_OK)
		{
			dbg("[TSP][ERROR] 5\n");
			return(DRIVER_SETUP_INCOMPLETE);
		}
		current_address += 2;

		status = read_mem(current_address, 1, (U8*)&object_table[i].size);
		if (status != READ_MEM_OK)
		{
			dbg("[TSP][ERROR] 6\n");
			return(DRIVER_SETUP_INCOMPLETE);
		}
		current_address++;

		status = read_mem(current_address, 1, &object_table[i].instances);
		if (status != READ_MEM_OK)
		{
			dbg("[TSP][ERROR] 7\n");
			return(DRIVER_SETUP_INCOMPLETE);
		}
		current_address++;

		status = read_mem(current_address, 1, &object_table[i].num_report_ids);
		if (status != READ_MEM_OK)
		{
			dbg("[TSP][ERROR] 8\n");
			return(DRIVER_SETUP_INCOMPLETE);
		}
		current_address++;

		max_report_id += object_table[i].num_report_ids;

		/* Find out the maximum message length. */
		if (object_table[i].object_type == GEN_MESSAGEPROCESSOR_T5)
		{
			max_message_length = object_table[i].size + 1;
		}
	}

	/* Check that message processor was found. */
	if (max_message_length == 0)
	{
		dbg("[TSP][ERROR] 9\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}

	/* Read CRC. */
	CRC = (uint32_t *) kmalloc(sizeof(info_id_t), GFP_KERNEL | GFP_ATOMIC);
	if (CRC == NULL)
	{
		dbg("[TSP][ERROR] 10\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}

	info_block = kmalloc(sizeof(info_block_t), GFP_KERNEL | GFP_ATOMIC);
	if (info_block == NULL)
	{
		dbg("err\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}

	info_block->info_id = *id;
	info_block->objects = object_table;
	crc_address = OBJECT_TABLE_START_ADDRESS + id->num_declared_objects * OBJECT_TABLE_ELEMENT_SIZE;

	status = read_mem(crc_address, 1u, &tmp);
	if (status != READ_MEM_OK)
	{
		dbg("[TSP][ERROR] 11\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}
	info_block->CRC = tmp;

	status = read_mem(crc_address + 1u, 1u, &tmp);
	if (status != READ_MEM_OK)
	{
		dbg("[TSP][ERROR] 12\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}
	info_block->CRC |= (tmp << 8u);

	status = read_mem(crc_address + 2, 1, &info_block->CRC_hi);
	if (status != READ_MEM_OK)
	{
		dbg("[TSP][ERROR] 13\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}

	/* Store message processor address, it is needed often on message reads. */
	message_processor_address = get_object_address(GEN_MESSAGEPROCESSOR_T5, 0);
	if (message_processor_address == 0)
	{
		dbg("[TSP][ERROR] 14 !!\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}

	/* Store command processor address. */
	command_processor_address = get_object_address(GEN_COMMANDPROCESSOR_T6, 0);
	if (command_processor_address == 0)
	{
		dbg("[TSP][ERROR] 15\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}

	quantum_msg = kmalloc(max_message_length, GFP_KERNEL | GFP_ATOMIC);
	if (quantum_msg == NULL)
	{
		dbg("[TSP][ERROR] 16\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}

	/* Allocate memory for report id map now that the number of report id's 
	 * is known. */

	report_id_map = kmalloc(sizeof(report_id_map_t) * max_report_id, GFP_KERNEL | GFP_ATOMIC);

	if (report_id_map == NULL)
	{
		dbg("[TSP][ERROR] 17\n");
		return(DRIVER_SETUP_INCOMPLETE);
	}

	/* Report ID 0 is reserved, so start from 1. */

	report_id_map[0].instance = 0;
	report_id_map[0].object_type = 0;
	current_report_id = 1;

	for (i = 0; i < id->num_declared_objects; i++)
	{
		if (object_table[i].num_report_ids != 0)
		{
			int instance;
			for (instance = 0; instance <= object_table[i].instances; instance++)
			{
				int start_report_id = current_report_id;
				for (; current_report_id < (start_report_id + object_table[i].num_report_ids); current_report_id++)
				{
					report_id_map[current_report_id].instance = instance;
					report_id_map[current_report_id].object_type = object_table[i].object_type;
				}
			}
		}
	}
	driver_setup = DRIVER_SETUP_OK;

	/* Initialize the pin connected to touch ic pin CHANGELINE to catch the
	 * falling edge of signal on that line. */

	dbg_func_out();
	return(DRIVER_SETUP_OK);
}

void  clear_event(uint8_t clear)
{
	uint8_t valid_area;
	int i;   

	dbg_func_in();

	valid_area = 0;
	for ( i= 0; i<MAX_NUM_FINGER; i++ )
	{
		if(fingerInfo[i].mode == TSC_EVENT_WINDOW)
		{
			dbg("[TSP] Finger[%d] Up(TSC_EVENT_WINDOW) XY(%d, %d) - forced event\n", i, fingerInfo[i].realx, fingerInfo[i].realy);                     
			if(i == 0 && clear != TSC_CLEAR_ALL)                
			{
				dbg("TSP_Fin=%d,Down_WIN_XY(%d, %d)-forced\n", i, fingerInfo[i].realx, fingerInfo[i].realy);
				input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_X, fingerInfo[i].realx);
				input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_Y, fingerInfo[i].realy);
				input_report_abs(qt602240_data->input_dev, ABS_MT_TOUCH_MAJOR, 1);//pressure[i]); // 0???? Release, ?????? Press ????(Down or Move)
				input_report_abs(qt602240_data->input_dev, ABS_MT_WIDTH_MAJOR, fingerInfo[i].id);            // Size ???? ID ????
				input_mt_sync(qt602240_data->input_dev);
			}
			else
			{
				dbg("TSP_Fin=%d,Up_WIN_XY(%d, %d)-forced\n", i, fingerInfo[i].realx, fingerInfo[i].realy);
				input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_X, fingerInfo[i].realx);
				input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_Y, fingerInfo[i].realy);
				input_report_abs(qt602240_data->input_dev, ABS_MT_TOUCH_MAJOR, 0);//pressure[i]); // 0???? Release, ?????? Press ????(Down or Move)
				input_report_abs(qt602240_data->input_dev, ABS_MT_WIDTH_MAJOR, fingerInfo[i].id);            // Size ???? ID ????
				input_mt_sync(qt602240_data->input_dev);
			}   
			valid_area |= 1;
		}
	}
	if(valid_area != 0)
		input_sync(qt602240_data->input_dev);

	for ( i= 0; i<MAX_NUM_FINGER; i++ )
	{
		if((fingerInfo[i].mode == TSC_EVENT_MENU) || (fingerInfo[i].mode == TSC_EVENT_HOME) || (fingerInfo[i].mode == TSC_EVENT_BACK))
		{
			switch (fingerInfo[i].mode) 
			{
				case TSC_EVENT_MENU:
					dbg("[TSP] Finger[%d] (TSC_EVENT_MENU = 0) - forced event\n", i);
					input_report_key(qt602240_data->input_dev, KEY_MENU, 0);
					break;
				case TSC_EVENT_HOME:		
					dbg("[TSP] Finger[%d] (TSC_EVENT_HOME = 0) - forced event\n", i);
					input_report_key(qt602240_data->input_dev, KEY_HOME, 0);
					break;		
				case TSC_EVENT_BACK:
					dbg("[TSP] Finger[%d] (TSC_EVENT_BACK = 0) - forced event\n", i);
					input_report_key(qt602240_data->input_dev, KEY_BACK, 0);
					break;	
				default:
					break;
			}
		}
	}

	if(clear == TSC_CLEAR_ALL)
	{
		for ( i= 0; i<MAX_NUM_FINGER; i++ )
		{
			fingerInfo[i].mode = TSC_EVENT_NONE;
			fingerInfo[i].status = -1;
			fingerInfo[i].area = 0;
			keyInfo[i].update = false; 
		}     
	}

	dbg_func_out();
}

void cal_maybe_good(void)
{    
	dbg_func_in();

	/* Check if the timer is enabled */
	if(timer_enabled)
	{
		if(timer_tick > 0) /* Check if the timer timedout of 0.5seconds */
		{
			/* cal was good - don't need to check any more */
			cal_check_flag = 0;
			/* Disable the timer */
			del_timer(&timer);
			timer_enabled = false;      
			timer_tick = 0; 

			is_cal_success = true;

			/* Write back the normal acquisition config to chip. */
			if (write_acquisition_config(acquisition_config) != CFG_WRITE_OK)
			{
				/* "Acquisition config write failed!\n" */
				dbg("[TSP] [ERROR] line : %d\n", __LINE__);
				//ret = WRITE_MEM_FAILED; /* calling function should retry calibration call */
			}
			//dbg("[TSP] calibration really good\n");
		}
		else
		{
			cal_check_flag = 1u;
		}
	}
	else
	{
		/* Timer not enabled, so enable it */
		timer.expires = jiffies + msecs_to_jiffies(500);
		add_timer(&timer);
		timer_enabled = true;
		timer_tick = 0u;
		cal_check_flag = 1u;
	}   

	dbg_func_out();

}


/* Calibration Checking routine - called from interrupt handler */

/*!
 * \brief Used to ensure that calibration was good
 *
 * This function will check if the last calibration was good.
 * 
 * It should be called on every touch message whilst 'cal_check_flag' is set,
 * it will recalibrate the chip if the calibration is bad. If the calibration
 * is good it will restore the ATCHCALST and ATCHCALSTHR settings in the chip 
 * so that we do not get water issues.
 *
 *
 */
int8_t check_chip_calibration(void)
{
	uint8_t CAL_THR = 10;

	dbg_func_in();


	if (driver_setup != DRIVER_SETUP_OK)
		return -1;

	if(get_touch_antitouch_info()) {

		/* process counters and decide if we must re-calibrate or if cal was good */      
		if((debugInfo.tch_ch) && (debugInfo.atch_ch == 0))
		{
			//dbg("[TSP] calibration maybe good\n");
			cal_maybe_good();
			nCalCount =0;
			dbg_func_out();

			return 0;
		}
		else if((debugInfo.tch_ch + CAL_THR /*10*/ ) <= debugInfo.atch_ch || nCalCount > 2)
		{
			//dbg("[TSP] calibration was bad\n");

			clear_event(TSC_CLEAR_ALL);
			touch_data_init();

			/* cal was bad - must recalibrate and check afterwards */
			calibrate_chip();

			/* Disable the timer */
			del_timer(&timer);
			timer_enabled = false;
			timer_tick = 0;
			nCalCount =0;
			dbg_func_out();

			return -1;
		}
		else
		{
			nCalCount++;
			//dbg("[TSP] calibration was not decided yet\n");
			/* we cannot confirm if good or bad - we must wait for next touch 
			 * message to confirm */
			cal_check_flag = 1u;
			/* Reset the 100ms timer */
			if(!timer_enabled){
				timer.expires = jiffies + msecs_to_jiffies(500);
				add_timer(&timer);
			}   
			timer_enabled = true;
			timer_tick = 0u;

			dbg_func_out();

			return 0;
		}
	}

	dbg_func_out();

	return 0;
}


int8_t get_touch_antitouch_info(void)
{
	uint8_t data_buffer[100] = { 0 };
	uint8_t try_ctr = 0;
	uint8_t data_byte = 0xF3; /* dianostic command to get touch flags */
	uint16_t diag_address;
	uint8_t tch_ch = 0, atch_ch = 0;
	uint8_t check_mask;
	uint8_t i;
	uint8_t j;
	uint8_t x_line_limit;

	dbg_func_in();

	if (driver_setup != DRIVER_SETUP_OK)
		return -1;

	/* we have had the first touchscreen or face suppression message 
	 * after a calibration - check the sensor state and try to confirm if
	 * cal was good or bad */

	/* get touch flags from the chip using the diagnostic object */
	/* write command to command processor to get touch flags - 0xF3 Command required to do this */
	write_mem(command_processor_address + DIAGNOSTIC_OFFSET, 1, &data_byte);
	/* get the address of the diagnostic object so we can get the data we need */
	diag_address = get_object_address(DEBUG_DIAGNOSTIC_T37,0);

	msleep(20); 

	/* read touch flags from the diagnostic object - clear buffer so the while loop can run first time */
	memset( data_buffer , 0xFF, sizeof( data_buffer ) );

	/* wait for diagnostic object to update */
	while(!((data_buffer[0] == 0xF3) && (data_buffer[1] == 0x00)))
	{
		/* wait for data to be valid  */
		if(try_ctr > 100)
		{
			/* Failed! */
			//dbg("[TSP] Diagnostic Data did not update!!\n");
			break;
		}
		msleep(5); 
		try_ctr++; /* timeout counter */
		read_mem(diag_address, 2,data_buffer);
		//dbg("[TSP] Waiting for diagnostic data to update, try %d\n", try_ctr);
	}

	/* data is ready - read the detection flags */
	read_mem(diag_address, 82,data_buffer);

	/* data array is 20 x 16 bits for each set of flags, 2 byte header, 40 bytes for touch flags 40 bytes for antitouch flags*/

	/* count up the channels/bits if we recived the data properly */
	if((data_buffer[0] == 0xF3) && (data_buffer[1] == 0x00))
	{

		/* mode 0 : 16 x line, mode 1 : 17 etc etc upto mode 4.*/
		x_line_limit = 16 + cte_t46_config.mode;

		if(x_line_limit > 20)
		{
			/* hard limit at 20 so we don't over-index the array */
			x_line_limit = 20;
		}

		/* double the limit as the array is in bytes not words */
		x_line_limit = x_line_limit << 1;

		/* count the channels and print the flags to the log */
		for(i = 0; i < x_line_limit; i+=2) /* check X lines - data is in words so increment 2 at a time */
		{
			/* print the flags to the log - only really needed for debugging */
			//dbg("[TSP] Detect Flags X%d, %x%x, %x%x \n", i>>1,data_buffer[3+i],data_buffer[2+i],data_buffer[43+i],data_buffer[42+i]);

			/* count how many bits set for this row */
			for(j = 0; j < 8; j++)
			{
				/* create a bit mask to check against */
				check_mask = 1 << j;

				/* check detect flags */
				if(data_buffer[2+i] & check_mask)
				{
					tch_ch++;
				}
				if(data_buffer[3+i] & check_mask)
				{
					tch_ch++;
				}

				/* check anti-detect flags */
				if(data_buffer[42+i] & check_mask)
				{
					atch_ch++;
				}
				if(data_buffer[43+i] & check_mask)
				{
					atch_ch++;
				}
			}
		}

		/* print how many channels we counted */
		//dbg("[TSP] Flags Counted channels: t:%d a:%d \n", tch_ch, atch_ch);

		/* send page up command so we can detect when data updates next time,
		 * page byte will sit at 1 until we next send F3 command */
		data_byte = 0x01;
		write_mem(command_processor_address + DIAGNOSTIC_OFFSET, 1, &data_byte);

		debugInfo.tch_ch = tch_ch;
		debugInfo.atch_ch = atch_ch;

		return 1;
	}
	return 0;
}

/* Just an example, __timer is used to check if timer is enabled or not
   Timer should run every 100ms */

/* Timer interrupt which overflows every 100ms */
static void check_chip_calibration_timer_func(unsigned long data)
{
	//dbg("[TSP] check_chip_calibration_timer_func\n");
	timer_tick++;
}

#ifdef LOCKSCREEN_MODE
static void lockscreen_mode_timer_func(unsigned long data)
{
	queue_work(qt602240_wq, &qt602240_data->work_lockscreen_mode_disable);
}
#endif 

void  get_message(struct work_struct * p)
{
	unsigned long x, y;
	unsigned int press = 3;
	uint8_t ret_val = MESSAGE_READ_FAILED;
	int size=0,i;   
	uint8_t touch_message_flag = 0;
	uint8_t object_type, instance;
#ifdef QT_MULTITOUCH_ENABLE
	uint8_t id = 0;
	int bChangeUpDn= 0;
#endif
	uint8_t valid_area;
	uint8_t valid_key;	
	int8_t ret_checkcal;

	dbg_func_in();

	/* Get the lock */
	mutex_lock(&qt602240_data->lock);

	if (driver_setup == DRIVER_SETUP_OK)
	{
		if(read_mem(message_processor_address, max_message_length, quantum_msg) == READ_MEM_OK)
		{
			/* Call the main application to handle the message. */
			dbg("[TSP] msg id =  %d, msg = %x\n", quantum_msg[0], quantum_msg[1]);

			object_type = report_id_to_type(quantum_msg[0], &instance);

			if (object_type == TOUCH_MULTITOUCHSCREEN_T9)
			{
#ifdef QT_MULTITOUCH_ENABLE
				id= quantum_msg[0] - 2;
#endif

				/* Detect & Press */
				if( (( quantum_msg[1] & (TOUCH_DETECT|TOUCH_PRESS)) == (TOUCH_DETECT|TOUCH_PRESS)))                                
				{
					touch_message_flag = 1;
					dbg("[TSP] Down Interrupt : (msg id =  %d, msg = %x)\n", quantum_msg[0], quantum_msg[1]);
				}

				x = quantum_msg[2];
				x = x << 2;
				x = x | quantum_msg[4] >> 6;

				y = quantum_msg[3];
#if defined (CONFIG_SKY_EF39S_BOARD) | defined (CONFIG_SKY_EF40K_BOARD) | defined (CONFIG_SKY_EF40S_BOARD)  | defined (CONFIG_SKY_EF65L_BOARD)
				y = y << 4;
				y = y | ((quantum_msg[4] & 0xf) );
#else
				y = y << 2;
				y = y | ((quantum_msg[4] & 0x6)  >> 2);
#endif
				size = quantum_msg[5];

#ifdef QT_MULTITOUCH_ENABLE
				if ( (quantum_msg[1] & TOUCH_RELEASE) )    
				{
					fingerInfo[id].status= 0;
					bChangeUpDn= 1;
					//dbg("\n##### Finger[%d] Up (%d,%d)!\n\n", id, fingerInfo[id].x, fingerInfo[id].y );
				}
				else if ( (quantum_msg[1] & TOUCH_DETECT) && (quantum_msg[1] & TOUCH_MOVE) )  
				{
					fingerInfo[id].id= id;
					fingerInfo[id].status= 1;
					fingerInfo[id].x= (int16_t)x;
					fingerInfo[id].y= (int16_t)y;
					fingerInfo[id].area= size;
#if 1 //#if EF30S_BDVER_GE(EF30S_PT10) 
					bChangeUpDn= 1;
#endif

					//dbg("##### Finger[%d] Move (%d,%d)!\n", id, fingerInfo[id].x, fingerInfo[id].y );
				}
				else if ( (quantum_msg[1] & TOUCH_DETECT) && (quantum_msg[1] & TOUCH_PRESS) )       
				{                               
					fingerInfo[id].id= id;
					fingerInfo[id].status= 1;
					fingerInfo[id].x= (int16_t)x;
					fingerInfo[id].y= (int16_t)y;
					fingerInfo[id].area= size;
					bChangeUpDn= 1;
					//dbg("\n##### Finger[%d] Down (%d,%d)!\n\n", id, fingerInfo[id].x, fingerInfo[id].y );
				}
#else                    
				if( ((quantum_msg[1] & TOUCH_DETECT) == TOUCH_DETECT ) && ((quantum_msg[1] & TOUCH_PRESS) == TOUCH_PRESS) )   
				{
					press = 1;
					btn_report = 1;

					//input_report_key(qt602240_data->input_dev, BTN_TOUCH, 1);
				}
				else if( ((quantum_msg[1] & TOUCH_DETECT) == TOUCH_DETECT ) && ((quantum_msg[1] & TOUCH_MOVE) == TOUCH_MOVE) )    
				{
					press = 1;
				}       

				else if( ((quantum_msg[1] & TOUCH_RELEASE ) == TOUCH_RELEASE))   
				{
					press = 0;

					input_report_key(qt602240_data->input_dev, BTN_TOUCH, 0);
				}
#endif                  
				else
				{
					press = 3;
					dbg("[TSP] Unknown state ! status = %d \n",quantum_msg[1]);

					goto interrupt_return;
				}

				ret_val = MESSAGE_READ_OK;
			}                     
			else if (object_type == TOUCH_KEYARRAY_T15)
			{

				dbg("[TSP] TOUCH_KEYARRAY_T15 (quantum_msg[1]: %d, quantum_msg[3]: %d, quantum_msg[4] %d)!\n\n", quantum_msg[1], quantum_msg[3], quantum_msg[4]);

			}               
			else if (object_type == GEN_COMMANDPROCESSOR_T6) 
			{
				//Reset, Overflow, SigErr, Calibration, CFG Err ...
				dbg("[TSP] msg id =  %d, status = 0x%x \n", quantum_msg[0],quantum_msg[1]);

				if((quantum_msg [1] != 0x10) && (quantum_msg [1] != 0x00))
					goto interrupt_return;
			}
			else    
			{
				dbg("[TSP] msg id =  %d, status = 0x%x \n", quantum_msg[0],quantum_msg[1]);
#if 0
				dbg("[TSP] msg id =  %d ", quantum_msg[0]);

				for(i = 1; i  < max_message_length;i++)
				{
					dbg("%x ",quantum_msg[i]);

				}
				dbg("\n");
#endif
				goto interrupt_return;
			}

		}
		else {
			printk("[TSP] read_mem failed.\n");
			goto interrupt_return;
		}


		/* touch_message_flag - set when tehre is a touch message
		   facesup_message_flag - set when there is a face suppression message
		   */
		/* Check if the cal_check_flag is enabled, and if there are any touch or face suppression messages, check the calibration */      
		if(cal_check_flag == 1)
		{   
			ret_checkcal = 0;
			if(touch_message_flag /*|| facesup_message_flag*/) /* Check if the chip sent a face suppression message when in calibration */
			{
				ret_checkcal = check_chip_calibration();
#ifdef RECHECK_AFTER_CALIBRATION
				calibration_message = 0;
#endif
			}
#ifdef RECHECK_AFTER_CALIBRATION
			else
			{
				/* The Device is calibration. */
				if(quantum_msg[0] == 0x01 && quantum_msg[1] == 0x10)
				{
					calibration_message = 1;
					ret_checkcal = -1;                  
				}

				if((quantum_msg[0] == 0x01 && quantum_msg[1] == 0x00) && calibration_message)
				{             
					calibration_message = 0;
					//                	 msleep(100); 
					ret_checkcal = check_chip_calibration();
				}
			}
#endif	

			if(ret_checkcal == -1) {
				goto interrupt_return;
			}           

		}

	}
	else {
		goto interrupt_return;
	}

	valid_area = 0;

#ifdef QT_MULTITOUCH_ENABLE
	if (bChangeUpDn)
	{   	
		for ( i= 0; i<MAX_NUM_FINGER; i++ )
		{
			//        	    	printk("X : %d, Y: %d\n", fingerInfo[i].x, fingerInfo[i].y);

			if ( fingerInfo[i].status == -1 || (fingerInfo[i].mode == TSC_EVENT_NONE && fingerInfo[i].status == 0)) 
				continue;

			if(fingerInfo[i].mode == TSC_EVENT_NONE)
			{
				//                	printk("[TSP] Finger[%d] Down(TSC_EVENT_WINDOW) XY(%d, %d)\n", i, fingerInfo[i].x, fingerInfo[i].y);
				/* Touch Window ?????? Touch ?? ???? */
				if(fingerInfo[i].y < SCREEN_RESOLUTION_Y)
				{
					//printk("[QT602240] D:(%d, %d) (id:%d)\n", fingerInfo[i].x, fingerInfo[i].y, fingerInfo[i].id);  
					input_report_key(qt602240_data->input_dev, BTN_TOUCH, 1);
					input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_X, fingerInfo[i].x);
					input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_Y, fingerInfo[i].y);
				input_report_abs(qt602240_data->input_dev, ABS_MT_WIDTH_MAJOR, fingerInfo[i].area);   // TOUCH_SIZE
					input_mt_sync(qt602240_data->input_dev);

					fingerInfo[i].realx = fingerInfo[i].x;
					fingerInfo[i].realy = fingerInfo[i].y;
					fingerInfo[i].mode = TSC_EVENT_WINDOW;
					valid_area |= 1;
				}
				/* Button ?????? Touch ?? ???? */
				/* ???? ?????? ???????? ?? 2.5mm */ 
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD)  || defined (CONFIG_SKY_EF65L_BOARD)
				else if(fingerInfo[i].y >= TOUCH_KEY_Y)
				{
					/* Menu button touch */
					/* ???? ?????? ???????? ?? 7mm , ?? 4mm */
					if(fingerInfo[i].x >= TOUCH_MENU_MIN && fingerInfo[i].x <= TOUCH_MENU_MAX)
					{
						fingerInfo[i].mode = TSC_EVENT_MENU;
						keyInfo[i].code = TSC_EVENT_MENU;
					keyInfo[i].status = 1;
						keyInfo[i].update = true;
					}
#if defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || (defined(CONFIG_SKY_EF39S_BOARD) && (BOARD_REV > WS10)) || defined(CONFIG_SKY_EF65L_BOARD)
					/* Home button touch */
					/* ???? ?????? ???????? ?? 4mm , ?? 7mm*/
					else if(fingerInfo[i].x >= TOUCH_HOME_MIN && fingerInfo[i].x <= TOUCH_HOME_MAX)	
					{
						fingerInfo[i].mode = TSC_EVENT_HOME;          
						keyInfo[i].code = TSC_EVENT_HOME;
					keyInfo[i].status = 1;
						keyInfo[i].update = true;
					}
#endif
					/* Back button touch */
					/* ???? ?????? ???????? ?? 4mm , ?? 7mm*/
					else if(fingerInfo[i].x >= TOUCH_BACK_MIN && fingerInfo[i].x <= TOUCH_BACK_MAX)	
					{
						fingerInfo[i].mode = TSC_EVENT_BACK;          
						keyInfo[i].code = TSC_EVENT_BACK;
					keyInfo[i].status = 1;
						keyInfo[i].update = true;
					}
				}
#endif
			}
			else
			{
				if (fingerInfo[i].status == 0)
				{
					if(fingerInfo[i].mode == TSC_EVENT_WINDOW)
					{
						fingerInfo[i].realx = fingerInfo[i].x;
						if(fingerInfo[i].y >= SCREEN_RESOLUTION_Y)
							fingerInfo[i].realy = (SCREEN_RESOLUTION_Y-1);
						else
							fingerInfo[i].realy = fingerInfo[i].y;

						//printk("[QT602240] U:(%d, %d) (id:%d)\n", fingerInfo[i].realx, fingerInfo[i].realy, fingerInfo[i].id);
						input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_X, fingerInfo[i].realx);
						input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_Y, fingerInfo[i].realy);
						input_report_abs(qt602240_data->input_dev, ABS_MT_TOUCH_MAJOR, fingerInfo[i].status);
						input_report_abs(qt602240_data->input_dev, ABS_MT_WIDTH_MAJOR, fingerInfo[i].area);
						input_mt_sync(qt602240_data->input_dev);

						valid_area |= 1;
					}
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF65L_BOARD)
					else if(fingerInfo[i].mode == TSC_EVENT_MENU){
						keyInfo[i].code = TSC_EVENT_MENU;
						keyInfo[i].status = 0;
						keyInfo[i].update = true;
					}
#if defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || (defined(CONFIG_SKY_EF39S_BOARD) && (BOARD_REV > WS10)) || defined(CONFIG_SKY_EF65L_BOARD)
					else if(fingerInfo[i].mode == TSC_EVENT_HOME){
						keyInfo[i].code = TSC_EVENT_HOME;
						keyInfo[i].status = 0;
						keyInfo[i].update = true;
					}
#endif
					else if(fingerInfo[i].mode == TSC_EVENT_BACK){
						keyInfo[i].code = TSC_EVENT_BACK;
						keyInfo[i].status = 0;
						keyInfo[i].update = true;
					}
#endif                        
					fingerInfo[i].mode = TSC_EVENT_NONE;
					//fingerInfo[i].status= -1;
				}
				else if(fingerInfo[i].mode == TSC_EVENT_WINDOW)
				{
					fingerInfo[i].realx = fingerInfo[i].x;
					if(fingerInfo[i].y >= SCREEN_RESOLUTION_Y)
						fingerInfo[i].realy = (SCREEN_RESOLUTION_Y-1);
					else
						fingerInfo[i].realy = fingerInfo[i].y;

					dbg("[TSP] Finger[%d] Move(TSC_EVENT_WINDOW) XY(%d, %d)\n", i, fingerInfo[i].realx, fingerInfo[i].realy);
					input_report_key(qt602240_data->input_dev, BTN_TOUCH, 1);
				input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_X, fingerInfo[i].realx);				// TOUCH_X
				input_report_abs(qt602240_data->input_dev, ABS_MT_POSITION_Y, fingerInfo[i].realy);				// TOUCH_Y
				input_report_abs(qt602240_data->input_dev, ABS_MT_TOUCH_MAJOR, fingerInfo[i].status);
				input_report_abs(qt602240_data->input_dev, ABS_MT_WIDTH_MAJOR, fingerInfo[i].area);   // TOUCH_SIZE
					input_mt_sync(qt602240_data->input_dev);
					valid_area |= 1;
				}

				else {
					if(fingerInfo[i].status == 1 && (fingerInfo[i].y >= SCREEN_RESOLUTION_Y && fingerInfo[i].y < TOUCH_KEY_Y)) {
						dbg("Move Key area to null key area\n");
						keyInfo[i].status = 0;
						keyInfo[i].update = true;
						fingerInfo[i].mode = TSC_EVENT_NONE;
					}
					else {
						continue;
					}
				}
			}

			if(fingerInfo[i].status == 0) fingerInfo[i].status= -1;
		}

		if(valid_area != 0)
			input_sync(qt602240_data->input_dev);
	}   

	valid_key = 0;

#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF65L_BOARD)
	for( i= 0; i<MAX_NUM_FINGER; i++)
	{
		if(keyInfo[i].update)
		{
			if(keyInfo[i].code == TSC_EVENT_MENU){
				printk("[QT602240]M:(%d)\n", keyInfo[i].status);
				input_report_key(qt602240_data->input_dev, KEY_MENU, keyInfo[i].status);
				valid_key |= 1;
			}
#if defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || (defined(CONFIG_SKY_EF39S_BOARD) && (BOARD_REV > WS10)) || defined(CONFIG_SKY_EF65L_BOARD)
			else if(keyInfo[i].code == TSC_EVENT_HOME){
				printk("[QT602240]H:(%d)\n", keyInfo[i].status);
				input_report_key(qt602240_data->input_dev, KEY_HOME, keyInfo[i].status);
				valid_key |= 1;
			} 
#endif
			else if(keyInfo[i].code == TSC_EVENT_BACK){
				printk("[QT602240]B:(%d)\n", keyInfo[i].status);
				input_report_key(qt602240_data->input_dev, KEY_BACK, keyInfo[i].status);
				valid_key |= 1;
			}    
		}
		keyInfo[i].update = false;
	}
#endif

#else // QT_MULTITOUCH_ENABLE
	if(press == 1)
	{
		input_report_abs(qt602240_data->input_dev, ABS_X, x);
		input_report_abs(qt602240_data->input_dev, ABS_Y, y);
		if( btn_report == 1)
			input_report_key(qt602240_data->input_dev, BTN_TOUCH, 1);

		input_sync(qt602240_data->input_dev);
		amplitude = quantum_msg[6];
		dbg("[TSP]%s x=%3d, y=%3d, BTN=%d, size=%d, amp=%d\n",__FUNCTION__, x, y,press, size , amplitude);

	}
	else if(press == 0)
	{
		input_sync(qt602240_data->input_dev);
		amplitude = quantum_msg[6];
		dbg("[TSP]%s x=%3d, y=%3d, BTN=%d, size=%d, amp=%d\n",__FUNCTION__, x, y,press, size , amplitude);
	}
#endif // QT_MULTITOUCH_ENABLE


interrupt_return:
	enable_irq(qt602240_data->client->irq);
	mutex_unlock(&qt602240_data->lock);

	dbg_func_out();
	return ;
}


/*------------------------------ I2C Driver block -----------------------------------*/
#define I2C_M_WR 0 /* for i2c */
#define I2C_MAX_SEND_LENGTH     300
int qt602240_i2c_write(u16 reg, u8 *read_val, unsigned int len)
{
	struct i2c_msg wmsg;
	unsigned char data[I2C_MAX_SEND_LENGTH];
	int ret,i;

	//dbg_func_in();

	address_pointer = reg;

	if(len+2 > I2C_MAX_SEND_LENGTH)
	{
		dbg("[TSP][ERROR] %s() data length error\n", __FUNCTION__);
		return -ENODEV;
	}

	wmsg.addr = qt602240_data->client->addr;
	wmsg.flags = I2C_M_WR;
	wmsg.len = len + 2;
	wmsg.buf = data;

	data[0] = reg & 0x00ff;
	data[1] = reg >> 8;

	for (i = 0; i < len; i++)
	{
		data[i+2] = *(read_val+i);
	}

	ret = i2c_transfer(qt602240_data->client->adapter, &wmsg, 1);

	//dbg_func_out();

	return ret;
}

int boot_qt602240_i2c_write(u16 reg, u8 *read_val, unsigned int len)
{
	struct i2c_msg wmsg;
	unsigned char data[I2C_MAX_SEND_LENGTH];
	int ret,i;

	if(len+2 > I2C_MAX_SEND_LENGTH)
	{
		dbg("[TSP][ERROR] %s() data length error\n", __FUNCTION__);
		return -ENODEV;
	}

	wmsg.addr = QT602240_I2C_BOOT_ADDR;
	wmsg.flags = I2C_M_WR;
	wmsg.len = len;
	wmsg.buf = data;


	for (i = 0; i < len; i++)
	{
		data[i] = *(read_val+i);
	}

	ret = i2c_transfer(qt602240_data->client->adapter, &wmsg, 1);

	return ret;
}


int qt602240_i2c_read(u16 reg,unsigned char *rbuf, int buf_size)
{
	static unsigned char first_read=1;
	struct i2c_msg rmsg;
	int ret;
	unsigned char data[2];

	rmsg.addr = qt602240_data->client->addr;

	if(first_read == 1)
	{
		first_read = 0;
		address_pointer = reg+1;
	}

	if((address_pointer != reg) || (reg != message_processor_address))
	{
		address_pointer = reg;

		rmsg.flags = I2C_M_WR;
		rmsg.len = 2;
		rmsg.buf = data;
		data[0] = reg & 0x00ff;
		data[1] = reg >> 8;
		ret = i2c_transfer(qt602240_data->client->adapter, &rmsg, 1);
	}

	rmsg.flags = I2C_M_RD;
	rmsg.len = buf_size;
	rmsg.buf = rbuf;
	ret = i2c_transfer(qt602240_data->client->adapter, &rmsg, 1);

	return ret;
}

/*! \brief Maxtouch Memory read by I2C bus */
U8 read_mem(U16 start, U8 size, U8 *mem)
{
	int ret;
	U8 rc;

	memset(mem,0xFF,size);
	ret = qt602240_i2c_read(start,mem,size);
	if(ret < 0) {
		dbg("%s : i2c read failed\n",__func__);
		rc = READ_MEM_FAILED;
	}
	else {
		rc = READ_MEM_OK;
	}

	return rc;
}

U8 boot_read_mem(U16 start, U8 size, U8 *mem)
{
	struct i2c_msg rmsg;
	int ret;

	dbg_func_in();

	rmsg.addr = QT602240_I2C_BOOT_ADDR;
	rmsg.flags = I2C_M_RD;
	rmsg.len = size;
	rmsg.buf = mem;
	ret = i2c_transfer(qt602240_data->client->adapter, &rmsg, 1);

	dbg_func_out();

	return ret;
}

U8 read_U16(U16 start, U16 *mem)
{
	U8 status;
	status = read_mem(start, 2, (U8 *) mem);
	return status;
}

U8 write_mem(U16 start, U8 size, U8 *mem)
{
	int ret;
	U8 rc;

	ret = qt602240_i2c_write(start,mem,size);
	if(ret < 0) 
		rc = WRITE_MEM_FAILED;
	else
		rc = WRITE_MEM_OK;

	return rc;
}

U8 boot_write_mem(U16 start, U16 size, U8 *mem)
{
	int ret;
	U8 rc;

	dbg_func_in();

	ret = boot_qt602240_i2c_write(start,mem,size);
	if(ret < 0) {
		dbg("boot write mem fail: %d \n",ret);
		rc = WRITE_MEM_FAILED;
	}
	else {
		rc = WRITE_MEM_OK;
	}

	dbg_func_out();

	return rc;
}

void write_message_to_usart(uint8_t msg[], uint8_t length)
{
	int i;
	dbg_func_in();

	for (i=0; i < length; i++)
	{
		dbg_raw("0x%02x ", msg[i]);
	}
	dbg_raw("\n\r");

	dbg_func_out();
}

irqreturn_t qt602240_irq_handler(int irq, void *dev_id)
{
	//dbg_func_in();
	disable_irq_nosync(qt602240_data->client->irq);
	queue_work(qt602240_wq, &qt602240_data->work);
	//dbg_func_out();
	return IRQ_HANDLED;
}


static int qt602240_remove(struct i2c_client *client)
{
	dbg_func_in();

	if(qt602240_data->client->irq)
	{
		free_irq(qt602240_data->client->irq, qt602240_data);
	}
	mutex_destroy(&qt602240_data->lock);	
#ifdef SKY_PROCESS_CMD_KEY
	misc_deregister(&touch_event);
#endif
#ifdef TOUCH_IO
	misc_deregister(&touch_io);
#endif //TOUCH_IO
#if defined(CONFIG_PM) && defined(CONFIG_HAS_EARLYSUSPEND)
	unregister_early_suspend(&qt602240_data->es);
#endif
	input_unregister_device(qt602240_data->input_dev);
	input_free_device(qt602240_data->input_dev);

	device_destroy(touch_atmel_class, 0);
	class_destroy(touch_atmel_class);

	if (qt602240_wq)
		destroy_workqueue(qt602240_wq);
	kfree(qt602240_data);        

	off_hw_setting();
	dbg_func_out();
	return 0;
}

#if defined(CONFIG_PM) && defined(CONFIG_HAS_EARLYSUSPEND)
static int qt602240_early_suspend(struct early_suspend *h)
{
	dbg_func_in();
#ifdef CHARGER_MODE
	tsp_power_status = 0;
#endif
	disable_irq(qt602240_data->client->irq);

	del_timer(&timer);
	timer_enabled = false;
#ifdef LOCKSCREEN_MODE 
	cancel_work_sync(&qt602240_data->work_lockscreen_mode_enable);
	cancel_work_sync(&qt602240_data->work_lockscreen_mode_disable);
	del_timer(&lockscreen_mode_timer);
#endif
	qt_Power_Sleep();
	clear_event(TSC_CLEAR_ALL);
	dbg_func_out();
	return 0;
}

static int  qt602240_late_resume(struct early_suspend *h)
{
	dbg_func_in();

	touch_data_init();
	qt_Power_Config_Init();

#ifdef CHARGER_MODE
	tsp_power_status = 1;
	qt_charger_mode_config(charger_mode);
#endif 
#ifdef LOCKSCREEN_MODE 
	queue_work(qt602240_wq, &qt602240_data->work_lockscreen_mode_enable);
	mod_timer(&lockscreen_mode_timer, jiffies + msecs_to_jiffies(LOCKSCREEN_MODE_TIMER_TICK_PERIOD));
#endif
	calibrate_chip();
	enable_irq(qt602240_data->client->irq);
	dbg_func_out();
	return 0;
}
#endif // CONFIG_PM && CONFIG_HAS_EARLYSUSPEND


/* I2C driver probe function */
static int __devinit qt602240_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	int rc;
	dbg_func_in();
	qt602240_data = kzalloc(sizeof(struct qt602240_data_t), GFP_KERNEL);
	if (qt602240_data == NULL)
	{
		pr_err("qt602240_data is not NULL.\n");
		return -ENOMEM;
	}
	qt602240_data->client = client;

	qt602240_wq = create_singlethread_workqueue("qt602240_wq");
	if (!qt602240_wq)
	{
		pr_err("create_singlethread_workqueue(qt602240_wq) error.\n");
		return -ENOMEM;
	}

	if(!touch_atmel_class)
		touch_atmel_class=class_create(THIS_MODULE, "touch_ateml");

	ts_dev = device_create(touch_atmel_class, NULL, 0, NULL, "ts");
	if (IS_ERR(ts_dev))
		pr_err("Failed to create device(ts)!\n");
	if (device_create_file(ts_dev, &dev_attr_gpio) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_gpio.attr.name);
	if (device_create_file(ts_dev, &dev_attr_i2c) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_i2c.attr.name);
	if (device_create_file(ts_dev, &dev_attr_setup) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_setup.attr.name);

	dbg("ts_dev creation : success.\n");

	dbg("+-----------------------------------------+\n");
	dbg("|  Quantum Touch Driver Probe!            |\n");
	dbg("+-----------------------------------------+\n");

	INIT_WORK(&qt602240_data->work, get_message );
#ifdef LOCKSCREEN_MODE
	INIT_WORK(&qt602240_data->work_lockscreen_mode_enable, qt_enable_lockscreen_mode);
	INIT_WORK(&qt602240_data->work_lockscreen_mode_disable, qt_Acquisition_Config_Init_locked);

	init_timer(&lockscreen_mode_timer);
	lockscreen_mode_timer.function = lockscreen_mode_timer_func;
	lockscreen_mode_timer.data =0;
#endif

	qt602240_data->input_dev = input_allocate_device();
	if (qt602240_data->input_dev == NULL)
	{
		rc = -ENOMEM;
		pr_err("qt602240_probe: Failed to allocate input device\n");
		goto err_input_dev_alloc_failed;
	}

	qt602240_data->input_dev->name = "qt602240_ts_input";
	set_bit(EV_SYN, qt602240_data->input_dev->evbit);
	set_bit(EV_KEY, qt602240_data->input_dev->keybit);
	set_bit(BTN_TOUCH, qt602240_data->input_dev->keybit);

	set_bit(KEY_MENU, qt602240_data->input_dev->keybit);
	set_bit(KEY_HOME, qt602240_data->input_dev->keybit);
	set_bit(KEY_BACK, qt602240_data->input_dev->keybit);

	set_bit(EV_ABS, qt602240_data->input_dev->evbit);

#ifdef SKY_PROCESS_CMD_KEY
	set_bit(KEY_SEARCH, qt602240_data->input_dev->keybit);
	set_bit(KEY_HOME, qt602240_data->input_dev->keybit);

	set_bit(KEY_0, qt602240_data->input_dev->keybit);
	set_bit(KEY_1, qt602240_data->input_dev->keybit);
	set_bit(KEY_2, qt602240_data->input_dev->keybit);
	set_bit(KEY_3, qt602240_data->input_dev->keybit);
	set_bit(KEY_4, qt602240_data->input_dev->keybit);
	set_bit(KEY_5, qt602240_data->input_dev->keybit);
	set_bit(KEY_6, qt602240_data->input_dev->keybit);
	set_bit(KEY_7, qt602240_data->input_dev->keybit);
	set_bit(KEY_8, qt602240_data->input_dev->keybit);
	set_bit(KEY_9, qt602240_data->input_dev->keybit);
	set_bit(0xe3, qt602240_data->input_dev->keybit); /* '*' */
	set_bit(0xe4, qt602240_data->input_dev->keybit); /* '#' */

	set_bit(KEY_LEFTSHIFT, qt602240_data->input_dev->keybit);
	set_bit(KEY_RIGHTSHIFT, qt602240_data->input_dev->keybit);

	set_bit(KEY_LEFT, qt602240_data->input_dev->keybit);
	set_bit(KEY_RIGHT, qt602240_data->input_dev->keybit);
	set_bit(KEY_UP, qt602240_data->input_dev->keybit);
	set_bit(KEY_DOWN, qt602240_data->input_dev->keybit);
	set_bit(KEY_ENTER, qt602240_data->input_dev->keybit);

	set_bit(KEY_SEND, qt602240_data->input_dev->keybit);
	set_bit(KEY_END, qt602240_data->input_dev->keybit);

	set_bit(KEY_VOLUMEUP, qt602240_data->input_dev->keybit);
	set_bit(KEY_VOLUMEDOWN, qt602240_data->input_dev->keybit);

	set_bit(KEY_CLEAR, qt602240_data->input_dev->keybit);

	set_bit(KEY_CAMERA, qt602240_data->input_dev->keybit);
	set_bit(KEY_F3, qt602240_data->input_dev->keybit);
	//    set_bit(KEY_HOLD, qt602240_data->input_dev->keybit);
#endif // SKY_PROCESS_CMD_KEY

	input_set_abs_params(qt602240_data->input_dev, ABS_X, 0, SCREEN_RESOLUTION_X, 0, 0);
	input_set_abs_params(qt602240_data->input_dev, ABS_Y, 0, SCREEN_RESOLUTION_Y, 0, 0);
	input_set_abs_params(qt602240_data->input_dev, ABS_PRESSURE, 0, 255, 0, 0);
	input_set_abs_params(qt602240_data->input_dev, ABS_TOOL_WIDTH, 0, 15, 0, 0);
#ifdef QT_MULTITOUCH_ENABLE
	input_set_abs_params(qt602240_data->input_dev, ABS_MT_POSITION_X, 0, SCREEN_RESOLUTION_X-1, 0, 0);
	input_set_abs_params(qt602240_data->input_dev, ABS_MT_POSITION_Y, 0, SCREEN_RESOLUTION_Y-1, 0, 0);
	input_set_abs_params(qt602240_data->input_dev, ABS_MT_TOUCH_MAJOR, 0, 255, 0, 0);
	input_set_abs_params(qt602240_data->input_dev, ABS_MT_WIDTH_MAJOR, 0, 15, 0, 0);
#endif  

	rc = input_register_device(qt602240_data->input_dev);
	if (rc) {
		pr_err("qt602240_probe: Unable to register %s input device\n", qt602240_data->input_dev->name);
		goto err_input_register_device_failed;
	}
	dbg("input_register_device : success.\n");

#ifdef TOUCH_IO  
	rc = misc_register(&touch_io);
	if (rc) 
	{
		pr_err("::::::::: can''t register qt602240 misc\n");
	}
#endif //TOUCH_IO

	mutex_init(&qt602240_data->lock);

#ifdef QT_FIRMUP_ENABLE
	QT_reprogram();
#else
	quantum_touch_probe();
#endif

#ifdef CHIP_NOINIT
	if(Chip_NoInit)
	{
		TSP_Restart();
		quantum_touch_probe();

		Chip_NoInit = false;
	}
#endif

	qt602240_data->client->irq = IRQ_TOUCH_INT;
	rc = request_irq(qt602240_data->client->irq, qt602240_irq_handler, IRQF_TRIGGER_LOW, "qt602240-irq", qt602240_data);
	if (!rc)
	{
		dbg("request_irq : success.\n");
		dbg("qt602240_probe: Start touchscreen %s\n", qt602240_data->input_dev->name);
	}
	else
	{
		printk("[QT602240]request_irq failed : %d\n", rc);
	}

#if defined(CONFIG_PM) && defined(CONFIG_HAS_EARLYSUSPEND)
	qt602240_data->es.level = EARLY_SUSPEND_LEVEL_BLANK_SCREEN;
	qt602240_data->es.suspend = (void*)qt602240_early_suspend;
	qt602240_data->es.resume = (void*)qt602240_late_resume;
	register_early_suspend(&qt602240_data->es);
#endif

#ifdef SKY_PROCESS_CMD_KEY
	rc = misc_register(&touch_event);
	if (rc) {
		pr_err("::::::::: can''t register touch_fops\n");
	}
#endif    

	init_timer(&timer);
	timer.function = check_chip_calibration_timer_func;
	timer.data = (unsigned long)(&timer_tick);        

	dbg_func_out();
	return 0;

err_input_register_device_failed:
	input_free_device(qt602240_data->input_dev);

err_input_dev_alloc_failed:
	kfree(qt602240_data);
	pr_err("qt602240 probe failed: rc=%d\n", rc);

	return rc;
}

#ifdef ITO_TYPE_CHECK
// Read ADC Value (Platform dependent codes)
static int check_analog_mpp(int channel,int *mv_reading)                   // read adc value 
{
	int ret;
	void *h;
	struct adc_chan_result adc_chan_result;
	struct completion  conv_complete_evt;
	dbg_func_in();
	ret = adc_channel_open(channel, &h);
	if (ret) {
		pr_err("%s: couldnt open channel %d ret=%d\n",
				__func__, channel, ret);
		goto out;
	}
	init_completion(&conv_complete_evt);
	ret = adc_channel_request_conv(h, &conv_complete_evt);
	if (ret) {
		pr_err("%s: couldnt request conv channel %d ret=%d\n",
				__func__, channel, ret);
		goto out;
	}
	wait_for_completion(&conv_complete_evt);
	ret = adc_channel_read_result(h, &adc_chan_result);
	if (ret) {
		pr_err("%s: couldnt read result channel %d ret=%d\n",
				__func__, channel, ret);
		goto out;
	}
	ret = adc_channel_close(h);
	if (ret) {
		pr_err("%s: couldnt close channel %d ret=%d\n",
				__func__, channel, ret);
	}
	if (mv_reading)
		*mv_reading = adc_chan_result.measurement;

	pr_debug("%s: done for %d\n", __func__, channel);
	dbg_func_out();
	return adc_chan_result.physical;
out:
	pr_debug("%s: done for %d\n", __func__, channel);
	return -EINVAL;
}
#endif

int init_hw_setting(void)
{
#ifdef ITO_TYPE_CHECK
	int i;
	int adc_value = -1;
#endif
	int rc; 
	unsigned gpioConfig;
	struct regulator *vreg_touch_2_8;
	struct regulator *vreg_touch_1_8;
#if (defined(CONFIG_SKY_EF40K_BOARD)||defined(CONFIG_SKY_EF40S_BOARD)) && (BOARD_REV >= WS20)
	struct regulator *vreg_lvs2b_1_8;
#endif
	dbg_func_in();

	// Init 2.8V regulator
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF65L_BOARD)
	vreg_touch_2_8 = regulator_get(NULL, "8058_l2");
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
	vreg_touch_2_8 = regulator_get(NULL, "8058_l17");
#endif

	if (IS_ERR(vreg_touch_2_8)) {
		rc = PTR_ERR(vreg_touch_2_8);
		printk(KERN_ERR "[QT602240]%s: regulator get of %s failed (%d)\n",
				__func__, "vreg_touch_power", rc);
	}

		rc = regulator_set_voltage(vreg_touch_2_8, 3300000, 3300000);

	if (rc) {
		printk(KERN_ERR "[QT602240]%s: vreg set level failed (%d)\n", __func__, rc);
		return 1;
	}
	rc = regulator_enable(vreg_touch_2_8);
	printk("[QT602240]Touch Power regulator_enable return:  %d \n", rc);
#if (defined(CONFIG_SKY_EF40K_BOARD)||defined(CONFIG_SKY_EF40S_BOARD)) && (BOARD_REV == TP20)
	gpio_request(GPIO_TOUCH_ENABLE1_AVdd, "touch_enable1_AVdd");
	gpioConfig = GPIO_CFG(GPIO_TOUCH_ENABLE1_AVdd, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA);
	rc = gpio_tlmm_config(gpioConfig, GPIO_CFG_ENABLE);
	if (rc) {
		printk(KERN_ERR "%s: GPIO_TOUCH_ENABLE1_AVdd failed (%d)\n",__func__, rc);
		return -1;
	}
	gpio_set_value(GPIO_TOUCH_ENABLE1_AVdd, 1);
#endif
	regulator_put(vreg_touch_2_8);

	// Init 1.8V regulator
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF65L_BOARD)
	vreg_touch_1_8 = regulator_get(NULL, "8058_s3");
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
	vreg_touch_1_8 = regulator_get(NULL, "8058_l20");
#endif

	if (IS_ERR(vreg_touch_1_8)) {
		rc = PTR_ERR(vreg_touch_1_8);
		printk(KERN_ERR "[QT602240]%s: regulator get of %s failed (%d)\n",
				__func__, "vreg_touch_1_8", rc);
	}

	rc = regulator_set_voltage(vreg_touch_1_8, 1800000, 1800000);
	if (rc) {
		printk(KERN_ERR "[QT602240]%s: vreg set level failed (%d)\n", __func__, rc);
		return 1;
	}
	rc = regulator_enable(vreg_touch_1_8);
	printk("[QT602240]8058_s3 regulator_enable return:  %d \n", rc);
#if (defined(CONFIG_SKY_EF40K_BOARD)||defined(CONFIG_SKY_EF40S_BOARD)) && (BOARD_REV == TP20)
	gpio_request(GPIO_TOUCH_ENABLE2_Vdd, "touch_enable2_DVdd");
	gpioConfig = GPIO_CFG(GPIO_TOUCH_ENABLE2_Vdd, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA);
	rc = gpio_tlmm_config(gpioConfig, GPIO_CFG_ENABLE);
	if (rc) {
		printk(KERN_ERR "%s: GPIO_TOUCH_ENABLE2_Vdd failed (%d)\n",__func__, rc);
		return -1;
	}
	gpio_set_value(GPIO_TOUCH_ENABLE2_Vdd, 1);
#endif
	regulator_put(vreg_touch_1_8);
#if (defined(CONFIG_SKY_EF40K_BOARD)||defined(CONFIG_SKY_EF40S_BOARD)) && (BOARD_REV >= WS20)
	vreg_lvs2b_1_8 = regulator_get(NULL, "8901_lvs2");
	if (IS_ERR(vreg_lvs2b_1_8)) {
		rc = PTR_ERR(vreg_lvs2b_1_8);
		printk(KERN_ERR "[QT602240]%s: regulator get of %s failed (%d)\n",
				__func__, "vreg_lvs2b_1_8", rc);
	}
	
	rc = regulator_enable(vreg_lvs2b_1_8);
	printk("[QT602240]8901 LVS2 regulator_enable return:  %d \n", rc);
	regulator_put(vreg_lvs2b_1_8);
#endif

	// Init Reset GPIO 
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF65L_BOARD)
	gpio_request(GPIO_TOUCH_RST, "touch_rst_n");
	gpioConfig = GPIO_CFG(GPIO_TOUCH_RST, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA);
	rc = gpio_tlmm_config(gpioConfig, GPIO_CFG_ENABLE);
	if (rc) {
		printk(KERN_ERR "%s: GPIO_TOUCH_RST failed (%d)\n",__func__, rc);
		return -1;
	}      
#endif
	
	// Init Interrupt GPIO
	gpio_request(GPIO_TOUCH_CHG, "touch_chg_int");
	gpioConfig = GPIO_CFG(GPIO_TOUCH_CHG, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA);
	rc = gpio_tlmm_config(gpioConfig, GPIO_CFG_ENABLE);
	if (rc) {
		printk(KERN_ERR "%s: GPIO_TOUCH_CHG failed (%d)\n",__func__, rc);
		return -1;
	}        

	TSP_Restart();
	msleep(100);

	//Get Color Information
#ifdef ITO_TYPE_CHECK
#if  defined (CONFIG_SKY_EF65L_BOARD)	// pz2123
	pm8058_mpp_config_analog_input(XOADC_MPP_10,PM_MPP_AIN_AMUX_CH5, PM_MPP_AOUT_CTL_DISABLE);
	check_analog_mpp(CHANNEL_ADC_HDSET, &adc_value);// read analog input 
	pm8058_mpp_config_analog_input(XOADC_MPP_10,PM_MPP_AIN_AMUX_CH7, PM_MPP_AOUT_CTL_DISABLE); // reset MPP AMUX Setting
#else 	
	pm8058_mpp_config_analog_input(XOADC_MPP_6,PM_MPP_AIN_AMUX_CH5, PM_MPP_AOUT_CTL_DISABLE);
	check_analog_mpp(CHANNEL_ADC_HDSET, &adc_value);// read analog input 
	pm8058_mpp_config_analog_input(XOADC_MPP_6,PM_MPP_AIN_AMUX_CH7, PM_MPP_AOUT_CTL_DISABLE); // reset MPP AMUX Setting

//	pm8058_mpp_config_analog_input(XOADC_MPP_6,PM_MPP_AIN_AMUX_CH7, PM_MPP_AOUT_CTL_DISABLE);
//	check_analog_mpp(CHANNEL_ADC_TOUCH_ID, &adc_value);// read analog input 
	
#endif
	for(i = 0; i < number_of_elements(ito_table); i++) {
		if(adc_value >= ito_table[i].min && adc_value <= ito_table[i].max) {
			tsp_ito_type = i;
			break;
		}
	}
	if (tsp_ito_type >= 0) {
		printk("[TSP] Color: %d (adc value:%d)\n", tsp_ito_type, adc_value);
	}
	else {
		printk("[TSP] ERROR! undefined ito type.\n");
	}
#endif
	dbg_func_out();
	return 0;
}
void off_hw_setting(void)
{
	int rc; 
	struct regulator *vreg_touch_2_8;
	struct regulator *vreg_touch_1_8;
#if defined(CONFIG_SKY_EF40K_BOARD) && (BOARD_REV >= WS20)
	struct regulator *vreg_lvs2b_1_8;
#endif
	dbg_func_in();

	// Init 2.8V regulator
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD)  || defined (CONFIG_SKY_EF65L_BOARD)
	vreg_touch_2_8 = regulator_get(NULL, "8058_l2");
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
	vreg_touch_2_8 = regulator_get(NULL, "8058_l17");
#endif

	if (IS_ERR(vreg_touch_2_8)) {
		rc = PTR_ERR(vreg_touch_2_8);
		printk(KERN_ERR "[QT602240]%s: regulator get of %s failed (%d)\n",
				__func__, "vreg_touch_power", rc);
	}

	rc = regulator_disable(vreg_touch_2_8);
	printk("[QT602240]Touch Power regulator_disable return:  %d \n", rc);
	regulator_put(vreg_touch_2_8);

	// Init 1.8V regulator
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined (CONFIG_SKY_EF65L_BOARD)
	vreg_touch_1_8 = regulator_get(NULL, "8058_s3");
#elif defined(CONFIG_PANTECH_PRESTO_BOARD)
	vreg_touch_1_8 = regulator_get(NULL, "8058_l20");
#endif

	if (IS_ERR(vreg_touch_1_8)) {
		rc = PTR_ERR(vreg_touch_1_8);
		printk(KERN_ERR "[QT602240]%s: regulator get of %s failed (%d)\n",
				__func__, "vreg_touch_1_8", rc);
	}

	rc = regulator_disable(vreg_touch_1_8);
	printk("[QT602240]8058_s3 regulator_disable return:  %d \n", rc);
	regulator_put(vreg_touch_1_8);
#if defined(CONFIG_SKY_EF40K_BOARD) && (BOARD_REV >= WS20)
	vreg_lvs2b_1_8 = regulator_get(NULL, "8901_lvs2");
	if (IS_ERR(vreg_lvs2b_1_8)) {
		rc = PTR_ERR(vreg_lvs2b_1_8);
		printk(KERN_ERR "[QT602240]%s: regulator get of %s failed (%d)\n",
				__func__, "vreg_lvs2b_1_8", rc);
	}
	rc = regulator_disable(vreg_lvs2b_1_8);
	printk("[QT602240]8901 LVS2 regulator_disable return:  %d \n", rc);
	regulator_put(vreg_lvs2b_1_8);
#endif

	gpio_free(GPIO_TOUCH_CHG);

	dbg_func_out();

	msleep(100);

}
void  qt602240_front_test_init(void)
{
	disable_irq(qt602240_data->client->irq);

	del_timer(&timer);
	timer_enabled = false;

	off_hw_setting();
	init_hw_setting();
	quantum_touch_probe();

	enable_irq(qt602240_data->client->irq);
	return ;
}

static ssize_t i2c_show(struct device *dev, struct device_attribute *attr, char *buf)
{       
	int ret;
	unsigned char read_buf[5];

	dbg_func_in();

	ret = qt602240_i2c_read(0,read_buf, 5);
	if (ret < 0) {
		dbg("qt602240 i2c read failed.\n");
	}

	dbg_func_out();
	return sprintf(buf, "%s\n", buf);
}

static ssize_t i2c_store(
		struct device *dev, struct device_attribute *attr,
		const char *buf, size_t size)
{
	dbg_func_in();
	dbg_func_out();
	return size;
}

static ssize_t gpio_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	dbg_func_in();
	dbg_func_out();
	return sprintf(buf, "%s\n", buf);
}

static ssize_t gpio_store(
		struct device *dev, struct device_attribute *attr,
		const char *buf, size_t size)
{
	dbg_func_in();

	/*
	   if(strncmp(buf, "ENHIGH", 6) == 0 || strncmp(buf, "enhigh", 6) == 0) {
	   gpio_set_value(GPIO_TOUCH_EN, 1);
	   dbg("set TOUCH_EN High.\n");
	   msleep(100);
	   }
	   if(strncmp(buf, "ENLOW", 5) == 0 || strncmp(buf, "enlow", 5) == 0) {
	   gpio_set_value(GPIO_TOUCH_EN, 0);
	   dbg("set TOUCH_EN Low.\n");
	   msleep(100);
	   }
	   */
	if(strncmp(buf, "RSTHIGH", 7) == 0 || strncmp(buf, "rsthigh", 7) == 0) {
		gpio_set_value(GPIO_TOUCH_RST, 1);
		dbg("(touch)set TOUCH_RST High.\n");
		msleep(100);
	}
	if(strncmp(buf, "RSTLOW", 6) == 0 || strncmp(buf, "rstlow", 6) == 0) {
		gpio_set_value(GPIO_TOUCH_RST, 0);
		dbg("(touch) TOUCH_RST Low.\n");
		msleep(100);
	}

	dbg_func_out();
	return size;
}

void TSP_Restart(void)
{
	dbg_func_in();
#if defined(CONFIG_SKY_EF39S_BOARD) || defined(CONFIG_SKY_EF40K_BOARD) || defined(CONFIG_SKY_EF40S_BOARD) || defined(CONFIG_SKY_EF65L_BOARD)
	msleep(10);
	gpio_set_value(GPIO_TOUCH_RST, 0);
	dbg("TOUCH_RST High.\n");
	msleep(50);

	gpio_set_value(GPIO_TOUCH_RST, 1);
	dbg("TOUCH_RST Low.\n");
	msleep(10);
#endif

}

#ifdef QT_FIRMUP_ENABLE

uint8_t boot_unlock(void)
{
	int ret;
	unsigned char data[2];
	uint8_t rc;

	dbg_func_in();

	//   read_buf = (char *)kmalloc(size, GFP_KERNEL | GFP_ATOMIC);
	data[0] = 0xDC;
	data[1] = 0xAA;

	ret = boot_qt602240_i2c_write(0,data,2);
	if(ret < 0) {
		dbg("%s : i2c write failed\n",__func__);
		rc = WRITE_MEM_FAILED;
	}
	else
	{
		rc = WRITE_MEM_OK;
	}

	dbg_func_out();

	return rc;
}

uint8_t QT_Boot(bool withReset)
{
	unsigned char	boot_status;
	unsigned char	retry_cnt, retry_cnt_max;
	unsigned long int	character_position = 0;
	unsigned int	frame_size = 0;
	unsigned int	next_frame = 0;
	unsigned int	crc_error_count = 0;
	unsigned int	size1,size2;
	uint8_t			data = 0xA5;
	uint8_t			reset_result = 0;
	unsigned char	*firmware_data;
       
	firmware_data = QT602240_firmware;

	dbg_func_in();

	if(withReset)
	{
		retry_cnt_max = 10;
		reset_result = write_mem(command_processor_address + RESET_OFFSET, 1, &data);

		if(reset_result != WRITE_MEM_OK)
		{
			for(retry_cnt =0; retry_cnt < 3; retry_cnt++)
			{
				msleep(100);
				reset_result = write_mem(command_processor_address + RESET_OFFSET, 1, &data);
				if(reset_result == WRITE_MEM_OK)
				{
					dbg("write_mem(RESET_OFFSET) : fail.\n");
					break;
				}
			}
			dbg("write_mem(RESET_OFFSET) : fail.\n");
		}
		else
		{
			dbg("write_mem(RESET_OFFSET) : fail.\n");
		}

		msleep(100);
	}
	else
	{
		retry_cnt_max = 30;
	}

	for(retry_cnt = 0; retry_cnt < retry_cnt_max; retry_cnt++)
	{
		if(boot_read_mem(0,1,&boot_status) == READ_MEM_OK)
		{
			retry_cnt = 0;
			dbg("TSP boot status is %x stage 2 \n", boot_status);

			if((boot_status & QT_WAITING_BOOTLOAD_COMMAND) == QT_WAITING_BOOTLOAD_COMMAND)
			{
				if(boot_unlock() == WRITE_MEM_OK)
				{
					msleep(10);
					dbg("Unlock OK\n");
				}
				else
				{
					dbg("Unlock fail\n");
				}
			}
			else if((boot_status & 0xC0) == QT_WAITING_FRAME_DATA)
			{
				/* Add 2 to frame size, as the CRC bytes are not included */
				size1 =  *(firmware_data+character_position);
				size2 =  *(firmware_data+character_position+1)+2;
				frame_size = (size1<<8) + size2;

				dbg("Frame size:%d\n", frame_size);
				dbg("Firmware pos:%d\n", (int)character_position);
				/* Exit if frame data size is zero */
				if( 0 == (int)frame_size )
				{
					printk("[QT602240]0 == frame_size\n");
					return 1;
				}
				next_frame = 1;
				boot_write_mem(0,frame_size, (firmware_data +character_position));
				msleep(10);
				dbg(".");

			}
			else if(boot_status == QT_FRAME_CRC_CHECK)
			{
				dbg("CRC Check\n");
			}
			else if(boot_status == QT_FRAME_CRC_PASS)
			{
				if( next_frame == 1)
				{
					dbg("CRC Ok\n");
					character_position += frame_size;
					next_frame = 0;
				}
				else
				{
					dbg("next_frame != 1\n");
				}
			}
			else if(boot_status  == QT_FRAME_CRC_FAIL)
			{
				dbg("CRC Fail\n");
				crc_error_count++;
			}
			if(crc_error_count > 10)
			{
				return QT_FRAME_CRC_FAIL;
			}
		}
	}
	dbg_func_out();
	return (0);
}

/* qt602240 chipset version check */
void QT_reprogram(void)
{
	uint8_t version = 0, build = 0;
	uint8_t status = 0;
	unsigned char rxdata = 0;

	dbg_func_in();

	if(boot_read_mem(0,1,&rxdata) == READ_MEM_OK)
	{
		dbg("Enter to new firmware : boot mode\n");
		if(QT_Boot(0)) {
			TSP_Restart();  //cte mode == 0
			quantum_touch_probe(); //cte mode ==3 
			TSP_Restart();
		}
		quantum_touch_probe(); //cte mode ==3 
		TSP_Restart();	
		dbg("Reprogram done : boot mode\n");       
	}

	/* find and initialise QT device */
	quantum_touch_probe();

	if (driver_setup != DRIVER_SETUP_OK)
		return;

	status = read_mem(3, 1, (void *) &build);
	if (status != READ_MEM_OK) printk("[TSP]ERROR!");
	dbg("familybuild_id = 	0x%x\n",build);

	status = read_mem(2, 1, (void *) &version);
	if (status != READ_MEM_OK) printk("[TSP]ERROR!");
	dbg("version = 		0x%x\n",version);

	// mXT224E	
	if(1)
	{
	}
	// mXT224
	else
	{
		printk("[TSP]ERROR!!! : (%d) %s\n", __LINE__, __func__);
		if((version < 0x20)||((version == 0x20)&&(build != 0xAA)))
		{
			dbg("Enter to new firmware : ADDR = Other Version\n");
			if(QT_Boot(1)) {
				TSP_Restart();
				quantum_touch_probe();
				TSP_Restart(); 
			}
			quantum_touch_probe();
			TSP_Restart(); 
			dbg("Reprogram done : ADDR = Other Version\n");
		}
	}

	dbg_func_out();
}
#endif

static ssize_t setup_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	dbg_func_in();
	dbg_func_out();
	return 0;
}

static ssize_t setup_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	dbg_func_in();
	dbg_func_out();
	return size;
}

int __init qt602240_init(void)
{
	int rc;

	dbg_func_in();
	rc = init_hw_setting();
	if(rc<0)
	{
		printk("[QT602240]init_hw_setting failed. (rc=%d)\n", rc);
		return rc;
	}
	dbg("i2c_add_driver\n");
	rc = i2c_add_driver(&qt602240_driver);
	if(rc)
	{
		printk("[QT602240]i2c_add_driver failed. (rc=%d)\n", rc);
	}

	dbg_func_out();
	return rc;
}

void __exit qt602240_exit(void)
{
	dbg_func_in();
	
	i2c_del_driver(&qt602240_driver);
	dbg_func_out();
	return;
}

late_initcall(qt602240_init);
module_exit(qt602240_exit);

MODULE_DESCRIPTION("ATMEL qt602240 Touchscreen Driver");
MODULE_LICENSE("GPL");

