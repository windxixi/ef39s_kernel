//=============================================================================
// File       : T3700_i2c.c
//
// Description: 
//
// Revision History:
//
// Version         Date           Author        Description of Changes
//-----------------------------------------------------------------------------
//  1.0.0       2009/05/06       yschoi         Create
//=============================================================================

#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/delay.h>

#include "tdmb_comdef.h"
#include "tdmb_dev.h"
#include "tdmb_i2c.h"
#include "tdmb_type.h"



/*================================================================== */
/*================      TDMB I2C Definition        ================= */
/*================================================================== */

#define MAX_REG_LEN 2
#define MAX_DATA_LEN 16


static struct i2c_client *tdmb_i2c_client = NULL;

static int tdmb_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id);

static uint8 tdmb_i2c_write(uint8 chipid, uint8 *reg, uint8 reg_len, uint8 *data, uint16 data_len);
static uint8 tdmb_i2c_read(uint8 chipid, uint8 *reg, uint8 reg_len, uint8 *data, uint16 data_len);


/*================================================================== */
/*==============       TDMB i2c Driver Function      =============== */
/*================================================================== */

static int tdmb_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
  int rc = 0;

  if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
  {
    tdmb_i2c_client = NULL;
    rc = -1;
    TDMB_MSG_I2C("[%s] failed!!!\n", __func__);
  }
  else
  {
  tdmb_i2c_client = client;
  }

  TDMB_MSG_I2C("[%s] succeed!!!\n", __func__);

  return rc;
}


static int tdmb_i2c_remove(struct i2c_client *client)
{
#if 0 // 20101102 cys
  int rc;

  tdmb_i2c_client = NULL;
#if 1
  rc = i2c_detach_client(client);

  return rc;
#endif
#endif // 0
  TDMB_MSG_I2C("[%s] removed!!!\n", __func__);
  return 0;
}


static const struct i2c_device_id tdmb_i2c_id[] = {
  {TDMB_I2C_DEV_NAME, 0},
};

static struct i2c_driver tdmb_i2c_driver = {
  .id_table = tdmb_i2c_id,
  .probe  = tdmb_i2c_probe,
  .remove = tdmb_i2c_remove,
  .driver = {
    .name = TDMB_I2C_DEV_NAME,
  },
};

void tdmb_i2c_api_Init(void)
{
  int result = 0;

  result = i2c_add_driver(&tdmb_i2c_driver);
  TDMB_MSG_I2C("[%s] i2c_add_driver!\n", __func__);

  if(result){
    TDMB_MSG_I2C("[%s] error!!!\n", __func__);
  }
}

void tdmb_i2c_api_DeInit(void)
{
  i2c_del_driver(&tdmb_i2c_driver);
}



/*================================================================== */
/*=================       TDMB i2c Function       ================== */
/*================================================================== */

/*====================================================================
FUNCTION       tdmb_i2c_check_id
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static void tdmb_i2c_check_id(uint8 chipid)
{
  if(tdmb_i2c_client->addr != (chipid >> 1))
  {
    TDMB_MSG_I2C("[%s] chipid error addr[0x%02x], chipid[0x%02x]!!!\n", __func__, tdmb_i2c_client->addr, chipid);
  }
}

/*====================================================================
FUNCTION       tdmb_i2c_write_word
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tdmb_i2c_write_word(uint8 chipid, uint16 reg, uint8 reg_len, uint16 *data, uint16 data_len)
{
  int ret, i;
  unsigned char data_buf[MAX_DATA_LEN];
  int data_buf_len;

#ifdef FEATURE_I2C_DBG_MSG
  TDMB_MSG_I2C("[%s]\n", __func__);
#endif /* FEATURE_I2C_DBG_MSG */

  for(i = 0; i < data_len / 2; i ++)
  {
    data_buf[i * 2] = (*(data + i) & 0xFF00) >> 8;
    data_buf[i * 2 + 1] = *(data + i) & 0x00FF;
  }

  data_buf_len = data_len * 2;

  ret = tdmb_i2c_write_len(chipid, reg, reg_len, data_buf, data_buf_len);

  return ret;
}

/*====================================================================
FUNCTION       tdmb_i2c_write_len
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tdmb_i2c_write_len(uint8 chipid, uint16 reg, uint8 reg_len, uint8 *data, uint16 data_len)
{
  int ret, i;
  unsigned char data_buf[MAX_DATA_LEN];
  uint16 data_buf_len = 0;

#ifdef FEATURE_I2C_DBG_MSG
  uint16 uiData;
#endif

#ifdef FEATURE_I2C_WRITE_CHECK
  unsigned char rdata_buf[MAX_DATA_LEN];
#endif

  tdmb_i2c_check_id(chipid);
  
  if (reg_len == 1)
  {
    data_buf[0] = (reg & 0x00FF);
  }
  else if (reg_len == 2)
  {
    data_buf[0] = (reg & 0xFF00) >> 8;
    data_buf[1] = (reg & 0x00FF);
  }
  else
  {
    TDMB_MSG_I2C("[%s] error!!! register length is [%d]\n", __func__, reg_len);
    return FALSE;
  }

  data_buf_len = reg_len + data_len;

  if(data_buf_len > MAX_DATA_LEN)
  {
    TDMB_MSG_I2C("[%s] error!!! data_buf length is [%d]\n", __func__, data_buf_len);
    return FALSE;
  }

#if 0
  for(i = 0; i < data_len; i ++)
  {
    data_buf[reg_len + i] = *(data + i);
  }
#else
  memcpy(data_buf + reg_len, data, data_len);
#endif

#ifdef FEATURE_I2C_DBG_MSG
  if (data_len == 1)
  {
    uiData = *data;
  }
  else // data_len >= 2
  {
    uiData = (((*data) << 8) & 0xFF00) + *(data + 1);
  }

  TDMB_MSG_I2C("[%s] reg[0x%04x].len[%d] data[0x%04x].len[%d]\n", __func__, reg, reg_len, uiData, data_len);

  for(i = 0; i < data_buf_len; i ++)
  {
    TDMB_MSG_I2C("[%s] data_buf[0x%02x].idx[%d]\n", __func__, data_buf[i], i);
  }
#endif /* FEATURE_I2C_DBG_MSG */

  ret = tdmb_i2c_write(chipid, (uint8 *)&reg, reg_len, data_buf, data_buf_len);

  if(!ret)
  {
#ifndef FEATURE_I2C_DBG_MSG
    TDMB_MSG_I2C("[%s] reg[0x%04x].len[%d]\n", __func__, reg, reg_len);
#endif
    return FALSE;
  }

#ifdef FEATURE_I2C_DBG_MSG
  TDMB_MSG_I2C("[%s] write OK!!!\n", __func__);
#endif /* FEATURE_I2C_DBG_MSG */

#ifdef FEATURE_I2C_WRITE_CHECK
  tdmb_i2c_read_len(chipid, reg, reg_len, rdata_buf, data_len);

  for(i = 0; i < data_len; i ++)
  {
    if (*(data + i) != rdata_buf[i])
    {
      TDMB_MSG_I2C("[%s] Check error! reg[0x%04x], idx[%d], w[0x%02x], r[0x%02x]\n", __func__, reg, i, *(data + i), rdata_buf[i]);
    }
    else
    {
      TDMB_MSG_I2C("[%s] Check OK! reg[0x%04x], idx[%d], w[0x%02x], r[0x%02x]\n", __func__, reg, i, *(data + i), rdata_buf[i]);
    }
  }
#endif /* FEATURE_I2C_WRITE_CHECK */

  return TRUE;
}


/*====================================================================
FUNCTION       tdmb_i2c_write
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static uint8 tdmb_i2c_write(uint8 chipid, uint8 *reg, uint8 reg_len, uint8 *data, uint16 data_len)
{
  static int ret = 0;

  struct i2c_msg msg[] = {
    {.addr = tdmb_i2c_client->addr, .flags = 0, .buf = data, .len = data_len},
  };
  
  if(!tdmb_i2c_client)
  {
    TDMB_MSG_I2C("[%s] tdmb_i2c_client is null!!!\n", __func__);
    return -1;
  }

  ret = i2c_transfer(tdmb_i2c_client->adapter, msg, 1);
  if (ret < 0)
  {
    TDMB_MSG_I2C("[%s] write error!!! addr[0x%02x] ret[%d]\n", __func__, tdmb_i2c_client->addr, ret);
    return FALSE;
  }

  return TRUE;
}


/*====================================================================
FUNCTION       tdmb_i2c_read_word
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tdmb_i2c_read_word(uint8 chipid, uint16 reg, uint8 reg_len, uint16 *data, uint16 data_len)
{
  int ret, i;
  unsigned char data_buf[MAX_DATA_LEN];
  int data_buf_len;

#ifdef FEATURE_I2C_DBG_MSG
  TDMB_MSG_I2C("[%s]\n", __func__);
#endif /* FEATURE_I2C_DBG_MSG */

  data_buf_len = data_len * 2;

  ret = tdmb_i2c_read_len(chipid, reg, reg_len, data_buf, data_buf_len);

  for(i = 0; i < data_len; i ++)
  {
    *(data + i) = ((data_buf[i * 2] << 8) & 0xFF00) + data_buf[i * 2 + 1];
  }

  return ret;
}


/*====================================================================
FUNCTION       tdmb_i2c_read_len
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
uint8 tdmb_i2c_read_len(uint8 chipid, uint16 reg, uint8 reg_len, uint8 *data, uint16 data_len)
{
  int ret;
  unsigned char reg_buf[MAX_REG_LEN];

#ifdef FEATURE_I2C_DBG_MSG  
  uint16 uiData;
#endif
  
  if (reg_len == 1)
  {
    reg_buf[0] = (reg & 0x00FF);
  }
  else if (reg_len == 2)
  {
    reg_buf[0] = (reg & 0xFF00) >> 8;
    reg_buf[1] = (reg & 0x00FF);
  }
  else
  {
    TDMB_MSG_I2C("[%s] error!!! register length is [%d]\n", __func__, reg_len);
    return FALSE;
  }

#ifdef FEATURE_I2C_DBG_MSG
  TDMB_MSG_I2C("[%s] reg[0x%04x].len[%d]\n", __func__, reg, reg_len);
#endif

  ret = tdmb_i2c_read(chipid, reg_buf, reg_len, data, data_len);

  if(!ret)
  {
#ifndef FEATURE_I2C_DBG_MSG
    TDMB_MSG_I2C("[%s] reg[0x%04x].len[%d]\n", __func__, reg, reg_len);
#endif
    return FALSE;
  }

#ifdef FEATURE_I2C_DBG_MSG
  TDMB_MSG_I2C("[%s] read OK!!!\n", __func__);

  if (data_len == 1)
  {
    uiData = *data;
  }
  else // data_len >= 2
  {
    uiData = ((*data << 8) & 0xFF00) + *(data + 1);
  }
  TDMB_MSG_I2C("[%s] reg[0x%04x], data[0x%04x].len[%d]\n", __func__, reg, uiData, data_len);
#endif

  return TRUE;  
}


/*====================================================================
FUNCTION       tdmb_i2c_read
DESCRIPTION 
DEPENDENCIES
RETURN VALUE
SIDE EFFECTS
======================================================================*/
static uint8 tdmb_i2c_read(uint8 chipid, uint8 *reg, uint8 reg_len, uint8 *data, uint16 data_len)
{
  static int ret = 0;

  struct i2c_msg msg[] = {
    {.addr = tdmb_i2c_client->addr, .flags = 0, .buf = reg, .len = reg_len},
    {.addr = tdmb_i2c_client->addr, .flags = I2C_M_RD, .buf = data, .len = data_len}
  };

  if(!tdmb_i2c_client)
  {
    TDMB_MSG_I2C("[%s] tdmb_i2c_client is null!!!\n", __func__);
    return -1;
  }

// 20090921 cys 도넛 버전에서 정상동작하여 원복함
#if 0 // android 3145 버전에서 msgs 2개 한번에 보내면 에러리턴됨
  ret = i2c_transfer(tdmb_i2c_client->adapter, msgs, 1);
  if (ret < 0)
  {
    TDMB_MSG_I2C("[%s] write error!!!\n", __func__);
    return FALSE;
  }
  ret = i2c_transfer(tdmb_i2c_client->adapter, &msgs[1], 1);
#else
  ret = i2c_transfer(tdmb_i2c_client->adapter, msg, 2);
#endif
  if (ret < 0)
  {
    TDMB_MSG_I2C("[%s] read error!!! addr[0x%02x] ret[%d]\n", __func__, tdmb_i2c_client->addr, ret);
    return FALSE;
  }

  return TRUE;
}

