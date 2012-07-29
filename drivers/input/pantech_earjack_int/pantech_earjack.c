/*
 *
 *  Pantech Earjack Driver
 *
 *  Copyright (C) 2011 Pantech, Inc.
 *
 *  Author: Sangwoo Kim, Yoonkoo Kang
 *
 */



#include <linux/slab.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/input.h>
#include <linux/switch.h>
#include <mach/mpp.h>                                

#include <linux/gpio.h>
#include <linux/interrupt.h>

#include <linux/regulator/consumer.h>                   
#include <linux/regulator/pmic8058-regulator.h>         
#include <linux/pmic8058-xoadc.h>

#include <linux/workqueue.h>
#include <linux/errno.h>
#include <linux/msm_adc.h>

#include <linux/spinlock.h>
#include <linux/wakelock.h>

// Metrico Temp Feature
//#if defined(CONFIG_MACH_MSM8X60_PRESTO) && (BOARD_REV < TP10)
#if defined(CONFIG_MACH_MSM8X60_PRESTO) && (BOARD_REV <= TP10)
#define CONFIG_MIC_BIAS_1_8V
#endif

/* -------------------------------------------------------------------- */
/* debug option */
/* -------------------------------------------------------------------- */
#define EARJACK_DBG
//#define EARJACK_FUNC_DBG

#ifdef EARJACK_DBG
#define dbg(fmt, args...)   printk("[earjack] " fmt, ##args)
#define dbg_without_label(fmt, args...)   printk(fmt, ##args)
#else
#define dbg(fmt, args...)
#endif

#ifdef EARJACK_FUNC_DBG
#define dbg_func_in()       dbg("++ %s\n", __func__)
#define dbg_func_out()      dbg("-- %s\n", __func__)
#define dbg_line()          dbg("line:%d(%s)\n", __LINE__, __func__)
#else
#define dbg_func_in()  
#define dbg_func_out() 
#define dbg_line()     
#endif
/* -------------------------------------------------------------------- */

#define DRIVER_NAME	"pantech_earjack"

#define EARJACK_DET     125 	/* Earjack sensing interrupt pin No. */
#define REMOTEKEY_DET   215     /* IRQ's:173 + PM GPIOs:40 + MPP No.:3 + INDEX:-1 = 215 */

#define 	BIT_HEADSET 					1
#define	BIT_HEADSET_SPEAKER_ONLY 	2
#define	BIT_HEADSET_MIC_ONLY 			4
#if (defined(CONFIG_MACH_MSM8X60_EF39S) && (BOARD_REV <= WS10)) || \
    (defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV <= PT20))
#define EARJACK_DET_ACTIVE_LOW // select if earjack_det pin is active low
#elif defined(CONFIG_MACH_MSM8X60_PRESTO)
#endif

#define REMOTEKEY_DET_ACTIVE_LOW // select if remoteky_det pin is active low

#ifdef EARJACK_DET_ACTIVE_LOW
#define EARJACK_INSERTED !gpio_get_value_cansleep(EARJACK_DET)
#else /*EARJACK_ACTIVE_HIGH*/
#define EARJACK_INSERTED  gpio_get_value_cansleep(EARJACK_DET)
#endif /*EARJACK_DET_ACTIVE_LOW*/
#define EARJACK_RELEASED !EARJACK_INSERTED

#ifdef REMOTEKEY_DET_ACTIVE_LOW 
#define REMOTEKEY_PRESSED !gpio_get_value_cansleep(REMOTEKEY_DET)
#else
#define REMOTEKEY_PRESSED  gpio_get_value_cansleep(REMOTEKEY_DET)
#endif
#define REMOTEKEY_RELEASED !REMOTEKEY_PRESSED

#define  ARR_SIZE( a )  ( sizeof( (a) ) / sizeof( (a[0]) ) )

typedef enum{
	EARJACK_STATE_OFF,
	EARJACK_STATE_ON_3POLE_CHECK,
	EARJACK_STATE_ON,
	EARJACK_STATE_CHECK,
}earjack_type;

#if defined(CONFIG_MIC_BIAS_1_8V)
#else
static struct regulator *hs_jack_l8;
#endif// defined(CONFIG_MACH_MSM8X60_PRESTO)

//static struct regulator *hs_jack_s3;

// Pantech Earjack Structure
struct pantech_earjack {
	struct input_dev *ipdev;
    	struct switch_dev sdev;
	earjack_type type;
	bool mic_on;
	bool hs_on;
	bool remotekey_pressed;
	int remotekey_index;
	bool car_kit;
};
static struct pantech_earjack *earjack;

// Interrupt Handlers
static irqreturn_t Earjack_Det_handler(int irq, void *dev_id);
static irqreturn_t Remotekey_Det_handler(int irq, void *dev_id);

// Locks
static struct wake_lock earjack_wake_lock;
static struct wake_lock remotekey_wake_lock;

// Remote Key 
typedef struct{
	char* key_name;
	int key_index;
	int min;
	int max;
} remotekey;


#if defined(CONFIG_MIC_BIAS_1_8V)
// PRESTO : 1.8V Mic bias Voltage
static remotekey remotekey_type[] ={// reffer to EF18
	{ "NO_KEY",		0,			2400,	2600},
	{ "KEY_MEDIA",		KEY_MEDIA,		60-30,	60+30},
	{ "KEY_VOLUMEDOWN",	KEY_VOLUMEDOWN,		400-40,	400+40},
	{ "KEY_VOLUMEUP",	KEY_VOLUMEUP,		220-22,	220+22},
	{ "KEY_MEDIA_CARKIT",	KEY_MEDIA,		0,	0+20}, // car_kit - should be on the last position 
};
#else
// Default : 2.7V
static remotekey remotekey_type[] ={ 
	{ "NO_KEY",		0,			2400,	2600},
	{ "KEY_MEDIA",		KEY_MEDIA,		60,	130},
	{ "KEY_VOLUMEDOWN",	KEY_VOLUMEDOWN,		500,	700},
	{ "KEY_VOLUMEUP",	KEY_VOLUMEUP,		250,	420},
	{ "KEY_MEDIA_CARKIT",	KEY_MEDIA,		0,	50}, // car_kit - should be on the last position 
};
#endif

static struct delayed_work earjack_work;
static struct delayed_work remotekey_work;

static void earjack_detect_func(struct work_struct * earjack_work);
static void remotekey_detect_func(struct work_struct * remotekey_work);

// Switch Device Functions
static ssize_t msm_headset_print_name(struct switch_dev *sdev, char *buf)
{
	switch (switch_get_state(&earjack->sdev)) {
	case 0: //NO_DEVICE:
		return sprintf(buf, "No Device\n");
	case 1: // MSM_HEADSET:
		return sprintf(buf, "Headset\n");
  default : //  MSM_HEADSET for 3pole earjack //kyk
    return sprintf(buf, "Headset\n");
	}
	return -EINVAL;
}

int switch_state(void)
{
	int state;
	if (earjack->mic_on && earjack->hs_on)
		state = BIT_HEADSET;
	else if (earjack->hs_on)
		state = BIT_HEADSET_SPEAKER_ONLY;
	else if (earjack->mic_on)
		state = BIT_HEADSET_MIC_ONLY;
	else
		state = 0;

	return state;
}

// Sysfs 
static ssize_t show_headset(struct device *dev, struct device_attribute *attr
, char *buf)
{
	int conn_headset_type = switch_state();
	return snprintf(buf, PAGE_SIZE, "%d\n", conn_headset_type);
}

static ssize_t set_headset(struct device *dev, struct device_attribute *attr, 
const char *buf, size_t count)
{
    return 0;
}

static DEVICE_ATTR(headset, S_IRUGO | S_IWUSR, show_headset, set_headset);

static struct attribute *dev_attrs[] = {
    &dev_attr_headset.attr,
    NULL,
};

static struct attribute_group dev_attr_grp = {
    .attrs = dev_attrs,
};


// Pantech Earjack Probe Function
static int __devinit pantech_earjack_probe(struct platform_device *pdev)
{
	int rc = 0;
	int err = 0;
	struct input_dev *ipdev;

	dbg_func_in();

	// Alloc Devices
	earjack = kzalloc(sizeof(struct pantech_earjack), GFP_KERNEL);
	if (!earjack)
		return -ENOMEM;

	earjack->sdev.name	= "h2w";     
	earjack->sdev.print_name = msm_headset_print_name;
	rc = switch_dev_register(&earjack->sdev);
	if (rc)
		goto err_switch_dev_register;

	ipdev = input_allocate_device();
	if (!ipdev) {
		rc = -ENOMEM;
		goto err_alloc_input_dev;
	}
	input_set_drvdata(ipdev, earjack);


	// Init Status Flags
	earjack->ipdev = ipdev;
	earjack->car_kit = 0;
	earjack->type=EARJACK_STATE_OFF;
	earjack->remotekey_pressed = 0;
	earjack->remotekey_index = 0;

	// Initialize Work Queue
	INIT_DELAYED_WORK(&earjack_work,earjack_detect_func);          // INIT WORK
	INIT_DELAYED_WORK(&remotekey_work,remotekey_detect_func);

	// Get Power Source
#if defined(CONFIG_MIC_BIAS_1_8V)
	err = 0;
#else
	hs_jack_l8 = regulator_get(NULL, "8058_l8");
	regulator_set_voltage(hs_jack_l8,2700000,2700000);

	dbg("regulator_enable hs_jack_l8 value => %d\n",err);
#endif

	// Initialize Wakelocks
	wake_lock_init(&earjack_wake_lock, WAKE_LOCK_SUSPEND, "earjack_wake_lock_init");
	wake_lock_init(&remotekey_wake_lock, WAKE_LOCK_SUSPEND, "remotekey_wake_lock_init");

	// Setup GPIO's
	gpio_request(EARJACK_DET, "earjack_det");
	gpio_request(REMOTEKEY_DET, "remotekey_det");
	gpio_tlmm_config(GPIO_CFG(EARJACK_DET, 0, GPIO_CFG_INPUT,GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	rc = request_irq(gpio_to_irq(EARJACK_DET), Earjack_Det_handler, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "earjack_det-irq", earjack);

	// Warning: REMOTEKEY_DET using default gpio config.
	//gpio_tlmm_config(GPIO_CFG(REMOTEKEY_DET, 0, GPIO_CFG_INPUT,GPIO_CFG_NO_PULL, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	
	set_irq_wake(gpio_to_irq(EARJACK_DET), 1);
	set_irq_wake(gpio_to_irq(REMOTEKEY_DET), 1);

	// Init Input Device
	ipdev->name	= DRIVER_NAME;
	ipdev->id.vendor    = 0x0001;
	ipdev->id.product   = 1;
	ipdev->id.version   = 1;

	input_set_capability(ipdev, EV_KEY, KEY_MEDIA);
	input_set_capability(ipdev, EV_KEY, KEY_VOLUMEUP);
	input_set_capability(ipdev, EV_KEY, KEY_VOLUMEDOWN);
	input_set_capability(ipdev, EV_KEY, KEY_POWER);    
	input_set_capability(ipdev, EV_KEY, KEY_END);
	input_set_capability(ipdev, EV_SW,  SW_HEADPHONE_INSERT);
	input_set_capability(ipdev, EV_SW,  SW_MICROPHONE_INSERT);

	rc = input_register_device(ipdev);
	if (rc) {
		dev_err(&ipdev->dev,
				"hs_probe: input_register_device rc=%d\n", rc);
		goto err_reg_input_dev;
	}
	platform_set_drvdata(pdev, earjack);

	rc = sysfs_create_group(&pdev->dev.kobj, &dev_attr_grp);
	if (rc) {
		dev_err(&ipdev->dev,
				"hs_probe: sysfs_create_group rc=%d\n", rc);
		goto err_earjack_init;
	}

	// Scehdule earjack_detect_func for initial detect
	disable_irq_nosync(gpio_to_irq(EARJACK_DET));
	wake_lock(&earjack_wake_lock);
	disable_irq_nosync(gpio_to_irq(REMOTEKEY_DET));

	schedule_delayed_work(&earjack_work,10);    // after 100ms
	
	dbg_func_out();
	return 0;

err_earjack_init:

err_reg_input_dev:
	input_unregister_device(ipdev);
	ipdev = NULL;
err_alloc_input_dev:
	input_free_device(ipdev);
err_switch_dev_register:
	kfree(earjack);
	return 0;
}

bool is_4pole_earjack (void) 
{
	bool retVal;
	int err;
	if (gpio_cansleep(REMOTEKEY_DET)){
		err=gpio_get_value_cansleep(REMOTEKEY_DET);
		dbg("gpio_get_value_cansleep(REMOTEKEY_DET) start\n");                    
	}else{
		err=gpio_get_value(REMOTEKEY_DET);
		dbg("gpio_get_value(REMOTEKEY_DET) start\n");
	}
	retVal = (err>0) ? 1:0;
	return retVal;
}

static irqreturn_t Earjack_Det_handler(int irq, void *dev_id)
{
	dbg_func_in();
	disable_irq_nosync(gpio_to_irq(EARJACK_DET));
	wake_lock(&earjack_wake_lock);
	schedule_delayed_work(&earjack_work, 10);    // after 100ms start function of earjack_detect_func
	dbg_func_out();
	return IRQ_HANDLED;
}

static void earjack_detect_func( struct work_struct *test_earjack)         
{
	int err;
	dbg_func_in();
	dbg("currnet earjack->type: ");
	switch(earjack->type){
		case    EARJACK_STATE_OFF : 
			{
				dbg_without_label("EARJACK_STATE_OFF\n");
				if(EARJACK_INSERTED){
					earjack->type = EARJACK_STATE_CHECK;
#if defined(CONFIG_MIC_BIAS_1_8V)
#else
					err=regulator_is_enabled(hs_jack_l8);
					if(err<=0)  err = regulator_enable(hs_jack_l8);
#endif //defined(CONFIG_MACH_MSM8X60_PRESTO)
					pm8058_mpp_config_digital_in(XOADC_MPP_3,PM8058_MPP_DIG_LEVEL_S3, PM_MPP_DIN_TO_INT);
					schedule_delayed_work(&earjack_work, 5); //50ms
					// return without enable IRQ, wake_unlock.
					return;
				}
				break;
			}
		case    EARJACK_STATE_CHECK  :   
			{
				dbg_without_label("EARJACK_STATE_CHECK\n");   
				if(EARJACK_INSERTED)
				{
					// 3pole
					if(!is_4pole_earjack()){
						dbg("3pole headset inserted.\n");
						earjack->type= EARJACK_STATE_ON_3POLE_CHECK;
						earjack->mic_on = 0;
						earjack->hs_on = 1;
						input_report_switch(earjack->ipdev, SW_HEADPHONE_INSERT,earjack->hs_on);
						switch_set_state(&earjack->sdev, switch_state());
						schedule_delayed_work(&earjack_work, 60);   // check if 4pole 600ms
						// return without enable IRQ, wake_unlock.
						return;
					}
					//4pole
					else {
						dbg("4pole headset inserted.\n");
						earjack->type= EARJACK_STATE_ON;
						err=request_threaded_irq(gpio_to_irq(REMOTEKEY_DET),NULL,Remotekey_Det_handler,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "remote_det-irq", earjack);
						if(err) 
							dbg("request_threaded_irq failed\n");
						earjack->mic_on = 1;
						earjack->hs_on = 1;
						input_report_switch(earjack->ipdev, SW_MICROPHONE_INSERT,earjack->mic_on); //TODO: NO USE?
						input_report_switch(earjack->ipdev, SW_HEADPHONE_INSERT,earjack->hs_on);
						switch_set_state(&earjack->sdev, switch_state());
					}
				}
				else // if EARJACK_RELEASED
				{
					earjack->type = EARJACK_STATE_OFF;                
					dbg("earjack_type: -> EARJACK_STATE_OFF");
				}

				break; // case EARJACK_STATE_CHECK 
			}

		case    EARJACK_STATE_ON_3POLE_CHECK  :   
			{                        
				// CHECKING IF 4POLE EARJACK IS INSERTIND?
				dbg_without_label("EARJACK_STATE_ON_3POLE_CHECK\n");   
				if(EARJACK_INSERTED){
					earjack->type= EARJACK_STATE_ON;                   
					if(!is_4pole_earjack()){
						dbg("3pole earjack insert.\n");
#if defined(CONFIG_MIC_BIAS_1_8V)
#else
            err=regulator_is_enabled(hs_jack_l8);
						dbg("regulator_is_enabled(hs_jack_l8) value => %d\n",err);
						if(err>0) regulator_disable(hs_jack_l8);
#endif //defined(CONFIG_MACH_MSM8X60_PRESTO)
					}
					else {
						dbg("4pole earjack insert.\n");
						earjack->mic_on =1;
						input_report_switch(earjack->ipdev, SW_MICROPHONE_INSERT,earjack->mic_on);
    						switch_set_state(&earjack->sdev, switch_state());
						err=request_threaded_irq(gpio_to_irq(REMOTEKEY_DET),NULL,Remotekey_Det_handler,IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "remote_det-irq", earjack);
						if(err) dbg("request_threaded_irq failed\n");
					}                 
				}
				else{
#if defined(CONFIG_MIC_BIAS_1_8V)
#else          
					err=regulator_is_enabled(hs_jack_l8);
					dbg("regulator_is_enabled(hs_jack_l8) value => %d\n",err);
					if(err>0) regulator_disable(hs_jack_l8);
#endif// defined(CONFIG_MACH_MSM8X60_PRESTO)
					earjack->type = EARJACK_STATE_OFF; 
					earjack->hs_on=0;
					input_report_switch(earjack->ipdev, SW_HEADPHONE_INSERT,earjack->hs_on);
    					switch_set_state(&earjack->sdev, switch_state());
				}
				break;   
			}

		case EARJACK_STATE_ON:   
			{
				dbg_without_label("EARJACK_STATE_ON\n");
				if(EARJACK_RELEASED){
					earjack->type = EARJACK_STATE_OFF;
					// if 4pole
					if (earjack->mic_on) {
						earjack->mic_on = 0;
						input_report_switch(earjack->ipdev, SW_MICROPHONE_INSERT,earjack->mic_on);
						// free remote key irq and turn off mic power.
						free_irq(gpio_to_irq(REMOTEKEY_DET), earjack);
#if defined(CONFIG_MIC_BIAS_1_8V)
#else
            err=regulator_is_enabled(hs_jack_l8);
						dbg("regulator_is_enabled(hs_jack_l8) value => %d\n",err);
						if(err>0){
							regulator_disable(hs_jack_l8);
						}
#endif //defined(CONFIG_MACH_MSM8X60_PRESTO)

						// release remote key if pressed	
						if(earjack->remotekey_pressed){
							earjack->remotekey_pressed = 0;
							if (earjack->remotekey_index != 0) 
								input_report_key(earjack->ipdev, remotekey_type[earjack->remotekey_index].key_index, earjack->remotekey_pressed);
							dbg("remote key: %s : %d->%d \n", remotekey_type[earjack->remotekey_index].key_name, !earjack->remotekey_pressed, earjack->remotekey_pressed);
							input_sync(earjack->ipdev);
						}                            
						dbg("earjack_release \n");

					}
					earjack->hs_on = 0;
					input_report_switch(earjack->ipdev, SW_HEADPHONE_INSERT,earjack->hs_on);
    					switch_set_state(&earjack->sdev, switch_state());
				}

				break;
			}                    
		default :   
			dbg("earjack_detect_func default.\n");
	}
	enable_irq(gpio_to_irq(EARJACK_DET));
	wake_unlock(&earjack_wake_lock);
	dbg_func_out();
	return;
}


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


static int adc_value_to_key(void)  // detect key sensing                                 
{
	int nAdcValue = 0;
	int i;
	dbg_func_in();
	check_analog_mpp(CHANNEL_ADC_HDSET,&nAdcValue);             // read analog input mpp_3    
	dbg("analog intput mpp_03 value : %d\n", nAdcValue);
	for( i = 1; i< ARR_SIZE( remotekey_type); i++ ) {
		if( nAdcValue >= remotekey_type[i].min && nAdcValue <= remotekey_type[i].max ) {
			// Check if car_kit
			earjack->car_kit = 0;
			if(i == ARR_SIZE(remotekey_type)-1 )
				earjack->car_kit = 1;
			// Return key index
			dbg_func_out();  	
			return i;
		}
	}
	// No Key, return 0
	dbg_func_out();  	
	return 0;
}

static irqreturn_t Remotekey_Det_handler(int irq, void *dev_id)                 // isr_Remotekey_Det_handler
{
	dbg_func_in();
	// if 3pole headset, return. 
	if(!earjack->mic_on) {
		return IRQ_HANDLED;
	}
	// disable_irq, wake_lock
	disable_irq_nosync(gpio_to_irq(REMOTEKEY_DET));
	wake_lock(&remotekey_wake_lock);
	schedule_delayed_work(&remotekey_work, 0);
	dbg_func_out();
	return IRQ_HANDLED;        
}

static void remotekey_detect_func(struct work_struct * test_remotekey_work)
{
	int key_first=0;
	int key_second=0;    
	//	int key_third=0;    

	dbg_func_in();
	// car kit
	if(earjack->car_kit ){
		if(EARJACK_INSERTED){
			dbg("report KEY_MEDIA input from car_kit");
			input_report_key(earjack->ipdev, KEY_MEDIA, earjack->remotekey_pressed);		// car kit button down
			earjack->car_kit = 0; // reset car_kit flag
			if(REMOTEKEY_RELEASED) {
				earjack->remotekey_pressed = 0;
				input_report_key(earjack->ipdev, KEY_MEDIA, earjack->remotekey_pressed);	// car kit button up
			}
		}		
		earjack->car_kit = 0; // reset car_kit flag.
		enable_irq(gpio_to_irq(REMOTEKEY_DET));
		wake_unlock(&remotekey_wake_lock);
		dbg_func_out();
		return ;
	}
	// if current state: key not pressed
	if(!earjack->remotekey_pressed){ 
		dbg("current state: remotekey not pressed, read adc value. \n");
		pm8058_mpp_config_analog_input(XOADC_MPP_3,PM_MPP_AIN_AMUX_CH5, PM_MPP_AOUT_CTL_DISABLE);
		// read adc value twice 
		key_first=adc_value_to_key();
		key_second=adc_value_to_key(); // after 20ms (adc reading delay)
		pm8058_mpp_config_digital_in(XOADC_MPP_3,PM8058_MPP_DIG_LEVEL_S3, PM_MPP_DIN_TO_INT);

		// is valid key && earjack inserted 
		if( (key_first==key_second) && EARJACK_INSERTED){
			earjack->remotekey_index = key_first;
			earjack->remotekey_pressed = 1; 
			if (earjack->remotekey_index != 0 && !earjack->car_kit)
			{
				input_report_key(earjack->ipdev, remotekey_type[earjack->remotekey_index].key_index, earjack->remotekey_pressed);
				dbg("remote key: %s : %d->%d \n", remotekey_type[earjack->remotekey_index].key_name, !earjack->remotekey_pressed, earjack->remotekey_pressed);
			}	
			else if (earjack->car_kit)
			{
				schedule_delayed_work(&remotekey_work, 20);	// for car kit : delayed work after 200ms for __  TODO
				return;
			}
			
			// Defense code :key pressed but released during processing key. 
			if(REMOTEKEY_RELEASED){
				earjack->remotekey_pressed=0;
				if (earjack->remotekey_index != 0) 
					input_report_key(earjack->ipdev, remotekey_type[earjack->remotekey_index].key_index, earjack->remotekey_pressed);
				dbg("remote key: %s : %d->%d \n", remotekey_type[earjack->remotekey_index].key_name, !earjack->remotekey_pressed, earjack->remotekey_pressed);
			}

		}
		// else, ignore key
		else {
			dbg("igrnore key.\n");
			enable_irq(gpio_to_irq(REMOTEKEY_DET));
			wake_unlock(&remotekey_wake_lock);
			dbg_func_out();
			return ;
		}	
	}
	// if current state : key pressed
	else
	{ 
		earjack->remotekey_pressed=0;
		if (earjack->remotekey_index != 0) 
			input_report_key(earjack->ipdev, remotekey_type[earjack->remotekey_index].key_index, earjack->remotekey_pressed);
		dbg("remote key: %s : %d->%d \n", remotekey_type[earjack->remotekey_index].key_name, !earjack->remotekey_pressed, earjack->remotekey_pressed);
	}        
	input_sync(earjack->ipdev);
	enable_irq(gpio_to_irq(REMOTEKEY_DET));
	wake_unlock(&remotekey_wake_lock);
	dbg_func_out();
	return;
}

// Remove Driver
static int __devexit pantech_earjack_remove(struct platform_device *pdev)
{
	struct pantech_earjack *earjack = platform_get_drvdata(pdev);

	dbg_func_in();
	input_unregister_device(earjack->ipdev);
	switch_dev_unregister(&earjack->sdev);
	kfree(earjack);
	wake_lock_destroy(&earjack_wake_lock);
	wake_lock_destroy(&remotekey_wake_lock);
	dbg_func_out();
	return 0;
}

// Suspend, Resume
static int pantech_earjack_suspend(struct platform_device * pdev, pm_message_t state)
{
	dbg_func_in();
	return 0;
}
static int pantech_earjack_resume(struct platform_device * pdev)
{
	dbg_func_in();
	return 0;
}

// Driver Structure
static struct platform_driver pantech_earjack_driver = {
	.probe	= pantech_earjack_probe,
	.remove	= pantech_earjack_remove,
	.driver = {
		.name = DRIVER_NAME,
		.owner = THIS_MODULE,
	},
	.resume = pantech_earjack_resume,
	.suspend = pantech_earjack_suspend,
};

// Module Init
static int __init pantech_earjack_init(void)
{
	printk("[earjack] pantech_earjack module init.\n");
	return platform_driver_register(&pantech_earjack_driver);
}
late_initcall(pantech_earjack_init);

// Module Exit 
static void __exit pantech_earjack_exit(void)
{
	printk("[earjack] pantech_earjack module exit.\n");
	platform_driver_unregister(&pantech_earjack_driver);
}
module_exit(pantech_earjack_exit);

MODULE_LICENSE("GPL v2");
MODULE_ALIAS("platform:pantech_earjack");
