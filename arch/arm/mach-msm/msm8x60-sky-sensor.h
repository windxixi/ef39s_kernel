#ifndef _MSM8X60_SKY_SENSOR_H_
#define _MSM8X60_SKY_SENSOR_H_

#include <linux/gpio.h>
#include <linux/i2c-gpio.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/i2c.h>
#include <mach/vreg.h>

#if !defined(CONFIG_MACH_MSM8X60_PRESTO)
#define I2C_DEV_INDEX_PROXIMITY			12 //  /dev/i2c-12
#define I2C_DEV_INDEX_GYROSCOPE			13 //  /dev/i2c-13
#else
#define I2C_DEV_INDEX_PROXIMITY			13 //  /dev/i2c-13
#define I2C_DEV_INDEX_GEOMAGNETIC		14 //  /dev/i2c-13
#define I2C_DEV_INDEX_ACCELEROMETER		15 //  /dev/i2c-13
#endif

// GYROSCOPE
#if (defined(CONFIG_EF39S_SENSORS_MPU3050) && (BOARD_REV >= TP10)) || \
	(defined(CONFIG_EF40K_SENSORS_MPU3050) && (BOARD_REV >= WS20)) || \
	(defined(CONFIG_EF40S_SENSORS_MPU3050) && (BOARD_REV >= WS20))
//#define GYROSCOPE_SCL_PIN 52
//#define GYROSCOPE_SDA_PIN 51
#else
#define GYROSCOPE_SCL_PIN 17
#define GYROSCOPE_SDA_PIN 18
#endif
#define GYROSCOPE_UDELAY 2 // 250 kHz
#define GYROSCOPE_INT_PIN       30

// GEOMAGNETIC
#if defined(CONFIG_PANTECH_PRESTO_SENSORS_YAS530)
#define GEOMAGNETIC_SCL_PIN	170
#define GEOMAGNETIC_SDA_PIN	171
#define GEOMAGNETIC_INT_PIN	33
#define GEOMAGNETIC_UDELAY  1
#endif

// ACCELEROMETER
#if defined(CONFIG_PANTECH_PRESTO_SENSORS_BMA250)
#define ACCELEROMETER_SCL_PIN	124
#define ACCELEROMETER_SDA_PIN	127
#define ACCELEROMETER_INT_PIN	58
#define ACCELEROMETER_UDELAY	1
#endif

// PROXIMITY
#if defined(CONFIG_EF39S_SENSORS_APDS9900_SW) || defined(CONFIG_EF40S_SENSORS_APDS9900_SW)  || defined(CONFIG_EF40K_SENSORS_APDS9900_SW) || defined(CONFIG_PANTECH_PRESTO_SENSORS_APDS9190)
#define PROXIMITY_SCL_PIN 36
#define PROXIMITY_SDA_PIN 35
#define PROXIMITY_UDELAY 2 // 250 kHz
#define PROXIMITY_INT_PIN       34
#endif

#if !defined(CONFIG_MACH_MSM8X60_PRESTO)
static struct i2c_gpio_platform_data gpio_i2c_gyroscope_data = {
#if defined(GYROSCOPE_SCL_PIN) && defined(GYROSCOPE_SDA_PIN) && defined(GYROSCOPE_UDELAY)
	.scl_pin = GYROSCOPE_SCL_PIN,
	.sda_pin = GYROSCOPE_SDA_PIN,
	.udelay = GYROSCOPE_UDELAY
#endif
};
#endif

#if defined(CONFIG_MACH_MSM8X60_PRESTO)
static struct i2c_gpio_platform_data gpio_i2c_geomagnetic_data = {
#if defined(GEOMAGNETIC_SCL_PIN) && defined(GEOMAGNETIC_SDA_PIN) && defined(GEOMAGNETIC_UDELAY)
	.scl_pin = GEOMAGNETIC_SCL_PIN,
	.sda_pin = GEOMAGNETIC_SDA_PIN,
	.udelay = GEOMAGNETIC_UDELAY
#endif
};
#endif

#if defined(CONFIG_MACH_MSM8X60_PRESTO)
static struct i2c_gpio_platform_data gpio_i2c_accelerometer_data = {
#if defined(ACCELEROMETER_SCL_PIN) && defined(ACCELEROMETER_SDA_PIN) && defined(ACCELEROMETER_UDELAY)
	.scl_pin = ACCELEROMETER_SCL_PIN,
	.sda_pin = ACCELEROMETER_SDA_PIN,
	.udelay = ACCELEROMETER_UDELAY
#endif
};
#endif

static struct i2c_gpio_platform_data gpio_i2c_proximity_data = {
#if defined(PROXIMITY_SCL_PIN) && defined(PROXIMITY_SDA_PIN) && defined(PROXIMITY_UDELAY)
	.scl_pin = PROXIMITY_SCL_PIN,
	.sda_pin = PROXIMITY_SDA_PIN,
	.udelay = PROXIMITY_UDELAY
#endif
};

#if !defined(CONFIG_MACH_MSM8X60_PRESTO)
struct platform_device msm_device_sensor_gyroscope = {
	.name = "i2c-gpio",
	.id = I2C_DEV_INDEX_GYROSCOPE,
	.dev = {
		.platform_data = &gpio_i2c_gyroscope_data,
	}
};
#endif

#if defined(CONFIG_MACH_MSM8X60_PRESTO)
struct platform_device msm_device_sensor_geomagnetic = {
	.name = "i2c-gpio",
	.id = I2C_DEV_INDEX_GEOMAGNETIC,
	.dev = {
		.platform_data = &gpio_i2c_geomagnetic_data,
	}
};
#endif

#if defined(CONFIG_MACH_MSM8X60_PRESTO)
struct platform_device msm_device_sensor_accelerometer = {
	.name = "i2c-gpio",
	.id = I2C_DEV_INDEX_ACCELEROMETER,
	.dev = {
		.platform_data = &gpio_i2c_accelerometer_data,
	}
};
#endif

struct platform_device msm_device_sensor_proximity = {
	.name = "i2c-gpio",
	.id = I2C_DEV_INDEX_PROXIMITY,
	.dev = {
		.platform_data = &gpio_i2c_proximity_data,
	}
};

#if defined(CONFIG_PANTECH_PRESTO_SENSORS_YAS530)
static struct i2c_board_info __initdata geomagnetic_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("geomagnetic", 0x2E),
	},
};
#endif

#if defined(CONFIG_PANTECH_PRESTO_SENSORS_BMA250)
static struct i2c_board_info __initdata accelerometer_i2c_info[] __initdata = {
	{
		I2C_BOARD_INFO("accelerometer", 0x38),
	},
};
#endif

static struct i2c_board_info __initdata proximity_i2c_info[] __initdata = {
#if defined(CONFIG_EF39S_SENSORS_APDS9900_SW) || defined(CONFIG_EF40S_SENSORS_APDS9900_SW) || defined(CONFIG_EF40K_SENSORS_APDS9900_SW) || defined(CONFIG_PANTECH_PRESTO_SENSORS_APDS9190)
	{
#if defined(CONFIG_PANTECH_PRESTO_SENSORS_APDS9190)
		I2C_BOARD_INFO("apds9190", 0x39),
#else
		I2C_BOARD_INFO("apds9900", 0x39),
#endif		
	},
#endif
};

#if !defined(CONFIG_MACH_MSM8X60_PRESTO)
static struct i2c_board_info __initdata gyroscope_i2c_info[] __initdata = {
#if defined(CONFIG_EF39S_SENSORS_MPU3050) || defined(CONFIG_EF40K_SENSORS_MPU3050) || defined(CONFIG_EF40S_SENSORS_MPU3050)
	{
		I2C_BOARD_INFO("mpu3050", 0x68),
		.irq = MSM_GPIO_TO_INT(GYROSCOPE_INT_PIN), //gpio_to_irq
	},
#endif
#if defined(CONFIG_EF39S_SENSORS_YAS530) || defined(CONFIG_EF40S_SENSORS_YAS530) || defined(CONFIG_EF40K_SENSORS_YAS530)
	{
		I2C_BOARD_INFO("geomagnetic", 0x2E),
	},
#endif
};
#endif


static int sensors_hw_init(void)
{
	int rc = 0;
	
#ifdef GYROSCOPE_INT_PIN
	rc = gpio_tlmm_config(GPIO_CFG(GYROSCOPE_INT_PIN, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, GYROSCOPE_INT_PIN);
		return rc;
	}
#endif

#if (defined(CONFIG_EF39S_SENSORS_MPU3050) && (BOARD_REV < TP10)) || \
	(defined(CONFIG_EF40K_SENSORS_MPU3050) && (BOARD_REV < WS20)) || \
	(defined(CONFIG_EF40S_SENSORS_MPU3050) && (BOARD_REV < WS20)) || \
	(defined(CONFIG_MACH_MSM8X60_PRESTO))
#ifdef GYROSCOPE_SCL_PIN
	rc = gpio_tlmm_config(GPIO_CFG(GYROSCOPE_SCL_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, GYROSCOPE_SCL_PIN);
		return rc;
	}
#endif

#ifdef GYROSCOPE_SDA_PIN
	rc = gpio_tlmm_config(GPIO_CFG(GYROSCOPE_SDA_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, GYROSCOPE_SDA_PIN);
		return rc;
	}
#endif
#endif

#ifdef GEOMAGNETIC_SCL_PIN
	rc = gpio_tlmm_config(GPIO_CFG(GEOMAGNETIC_SCL_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, GEOMAGNETIC_SCL_PIN);
		return rc;
	}
#endif

#ifdef GEOMAGNETIC_SDA_PIN
	rc = gpio_tlmm_config(GPIO_CFG(GEOMAGNETIC_SDA_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, GEOMAGNETIC_SDA_PIN);
		return rc;
	}
#endif

#ifdef GEOMAGNETIC_INT_PIN
	rc = gpio_tlmm_config(GPIO_CFG(GEOMAGNETIC_INT_PIN, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, GEOMAGNETIC_INT_PIN);
		return rc;
	}
#endif

#ifdef ACCELEROMETER_SCL_PIN
	rc = gpio_tlmm_config(GPIO_CFG(ACCELEROMETER_SCL_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, ACCELEROMETER_SCL_PIN);
		return rc;
	}
#endif

#ifdef ACCELEROMETER_SDA_PIN
	rc = gpio_tlmm_config(GPIO_CFG(ACCELEROMETER_SDA_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, ACCELEROMETER_SDA_PIN);
		return rc;
	}
#endif

#ifdef ACCELEROMETER_INT_PIN
	rc = gpio_tlmm_config(GPIO_CFG(ACCELEROMETER_INT_PIN, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, ACCELEROMETER_INT_PIN);
		return rc;
	}
#endif

#ifdef PROXIMITY_INT_PIN
	rc = gpio_tlmm_config(GPIO_CFG(PROXIMITY_INT_PIN, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, PROXIMITY_INT_PIN);
		return rc;
	}
#endif

#ifdef PROXIMITY_SCL_PIN
	rc = gpio_tlmm_config(GPIO_CFG(PROXIMITY_SCL_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, PROXIMITY_SCL_PIN);
		return rc;
	}
#endif

#ifdef PROXIMITY_SDA_PIN
	rc = gpio_tlmm_config(GPIO_CFG(PROXIMITY_SDA_PIN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_PULL_UP, GPIO_CFG_2MA), GPIO_CFG_ENABLE);
	if (rc) {
		pr_err("%s: Could not configure gpio %d\n", __func__, PROXIMITY_SDA_PIN);
		return rc;
	}
#endif

	return rc;
}


#endif // _MSM8X60_SKY_SENSOR_H_
