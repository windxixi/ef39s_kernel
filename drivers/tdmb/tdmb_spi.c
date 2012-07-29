//=============================================================================
// File       : tdmb_spi.c
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2011/04/20       yschoi         Create
//=============================================================================

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/spi/spi.h>
#include <linux/interrupt.h>
#include <linux/errno.h>
#include <linux/gpio.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <mach/board.h>
#include <mach/gpio.h>

#include "tdmb_comdef.h"
#include "tdmb_spi.h"
#include "tdmb_gpio.h"

#ifdef FEATURE_QTDMB_SPI_IF

#ifdef FEATURE_QTDMB_USE_INC_T3900
#include "t3900/t3900_includes.h"
#include "t3900/t3900_bb.h"
#endif

#ifdef FEATURE_QTDMB_USE_INC_T3700
#include "t3700/t3700_includes.h"
#include "t3700/t3700_bb.h"
#endif

#ifdef FEATURE_QTDMB_USE_FCI_FC8050
#include "fc8050/fc8050_wrapper.h"
#endif

#ifdef FEATURE_QTDMB_USE_RTV_MTV350
#include "mtv350/mtv350_bb.h"
#endif /* FEATURE_QTDMB_USE_RTV */

#ifdef FEATURE_QTDMB_USE_TCC_TCC3170
#include "tcc3170/tcc3170_bb.h"
#include "tcc3170/IO/TC317X_IO_CSPI.h"
#endif


#if 0//def FEATURE_QTDMB_USE_FCI_FC8050
extern int fc8050_spi_write_then_read(struct spi_device *spi, u8 *txbuf, u16 tx_length, u8 *rxbuf, u16 rx_length);
extern int fc8050_spi_write_then_read_burst(struct spi_device *spi, u8 *txbuf, u16 tx_length, u8 *rxbuf, u16 rx_length);
#endif


struct spi_device *tdmb_spi_dev = NULL;


#ifdef CONFIG_SKY_TDMB_SPI_GPIO
#define DMB_SPI_MOSI 37
#define DMB_SPI_MISO 38
#define DMB_SPI_CS   39
#define DMB_SPI_CLK  40

static uint32 tdmb_spi_gpio_table[] = {
  GPIO_CFG(DMB_SPI_CS, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
  GPIO_CFG(DMB_SPI_CLK, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
  GPIO_CFG(DMB_SPI_MOSI, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
  GPIO_CFG(DMB_SPI_MISO, 0, GPIO_CFG_INPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA),
};
#endif


/*================================================================== */
/*==============       TDMB SPI Driver Function      =============== */
/*================================================================== */

#ifdef CONFIG_SKY_TDMB_SPI_GPIO
/*====================================================================
FUNCTION       tcc3170_gpio_spi_init  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tdmb_spi_gpio_init(void)
{
  int i, rc;

  //TDMB_MSG_SPI("[%s] \n", __func__);
  
  for(i = 0; i < ARRAY_SIZE(tdmb_spi_gpio_table); i ++)
  {
    rc = gpio_tlmm_config(tdmb_spi_gpio_table[i], GPIO_CFG_ENABLE);
    if (rc)
    {
      TDMB_MSG_SPI("[%s] error!!! index=%d, rc=%d\n",__func__, i  , rc);
      break;
    }
  }
}
#endif


/*====================================================================
FUNCTION       fc8050_spi_write_then_read  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
int tdmb_spi_write_then_read(u8 *txbuf, unsigned n_tx,u8 *rxbuf, unsigned n_rx)
{
  int ret;
  
  //TDMB_MSG_SPI("[%s] \n", __func__);

  //TDMB_MSG_SPI("[%s] tdmb_spi_dev[0x%x]\n", __func__, (unsigned int) tdmb_spi_dev);

#if 0//def FEATURE_QTDMB_USE_FCI_FC8050
  //ret = fc8050_spi_write_then_read(tdmb_spi_dev, txbuf, n_tx, rxbuf, n_rx);
  ret = fc8050_spi_write_then_read_burst(tdmb_spi_dev, txbuf, n_tx, rxbuf, n_rx);
#else
  ret = spi_write_then_read(tdmb_spi_dev, txbuf, n_tx, rxbuf, n_rx);
#endif

  if(ret)
  {
    TDMB_MSG_SPI("[%s] fail : %d\n", __func__, ret);
    return 1;
  }

  return 0;
}


/*====================================================================
FUNCTION       tdmb_spi_clear_int  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tdmb_spi_clear_int(void)
{
  //TDMB_MSG_SPI("[%s] \n", __func__);

#ifdef FEATURE_QTDMB_USE_FCI
  FC8050_BUFFER_RESET(VIDEO_MASK);
#endif

  return;
}


#if 0
/*====================================================================
FUNCTION       find_spi_device  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static struct spi_device* find_spi_device(int bus_num)
{
  struct spi_master *spi_master;
  struct spi_device *spi_device;
  struct device *pdev;
  char buff[64];

  spi_master = spi_busnum_to_master(bus_num);
  if (!spi_master) {
    TDMB_MSG_SPI("[%s] spi_busnum_to_master(%d) returned NULL\n", __func__, bus_num);
    return NULL;
  }

  spi_device = spi_alloc_device(spi_master);
  if (!spi_device) {
    put_device(&spi_master->dev);
    TDMB_MSG_SPI("[%s] spi_alloc_device() failed\n", __func__);
    return NULL;
  }

  /* specify a chip select line */
  spi_device->chip_select = 0;

  snprintf(buff, sizeof(buff), "%s.%u",
      dev_name(&spi_device->master->dev),
      spi_device->chip_select);

  pdev = bus_find_device_by_name(spi_device->dev.bus, NULL, buff);

  //if (pdev) {
  //  TCDBG("spi_device :0x%X\n", (unsigned int)spi_device);
  //}

  put_device(&spi_master->dev);
  return spi_device;
}
#endif


/*====================================================================
FUNCTION       tdmb_spi_setup  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void* tdmb_spi_setup(void)
{
  int rc;

  TDMB_MSG_SPI("[%s] \n", __func__);

#ifdef CONFIG_SKY_TDMB_SPI_GPIO
  tdmb_spi_gpio_init();
#endif

#if 1
  //TDMB_MSG_SPI("[%s] tdmb_spi_dev[0x%x]\n", __func__, (unsigned int) tdmb_spi_dev);

#if defined(FEATURE_QTDMB_USE_FCI)
  //tdmb_spi_dev->max_speed_hz = 24000000; // 24M
  //tdmb_spi_dev->max_speed_hz = 5400000; // 5.4M
  tdmb_spi_dev->max_speed_hz = 10800000; // 10.8M
  tdmb_spi_dev->mode = SPI_MODE_0;
#elif defined(FEATURE_QTDMB_USE_TCC)
  tdmb_spi_dev->max_speed_hz = 10800000;
  tdmb_spi_dev->mode = SPI_MODE_2;
#else
  ##error
#endif
  //tdmb_spi_dev->chip_select    = 0;
  //tdmb_spi_dev->bits_per_word  = 8;

  rc = spi_setup(tdmb_spi_dev);

  if (rc < 0)
  {
    TDMB_MSG_SPI("[%s] rc2 [%d]\n", __func__,rc);
  }
  else
  {
    if(tdmb_spi_dev == NULL)
    {
      TDMB_MSG_SPI("[%s] tdmb_spi_dev NULL \n",__func__);
      return 0;
    }
    TDMB_MSG_SPI("[%s] spi_setup OK [%d]\n", __func__,rc);
  }
  
#endif

  return tdmb_spi_dev;
}


/*====================================================================
FUNCTION       tdmb_spi_probe  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static int __devinit tdmb_spi_probe(struct spi_device *spi)
{
  TDMB_MSG_SPI("[%s] \n", __func__);

  tdmb_spi_dev = spi;

  if(tdmb_spi_dev == NULL)
  {
    TDMB_MSG_SPI("[%s] SPI device is null\n", __func__);
    return 0;
  }
  else 
  {
    //TDMB_MSG_SPI("[%s] tdmb_spi_dev[0x%x]\n", __func__, (unsigned int) tdmb_spi_dev);
    TDMB_MSG_SPI("[%s] SPI  Max speed [%d] CS[%d]  Mode[%d] \n", __func__,
    tdmb_spi_dev->max_speed_hz, tdmb_spi_dev->chip_select, tdmb_spi_dev->mode);
  }

  return 0;
}


/*====================================================================
FUNCTION       tdmb_spi_remove  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static int __devexit tdmb_spi_remove (struct spi_device *spi)
{
  TDMB_MSG_SPI("[%s] \n", __func__);
  return 0;
}


/*====================================================================

======================================================================*/

/* spi driver data */
static struct spi_driver tdmb_spi_driver = {
  .probe = tdmb_spi_probe,
  .remove = __devexit_p(tdmb_spi_remove),
  .driver = {
    .name = TDMB_SPI_DEV_NAME,
    .bus = &spi_bus_type,
    .owner = THIS_MODULE,
  },
};


/*====================================================================
FUNCTION       tdmb_spi_init  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tdmb_spi_init(void)
{
  int rc;

  TDMB_MSG_SPI("[%s] \n", __func__);

#ifdef CONFIG_SKY_TDMB_SPI_GPIO
  //tdmb_spi_gpio_init();
#endif

  rc = spi_register_driver(&tdmb_spi_driver);

  if(rc)
  {
    TDMB_MSG_SPI("[%s] spi_probe fail !!!, rc [%d]", __func__,rc);
  }

#if 0
  tdmb_spi_dev = find_spi_device(5);

  if(tdmb_spi_dev == NULL)
  {
    TDMB_MSG_SPI("[%s] SPI device is null\n", __func__);
    return;
  }
  else 
  {
    TDMB_MSG_SPI("[%s] SPI  Max speed [%d] CS[%d]  Mode[%d] \n", __func__,
    tdmb_spi_dev->max_speed_hz, tdmb_spi_dev->chip_select, tdmb_spi_dev->mode);
  }
#endif

  return;
}


/*====================================================================
FUNCTION       tdmb_spi_exit  
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
void tdmb_spi_exit(void)
{
  TDMB_MSG_SPI("[%s] \n", __func__);
  spi_unregister_driver(&tdmb_spi_driver);
}

#endif /* FEATURE_QTDMB_SPI_IF */

