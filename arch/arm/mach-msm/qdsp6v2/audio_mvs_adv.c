/* Copyright (c) 2010, Code Aurora Forum. All rights reserved.
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
#include <linux/msm_audio_mvs.h>
#include <linux/fs.h>

#include "amr_transcode.h"
#include "audio_mvs_adv.h"
#include "mvs_amr.h"
#include "mvs_pcm.h"
/*---------------------- FEATURE DEFINE ------------------------*/
/*---------------------- FEATURE DEFINE ------------------------*/
//#define SUPPORT_VARIABLE_RATE
/*---------------------- VAUE DEFINE ------------------------*/
/* Length of the DSP frame info header added to the voc packet. */
#define DSP_FRAME_HDR_LEN 1
/*---------------------- STATIC VARIABLE DEFINE ------------------------*/
static mvs_adv_buf_info_t bf;
static long elapsed_time;
#ifdef VOLUMEKEY_NOISE_FIX
static char amr_null_packet[MAX_AMR_PKT_SIZE]={0x54, 0xf5, 0x1f, 0x96, 0x66,
0x79, 0xe1, 0xe0, 0x1, 0xe7, 0x8a, 0xf0, 0x0, 0x0, 0x0,
0xc0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0,
0x0, 0x0, 0x0, 0x0, 0x0, 0x0};

static char noise_critical_section=0;
void enter_noise_critical_section(char OnOff)
{
    if(!OnOff)    noise_critical_section=0;
    else    noise_critical_section=1;
}
#endif
/*---------------------- DEBUG or TEST function's here  ------------------------*/
static uint32_t current_amr_rate_type;
#ifdef SUPPORT_VARIABLE_RATE
extern uint32_t voice_get_config_vocoder_rate();
extern void voice_set_config_vocoder_rate(uint32_t rate);
#endif

/*---------------------- FUNCTION DEFINE ------------------------*/
void mvs_adv_buf_init(void)
{
    memset(&bf.rx.buf[0], 0, MAX_MVS_PKT_NUM*MAX_PCM_PKT_SIZE);
    memset(&bf.tx.buf[0], 0, MAX_MVS_PKT_NUM*MAX_PCM_PKT_SIZE);
    memset(&bf.rx.buf_sz[0], 0, MAX_MVS_PKT_NUM*sizeof(int));
    memset(&bf.tx.buf_sz[0], 0, MAX_MVS_PKT_NUM*sizeof(int));    
    
    bf.rx.read_pos=bf.rx.write_pos=0;
    bf.tx.read_pos=bf.tx.write_pos=0;
    bf.rx.state=bf.tx.state=MVS_CLOSE;
    bf.rx.fill_buf_status=bf.tx.fill_buf_status=0;

    elapsed_time=0;
    pr_debug("%s: reset size=%d\n", __func__, sizeof(mvs_adv_buf_info_t));

    current_amr_rate_type=MVS_AMR_MODE_1220;

#ifdef VOLUMEKEY_NOISE_FIX
    enter_noise_critical_section(1);
#endif
}
void mvs_adv_set_state(mvs_adv_state state)
{
    bf.rx.state=bf.tx.state=state;
}
mvs_adv_state mvs_adv_get_state(void)
{
    return bf.rx.state;
}

long audio_mvs_get_elapsed_time(void)
{
    //pr_info("%s: elapsed_time:%d\n", __func__, elapsed_time);
    return elapsed_time;
}
static void audio_mvs_write_sync_cb(int time)
{
    elapsed_time+=time;
}
static int audio_mvs_get_tx_available_size(void)
{
    int avail_size;
    if(bf.tx.read_pos>=bf.tx.write_pos)  avail_size=bf.tx.read_pos-bf.tx.write_pos;
    else    avail_size=bf.tx.read_pos+MAX_MVS_PKT_NUM-bf.tx.write_pos;

    return avail_size;
}
static int audio_mvs_get_rx_available_size(void)
{
    int avail_size;
    if(bf.rx.read_pos<=bf.rx.write_pos)  avail_size=bf.rx.write_pos-bf.rx.read_pos;
    else    avail_size=bf.rx.write_pos+MAX_MVS_PKT_NUM-bf.rx.read_pos;

    return avail_size;
}
int audio_mvs_pcm_read_func(unsigned char * buf)
{
    int size;
    if(audio_mvs_get_tx_available_size()==0)  {
        size = 0;
    }
    else    {
        if(bf.tx.buf_sz[bf.tx.write_pos]>MAX_PCM_PKT_SIZE)  {// SangwonLee 110427 PROTECT
            size = 0;
            pr_err("%s: wrong size(%d)\n", __func__, bf.tx.buf_sz[bf.rx.write_pos]);
        }
        else    {
            size = bf.tx.buf_sz[bf.tx.write_pos];
                memcpy(buf, &bf.tx.buf[bf.tx.write_pos][0], bf.tx.buf_sz[bf.tx.write_pos]);
            
            if(bf.tx.write_pos<MAX_MVS_PKT_NUM-1)  bf.tx.write_pos++;
            else bf.tx.write_pos=0;
        }
    }
    return size;
}

void audio_mvs_pcm_write_func(char * buf, int size)
{
    audio_mvs_write_sync_cb(10);

    if(size>MAX_PCM_PKT_SIZE)  {// SangwonLee 110427 PROTECT
        pr_err("%s: wrong size(%d)\n", __func__, size);
    }
    else    {
    memset(&bf.rx.buf[bf.rx.write_pos][0], 0, size);
    bf.rx.buf_sz[bf.rx.write_pos]=size;
    memcpy(&bf.rx.buf[bf.rx.write_pos][0], buf, size);

    if(bf.rx.write_pos<MAX_MVS_PKT_NUM-1)  bf.rx.write_pos++;
    else bf.rx.write_pos=0;

    mvs_pcm_packet_is_ready(audio_mvs_get_tx_available_size());
    //pr_info("%s: size=%d, write_seq=%d\n", __func__, size, write_seq);   
    }
}
int audio_mvs_amr_read_func(char * buf)
{
    int size;
    if(audio_mvs_get_tx_available_size()==0)  {
        size = 0;
    }
    else    {
        if(bf.tx.buf_sz[bf.tx.write_pos]>MAX_PCM_PKT_SIZE)  {// SangwonLee 110427 PROTECT
            size = 0;
            pr_err("%s: wrong size(%d)\n", __func__, bf.tx.buf_sz[bf.tx.write_pos]);
        }
        else    {
        size = bf.tx.buf_sz[bf.tx.write_pos];
        memcpy(buf, &bf.tx.buf[bf.tx.write_pos][0], bf.tx.buf_sz[bf.tx.write_pos]);
        if(bf.tx.write_pos<MAX_MVS_PKT_NUM-1)  bf.tx.write_pos++;
        else bf.tx.write_pos=0;
        }
    }
    return size;
}
void audio_mvs_amr_write_func(char * buf, int size)
{
    audio_mvs_write_sync_cb(20);

    if(size>MAX_AMR_PKT_SIZE)  {// SangwonLee 110427 PROTECT
        pr_err("%s: wrong size(%d)\n", __func__, size);
    }
    else    {
    memset(&bf.rx.buf[bf.rx.write_pos][0], 0, MAX_PCM_PKT_SIZE/*MAX_AMR_PKT_SIZE*/);
    memcpy(&bf.rx.buf[bf.rx.write_pos][0], buf, size);
    bf.rx.buf_sz[bf.rx.write_pos]=size;
    //pr_info("%s: bf.rx.write_pos=%d\n", __func__, bf.rx.write_pos);
    if(bf.rx.write_pos<MAX_MVS_PKT_NUM-1)  bf.rx.write_pos++;
    else bf.rx.write_pos=0;
    }
}

void audio_adv_mvs_process_pcm_uplink(uint8_t *voc_pkt,
				     uint32_t pkt_len,
				     void *private_data)
{
        char seq;
        for(seq=0;seq<2;seq++)
        {
            memcpy(&bf.tx.buf[bf.tx.read_pos][0], voc_pkt+seq*MAX_PCM_PKT_SIZE, MAX_PCM_PKT_SIZE);
            bf.tx.buf_sz[bf.tx.read_pos]=MAX_PCM_PKT_SIZE;
            if(bf.tx.read_pos<MAX_MVS_PKT_NUM-1)  bf.tx.read_pos++;
            else bf.tx.read_pos=0;
        }
        //mvs_pcm_packet_is_ready(audio_mvs_get_tx_available_size());
}

void audio_adv_mvs_process_pcm_downlink(uint8_t *voc_pkt,
				     uint32_t *pkt_len,
				     void *private_data)
{
        unsigned char dl_buf[VOC_PCM_PACKET_SIZE];
        char seq;

        for(seq=0;seq<2;seq++)
        {

            if(audio_mvs_get_rx_available_size()==0 || (!bf.rx.fill_buf_status && MAX_MVS_START_PKT_NUM))  {
                //pr_info("%s: Silence (available num=%d, fill_buf_status=%d)\n", __func__, audio_mvs_get_rx_available_size(), bf.rx.fill_buf_status);   
                memset(dl_buf, 0, VOC_PCM_PACKET_SIZE);
                if(audio_mvs_get_rx_available_size()>=MAX_MVS_START_PKT_NUM)    bf.rx.fill_buf_status=1;
                else    bf.rx.fill_buf_status=0;
                break;
            }
            else    {
                if(bf.rx.buf_sz[bf.rx.read_pos]!=MAX_PCM_PKT_SIZE)  {
                    pr_err("%s: wrong size(%d)\n", __func__, bf.rx.buf_sz[bf.rx.read_pos]);
                }
                else    {
                    memcpy(&dl_buf[seq*MAX_PCM_PKT_SIZE],
                    	       &bf.rx.buf[bf.rx.read_pos][0],
                    	       bf.rx.buf_sz[bf.rx.read_pos]);
                    if(bf.rx.read_pos<MAX_MVS_PKT_NUM-1)  bf.rx.read_pos++;
                    else bf.rx.read_pos=0;
                }
            }
        }
        *pkt_len = VOC_PCM_PACKET_SIZE;
        memcpy(voc_pkt, dl_buf, VOC_PCM_PACKET_SIZE);
}

void audio_adv_mvs_process_amr_uplink(uint8_t *voc_pkt,
				     uint32_t pkt_len,
				     void *private_data)
{
       int result, trans_size;
       unsigned char if2_data[MAX_IF2_PACKET_SIZE];
       unsigned char if1_data[MAX_IF1_PACKET_SIZE];

        voc_pkt++;
        //pr_info("%s: bf.tx.read_pos=%d, buf size=%d\n", __func__, bf.tx.read_pos, bf.tx.buf_sz[bf.tx.read_pos]);       

        memset(&bf.tx.buf[bf.tx.read_pos][0], 0, MAX_PCM_PKT_SIZE);
        memset(if1_data, 0, MAX_IF1_PACKET_SIZE);
        memset(if2_data, 0, MAX_IF2_PACKET_SIZE);

        #ifdef SUPPORT_VARIABLE_RATE
        if1_data[0] = current_amr_rate_type<<3 | 4/*GOOD FRAME*/;
        #else
        if1_data[0] = MVS_AMR_MODE_1220<<3 | 4/*GOOD FRAME*/;
        #endif
        if(pkt_len > MAX_IF1_PACKET_SIZE)    {// SangwonLee 110427 PROTECT
            pr_err("%s: wrong size(%d)\n", __func__, pkt_len);
        }
        memcpy(&if1_data[1], voc_pkt, MAX_IF1_PACKET_SIZE - DSP_FRAME_HDR_LEN);
        trans_size =0;
        result = qvp_if1_to_if2(if1_data, pkt_len, if2_data,MAX_IF2_PACKET_SIZE, (uint16*)&trans_size, NULL);

        bf.tx.buf_sz[bf.tx.read_pos] = trans_size;
        if(trans_size > MAX_IF2_PACKET_SIZE)    {// SangwonLee 110427 PROTECT
            pr_err("%s: wrong size(%d)\n", __func__, pkt_len);
        }
        else    {
        memcpy(&bf.tx.buf[bf.tx.read_pos][0], if2_data, bf.tx.buf_sz[bf.tx.read_pos]);

        if(bf.tx.read_pos<MAX_MVS_PKT_NUM-1)  bf.tx.read_pos++;
        else bf.tx.read_pos=0;
        mvs_amr_packet_is_ready(audio_mvs_get_tx_available_size());
        }
}

static uint32_t audio_adv_mvs_get_rate(uint32_t frametype)
{
	uint32_t cvs_rate;

       cvs_rate = frametype&0x7;
	return cvs_rate;
}

void audio_adv_mvs_process_amr_downlink(uint8_t *voc_pkt,
				     uint32_t *pkt_len,
				     void *private_data)
{
        int result, trans_size;
        unsigned char if1_data[MAX_IF1_PACKET_SIZE];
        #ifdef SUPPORT_VARIABLE_RATE
        uint32_t rate_type;
        #endif

        if(audio_mvs_get_rx_available_size()==0 || (!bf.rx.fill_buf_status && MAX_MVS_START_PKT_NUM))  {
            //pr_info("%s: Silence (available num=%d, fill_buf_status=%d)\n", __func__, audio_mvs_get_rx_available_size(), bf.rx.fill_buf_status);   
            #ifdef VOLUMEKEY_NOISE_FIX
            if(noise_critical_section)  {
                *voc_pkt = ((MVS_AMR_SPEECH_GOOD & 0x0F) << 4) |(current_amr_rate_type & 0x0F);
                voc_pkt++;
                memcpy(voc_pkt, amr_null_packet, MAX_AMR_PKT_SIZE);     // SangwonLee 110330
                *pkt_len = MAX_AMR_PKT_SIZE + DSP_FRAME_HDR_LEN; // SangwonLee 110330
            }
            else    {
            #endif
            *voc_pkt = ((MVS_AMR_NO_DATA & 0x0F) << 4) |(current_amr_rate_type & 0x0F);
            voc_pkt++;
            *pkt_len = DSP_FRAME_HDR_LEN;//MAX_AMR_PKT_SIZE + DSP_FRAME_HDR_LEN; // SangwonLee 110330
            #ifdef VOLUMEKEY_NOISE_FIX
            }
            #endif
            if(audio_mvs_get_rx_available_size()>=MAX_MVS_START_PKT_NUM)    bf.rx.fill_buf_status=1;
            else    bf.rx.fill_buf_status=0;
        }
        else    {
            trans_size=0;
            memset(if1_data, 0, MAX_IF1_PACKET_SIZE);
            current_amr_rate_type = audio_adv_mvs_get_rate(bf.rx.buf[bf.rx.read_pos][0]);
            #ifdef SUPPORT_VARIABLE_RATE
            rate_type = voice_get_config_vocoder_rate();
            if(current_amr_rate_type!=rate_type)  {
                voice_set_config_vocoder_rate(current_amr_rate_type);
                pr_info("%s: AMR rate is changed from %d to %d\n", __func__, rate_type, current_amr_rate_type);   
            }
            #endif

            if(bf.rx.buf_sz[bf.rx.read_pos] > MAX_IF2_PACKET_SIZE)    {// SangwonLee 110427 PROTECT
                pr_err("%s: wrong size(%d)\n", __func__, bf.rx.buf_sz[bf.rx.read_pos]);
            }
            else bf.rx.buf_sz[bf.rx.read_pos] = MAX_IF2_PACKET_SIZE;
            
            result = qvp_if2_to_if1(&bf.rx.buf[bf.rx.read_pos][0], bf.rx.buf_sz[bf.rx.read_pos], if1_data,MAX_IF1_PACKET_SIZE, (uint16*)&trans_size, NULL);
            //pr_info("%s: bf.rx.read_pos:%d\n", __func__, bf.rx.read_pos);   

            if(trans_size>MAX_IF1_PACKET_SIZE)    {// SangwonLee 110427 PROTECT
                pr_err("%s: wrong size(%d)\n", __func__, trans_size);
            }
            else    {
                *voc_pkt = ((MVS_AMR_SPEECH_GOOD & 0x0F) << 4) |(current_amr_rate_type & 0x0F);
                voc_pkt++;

                memcpy(voc_pkt, &if1_data[1], trans_size-DSP_FRAME_HDR_LEN);
                *pkt_len = trans_size;

                if(bf.rx.read_pos<MAX_MVS_PKT_NUM-1)  bf.rx.read_pos++;
                else bf.rx.read_pos=0;
            }
        }
}

