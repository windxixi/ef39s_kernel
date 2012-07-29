/*
 $License:
    Copyright (C) 2010 InvenSense Corporation, All Rights Reserved.
 $
 */

/**
 *  @defgroup   ACCELDL (Motion Library - Accelerometer Driver Layer)
 *  @brief      Provides the interface to setup and handle an accelerometers
 *              connected to the secondary I2C interface of the gyroscope.
 *
 *  @{
 *      @file   bma250.c
 *      @brief  Accelerometer setup and handling methods.
 */


/* ------------------ */
/* - Include Files. - */
/* ------------------ */

#ifdef __KERNEL__
#include <linux/module.h>
#endif

#include "mpu.h"
#include "mlos.h"
#include "mlsl.h"


/* full scale setting - register and mask */
#define ACCEL_BMA250_CTRL_REG       (0x0f)  /* BMA250 : full scale setting register */
#define ACCEL_BMA250_CTRL_MASK      (0x0f)  /* BMA250 : full scale setting mask */
#define ACCEL_BMA250_BW_REG       (0x10)  /* BMA250 : BW setting register */
#define ACCEL_BMA250_BW_MASK      (0x1f)     /* BMA250 : BW setting mask */


/*********************************************
    Accelerometer Initialization Functions
**********************************************/

static int bma250_suspend(void *mlsl_handle,
					struct ext_slave_descr *slave,
					struct ext_slave_platform_data *pdata)
{

    int result;
#if 1
	//---	11/07/14 added by hkkwon@PS2	---//
	result = MLSLSerialWriteSingle(mlsl_handle, pdata->address, 0x11, 0x80);	/*	BMA250 : suspend mode	*/
	MLOSSleep(3); /* 3 ms powerup time maximum */
	ERROR_CHECK(result);
#else
	result = MLSLSerialWriteSingle(mlsl_handle, pdata->address, 0x14, 0xb6); /* BMA250 : Software reset */
#endif
    return result;
}

static int bma250_resume(void * mlsl_handle,
					struct ext_slave_descr *slave,
					struct ext_slave_platform_data *pdata)
{

int result;
unsigned char reg = 0;

/* Soft reset */
result = MLSLSerialWriteSingle(mlsl_handle, pdata->address, 0x14, 0xb6 );		  /* BMA250 : Software reset */
ERROR_CHECK(result);
MLOSSleep(10);

result = MLSLSerialRead(mlsl_handle, pdata->address, ACCEL_BMA250_CTRL_REG, 1, &reg);
ERROR_CHECK(result);

/* BMA250 : Full Scale */
reg &= ~ACCEL_BMA250_CTRL_MASK;
reg |= 0x00;

if (slave->range.mantissa==2) {
	reg |= 0x03;
} else if (slave->range.mantissa==4) {
	reg |= 0x05;
} else if (slave->range.mantissa==8) {
	reg |= 0x08;
}

result = MLSLSerialWriteSingle(mlsl_handle, pdata->address, ACCEL_BMA250_CTRL_REG, reg );
ERROR_CHECK(result);

result = MLSLSerialRead(mlsl_handle, pdata->address, ACCEL_BMA250_BW_REG, 1, &reg);
ERROR_CHECK(result);
reg &= ~ACCEL_BMA250_BW_MASK;
reg |= 0x00;

/* BMA250: Bandwidth */
reg |= 0x0d;	  // bw=250
result = MLSLSerialWriteSingle(mlsl_handle, pdata->address, ACCEL_BMA250_BW_REG, reg );
ERROR_CHECK(result);

return result;
}



static int bma250_read(void * mlsl_handle,
				struct ext_slave_descr *slave,
				struct ext_slave_platform_data *pdata,
				unsigned char *data)
{
	int result;
	result = MLSLSerialRead(mlsl_handle, pdata->address,
				slave->reg, slave->len, data);
	return result;
}

static struct ext_slave_descr bma250_descr = {
    /*.init             = */ NULL,
    /*.exit             = */ NULL,
    /*.suspend          = */ bma250_suspend,
    /*.resume           = */ bma250_resume,
    /*.read             = */ bma250_read,
    /*.config           = */ NULL,
#if !defined(CONFIG_EF39S_SENSORS_BMA250) && !defined(CONFIG_EF40K_SENSORS_BMA250) && !defined(CONFIG_EF40S_SENSORS_BMA250)
    /*.get_config       = */ NULL,
#endif
    /*.name             = */ "bma250",
    /*.type             = */ EXT_SLAVE_TYPE_ACCELEROMETER,
    /*.id               = */ ACCEL_ID_BMA250,
    /*.reg              = */ 0x02,
    /*.len              = */ 6,
    /*.endian           = */ EXT_SLAVE_LITTLE_ENDIAN,
    /*.range            = */ { 2, 0 },
};

struct ext_slave_descr *bma250_get_slave_descr(void)
{
    return &bma250_descr;
}

#ifdef __KERNEL__
EXPORT_SYMBOL(bma250_get_slave_descr);
#endif

#ifdef __KERNEL__
MODULE_AUTHOR("Invensense");
MODULE_DESCRIPTION("User space IRQ handler for MPU3xxx devices");
MODULE_LICENSE("GPL");
MODULE_ALIAS("bma");
#endif

/**
 *  @}
**/
