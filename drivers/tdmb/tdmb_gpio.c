//=============================================================================
// File       : tdmb_gpio.c
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2010/11/02       yschoi         Create
//=============================================================================

#include <linux/kernel.h>
#include <linux/module.h>
#include <mach/gpio.h>

#include "tdmb_comdef.h"
#include "tdmb_dev.h"
#include "tdmb_gpio.h"


#ifdef FEATURE_TDMB_GPIO_INIT
#ifdef FEATURE_QTDMB_TSIF_IF
#define DMB_TSIF_CLK      84
#define DMB_TSIF_EN       85
#define DMB_TSIF_DATA     86
#endif /* FEATURE_QTDMB_TSIF_IF */


static uint32_t tdmb_gpio_init_table[] = {
  GPIO_CFG(DMB_INT, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),

  GPIO_CFG(DMB_RESET, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),

#ifndef FEATURE_TDMB_PMIC_POWER
  GPIO_CFG(DMB_PWR_EN, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
#endif

#ifdef FEATURE_TDMB_SET_ANT_PATH
  GPIO_CFG(DMB_ANT_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
#endif

#ifdef FEATURE_QTDMB_I2C_CMD
  GPIO_CFG(DMB_I2C_SCL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
  GPIO_CFG(DMB_I2C_SDA, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
#endif /* FEATURE_QTDMB_I2C_CMD */

#ifdef FEATURE_TDMB_PMIC_TCXO_192M
  GPIO_CFG(DMB_XO_SEL, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
#endif

#ifdef FEATURE_QTDMB_TSIF_IF
  GPIO_CFG(DMB_TSIF_CLK, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
  GPIO_CFG(DMB_TSIF_EN, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
  GPIO_CFG(DMB_TSIF_DATA, 1, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA),
#endif /* FEATURE_QTDMB_TSIF_IF */
};


void tdmb_gpio_init(void)
{
  int i, rc;

  TDMB_MSG_GPIO("[%s] tdmb_gpio_init!!!\n",__func__);

  for(i = 0; i < ARRAY_SIZE(tdmb_gpio_init_table); i ++)
  {
    rc = gpio_tlmm_config(tdmb_gpio_init_table[i], GPIO_CFG_ENABLE);
    if (rc)
    {
      TDMB_MSG_GPIO("[%s] gpio_tlmm_config(%#x)=%d\n",__func__, tdmb_gpio_init_table[i], rc);
      break;
    }
  }

// EF39S Test
#if (defined(CONFIG_SKY_EF39S_BOARD) && (BOARD_REV > WS10))
  tdmb_set_gpio(DMB_ANT_SEL, 0);
#endif

  TDMB_MSG_GPIO("[%s] tdmb_gpio_init end cnt[%d]!!!\n",__func__, i);
}
#endif /* FEATURE_TDMB_GPIO_INIT */


void tdmb_set_gpio(uint gpio, bool value)
{
#if 1
  gpio_set_value(gpio, value);
  TDMB_MSG_GPIO("[%s] gpio [%d] set [%d]\n", __func__, gpio, value);

#ifdef FEATURE_GPIO_DEBUG
  TDMB_MSG_GPIO("[%s] gpio [%d] get [%d]\n", __func__, gpio, gpio_get_value(gpio));
#endif
#else
  int rc = 0;

  rc = gpio_request(gpio, "tdmb_gpio");
  if (!rc)
  {
    rc = gpio_direction_output(gpio, value);
    TDMB_MSG_GPIO("[%s] gpio [%d] set [%d]\n", __func__, gpio, value);
  }
  else
  {
    TDMB_MSG_GPIO("[%s] gpio_request fail!!!\n", __func__);
  }

#ifdef FEATURE_GPIO_DEBUG
  TDMB_MSG_GPIO("[%s] gpio [%d] get [%d]\n", __func__, gpio, gpio_get_value(gpio));
#endif

  gpio_free(gpio);
#endif // 0
}

