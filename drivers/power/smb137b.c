/* Copyright (c) 2010-2011 Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 *
 */

#include <linux/i2c.h>
#include <linux/gpio.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/module.h>
#include <linux/debugfs.h>
#include <linux/workqueue.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/i2c/smb137b.h>
#include <linux/power_supply.h>
#include <linux/msm-charger.h>

#if 0
//pz1946 20111018 test usb-detect
#include <linux/usb/composite.h>
#endif

#define SMB137B_MASK(BITS, POS)  ((unsigned char)(((1 << BITS) - 1) << POS))

#define CHG_CURRENT_REG		0x00
#define FAST_CHG_CURRENT_MASK		SMB137B_MASK(3, 5)
#define PRE_CHG_CURRENT_MASK		SMB137B_MASK(2, 3)
#define TERM_CHG_CURRENT_MASK		SMB137B_MASK(2, 1)

#define INPUT_CURRENT_LIMIT_REG	0x01
#define IN_CURRENT_MASK			SMB137B_MASK(3, 5)
#define IN_CURRENT_LIMIT_EN_BIT		BIT(2)
#define IN_CURRENT_DET_THRESH_MASK	SMB137B_MASK(2, 0)

#define FLOAT_VOLTAGE_REG	0x02
#define STAT_OUT_POLARITY_BIT		BIT(7)
#define FLOAT_VOLTAGE_MASK		SMB137B_MASK(7, 0)

#define CONTROL_A_REG		0x03
#define AUTO_RECHARGE_DIS_BIT		BIT(7)
#define CURR_CYCLE_TERM_BIT		BIT(6)
#define PRE_TO_FAST_V_MASK		SMB137B_MASK(3, 3)
#define TEMP_BEHAV_BIT			BIT(2)
#define THERM_NTC_CURR_MODE_BIT		BIT(1)
#define THERM_NTC_47KOHM_BIT		BIT(0)

#define CONTROL_B_REG		0x04
#define STAT_OUTPUT_MODE_MASK		SMB137B_MASK(2, 6)
#define BATT_OV_ENDS_CYCLE_BIT		BIT(5)
#define AUTO_PRE_TO_FAST_DIS_BIT	BIT(4)
#define SAFETY_TIMER_EN_BIT		BIT(3)
#define OTG_LBR_WD_EN_BIT		BIT(2)
#define CHG_WD_TIMER_EN_BIT		BIT(1)
#define IRQ_OP_MASK			BIT(0)

#define PIN_CTRL_REG		0x05
#define AUTO_CHG_EN_BIT			BIT(7)
#define AUTO_LBR_EN_BIT			BIT(6)
#define OTG_LBR_BIT			BIT(5)
#define I2C_PIN_BIT			BIT(4)
#define PIN_EN_CTRL_MASK		SMB137B_MASK(2, 2)
#define OTG_LBR_PIN_CTRL_MASK		SMB137B_MASK(2, 0)

#define OTG_LBR_CTRL_REG	0x06
#define BATT_MISSING_DET_EN_BIT		BIT(7)
#define AUTO_RECHARGE_THRESH_MASK	BIT(6)
#define USB_DP_DN_DET_EN_MASK		BIT(5)
#define OTG_LBR_BATT_CURRENT_LIMIT_MASK	SMB137B_MASK(2, 3)
#define OTG_LBR_UVLO_THRESH_MASK	SMB137B_MASK(3, 0)

#define FAULT_INTR_REG		0x07
#define SAFETY_TIMER_EXP_MASK		SMB137B_MASK(1, 7)
#define BATT_TEMP_UNSAFE_MASK		SMB137B_MASK(1, 6)
#define INPUT_OVLO_IVLO_MASK		SMB137B_MASK(1, 5)
#define BATT_OVLO_MASK			SMB137B_MASK(1, 4)
#define INTERNAL_OVER_TEMP_MASK		SMB137B_MASK(1, 2)
#define ENTER_TAPER_CHG_MASK		SMB137B_MASK(1, 1)
#define CHG_MASK			SMB137B_MASK(1, 0)

#define CELL_TEMP_MON_REG	0x08
#define THERMISTOR_CURR_MASK		SMB137B_MASK(2, 6)
#define LOW_TEMP_CHG_INHIBIT_MASK	SMB137B_MASK(3, 3)
#define HIGH_TEMP_CHG_INHIBIT_MASK	SMB137B_MASK(3, 0)

#define	SAFETY_TIMER_THERMAL_SHUTDOWN_REG	0x09
#define DCIN_OVLO_SEL_MASK		SMB137B_MASK(2, 7)
#define RELOAD_EN_INPUT_VOLTAGE_MASK	SMB137B_MASK(1, 6)
#define THERM_SHUTDN_EN_MASK		SMB137B_MASK(1, 5)
#define STANDBY_WD_TIMER_EN_MASK		SMB137B_MASK(1, 4)
#define COMPLETE_CHG_TMOUT_MASK		SMB137B_MASK(2, 2)
#define PRE_CHG_TMOUT_MASK		SMB137B_MASK(2, 0)

#define	VSYS_REG	0x0A
#define	VSYS_MASK			SMB137B_MASK(3, 4)

#define IRQ_RESET_REG	0x30

#define COMMAND_A_REG	0x31
#define	VOLATILE_REGS_WRITE_PERM_BIT	BIT(7)
#define	POR_BIT				BIT(6)
#define	FAST_CHG_SETTINGS_BIT		BIT(5)
#define	BATT_CHG_EN_BIT			BIT(4)
#define	USBIN_MODE_500_BIT		BIT(3)
#define	USBIN_MODE_HCMODE_BIT		BIT(2)
#define	OTG_LBR_EN_BIT			BIT(1)
#define	STAT_OE_BIT			BIT(0)

#define STATUS_A_REG	0x32
#define INTERNAL_TEMP_IRQ_STAT		BIT(4)
#define DCIN_OV_IRQ_STAT		BIT(3)
#define DCIN_UV_IRQ_STAT		BIT(2)
#define USBIN_OV_IRQ_STAT		BIT(1)
#define USBIN_UV_IRQ_STAT		BIT(0)

#define STATUS_B_REG	0x33
#define USB_PIN_STAT			BIT(7)
#define USB51_MODE_STAT			BIT(6)
#define USB51_HC_MODE_STAT		BIT(5)
#define INTERNAL_TEMP_LIMIT_B_STAT	BIT(4)
#define DC_IN_OV_STAT			BIT(3)
#define DC_IN_UV_STAT			BIT(2)
#define USB_IN_OV_STAT			BIT(1)
#define USB_IN_UV_STAT			BIT(0)

#define	STATUS_C_REG	0x34
#define AUTO_IN_CURR_LIMIT_MASK		SMB137B_MASK(4, 4)
#define AUTO_IN_CURR_LIMIT_STAT		BIT(3)
#define AUTO_SOURCE_DET_COMP_STAT_MASK	SMB137B_MASK(2, 1)
#define AUTO_SOURCE_DET_RESULT_STAT	BIT(0)

#define	STATUS_D_REG	0x35
#define	VBATT_LESS_THAN_VSYS_STAT	BIT(7)
#define	USB_FAIL_STAT			BIT(6)
#define	BATT_TEMP_STAT_MASK		SMB137B_MASK(2, 4)
#define	INTERNAL_TEMP_LIMIT_STAT	BIT(2)
#define	OTG_LBR_MODE_EN_STAT		BIT(1)
#define	OTG_LBR_VBATT_UVLO_STAT		BIT(0)

#define	STATUS_E_REG	0x36
#define	CHARGE_CYCLE_COUNT_STAT		BIT(7)
#define	CHARGER_TERM_STAT		BIT(6)
#define	SAFETY_TIMER_STAT_MASK		SMB137B_MASK(2, 4)
#define	CHARGER_ERROR_STAT		BIT(3)
#define	CHARGING_STAT_E			SMB137B_MASK(2, 1)
#define	CHARGING_EN			BIT(0)

#define	STATUS_F_REG	0x37
#define	WD_IRQ_ACTIVE_STAT		BIT(7)
#define	OTG_OVERCURRENT_STAT		BIT(6)
#define	BATT_PRESENT_STAT		BIT(4)
#define	BATT_OV_LATCHED_STAT		BIT(3)
#define	CHARGER_OVLO_STAT		BIT(2)
#define	CHARGER_UVLO_STAT		BIT(1)
#define	BATT_LOW_STAT			BIT(0)

#define	STATUS_G_REG	0x38
#define	CHARGE_TIMEOUT_IRQ_STAT		BIT(7)
#define	PRECHARGE_TIMEOUT_IRQ_STAT	BIT(6)
#define	BATT_HOT_IRQ_STAT		BIT(5)
#define	BATT_COLD_IRQ_STAT		BIT(4)
#define	BATT_OV_IRQ_STAT		BIT(3)
#define	TAPER_CHG_IRQ_STAT		BIT(2)
#define	FAST_CHG_IRQ_STAT		BIT(1)
#define	CHARGING_IRQ_STAT		BIT(0)

#define	STATUS_H_REG	0x39
#define	CHARGE_TIMEOUT_STAT		BIT(7)
#define	PRECHARGE_TIMEOUT_STAT		BIT(6)
#define	BATT_HOT_STAT			BIT(5)
#define	BATT_COLD_STAT			BIT(4)
#define	BATT_OV_STAT			BIT(3)
#define	TAPER_CHG_STAT			BIT(2)
#define	FAST_CHG_STAT			BIT(1)
#define	CHARGING_STAT_H			BIT(0)

#define DEV_ID_REG	0x3B

#define COMMAND_B_REG	0x3C
#define	THERM_NTC_CURR_VERRIDE		BIT(7)

#define SMB137B_CHG_PERIOD	((HZ) * 150)

#define INPUT_CURRENT_REG_DEFAULT	0xE1
#define INPUT_CURRENT_REG_MIN		0x01
#define	COMMAND_A_REG_DEFAULT		0xA0
#define	COMMAND_A_REG_OTG_MODE		0xA2

#define	PIN_CTRL_REG_DEFAULT		0x00
#define	PIN_CTRL_REG_CHG_OFF		0x04

#define	FAST_CHG_E_STATUS 0x2

#define SMB137B_DEFAULT_BATT_RATING   950
struct smb137b_data {
	struct i2c_client *client;
	struct delayed_work charge_work;

	bool charging;
	int chgcurrent;
	int cur_charging_mode;
	int max_system_voltage;
	int min_system_voltage;

	int valid_n_gpio;

	int batt_status;
	int batt_chg_type;
	int batt_present;
	int min_design;
	int max_design;
	int batt_mah_rating;

	int usb_status;

	u8 dev_id_reg;
	struct msm_hardware_charger adapter_hw_chg;
};

static unsigned int disabled;
enum charger_stat {
	SMB137B_ABSENT,
	SMB137B_PRESENT,
	SMB137B_ENUMERATED,
};

static struct smb137b_data *usb_smb137b_chg;

#ifdef CONFIG_SKY_SMB_CHARGER
extern int pm8058_is_factory_cable(void);
//pz1946
int smb_charger_state = 0;
#endif

static int smb137b_read_reg(struct i2c_client *client, int reg,
	u8 *val)
{
	s32 ret;
	struct smb137b_data *smb137b_chg;

	smb137b_chg = i2c_get_clientdata(client);
	ret = i2c_smbus_read_byte_data(smb137b_chg->client, reg);
	if (ret < 0) {
		dev_err(&smb137b_chg->client->dev,
			"i2c read fail: can't read from %02x: %d\n", reg, ret);
		return ret;
	} else
		*val = ret;

	return 0;
}

static int smb137b_write_reg(struct i2c_client *client, int reg,
	u8 val)
{
	s32 ret;
	struct smb137b_data *smb137b_chg;

	smb137b_chg = i2c_get_clientdata(client);
	ret = i2c_smbus_write_byte_data(smb137b_chg->client, reg, val);
	if (ret < 0) {
		dev_err(&smb137b_chg->client->dev,
			"i2c write fail: can't write %02x to %02x: %d\n",
			val, reg, ret);
		return ret;
	}
	return 0;
}

static ssize_t id_reg_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	struct smb137b_data *smb137b_chg;

	smb137b_chg = i2c_get_clientdata(to_i2c_client(dev));

	return sprintf(buf, "%02x\n", smb137b_chg->dev_id_reg);
}

#ifdef CONFIG_SKY_SMB_CHARGER
static ssize_t status_b_reg_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	int ret;
	u8 temp;
	struct smb137b_data *smb137b_chg;

	smb137b_chg = i2c_get_clientdata(to_i2c_client(dev));

	ret = smb137b_read_reg(smb137b_chg->client, STATUS_B_REG, &temp);

	return sprintf(buf, "%s\n", (temp & USB51_HC_MODE_STAT) ? "HC mode" : "USB 5/1 mode");
}

static ssize_t status_c_reg_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	int ret;
	u8 temp;
	struct smb137b_data *smb137b_chg;

	smb137b_chg = i2c_get_clientdata(to_i2c_client(dev));

	ret = smb137b_read_reg(smb137b_chg->client, STATUS_C_REG, &temp);

        temp = ((temp & AUTO_IN_CURR_LIMIT_MASK) >> 4);

        if(temp == 0)
        	return sprintf(buf, "%s\n", "700mA");
        else if(temp == 1)
        	return sprintf(buf, "%s\n", "800mA");
        else if(temp == 2)
        	return sprintf(buf, "%s\n", "900mA");
        else if(temp == 3)
        	return sprintf(buf, "%s\n", "1000mA");
        else if(temp == 4)
        	return sprintf(buf, "%s\n", "1100mA");
        else if(temp == 5)
        	return sprintf(buf, "%s\n", "1200mA");
        else if(temp == 6)
        	return sprintf(buf, "%s\n", "1300mA");
        else if(temp == 7)
        	return sprintf(buf, "%s\n", "1400mA");
        else if(temp == 14)
        	return sprintf(buf, "%s\n", "275mA");
        else if(temp == 15)
        	return sprintf(buf, "%s\n", "500mA");
        else
        	return sprintf(buf, "%s\n", "ERROR");

}

static ssize_t status_e_reg_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	int ret;
	u8 temp;
	struct smb137b_data *smb137b_chg;

	smb137b_chg = i2c_get_clientdata(to_i2c_client(dev));

	ret = smb137b_read_reg(smb137b_chg->client, STATUS_E_REG, &temp);

        temp = ((temp & CHARGING_STAT_E) >> 1);

        if(temp == 0)
        	return sprintf(buf, "%s\n", "Not Charging");
        else if(temp == 1)
        	return sprintf(buf, "%s\n", "Pre Charging");
        else if(temp == 2)
        	return sprintf(buf, "%s\n", "Fast Charging");
        else if(temp == 3)
        	return sprintf(buf, "%s\n", "Taper Charging");
        else
        	return sprintf(buf, "%s\n", "ERROR");
        
}

static ssize_t status_f_reg_show(struct device *dev, struct device_attribute *attr,
		char *buf)
{
	int ret;
	u8 temp;
	struct smb137b_data *smb137b_chg;

	smb137b_chg = i2c_get_clientdata(to_i2c_client(dev));

	ret = smb137b_read_reg(smb137b_chg->client, STATUS_F_REG, &temp);

	return sprintf(buf, "%s\n", (temp & BATT_LOW_STAT) ? "AUXPWR < 2.1V" : "AUXPWR > 2.1V");
}
#endif

static DEVICE_ATTR(id_reg, S_IRUGO | S_IWUSR, id_reg_show, NULL);
#ifdef CONFIG_SKY_SMB_CHARGER
static DEVICE_ATTR(status_b_reg, S_IRUGO | S_IWUSR, status_b_reg_show, NULL);
static DEVICE_ATTR(status_c_reg, S_IRUGO | S_IWUSR, status_c_reg_show, NULL);
static DEVICE_ATTR(status_e_reg, S_IRUGO | S_IWUSR, status_e_reg_show, NULL);
static DEVICE_ATTR(status_f_reg, S_IRUGO | S_IWUSR, status_f_reg_show, NULL);

static struct attribute *smb137b_attrs[] = {
	&dev_attr_status_b_reg.attr,
	&dev_attr_status_c_reg.attr,
	&dev_attr_status_e_reg.attr,
	&dev_attr_status_f_reg.attr,
	NULL
};

static const struct attribute_group smb137b_sysfs_files = {
	.name	= "smb137b_status",
	.attrs	= smb137b_attrs,
};
#endif

#ifdef DEBUG
static void smb137b_dbg_print_status_regs(struct smb137b_data *smb137b_chg)
{
	int ret;
	u8 temp;

	ret = smb137b_read_reg(smb137b_chg->client, STATUS_A_REG, &temp);
	dev_dbg(&smb137b_chg->client->dev, "%s A=0x%x\n", __func__, temp);
	ret = smb137b_read_reg(smb137b_chg->client, STATUS_B_REG, &temp);
	dev_dbg(&smb137b_chg->client->dev, "%s B=0x%x\n", __func__, temp);
	ret = smb137b_read_reg(smb137b_chg->client, STATUS_C_REG, &temp);
	dev_dbg(&smb137b_chg->client->dev, "%s C=0x%x\n", __func__, temp);
	ret = smb137b_read_reg(smb137b_chg->client, STATUS_D_REG, &temp);
	dev_dbg(&smb137b_chg->client->dev, "%s D=0x%x\n", __func__, temp);
	ret = smb137b_read_reg(smb137b_chg->client, STATUS_E_REG, &temp);
	dev_dbg(&smb137b_chg->client->dev, "%s E=0x%x\n", __func__, temp);
	ret = smb137b_read_reg(smb137b_chg->client, STATUS_F_REG, &temp);
	dev_dbg(&smb137b_chg->client->dev, "%s F=0x%x\n", __func__, temp);
	ret = smb137b_read_reg(smb137b_chg->client, STATUS_G_REG, &temp);
	dev_dbg(&smb137b_chg->client->dev, "%s G=0x%x\n", __func__, temp);
	ret = smb137b_read_reg(smb137b_chg->client, STATUS_H_REG, &temp);
	dev_dbg(&smb137b_chg->client->dev, "%s H=0x%x\n", __func__, temp);
}
#else
static void smb137b_dbg_print_status_regs(struct smb137b_data *smb137b_chg)
{
}
#endif

static int smb137b_start_charging(struct msm_hardware_charger *hw_chg,
		int chg_voltage, int chg_current)
{
	int cmd_val = COMMAND_A_REG_DEFAULT;
	u8 temp = 0;
	int ret = 0;
#if 0
//pz1946 20111019 add 
	int real_usb_detect = 0;
	int i;
#endif
	struct smb137b_data *smb137b_chg;

	smb137b_chg = container_of(hw_chg, struct smb137b_data, adapter_hw_chg);
	if (disabled) {
		dev_err(&smb137b_chg->client->dev,
			"%s called when disabled\n", __func__);
		goto out;
	}

	if (smb137b_chg->charging == true
		&& smb137b_chg->chgcurrent == chg_current)
		/* we are already charging with the same current*/
		 dev_err(&smb137b_chg->client->dev,
			 "%s charge with same current %d called again\n",
			  __func__, chg_current);

#ifdef CONFIG_SKY_SMB_CHARGER
	dev_info(&smb137b_chg->client->dev, "[SKY CHG]%s, chg_current = %d\n", __func__, chg_current);
#else
	dev_dbg(&smb137b_chg->client->dev, "%s\n", __func__);
#endif
//pz1946 20110902 usb ac display
#ifdef CONFIG_SKY_SMB_CHARGER
	if (chg_current < 500) {
		cmd_val &= ~USBIN_MODE_500_BIT;
		smb_charger_state = CHG_TYPE_NONE;
	}
	else if (chg_current == 500) {
//pz1946 20111018 usb-detect
//printk("~~~~~pz1946 smb usb detect first check real_usb_detect = %d\n", real_usb_detect);
//pz1946 test 20111019
#if 0
		for(i = 0; i < 5; i++) {
			real_usb_detect = composite_get_udc_state();
			if (real_usb_detect != 1) {
				msleep(1000);
			}
			else {
				break;
			}
		}

//		real_usb_detect = composite_get_udc_state();
		if(real_usb_detect) {
			cmd_val |= USBIN_MODE_500_BIT;
			smb_charger_state = CHG_TYPE_USB;
		}
		else {
			cmd_val |= USBIN_MODE_HCMODE_BIT;
			smb_charger_state = CHG_TYPE_AC;
		}
#else
		cmd_val |= USBIN_MODE_500_BIT;
		smb_charger_state = CHG_TYPE_USB;
#endif
	}
	else {	
		cmd_val |= USBIN_MODE_HCMODE_BIT;
		smb_charger_state = CHG_TYPE_AC;
	}

//#ifdef CONFIG_SKY_SMB_CHARGER
        if(pm8058_is_factory_cable()) {
		cmd_val |= USBIN_MODE_HCMODE_BIT;
		smb_charger_state = CHG_TYPE_FACTORY;
	}
#endif

	smb137b_chg->chgcurrent = chg_current;
	smb137b_chg->cur_charging_mode = cmd_val;

	/* Due to non-volatile reload feature,always enable volatile
	 * mirror writes before modifying any 00h~09h control register.
	 * Current mode needs to be programmed according to what's detected
	 * Otherwise default 100mA mode might cause VOUTL drop and fail
	 * the system in case of dead battery.
	 */
	ret = smb137b_write_reg(smb137b_chg->client,
					COMMAND_A_REG, cmd_val);
	if (ret) {
		dev_err(&smb137b_chg->client->dev,
			"%s couldn't write to command_reg\n", __func__);
		goto out;
	}
	ret = smb137b_write_reg(smb137b_chg->client,
					PIN_CTRL_REG, PIN_CTRL_REG_DEFAULT);
	if (ret) {
		dev_err(&smb137b_chg->client->dev,
			"%s couldn't write to pin ctrl reg\n", __func__);
		goto out;
	}
//pz1946 36hr dischager status
        ret = smb137b_write_reg(smb137b_chg->client,
                      SAFETY_TIMER_THERMAL_SHUTDOWN_REG, 0x4F);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't write to safty timer thermal shutdown\n", __func__);
                goto out;
	}
//pz1946 20111018 usb detect
#if defined(CONFIG_MACH_MSM8X60_EF39S) || defined(CONFIG_MACH_MSM8X60_EF40S) || defined(CONFIG_MACH_MSM8X60_EF40K)
	msleep(2000);

	if(smb_charger_state == CHG_TYPE_AC) {
                ret = smb137b_read_reg(smb137b_chg->client, STATUS_C_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%d \n", __func__,
                STATUS_C_REG, temp);

        	temp = ((temp & AUTO_IN_CURR_LIMIT_MASK) >> 4);
        	if (temp == 14 || temp == 15 || temp == 0) {
			ret = smb137b_write_reg(smb137b_chg->client,
                      	INPUT_CURRENT_LIMIT_REG, 0x04);
        		if(ret < 0)
        			dev_err(&smb137b_chg->client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, INPUT_CURRENT_LIMIT_REG, ret);
		}
		else if (temp == 1) {
			ret = smb137b_write_reg(smb137b_chg->client,
                      	INPUT_CURRENT_LIMIT_REG, 0x24);
        		if(ret < 0)
        			dev_err(&smb137b_chg->client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, INPUT_CURRENT_LIMIT_REG, ret);
		}
		else if (temp == 2) {
			ret = smb137b_write_reg(smb137b_chg->client,
                      	INPUT_CURRENT_LIMIT_REG, 0x44);
        		if(ret < 0)
        			dev_err(&smb137b_chg->client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, INPUT_CURRENT_LIMIT_REG, ret);
		}
		else if (temp == 3 || temp == 4 || temp == 5 || temp == 6 || temp == 7) {
			ret = smb137b_write_reg(smb137b_chg->client,
                      	INPUT_CURRENT_LIMIT_REG, 0x64);
        		if(ret < 0)
        			dev_err(&smb137b_chg->client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, INPUT_CURRENT_LIMIT_REG, ret);
		}
		else {
			dev_err(&smb137b_chg->client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, INPUT_CURRENT_LIMIT_REG, ret);
		}
	}
	                
	
#endif
	smb137b_chg->charging = true;
	smb137b_chg->batt_status = POWER_SUPPLY_STATUS_CHARGING;
	smb137b_chg->batt_chg_type = POWER_SUPPLY_CHARGE_TYPE_TRICKLE;

	ret = smb137b_read_reg(smb137b_chg->client, STATUS_E_REG, &temp);
	if (ret) {
		dev_err(&smb137b_chg->client->dev,
			"%s couldn't read status e reg %d\n", __func__, ret);
	} else {
		if (temp & CHARGER_ERROR_STAT) {
			dev_err(&smb137b_chg->client->dev,
				"%s chg error E=0x%x\n", __func__, temp);
			smb137b_dbg_print_status_regs(smb137b_chg);
		}
		if (((temp & CHARGING_STAT_E) >> 1) >= FAST_CHG_E_STATUS)
#ifdef CONFIG_SKY_SMB_CHARGER
                {      
#endif
			smb137b_chg->batt_chg_type
						= POWER_SUPPLY_CHARGE_TYPE_FAST;
#ifdef CONFIG_SKY_SMB_CHARGER
			msm_charger_notify_event(
				&smb137b_chg->adapter_hw_chg,
				CHG_BATT_BEGIN_FAST_CHARGING);
                }
#endif
	}
	/*schedule charge_work to keep track of battery charging state*/
	schedule_delayed_work(&smb137b_chg->charge_work, SMB137B_CHG_PERIOD);

out:
	return ret;
}

static int smb137b_stop_charging(struct msm_hardware_charger *hw_chg)
{
	int ret = 0;
	struct smb137b_data *smb137b_chg;

	smb137b_chg = container_of(hw_chg, struct smb137b_data, adapter_hw_chg);

	dev_dbg(&smb137b_chg->client->dev, "%s\n", __func__);
	if (smb137b_chg->charging == false)
		return 0;

	smb137b_chg->charging = false;
	smb137b_chg->batt_status = POWER_SUPPLY_STATUS_DISCHARGING;
	smb137b_chg->batt_chg_type = POWER_SUPPLY_CHARGE_TYPE_NONE;
//pz1946 20110919 charger interrupt state change
#ifdef CONFIG_SKY_SMB_CHARGER
	smb_charger_state = CHG_TYPE_NONE;
#endif //Do not use at PRESTO
//pz1946 20110907 interrupt pin change
#if 0 //because if charger is unpluged then charger IC reset 
	ret = smb137b_write_reg(smb137b_chg->client, COMMAND_A_REG,
				smb137b_chg->cur_charging_mode);
	if (ret) {
		dev_err(&smb137b_chg->client->dev,
			"%s couldn't write to command_reg\n", __func__);
		goto out;
	}

	ret = smb137b_write_reg(smb137b_chg->client,
					PIN_CTRL_REG, PIN_CTRL_REG_CHG_OFF);
	if (ret)
		dev_err(&smb137b_chg->client->dev,
			"%s couldn't write to pin ctrl reg\n", __func__);

out:
#endif
	return ret;
}

static int smb137b_charger_switch(struct msm_hardware_charger *hw_chg)
{
	struct smb137b_data *smb137b_chg;

	smb137b_chg = container_of(hw_chg, struct smb137b_data, adapter_hw_chg);
	dev_dbg(&smb137b_chg->client->dev, "%s\n", __func__);
	return 0;
}

static irqreturn_t smb137b_valid_handler(int irq, void *dev_id)
{
	int val;
	struct smb137b_data *smb137b_chg;
	struct i2c_client *client = dev_id;

	smb137b_chg = i2c_get_clientdata(client);

	pr_debug("%s Cable Detected USB inserted\n", __func__);
	/*extra delay needed to allow CABLE_DET_N settling down and debounce
	 before	trying to sample its correct value*/
#ifdef CONFIG_SKY_SMB_CHARGER
        msleep(300);
#else
	usleep_range(1000, 1200);
#endif
	val = gpio_get_value_cansleep(smb137b_chg->valid_n_gpio);
	if (val < 0) {
		dev_err(&smb137b_chg->client->dev,
			"%s gpio_get_value failed for %d ret=%d\n", __func__,
			smb137b_chg->valid_n_gpio, val);
		goto err;
	}
#ifdef CONFIG_SKY_SMB_CHARGER
	dev_info(&smb137b_chg->client->dev, "[SKY CHG]%s: gpio =%d, val=%d\n", __func__, smb137b_chg->valid_n_gpio, val);
#else
	dev_dbg(&smb137b_chg->client->dev, "%s val=%d\n", __func__, val);
#endif
	if (val) {
		if (smb137b_chg->usb_status != SMB137B_ABSENT) {
			smb137b_chg->usb_status = SMB137B_ABSENT;
			msm_charger_notify_event(&smb137b_chg->adapter_hw_chg,
					CHG_REMOVED_EVENT);
		}
	} else {
		if (smb137b_chg->usb_status == SMB137B_ABSENT) {
			smb137b_chg->usb_status = SMB137B_PRESENT;
			msm_charger_notify_event(&smb137b_chg->adapter_hw_chg,
					CHG_INSERTED_EVENT);
		}
	}
err:
	return IRQ_HANDLED;
}

#ifdef CONFIG_DEBUG_FS
static struct dentry *dent;
static int debug_fs_otg;
static int otg_get(void *data, u64 *value)
{
	*value = debug_fs_otg;
	return 0;
}
static int otg_set(void *data, u64 value)
{
	smb137b_otg_power(debug_fs_otg);
	return 0;
}
DEFINE_SIMPLE_ATTRIBUTE(smb137b_otg_fops, otg_get, otg_set, "%llu\n");

static void smb137b_create_debugfs_entries(struct smb137b_data *smb137b_chg)
{
	dent = debugfs_create_dir("smb137b", NULL);
	if (dent) {
		debugfs_create_file("otg", 0644, dent, NULL, &smb137b_otg_fops);
	}
}
static void smb137b_destroy_debugfs_entries(void)
{
	if (dent)
		debugfs_remove_recursive(dent);
}
#else
static void smb137b_create_debugfs_entries(struct smb137b_data *smb137b_chg)
{
}
static void smb137b_destroy_debugfs_entries(void)
{
}
#endif

static int set_disable_status_param(const char *val, struct kernel_param *kp)
{
	int ret;

	ret = param_set_int(val, kp);
	if (ret)
		return ret;

	if (usb_smb137b_chg && disabled)
		msm_charger_notify_event(&usb_smb137b_chg->adapter_hw_chg,
				CHG_DONE_EVENT);

	pr_debug("%s disabled =%d\n", __func__, disabled);
	return 0;
}
module_param_call(disabled, set_disable_status_param, param_get_uint,
					&disabled, 0644);
static void smb137b_charge_sm(struct work_struct *smb137b_work)
{
	int ret;
	struct smb137b_data *smb137b_chg;
	u8 temp = 0;
	int notify_batt_changed = 0;
#ifdef CONFIG_SKY_SMB_CHARGER
    	char buf[16];
#endif
	smb137b_chg = container_of(smb137b_work, struct smb137b_data,
			charge_work.work);

	/*if not charging, exit smb137b charging state transition*/
	if (!smb137b_chg->charging)
		return;

	dev_dbg(&smb137b_chg->client->dev, "%s\n", __func__);

	ret = smb137b_read_reg(smb137b_chg->client, STATUS_F_REG, &temp);
	if (ret) {
		dev_err(&smb137b_chg->client->dev,
			"%s couldn't read status f reg %d\n", __func__, ret);
		goto out;
	}
	if (smb137b_chg->batt_present != !(temp & BATT_PRESENT_STAT)) {
		smb137b_chg->batt_present = !(temp & BATT_PRESENT_STAT);
		notify_batt_changed = 1;
	}

	if (!smb137b_chg->batt_present)
		smb137b_chg->batt_chg_type = POWER_SUPPLY_CHARGE_TYPE_NONE;

	if (!smb137b_chg->batt_present && smb137b_chg->charging)
		msm_charger_notify_event(&smb137b_chg->adapter_hw_chg,
				CHG_DONE_EVENT);

	if (smb137b_chg->batt_present
		&& smb137b_chg->charging
		&& smb137b_chg->batt_chg_type
			!= POWER_SUPPLY_CHARGE_TYPE_FAST) {
		ret = smb137b_read_reg(smb137b_chg->client,
						STATUS_E_REG, &temp);
		if (ret) {
			dev_err(&smb137b_chg->client->dev,
				"%s couldn't read cntrl reg\n", __func__);
			goto out;

		} else {
			if (temp & CHARGER_ERROR_STAT) {
				dev_err(&smb137b_chg->client->dev,
					"%s error E=0x%x\n", __func__, temp);
				smb137b_dbg_print_status_regs(smb137b_chg);
			}
			if (((temp & CHARGING_STAT_E) >> 1)
					>= FAST_CHG_E_STATUS) {
				smb137b_chg->batt_chg_type
					= POWER_SUPPLY_CHARGE_TYPE_FAST;
				notify_batt_changed = 1;
				msm_charger_notify_event(
					&smb137b_chg->adapter_hw_chg,
					CHG_BATT_BEGIN_FAST_CHARGING);
			} else {
				smb137b_chg->batt_chg_type
					= POWER_SUPPLY_CHARGE_TYPE_TRICKLE;
			}
		}
	}
#ifdef CONFIG_SKY_SMB_CHARGER
	ret = smb137b_read_reg(smb137b_chg->client,
					STATUS_E_REG, &temp);
        if(ret)
                temp = 0;
        else
                temp = ((temp & CHARGING_STAT_E) >> 1);

        if(temp == 0)
        	sprintf(buf, "%s\n", "Not Charging");
        else if(temp == 1)
        	sprintf(buf, "%s\n", "Pre Charging");
        else if(temp == 2)
        	sprintf(buf, "%s\n", "Fast Charging");
        else if(temp == 3)
        	sprintf(buf, "%s\n", "Taper Charging");
        else
        	sprintf(buf, "%s\n", "ERROR");

	dev_info(&smb137b_chg->client->dev, "[SKY CHG]%s, chg_type=%d, chg_status=%s\n", __func__,smb137b_chg->batt_chg_type, buf);
#endif
//pz1946 timer setting register read test
#if 0 
                ret = smb137b_read_reg(smb137b_chg->client, CHG_CURRENT_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                CHG_CURRENT_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, INPUT_CURRENT_LIMIT_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                INPUT_CURRENT_LIMIT_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, FLOAT_VOLTAGE_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                FLOAT_VOLTAGE_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, CONTROL_A_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                CONTROL_A_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, CONTROL_B_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                CONTROL_B_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, PIN_CTRL_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                PIN_CTRL_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, OTG_LBR_CTRL_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                OTG_LBR_CTRL_REG, temp);


                ret = smb137b_read_reg(smb137b_chg->client, FAULT_INTR_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                FAULT_INTR_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, CELL_TEMP_MON_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                CELL_TEMP_MON_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, SAFETY_TIMER_THERMAL_SHUTDOWN_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                SAFETY_TIMER_THERMAL_SHUTDOWN_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, VSYS_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                VSYS_REG, temp);


                ret = smb137b_read_reg(smb137b_chg->client, IRQ_RESET_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                IRQ_RESET_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, COMMAND_A_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                COMMAND_A_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, STATUS_A_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                STATUS_A_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, STATUS_B_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                STATUS_B_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, STATUS_C_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                STATUS_C_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, STATUS_D_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                STATUS_D_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, STATUS_E_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                STATUS_E_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, STATUS_F_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                STATUS_F_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, STATUS_G_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                STATUS_G_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, STATUS_H_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                STATUS_H_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, DEV_ID_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                DEV_ID_REG, temp);

                ret = smb137b_read_reg(smb137b_chg->client, COMMAND_B_REG, &temp);
        if (ret) {
                dev_err(&smb137b_chg->client->dev,
                        "%s couldn't read status e reg %d\n", __func__, ret);
        }
        dev_info(&smb137b_chg->client->dev,
                "%s smb137b_read_reg R%x = 0x%x \n", __func__,
                COMMAND_B_REG, temp);

#endif
out:
	schedule_delayed_work(&smb137b_chg->charge_work,
					SMB137B_CHG_PERIOD);
}

static int __devinit smb137b_probe(struct i2c_client *client,
				    const struct i2c_device_id *id)
{
	const struct smb137b_platform_data *pdata;
	struct smb137b_data *smb137b_chg;
	int ret = 0;
//pz1946 EF40K PLM 445 debug 110826
#if defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV == WS20) 
	u8 temp;
#endif

	pdata = client->dev.platform_data;

	if (pdata == NULL) {
		dev_err(&client->dev, "%s no platform data\n", __func__);
		ret = -EINVAL;
		goto out;
	}

#ifdef CONFIG_SKY_SMB_CHARGER
	dev_info(&client->dev, "[SKY CHG]%s start\n", __func__);
#endif

	if (!i2c_check_functionality(client->adapter,
				I2C_FUNC_SMBUS_BYTE_DATA)) {
		ret = -EIO;
		goto out;
	}

	smb137b_chg = kzalloc(sizeof(*smb137b_chg), GFP_KERNEL);
	if (!smb137b_chg) {
		ret = -ENOMEM;
		goto out;
	}

	INIT_DELAYED_WORK(&smb137b_chg->charge_work, smb137b_charge_sm);
	smb137b_chg->client = client;
	smb137b_chg->valid_n_gpio = pdata->valid_n_gpio;
	smb137b_chg->batt_mah_rating = pdata->batt_mah_rating;
	if (smb137b_chg->batt_mah_rating == 0)
		smb137b_chg->batt_mah_rating = SMB137B_DEFAULT_BATT_RATING;

	/*It supports USB-WALL charger and PC USB charger */
	smb137b_chg->adapter_hw_chg.type = CHG_TYPE_USB;
	smb137b_chg->adapter_hw_chg.rating = pdata->batt_mah_rating;
	smb137b_chg->adapter_hw_chg.name = "smb137b-charger";
	smb137b_chg->adapter_hw_chg.start_charging = smb137b_start_charging;
	smb137b_chg->adapter_hw_chg.stop_charging = smb137b_stop_charging;
	smb137b_chg->adapter_hw_chg.charging_switched = smb137b_charger_switch;

	if (pdata->chg_detection_config)
		ret = pdata->chg_detection_config();
	if (ret) {
		dev_err(&client->dev, "%s valid config failed ret=%d\n",
			__func__, ret);
		goto free_smb137b_chg;
	}

	ret = gpio_request(pdata->valid_n_gpio, "smb137b_charger_valid");
	if (ret) {
		dev_err(&client->dev, "%s gpio_request failed for %d ret=%d\n",
			__func__, pdata->valid_n_gpio, ret);
		goto free_smb137b_chg;
	}

	i2c_set_clientdata(client, smb137b_chg);

	ret = msm_charger_register(&smb137b_chg->adapter_hw_chg);
	if (ret) {
		dev_err(&client->dev, "%s msm_charger_register\
			failed for ret=%d\n", __func__, ret);
		goto free_valid_gpio;
	}

	ret = set_irq_wake(client->irq, 1);
	if (ret) {
		dev_err(&client->dev, "%s failed for set_irq_wake %d ret =%d\n",
			 __func__, client->irq, ret);
		goto unregister_charger;
	}

	ret = request_threaded_irq(client->irq, NULL,
				   smb137b_valid_handler,
				   IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING,
				   "smb137b_charger_valid", client);
	if (ret) {
		dev_err(&client->dev,
			"%s request_threaded_irq failed for %d ret =%d\n",
			__func__, client->irq, ret);
		goto disable_irq_wake;
	}

	ret = gpio_get_value_cansleep(smb137b_chg->valid_n_gpio);
	if (ret < 0) {
		dev_err(&client->dev,
			"%s gpio_get_value failed for %d ret=%d\n", __func__,
			pdata->valid_n_gpio, ret);
		/* assume absent */
		ret = 1;
	}
	if (!ret) {
		msm_charger_notify_event(&smb137b_chg->adapter_hw_chg,
			CHG_INSERTED_EVENT);
		smb137b_chg->usb_status = SMB137B_PRESENT;
	}

	ret = smb137b_read_reg(smb137b_chg->client, DEV_ID_REG,
			&smb137b_chg->dev_id_reg);

	ret = device_create_file(&smb137b_chg->client->dev, &dev_attr_id_reg);

#ifdef CONFIG_SKY_SMB_CHARGER
	ret = sysfs_create_group(&smb137b_chg->client->dev.kobj, &smb137b_sysfs_files);
        if(ret)
                dev_err(&client->dev, "%s fail sysfs_create_group, ret = %d\n", __func__,ret);
#endif

	/* TODO read min_design and max_design from chip registers */
	smb137b_chg->min_design = 3200;
#ifdef CONFIG_SKY_SMB_CHARGER
	smb137b_chg->max_design = 4350;
#else
	smb137b_chg->max_design = 4200;
#endif

	smb137b_chg->batt_status = POWER_SUPPLY_STATUS_DISCHARGING;
	smb137b_chg->batt_chg_type = POWER_SUPPLY_CHARGE_TYPE_NONE;

	device_init_wakeup(&client->dev, 1);

	usb_smb137b_chg = smb137b_chg;
	smb137b_create_debugfs_entries(smb137b_chg);
#ifdef CONFIG_SKY_SMB_CHARGER
	dev_info(&client->dev,
		"%s OK device_id = %x chg_state=%d\n", __func__,
		smb137b_chg->dev_id_reg, smb137b_chg->usb_status);
#else
	dev_dbg(&client->dev,
		"%s OK device_id = %x chg_state=%d\n", __func__,
		smb137b_chg->dev_id_reg, smb137b_chg->usb_status);
#endif
//pz1946 EF40K PLM 445 debug 110826
#if defined(CONFIG_MACH_MSM8X60_EF40K) && (BOARD_REV == WS20) 
	dev_info(&client->dev, "%s~~~~pz1946 smb register write start~~~~\n", __func__);
        ret = smb137b_write_reg(smb137b_chg->client,
                        COMMAND_A_REG, COMMAND_A_REG_DEFAULT);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, COMMAND_A_REG,ret);
	mdelay(70);	

        ret = smb137b_write_reg(smb137b_chg->client,
                      CHG_CURRENT_REG, 0xF4);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, CHG_CURRENT_REG, ret);
        ret = smb137b_write_reg(smb137b_chg->client,
                      INPUT_CURRENT_LIMIT_REG, 0xE0);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, INPUT_CURRENT_LIMIT_REG, ret);
        ret = smb137b_write_reg(smb137b_chg->client,
                      FLOAT_VOLTAGE_REG, 0xD9);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, FLOAT_VOLTAGE_REG, ret);
        ret = smb137b_write_reg(smb137b_chg->client,
                      CONTROL_A_REG, 0x4C);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, CONTROL_A_REG, ret);
        ret = smb137b_write_reg(smb137b_chg->client,
                      CONTROL_B_REG, 0x28);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, CONTROL_B_REG, ret);
        ret = smb137b_write_reg(smb137b_chg->client,
                      PIN_CTRL_REG, 0x18);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, PIN_CTRL_REG, ret);
        ret = smb137b_write_reg(smb137b_chg->client,
                      OTG_LBR_CTRL_REG, 0x5F);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, OTG_LBR_CTRL_REG, ret);
        ret = smb137b_write_reg(smb137b_chg->client,
                      FAULT_INTR_REG, 0);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__,FAULT_INTR_REG, ret);
        ret = smb137b_write_reg(smb137b_chg->client,
                      CELL_TEMP_MON_REG, 0xD7);
	if(ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__,CELL_TEMP_MON_REG, ret);
        ret = smb137b_write_reg(smb137b_chg->client,
                      SAFETY_TIMER_THERMAL_SHUTDOWN_REG, 0x4B);
        if (ret < 0)
	dev_err(&client->dev, "%s fail smb137b_write_reg Reg = %d, ret = %d\n", __func__, SAFETY_TIMER_THERMAL_SHUTDOWN_REG, ret);

	mdelay(100);
	
	ret = smb137b_read_reg(smb137b_chg->client, CHG_CURRENT_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		CHG_CURRENT_REG, temp);

	ret = smb137b_read_reg(smb137b_chg->client, INPUT_CURRENT_LIMIT_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		INPUT_CURRENT_LIMIT_REG, temp);

	ret = smb137b_read_reg(smb137b_chg->client, FLOAT_VOLTAGE_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		FLOAT_VOLTAGE_REG, temp);

	ret = smb137b_read_reg(smb137b_chg->client, CONTROL_A_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		CONTROL_A_REG, temp);

	ret = smb137b_read_reg(smb137b_chg->client, CONTROL_B_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		CONTROL_B_REG, temp);

	ret = smb137b_read_reg(smb137b_chg->client, PIN_CTRL_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		PIN_CTRL_REG, temp);

	ret = smb137b_read_reg(smb137b_chg->client, OTG_LBR_CTRL_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		OTG_LBR_CTRL_REG, temp);

	ret = smb137b_read_reg(smb137b_chg->client, FAULT_INTR_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		FAULT_INTR_REG, temp);

	ret = smb137b_read_reg(smb137b_chg->client, CELL_TEMP_MON_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		CELL_TEMP_MON_REG, temp);

	ret = smb137b_read_reg(smb137b_chg->client, SAFETY_TIMER_THERMAL_SHUTDOWN_REG, &temp);
	if (ret == 0)
	dev_info(&client->dev,
		"%s smb137b_read_reg R%d = 0x%x \n", __func__,
		SAFETY_TIMER_THERMAL_SHUTDOWN_REG, temp);
#endif

	return 0;

disable_irq_wake:
	set_irq_wake(client->irq, 0);
unregister_charger:
	msm_charger_unregister(&smb137b_chg->adapter_hw_chg);
free_valid_gpio:
	gpio_free(pdata->valid_n_gpio);
free_smb137b_chg:
	kfree(smb137b_chg);
out:
	return ret;
}

void smb137b_otg_power(int on)
{
pr_debug("%s Enter on=%d only check can't otg+power_set pz1946\n", __func__, on);
//pz1946 20110907 EF39S EF40K EF40S OTG don't use
#if 0 
	int ret;

	pr_debug("%s Enter on=%d\n", __func__, on);
	if (on) {
		ret = smb137b_write_reg(usb_smb137b_chg->client,
					PIN_CTRL_REG, PIN_CTRL_REG_CHG_OFF);
		if (ret) {
			pr_err("%s turning off charging in pin_ctrl err=%d\n",
								__func__, ret);
			/*
			 * dont change the command register if we cant
			 * overwrite pin control
			 */
			return;
		}

		ret = smb137b_write_reg(usb_smb137b_chg->client,
			COMMAND_A_REG, COMMAND_A_REG_OTG_MODE);
		if (ret)
			pr_err("%s failed turning on OTG mode ret=%d\n",
								__func__, ret);
	} else {
		ret = smb137b_write_reg(usb_smb137b_chg->client,
			COMMAND_A_REG, COMMAND_A_REG_DEFAULT);
		if (ret)
			pr_err("%s failed turning off OTG mode ret=%d\n",
								__func__, ret);
		ret = smb137b_write_reg(usb_smb137b_chg->client,
				PIN_CTRL_REG, PIN_CTRL_REG_DEFAULT);
		if (ret)
			pr_err("%s failed writing to pn_ctrl ret=%d\n",
								__func__, ret);
	}
#endif
}

static int __devexit smb137b_remove(struct i2c_client *client)
{
	const struct smb137b_platform_data *pdata;
	struct smb137b_data *smb137b_chg = i2c_get_clientdata(client);

	pdata = client->dev.platform_data;
	device_init_wakeup(&client->dev, 0);
	set_irq_wake(client->irq, 0);
	free_irq(client->irq, client);
	gpio_free(pdata->valid_n_gpio);
	cancel_delayed_work_sync(&smb137b_chg->charge_work);

	msm_charger_notify_event(&smb137b_chg->adapter_hw_chg,
			 CHG_REMOVED_EVENT);
	msm_charger_unregister(&smb137b_chg->adapter_hw_chg);
	smb137b_destroy_debugfs_entries();
#ifdef CONFIG_SKY_SMB_CHARGER
	sysfs_remove_group(&smb137b_chg->client->dev.kobj, &smb137b_sysfs_files);
#endif
	kfree(smb137b_chg);
	return 0;
}

#ifdef CONFIG_PM
static int smb137b_suspend(struct device *dev)
{
	struct smb137b_data *smb137b_chg = dev_get_drvdata(dev);

	dev_dbg(&smb137b_chg->client->dev, "%s\n", __func__);
	if (smb137b_chg->charging)
		return -EBUSY;
	return 0;
}

static int smb137b_resume(struct device *dev)
{
	struct smb137b_data *smb137b_chg = dev_get_drvdata(dev);

	dev_dbg(&smb137b_chg->client->dev, "%s\n", __func__);
	return 0;
}

static const struct dev_pm_ops smb137b_pm_ops = {
	.suspend = smb137b_suspend,
	.resume = smb137b_resume,
};
#endif

static const struct i2c_device_id smb137b_id[] = {
	{"smb137b", 0},
	{},
};
MODULE_DEVICE_TABLE(i2c, smb137b_id);

static struct i2c_driver smb137b_driver = {
	.driver = {
		   .name = "smb137b",
		   .owner = THIS_MODULE,
#ifdef CONFIG_PM
		   .pm = &smb137b_pm_ops,
#endif
	},
	.probe = smb137b_probe,
	.remove = __devexit_p(smb137b_remove),
	.id_table = smb137b_id,
};

static int __init smb137b_init(void)
{
	return i2c_add_driver(&smb137b_driver);
}
module_init(smb137b_init);

static void __exit smb137b_exit(void)
{
	return i2c_del_driver(&smb137b_driver);
}
module_exit(smb137b_exit);

MODULE_AUTHOR("Abhijeet Dharmapurikar <adharmap@codeaurora.org>");
MODULE_DESCRIPTION("Driver for SMB137B Charger chip");
MODULE_LICENSE("GPL v2");
MODULE_ALIAS("i2c:smb137b");
