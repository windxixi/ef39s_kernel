/*
 * Copyright (c) 2010 Pantech Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/i2c.h>
#include <linux/mutex.h>
#include <linux/delay.h>
#include <mach/vreg.h>
#include <linux/gpio.h>
#include <linux/delay.h>
#include <linux/wakelock.h>
#include <linux/syscalls.h>
#include <linux/delay.h>
#include <mach/gpio.h>
#include "apds9900.h"

#define APDS9900_DRV_NAME	"apds9900"
#define DRIVER_VERSION		"1.1.0"	// for INVENSENSE

/* -------------------------------------------------------------------- */
/* debug option */
/* -------------------------------------------------------------------- */
#define SENSOR_APDS9900_DBG_ENABLE
#ifdef SENSOR_APDS9900_DBG_ENABLE
#define dbg(fmt, args...)   pr_debug("[APDS9900] " fmt, ##args)
#else
#define dbg(fmt, args...)
#endif
#define dbg_func_in()       dbg("[FUNC_IN] %s\n", __func__)
#define dbg_func_out()      dbg("[FUNC_OUT] %s\n", __func__)
#define dbg_line()          dbg("[LINE] %d(%s)\n", __LINE__, __func__)
/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* SKY BOARD FEATURE */
/* -------------------------------------------------------------------- */
#if defined(CONFIG_MACH_MSM8X60_EF39S) || defined(CONFIG_MACH_MSM8X60_EF40K)  // 20110429, FEATURE-WORK
#define APDS9900_PS_INT_N				34
#endif

#define APDS9900_PS_IRQ							gpio_to_irq(APDS9900_PS_INT_N)
#if defined(CONFIG_MACH_MSM8X60_EF39S) || defined(CONFIG_MACH_MSM8X60_EF40K)
#define APDS9900_THRESHOLD_HIGH			30
#define APDS9900_THRESHOLD_LOW			15
#endif

#define APDS9900_ALSTIME						0xDB // 100ms
#define APDS9900_ALSTIME_MS						100 // ms
#define APDS9900_PENDING_WAKELOCK
#define APDS9900_SKY_FITNESS_ENABLE
//#define APDS9900_ALWAYSON_ENABLE
//#define APDS9900_WAKELOCK_ENABLE
//#define APDS9900_PS_MEASURE_POLL_ENABLE

// ALS Calculation Lux
#define ALS_COEF_UNIT 100
#define ALS_COEF_DF 52
#if defined(CONFIG_MACH_MSM8X60_EF39S) || defined(CONFIG_MACH_MSM8X60_EF40K)
#define ALS_COEF_GA 85 // EF33-110421
#else
#define ALS_COEF_GA 72 
#endif
#define ALS_COEF_B 223
#define ALS_COEF_C 70
#define ALS_COEF_D 142
#define ALS_COEF_MAX_CDATA ((75*(1024*(0x0100-APDS9900_ALSTIME)))/100) // == 28416


// Sunlight Cancelation
#define APDS9900_SUNLIGHT_CANCELATION // P12095. 110419 sunlight cancelation
#ifdef APDS9900_SUNLIGHT_CANCELATION
#define CANCELATION_PSDATA 63
#endif
/* -------------------------------------------------------------------- */
/* ERROR FLAG */
/* -------------------------------------------------------------------- */
#define SH_FLAG_SUCCESS				0x00000000
#define SH_FLAG_NO_DEVICE			0x00000000
#define SH_FLAG_ERR_POWER			0x00000002
#define SH_FLAG_ERR_I2C				0x00000004

/* -------------------------------------------------------------------- */
/* APDS9900 PROTOCOL */
/* -------------------------------------------------------------------- */
#define APDS9900_CHIP_ID			0x29
#define APDS9900_REG_BASE			0x80
#define APDS9900_ENABLE_REG		(APDS9900_REG_BASE+0x00)
#define APDS9900_ATIME_REG		(APDS9900_REG_BASE+0x01)
#define APDS9900_PTIME_REG		(APDS9900_REG_BASE+0x02)
#define APDS9900_WTIME_REG		(APDS9900_REG_BASE+0x03)
#define APDS9900_AILTL_REG		(APDS9900_REG_BASE+0x04)
#define APDS9900_AILTH_REG		(APDS9900_REG_BASE+0x05)
#define APDS9900_AIHTL_REG		(APDS9900_REG_BASE+0x06)
#define APDS9900_AIHTH_REG		(APDS9900_REG_BASE+0x07)
#define APDS9900_PILTL_REG		(APDS9900_REG_BASE+0x08)
#define APDS9900_PILTH_REG		(APDS9900_REG_BASE+0x09)
#define APDS9900_PIHTL_REG		(APDS9900_REG_BASE+0x0A)
#define APDS9900_PIHTH_REG		(APDS9900_REG_BASE+0x0B)
#define APDS9900_PERS_REG			(APDS9900_REG_BASE+0x0C)
#define APDS9900_CONFIG_REG		(APDS9900_REG_BASE+0x0D)
#define APDS9900_PPCOUNT_REG	(APDS9900_REG_BASE+0x0E)
#define APDS9900_CONTROL_REG	(APDS9900_REG_BASE+0x0F)
#define APDS9900_REV_REG			(APDS9900_REG_BASE+0x11)
#define APDS9900_ID_REG				(APDS9900_REG_BASE+0x12)
#define APDS9900_STATUS_REG		(APDS9900_REG_BASE+0x13)
#define APDS9900_CDATAL_REG		(APDS9900_REG_BASE+0x14)
#define APDS9900_CDATAH_REG		(APDS9900_REG_BASE+0x15)
#define APDS9900_IRDATAL_REG	(APDS9900_REG_BASE+0x16)
#define APDS9900_IRDATAH_REG	(APDS9900_REG_BASE+0x17)
#define APDS9900_PDATAL_REG		(APDS9900_REG_BASE+0x18)
#define APDS9900_PDATAH_REG		(APDS9900_REG_BASE+0x19)
#define CMD_CLR_PS_INT				0xE5

#define apds9900_set_near2far()	apds9900_set_thrconfig(0x03FF, APDS9900_THRESHOLD_LOW<<4)
#define apds9900_set_far2near()	apds9900_set_thrconfig(APDS9900_THRESHOLD_HIGH<<4, 0x0000)

/* -------------------------------------------------------------------- */
/* Structure */
/* -------------------------------------------------------------------- */

typedef struct {
	u8 enable;
	u8 atime;
	u8 ptime;
	u8 wtime;
	u8 ppcount;
	u8 control;
	u8 interrupt;
	u8 piltl;
	u8 pilth;
	u8 pihtl;
	u8 pihth;
	u8 pers;
} register_t;

typedef struct {
	struct i2c_client	*client;
	struct mutex		i2clock;
	
	bool				isNear2Far;
	struct mutex		psvallock;
	struct mutex		lsvallock;
	struct mutex		activelock;
	
	int					active;
	bool				irqon;
	bool				available;
	axes_t				lastpsval;
#ifdef APDS9900_PENDING_WAKELOCK
	struct wake_lock pendinglock;
#endif
#ifdef APDS9900_WAKELOCK_ENABLE
	struct wake_lock wakelock;
	bool				wakestatus;
#endif
#ifdef APDS9900_ALWAYSON_ENABLE
	struct wake_lock alwayson;
	bool alwaysonstatus;
#endif

} apds9900data_t;

static  apds9900data_t apds9900data;

/* -------------------------------------------------------------------- */
/* Function Proto type */
/* -------------------------------------------------------------------- */
static int apds9900_i2c_read(u8 reg, u8 *buf, int count);
static int apds9900_i2c_write(u8 reg, u8 *data, int len);
static int apds9900_i2c_command(u8 cmd);
static int apds9900_set_reg(u8 reg, u8 data);
static int apds9900_set_thrconfig(u16 thrh, u16 thrl);
static bool apds9900_get_available(void);
static int apds9900_get_psval(u8* psval);
static int apds9900_get_lsval(axes_t* lsval);
#define apds9900_get_reg(reg,data)	apds9900_i2c_read(reg,data,1)
#define apds9900_clear_intreg()			apds9900_i2c_command(CMD_CLR_PS_INT)
static irqreturn_t apds9900_irq_handler(int irq, void *dev_id);
static void apds9900_work_f(struct work_struct *work);
static int apds9900_set_irq(bool enable);
static int apds9900_set_defconfig(int active);
static int apds9900_set_irqdirection(void);


#ifdef APDS9900_WAKELOCK_ENABLE
static inline void apds9900_init_wakelock(void);
static inline void apds9900_deinit_suspend(void);
static inline void apds9900_prevent_suspend(void);
static inline void apds9900_allow_suspend(void);
#endif
#ifdef APDS9900_ALWAYSON_ENABLE
static inline void apds9900_init_alwayson(void);
static inline void apds9900_deinit_alwayson(void);
static inline void apds9900_open_alwayson(void);
static inline void apds9900_close_alwayson(void);
#endif


/* -------------------------------------------------------------------- */
/* External Functions */
/* -------------------------------------------------------------------- */

int apds9900_control_enable(int type, bool enable)
{
	int ret = 0;
	int newactive = 0;

	dbg_func_in();

	printk("sensor_type = %d / enable = %d / available = %d\n", type, enable, apds9900data.available);
	if(apds9900data.available == 0) {
		printk("%s : apds9900data is unavailable\n", __func__);
		return -1;
	}

	mutex_lock(&apds9900data.activelock);
	
	if(enable) newactive = apds9900data.active | type;
	else newactive = apds9900data.active & (APDS9900_TYPE_ALL & (~type));

	dbg("current_active = %d / new_active = %d\n", apds9900data.active, newactive);

	if(newactive != apds9900data.active) {
		#ifdef APDS9900_ALWAYSON_ENABLE
		if(newactive & APDS9900_TYPE_PROXIMITY) apds9900_open_alwayson();
		else apds9900_close_alwayson();
		#endif
		apds9900data.active = newactive;
		ret = apds9900_set_defconfig(newactive);
	}

	mutex_unlock(&apds9900data.activelock);

	return ret;
}

static int apds9900_suspend(struct device *dev)
{
	dbg_func_in();

	mutex_lock(&apds9900data.activelock);

	#ifdef APDS9900_WAKELOCK_ENABLE
	apds9900_prevent_suspend();
	#endif
	
	if( apds9900data.irqon ) {
		set_irq_wake(APDS9900_PS_IRQ, 1);
	}
	
	#ifdef APDS9900_WAKELOCK_ENABLE
	apds9900_allow_suspend();
	#endif
	
	mutex_unlock(&apds9900data.activelock);

	dbg_func_out();

	return 0;
}

static int apds9900_resume(struct device *dev)
{
	dbg_func_in();

	mutex_lock(&apds9900data.activelock);
	
	if( apds9900data.irqon ) {
		set_irq_wake(APDS9900_PS_IRQ, 0);
	}
	
	mutex_unlock(&apds9900data.activelock);

	dbg_func_out();
	
	return 0;
}

int apds9900_ps_measure(axes_t *val)
{
	//int thr;

	#ifdef APDS9900_SUNLIGHT_CANCELATION
	axes_t lsval;
	#endif

	mutex_lock(&apds9900data.activelock);
	
	if(apds9900data.active & APDS9900_TYPE_PROXIMITY) {

		// check apds9900 is available.
		if(!apds9900data.available) return -1;

		#if defined(APDS9900_PS_MEASURE_POLL_ENABLE) || defined(APDS9900_SKY_FITNESS_ENABLE)
		{
			u8 adc;
			apds9900_get_psval(&adc);
			val->z = adc;
		}
		#else	
		mutex_lock(&apds9900data.psvallock);
		val->z = apds9900data.lastpsval.z;
		mutex_unlock(&apds9900data.psvallock);
		#endif
		/*
		dbg("isNear2Far = %d\n", apds9900data.isNear2Far);
		thr = apds9900data.isNear2Far ? APDS9900_THRESHOLD_LOW : APDS9900_THRESHOLD_HIGH;
		*/
		if(apds9900data.lastpsval.z >= APDS9900_THRESHOLD_HIGH) val->x = 0;
		else val->x = 5;
		val->y = apds9900data.lastpsval.z;
	}
	else {
		dbg("apds9900_ps_measure : disable proximity");
		val->x = 5;
		val->y = 0;
		val->z = -1;
	}
	
	#ifdef APDS9900_SUNLIGHT_CANCELATION
	if(val->x < 5) {
		apds9900_get_lsval(&lsval);
		if(lsval.y >= ALS_COEF_MAX_CDATA) {
			val->x = 5;
		}
		val->y = lsval.y;
	}
	#endif

	mutex_unlock(&apds9900data.activelock);

	dbg("apds9900_ps_measure : (%d,%d,%d)\n", val->x, val->y, val->z);
	
	return 0;
}

int apds9900_ls_measure(axes_t *val)
{
	int ret = 0;
	
	mutex_lock(&apds9900data.activelock);
	
	if(apds9900data.active & APDS9900_TYPE_LIGHT) {
	
		ret = apds9900_get_lsval(val);
	}
	else {
		val->x = 0;
		val->y = 0;
		val->z = -1;
	}
	
	mutex_unlock(&apds9900data.activelock);
	
	dbg("apds9900_ls_measure : (%d,%d,%d)\n", val->x, val->y, val->z);

	//dbg_func_out();

	return ret;
}


/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* Internal Functions */
/* -------------------------------------------------------------------- */

/* ------------- I2C Interface ---------------*/
static int apds9900_i2c_read(u8 reg, u8 *buf, int count)
{
	int rc1, rc2;
	int ret = 0; 

	//dbg_func_in();
	
	if ( apds9900data.client == NULL ) {
		printk("%s : apds9900data.client is NULL\n", __func__);
		return -1;
	}

	buf[0] = reg;
	
	//client->addr = APDS9900_SLAVE_ADDR;

	mutex_lock(&apds9900data.i2clock); 
	rc1 = i2c_master_send(apds9900data.client,  buf, 1);

	rc2 = i2c_master_recv(apds9900data.client, buf, count);
	mutex_unlock(&apds9900data.i2clock);

	if ( (rc1 != 1) || (rc2 != count ) ) {
		printk("apds9900_i2c_read FAILED: read of register %d(rc1=%d/rc2=%d)\n", reg, rc1,rc2);
		ret = -1;
	}

	//dbg_func_out();

	return ret;
}

static int apds9900_i2c_write(u8 reg, u8 *data, int len)
{
	u8  buf[20];
	int rc;
	int ret = 0;
	int i;
	
	//dbg_func_in();
	
	if ( apds9900data.client == NULL ) {
		printk("%s : apds9900data.client is NULL\n", __func__);
		return -ENODEV;
	}
	
	buf[0] = reg;
	if (len >= 20) {
		dev_err(&apds9900data.client->dev, "apds9900_i2c_write FAILED: buffer size is limitted(20)\n");
		return -1;
	}
	for( i=0 ; i<len; i++ ) buf[i+1] = data[i];

	//client->addr = APDS9900_SLAVE_ADDR;

	mutex_lock(&apds9900data.i2clock);  
	rc = i2c_master_send(apds9900data.client, buf, len+1);
	mutex_unlock(&apds9900data.i2clock);
	if (rc != len+1) {
		dev_err(&apds9900data.client->dev, "apds9900_i2c_write FAILED: writing to reg %d\n", reg);
		ret = -1;
	}

	//dbg_func_out();
	
	return ret;
}

static int apds9900_i2c_command(u8 cmd)
{
	u8            buf[2];
	int           rc;
	int           ret = 0;

	//dbg_func_in();
	
	if ( apds9900data.client == NULL ) {
		printk("%s : apds9900data.client is NULL\n", __func__);
		return -ENODEV;
	}
	
	buf[0] = cmd;

	mutex_lock(&apds9900data.i2clock);
	rc = i2c_master_send(apds9900data.client, buf, 1);
	mutex_unlock(&apds9900data.i2clock);
	if (rc != 1) {
		dev_err(&apds9900data.client->dev, "apds9900_i2c_command FAILED: writing to command %d\n", cmd);
		ret = -1;
	}

	//dbg_func_out();
	
	return ret;
}
/* -------------------------------------------------------------------- */

/* ------------- Interrupt and Handler ---------------*/
static DECLARE_WORK(apds9900_work, apds9900_work_f);

int apds9900_set_irq(bool enable)
{
	int ret = 0;
	
	dbg("%s : irqon=%d / enable=%d\n", __func__, apds9900data.irqon, enable);

	// 1. check apds9900 is available.
	if(!apds9900data.available) return -1;

	// 2. ignore if current setting is same with old setting.
	if ( enable == apds9900data.irqon ) return 0;

	#ifdef APDS9900_WAKELOCK_ENABLE
	// 3. prevent suspend until setting irq.
	apds9900_prevent_suspend();
	#endif

	// 4. enable irq.
	if ( enable ) {
		// 4.1 enable irq
		ret = request_irq( APDS9900_PS_IRQ, &apds9900_irq_handler, IRQF_TRIGGER_FALLING, "apds9900_ps_irq", 0 );
		if (ret) {
			printk("%s : FAILED: request_irq ret=[  %d  ]\n", __func__, ret);
			#ifdef APDS9900_WAKELOCK_ENABLE
			apds9900_allow_suspend();
			#endif
			return ret;
		}
	} else {
		// 4.2 disable irq
		free_irq( APDS9900_PS_IRQ, NULL );

		printk("interrupt disabled\n");
	}

	// 5. updaet irqon status
	apds9900data.irqon = enable;

	#ifdef APDS9900_WAKELOCK_ENABLE
	// 6. allow suspend
	apds9900_allow_suspend();
	#endif
	
	return 0;
}


static irqreturn_t apds9900_irq_handler(int irq, void *dev_id)
{
	printk("%s : irq_handler_index\n", __func__);
	schedule_work(&apds9900_work);
//	printk("call apds9900_clear_intreg() at %s\n", __func__);
//	apds9900_clear_intreg();
	return IRQ_HANDLED;
}

static void apds9900_work_f(struct work_struct *work)
{
	#ifdef APDS9900_PENDING_WAKELOCK
	wake_lock_timeout(&apds9900data.pendinglock, 100);
	printk("apds9900 wakelock timer 100ms\n");
	#endif

	printk("%s : run work_function\n", __func__);

	mutex_lock(&apds9900data.activelock);

	#ifdef APDS9900_WAKELOCK_ENABLE
	// 1. prevent suspend until prossing this ISR.
	// If aARM was waked by interrupt, aARM must be prevented going suspend status. Otherwise aARM may suspend before prossing ISR.
	apds9900_prevent_suspend();
	#endif

	// 2. switch interrupt condition
	if(apds9900data.irqon) apds9900_set_irqdirection();

	#ifdef APDS9900_WAKELOCK_ENABLE
	// 3. allow suspend
	apds9900_allow_suspend();
	#endif

//	printk("call apds9900_clear_intreg() at %s\n", __func__);
//	apds9900_clear_intreg();
	
	mutex_unlock(&apds9900data.activelock);
}

/* -------------------------------------------------------------------- */


/* ------------- Register ---------------*/

static bool apds9900_get_available(void)
{
	u8 id=0;
	int ret = 0;
	
	dbg_func_in();

	// 1. check client
	if(apds9900data.client == NULL) {
		printk("[ERR] %s : i2c client is NULL\n", __func__);
		return 0;
	}

	// 2. get chip-id and compare
	ret = apds9900_get_reg(APDS9900_ID_REG, &id);
	printk("%s : ret = %d / id = 0x%02X\n", __func__, ret, id);
	if( (ret) /*|| (id != APDS9900_CHIP_ID)*/ ) {
		printk("[ERR] %s : apds9900 id is not 0x%02X\n", __func__, APDS9900_CHIP_ID);
		return 0;
	}

	dbg_func_out();
	
	return 1;
}

static int apds9900_set_irqdirection(void)
{
	u8 adc = 0;
	int ret = 0;

	dbg_func_in();

	if( (!apds9900data.available) ) {
		ret = -1;
	}
	else {
		// 1. get ps val
		ret = apds9900_get_psval(&adc);
	}

	if(ret) {
		printk("APDS9900 I2C is not alive\n");
		mutex_lock(&apds9900data.psvallock);
		apds9900data.lastpsval.z = 0;
		mutex_unlock(&apds9900data.psvallock);
	}
	else {
		printk("adc = %d\n", adc);
		mutex_lock(&apds9900data.psvallock);
		apds9900data.lastpsval.z = adc;
		mutex_unlock(&apds9900data.psvallock);
	}

	if(adc >= APDS9900_THRESHOLD_HIGH ) apds9900_set_near2far();
	else apds9900_set_far2near();

	dbg_func_out();

	return ret;
}


static int apds9900_set_thrconfig(u16 thrh, u16 thrl)
{
	int ret = 0;
	u8 enable = 0x2D;

	dbg("threshold (%d~%d)\n", thrl>>4, thrh>>4);

	#ifdef APDS9900_SUNLIGHT_CANCELATION
	if ( (apds9900data.active & APDS9900_TYPE_PROXIMITY) || (apds9900data.active & APDS9900_TYPE_LIGHT) ) {
		enable |= 0x03;
	}
	#else
	if(apds9900data.active & APDS9900_TYPE_LIGHT) {
		enable |= 0x03;
	}
	#endif

	ret |= apds9900_set_reg(APDS9900_PERS_REG, 0x30);
	
	ret |= apds9900_set_reg(APDS9900_PILTL_REG, (u8)(thrl & 0x00FF));
	ret |= apds9900_set_reg(APDS9900_PILTH_REG, (u8)(((thrl & 0xFF00) >> 8) & 0xFF));
	ret |= apds9900_set_reg(APDS9900_PIHTL_REG, (u8)(thrh & 0x00FF));
	ret |= apds9900_set_reg(APDS9900_PIHTH_REG, (u8)(((thrh & 0xFF00) >> 8) & 0xFF));
	
	ret |= apds9900_set_reg(APDS9900_ENABLE_REG, enable);
	printk("call apds9900_clear_intreg() at %s\n", __func__);
	ret |= apds9900_clear_intreg();

	apds9900data.isNear2Far = thrl ? 1 : 0;

	return ret;
}

int apds9900_set_defconfig(int active)
{
	u8 oldenable = 0;
	int ret = 0;
	register_t reg = {
		.enable = 0,
		.atime = 0,
		.ptime = 0,
		.wtime = 0,
		.ppcount = 0,
		.control = 0
	};

	dbg_func_in();

	// 0. check apds9900 is available.
	if(!apds9900data.available) return -1;

	#ifdef APDS9900_SUNLIGHT_CANCELATION

	if ( (active & APDS9900_TYPE_PROXIMITY) || (active & APDS9900_TYPE_LIGHT) ) {
		if(active & APDS9900_TYPE_PROXIMITY) reg.enable |= 0x2F; 
		if(active & APDS9900_TYPE_LIGHT) reg.enable |= 0x03; 
		reg.atime |= ( 0 | APDS9900_ALSTIME);
		reg.ptime |= ( 0xFF | 0 ); // 2.72ms
		reg.wtime |= 0xF6; // 27.2ms
		reg.ppcount |= ( 0x04 | 0 ); // 4 proximity cycle generates an interrupt
		reg.control |= 0x20; // 100mA, IR-dioad, 1X Gain
	}

	#else

	if(active & APDS9900_TYPE_PROXIMITY) {
		reg.enable |= 0x0D;
		reg.atime |= 0; // none als time
		reg.ptime |= 0xFF; // 2.72ms
		reg.wtime |= 0xF6; // 27.2ms
		reg.ppcount |= 0x04; // 4 proximity cycle generates an interrupt
		reg.control |= 0x20; // 100mA, IR-dioad, 1X Gain
	}
	if(active & APDS9900_TYPE_LIGHT) {
		reg.enable |= 0x03;
		reg.atime |= APDS9900_ALSTIME;
		reg.ptime |= 0; // none ps time
		reg.wtime |= 0xF6; // 27.2ms
		reg.ppcount |= 0;
		//reg.control |= 0x01; // 8X Gain
		reg.control |= 0x00; // 1X Gain
	}

	#endif

	// 2. enable active sensor
	ret |= apds9900_get_reg(APDS9900_ENABLE_REG, &oldenable);
	printk("APDS9900_ENABLE_REG oldenable = [%d] reg.eanble =[%d]\n",oldenable,reg.enable);
	ret |= apds9900_set_reg(APDS9900_ENABLE_REG, (oldenable & reg.enable));


	// 3. setup default config to enable activity
	if(active) {
		ret |= apds9900_set_reg(APDS9900_ATIME_REG, reg.atime);
		ret |= apds9900_set_reg(APDS9900_PTIME_REG, reg.ptime);
		ret |= apds9900_set_reg(APDS9900_WTIME_REG, reg.wtime);
		ret |= apds9900_set_reg(APDS9900_PPCOUNT_REG, reg.ppcount);
		ret |= apds9900_set_reg(APDS9900_CONTROL_REG, reg.control);
		
		// 4. enable interrupt
		if(active & APDS9900_TYPE_PROXIMITY) {
			ret |= apds9900_set_irq(1);
			apds9900_set_irqdirection();
			//printk("call apds9900_clear_intreg() at %s\n", __func__);
			//ret |= apds9900_clear_intreg();
		}
		else {
			// 1. disable irq
			ret |= apds9900_set_irq(0);
		}
	}
	ret |= apds9900_set_reg(APDS9900_ENABLE_REG, reg.enable);
	
	// 5. delay minimum 2.7ms after setup register
	msleep(3);
	
	dbg_func_out();
	
	return ret;
}

static int apds9900_set_reg(u8  reg, u8 data)
{
	int ret = apds9900_i2c_write(reg, &data, 1);

	#ifdef SENSOR_APDS9900_DBG_ENABLE
	if (!ret) {
		switch (reg) {
			case APDS9900_ENABLE_REG	: 
				dbg("%s : APDS9900_ENABLE_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_ATIME_REG		: 
				dbg("%s : APDS9900_ATIME_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_PTIME_REG		: 
				dbg("%s : APDS9900_PTIME_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_WTIME_REG		: 
				dbg("%s : APDS9900_WTIME_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_PPCOUNT_REG	: 
				dbg("%s : APDS9900_PPCOUNT_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_CONTROL_REG	: 
				dbg("%s : APDS9900_CONTROL_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_PILTL_REG		: 
				dbg("%s : APDS9900_PILTL_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_PILTH_REG		: 
				dbg("%s : APDS9900_PILTH_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_PIHTL_REG		: 
				dbg("%s : APDS9900_PIHTL_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_PIHTH_REG		: 
				dbg("%s : APDS9900_PIHTH_REG = 0x%02X\n", __func__, data);
				break;
			case APDS9900_PERS_REG		: 
				dbg("%s : APDS9900_PERS_REG = 0x%02X\n", __func__, data);
				break;
			default:
				dbg("%s : REG[0x%02X] = 0x%02X\n", __func__, reg, data);
				break;
		}
	}
	#endif

	return  ret;
}
/* -------------------------------------------------------------------- */

static int apds9900_ls_calculate(int cdata, int irdata)
{
	int IAC1, IAC2, IAC;
	int luxValue;

	if(cdata < ALS_COEF_MAX_CDATA) {
		IAC1 = (cdata*ALS_COEF_UNIT) - (int)(ALS_COEF_B*irdata);
		IAC2 = (int)(ALS_COEF_C*cdata) - (int)(ALS_COEF_D*irdata);
		IAC = (IAC1>IAC2) ? (int)(IAC1/ALS_COEF_UNIT) : (int)(IAC2/ALS_COEF_UNIT);
		luxValue = (int)(IAC*ALS_COEF_GA*ALS_COEF_DF);
		luxValue = (int)(luxValue/APDS9900_ALSTIME_MS);
		luxValue = (int)(luxValue/ALS_COEF_UNIT);
	}
	else {
		luxValue = 2000;
	}

	return luxValue;
}

static int apds9900_get_psval(u8* psval)
{
	int ret = 0;
	u8 data[2];
	
	ret |= apds9900_get_reg(APDS9900_PDATAL_REG, &data[0]);
	ret |= apds9900_get_reg(APDS9900_PDATAH_REG, &data[1]);
	if(ret) {
		printk("[ERR] %s : can't get psval\n", __func__);
		return ret;
	}
	
	*psval = ((data[1]&0x0F) << 4 ) | ((data[0]& 0xF0) >> 4);
	
	dbg("data(0x%02X%02X) -> psval(0x%02X)\n", data[1], data[0], *psval);
	
	return ret;
}

static int apds9900_get_lsval(axes_t* lsval)
{
	int ret=0;
	int cdata, irdata;
	u8 adata_ls[4];

	dbg_func_in();

	if(!apds9900data.available) return -1;

	ret=apds9900_get_reg(APDS9900_CDATAL_REG,&adata_ls[0]);
	ret=apds9900_get_reg(APDS9900_CDATAH_REG,&adata_ls[1]);
	ret=apds9900_get_reg(APDS9900_IRDATAL_REG,&adata_ls[2]);	
	ret=apds9900_get_reg(APDS9900_IRDATAH_REG,&adata_ls[3]);

	cdata=adata_ls[0] | (adata_ls[1]<<8);
	irdata=adata_ls[2] | (adata_ls[3]<<8);

	lsval->x =apds9900_ls_calculate(cdata, irdata);
	lsval->y = cdata;
	lsval->z = irdata;

	dbg("cdata = %d / irdata = %d ==> %d Lux\n", cdata, irdata, lsval->x); 

	dbg_func_out();

	return 0;
}

/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* WAKELOCK Functions */
/* -------------------------------------------------------------------- */
#ifdef APDS9900_WAKELOCK_ENABLE
static inline void apds9900_init_wakelock(void)
{
	if(apds9900data.available)
		wake_lock_init(&apds9900data.wakelock, WAKE_LOCK_SUSPEND, "apds9900_wakelock");
}
static inline void apds9900_deinit_suspend(void)
{
	if(apds9900data.available)
		wake_lock_destroy(&apds9900data.wakelock);
}
static inline void apds9900_prevent_suspend(void)
{
	if(apds9900data.available) {
		if(!apds9900data.wakestatus)
		{
			dbg_func_in();
			wake_lock(&apds9900data.wakelock);
			apds9900data.wakestatus = 1;
			dbg_func_out();
		}
	}
}
static inline void apds9900_allow_suspend(void)
{
	if(apds9900data.available) {
		if(apds9900data.wakestatus)
		{
			dbg_func_in();
			wake_unlock(&apds9900data.wakelock);
			apds9900data.wakestatus = 0;
			dbg_func_out();
		}
	}
}
#endif

#ifdef APDS9900_ALWAYSON_ENABLE
static inline void apds9900_init_alwayson(void)
{
	if(apds9900data.available)
		wake_lock_init(&apds9900data.alwayson, WAKE_LOCK_SUSPEND, "apds9900_alwayson");
}
static inline void apds9900_deinit_alwayson(void)
{
	if(apds9900data.available)
		wake_lock_destroy(&apds9900data.alwayson);
}
static inline void apds9900_open_alwayson(void)
{
	if(apds9900data.available) {
		if(!apds9900data.alwaysonstatus)
		{
			dbg_func_in();
			wake_lock(&apds9900data.alwayson);
			apds9900data.alwaysonstatus = 1;
			dbg_func_out();
		}
	}
}
static inline void apds9900_close_alwayson(void)
{
	if(apds9900data.available) {
		if(apds9900data.alwaysonstatus)
		{
			dbg_func_in();
			wake_unlock(&apds9900data.alwayson);
			apds9900data.alwaysonstatus = 0;
			dbg_func_out();
		}
	}
}
#endif

/* -------------------------------------------------------------------- */

/* -------------------------------------------------------------------- */
/* Driver */
/* -------------------------------------------------------------------- */

static struct i2c_driver apds9900_driver;
static int __devinit apds9900_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct i2c_adapter *adapter = to_i2c_adapter(client->dev.parent);
	int err = SH_FLAG_SUCCESS;
	
	dbg_func_in();

	if (!i2c_check_functionality(adapter, I2C_FUNC_SMBUS_BYTE)) {
		err = -EIO;
		goto err_exit;
	}
	
	// 0. set direction input to catch interrupt.
	gpio_request(APDS9900_PS_INT_N, "apds9900_ps_int");
	gpio_direction_input(APDS9900_PS_INT_N);
	
	// 1. initialize mutex
	mutex_init(&apds9900data.i2clock);
	mutex_init(&apds9900data.psvallock);
	mutex_init(&apds9900data.lsvallock);
	mutex_init(&apds9900data.activelock);
	
	// 2. assign i2c client
	apds9900data.client = client;
	i2c_set_clientdata(client, &apds9900data);

	// 3. check available
	apds9900data.available = apds9900_get_available();
	if(!apds9900data.available) goto err_exit;

	// 4. initialize data
	apds9900data.active = 0;
	apds9900data.irqon			= 0;
	#ifdef APDS9900_WAKELOCK_ENABLE
	apds9900data.wakestatus = 0;
	#endif
	#ifdef APDS9900_ALWAYSON_ENABLE
	apds9900data.alwaysonstatus = 0;
	#endif

	// 5. initialize wakelock
	#ifdef APDS9900_PENDING_WAKELOCK
	wake_lock_init(&apds9900data.pendinglock, WAKE_LOCK_SUSPEND, "apds9900_pendinglock");
	#endif
	#ifdef APDS9900_WAKELOCK_ENABLE
	apds9900_init_wakelock();
	#endif
	#ifdef APDS9900_ALWAYSON_ENABLE
	apds9900_init_alwayson();
	#endif

	dbg_func_out();
	
	return err;

err_exit:
	apds9900data.client = NULL;
	return -EIO;
}

static int __devexit apds9900_remove(struct i2c_client *client)
{
	dbg_func_in();

	#ifdef APDS9900_WAKELOCK_ENABLE
	apds9900_deinit_suspend();
	#endif
	if(client != NULL) kfree(i2c_get_clientdata(client));

	dbg_func_out();
	
	return 0;
}

static const struct i2c_device_id apds9900_id[] = {
	{ "apds9900", 0 },
	{ }
};
MODULE_DEVICE_TABLE(i2c, apds9900_id);

const static struct dev_pm_ops i2c_device_apds9900_pm_ops = {
	.suspend = apds9900_suspend,
	.resume = apds9900_resume,
};

static struct i2c_driver apds9900_driver = {
	.driver = {
		.name	= APDS9900_DRV_NAME,
		.owner	= THIS_MODULE,
		.pm = &i2c_device_apds9900_pm_ops,
	},
	.probe	= apds9900_probe,
	.remove	= __devexit_p(apds9900_remove),
	.id_table = apds9900_id,
};

static int __init apds9900_init(void)
{
	return i2c_add_driver(&apds9900_driver);
}

static void __exit apds9900_exit(void)
{
	i2c_del_driver(&apds9900_driver);
}

/* -------------------------------------------------------------------- */

MODULE_AUTHOR("Park Seunghwan <park.seunghwan@pantech.com>");
MODULE_DESCRIPTION("APDS9900 proximity sensor driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DRIVER_VERSION);

module_init(apds9900_init);
module_exit(apds9900_exit);

/* -------------------------------------------------------------------- */
