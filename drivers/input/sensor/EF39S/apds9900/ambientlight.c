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

#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/platform_device.h>
#include <linux/workqueue.h>
#include <linux/slab.h>
#include "apds9900.h"

/* -------------------------------------------------------------------- */
/* debug option */
/* -------------------------------------------------------------------- */
//#define SENSOR_SKY_LS_DBG_ENABLE
#ifdef SENSOR_SKY_LS_DBG_ENABLE 
#define dbg(fmt, args...)   printk("[ALS] " fmt, ##args)
#else
#define dbg(fmt, args...)
#endif
#define dbg_func_in()       dbg("[FUNC_IN] %s\n", __func__)
#define dbg_func_out()      dbg("[FUNC_OUT] %s\n", __func__)
#define dbg_line()          dbg("[LINE] %d(%s)\n", __LINE__, __func__)
/* -------------------------------------------------------------------- */

#define SENSOR_INPUTDEV_NAME	"ambientlight"
#define SENSOR_DEFAULT_DELAY    (200)   /* 200 ms */
#define SENSOR_MIN_DELAY        (20)    /* 20 ms */
#define SENSOR_MAX_DELAY        (2000)  /* 2000 ms */
#define SENSOR_MIN_ABS			(0)
#define SENSOR_MAX_ABS			(1024)
#define delay_to_jiffies(d) 	((d)?msecs_to_jiffies(d):1)

struct sensor_data {
	atomic_t enable;
	atomic_t delay;
	axes_t lastval;
    struct mutex enable_mutex;
	struct mutex data_mutex;
	struct delayed_work work;
	bool status;
};


static struct platform_device *sensor_pdev = NULL;
static struct input_dev *input_pdev = NULL;

static int sensor_get_enable(struct device *dev)
{
	struct input_dev *inputdev = to_input_dev(dev);
    struct sensor_data *sensordata = input_get_drvdata(inputdev);
	int enable;
	
	dbg_func_in();

	enable = atomic_read(&sensordata->enable);

	dbg_func_out();

	return enable;
}

static void sensor_set_enable(struct device *dev, int enable)
{
	struct input_dev *inputdev = to_input_dev(dev);
    struct sensor_data *sensordata = input_get_drvdata(inputdev);
	int delay = atomic_read(&sensordata->delay);

	dbg_func_in();

	mutex_lock(&sensordata->enable_mutex);

	if (enable) {                   /* enable if state will be changed */
		if (!atomic_cmpxchg(&sensordata->enable, 0, 1)) {
			sensordata->status = apds9900_control_enable(APDS9900_TYPE_LIGHT, true) ? 0 : 1;
			if(sensordata->status) {
			    schedule_delayed_work(&sensordata->work, delay_to_jiffies(delay) + 1);
		    }
		}
	} else {                        /* disable if state will be changed */
		if (atomic_cmpxchg(&sensordata->enable, 1, 0) && sensordata->status) {
			if(sensordata->status) {
				cancel_delayed_work_sync(&sensordata->work);
				sensordata->status = apds9900_control_enable(APDS9900_TYPE_LIGHT, false) ? 1 : 0;
			}
		}
	}
	atomic_set(&sensordata->enable, enable);

	mutex_unlock(&sensordata->enable_mutex);

	dbg_func_out();
}

static int sensor_get_delay(struct device *dev)
{
	struct input_dev *inputdev = to_input_dev(dev);
    struct sensor_data *sensordata = input_get_drvdata(inputdev);
	int delay;
	
	dbg_func_in();

	delay = atomic_read(&sensordata->delay);

	dbg_func_out();

	return delay;
}

static void sensor_set_delay(struct device *dev, int delay)
{
    struct input_dev *inputdev = to_input_dev(dev);
    struct sensor_data *sensordata = input_get_drvdata(inputdev);

	dbg_func_in();

	atomic_set(&sensordata->delay, delay);

    mutex_lock(&sensordata->enable_mutex);

	if (sensor_get_enable(dev)) {
		cancel_delayed_work_sync(&sensordata->work);
		schedule_delayed_work(&sensordata->work, delay_to_jiffies(delay) + 1);
	}

    mutex_unlock(&sensordata->enable_mutex);

	dbg_func_out();
}


/* Sysfs interface - control */
static ssize_t sensor_delay_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct input_dev *inputdev = to_input_dev(dev);
    struct sensor_data *sensordata = input_get_drvdata(inputdev);
    int delay;

	dbg_func_in();

    mutex_lock(&sensordata->enable_mutex);

    delay = sensor_get_delay(dev);

    mutex_unlock(&sensordata->enable_mutex);

	dbg("%s : delay = %d\n", __func__, delay);

	dbg_func_out();

    return sprintf(buf, "%d\n", delay);
}

static ssize_t sensor_delay_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int delay = simple_strtoul(buf, NULL, 10);

	dbg_func_in();
	
	if (delay < 0) {
        return count;
    }
	else if (delay < SENSOR_MIN_DELAY) {
		delay = SENSOR_MIN_DELAY;
	}
    else if (delay > SENSOR_MAX_DELAY) {
        delay = SENSOR_MAX_DELAY;
    }
	sensor_set_delay(dev, delay);

	dbg_func_out();

	return count;
}

static ssize_t sensor_enable_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    struct input_dev *inputdev = to_input_dev(dev);
    struct sensor_data *sensordata = input_get_drvdata(inputdev);
    int enable;

	dbg_func_in();

    mutex_lock(&sensordata->enable_mutex);

    enable = sensor_get_enable(dev);

    mutex_unlock(&sensordata->enable_mutex);

	dbg("%s : enable = %d\n", __func__, enable);

	dbg_func_out();

    return sprintf(buf, "%d\n", enable);
}

static ssize_t sensor_enable_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	unsigned long enable = simple_strtoul(buf, NULL, 10);

	dbg_func_in();

	if ((enable == 0) || (enable == 1)) {
		sensor_set_enable(dev, enable);
	}

	dbg_func_out();

	return count;
}

static DEVICE_ATTR(delay, S_IRUGO|S_IWUSR|S_IWGRP,
        sensor_delay_show, sensor_delay_store);
static DEVICE_ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP,
        sensor_enable_show, sensor_enable_store);

static struct attribute *sensor_attributes[] = {
    &dev_attr_delay.attr,
    &dev_attr_enable.attr,
    NULL
};

static struct attribute_group sensor_attribute_group = {
    .attrs = sensor_attributes
};



static int sensor_measure(struct sensor_data *data, axes_t *val)
{
	dbg_func_in();

	return apds9900_ls_measure(val);
}

static void sensor_work_func(struct work_struct *work)
{
	struct sensor_data *sensordata = container_of((struct delayed_work *)work, struct sensor_data, work);
	axes_t val;
	unsigned long delay;

	dbg_func_in();

	if(input_pdev == NULL) {
		cancel_delayed_work_sync(&sensordata->work);
	} else {
		sensor_measure(sensordata, &val);

		input_report_abs(input_pdev, ABS_X, (int)val.x);
		input_report_abs(input_pdev, ABS_Y, (int)val.y);
		input_report_abs(input_pdev, ABS_Z, (int)val.z);
		input_sync(input_pdev);

		mutex_lock(&sensordata->data_mutex);
		sensordata->lastval = val;
		mutex_unlock(&sensordata->data_mutex);

		delay = delay_to_jiffies(atomic_read(&sensordata->delay));
		schedule_delayed_work(&sensordata->work, delay);
	}

	dbg_func_out();
}

static int sensor_suspend(struct platform_device *pdev, pm_message_t state)
{
	dbg_func_in();

    /* implement suspend of the sensor */

	dbg_func_out();

    return 0;
}

static int sensor_resume(struct platform_device *pdev)
{
	dbg_func_in();

    /* implement resume of the sensor */
    
	dbg_func_out();

    return 0;
}

static int sensor_probe(struct platform_device *pdev)
{
    struct sensor_data *sensordata = NULL;
    struct input_dev *inputdev = NULL;
    int input_registered = 0, sysfs_created = 0;
    int rt;

	dbg_func_in();

    sensordata = kzalloc(sizeof(struct sensor_data), GFP_KERNEL);
    if (!sensordata) {
        rt = -ENOMEM;
        goto err;
    }

	atomic_set(&sensordata->enable, 0);
	atomic_set(&sensordata->delay, SENSOR_DEFAULT_DELAY);
	sensordata->status = 0;

    inputdev = input_allocate_device();
    if (!inputdev) {
        rt = -ENOMEM;
        printk(KERN_ERR "sensor_probe: Failed to allocate input_data device\n");
        goto err;
    }

	inputdev->name = SENSOR_INPUTDEV_NAME;
	input_set_capability(inputdev, EV_ABS, ABS_MISC);
	input_set_abs_params(inputdev, ABS_X, SENSOR_MIN_ABS, SENSOR_MAX_ABS, 0, 0);
	input_set_abs_params(inputdev, ABS_Y, SENSOR_MIN_ABS, SENSOR_MAX_ABS, 0, 0);
	input_set_abs_params(inputdev, ABS_Z, SENSOR_MIN_ABS, SENSOR_MAX_ABS, 0, 0);

    rt = input_register_device(inputdev);
    if (rt) {
        printk(KERN_ERR "sensor_probe: Unable to register input_data device: %s\n", inputdev->name);
        goto err;
    }
    input_set_drvdata(inputdev, sensordata);
    input_registered = 1;

    rt = sysfs_create_group(&inputdev->dev.kobj, &sensor_attribute_group);
    if (rt) {
        printk(KERN_ERR "sensor_probe: sysfs_create_group failed[%s]\n", inputdev->name);
        goto err;
    }
    sysfs_created = 1;

	mutex_init(&sensordata->enable_mutex);
	mutex_init(&sensordata->data_mutex);
	
	INIT_DELAYED_WORK(&sensordata->work, sensor_work_func);

	input_pdev = inputdev;

	dbg_func_out();

    return 0;

err:
    if (sensordata != NULL) {
        if (inputdev != NULL) {
            if (sysfs_created) {
                sysfs_remove_group(&inputdev->dev.kobj,
                        &sensor_attribute_group);
            }
            if (input_registered) {
                input_unregister_device(inputdev);
            }
            else {
                input_free_device(inputdev);
            }
            inputdev = NULL;
        }
        kfree(sensordata);
    }

	dbg_func_out();

    return rt;
}

static int sensor_remove(struct platform_device *pdev)
{
    struct sensor_data *sensordata;

	dbg_func_in();

    if (input_pdev != NULL) {
        sensordata = input_get_drvdata(input_pdev);
        sysfs_remove_group(&input_pdev->dev.kobj, &sensor_attribute_group);
        input_unregister_device(input_pdev);
        if (sensordata != NULL) {
            kfree(sensordata);
        }
    }

dbg_func_out();

    return 0;
}

/*
 * Module init and exit
 */
static struct platform_driver sensor_driver = {
    .probe      = sensor_probe,
    .remove     = sensor_remove,
    .suspend    = sensor_suspend,
    .resume     = sensor_resume,
    .driver = {
        .name   = SENSOR_INPUTDEV_NAME,
        .owner  = THIS_MODULE,
    },
};

static int __init sensor_init(void)
{
	dbg_func_in();

    sensor_pdev = platform_device_register_simple(SENSOR_INPUTDEV_NAME, 0, NULL, 0);
    if (IS_ERR(sensor_pdev)) {
        return -1;
    }

	dbg_func_out();
	
    return platform_driver_register(&sensor_driver);
}
module_init(sensor_init);

static void __exit sensor_exit(void)
{
	dbg_func_in();

    platform_driver_unregister(&sensor_driver);
    platform_device_unregister(sensor_pdev);

	dbg_func_out();

}
module_exit(sensor_exit);

MODULE_AUTHOR("Pantech Corporation");
MODULE_LICENSE( "GPL" );
MODULE_VERSION("1.1.0");
