/* drivers/misc/apanic_pantech.c
 *
 * Copyright (C) 2011 PANTECH, Co. Ltd.
 * based on drivers/misc/apanic.c
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.      See the
 * GNU General Public License for more details.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/wait.h>
#include <linux/wakelock.h>
#include <linux/platform_device.h>
#include <linux/uaccess.h>
#include <linux/notifier.h>
#include <linux/debugfs.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/preempt.h>
#include <asm/cacheflush.h>
#include <asm/system.h>
#include <linux/fb.h>
#include <linux/time.h>

#include "smd_private.h"
#include "modem_notifier.h"

#ifdef CONFIG_SW_RESET
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/nmi.h>
#include <mach/msm_iomap.h>
#endif

#define POS(x) (x > 0 ? x : 0)
#define WRITE_LOG(...) \
  if(bufsize != 0) { \
    n = snprintf(s, POS(bufsize), __VA_ARGS__); \
    s+=n; \
    total +=n; \
    bufsize-=n;\
  }

#define MMU_SCRIPT_BUF_SIZE 512
#define EXCP_SCRIPT_BUF_SIZE 512

#define LOG_SIZE 256

extern void ram_console_enable_console(int);

#define PANIC_MAGIC      0xDAEDDAED
#define PHDR_VERSION   0x01

struct painc_info_date {
      unsigned short year;
      unsigned short month;
      unsigned short day;
      unsigned short hour;
      unsigned short minute;
      unsigned short second;
};

struct kernel_stack_info {
      char kernel_stack[LOG_SIZE];
      int stack_dump_size;
 };

struct panic_info {
      unsigned int magic;
      unsigned int version;
      struct painc_info_date date;

      char mmu_cmm_script[MMU_SCRIPT_BUF_SIZE];
      int mmu_cmm_size;

      unsigned int console_offset;
      unsigned int console_length;

      unsigned int threads_offset;
      unsigned int threads_length;
      
      unsigned int logcat_offset;
      unsigned int logcat_length;
      unsigned int total_size;
      
      struct kernel_stack_info stack;      
};

struct apanic_config {
      unsigned int initialized;
      unsigned buf_size;
      unsigned int writesize;
      void *bounce; 
};

static struct apanic_config driver_context;

static int in_panic = 0;

static char stack_dump[LOG_SIZE];

static int logindex;

static bool kernel_exception_status = 0;

extern int log_buf_copy(char *dest, int idx, int len);
extern void log_buf_clear(void);

extern int logcat_buf_copy(char *dest,int idx,int len);
extern void logcat_set_log(int index);

// paiksun...
#ifdef CONFIG_SW_RESET
extern int pm8058_get_pwrkey_status(void);
#endif

#define QUAD_YEAR (366+(3*365))

/*
 * The year_tab table is used for determining the number of days which
 * have elapsed since the start of each year of a leap year set. It has
 * 1 extra entry which is used when trying to find a 'bracketing' year.
 * The search is for a day >= year_tab[i] and day < year_tab[i+1].
 */
static const unsigned short year_tab[] = {
  0,                              /* Year 0 (leap year) */
  366,                            /* Year 1             */
  366+365,                        /* Year 2             */
  366+365+365,                    /* Year 3             */
  366+365+365+365                 /* Bracket year       */
};

/*
 * The norm_month_tab table holds the number of cumulative days that have
 * elapsed as of the end of each month during a non-leap year.
 */
static const unsigned short norm_month_tab[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+28,                                /* Feb */
  31+28+31,                             /* Mar */
  31+28+31+30,                          /* Apr */
  31+28+31+30+31,                       /* May */
  31+28+31+30+31+30,                    /* Jun */
  31+28+31+30+31+30+31,                 /* Jul */
  31+28+31+30+31+30+31+31,              /* Aug */
  31+28+31+30+31+30+31+31+30,           /* Sep */
  31+28+31+30+31+30+31+31+30+31,        /* Oct */
  31+28+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+28+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/*
 * The leap_month_tab table holds the number of cumulative days that have
 * elapsed as of the end of each month during a leap year.
 */
static const unsigned short leap_month_tab[] = {
  0,                                    /* --- */
  31,                                   /* Jan */
  31+29,                                /* Feb */
  31+29+31,                             /* Mar */
  31+29+31+30,                          /* Apr */
  31+29+31+30+31,                       /* May */
  31+29+31+30+31+30,                    /* Jun */
  31+29+31+30+31+30+31,                 /* Jul */
  31+29+31+30+31+30+31+31,              /* Aug */
  31+29+31+30+31+30+31+31+30,           /* Sep */
  31+29+31+30+31+30+31+31+30+31,        /* Oct */
  31+29+31+30+31+30+31+31+30+31+30,     /* Nov */
  31+29+31+30+31+30+31+31+30+31+30+31   /* Dec */
};

/*
 * The day_offset table holds the number of days to offset as of the end of
 * each year.
 */
static const unsigned short day_offset[] = {
  1,                                    /* Year 0 (leap year) */
  1+2,                                  /* Year 1             */
  1+2+1,                                /* Year 2             */
  1+2+1+1                               /* Year 3             */
};

/*****************************************************
 * ERROR LOG DISPLAY ROUTINE
 * **************************************************/
/* For Display */
#define DISPLAY_LOG_SIZE	1024
//static char *pDisplayBuffer;
//static int DisplayLogIdx = 0;
static atomic_t is_displayed = ATOMIC_INIT(0);

#if 0
extern struct fb_info *registered_fb[FB_MAX];

static int cx, cy, cmaxx, cmaxy;
#ifdef CONFIG_F_SKYDISP_FRAMEBUFFER_32
static const unsigned int BGCOLOR = 0x00FF0000;
static const unsigned int FGCOLOR = 0x00FFFFFF;
#else
static const unsigned short BGCOLOR = 0x001F;
static const unsigned short FGCOLOR = 0xFFFF;
#endif

static const unsigned font5x12[] = {
  0x00000000, 0x00000000,
  0x08421080, 0x00020084,
  0x00052940, 0x00000000,
  0x15f52800, 0x0000295f,
  0x1c52f880, 0x00023e94,
  0x08855640, 0x0004d542,
  0x04528800, 0x000b2725,
  0x00021080, 0x00000000,
  0x04211088, 0x00821042,
  0x10841082, 0x00221108,
  0x09575480, 0x00000000,
  0x3e420000, 0x00000084,
  0x00000000, 0x00223000,
  0x3e000000, 0x00000000,
  0x00000000, 0x00471000,
  0x08844200, 0x00008442,
  0x2318a880, 0x00022a31,
  0x08429880, 0x000f9084,
  0x1108c5c0, 0x000f8444,
  0x1c4443e0, 0x00074610,
  0x14a62100, 0x000423e9,
  0x26d087e0, 0x00074610,
  0x1e10c5c0, 0x00074631,
  0x088443e0, 0x00010844,
  0x1d18c5c0, 0x00074631,
  0x3d18c5c0, 0x00074610,
  0x08e20000, 0x00471000,
  0x08e20000, 0x00223000,
  0x02222200, 0x00082082,
  0x01f00000, 0x000003e0,
  0x20820820, 0x00008888,
  0x1108c5c0, 0x00020084,
  0x2b98c5c0, 0x000f05b5,
  0x2318a880, 0x0008c63f,
  0x1d2949e0, 0x0007ca52,
  0x0210c5c0, 0x00074421,
  0x252949e0, 0x0007ca52,
  0x1e1087e0, 0x000f8421,
  0x1e1087e0, 0x00008421,
  0x0210c5c0, 0x00074639,
  0x3f18c620, 0x0008c631,
  0x084211c0, 0x00071084,
  0x10842380, 0x00032508,
  0x0654c620, 0x0008c525,
  0x02108420, 0x000f8421,
  0x2b5dc620, 0x0008c631,
  0x2b59ce20, 0x0008c739,
  0x2318c5c0, 0x00074631,
  0x1f18c5e0, 0x00008421,
  0x2318c5c0, 0x01075631,
  0x1f18c5e0, 0x0008c525,
  0x1c10c5c0, 0x00074610,
  0x084213e0, 0x00021084,
  0x2318c620, 0x00074631,
  0x1518c620, 0x0002114a,
  0x2b18c620, 0x000556b5,
  0x08a54620, 0x0008c54a,
  0x08a54620, 0x00021084,
  0x088443e0, 0x000f8442,
  0x0421084e, 0x00e10842,
  0x08210420, 0x00084108,
  0x1084210e, 0x00e42108,
  0x0008a880, 0x00000000,
  0x00000000, 0x01f00000,
  0x00000104, 0x00000000,
  0x20e00000, 0x000b663e,
  0x22f08420, 0x0007c631,
  0x22e00000, 0x00074421,
  0x23e84200, 0x000f4631,
  0x22e00000, 0x0007443f,
  0x1e214980, 0x00010842,
  0x22e00000, 0x1d187a31,
  0x26d08420, 0x0008c631,
  0x08601000, 0x00071084,
  0x10c02000, 0x0c94a108,
  0x0a908420, 0x0008a4a3,
  0x084210c0, 0x00071084,
  0x2ab00000, 0x0008d6b5,
  0x26d00000, 0x0008c631,
  0x22e00000, 0x00074631,
  0x22f00000, 0x0210be31,
  0x23e00000, 0x21087a31,
  0x26d00000, 0x00008421,
  0x22e00000, 0x00074506,
  0x04f10800, 0x00064842,
  0x23100000, 0x000b6631,
  0x23100000, 0x00022951,
  0x23100000, 0x000556b5,
  0x15100000, 0x0008a884, 
  0x23100000, 0x1d185b31,
  0x11f00000, 0x000f8444,
  0x06421098, 0x01821084,
  0x08421080, 0x00021084,
  0x30421083, 0x00321084,
  0x0004d640, 0x00000000,
  0x00000000, 0x00000000, 
};


#ifdef CONFIG_F_SKYDISP_FRAMEBUFFER_32
static void drawglyph(unsigned int *base, int pixels, unsigned int paint,
                      unsigned stride, const unsigned *glyph, int bpp)
{
    unsigned x, y, data;
    stride -= 5;

    data = glyph[0];
    for (y = 0; y < 6; y++) {
        for (x = 0; x < 5; x++) {
            if (data & 1) {
                base[pixels] = paint;
            }
            data >>= 1;
            pixels++;
        }
        pixels += stride;
    }
    data = glyph[1];
    for (y = 0; y < 6; y++) {
        for (x = 0; x < 5; x++) {
            if (data & 1) {
                base[pixels] = paint;
            }
            data >>= 1;
            pixels++;
        }
        pixels += stride;
    }
}                    
#else
static void drawglyph(unsigned char *base, int pixels, unsigned short paint,
                      unsigned stride, const unsigned *glyph, int bpp)
{
    unsigned x, y, data;
    stride -= 5;

    data = glyph[0];
    for(y = 0; y < 6; y++) {
        for(x = 0; x < 5; x++) {
            if(data & 1) {
                if(bpp == 2) {
                    ((unsigned short *)base)[pixels] = paint;
                } else if(bpp == 3){
                    base[pixels*3] = (paint & 0xF800) >> 11;
                    base[pixels*3 + 1] = (paint & 0x07E0) >> 5;
                    base[pixels*3 + 2] = (paint & 0x001F);
                }
            }
            data >>= 1;
            pixels++;
        }
        pixels += stride;
    }
    data = glyph[1];
    for(y = 0; y < 6; y++) {
        for(x = 0; x < 5; x++) {
            if(data & 1) {
                if(bpp == 2) {
                    ((unsigned short *)base)[pixels] = paint;
                } else if(bpp == 3){
                    base[pixels*3] = (paint & 0xF800) >> 11;
                    base[pixels*3 + 1] = (paint & 0x07E0) >> 5;
                    base[pixels*3 + 2] = (paint & 0x001F);
                }
            }
            data >>= 1;
            pixels++;
        } 
        pixels += stride;
    }
}
#endif

static void display_log(const char *str, int size)
{
    struct fb_info *info;
    unsigned short c;
    int i, j,k, count;
    int fb_width, fb_height;
    int fb_num;
    int bpp;
    int t_cx, t_cy;
    if (size <= 0) return;

    info = registered_fb[0];
    fb_width = info->var.xres;
    fb_height = info->var.yres;

    if (cx == 0 && cy == 0 && cmaxx == 0 && cmaxy == 0) { 
        cmaxx = fb_width / 6;
        cmaxy = (fb_height - 1) / 12;
        cx = cy = 0;
    }
    bpp = info->var.bits_per_pixel >> 3; // / 8;
    fb_num = info->fix.smem_len / (fb_width * fb_height * bpp);

    if (fb_num > 2 ) fb_num = 2;

    t_cx = t_cy = 0;
#ifdef CONFIG_F_SKYDISP_FRAMEBUFFER_32
    for (k = 0; k < fb_num; k++ ) {
        unsigned int *base;

        base  = ((unsigned int *)info->screen_base) + fb_width * fb_height * k;
        t_cx = cx; t_cy = cy;

        count = fb_width * 12;

        j = t_cy * fb_width * 12;

        while (count--) {                
            base[j++] = BGCOLOR;
        }

        for (i = 0; i < size; i++) {
            c = str[i];
            if (c > 127) continue;
            if (c < 32) {
                if (c == '\n') {
                    t_cy++;
                    t_cx = 0;
                    count = fb_width * 12;
                    j = t_cy * fb_width * 12;
                    while (count--) {                             
                        base[j++] = BGCOLOR;
                    }
                }
                continue;
            }
            drawglyph(base, t_cy * 12 * fb_width + t_cx * 6, FGCOLOR,
                    fb_width, font5x12 + (c - 32) * 2, bpp);
            t_cx++;
            if (t_cx >= cmaxx ) {
                t_cy++;
                t_cx = 0;
                count = fb_width * 12;
                j = t_cy * fb_width * 12;
                while (count--) {                        
                    base[j++] = BGCOLOR;
                }
            }
        }
    }
    cx = t_cx; cy = t_cy;
    cy++; cx= 0;
#else    
    if (bpp == 2) { // RGB565
        for(k = 0; k < fb_num; k++ ){
            unsigned short *base;

            base  = (unsigned short *)(((unsigned char *)info->screen_base) 
                    + fb_width * fb_height * bpp * k);
            t_cx = cx; t_cy = cy;

            count = fb_width * 12;  
            j = t_cy * fb_width * 12;
            while(count--) {
                base[j++] = BGCOLOR;
            }

            for(i = 0; i < size; i++) {
                c = str[i];
                if(c > 127) continue;
                if(c < 32){
                    if(c == '\n') {
                        t_cy++;
                        t_cx = 0;
                        count = fb_width * 12;
                        j = t_cy * fb_width * 12;
                        while(count--) {
                            base[j++] = BGCOLOR;
                        }
                    }
                    continue;
                }

                drawglyph((unsigned char *)base, t_cy * 12 * fb_width + t_cx * 6, FGCOLOR, 
                                 fb_width, font5x12 + (c - 32) * 2, bpp );
                t_cx++;
                if(t_cx >= cmaxx ){
                    t_cy++;
                    t_cx = 0;
                    count = fb_width * 12;
                    j = t_cy * fb_width * 12;
                    while(count--) {
                        base[j++] = BGCOLOR;
                    }
                }
            }

        }
        cx = t_cx; cy = t_cy;   

    } else { //RGB888
        for (k = 0; k < fb_num; k++ ) {
            unsigned char *base;

            base  = ((unsigned char *)info->screen_base) + fb_width * fb_height * bpp * k;
            t_cx = cx; t_cy = cy;

            count = fb_width * 12;  
            j = t_cy * fb_width * 12;
            while(count--) {
                base[j++] = (BGCOLOR & 0xF800) >> 11;
                base[j++] = (BGCOLOR & 0x07E0) >> 5;
                base[j++] = (BGCOLOR & 0x001F);
            }

            for(i = 0; i < size; i++) {
                c = str[i];
                if(c > 127) continue;
                if(c < 32){
                    if(c == '\n') {
                        t_cy++;
                        t_cx = 0;
                        count = fb_width * 12;
                        j = t_cy * fb_width * 12;
                        while(count--) {
                            base[j++] = (BGCOLOR & 0xF800) >> 11;
                            base[j++] = (BGCOLOR & 0x07E0) >> 5;
                            base[j++] = (BGCOLOR & 0x001F);
                        }
                    }
                    continue;
                }

                drawglyph((unsigned char*)base , t_cy * 12 * fb_width + t_cx * 6, FGCOLOR, 
                                 fb_width, font5x12 + (c - 32) * 2, bpp );
                t_cx++;
                if( t_cx >= cmaxx ){
                    t_cy++;
                    t_cx = 0;
                    count = fb_width * 12;
                    j = t_cy * fb_width * 12;
                    while(count--) {
                        base[j++] = (BGCOLOR & 0xF800) >> 11;
                        base[j++] = (BGCOLOR & 0x07E0) >> 5;
                        base[j++] = (BGCOLOR & 0x001F);
                    }
                }
            }
        }
        cx = t_cx; cy = t_cy;   
    }
    cy++; cx= 0;
#endif    
}

extern void force_mdp_on(void);
//extern void force_lcd_screen_on(void);
static void lcd_panel_power_on(bool clock_on)
{
    // [PS1] Kang Seong-Goo, Must be called
    force_mdp_on();
#if 0
    if (clock_on) {
        //force_lcd_screen_on();
    }
#endif
}

extern void force_mdp4_overlay_unset(void);
#endif

static DEFINE_SPINLOCK(pause_on_errlog_lock);

#define WDT0_RST	(MSM_TMR0_BASE + 0x38)
void pantech_errlog_display_put_log(const char *log, int size)
{
    unsigned long flags;
    //lock -> unlock skip because of blue screen disappearing and block interrupt (Q6 etc)
    spin_lock_irqsave(&pause_on_errlog_lock, flags);

    if (atomic_read(&is_displayed) == 0) {
        atomic_set(&is_displayed, 1);
#if 0
        preempt_disable();
        bust_spinlocks(1);

        force_mdp4_overlay_unset();
        
        display_log(log, size);
 
        bust_spinlocks(0);
        preempt_enable(); 

        lcd_panel_power_on(true);

// paiksun...
#ifdef CONFIG_SW_RESET
        while(!pm8058_get_pwrkey_status())
        {
                writel(1, WDT0_RST);
                mdelay(1000);
//                printk(KERN_INFO " pwrkey in released..");
        }
#endif
#endif
    }

    return;
}
EXPORT_SYMBOL(pantech_errlog_display_put_log);
/*************************************************************************
 * END DISPLAY ROUTINE ***************************************************
 * **********************************************************************/

/*****************************************************
 * ERROR LOG TIME  ROUTINE
 * **************************************************/
static unsigned int tmr_GetLocalTime(void)
{
      struct timeval tv;
      unsigned int seconds;

      do_gettimeofday(&tv);
      seconds = (unsigned int)tv.tv_sec;
    
      /* Offset to sync timestamps between Arm9 & Arm11
      Number of seconds between Jan 1, 1970 & Jan 6, 1980 */
      seconds = seconds - (10UL*365+5+2)*24*60*60 ;

      return seconds;
}

static unsigned int div4x2( unsigned int dividend, unsigned short divisor, unsigned short *rem_ptr)
{
      *rem_ptr = (unsigned short) (dividend % divisor);
      return (dividend /divisor);
}

static void clk_secs_to_julian(unsigned int secs, struct painc_info_date *julian_ptr)
{
      int i;
      unsigned short days;

      /* 5 days (duration between Jan 1 and Jan 6), expressed as seconds. */
      secs += 432000;
      /* GMT to Local time */
      secs += 32400;
  
      secs = div4x2( secs, 60, &julian_ptr->second );
      secs = div4x2( secs, 60, &julian_ptr->minute );
      secs = div4x2( secs, 24, &julian_ptr->hour );
      julian_ptr->year = 1980 + ( 4 * div4x2( secs, QUAD_YEAR, &days));

      for(i = 0; days >= year_tab[i + 1]; i++);
      days -= year_tab[i];
      julian_ptr->year += i;

      if(i == 0)
      {
            for(i = 0; days >= leap_month_tab[i + 1]; i++ );
            julian_ptr->day = days - leap_month_tab[i] + 1;
            julian_ptr->month = i + 1;

      } else {
            for(i = 0; days >= norm_month_tab[i + 1]; i++ );
            julian_ptr->day = days - norm_month_tab[i] + 1;
            julian_ptr->month = i + 1;
      }
}

static void tmr_GetDate( unsigned int dseconds, struct painc_info_date *pDate)
{
      if(pDate) {
            if(dseconds == 0) {
                memset(pDate, 0x00, sizeof(struct painc_info_date));
                pDate->year = 1980;
                pDate->month = 1;
                pDate->day = 6;
            } else {
                clk_secs_to_julian(dseconds, (struct painc_info_date *)pDate);
            }
      }
}
/*************************************************************************
 * END LOG TIME ROUTINE ***************************************************
 * **********************************************************************/      
void apainc_kernel_stack_dump(const char *name, int size)
{
      int length;

      if(!kernel_exception_status) 
            return;

      if(logindex >= LOG_SIZE - 1 - 1)
            return;

 
      if((logindex + size + 1) > LOG_SIZE - 1) 
            length = LOG_SIZE - 1 - logindex - 1; 
      else
            length = size;

      if(logindex != 0) {
            stack_dump[logindex++] = ' ';
            stack_dump[logindex++] = '<';
      }
      memcpy(&stack_dump[logindex], name, length);
      logindex += length;
}
EXPORT_SYMBOL(apainc_kernel_stack_dump);

void apainc_kernel_stack_dump_start(void)
{
      kernel_exception_status = 1;
 
      logindex = 0;
      memset(stack_dump, 0x00, LOG_SIZE);
}
EXPORT_SYMBOL(apainc_kernel_stack_dump_start);

void apainc_kernel_stack_dump_end(void)
{
      kernel_exception_status = 0;
}
EXPORT_SYMBOL(apainc_kernel_stack_dump_end);

static inline unsigned get_ctrl(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c1,c0,0" : "=r" (val));
      return val;
}

static inline unsigned get_transbase0(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c2,c0,0" : "=r" (val));
      return val;
}

static inline unsigned get_transbase1(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c2,c0,1" : "=r" (val));
      return val;
}

static inline unsigned get_dac(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c3,c0,0" : "=r" (val));
      return val;
}

static inline unsigned get_prrr(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c10,c2,0" : "=r" (val));
      return val;
}

static inline unsigned get_nmrr(void)
{
      unsigned int val;
      asm("mrc p15,0,%0,c10,c2,1" : "=r" (val));
      return val;
}

static void apanic_get_mmu_info(struct panic_info *info)
{
      int  bufsize = MMU_SCRIPT_BUF_SIZE, n = 0,total=0;
      char *s;
      unsigned int mmu_transbase0,mmu_transbase1;
      unsigned int mmu_dac,mmu_control;
      unsigned int mmu_prrr,mmu_nmrr;

      mmu_control =  get_ctrl();
      mmu_transbase0 = get_transbase0();
      mmu_dac = get_dac();
      mmu_transbase1 = get_transbase1();
      mmu_prrr = get_prrr();
      mmu_nmrr = get_nmrr();

      s =(char *)info->mmu_cmm_script;
      WRITE_LOG("PER.S C15:0x1 %%LONG 0x%X\n",mmu_control);
      WRITE_LOG("PER.S C15:0x2 %%LONG 0x%X\n",mmu_transbase0);
      WRITE_LOG("PER.S C15:0x3 %%LONG 0x%X\n",mmu_dac);
      WRITE_LOG("PER.S C15:0x102 %%LONG 0x%X\n",mmu_transbase1);
      WRITE_LOG("PER.S C15:0x2A %%LONG 0x%X\n",mmu_prrr);
      WRITE_LOG("PER.S C15:0x12A %%LONG 0x%X\n",mmu_nmrr);
      WRITE_LOG("MMU.SCAN\n");
      WRITE_LOG("MMU.ON\n");
      WRITE_LOG("\n\n\n"); /* 32bit boundary */
      info->mmu_cmm_size = total;
}
/*
 * Writes the contents of the console to the specified offset in flash.
 * Returns number of bytes written
 */
static int apanic_write_console_log(unsigned int off)
{
      struct apanic_config *ctx = &driver_context;
      int saved_oip;
      int idx = 0;
      int rc;
      unsigned int last_chunk = 0;
      unsigned char *cur_bounce;

      cur_bounce = (unsigned char *)((unsigned int)ctx->bounce + off);

      while (!last_chunk) {
            saved_oip = oops_in_progress;
            oops_in_progress = 1;
            rc = log_buf_copy(cur_bounce+idx, idx, ctx->writesize);
            if (rc < 0)
                  break;

            if (rc != ctx->writesize)
                  last_chunk = rc;

            oops_in_progress = saved_oip;
            if (rc <= 0)
                  break;

            if (rc != ctx->writesize)
                  memset(cur_bounce + idx + rc, 0, ctx->writesize - rc);

            if (!last_chunk)
                  idx += rc;
            else
                  idx += last_chunk;

      }
      return idx;
}

static int apanic_write_logcat_log(unsigned int off)
{
      struct apanic_config *ctx = &driver_context;
      int saved_oip;
      int idx = 0;
      int rc ;
      unsigned int last_chunk = 0;
      unsigned char *cur_bounce;

      cur_bounce = (unsigned char *)((unsigned int)ctx->bounce + off);

      while (!last_chunk) {
            saved_oip = oops_in_progress;
            oops_in_progress = 1;
            rc = logcat_buf_copy(cur_bounce+idx, idx, ctx->writesize);
            if (rc < 0)
                  break;

            if (rc != ctx->writesize)
                  last_chunk = rc;

            oops_in_progress = saved_oip;
            if (rc <= 0)
                  break;

            if (rc != ctx->writesize)
                  memset(cur_bounce + idx + rc, 0, ctx->writesize - rc);

            if (!last_chunk)
                  idx += rc;
            else
                  idx += last_chunk;

      }
      return idx;
}


static int apanic_logging(struct notifier_block *this, unsigned long event,
                  void *ptr)
{
      struct apanic_config *ctx = &driver_context;
      struct panic_info *hdr = (struct panic_info *) ctx->bounce;
      int console_offset = 0;
      int console_len = 0;
      int threads_offset = 0;
      int threads_len = 0;
      int logcat_offset = 0;
      int logcat_len = 0;

      if(!ctx->initialized)
            return -1;

      if (in_panic)
            return NOTIFY_DONE;
      in_panic = 1;
     
#ifdef CONFIG_PREEMPT
      /* Ensure that cond_resched() won't try to preempt anybody */
      add_preempt_count(PREEMPT_ACTIVE);
#endif

      touch_softlockup_watchdog();
 
      /* this case is kernel panic message send to modem err */
      if(ptr){
            //smem_diag_set_message((char *)ptr);
            printcrash((const char *)ptr);
      }
      console_offset = 4096;

      /*
       * Write out the console
       */
      console_len = apanic_write_console_log(console_offset);
      if (console_len < 0) {
            printk(KERN_EMERG "Error writing console to panic log! (%d)\n",
                   console_len);
            console_len = 0;
      }

      /*
       * Write out all threads
       */
      threads_offset = ALIGN(console_offset + console_len,ctx->writesize);
      if (!threads_offset)
            threads_offset = ctx->writesize;
#ifdef CONFIG_THREAD_STATE_INFO
/*
      logging time issue!!
      do use T32 simulator for thread info about it's schedule.
*/
      ram_console_enable_console(0);

      log_buf_clear();
      show_state_filter(0);

      threads_len = apanic_write_console_log(threads_offset);
      if (threads_len < 0) {
            printk(KERN_EMERG "Error writing threads to panic log! (%d)\n",
                   threads_len);
            threads_len = 0;
      }
#endif
      logcat_offset = ALIGN(threads_offset + threads_len,ctx->writesize);
      if(!logcat_offset)
            logcat_offset = ctx->writesize;

      /*
            TO - DO 
            1.log_main
            2.log_event
            3.log_radio
            4.log_system
      */

       logcat_set_log(4); /* logcat log system */

      logcat_len = apanic_write_logcat_log(logcat_offset);
      if (logcat_len < 0) {
            printk(KERN_EMERG "Error writing logcat to panic log! (%d)\n",
                   logcat_len);
            logcat_len = 0;
      }

      /*
       * Finally write the panic header
       */
      memset(ctx->bounce, 0, PAGE_SIZE);
      hdr->magic = PANIC_MAGIC;
      hdr->version = PHDR_VERSION;
      tmr_GetDate( tmr_GetLocalTime(), &hdr->date );
      printk("===time is %4d %02d %02d %02d %02d %02d ===\n", hdr->date.year, hdr->date.month, hdr->date.day,
                                hdr->date.hour, hdr->date.minute, hdr->date.second);
      
      apanic_get_mmu_info(hdr);
      //apanic_get_regs_info(hdr);

      hdr->console_offset    = console_offset;
      hdr->console_length  = console_len;
      
      hdr->threads_offset = threads_offset;
      hdr->threads_length = threads_len;
      
      hdr->logcat_offset   = logcat_offset;
      hdr->logcat_length  = logcat_len;

      hdr->total_size = logcat_offset + logcat_len;
      
      memset(hdr->stack.kernel_stack, 0x0, LOG_SIZE);
      memcpy(hdr->stack.kernel_stack, stack_dump, LOG_SIZE);      
      hdr->stack.stack_dump_size = logindex;
      
      printk(KERN_EMERG "pantech_apanic: Panic dump sucessfully written to smem\n");

      flush_cache_all();

#ifdef CONFIG_PREEMPT
      sub_preempt_count(PREEMPT_ACTIVE);
#endif
      in_panic = 0;
      return NOTIFY_DONE;
}

#if defined(CONFIG_PANTECH_APANIC_RESET)
static int apanic_reset(struct notifier_block *this, unsigned long event,void *ptr)
{
      printk(KERN_EMERG "pantech_apanic: reset modem & system download \n");

      smsm_reset_modem(SMSM_SYSTEM_DOWNLOAD);
}

static struct notifier_block next_blk = {
      .notifier_call = apanic_reset,
};
#endif

static struct notifier_block panic_blk = {
      .notifier_call    = apanic_logging,
#if defined(CONFIG_PANTECH_APANIC_RESET)
      .next = &next_blk,
#endif
};

int __init apanic_pantech_init(void)
{
      unsigned size = MAX_CRASH_BUF_SIZE;
      unsigned char *crash_buf;

      /*
            using smem_get_entry function if already memory have allocated. 
            if not AMSS err_init function must be called alloc function for ID.
      */
       memset(&driver_context,0,sizeof(struct apanic_config));

      crash_buf = (unsigned char *)smem_get_entry(SMEM_ID_VENDOR2, &size);
      if(!crash_buf){
            printk(KERN_ERR "pantech_apanic: no available crash buffer , initial failed\n");
            return 0;
      } else {
            atomic_notifier_chain_register(&panic_notifier_list, &panic_blk);
            driver_context.buf_size = size;
            driver_context.writesize = 4096;
            driver_context.bounce    = (void *)crash_buf;
            driver_context.initialized = 1;
      }
      printk(KERN_INFO "Android kernel / Modem panic handler initialized \n");
      return 0;
}

module_init(apanic_pantech_init);

MODULE_AUTHOR("JungSik Jeong <chero@pantech.com>");
MODULE_DESCRIPTION("PANTECH Error Crash misc driver");
MODULE_LICENSE("GPL");
