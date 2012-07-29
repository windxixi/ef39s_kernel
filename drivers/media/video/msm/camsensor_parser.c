
//#include <stdio.h>
//#include <stdlib.h>

//#include "customer.h"
//#include "comdef.h" 
#include <linux/types.h>
#include <linux/string.h>
#include "camsensor_parser.h"

#define MAX_SENSORPARAM_SIZE (10*1024)
#define VERSTR "//VERSION"
#define MAX_TOKEN 20
#define SLASH 0x2F
#define SPACE 0x20
#define CR 0x0D
#define LF 0x0A
#define TAB 0x09
#define COLON 0x3A
#define G_CHAR 0x47
#define g_CHAR 0x67
#define COMMA 0x2C
#define RBRACE 0x7B
#define LBRACE 0x7D

 
static unsigned char *find_next_token(unsigned char *fpage, unsigned char *token)
{
  unsigned char *cur_ptr;
  unsigned char temp;
  int idx=0;

  cur_ptr = fpage;
  temp = *cur_ptr;
  memset(token, (int)NULL, MAX_TOKEN);

  while(temp)
  {
    if( temp==SLASH)
    {
      if(*(cur_ptr+1)==SLASH)
      {
        cur_ptr = (unsigned char *)strchr((char *)cur_ptr, (int)LF);
        if(idx>0)
          break;
      }
      else
      {
        token[idx++]=*cur_ptr;
      }
    }
    else if(temp==SPACE || temp==TAB ||temp==LF||temp==CR||temp==COMMA || temp == RBRACE || temp == LBRACE)
    {
      if(idx > 0 )
        break;
    }
    else
    {
      token[idx++]=*cur_ptr;
    }
    temp = *(++cur_ptr);  
  }

  if(idx)
    return cur_ptr;
  else
    return NULL;
}


#define HEXADECIMAL 16
#define DECIMAL 10
#define BINARY 2




static unsigned long convert_str_to_number(unsigned char *token)
{
  unsigned char scale;
  int ret_val=0;
  int diff;
  unsigned char temp;
  
  if(!token || *token==0)
    return 0;

  if(*token=='0' && (*(token+1)=='x' || *(token+1)=='X'))
  {
    scale = HEXADECIMAL;
    token+=2;
  }
  else if(*token=='0' && (*(token+1)=='b' || *(token+1)=='B'))
  {
    scale = BINARY;
    token+=2;
  }
  else
  {
    scale = DECIMAL;
  }

//20070622_c@m_이창원_chg [ 
#if 1
  while((temp = *token) != 0)
#else
  while((temp = *token) != NULL)
#endif
  {
    if(temp < COLON)
      diff = SLASH+1;
    else if(temp < G_CHAR)
      diff = 'A' - 10;
    else if(temp < g_CHAR)
      diff = 'a' -10;
    else
      return 0;
    ret_val = ret_val*scale + (*(token++) -diff);
  }
  return ret_val;
}  

//20070622_c@m_이창원_blk [ 
#if 0
static unsigned char *find_param_version(unsigned char *fpage, unsigned short *version)
{
  unsigned char *cur_ptr;
  unsigned char token[MAX_TOKEN];
  if((cur_ptr = (unsigned char *)strstr((char *)fpage,VERSTR)))
  {
    cur_ptr+= (sizeof(VERSTR)-1);
    cur_ptr = find_next_token(cur_ptr,token);
    if(cur_ptr)
      *version = (unsigned short)convert_str_to_number(token);
  }
  return cur_ptr;
}
#endif//20070622_c@m_이창원_blk_end

#if 0
static unsigned char *find_next_address(unsigned char *fpage, unsigned short *address)
{
  unsigned char *cur_ptr;
  unsigned char token[MAX_TOKEN];
  memset(token, (int)NULL, sizeof(token));
  cur_ptr = find_next_token(fpage, token);
  if(cur_ptr)
    *address = (unsigned short)convert_str_to_number(token);
  return cur_ptr;
}

static unsigned char *find_next_val(unsigned char *fpage, u_int16_t*value)
{
  unsigned char *cur_ptr;
  unsigned char token[MAX_TOKEN];
  cur_ptr = find_next_token(fpage, token);
  if(cur_ptr)
    *value = (u_int16_t)convert_str_to_number(token);

  return cur_ptr;
}
#endif

//FEATURE_PANTECH_CAMERA_TODO
unsigned char * camsensor_parser_find_addr
(
    unsigned char *fpage, 
    unsigned short *address
)
{
    unsigned char *cur_ptr;
    unsigned char token[MAX_TOKEN];
    memset(token, (int)NULL, sizeof(token));
    cur_ptr = find_next_token(fpage, token);
    if (cur_ptr)
    {
        *address = (unsigned short)convert_str_to_number(token);
    }
    return cur_ptr;
}

//FEATURE_PANTECH_CAMERA_TODO
unsigned char * camsensor_parser_find_data
(
    unsigned char *fpage, 
    u_int16_t *value
)
{
    unsigned char *cur_ptr;
    unsigned char token[MAX_TOKEN];
    cur_ptr = find_next_token(fpage, token);
    if (cur_ptr)
    {
        *value = (u_int16_t)convert_str_to_number(token);
    }
    return cur_ptr;
}

//FEATURE_PANTECH_CAMERA_TODO
int8_t* camsensor_parser_find_line 
(
    u_int8_t *str
)
{
    int8_t *tmp_str = str;

    if (tmp_str == NULL)
    {
        return NULL;
    }

    for (;;)
    {
        if (*tmp_str == '\n')
        {
            break;
        }
        tmp_str++;
    }
    tmp_str++;

    return tmp_str;
}

#if 0 //[[ PANTECH_CAMERA_PSJ_0100405_BEGIN -- 포팅 수정
uint32 camsensor_reg_file_read
(
    const char *filename,
    uint32 register_set_size,
    camsensor_parser_register_t *register_set
)
{
  //int i;
  fs_rsp_msg_type        rsp_msg;     
  fs_handle_type         fhandle = 0;

  unsigned char *param_file;
  unsigned char *temp_ptr;
  uint32 file_size;
  uint32 register_index = 0;

  //u_int16_t param_version;

  u_int16_t reg_address;
  u_int16_t reg_val;


  fs_nametest(filename, FS_TEST_FILE, NULL, &rsp_msg);

  if ( !rsp_msg.nametest.name_found )
  {
    MSG_FATAL(" File Not Found! ",0,0,0);
    return 0;
  } 
  else 
  {  //when SensorParamTable exists
    fs_open(filename, FS_OA_READONLY, NULL, NULL, &rsp_msg);

    MSG_FATAL(" File Open O.K ! ",0,0,0);

    if (rsp_msg.open.handle == FS_NULL_HANDLE)
    {
      MSG_ERROR ("File was not opened!!!",0,0,0);
      fs_close(fhandle,NULL,&rsp_msg);
      return 0;
    }
    fhandle = rsp_msg.open.handle;

    fs_file_size(filename, NULL, &rsp_msg);
    if (rsp_msg.file_size.status != FS_OKAY_S)
    {
      MSG_ERROR ("File size read fail!!!",0,0,0);
      rsp_msg.file_size.size = MAX_SENSORPARAM_SIZE;
      fs_close(fhandle,NULL,&rsp_msg);
      ERR_FATAL("ERR!",0,0,0);
      return 0;
    }
    file_size = rsp_msg.file_size.size;

    param_file = (unsigned char *)malloc(file_size+2);

    if(param_file==NULL)
    {
      fs_close(fhandle,NULL,&rsp_msg);
      ERR_FATAL("ERR!",0,0,0);
      return 0;
    }

    *(param_file+file_size)= NULL;
    *(param_file+file_size+1)=NULL;

    fs_read ( fhandle, (void *)param_file, file_size, NULL, &rsp_msg );
  
    if ( rsp_msg.read.status != FS_OKAY_S )
    {
      MSG_ERROR ("File read file header fail!!!",0,0,0);
      free((void *)param_file);
      fs_close(fhandle,NULL,&rsp_msg);
      return 0;
    }
    else
    {
      fs_close(fhandle, NULL, &rsp_msg);
      if ( rsp_msg.close.status != FS_OKAY_S )
      {
        MSG_ERROR ("File closing fail!!!",0,0,0);
      }

      temp_ptr = param_file;

      while (temp_ptr)
      {  
//20070622_c@m_이창원_chg [ 
//        if ((temp_ptr=find_next_address(temp_ptr, &reg_address)))
        if ((temp_ptr=find_next_address(temp_ptr, &reg_address)) != NULL)
        {
//20070622_c@m_이창원_chg [ 
//          if ((temp_ptr=find_next_val(temp_ptr, &reg_val)))
          if ((temp_ptr=find_next_val(temp_ptr, &reg_val)) != NULL)
          {
              register_set[register_index].address = reg_address;
              register_set[register_index].value = reg_val;
              register_index++;
              if (register_index >= register_set_size)
              {
                ERR_FATAL("ERR!",0,0,0);
                return 0;
              }
          }
        }
      }
    }

    free((void *)param_file);
  }

  return register_index;
}
#endif //]] PANTECH_CAMERA_PSJ_0100405_END -- 포팅 수정



