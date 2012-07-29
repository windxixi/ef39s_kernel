//----------------------------------------------------------------------------
//   FileName    : TC317X_IO_I2C.c
//    Description :
//----------------------------------------------------------------------------
//
//   TCC Version : 1.0.0
//   Copyright (c) Telechips, Inc.
//   ALL RIGHTS RESERVED
//
//--------------------------------------------------------------------------
#include "TC317X_IO_I2C.h"
#include "TC317X_IO.h"
#include "../TC317X_Common.h"
#include <linux/i2c.h>

#if defined (USE_IF_I2C)

//#define LEGACY_DRIVER
#define NEW_STYLE_DRIVER

#define MAX_I2C_BURST        512
#define Bit7          0x00000080

#define I2C_BUS                1
#define I2C_ADDR       (0xA0>>1)

unsigned char gi2c_buff[MAX_I2C_BURST+4];

static struct i2c_driver tc317x_i2c_driver;
static struct i2c_client *tc317x_i2c_client = NULL;

static int tc317x_add_i2c_device(void);

void TC317X_IO_I2C_Variables_Init (int moduleidx)
{
  // none
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Init
//    Description : Initialize I2C
//    Input :
//      moduleidx : module index
//    Output :
//
//    Remark :
//--------------------------------------------------------------------------
void TC317X_IO_I2C_Init (int moduleidx)
{
  int ret = -1;
#ifdef NEW_STYLE_DRIVER
  ret = tc317x_add_i2c_device();
#else
  ret = i2c_add_driver(&tc317x_i2c_driver);
#endif
  if(ret < 0 ) TCDBG("can't add i2c driver!\n");
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Close
//    Description : Close I2C
//    Input :
//      moduleidx : module index
//    Output :
//      TC3X_IO_SUCCESS
//    Remark :
//--------------------------------------------------------------------------
TC3X_IO_Err TC317X_IO_I2C_Close (int moduleidx)
{
#ifdef NEW_STYLE_DRIVER
  i2c_unregister_device(tc317x_i2c_client);
#endif
  i2c_del_driver(&tc317x_i2c_driver);

#ifdef LEGACY_DRIVER
  if(tc317x_i2c_client){
    kfree(tc317x_i2c_client);
    tc317x_i2c_client = NULL;
  }
#endif
  TCDBG("\n");
  return TC3X_IO_SUCCESS;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Reg_Write
//    Description : Reg write
//    Input :
//      moduleidx : module index
//      ChipAddr : chip address
//      RegAddr : Register Address
//      data : data
//    Output :
//      TC3X_IO_SUCCESS
//    Remark :
//--------------------------------------------------------------------------
TC3X_IO_Err TC317X_IO_I2C_Reg_Write (int moduleidx, int ChipAddr, int RegAddr, unsigned int data)
{
  int ret =0;
  unsigned char buf[2];

  if(!tc317x_i2c_client) return TC3X_IO_FAIL;

  buf[0] = (unsigned char)(RegAddr);
  buf[1] = (unsigned char)(data);

  TC317X_IO_IF_LOCK (0);

  ret = i2c_master_send(tc317x_i2c_client, buf, 2);

  TC317X_IO_IF_UnLOCK (0);
  if(ret < 0) return TC3X_IO_FAIL;

  return TC3X_IO_SUCCESS;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Reg_Write_InnerSem
//    Description : Reg write (inside semaphore)
//      moduleidx : module index
//      ChipAddr : chip address
//      RegAddr : Register Address
//      data : data
//    Output :
//      TC3X_IO_SUCCESS
///    Remark :
//--------------------------------------------------------------------------
TC3X_IO_Err TC317X_IO_I2C_Reg_Write_InnerSem (int moduleidx, int ChipAddr, int RegAddr, unsigned int data)
{
  int ret = 0;
  unsigned char buf[2];

  if(!tc317x_i2c_client) return TC3X_IO_FAIL;

  buf[0] = (unsigned char)(RegAddr);
  buf[1] = (unsigned char)(data);

  ret = i2c_master_send(tc317x_i2c_client, buf, 2);

  if(ret < 0) return TC3X_IO_FAIL;

  return TC3X_IO_SUCCESS;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Reg_Read
//    Description : Reg read
//    Input :
//      moduleidx : module index
//      ChipAddr : chip address
//      RegAddr : Register Address
//      pError : Error flag
//    Output :
//      TC3X_IO_SUCCESS///    Remark :
//--------------------------------------------------------------------------
unsigned int TC317X_IO_I2C_Reg_Read (int moduleidx, int ChipAddr, int RegAddr, TC3X_IO_Err * pError)
{
  unsigned int ret = -1;
  unsigned char buf[2] = {0,};

  if(!tc317x_i2c_client) return ret;

  TC317X_IO_IF_LOCK (0);

  buf[0] = (unsigned char)(RegAddr&0xff);
  buf[1] = 0x00;
  ret = i2c_master_send(tc317x_i2c_client, buf, 1);
  if(ret < 0) { TCDBG("I2C write error %d\n", ret); return TC3X_IO_FAIL; }

  ret = i2c_master_recv(tc317x_i2c_client, buf, 1);
  if(ret < 0) { TCDBG("I2C read error %d\n", ret); return TC3X_IO_FAIL; }

  TC317X_IO_IF_UnLOCK (0);

  ret = (unsigned int)(buf[0]);
  return ret;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Reg_Read_InnerSem
//    Description : Reg read (inside semaphore)
//    Input :
//      moduleidx : module index
//      ChipAddr : chip address
//      RegAddr : Register Address
//      pError : Error flag
//    Output :
//      TC3X_IO_SUCCESS
//    Remark :
//--------------------------------------------------------------------------
unsigned int TC317X_IO_I2C_Reg_Read_InnerSem (int moduleidx, int ChipAddr, int RegAddr, TC3X_IO_Err * pError)
{
  char buf[2] = {0,};
  unsigned int ret = 0;

  if(!tc317x_i2c_client) return TC3X_IO_FAIL;

  buf[0] = (unsigned char)(RegAddr&0xff);
  buf[1] = 0x00;
  ret = i2c_master_send(tc317x_i2c_client, buf, 1);
  if(ret < 0) { TCDBG("I2C write error %d\n", ret); return TC3X_IO_FAIL; }

  ret = i2c_master_recv(tc317x_i2c_client, buf, 1);
  if(ret < 0) { TCDBG("I2C read error %d\n", ret); return TC3X_IO_FAIL; }

  ret = (unsigned int)(buf[0]);
  return ret;
}
//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Reg_WriteEx
//    Description : Register multi-write
//    Input :
//      moduleidx : module index
//      ChipAddr : chip address
//      RegAddr : Register Address
//      pCh : data pointer
//      iSize : size
//    Output :
//      TC3X_IO_SUCCESS
///    Remark :
//--------------------------------------------------------------------------
TC3X_IO_Err TC317X_IO_I2C_Reg_WriteEx (
    int moduleidx,
    int ChipAddr,
    int RegAddr,
    unsigned char *cData,
    int iSize)
{
  int cMax, remain;
  int i, berr, ret;

  if(!tc317x_i2c_client) return TC3X_IO_FAIL;

  TC317X_IO_IF_LOCK (0);

  berr = 0;
  cMax = iSize/MAX_I2C_BURST;
  remain = iSize%MAX_I2C_BURST;

  gi2c_buff[0] = (unsigned char)(RegAddr)| Bit7;
  for(i = 0; i < cMax; i++) {
    TC317X_IO_memcpy(&gi2c_buff[1], &cData[i*MAX_I2C_BURST], MAX_I2C_BURST);
    ret = i2c_master_send(tc317x_i2c_client, gi2c_buff, MAX_I2C_BURST+1);
    if(ret < 0) { TCDBG("I2C write error %d\n", ret); break; }

    if(ret < 1) {
      TCDBG("I2C Multi write 8 Error!!\n");
      berr = 1; break;
    }
  }

  if(remain && !berr) {
    TC317X_IO_memcpy(&gi2c_buff[1], &cData[cMax*MAX_I2C_BURST], remain);
    ret = i2c_master_send(tc317x_i2c_client, gi2c_buff, remain+1);
    if(ret < 1) TCDBG("I2C Multi write 8 Error!!\n");
  }

  TC317X_IO_IF_UnLOCK (0);

  if(berr)
    return TC3X_IO_FAIL;
  return TC3X_IO_SUCCESS;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Reg_WriteEx_InnerSem
//    Description : Reg write (inside semaphore)
//    Input :
//      moduleidx : module index
//      ChipAddr : chip address
//      RegAddr : Register Address
//      pCh : data
//      iSize : size
//    Output :
//      TC3X_IO_SUCCESS
///    Remark :
//--------------------------------------------------------------------------
TC3X_IO_Err TC317X_IO_I2C_Reg_WriteEx_InnerSem (int moduleidx, int ChipAddr, int RegAddr, unsigned char *cData, int iSize)
{
  int cMax, remain;
  int i, berr, ret;

  if(!tc317x_i2c_client) return TC3X_IO_FAIL;

  berr = 0;
  cMax = iSize/MAX_I2C_BURST;
  remain = iSize%MAX_I2C_BURST;

  gi2c_buff[0] = (unsigned char)(RegAddr)| Bit7;
  for(i=0; i<cMax; i++) {
    TC317X_IO_memcpy(&gi2c_buff[1], &cData[i*MAX_I2C_BURST], MAX_I2C_BURST);
    ret = i2c_master_send(tc317x_i2c_client, gi2c_buff, MAX_I2C_BURST+1);
    if(ret < 0) { TCDBG("I2C write error %d\n", ret); break; }

    if(ret < 1) {
      TCDBG("I2C Multi write 8 Error!!\n");
      berr = 1; break;
    }
  }

  if(remain && !berr) {
    TC317X_IO_memcpy(&gi2c_buff[1], &cData[cMax*MAX_I2C_BURST], remain);
    ret = i2c_master_send(tc317x_i2c_client, gi2c_buff, remain+1);
    if(ret < 1) TCDBG("I2C Multi write 8 Error!!\n");
  }

  if(berr)
    return TC3X_IO_FAIL;
  return TC3X_IO_SUCCESS;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Reg_Write_InnerSem
//    Description : Reg multi-read
//    Input :
//      moduleidx : module index
//      ChipAddr : chip address
//      RegAddr : Register Address
//      data : data pointer
//      iSize : size
//    Output :
//      TC3X_IO_SUCCESS
///    Remark :
//--------------------------------------------------------------------------
TC3X_IO_Err TC317X_IO_I2C_Reg_ReadEx (int moduleidx, int ChipAddr, int RegAddr, unsigned char *cData, int iSize)
{
  char buf[2];
  int cMax, remain;
  int i, ret, berr;

  berr = 0;
  cMax = iSize/MAX_I2C_BURST;
  remain = iSize%MAX_I2C_BURST;

  if(!tc317x_i2c_client) return TC3X_IO_FAIL;

  TC317X_IO_IF_LOCK (0);

  for(i = 0; i < cMax; i++) {
    buf[0] = RegAddr | Bit7;
    ret = i2c_master_send(tc317x_i2c_client, buf, 1);
    if(ret < 1) {
      TCDBG("I2C Multi read 8 Addr Error!!\n");
      berr = 1; break;
    }
    ret = i2c_master_recv(tc317x_i2c_client, &cData[i*MAX_I2C_BURST], MAX_I2C_BURST);
    if (ret < 0) {
      /* ERROR HANDLING: i2c transaction failed */
      PRINTF_LV_0("I2C Multi read 8 data Error!!\n");
      berr = 1; break;
    }
  }

  if(remain && !berr) {
    buf[0] = RegAddr | Bit7;
    ret = i2c_master_send(tc317x_i2c_client, buf, 1);
    if(ret < 1) {
      TCDBG("I2C Multi read 8 Addr Error!!\n");
      berr = 1;
    }
    ret = i2c_master_recv(tc317x_i2c_client, &cData[cMax*MAX_I2C_BURST], remain);
    if (ret < 0) {
      /* ERROR HANDLING: i2c transaction failed */
      TCDBG("I2C Multi read 8 data Error!!\n");
      berr = 1;
    }
  }

  TC317X_IO_IF_UnLOCK (0);

  if(berr == 1)
    return TC3X_IO_FAIL;

  return TC3X_IO_SUCCESS;
}

//--------------------------------------------------------------------------
//    Function name :
//        TC317X_IO_I2C_Reg_ReadEx_InnerSem
//    Description : Reg multi-read (inside semaphore)
//    Input :
//      moduleidx : module index
//      ChipAddr : chip address
//      RegAddr : Register Address
//      data : data pointer
//      iSize : size
//    Output :
//      TC3X_IO_SUCCESS
///    Remark :
//--------------------------------------------------------------------------
TC3X_IO_Err TC317X_IO_I2C_Reg_ReadEx_InnerSem (
    int moduleidx, int ChipAddr, int RegAddr, unsigned char *cData, int iSize)
{
  int cMax, remain;
  char buf[2];
  int i, berr, ret;

  berr = 0;
  cMax = iSize/MAX_I2C_BURST;
  remain = iSize%MAX_I2C_BURST;

  if(!tc317x_i2c_client) return TC3X_IO_FAIL;

  for(i = 0; i < cMax; i++) {
    buf[0] = RegAddr | Bit7;
    ret = i2c_master_send(tc317x_i2c_client, buf, 1);
    if(ret < 1) {
      TCDBG("I2C Multi read 8 Addr Error!!\n");
      berr = 1; break;
    }
    ret = i2c_master_recv(tc317x_i2c_client, &cData[i*MAX_I2C_BURST], MAX_I2C_BURST);
    if (ret < 0) {
      /* ERROR HANDLING: i2c transaction failed */
      PRINTF_LV_0("I2C Multi read 8 data Error!!\n");
      berr = 1; break;
    }
  }

  if(remain && !berr) {
    buf[0] = RegAddr | Bit7;
    ret = i2c_master_send(tc317x_i2c_client, buf, 1);
    if(ret < 1) {
      TCDBG("I2C Multi read 8 Addr Error!!\n");
      berr = 1;
    }
    ret = i2c_master_recv(tc317x_i2c_client, &cData[cMax*MAX_I2C_BURST], remain);
    if (ret < 0) {
      /* ERROR HANDLING: i2c transaction failed */
      TCDBG("I2C Multi read 8 data Error!!\n");
      berr = 1;
    }
  }

  if(berr==1) return TC3X_IO_FAIL;
  return TC3X_IO_SUCCESS;
}

#ifdef LEGACY_DRIVER
static unsigned short normal_i2c[] = {I2C_ADDR, I2C_CLIENT_END };
static unsigned short dummy[] = {I2C_CLIENT_END };
static struct i2c_client_address_data addr_data = {
  .normal_i2c = normal_i2c,
  .probe      = dummy,
  .ignore     = dummy,
};

static int tc317x_i2c_probe(struct i2c_adapter *adap, int addr, int kind)
{
  struct i2c_client *i2c;
  int ret;

  i2c = kzalloc(sizeof(*i2c), GFP_KERNEL);
  if(!i2c) {
    TCDBG("failed to alloc memory! \n");
    return -ENOMEM;
  }

  strcpy(i2c->name, "tc317x");
  i2c->flags = 0;
  i2c->addr = addr;
  i2c->adapter = adap;
  i2c->driver = &tc317x_i2c_driver;

  tc317x_i2c_client = i2c;

  ret = i2c_attach_client(i2c);
  if(ret < 0){
    TCDBG("failed to attach tc317x at addr 0x%X\n", addr);
    goto err;
  }

  return ret;
err:
  kfree(i2c);
  return ret;
}

static int tc317x_i2c_detach(struct i2c_client *client)
{
  i2c_detach_client(client);
  return 0;
}

static int tc317x_i2c_attach(struct i2c_adapter *adap)
{
  int ret = -1;
  if(adap->nr == I2C_BUS) //bus number
    ret = i2c_probe(adap, &addr_data, tc317x_i2c_probe);
  return ret;
}

#define I2C_DRIVERID_TC317X		110
static struct i2c_driver tc317x_i2c_driver = {
  .driver = {
    .name = "tc317x",
    .owner = THIS_MODULE,
  },
  .id = I2C_DRIVERID_TC317X,
  .attach_adapter = tc317x_i2c_attach,
  .detach_client  = tc317x_i2c_detach,
};

#elif defined(NEW_STYLE_DRIVER)
static const struct i2c_device_id tc317x_i2c_id[] = {
  {"tc317x", 0},
  {}
};

MODULE_DEVICE_TABLE(i2c, tc317x_i2c_id);

static int tc317x_i2c_probe(struct i2c_client *i2c,
    const struct i2c_device_id *id)
{
  int ret = 0;

  tc317x_i2c_client = i2c;
  TCDBG("tc317x_i2c_client : 0x%x\n", (unsigned int)i2c);
  return ret;
}

static int tc317x_i2c_remove(struct i2c_client *client)
{
  //struct tc317x_dxb_dev *dxb = i2c_get_clientdata(client);
  TCDBG("tc317x_i2c_client : 0x%x\n", (unsigned int)client);
  return 0;
}

static struct i2c_driver tc317x_i2c_driver = {
  .driver = {
    .name  = "tc317x",
    .owner = THIS_MODULE,
  },
  .probe    = tc317x_i2c_probe,
  .remove   = tc317x_i2c_remove,
  .id_table = tc317x_i2c_id,
};

static int tc317x_add_i2c_device(void)
{
  int ret;
  struct i2c_board_info info;
  struct i2c_adapter *adapter;
  struct i2c_client *client;

  ret = i2c_add_driver(&tc317x_i2c_driver);
  if(ret != 0){
    TCDBG("can't add i2c driver\n");
    return ret;
  }

  memset(&info, 0, sizeof(struct i2c_board_info));
  info.addr = I2C_ADDR;
  strlcpy(info.type, "tc317x", I2C_NAME_SIZE);

  adapter = i2c_get_adapter(I2C_BUS);
  if(!adapter){
    TCDBG("can't get i2c adapter :%d\n", I2C_BUS);
    goto err_driver;
  }

  client = i2c_new_device(adapter, &info);
  i2c_put_adapter(adapter);
  if(!client){
    TCDBG("can't add i2c device at 0x%X\n", (unsigned int)info.addr);
    goto err_driver;
  }

  return 0;

err_driver:
  i2c_del_driver(&tc317x_i2c_driver);
  return -ENODEV;
}
#endif

#endif // USE_IF_I2C
