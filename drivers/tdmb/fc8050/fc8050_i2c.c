/*****************************************************************************
 Copyright(c) 2009 FCI Inc. All Rights Reserved

 File name : fc8050_i2c.c

 Description : fc8050 host interface

 History :
 ----------------------------------------------------------------------
*******************************************************************************/
#include <linux/kernel.h>
#include "../tdmb_comdef.h"
#include "fci_types.h"
#include "fc8050_regs.h"
#include "fc8050_wrapper.h"
//#include "rex.h"

#define HPIC_READ     0x01 // read command
#define HPIC_WRITE    0x02 // write command
#define HPIC_AINC     0x04 // address increment
#define HPIC_BMODE    0x00 // byte mode
#define HPIC_WMODE    0x10 // word mode
#define HPIC_LMODE    0x20 // long mode
#define HPIC_ENDIAN   0x00 // little endian
#define HPIC_CLEAR    0x80 // currently not used

#define I2C_LENGTH_ONE  1

static unsigned char DADR       = 0x58;
static unsigned char IADDR_SIZE = 1;

//LOCAL rex_crit_sect_type  bbm_crit_sect;

static int i2c_bulkread(HANDLE hDevice, u8 addr, u8 *data, u16 length)
{
  if(length == I2C_LENGTH_ONE)
  {
    fc8050_i2c_read_byte(DADR, addr, data, length);  
  }
  else
  {
    fc8050_i2c_read_len(DADR, addr, data, length);
  }

  return 0;
}

static int i2c_bulkwrite(HANDLE hDevice, u8 addr, u8* data, u16 length)
{
  if(length == I2C_LENGTH_ONE)
  {
    fc8050_i2c_write_byte(DADR, addr, data, length);
  }
  else
  {
    fc8050_i2c_write_len(DADR, addr, data, length);
  }

  return 0;
}

static int i2c_dataread(HANDLE hDevice, u8 addr, u8* data, u16 length)
{
  return i2c_bulkread(hDevice, addr, data, length);
}

int fc8050_i2c_init(HANDLE hDevice, u16 param1, u16 param2)
{
  u8 device_addr        = 0x58<<1;
  u8 internal_addr_size = 1;

  // set DADR
  DADR = device_addr;

  // set iaddr size
  IADDR_SIZE = internal_addr_size;

  //rex_init_crit_sect(&bbm_crit_sect);

  return BBM_OK;
}

int fc8050_i2c_byteread(HANDLE hDevice, u16 addr, u8 *data)
{
  int res;
  u8 command = HPIC_READ | HPIC_BMODE | HPIC_ENDIAN;
  unsigned long flag;

  //rex_enter_crit_sect(&bbm_crit_sect);

  local_irq_save(flag);
  res  = i2c_bulkwrite(hDevice, BBM_COMMAND_REG, &command, 1);
  res |= i2c_bulkwrite(hDevice, BBM_ADDRESS_REG, (u8*)&addr, 2);
  res |= i2c_bulkread(hDevice, BBM_DATA_REG, data, 1);
  local_irq_restore(flag);

  //rex_leave_crit_sect(&bbm_crit_sect);

  return res;
}

int fc8050_i2c_wordread(HANDLE hDevice, u16 addr, u16 *data)
{
  int res;
  u8 command = HPIC_READ | HPIC_AINC | HPIC_BMODE | HPIC_ENDIAN;
  unsigned long flag;

  if(BBM_SCI_DATA <= addr && BBM_SCI_SYNCRX >= addr)
    command = HPIC_READ | HPIC_WMODE | HPIC_ENDIAN;

  //rex_enter_crit_sect(&bbm_crit_sect);

  local_irq_save(flag);
  res  = i2c_bulkwrite(hDevice, BBM_COMMAND_REG, &command, 1);
  res |= i2c_bulkwrite(hDevice, BBM_ADDRESS_REG, (u8*)&addr, 2);
  res |= i2c_bulkread(hDevice, BBM_DATA_REG, (u8*)data, 2);
  local_irq_restore(flag);

  //rex_leave_crit_sect(&bbm_crit_sect);

  return res;
}

int fc8050_i2c_longread(HANDLE hDevice, u16 addr, u32 *data)
{
  int res;
  u8 command = HPIC_READ | HPIC_AINC | HPIC_BMODE | HPIC_ENDIAN;
  unsigned long flag;

  //rex_enter_crit_sect(&bbm_crit_sect);

  local_irq_save(flag);
  res  = i2c_bulkwrite(hDevice, BBM_COMMAND_REG, &command, 1);
  res |= i2c_bulkwrite(hDevice, BBM_ADDRESS_REG, (u8*)&addr, 2);
  res |= i2c_bulkread(hDevice, BBM_DATA_REG, (u8*)data, 4);
  local_irq_restore(flag);

  //rex_leave_crit_sect(&bbm_crit_sect);

  return res;
}

int fc8050_i2c_bulkread(HANDLE hDevice, u16 addr, u8 *data, u16 length)
{
  int res;
  u8 command = HPIC_READ | HPIC_AINC | HPIC_BMODE | HPIC_ENDIAN;
  unsigned long flag;

  //rex_enter_crit_sect(&bbm_crit_sect);

  local_irq_save(flag);
  res  = i2c_bulkwrite(hDevice, BBM_COMMAND_REG, &command, 1);
  res |= i2c_bulkwrite(hDevice, BBM_ADDRESS_REG, (u8*)&addr, 2);
  res |= i2c_bulkread(hDevice, BBM_DATA_REG, data, length);
  local_irq_restore(flag);

  //rex_leave_crit_sect(&bbm_crit_sect);

  return res;
}

int fc8050_i2c_bytewrite(HANDLE hDevice, u16 addr, u8 data)
{
  int res;
  u8 command = HPIC_WRITE | HPIC_BMODE | HPIC_ENDIAN;
  unsigned long flag;

  //rex_enter_crit_sect(&bbm_crit_sect);

  local_irq_save(flag);
  res  = i2c_bulkwrite(hDevice, BBM_COMMAND_REG, &command, 1);
  res |= i2c_bulkwrite(hDevice, BBM_ADDRESS_REG, (u8*)&addr, 2);
  res |= i2c_bulkwrite(hDevice, BBM_DATA_REG, (u8*)&data, 1);
  local_irq_restore(flag);

  //rex_leave_crit_sect(&bbm_crit_sect);

  return res;
}

int fc8050_i2c_wordwrite(HANDLE hDevice, u16 addr, u16 data)
{
  int res;
  u8 command = HPIC_WRITE | HPIC_AINC | HPIC_BMODE | HPIC_ENDIAN;
  unsigned long flag;

  if(BBM_SCI_DATA <= addr && BBM_SCI_SYNCRX >= addr)
    command = HPIC_WRITE | HPIC_WMODE | HPIC_ENDIAN;

  //rex_enter_crit_sect(&bbm_crit_sect);

  local_irq_save(flag);
  res  = i2c_bulkwrite(hDevice, BBM_COMMAND_REG, &command, 1);
  res |= i2c_bulkwrite(hDevice, BBM_ADDRESS_REG, (u8*)&addr, 2);
  res |= i2c_bulkwrite(hDevice, BBM_DATA_REG, (u8*)&data, 2);
  local_irq_restore(flag);

  //rex_leave_crit_sect(&bbm_crit_sect);

  return res;
}

int fc8050_i2c_longwrite(HANDLE hDevice, u16 addr, u32 data)
{
  int res;
  u8 command = HPIC_WRITE | HPIC_AINC | HPIC_BMODE | HPIC_ENDIAN;
  unsigned long flag;

  //rex_enter_crit_sect(&bbm_crit_sect);

  local_irq_save(flag);
  res  = i2c_bulkwrite(hDevice, BBM_COMMAND_REG, &command, 1);
  res |= i2c_bulkwrite(hDevice, BBM_ADDRESS_REG, (u8*)&addr, 2);
  res |= i2c_bulkwrite(hDevice, BBM_DATA_REG, (u8*)&data, 4);
  local_irq_restore(flag);

  //rex_leave_crit_sect(&bbm_crit_sect);

  return res;
}

int fc8050_i2c_bulkwrite(HANDLE hDevice, u16 addr, u8* data, u16 length)
{
  int res;
  u8 command = HPIC_WRITE | HPIC_AINC | HPIC_BMODE | HPIC_ENDIAN;
  unsigned long flag;

  //rex_enter_crit_sect(&bbm_crit_sect);

  local_irq_save(flag);
  res  = i2c_bulkwrite(hDevice, BBM_COMMAND_REG, &command, 1);
  res |= i2c_bulkwrite(hDevice, BBM_ADDRESS_REG, (u8*)&addr, 2);
  res |= i2c_bulkwrite(hDevice, BBM_DATA_REG, data, length);
  local_irq_restore(flag);

  //rex_leave_crit_sect(&bbm_crit_sect);

  return res;
}

int fc8050_i2c_dataread(HANDLE hDevice, u16 addr, u8* data, u16 length)
{
  int res;
  u8 command = HPIC_READ | HPIC_BMODE | HPIC_ENDIAN;
  unsigned long flag;

  //rex_enter_crit_sect(&bbm_crit_sect);

  local_irq_save(flag);
  res  = i2c_bulkwrite(hDevice, BBM_COMMAND_REG, &command, 1);
  res |= i2c_bulkwrite(hDevice, BBM_ADDRESS_REG, (u8*)&addr, 2);
  res |= i2c_dataread(hDevice, BBM_DATA_REG, data, length);
  local_irq_restore(flag);

  //rex_leave_crit_sect(&bbm_crit_sect);

  return res;
}

int fc8050_i2c_deinit(HANDLE hDevice)
{
  return BBM_OK;
}
