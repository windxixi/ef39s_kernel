/* Copyright (c) 2011, PANTECH. All rights reserved.  */

#include "s5k4ecgx.h"

static const struct s5k4ecgx_i2c_reg_conf s5k4ecgx_init_parm[] =
{
//==================================================================================
// 00.History
//==================================================================================
//2010 : EVT1.1
//20110429 : LSI CSE Standard
//==================================================================================
//==================================================================================
// 01.Start Setting
//==================================================================================
{0xFCFC,0xD000, WORD_LEN, 0},	
{0x0010,0x0001, WORD_LEN, 0},	//S/W Reset
{0x1030,0x0000, WORD_LEN, 0},	//contint_host_int
{0x0014,0x0001, WORD_LEN, 0},	//sw_load_complete - Release CORE (Arm) from reset state

{0xFFFF,0x000A, WORD_LEN, 10}, //p10

//==================================================================================                
//02.ETC Setting                                                                                    
//==================================================================================                
                                                                                            
{0x0028,0xD000, WORD_LEN, 0},	//Driving Current                                                                   
{0x002A,0x1082, WORD_LEN, 0},                                                                                           
{0x0f12,0x0000, WORD_LEN, 0}, 	//cregs_d0_d4_cd10 //D4[9:8], D3[7:6], D2[5:4], D1[3:2], D0[1:0]                    
{0x002A,0x1084, WORD_LEN, 0},                                                                                           
{0x0f12,0x0000, WORD_LEN, 0}, 	//cregs_d5_d9_cd10 //D9[9:8], D8[7:6], D7[5:4], D6[3:2], D5[1:0]                    
{0x002A,0x1088, WORD_LEN, 0},                                                                                           
{0x0f12,0x0000, WORD_LEN, 0},	//cregs_clks_output_cd10 //SDA[11:10], SCL[9:8], PCLK[7:6], VSYNC[3:2], HSYNC[1:0]  
                                                                                     
{0x0028,0x7000, WORD_LEN, 0},	                                                                                    
{0x002a,0x01a2, WORD_LEN, 0},	//ESD Check                                                                         
{0x0f12,0xaaaa, WORD_LEN, 0},	                                                                                    
                                                                                            

//==================================================================================
// 03.Analog Setting & ASP Control
//==================================================================================

//This register is for FACTORY ONLY. 
//If you change it without prior notification
//YOU are RESPONSIBLE for the FAILURE that will happen in the future

{0x0028,0xD000, WORD_LEN, 0},
{0x002A,0x007A, WORD_LEN, 0},
{0x0f12,0x0000, WORD_LEN, 0},	
{0x002A,0xE406, WORD_LEN, 0},	//[7]f_ladlc_en [6:5]f max [4]fadlc_en [3:2]L max [1]ladlc_en [0]adlc_ch_sel
{0x0F12,0x0092, WORD_LEN, 0},	
{0x002A,0xE410, WORD_LEN, 0},	
{0x0F12,0x3804, WORD_LEN, 0},	//[15:8]fadlc_filter_co_b, [7:0]fadlc_filter_co_a	
{0x002A,0xE41A, WORD_LEN, 0},	
{0x0F12,0x0010, WORD_LEN, 0},	 	
{0x002A,0xE420, WORD_LEN, 0},	
{0x0F12,0x0003, WORD_LEN, 0},	//adlc_fadlc_filter_refresh 	
{0x0F12,0x0060, WORD_LEN, 0},	//adlc_filter_level_diff_threshold
{0x002A,0xE42E, WORD_LEN, 0},	
{0x0F12,0x0004, WORD_LEN, 0},	//dithered l-ADLC(4bit)
{0x002A,0xF400, WORD_LEN, 0},	
{0x0F12,0x5A3C, WORD_LEN, 0},	//[15:8]stx_width, [7:0]dstx_width 
{0x0F12,0x0023, WORD_LEN, 0},	//[14]binning_test [13]gain_mode [11:12]row_id [10]cfpn_test [9]pd_pix [8]teg_en, [7]adc_res, [6]smp_en, [5]ldb_en, [4]ld_en, [3]clp_en [2]srx_en, [1]dshut_en, [0]dcds_en 
{0x0F12,0x8080, WORD_LEN, 0},	//CDS option 
{0x0F12,0x03AF, WORD_LEN, 0},	//[11:6]rst_mx, [5:0]sig_mx
{0x0F12,0x000A, WORD_LEN, 0},	//Avg mode
{0x0F12,0xAA54, WORD_LEN, 0},	//x1~x1.49:No MS, x1.5~x3.99:MS2, x4~x16:MS4
{0x0F12,0x0040, WORD_LEN, 0},	//RMP option [6]1: RES gain
{0x0F12,0x464E, WORD_LEN, 0},	//[14]msoff_en, [13:8]off_rst, [7:0]adc_sat
{0x0F12,0x0240, WORD_LEN, 0},	//bist_sig_width_e
{0x0F12,0x0240, WORD_LEN, 0},	//bist_sig_width_o
{0x0F12,0x0040, WORD_LEN, 0},	//[9]dbs_bist_en, [8:0]bist_rst_width
{0x0F12,0x1000, WORD_LEN, 0},	//[15]aac_en, [14]GCLK_DIV2_EN, [13:10]dl_cont [9:8]dbs_mode, [7:0]dbs_option
{0x0F12,0x55FF, WORD_LEN, 0},	//bias [15:12]pix, [11:8]pix_bst [7:4]comp2, [3:0]comp1  	
{0x0F12,0xD000, WORD_LEN, 0},	//[15:8]clp_lvl, [7:0]ref_option, [5]pix_bst_en
{0x0F12,0x0010, WORD_LEN, 0},	//[7:0]monit
{0x0F12,0x0202, WORD_LEN, 0},	//[15:8]dbr_tune_tgsl, [7:0]dbr_tune_pix
{0x0F12,0x0401, WORD_LEN, 0},	//[15:8]dbr_tune_ntg, [7:0]dbr_tune_rg                                                                       
{0x0F12,0x0022, WORD_LEN, 0},	//[15:8]reg_option, [7:4]rosc_tune_ncp, [3:0]rosc_tune_cp                                                    
{0x0F12,0x0088, WORD_LEN, 0},	//PD [8]inrush_ctrl, [7]fblv, [6]reg_ntg, [5]reg_tgsl, [4]reg_rg, [3]reg_pix, [2]ncp_rosc, [1]cp_rosc, [0]cp 
{0x0F12,0x009F, WORD_LEN, 0},	//[9]capa_ctrl_en, [8:7]fb_lv, [6:5]dbr_clk_sel, [4:0]cp_capa                                                
{0x0F12,0x0000, WORD_LEN, 0},	//[15:0]blst_en_cintr                                                                                        
{0x0F12,0x1800, WORD_LEN, 0},	//[11]blst_en, [10]rfpn_test, [9]sl_off, [8]tx_off, [7:0]rdv_option                                          
{0x0F12,0x0088, WORD_LEN, 0},	//[15:0]pmg_reg_tune                                                                                         
{0x0F12,0x0000, WORD_LEN, 0},	//[15:1]analog_dummy, [0]pd_reg_test 
{0x0F12,0x2428, WORD_LEN, 0},	//[13:11]srx_gap1, [10:8]srx_gap0, [7:0]stx_gap
{0x0F12,0x0000, WORD_LEN, 0},	//[0]atx_option       
{0x0F12,0x03EE, WORD_LEN, 0},	//aig_avg_half
{0x0F12,0x0000, WORD_LEN, 0},	//[0]hvs_test_reg     
{0x0F12,0x0000, WORD_LEN, 0},	//[0]dbus_bist_auto   
{0x0F12,0x0000, WORD_LEN, 0},	//[7:0]dbr_option	    
{0x002A,0xF552, WORD_LEN, 0},	
{0x0F12,0x0708, WORD_LEN, 0},	//[7:0]lat_st, [15:8]lat_width  
{0x0F12,0x080C, WORD_LEN, 0},	//[7:0]hold_st, [15:8]hold_width

//For subsampling Size	

{0x0028,0x7000, WORD_LEN, 0},	
{0x002A,0x18BC, WORD_LEN, 0},	
{0x0F12,0x0004, WORD_LEN, 0},	
{0x0F12,0x05B6, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	
{0x0F12,0x05BA, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0007, WORD_LEN, 0},	
{0x0F12,0x05BA, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x01F4, WORD_LEN, 0},	
{0x0F12,0x024E, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x01F4, WORD_LEN, 0},	
{0x0F12,0x05B6, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x01F4, WORD_LEN, 0},	
{0x0F12,0x05BA, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x01F4, WORD_LEN, 0},	
{0x0F12,0x024F, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0075, WORD_LEN, 0},	
{0x0F12,0x00CF, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0075, WORD_LEN, 0},	
{0x0F12,0x00D6, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0004, WORD_LEN, 0},	
{0x0F12,0x01F4, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x00F0, WORD_LEN, 0},	
{0x0F12,0x01F4, WORD_LEN, 0},	
{0x0F12,0x029E, WORD_LEN, 0},	
{0x0F12,0x05B2, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x01F8, WORD_LEN, 0},	
{0x0F12,0x0228, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0208, WORD_LEN, 0},	
{0x0F12,0x0238, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0218, WORD_LEN, 0},	
{0x0F12,0x0238, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	
{0x0F12,0x0009, WORD_LEN, 0},	
{0x0F12,0x00DE, WORD_LEN, 0},	
{0x0F12,0x05C0, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x00DF, WORD_LEN, 0},	
{0x0F12,0x00E4, WORD_LEN, 0},	
{0x0F12,0x01F8, WORD_LEN, 0},	
{0x0F12,0x01FD, WORD_LEN, 0},	
{0x0F12,0x05B6, WORD_LEN, 0},	
{0x0F12,0x05BB, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x01F8, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0077, WORD_LEN, 0},	
{0x0F12,0x007E, WORD_LEN, 0},	
{0x0F12,0x024F, WORD_LEN, 0},	
{0x0F12,0x025E, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	

// For Capture 	

{0x0F12,0x0004, WORD_LEN, 0},	
{0x0F12,0x09D1, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	
{0x0F12,0x09D5, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0008, WORD_LEN, 0},	
{0x0F12,0x09D5, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x02AA, WORD_LEN, 0},	
{0x0F12,0x0326, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x02AA, WORD_LEN, 0},	
{0x0F12,0x09D1, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x02AA, WORD_LEN, 0},	
{0x0F12,0x09D5, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x02AA, WORD_LEN, 0},	
{0x0F12,0x0327, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0008, WORD_LEN, 0},	
{0x0F12,0x0084, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0008, WORD_LEN, 0},	
{0x0F12,0x008D, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0008, WORD_LEN, 0},	
{0x0F12,0x02AA, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x00AA, WORD_LEN, 0},	
{0x0F12,0x02AA, WORD_LEN, 0},	
{0x0F12,0x03AD, WORD_LEN, 0},	
{0x0F12,0x09CD, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x02AE, WORD_LEN, 0},	
{0x0F12,0x02DE, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x02BE, WORD_LEN, 0},	
{0x0F12,0x02EE, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x02CE, WORD_LEN, 0},	
{0x0F12,0x02EE, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	
{0x0F12,0x0009, WORD_LEN, 0},	
{0x0F12,0x0095, WORD_LEN, 0},	
{0x0F12,0x09DB, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0096, WORD_LEN, 0},	
{0x0F12,0x009B, WORD_LEN, 0},	
{0x0F12,0x02AE, WORD_LEN, 0},	
{0x0F12,0x02B3, WORD_LEN, 0},	
{0x0F12,0x09D1, WORD_LEN, 0},	
{0x0F12,0x09D6, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x02AE, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0009, WORD_LEN, 0},	
{0x0F12,0x0010, WORD_LEN, 0},	
{0x0F12,0x0327, WORD_LEN, 0},	
{0x0F12,0x0336, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	

{0x002A,0x1AF8, WORD_LEN, 0},	
{0x0F12,0x5A3C, WORD_LEN, 0},	//senHal_TuneStr_AngTuneData1_2_D000F400 register at subsampling
{0x002A,0x1896, WORD_LEN, 0},	
{0x0F12,0x0002, WORD_LEN, 0},	//senHal_SamplingType	0002 03EE: PLA setting          
{0x0F12,0x0000, WORD_LEN, 0},	//senHal_SamplingMode 0 : 2 PLA / 1 : 4PLA              
{0x0F12,0x0003, WORD_LEN, 0},	//senHal_PLAOption	[0] VPLA enable  [1] HPLA enable
{0x002A,0x189E, WORD_LEN, 0},	
{0x0F12,0x0FB0, WORD_LEN, 0},	//senHal_ExpMinPixels
{0x002A,0x18AC, WORD_LEN, 0},	
{0x0F12,0x0060, WORD_LEN, 0},	//senHal_uAddColsBin  
{0x0F12,0x0060, WORD_LEN, 0},	//senHal_uAddColsNoBin
{0x0F12,0x05C0, WORD_LEN, 0},	//senHal_uMinColsBin  
{0x0F12,0x05C0, WORD_LEN, 0},	//senHal_uMinColsNoBin
{0x002A,0x1AEA, WORD_LEN, 0},	
{0x0F12,0x8080, WORD_LEN, 0},	//senHal_SubF404Tune 
{0x0F12,0x0080, WORD_LEN, 0},	//senHal_FullF404Tune
{0x002A,0x1AE0, WORD_LEN, 0},	                     
{0x0F12,0x0000, WORD_LEN, 0},	//senHal_bSenAAC     
{0x002A,0x1A72, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	//senHal_bSRX	SRX off
{0x002A,0x18A2, WORD_LEN, 0},	
{0x0F12,0x0004, WORD_LEN, 0},	//senHal_NExpLinesCheckFine extend Forbidden area line       
{0x002A,0x1A6A, WORD_LEN, 0},	                                                             
{0x0F12,0x009A, WORD_LEN, 0},	//senHal_usForbiddenRightOfs extend right Forbidden area line
{0x002A,0x385E, WORD_LEN, 0},	                                                             
{0x0F12,0x024C, WORD_LEN, 0},	//Mon_Sen_uExpPixelsOfs                                      
{0x002A,0x0EE6, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	//setot_bUseDigitalHbin
{0x002A,0x1B2A, WORD_LEN, 0},	
{0x0F12,0x0300, WORD_LEN, 0},	//70001B2A //senHal_TuneStr2_usAngTuneGainTh	
{0x0F12,0x00D6, WORD_LEN, 0},	//70001B2C //senHal_TuneStr2_AngTuneF4CA_0_ 
{0x0F12,0x008D, WORD_LEN, 0},	//70001B2E //senHal_TuneStr2_AngTuneF4CA_1_ 
{0x0F12,0x00CF, WORD_LEN, 0},	//70001B30 //senHal_TuneStr2_AngTuneF4C2_0_ 
{0x0F12,0x0084, WORD_LEN, 0},	//70001B32 //senHal_TuneStr2_AngTuneF4C2_1_ 

//==================================================================================
//04.Gas_Anti Shading_Otp
//==================================================================================

{0x002A,0x0722, WORD_LEN, 0},	
{0x0F12,0x0100, WORD_LEN, 0},	//skl_OTP_usWaitTime This register should be positioned in fornt of D0001000	
{0x002A,0x0726, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	//skl_bUseOTPfunc This is OTP on/off function	
{0x002A,0x08D6, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	//ash_bUseOTPData	
{0x002A,0x146E, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_otp_disable	
{0x002A,0x08DC, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	//ash_bUseGasAlphaOTP	

// Refer Mon_AWB_RotGain         	
{0x002A,0x08B4, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	//wbt_bUseOutdoorASH	
{0x002A,0x08BC, WORD_LEN, 0},	
{0x0F12,0x00C0, WORD_LEN, 0},	//TVAR_ash_AwbAshCord_0_ 2300K	
{0x0F12,0x00DF, WORD_LEN, 0},	//TVAR_ash_AwbAshCord_1_ 2750K	
{0x0F12,0x0100, WORD_LEN, 0},	//TVAR_ash_AwbAshCord_2_ 3300K	
{0x0F12,0x0125, WORD_LEN, 0},	//TVAR_ash_AwbAshCord_3_ 4150K	
{0x0F12,0x015F, WORD_LEN, 0},	//TVAR_ash_AwbAshCord_4_ 5250K	
{0x0F12,0x017C, WORD_LEN, 0},	//TVAR_ash_AwbAshCord_5_ 6400K	
{0x0F12,0x0194, WORD_LEN, 0},	//TVAR_ash_AwbAshCord_6_ 7500K	

{0x002A,0x08F6, WORD_LEN, 0},	
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_0__0_ R  // 2300K     	
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_0__1_ GR          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_0__2_ GB          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_0__3_ B    

{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_1__0_ R  // 2750K 
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_1__1_ GR          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_1__2_ GB  
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_1__3_ B   

{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_2__0_ R  // 3300K 
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_2__1_ GR          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_2__2_ GB          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_2__3_ B   

{0x0F12,0x4000, WORD_LEN, 0}, //3800   	//TVAR_ash_GASAlpha_3__0_ R  // 4150K 
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_3__1_ GR          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_3__2_ GB          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_3__3_ B    

{0x0F12,0x4000, WORD_LEN, 0}, //3B00 	//TVAR_ash_GASAlpha_4__0_ R  // 5250K 
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_4__1_ GR          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_4__2_ GB          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_4__3_ B      

{0x0F12,0x4300, WORD_LEN, 0}, 	//TVAR_ash_GASAlpha_5__0_ R  // 6400K 
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_5__1_ GR          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_5__2_ GB          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_5__3_ B        

{0x0F12,0x4300, WORD_LEN, 0},	//TVAR_ash_GASAlpha_6__0_ R  // 7500K 
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_6__1_ GR          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_6__2_ GB          
{0x0F12,0x4000, WORD_LEN, 0},	//TVAR_ash_GASAlpha_6__3_ B        

//Outdoor GAS Alpha   
{0x0F12,0x4500, WORD_LEN, 0},  	
{0x0F12,0x4000, WORD_LEN, 0},	
{0x0F12,0x4000, WORD_LEN, 0},	
{0x0F12,0x4000, WORD_LEN, 0},	

{0x002A,0x08F4, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	//ash_bUseGasAlpha

//=================================================================================
// 05.Trap and Patch
//=================================================================================
// Start of Patch data
{0x0028,0x7000, WORD_LEN, 0},                             
{0x002A,0x3AF8, WORD_LEN, 0},                             
{0x0F12,0xB570, WORD_LEN, 0},	// 70003AF8              
{0x0F12,0x4B39, WORD_LEN, 0},	// 70003AFA              
{0x0F12,0x4939, WORD_LEN, 0},	// 70003AFC              
{0x0F12,0x483A, WORD_LEN, 0},	// 70003AFE              
{0x0F12,0x2200, WORD_LEN, 0},	// 70003B00              
{0x0F12,0xC008, WORD_LEN, 0},	// 70003B02              
{0x0F12,0x6001, WORD_LEN, 0},	// 70003B04              
{0x0F12,0x4939, WORD_LEN, 0},	// 70003B06              
{0x0F12,0x4839, WORD_LEN, 0},	// 70003B08              
{0x0F12,0x2401, WORD_LEN, 0},	// 70003B0A              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003B0C              
{0x0F12,0xFBEC, WORD_LEN, 0},	// 70003B0E              
{0x0F12,0x4938, WORD_LEN, 0},	// 70003B10              
{0x0F12,0x4839, WORD_LEN, 0},	// 70003B12              
{0x0F12,0x2502, WORD_LEN, 0},	// 70003B14              
{0x0F12,0x0022, WORD_LEN, 0},	// 70003B16              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003B18              
{0x0F12,0xFBE6, WORD_LEN, 0},	// 70003B1A              
{0x0F12,0x4837, WORD_LEN, 0},	// 70003B1C              
{0x0F12,0x0261, WORD_LEN, 0},	// 70003B1E              
{0x0F12,0x8001, WORD_LEN, 0},	// 70003B20              
{0x0F12,0x2100, WORD_LEN, 0},	// 70003B22              
{0x0F12,0x8041, WORD_LEN, 0},	// 70003B24              
{0x0F12,0x4936, WORD_LEN, 0},	// 70003B26              
{0x0F12,0x4836, WORD_LEN, 0},	// 70003B28              
{0x0F12,0x6041, WORD_LEN, 0},	// 70003B2A              
{0x0F12,0x4936, WORD_LEN, 0},	// 70003B2C              
{0x0F12,0x4837, WORD_LEN, 0},	// 70003B2E              
{0x0F12,0x2403, WORD_LEN, 0},	// 70003B30              
{0x0F12,0x002A, WORD_LEN, 0},	// 70003B32              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003B34              
{0x0F12,0xFBD8, WORD_LEN, 0},	// 70003B36              
{0x0F12,0x4832, WORD_LEN, 0},	// 70003B38              
{0x0F12,0x4935, WORD_LEN, 0},	// 70003B3A              
{0x0F12,0x30C0, WORD_LEN, 0},	// 70003B3C              
{0x0F12,0x63C1, WORD_LEN, 0},	// 70003B3E              
{0x0F12,0x4930, WORD_LEN, 0},	// 70003B40              
{0x0F12,0x4834, WORD_LEN, 0},	// 70003B42              
{0x0F12,0x3980, WORD_LEN, 0},	// 70003B44              
{0x0F12,0x6408, WORD_LEN, 0},	// 70003B46              
{0x0F12,0x4833, WORD_LEN, 0},	// 70003B48              
{0x0F12,0x4934, WORD_LEN, 0},	// 70003B4A              
{0x0F12,0x6388, WORD_LEN, 0},	// 70003B4C              
{0x0F12,0x4934, WORD_LEN, 0},	// 70003B4E              
{0x0F12,0x4834, WORD_LEN, 0},	// 70003B50              
{0x0F12,0x0022, WORD_LEN, 0},	// 70003B52              
{0x0F12,0x2504, WORD_LEN, 0},	// 70003B54              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003B56              
{0x0F12,0xFBC7, WORD_LEN, 0},	// 70003B58              
{0x0F12,0x4933, WORD_LEN, 0},	// 70003B5A              
{0x0F12,0x4833, WORD_LEN, 0},	// 70003B5C              
{0x0F12,0x2405, WORD_LEN, 0},	// 70003B5E              
{0x0F12,0x002A, WORD_LEN, 0},	// 70003B60              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003B62              
{0x0F12,0xF881, WORD_LEN, 0},	// 70003B64              
{0x0F12,0x491F, WORD_LEN, 0},	// 70003B66              
{0x0F12,0x4830, WORD_LEN, 0},	// 70003B68              
{0x0F12,0x0022, WORD_LEN, 0},	// 70003B6A              
{0x0F12,0x2506, WORD_LEN, 0},	// 70003B6C              
{0x0F12,0x39B6, WORD_LEN, 0},	// 70003B6E              
{0x0F12,0x1D80, WORD_LEN, 0},	// 70003B70              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003B72              
{0x0F12,0xF879, WORD_LEN, 0},	// 70003B74              
{0x0F12,0x482D, WORD_LEN, 0},	// 70003B76              
{0x0F12,0x492D, WORD_LEN, 0},	// 70003B78              
{0x0F12,0x2407, WORD_LEN, 0},	// 70003B7A              
{0x0F12,0x002A, WORD_LEN, 0},	// 70003B7C              
{0x0F12,0x300C, WORD_LEN, 0},	// 70003B7E              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003B80              
{0x0F12,0xF872, WORD_LEN, 0},	// 70003B82              
{0x0F12,0x4829, WORD_LEN, 0},	// 70003B84              
{0x0F12,0x492B, WORD_LEN, 0},	// 70003B86              
{0x0F12,0x0022, WORD_LEN, 0},	// 70003B88              
{0x0F12,0x2508, WORD_LEN, 0},	// 70003B8A              
{0x0F12,0x3010, WORD_LEN, 0},	// 70003B8C              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003B8E              
{0x0F12,0xF86B, WORD_LEN, 0},	// 70003B90              
{0x0F12,0x4929, WORD_LEN, 0},	// 70003B92              
{0x0F12,0x4829, WORD_LEN, 0},	// 70003B94              
{0x0F12,0x2409, WORD_LEN, 0},	// 70003B96              
{0x0F12,0x002A, WORD_LEN, 0},	// 70003B98              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003B9A              
{0x0F12,0xFBA5, WORD_LEN, 0},	// 70003B9C              
{0x0F12,0x4928, WORD_LEN, 0},	// 70003B9E              
{0x0F12,0x4828, WORD_LEN, 0},	// 70003BA0              
{0x0F12,0x0022, WORD_LEN, 0},	// 70003BA2              
{0x0F12,0x250A, WORD_LEN, 0},	// 70003BA4              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003BA6              
{0x0F12,0xFB9F, WORD_LEN, 0},	// 70003BA8              
{0x0F12,0x4927, WORD_LEN, 0},	// 70003BAA              
{0x0F12,0x4827, WORD_LEN, 0},	// 70003BAC              
{0x0F12,0x240B, WORD_LEN, 0},	// 70003BAE              
{0x0F12,0x002A, WORD_LEN, 0},	// 70003BB0              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003BB2              
{0x0F12,0xFB99, WORD_LEN, 0},	// 70003BB4              
{0x0F12,0x4926, WORD_LEN, 0},	// 70003BB6              
{0x0F12,0x4826, WORD_LEN, 0},	// 70003BB8              
{0x0F12,0x0022, WORD_LEN, 0},	// 70003BBA              
{0x0F12,0x250C, WORD_LEN, 0},	// 70003BBC              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003BBE              
{0x0F12,0xFB93, WORD_LEN, 0},	// 70003BC0              
{0x0F12,0x4925, WORD_LEN, 0},	// 70003BC2              
{0x0F12,0x4825, WORD_LEN, 0},	// 70003BC4              
{0x0F12,0x240D, WORD_LEN, 0},	// 70003BC6              
{0x0F12,0x002A, WORD_LEN, 0},	// 70003BC8              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003BCA              
{0x0F12,0xFB8D, WORD_LEN, 0},	// 70003BCC              
{0x0F12,0x4924, WORD_LEN, 0},	// 70003BCE              
{0x0F12,0x4824, WORD_LEN, 0},	// 70003BD0              
{0x0F12,0x0022, WORD_LEN, 0},	// 70003BD2              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003BD4              
{0x0F12,0xFB88, WORD_LEN, 0},	// 70003BD6              
{0x0F12,0xBC70, WORD_LEN, 0},	// 70003BD8              
{0x0F12,0xBC08, WORD_LEN, 0},	// 70003BDA              
{0x0F12,0x4718, WORD_LEN, 0},	// 70003BDC              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003BDE              
{0x0F12,0x017B, WORD_LEN, 0},	// 70003BE0              
{0x0F12,0x4EC2, WORD_LEN, 0},	// 70003BE2              
{0x0F12,0x037F, WORD_LEN, 0},	// 70003BE4              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003BE6              
{0x0F12,0x1F90, WORD_LEN, 0},	// 70003BE8              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003BEA              
{0x0F12,0x3C81, WORD_LEN, 0},	// 70003BEC              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003BEE              
{0x0F12,0xE38B, WORD_LEN, 0},	// 70003BF0              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003BF2              
{0x0F12,0x3CB9, WORD_LEN, 0},	// 70003BF4              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003BF6              
{0x0F12,0xC3B1, WORD_LEN, 0},	// 70003BF8              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003BFA              
{0x0F12,0x4780, WORD_LEN, 0},	// 70003BFC              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003BFE              
{0x0F12,0x3D17, WORD_LEN, 0},	// 70003C00              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C02              
{0x0F12,0x0080, WORD_LEN, 0},	// 70003C04              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C06              
{0x0F12,0x3D53, WORD_LEN, 0},	// 70003C08              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C0A              
{0x0F12,0xB49D, WORD_LEN, 0},	// 70003C0C              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C0E              
{0x0F12,0x3DFF, WORD_LEN, 0},	// 70003C10              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C12              
{0x0F12,0x3DB3, WORD_LEN, 0},	// 70003C14              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C16              
{0x0F12,0xFFFF, WORD_LEN, 0},	// 70003C18              
{0x0F12,0x00FF, WORD_LEN, 0},	// 70003C1A              
{0x0F12,0x17E0, WORD_LEN, 0},	// 70003C1C              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C1E              
{0x0F12,0x3F7B, WORD_LEN, 0},	// 70003C20              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C22              
{0x0F12,0x053D, WORD_LEN, 0},	// 70003C24              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C26              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C28              
{0x0F12,0x0A89, WORD_LEN, 0},	// 70003C2A              
{0x0F12,0x6CD2, WORD_LEN, 0},	// 70003C2C              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C2E              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C30              
{0x0F12,0x0A9A, WORD_LEN, 0},	// 70003C32              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C34              
{0x0F12,0x02D2, WORD_LEN, 0},	// 70003C36              
{0x0F12,0x3FC9, WORD_LEN, 0},	// 70003C38              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C3A              
{0x0F12,0x9E65, WORD_LEN, 0},	// 70003C3C              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C3E              
{0x0F12,0x403D, WORD_LEN, 0},	// 70003C40              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C42              
{0x0F12,0x7C49, WORD_LEN, 0},	// 70003C44              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C46              
{0x0F12,0x40B1, WORD_LEN, 0},	// 70003C48              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C4A              
{0x0F12,0x7C63, WORD_LEN, 0},	// 70003C4C              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C4E              
{0x0F12,0x40CD, WORD_LEN, 0},	// 70003C50              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C52              
{0x0F12,0x8F01, WORD_LEN, 0},	// 70003C54              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C56              
{0x0F12,0x416F, WORD_LEN, 0},	// 70003C58              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C5A              
{0x0F12,0x7F3F, WORD_LEN, 0},	// 70003C5C              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C5E              
{0x0F12,0x41FD, WORD_LEN, 0},	// 70003C60              
{0x0F12,0x7000, WORD_LEN, 0},	// 70003C62              
{0x0F12,0x98C5, WORD_LEN, 0},	// 70003C64              
{0x0F12,0x0000, WORD_LEN, 0},	// 70003C66              
{0x0F12,0xB570, WORD_LEN, 0},	// 70003C68              
{0x0F12,0x000C, WORD_LEN, 0},	// 70003C6A              
{0x0F12,0x0015, WORD_LEN, 0},	// 70003C6C              
{0x0F12,0x0029, WORD_LEN, 0},	// 70003C6E              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003C70              
{0x0F12,0xFB42, WORD_LEN, 0},	// 70003C72              
{0x0F12,0x49F8, WORD_LEN, 0},	// 70003C74              
{0x0F12,0x00A8, WORD_LEN, 0},	// 70003C76              
{0x0F12,0x500C, WORD_LEN, 0},	// 70003C78              
{0x0F12,0xBC70, WORD_LEN, 0},	// 70003C7A              
{0x0F12,0xBC08, WORD_LEN, 0},	// 70003C7C              
{0x0F12,0x4718, WORD_LEN, 0},	// 70003C7E              
{0x0F12,0x6808, WORD_LEN, 0},	// 70003C80              
{0x0F12,0x0400, WORD_LEN, 0},	// 70003C82              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70003C84              
{0x0F12,0x6849, WORD_LEN, 0},	// 70003C86              
{0x0F12,0x0409, WORD_LEN, 0},	// 70003C88              
{0x0F12,0x0C09, WORD_LEN, 0},	// 70003C8A              
{0x0F12,0x4AF3, WORD_LEN, 0},	// 70003C8C              
{0x0F12,0x8992, WORD_LEN, 0},	// 70003C8E              
{0x0F12,0x2A00, WORD_LEN, 0},	// 70003C90              
{0x0F12,0xD00D, WORD_LEN, 0},	// 70003C92              
{0x0F12,0x2300, WORD_LEN, 0},	// 70003C94              
{0x0F12,0x1A89, WORD_LEN, 0},	// 70003C96              
{0x0F12,0xD400, WORD_LEN, 0},	// 70003C98              
{0x0F12,0x000B, WORD_LEN, 0},	// 70003C9A              
{0x0F12,0x0419, WORD_LEN, 0},	// 70003C9C              
{0x0F12,0x0C09, WORD_LEN, 0},	// 70003C9E              
{0x0F12,0x23FF, WORD_LEN, 0},	// 70003CA0              
{0x0F12,0x33C1, WORD_LEN, 0},	// 70003CA2              
{0x0F12,0x1810, WORD_LEN, 0},	// 70003CA4              
{0x0F12,0x4298, WORD_LEN, 0},	// 70003CA6              
{0x0F12,0xD800, WORD_LEN, 0},	// 70003CA8              
{0x0F12,0x0003, WORD_LEN, 0},	// 70003CAA              
{0x0F12,0x0418, WORD_LEN, 0},	// 70003CAC              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70003CAE              
{0x0F12,0x4AEB, WORD_LEN, 0},	// 70003CB0              
{0x0F12,0x8150, WORD_LEN, 0},	// 70003CB2              
{0x0F12,0x8191, WORD_LEN, 0},	// 70003CB4              
{0x0F12,0x4770, WORD_LEN, 0},	// 70003CB6              
{0x0F12,0xB5F3, WORD_LEN, 0},	// 70003CB8              
{0x0F12,0x0004, WORD_LEN, 0},	// 70003CBA              
{0x0F12,0xB081, WORD_LEN, 0},	// 70003CBC              
{0x0F12,0x9802, WORD_LEN, 0},	// 70003CBE              
{0x0F12,0x6800, WORD_LEN, 0},	// 70003CC0              
{0x0F12,0x0600, WORD_LEN, 0},	// 70003CC2              
{0x0F12,0x0E00, WORD_LEN, 0},	// 70003CC4              
{0x0F12,0x2201, WORD_LEN, 0},	// 70003CC6              
{0x0F12,0x0015, WORD_LEN, 0},	// 70003CC8              
{0x0F12,0x0021, WORD_LEN, 0},	// 70003CCA              
{0x0F12,0x3910, WORD_LEN, 0},	// 70003CCC              
{0x0F12,0x408A, WORD_LEN, 0},	// 70003CCE              
{0x0F12,0x40A5, WORD_LEN, 0},	// 70003CD0              
{0x0F12,0x4FE4, WORD_LEN, 0},	// 70003CD2              
{0x0F12,0x0016, WORD_LEN, 0},	// 70003CD4              
{0x0F12,0x2C10, WORD_LEN, 0},	// 70003CD6              
{0x0F12,0xDA03, WORD_LEN, 0},	// 70003CD8              
{0x0F12,0x8839, WORD_LEN, 0},	// 70003CDA              
{0x0F12,0x43A9, WORD_LEN, 0},	// 70003CDC              
{0x0F12,0x8039, WORD_LEN, 0},	// 70003CDE              
{0x0F12,0xE002, WORD_LEN, 0},	// 70003CE0              
{0x0F12,0x8879, WORD_LEN, 0},	// 70003CE2              
{0x0F12,0x43B1, WORD_LEN, 0},	// 70003CE4              
{0x0F12,0x8079, WORD_LEN, 0},	// 70003CE6              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003CE8              
{0x0F12,0xFB0E, WORD_LEN, 0},	// 70003CEA              
{0x0F12,0x2C10, WORD_LEN, 0},	// 70003CEC              
{0x0F12,0xDA03, WORD_LEN, 0},	// 70003CEE              
{0x0F12,0x8839, WORD_LEN, 0},	// 70003CF0              
{0x0F12,0x4329, WORD_LEN, 0},	// 70003CF2              
{0x0F12,0x8039, WORD_LEN, 0},	// 70003CF4              
{0x0F12,0xE002, WORD_LEN, 0},	// 70003CF6              
{0x0F12,0x8879, WORD_LEN, 0},	// 70003CF8              
{0x0F12,0x4331, WORD_LEN, 0},	// 70003CFA              
{0x0F12,0x8079, WORD_LEN, 0},	// 70003CFC              
{0x0F12,0x49DA, WORD_LEN, 0},	// 70003CFE              
{0x0F12,0x8809, WORD_LEN, 0},	// 70003D00              
{0x0F12,0x2900, WORD_LEN, 0},	// 70003D02              
{0x0F12,0xD102, WORD_LEN, 0},	// 70003D04              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003D06              
{0x0F12,0xFB07, WORD_LEN, 0},	// 70003D08              
{0x0F12,0x2000, WORD_LEN, 0},	// 70003D0A              
{0x0F12,0x9902, WORD_LEN, 0},	// 70003D0C              
{0x0F12,0x6008, WORD_LEN, 0},	// 70003D0E              
{0x0F12,0xBCFE, WORD_LEN, 0},	// 70003D10              
{0x0F12,0xBC08, WORD_LEN, 0},	// 70003D12              
{0x0F12,0x4718, WORD_LEN, 0},	// 70003D14              
{0x0F12,0xB538, WORD_LEN, 0},	// 70003D16              
{0x0F12,0x9C04, WORD_LEN, 0},	// 70003D18              
{0x0F12,0x0015, WORD_LEN, 0},	// 70003D1A              
{0x0F12,0x002A, WORD_LEN, 0},	// 70003D1C              
{0x0F12,0x9400, WORD_LEN, 0},	// 70003D1E              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003D20              
{0x0F12,0xFB02, WORD_LEN, 0},	// 70003D22              
{0x0F12,0x4AD1, WORD_LEN, 0},	// 70003D24              
{0x0F12,0x8811, WORD_LEN, 0},	// 70003D26              
{0x0F12,0x2900, WORD_LEN, 0},	// 70003D28              
{0x0F12,0xD00F, WORD_LEN, 0},	// 70003D2A              
{0x0F12,0x8820, WORD_LEN, 0},	// 70003D2C              
{0x0F12,0x4281, WORD_LEN, 0},	// 70003D2E              
{0x0F12,0xD20C, WORD_LEN, 0},	// 70003D30              
{0x0F12,0x8861, WORD_LEN, 0},	// 70003D32              
{0x0F12,0x8853, WORD_LEN, 0},	// 70003D34              
{0x0F12,0x4299, WORD_LEN, 0},	// 70003D36              
{0x0F12,0xD200, WORD_LEN, 0},	// 70003D38              
{0x0F12,0x1E40, WORD_LEN, 0},	// 70003D3A              
{0x0F12,0x0400, WORD_LEN, 0},	// 70003D3C              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70003D3E              
{0x0F12,0x8020, WORD_LEN, 0},	// 70003D40              
{0x0F12,0x8851, WORD_LEN, 0},	// 70003D42              
{0x0F12,0x8061, WORD_LEN, 0},	// 70003D44              
{0x0F12,0x4368, WORD_LEN, 0},	// 70003D46              
{0x0F12,0x1840, WORD_LEN, 0},	// 70003D48              
{0x0F12,0x6060, WORD_LEN, 0},	// 70003D4A              
{0x0F12,0xBC38, WORD_LEN, 0},	// 70003D4C              
{0x0F12,0xBC08, WORD_LEN, 0},	// 70003D4E              
{0x0F12,0x4718, WORD_LEN, 0},	// 70003D50              
{0x0F12,0xB5F8, WORD_LEN, 0},	// 70003D52              
{0x0F12,0x0004, WORD_LEN, 0},	// 70003D54              
{0x0F12,0x6808, WORD_LEN, 0},	// 70003D56              
{0x0F12,0x0400, WORD_LEN, 0},	// 70003D58              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70003D5A              
{0x0F12,0x2201, WORD_LEN, 0},	// 70003D5C              
{0x0F12,0x0015, WORD_LEN, 0},	// 70003D5E              
{0x0F12,0x0021, WORD_LEN, 0},	// 70003D60              
{0x0F12,0x3910, WORD_LEN, 0},	// 70003D62              
{0x0F12,0x408A, WORD_LEN, 0},	// 70003D64              
{0x0F12,0x40A5, WORD_LEN, 0},	// 70003D66              
{0x0F12,0x4FBE, WORD_LEN, 0},	// 70003D68              
{0x0F12,0x0016, WORD_LEN, 0},	// 70003D6A              
{0x0F12,0x2C10, WORD_LEN, 0},	// 70003D6C              
{0x0F12,0xDA03, WORD_LEN, 0},	// 70003D6E              
{0x0F12,0x8839, WORD_LEN, 0},	// 70003D70              
{0x0F12,0x43A9, WORD_LEN, 0},	// 70003D72              
{0x0F12,0x8039, WORD_LEN, 0},	// 70003D74              
{0x0F12,0xE002, WORD_LEN, 0},	// 70003D76              
{0x0F12,0x8879, WORD_LEN, 0},	// 70003D78              
{0x0F12,0x43B1, WORD_LEN, 0},	// 70003D7A              
{0x0F12,0x8079, WORD_LEN, 0},	// 70003D7C              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003D7E              
{0x0F12,0xFADB, WORD_LEN, 0},	// 70003D80              
{0x0F12,0x2C10, WORD_LEN, 0},	// 70003D82              
{0x0F12,0xDA03, WORD_LEN, 0},	// 70003D84              
{0x0F12,0x8838, WORD_LEN, 0},	// 70003D86              
{0x0F12,0x4328, WORD_LEN, 0},	// 70003D88              
{0x0F12,0x8038, WORD_LEN, 0},	// 70003D8A              
{0x0F12,0xE002, WORD_LEN, 0},	// 70003D8C              
{0x0F12,0x8878, WORD_LEN, 0},	// 70003D8E              
{0x0F12,0x4330, WORD_LEN, 0},	// 70003D90              
{0x0F12,0x8078, WORD_LEN, 0},	// 70003D92              
{0x0F12,0x48B6, WORD_LEN, 0},	// 70003D94              
{0x0F12,0x8800, WORD_LEN, 0},	// 70003D96              
{0x0F12,0x0400, WORD_LEN, 0},	// 70003D98              
{0x0F12,0xD507, WORD_LEN, 0},	// 70003D9A              
{0x0F12,0x4BB5, WORD_LEN, 0},	// 70003D9C              
{0x0F12,0x7819, WORD_LEN, 0},	// 70003D9E              
{0x0F12,0x4AB5, WORD_LEN, 0},	// 70003DA0              
{0x0F12,0x7810, WORD_LEN, 0},	// 70003DA2              
{0x0F12,0x7018, WORD_LEN, 0},	// 70003DA4              
{0x0F12,0x7011, WORD_LEN, 0},	// 70003DA6              
{0x0F12,0x49B4, WORD_LEN, 0},	// 70003DA8              
{0x0F12,0x8188, WORD_LEN, 0},	// 70003DAA              
{0x0F12,0xBCF8, WORD_LEN, 0},	// 70003DAC              
{0x0F12,0xBC08, WORD_LEN, 0},	// 70003DAE              
{0x0F12,0x4718, WORD_LEN, 0},	// 70003DB0              
{0x0F12,0xB538, WORD_LEN, 0},	// 70003DB2              
{0x0F12,0x48B2, WORD_LEN, 0},	// 70003DB4              
{0x0F12,0x4669, WORD_LEN, 0},	// 70003DB6              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003DB8              
{0x0F12,0xFAC6, WORD_LEN, 0},	// 70003DBA              
{0x0F12,0x48B1, WORD_LEN, 0},	// 70003DBC              
{0x0F12,0x49B0, WORD_LEN, 0},	// 70003DBE              
{0x0F12,0x69C2, WORD_LEN, 0},	// 70003DC0              
{0x0F12,0x2400, WORD_LEN, 0},	// 70003DC2              
{0x0F12,0x31A8, WORD_LEN, 0},	// 70003DC4              
{0x0F12,0x2A00, WORD_LEN, 0},	// 70003DC6              
{0x0F12,0xD008, WORD_LEN, 0},	// 70003DC8              
{0x0F12,0x61C4, WORD_LEN, 0},	// 70003DCA              
{0x0F12,0x684A, WORD_LEN, 0},	// 70003DCC              
{0x0F12,0x6242, WORD_LEN, 0},	// 70003DCE              
{0x0F12,0x6282, WORD_LEN, 0},	// 70003DD0              
{0x0F12,0x466B, WORD_LEN, 0},	// 70003DD2              
{0x0F12,0x881A, WORD_LEN, 0},	// 70003DD4              
{0x0F12,0x6302, WORD_LEN, 0},	// 70003DD6              
{0x0F12,0x885A, WORD_LEN, 0},	// 70003DD8              
{0x0F12,0x6342, WORD_LEN, 0},	// 70003DDA              
{0x0F12,0x6A02, WORD_LEN, 0},	// 70003DDC              
{0x0F12,0x2A00, WORD_LEN, 0},	// 70003DDE              
{0x0F12,0xD00A, WORD_LEN, 0},	// 70003DE0              
{0x0F12,0x6204, WORD_LEN, 0},	// 70003DE2              
{0x0F12,0x6849, WORD_LEN, 0},	// 70003DE4              
{0x0F12,0x6281, WORD_LEN, 0},	// 70003DE6              
{0x0F12,0x466B, WORD_LEN, 0},	// 70003DE8              
{0x0F12,0x8819, WORD_LEN, 0},	// 70003DEA              
{0x0F12,0x6301, WORD_LEN, 0},	// 70003DEC              
{0x0F12,0x8859, WORD_LEN, 0},	// 70003DEE              
{0x0F12,0x6341, WORD_LEN, 0},	// 70003DF0              
{0x0F12,0x49A5, WORD_LEN, 0},	// 70003DF2              
{0x0F12,0x88C9, WORD_LEN, 0},	// 70003DF4              
{0x0F12,0x63C1, WORD_LEN, 0},	// 70003DF6              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003DF8              
{0x0F12,0xFAAE, WORD_LEN, 0},	// 70003DFA              
{0x0F12,0xE7A6, WORD_LEN, 0},	// 70003DFC              
{0x0F12,0xB5F0, WORD_LEN, 0},	// 70003DFE              
{0x0F12,0xB08B, WORD_LEN, 0},	// 70003E00              
{0x0F12,0x20FF, WORD_LEN, 0},	// 70003E02              
{0x0F12,0x1C40, WORD_LEN, 0},	// 70003E04              
{0x0F12,0x49A1, WORD_LEN, 0},	// 70003E06              
{0x0F12,0x89CC, WORD_LEN, 0},	// 70003E08              
{0x0F12,0x4E9E, WORD_LEN, 0},	// 70003E0A              
{0x0F12,0x6AB1, WORD_LEN, 0},	// 70003E0C              
{0x0F12,0x4284, WORD_LEN, 0},	// 70003E0E              
{0x0F12,0xD101, WORD_LEN, 0},	// 70003E10              
{0x0F12,0x489F, WORD_LEN, 0},	// 70003E12              
{0x0F12,0x6081, WORD_LEN, 0},	// 70003E14              
{0x0F12,0x6A70, WORD_LEN, 0},	// 70003E16              
{0x0F12,0x0200, WORD_LEN, 0},	// 70003E18              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003E1A              
{0x0F12,0xFAA5, WORD_LEN, 0},	// 70003E1C              
{0x0F12,0x0400, WORD_LEN, 0},	// 70003E1E              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70003E20              
{0x0F12,0x4A96, WORD_LEN, 0},	// 70003E22              
{0x0F12,0x8A11, WORD_LEN, 0},	// 70003E24              
{0x0F12,0x9109, WORD_LEN, 0},	// 70003E26              
{0x0F12,0x2101, WORD_LEN, 0},	// 70003E28              
{0x0F12,0x0349, WORD_LEN, 0},	// 70003E2A              
{0x0F12,0x4288, WORD_LEN, 0},	// 70003E2C              
{0x0F12,0xD200, WORD_LEN, 0},	// 70003E2E              
{0x0F12,0x0001, WORD_LEN, 0},	// 70003E30              
{0x0F12,0x4A92, WORD_LEN, 0},	// 70003E32              
{0x0F12,0x8211, WORD_LEN, 0},	// 70003E34              
{0x0F12,0x4D97, WORD_LEN, 0},	// 70003E36              
{0x0F12,0x8829, WORD_LEN, 0},	// 70003E38              
{0x0F12,0x9108, WORD_LEN, 0},	// 70003E3A              
{0x0F12,0x4A8B, WORD_LEN, 0},	// 70003E3C              
{0x0F12,0x2303, WORD_LEN, 0},	// 70003E3E              
{0x0F12,0x3222, WORD_LEN, 0},	// 70003E40              
{0x0F12,0x1F91, WORD_LEN, 0},	// 70003E42              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003E44              
{0x0F12,0xFA96, WORD_LEN, 0},	// 70003E46              
{0x0F12,0x8028, WORD_LEN, 0},	// 70003E48              
{0x0F12,0x488E, WORD_LEN, 0},	// 70003E4A              
{0x0F12,0x4987, WORD_LEN, 0},	// 70003E4C              
{0x0F12,0x6BC2, WORD_LEN, 0},	// 70003E4E              
{0x0F12,0x6AC0, WORD_LEN, 0},	// 70003E50              
{0x0F12,0x4282, WORD_LEN, 0},	// 70003E52              
{0x0F12,0xD201, WORD_LEN, 0},	// 70003E54              
{0x0F12,0x8CC8, WORD_LEN, 0},	// 70003E56              
{0x0F12,0x8028, WORD_LEN, 0},	// 70003E58              
{0x0F12,0x88E8, WORD_LEN, 0},	// 70003E5A              
{0x0F12,0x9007, WORD_LEN, 0},	// 70003E5C              
{0x0F12,0x2240, WORD_LEN, 0},	// 70003E5E              
{0x0F12,0x4310, WORD_LEN, 0},	// 70003E60              
{0x0F12,0x80E8, WORD_LEN, 0},	// 70003E62              
{0x0F12,0x2000, WORD_LEN, 0},	// 70003E64              
{0x0F12,0x0041, WORD_LEN, 0},	// 70003E66              
{0x0F12,0x194B, WORD_LEN, 0},	// 70003E68              
{0x0F12,0x001E, WORD_LEN, 0},	// 70003E6A              
{0x0F12,0x3680, WORD_LEN, 0},	// 70003E6C              
{0x0F12,0x8BB2, WORD_LEN, 0},	// 70003E6E              
{0x0F12,0xAF04, WORD_LEN, 0},	// 70003E70              
{0x0F12,0x527A, WORD_LEN, 0},	// 70003E72              
{0x0F12,0x4A7D, WORD_LEN, 0},	// 70003E74              
{0x0F12,0x188A, WORD_LEN, 0},	// 70003E76              
{0x0F12,0x8897, WORD_LEN, 0},	// 70003E78              
{0x0F12,0x83B7, WORD_LEN, 0},	// 70003E7A              
{0x0F12,0x33A0, WORD_LEN, 0},	// 70003E7C              
{0x0F12,0x891F, WORD_LEN, 0},	// 70003E7E              
{0x0F12,0xAE01, WORD_LEN, 0},	// 70003E80              
{0x0F12,0x5277, WORD_LEN, 0},	// 70003E82              
{0x0F12,0x8A11, WORD_LEN, 0},	// 70003E84              
{0x0F12,0x8119, WORD_LEN, 0},	// 70003E86              
{0x0F12,0x1C40, WORD_LEN, 0},	// 70003E88              
{0x0F12,0x0400, WORD_LEN, 0},	// 70003E8A              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70003E8C              
{0x0F12,0x2806, WORD_LEN, 0},	// 70003E8E              
{0x0F12,0xD3E9, WORD_LEN, 0},	// 70003E90              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003E92              
{0x0F12,0xFA77, WORD_LEN, 0},	// 70003E94              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003E96              
{0x0F12,0xFA7D, WORD_LEN, 0},	// 70003E98              
{0x0F12,0x4F79, WORD_LEN, 0},	// 70003E9A              
{0x0F12,0x37A8, WORD_LEN, 0},	// 70003E9C              
{0x0F12,0x2800, WORD_LEN, 0},	// 70003E9E              
{0x0F12,0xD10A, WORD_LEN, 0},	// 70003EA0              
{0x0F12,0x1FE0, WORD_LEN, 0},	// 70003EA2              
{0x0F12,0x38FD, WORD_LEN, 0},	// 70003EA4              
{0x0F12,0xD001, WORD_LEN, 0},	// 70003EA6              
{0x0F12,0x1CC0, WORD_LEN, 0},	// 70003EA8              
{0x0F12,0xD105, WORD_LEN, 0},	// 70003EAA              
{0x0F12,0x4874, WORD_LEN, 0},	// 70003EAC              
{0x0F12,0x8829, WORD_LEN, 0},	// 70003EAE              
{0x0F12,0x3818, WORD_LEN, 0},	// 70003EB0              
{0x0F12,0x6840, WORD_LEN, 0},	// 70003EB2              
{0x0F12,0x4348, WORD_LEN, 0},	// 70003EB4              
{0x0F12,0x6078, WORD_LEN, 0},	// 70003EB6              
{0x0F12,0x4972, WORD_LEN, 0},	// 70003EB8              
{0x0F12,0x6878, WORD_LEN, 0},	// 70003EBA              
{0x0F12,0x6B89, WORD_LEN, 0},	// 70003EBC              
{0x0F12,0x4288, WORD_LEN, 0},	// 70003EBE              
{0x0F12,0xD300, WORD_LEN, 0},	// 70003EC0              
{0x0F12,0x0008, WORD_LEN, 0},	// 70003EC2              
{0x0F12,0x6078, WORD_LEN, 0},	// 70003EC4              
{0x0F12,0x2000, WORD_LEN, 0},	// 70003EC6              
{0x0F12,0x0041, WORD_LEN, 0},	// 70003EC8              
{0x0F12,0xAA04, WORD_LEN, 0},	// 70003ECA              
{0x0F12,0x5A53, WORD_LEN, 0},	// 70003ECC              
{0x0F12,0x194A, WORD_LEN, 0},	// 70003ECE              
{0x0F12,0x269C, WORD_LEN, 0},	// 70003ED0              
{0x0F12,0x52B3, WORD_LEN, 0},	// 70003ED2              
{0x0F12,0xAB01, WORD_LEN, 0},	// 70003ED4              
{0x0F12,0x5A59, WORD_LEN, 0},	// 70003ED6              
{0x0F12,0x32A0, WORD_LEN, 0},	// 70003ED8              
{0x0F12,0x8111, WORD_LEN, 0},	// 70003EDA              
{0x0F12,0x1C40, WORD_LEN, 0},	// 70003EDC              
{0x0F12,0x0400, WORD_LEN, 0},	// 70003EDE              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70003EE0              
{0x0F12,0x2806, WORD_LEN, 0},	// 70003EE2              
{0x0F12,0xD3F0, WORD_LEN, 0},	// 70003EE4              
{0x0F12,0x4965, WORD_LEN, 0},	// 70003EE6              
{0x0F12,0x9809, WORD_LEN, 0},	// 70003EE8              
{0x0F12,0x8208, WORD_LEN, 0},	// 70003EEA              
{0x0F12,0x9808, WORD_LEN, 0},	// 70003EEC              
{0x0F12,0x8028, WORD_LEN, 0},	// 70003EEE              
{0x0F12,0x9807, WORD_LEN, 0},	// 70003EF0              
{0x0F12,0x80E8, WORD_LEN, 0},	// 70003EF2              
{0x0F12,0x1FE0, WORD_LEN, 0},	// 70003EF4              
{0x0F12,0x38FD, WORD_LEN, 0},	// 70003EF6              
{0x0F12,0xD13B, WORD_LEN, 0},	// 70003EF8              
{0x0F12,0x4D64, WORD_LEN, 0},	// 70003EFA              
{0x0F12,0x89E8, WORD_LEN, 0},	// 70003EFC              
{0x0F12,0x1FC1, WORD_LEN, 0},	// 70003EFE              
{0x0F12,0x39FF, WORD_LEN, 0},	// 70003F00              
{0x0F12,0xD136, WORD_LEN, 0},	// 70003F02              
{0x0F12,0x4C5F, WORD_LEN, 0},	// 70003F04              
{0x0F12,0x8AE0, WORD_LEN, 0},	// 70003F06              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003F08              
{0x0F12,0xFA4C, WORD_LEN, 0},	// 70003F0A              
{0x0F12,0x0006, WORD_LEN, 0},	// 70003F0C              
{0x0F12,0x8B20, WORD_LEN, 0},	// 70003F0E              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003F10              
{0x0F12,0xFA50, WORD_LEN, 0},	// 70003F12              
{0x0F12,0x9000, WORD_LEN, 0},	// 70003F14              
{0x0F12,0x6AA1, WORD_LEN, 0},	// 70003F16              
{0x0F12,0x6878, WORD_LEN, 0},	// 70003F18              
{0x0F12,0x1809, WORD_LEN, 0},	// 70003F1A              
{0x0F12,0x0200, WORD_LEN, 0},	// 70003F1C              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003F1E              
{0x0F12,0xFA23, WORD_LEN, 0},	// 70003F20              
{0x0F12,0x0400, WORD_LEN, 0},	// 70003F22              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70003F24              
{0x0F12,0x0022, WORD_LEN, 0},	// 70003F26              
{0x0F12,0x3246, WORD_LEN, 0},	// 70003F28              
{0x0F12,0x0011, WORD_LEN, 0},	// 70003F2A              
{0x0F12,0x310A, WORD_LEN, 0},	// 70003F2C              
{0x0F12,0x2305, WORD_LEN, 0},	// 70003F2E              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003F30              
{0x0F12,0xFA20, WORD_LEN, 0},	// 70003F32              
{0x0F12,0x66E8, WORD_LEN, 0},	// 70003F34              
{0x0F12,0x6B23, WORD_LEN, 0},	// 70003F36              
{0x0F12,0x0002, WORD_LEN, 0},	// 70003F38              
{0x0F12,0x0031, WORD_LEN, 0},	// 70003F3A              
{0x0F12,0x0018, WORD_LEN, 0},	// 70003F3C              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003F3E              
{0x0F12,0xFA41, WORD_LEN, 0},	// 70003F40              
{0x0F12,0x466B, WORD_LEN, 0},	// 70003F42              
{0x0F12,0x8518, WORD_LEN, 0},	// 70003F44              
{0x0F12,0x6EEA, WORD_LEN, 0},	// 70003F46              
{0x0F12,0x6B60, WORD_LEN, 0},	// 70003F48              
{0x0F12,0x9900, WORD_LEN, 0},	// 70003F4A              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003F4C              
{0x0F12,0xFA3A, WORD_LEN, 0},	// 70003F4E              
{0x0F12,0x466B, WORD_LEN, 0},	// 70003F50              
{0x0F12,0x8558, WORD_LEN, 0},	// 70003F52              
{0x0F12,0x0029, WORD_LEN, 0},	// 70003F54              
{0x0F12,0x980A, WORD_LEN, 0},	// 70003F56              
{0x0F12,0x3170, WORD_LEN, 0},	// 70003F58              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003F5A              
{0x0F12,0xFA3B, WORD_LEN, 0},	// 70003F5C              
{0x0F12,0x0028, WORD_LEN, 0},	// 70003F5E              
{0x0F12,0x3060, WORD_LEN, 0},	// 70003F60              
{0x0F12,0x8A02, WORD_LEN, 0},	// 70003F62              
{0x0F12,0x4946, WORD_LEN, 0},	// 70003F64              
{0x0F12,0x3128, WORD_LEN, 0},	// 70003F66              
{0x0F12,0x808A, WORD_LEN, 0},	// 70003F68              
{0x0F12,0x8A42, WORD_LEN, 0},	// 70003F6A              
{0x0F12,0x80CA, WORD_LEN, 0},	// 70003F6C              
{0x0F12,0x8A80, WORD_LEN, 0},	// 70003F6E              
{0x0F12,0x8108, WORD_LEN, 0},	// 70003F70              
{0x0F12,0xB00B, WORD_LEN, 0},	// 70003F72              
{0x0F12,0xBCF0, WORD_LEN, 0},	// 70003F74              
{0x0F12,0xBC08, WORD_LEN, 0},	// 70003F76              
{0x0F12,0x4718, WORD_LEN, 0},	// 70003F78              
{0x0F12,0xB570, WORD_LEN, 0},	// 70003F7A              
{0x0F12,0x2400, WORD_LEN, 0},	// 70003F7C              
{0x0F12,0x4D46, WORD_LEN, 0},	// 70003F7E              
{0x0F12,0x4846, WORD_LEN, 0},	// 70003F80              
{0x0F12,0x8881, WORD_LEN, 0},	// 70003F82              
{0x0F12,0x4846, WORD_LEN, 0},	// 70003F84              
{0x0F12,0x8041, WORD_LEN, 0},	// 70003F86              
{0x0F12,0x2101, WORD_LEN, 0},	// 70003F88              
{0x0F12,0x8001, WORD_LEN, 0},	// 70003F8A              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003F8C              
{0x0F12,0xFA2A, WORD_LEN, 0},	// 70003F8E              
{0x0F12,0x4842, WORD_LEN, 0},	// 70003F90              
{0x0F12,0x3820, WORD_LEN, 0},	// 70003F92              
{0x0F12,0x8BC0, WORD_LEN, 0},	// 70003F94              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003F96              
{0x0F12,0xFA2D, WORD_LEN, 0},	// 70003F98              
{0x0F12,0x4B42, WORD_LEN, 0},	// 70003F9A              
{0x0F12,0x220D, WORD_LEN, 0},	// 70003F9C              
{0x0F12,0x0712, WORD_LEN, 0},	// 70003F9E              
{0x0F12,0x18A8, WORD_LEN, 0},	// 70003FA0              
{0x0F12,0x8806, WORD_LEN, 0},	// 70003FA2              
{0x0F12,0x00E1, WORD_LEN, 0},	// 70003FA4              
{0x0F12,0x18C9, WORD_LEN, 0},	// 70003FA6              
{0x0F12,0x81CE, WORD_LEN, 0},	// 70003FA8              
{0x0F12,0x8846, WORD_LEN, 0},	// 70003FAA              
{0x0F12,0x818E, WORD_LEN, 0},	// 70003FAC              
{0x0F12,0x8886, WORD_LEN, 0},	// 70003FAE              
{0x0F12,0x824E, WORD_LEN, 0},	// 70003FB0              
{0x0F12,0x88C0, WORD_LEN, 0},	// 70003FB2              
{0x0F12,0x8208, WORD_LEN, 0},	// 70003FB4              
{0x0F12,0x3508, WORD_LEN, 0},	// 70003FB6              
{0x0F12,0x042D, WORD_LEN, 0},	// 70003FB8              
{0x0F12,0x0C2D, WORD_LEN, 0},	// 70003FBA              
{0x0F12,0x1C64, WORD_LEN, 0},	// 70003FBC              
{0x0F12,0x0424, WORD_LEN, 0},	// 70003FBE              
{0x0F12,0x0C24, WORD_LEN, 0},	// 70003FC0              
{0x0F12,0x2C07, WORD_LEN, 0},	// 70003FC2              
{0x0F12,0xD3EC, WORD_LEN, 0},	// 70003FC4              
{0x0F12,0xE658, WORD_LEN, 0},	// 70003FC6              
{0x0F12,0xB510, WORD_LEN, 0},	// 70003FC8              
{0x0F12,0x4834, WORD_LEN, 0},	// 70003FCA              
{0x0F12,0x4C34, WORD_LEN, 0},	// 70003FCC              
{0x0F12,0x88C0, WORD_LEN, 0},	// 70003FCE              
{0x0F12,0x8060, WORD_LEN, 0},	// 70003FD0              
{0x0F12,0x2001, WORD_LEN, 0},	// 70003FD2              
{0x0F12,0x8020, WORD_LEN, 0},	// 70003FD4              
{0x0F12,0x4831, WORD_LEN, 0},	// 70003FD6              
{0x0F12,0x3820, WORD_LEN, 0},	// 70003FD8              
{0x0F12,0x8BC0, WORD_LEN, 0},	// 70003FDA              
{0x0F12,0xF000, WORD_LEN, 0},	// 70003FDC              
{0x0F12,0xFA0A, WORD_LEN, 0},	// 70003FDE              
{0x0F12,0x88E0, WORD_LEN, 0},	// 70003FE0              
{0x0F12,0x4A31, WORD_LEN, 0},	// 70003FE2              
{0x0F12,0x2800, WORD_LEN, 0},	// 70003FE4              
{0x0F12,0xD003, WORD_LEN, 0},	// 70003FE6              
{0x0F12,0x4930, WORD_LEN, 0},	// 70003FE8              
{0x0F12,0x8849, WORD_LEN, 0},	// 70003FEA              
{0x0F12,0x2900, WORD_LEN, 0},	// 70003FEC              
{0x0F12,0xD009, WORD_LEN, 0},	// 70003FEE              
{0x0F12,0x2001, WORD_LEN, 0},	// 70003FF0              
{0x0F12,0x03C0, WORD_LEN, 0},	// 70003FF2              
{0x0F12,0x8050, WORD_LEN, 0},	// 70003FF4              
{0x0F12,0x80D0, WORD_LEN, 0},	// 70003FF6              
{0x0F12,0x2000, WORD_LEN, 0},	// 70003FF8              
{0x0F12,0x8090, WORD_LEN, 0},	// 70003FFA              
{0x0F12,0x8110, WORD_LEN, 0},	// 70003FFC              
{0x0F12,0xBC10, WORD_LEN, 0},	// 70003FFE              
{0x0F12,0xBC08, WORD_LEN, 0},	// 70004000              
{0x0F12,0x4718, WORD_LEN, 0},	// 70004002              
{0x0F12,0x8050, WORD_LEN, 0},	// 70004004              
{0x0F12,0x8920, WORD_LEN, 0},	// 70004006              
{0x0F12,0x80D0, WORD_LEN, 0},	// 70004008              
{0x0F12,0x8960, WORD_LEN, 0},	// 7000400A              
{0x0F12,0x0400, WORD_LEN, 0},	// 7000400C              
{0x0F12,0x1400, WORD_LEN, 0},	// 7000400E              
{0x0F12,0x8090, WORD_LEN, 0},	// 70004010              
{0x0F12,0x89A1, WORD_LEN, 0},	// 70004012              
{0x0F12,0x0409, WORD_LEN, 0},	// 70004014              
{0x0F12,0x1409, WORD_LEN, 0},	// 70004016              
{0x0F12,0x8111, WORD_LEN, 0},	// 70004018              
{0x0F12,0x89E3, WORD_LEN, 0},	// 7000401A              
{0x0F12,0x8A24, WORD_LEN, 0},	// 7000401C              
{0x0F12,0x2B00, WORD_LEN, 0},	// 7000401E              
{0x0F12,0xD104, WORD_LEN, 0},	// 70004020              
{0x0F12,0x17C3, WORD_LEN, 0},	// 70004022              
{0x0F12,0x0F5B, WORD_LEN, 0},	// 70004024              
{0x0F12,0x1818, WORD_LEN, 0},	// 70004026              
{0x0F12,0x10C0, WORD_LEN, 0},	// 70004028              
{0x0F12,0x8090, WORD_LEN, 0},	// 7000402A              
{0x0F12,0x2C00, WORD_LEN, 0},	// 7000402C              
{0x0F12,0xD1E6, WORD_LEN, 0},	// 7000402E              
{0x0F12,0x17C8, WORD_LEN, 0},	// 70004030              
{0x0F12,0x0F40, WORD_LEN, 0},	// 70004032              
{0x0F12,0x1840, WORD_LEN, 0},	// 70004034              
{0x0F12,0x10C0, WORD_LEN, 0},	// 70004036              
{0x0F12,0x8110, WORD_LEN, 0},	// 70004038              
{0x0F12,0xE7E0, WORD_LEN, 0},	// 7000403A              
{0x0F12,0xB510, WORD_LEN, 0},	// 7000403C              
{0x0F12,0x000C, WORD_LEN, 0},	// 7000403E              
{0x0F12,0x4919, WORD_LEN, 0},	// 70004040              
{0x0F12,0x2204, WORD_LEN, 0},	// 70004042              
{0x0F12,0x6820, WORD_LEN, 0},	// 70004044              
{0x0F12,0x5E8A, WORD_LEN, 0},	// 70004046              
{0x0F12,0x0140, WORD_LEN, 0},	// 70004048              
{0x0F12,0x1A80, WORD_LEN, 0},	// 7000404A              
{0x0F12,0x0280, WORD_LEN, 0},	// 7000404C              
{0x0F12,0x8849, WORD_LEN, 0},	// 7000404E              
{0x0F12,0xF000, WORD_LEN, 0},	// 70004050              
{0x0F12,0xF9D8, WORD_LEN, 0},	// 70004052              
{0x0F12,0x6020, WORD_LEN, 0},	// 70004054              
{0x0F12,0xE7D2, WORD_LEN, 0},	// 70004056              
{0x0F12,0x38D4, WORD_LEN, 0},	// 70004058              
{0x0F12,0x7000, WORD_LEN, 0},	// 7000405A              
{0x0F12,0x17D0, WORD_LEN, 0},	// 7000405C              
{0x0F12,0x7000, WORD_LEN, 0},	// 7000405E              
{0x0F12,0x5000, WORD_LEN, 0},	// 70004060              
{0x0F12,0xD000, WORD_LEN, 0},	// 70004062              
{0x0F12,0x1100, WORD_LEN, 0},	// 70004064              
{0x0F12,0xD000, WORD_LEN, 0},	// 70004066              
{0x0F12,0x171A, WORD_LEN, 0},	// 70004068              
{0x0F12,0x7000, WORD_LEN, 0},	// 7000406A              
{0x0F12,0x4780, WORD_LEN, 0},	// 7000406C              
{0x0F12,0x7000, WORD_LEN, 0},	// 7000406E              
{0x0F12,0x2FCA, WORD_LEN, 0},	// 70004070              
{0x0F12,0x7000, WORD_LEN, 0},	// 70004072              
{0x0F12,0x2FC5, WORD_LEN, 0},	// 70004074              
{0x0F12,0x7000, WORD_LEN, 0},	// 70004076              
{0x0F12,0x2FC6, WORD_LEN, 0},	// 70004078              
{0x0F12,0x7000, WORD_LEN, 0},	// 7000407A              
{0x0F12,0x2ED8, WORD_LEN, 0},	// 7000407C              
{0x0F12,0x7000, WORD_LEN, 0},	// 7000407E              
{0x0F12,0x2BD0, WORD_LEN, 0},	// 70004080              
{0x0F12,0x7000, WORD_LEN, 0},	// 70004082              
{0x0F12,0x17E0, WORD_LEN, 0},	// 70004084              
{0x0F12,0x7000, WORD_LEN, 0},	// 70004086              
{0x0F12,0x2DE8, WORD_LEN, 0},	// 70004088              
{0x0F12,0x7000, WORD_LEN, 0},	// 7000408A              
{0x0F12,0x37E0, WORD_LEN, 0},	// 7000408C              
{0x0F12,0x7000, WORD_LEN, 0},	// 7000408E              
{0x0F12,0x210C, WORD_LEN, 0},	// 70004090              
{0x0F12,0x7000, WORD_LEN, 0},	// 70004092              
{0x0F12,0x1484, WORD_LEN, 0},	// 70004094              
{0x0F12,0x7000, WORD_LEN, 0},	// 70004096              
{0x0F12,0xA006, WORD_LEN, 0},	// 70004098              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000409A              
{0x0F12,0x0724, WORD_LEN, 0},	// 7000409C              
{0x0F12,0x7000, WORD_LEN, 0},	// 7000409E              
{0x0F12,0xA000, WORD_LEN, 0},	// 700040A0              
{0x0F12,0xD000, WORD_LEN, 0},	// 700040A2              
{0x0F12,0x2270, WORD_LEN, 0},	// 700040A4              
{0x0F12,0x7000, WORD_LEN, 0},	// 700040A6              
{0x0F12,0x2558, WORD_LEN, 0},	// 700040A8              
{0x0F12,0x7000, WORD_LEN, 0},	// 700040AA              
{0x0F12,0x146C, WORD_LEN, 0},	// 700040AC              
{0x0F12,0x7000, WORD_LEN, 0},	// 700040AE              
{0x0F12,0xB510, WORD_LEN, 0},	// 700040B0              
{0x0F12,0x000C, WORD_LEN, 0},	// 700040B2              
{0x0F12,0x4983, WORD_LEN, 0},	// 700040B4              
{0x0F12,0x2208, WORD_LEN, 0},	// 700040B6              
{0x0F12,0x6820, WORD_LEN, 0},	// 700040B8              
{0x0F12,0x5E8A, WORD_LEN, 0},	// 700040BA              
{0x0F12,0x0140, WORD_LEN, 0},	// 700040BC              
{0x0F12,0x1A80, WORD_LEN, 0},	// 700040BE              
{0x0F12,0x0280, WORD_LEN, 0},	// 700040C0              
{0x0F12,0x88C9, WORD_LEN, 0},	// 700040C2              
{0x0F12,0xF000, WORD_LEN, 0},	// 700040C4              
{0x0F12,0xF99E, WORD_LEN, 0},	// 700040C6              
{0x0F12,0x6020, WORD_LEN, 0},	// 700040C8              
{0x0F12,0xE798, WORD_LEN, 0},	// 700040CA              
{0x0F12,0xB5FE, WORD_LEN, 0},	// 700040CC              
{0x0F12,0x000C, WORD_LEN, 0},	// 700040CE              
{0x0F12,0x6825, WORD_LEN, 0},	// 700040D0              
{0x0F12,0x6866, WORD_LEN, 0},	// 700040D2              
{0x0F12,0x68A0, WORD_LEN, 0},	// 700040D4              
{0x0F12,0x9001, WORD_LEN, 0},	// 700040D6              
{0x0F12,0x68E7, WORD_LEN, 0},	// 700040D8              
{0x0F12,0x1BA8, WORD_LEN, 0},	// 700040DA              
{0x0F12,0x42B5, WORD_LEN, 0},	// 700040DC              
{0x0F12,0xDA00, WORD_LEN, 0},	// 700040DE              
{0x0F12,0x1B70, WORD_LEN, 0},	// 700040E0              
{0x0F12,0x9000, WORD_LEN, 0},	// 700040E2              
{0x0F12,0x4977, WORD_LEN, 0},	// 700040E4              
{0x0F12,0x4878, WORD_LEN, 0},	// 700040E6              
{0x0F12,0x884A, WORD_LEN, 0},	// 700040E8              
{0x0F12,0x8843, WORD_LEN, 0},	// 700040EA              
{0x0F12,0x435A, WORD_LEN, 0},	// 700040EC              
{0x0F12,0x2304, WORD_LEN, 0},	// 700040EE              
{0x0F12,0x5ECB, WORD_LEN, 0},	// 700040F0              
{0x0F12,0x0A92, WORD_LEN, 0},	// 700040F2              
{0x0F12,0x18D2, WORD_LEN, 0},	// 700040F4              
{0x0F12,0x02D2, WORD_LEN, 0},	// 700040F6              
{0x0F12,0x0C12, WORD_LEN, 0},	// 700040F8              
{0x0F12,0x88CB, WORD_LEN, 0},	// 700040FA              
{0x0F12,0x8880, WORD_LEN, 0},	// 700040FC              
{0x0F12,0x4343, WORD_LEN, 0},	// 700040FE              
{0x0F12,0x0A98, WORD_LEN, 0},	// 70004100              
{0x0F12,0x2308, WORD_LEN, 0},	// 70004102              
{0x0F12,0x5ECB, WORD_LEN, 0},	// 70004104              
{0x0F12,0x18C0, WORD_LEN, 0},	// 70004106              
{0x0F12,0x02C0, WORD_LEN, 0},	// 70004108              
{0x0F12,0x0C00, WORD_LEN, 0},	// 7000410A              
{0x0F12,0x0411, WORD_LEN, 0},	// 7000410C              
{0x0F12,0x0400, WORD_LEN, 0},	// 7000410E              
{0x0F12,0x1409, WORD_LEN, 0},	// 70004110              
{0x0F12,0x1400, WORD_LEN, 0},	// 70004112              
{0x0F12,0x1A08, WORD_LEN, 0},	// 70004114              
{0x0F12,0x496C, WORD_LEN, 0},	// 70004116              
{0x0F12,0x39E0, WORD_LEN, 0},	// 70004118              
{0x0F12,0x6148, WORD_LEN, 0},	// 7000411A              
{0x0F12,0x9801, WORD_LEN, 0},	// 7000411C              
{0x0F12,0x3040, WORD_LEN, 0},	// 7000411E              
{0x0F12,0x7880, WORD_LEN, 0},	// 70004120              
{0x0F12,0x2800, WORD_LEN, 0},	// 70004122              
{0x0F12,0xD103, WORD_LEN, 0},	// 70004124              
{0x0F12,0x9801, WORD_LEN, 0},	// 70004126              
{0x0F12,0x0029, WORD_LEN, 0},	// 70004128              
{0x0F12,0xF000, WORD_LEN, 0},	// 7000412A              
{0x0F12,0xF971, WORD_LEN, 0},	// 7000412C              
{0x0F12,0x8839, WORD_LEN, 0},	// 7000412E              
{0x0F12,0x9800, WORD_LEN, 0},	// 70004130              
{0x0F12,0x4281, WORD_LEN, 0},	// 70004132              
{0x0F12,0xD814, WORD_LEN, 0},	// 70004134              
{0x0F12,0x8879, WORD_LEN, 0},	// 70004136              
{0x0F12,0x9800, WORD_LEN, 0},	// 70004138              
{0x0F12,0x4281, WORD_LEN, 0},	// 7000413A              
{0x0F12,0xD20C, WORD_LEN, 0},	// 7000413C              
{0x0F12,0x9801, WORD_LEN, 0},	// 7000413E              
{0x0F12,0x0029, WORD_LEN, 0},	// 70004140              
{0x0F12,0xF000, WORD_LEN, 0},	// 70004142              
{0x0F12,0xF96D, WORD_LEN, 0},	// 70004144              
{0x0F12,0x9801, WORD_LEN, 0},	// 70004146              
{0x0F12,0x0029, WORD_LEN, 0},	// 70004148              
{0x0F12,0xF000, WORD_LEN, 0},	// 7000414A              
{0x0F12,0xF969, WORD_LEN, 0},	// 7000414C              
{0x0F12,0x9801, WORD_LEN, 0},	// 7000414E              
{0x0F12,0x0029, WORD_LEN, 0},	// 70004150              
{0x0F12,0xF000, WORD_LEN, 0},	// 70004152              
{0x0F12,0xF965, WORD_LEN, 0},	// 70004154              
{0x0F12,0xE003, WORD_LEN, 0},	// 70004156              
{0x0F12,0x9801, WORD_LEN, 0},	// 70004158              
{0x0F12,0x0029, WORD_LEN, 0},	// 7000415A              
{0x0F12,0xF000, WORD_LEN, 0},	// 7000415C              
{0x0F12,0xF960, WORD_LEN, 0},	// 7000415E              
{0x0F12,0x9801, WORD_LEN, 0},	// 70004160              
{0x0F12,0x0032, WORD_LEN, 0},	// 70004162              
{0x0F12,0x0039, WORD_LEN, 0},	// 70004164              
{0x0F12,0xF000, WORD_LEN, 0},	// 70004166              
{0x0F12,0xF963, WORD_LEN, 0},	// 70004168              
{0x0F12,0x6020, WORD_LEN, 0},	// 7000416A              
{0x0F12,0xE5D0, WORD_LEN, 0},	// 7000416C              
{0x0F12,0xB57C, WORD_LEN, 0},	// 7000416E              
{0x0F12,0x4856, WORD_LEN, 0},	// 70004170              
{0x0F12,0xA901, WORD_LEN, 0},	// 70004172              
{0x0F12,0x0004, WORD_LEN, 0},	// 70004174              
{0x0F12,0xF000, WORD_LEN, 0},	// 70004176              
{0x0F12,0xF8E7, WORD_LEN, 0},	// 70004178              
{0x0F12,0x466B, WORD_LEN, 0},	// 7000417A              
{0x0F12,0x88D9, WORD_LEN, 0},	// 7000417C              
{0x0F12,0x8898, WORD_LEN, 0},	// 7000417E              
{0x0F12,0x4B51, WORD_LEN, 0},	// 70004180              
{0x0F12,0x3346, WORD_LEN, 0},	// 70004182              
{0x0F12,0x1E9A, WORD_LEN, 0},	// 70004184              
{0x0F12,0xF000, WORD_LEN, 0},	// 70004186              
{0x0F12,0xF95B, WORD_LEN, 0},	// 70004188              
{0x0F12,0x4850, WORD_LEN, 0},	// 7000418A              
{0x0F12,0x494E, WORD_LEN, 0},	// 7000418C              
{0x0F12,0x3812, WORD_LEN, 0},	// 7000418E              
{0x0F12,0x3140, WORD_LEN, 0},	// 70004190              
{0x0F12,0x8A42, WORD_LEN, 0},	// 70004192              
{0x0F12,0x888B, WORD_LEN, 0},	// 70004194              
{0x0F12,0x18D2, WORD_LEN, 0},	// 70004196              
{0x0F12,0x8242, WORD_LEN, 0},	// 70004198              
{0x0F12,0x8AC2, WORD_LEN, 0},	// 7000419A              
{0x0F12,0x88C9, WORD_LEN, 0},	// 7000419C              
{0x0F12,0x1851, WORD_LEN, 0},	// 7000419E              
{0x0F12,0x82C1, WORD_LEN, 0},	// 700041A0              
{0x0F12,0x0020, WORD_LEN, 0},	// 700041A2              
{0x0F12,0x4669, WORD_LEN, 0},	// 700041A4              
{0x0F12,0xF000, WORD_LEN, 0},	// 700041A6              
{0x0F12,0xF8CF, WORD_LEN, 0},	// 700041A8              
{0x0F12,0x4849, WORD_LEN, 0},	// 700041AA              
{0x0F12,0x214D, WORD_LEN, 0},	// 700041AC              
{0x0F12,0x8301, WORD_LEN, 0},	// 700041AE              
{0x0F12,0x2196, WORD_LEN, 0},	// 700041B0              
{0x0F12,0x8381, WORD_LEN, 0},	// 700041B2              
{0x0F12,0x211D, WORD_LEN, 0},	// 700041B4              
{0x0F12,0x3020, WORD_LEN, 0},	// 700041B6              
{0x0F12,0x8001, WORD_LEN, 0},	// 700041B8              
{0x0F12,0xF000, WORD_LEN, 0},	// 700041BA              
{0x0F12,0xF949, WORD_LEN, 0},	// 700041BC              
{0x0F12,0xF000, WORD_LEN, 0},	// 700041BE              
{0x0F12,0xF94F, WORD_LEN, 0},	// 700041C0              
{0x0F12,0x4844, WORD_LEN, 0},	// 700041C2              
{0x0F12,0x4C44, WORD_LEN, 0},	// 700041C4              
{0x0F12,0x6E00, WORD_LEN, 0},	// 700041C6              
{0x0F12,0x60E0, WORD_LEN, 0},	// 700041C8              
{0x0F12,0x466B, WORD_LEN, 0},	// 700041CA              
{0x0F12,0x8818, WORD_LEN, 0},	// 700041CC              
{0x0F12,0x8859, WORD_LEN, 0},	// 700041CE              
{0x0F12,0x0025, WORD_LEN, 0},	// 700041D0              
{0x0F12,0x1A40, WORD_LEN, 0},	// 700041D2              
{0x0F12,0x3540, WORD_LEN, 0},	// 700041D4              
{0x0F12,0x61A8, WORD_LEN, 0},	// 700041D6              
{0x0F12,0x483B, WORD_LEN, 0},	// 700041D8              
{0x0F12,0x9900, WORD_LEN, 0},	// 700041DA              
{0x0F12,0x3060, WORD_LEN, 0},	// 700041DC              
{0x0F12,0xF000, WORD_LEN, 0},	// 700041DE              
{0x0F12,0xF947, WORD_LEN, 0},	// 700041E0              
{0x0F12,0x466B, WORD_LEN, 0},	// 700041E2              
{0x0F12,0x8819, WORD_LEN, 0},	// 700041E4              
{0x0F12,0x1DE0, WORD_LEN, 0},	// 700041E6              
{0x0F12,0x30F9, WORD_LEN, 0},	// 700041E8              
{0x0F12,0x8741, WORD_LEN, 0},	// 700041EA              
{0x0F12,0x8859, WORD_LEN, 0},	// 700041EC              
{0x0F12,0x8781, WORD_LEN, 0},	// 700041EE              
{0x0F12,0x2000, WORD_LEN, 0},	// 700041F0              
{0x0F12,0x71A0, WORD_LEN, 0},	// 700041F2              
{0x0F12,0x74A8, WORD_LEN, 0},	// 700041F4              
{0x0F12,0xBC7C, WORD_LEN, 0},	// 700041F6              
{0x0F12,0xBC08, WORD_LEN, 0},	// 700041F8              
{0x0F12,0x4718, WORD_LEN, 0},	// 700041FA              
{0x0F12,0xB5F8, WORD_LEN, 0},	// 700041FC              
{0x0F12,0x0005, WORD_LEN, 0},	// 700041FE              
{0x0F12,0x6808, WORD_LEN, 0},	// 70004200              
{0x0F12,0x0400, WORD_LEN, 0},	// 70004202              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70004204              
{0x0F12,0x684A, WORD_LEN, 0},	// 70004206              
{0x0F12,0x0412, WORD_LEN, 0},	// 70004208              
{0x0F12,0x0C12, WORD_LEN, 0},	// 7000420A              
{0x0F12,0x688E, WORD_LEN, 0},	// 7000420C              
{0x0F12,0x68CC, WORD_LEN, 0},	// 7000420E              
{0x0F12,0x492C, WORD_LEN, 0},	// 70004210              
{0x0F12,0x884B, WORD_LEN, 0},	// 70004212              
{0x0F12,0x4343, WORD_LEN, 0},	// 70004214              
{0x0F12,0x0A98, WORD_LEN, 0},	// 70004216              
{0x0F12,0x2304, WORD_LEN, 0},	// 70004218              
{0x0F12,0x5ECB, WORD_LEN, 0},	// 7000421A              
{0x0F12,0x18C0, WORD_LEN, 0},	// 7000421C              
{0x0F12,0x02C0, WORD_LEN, 0},	// 7000421E              
{0x0F12,0x0C00, WORD_LEN, 0},	// 70004220              
{0x0F12,0x88CB, WORD_LEN, 0},	// 70004222              
{0x0F12,0x4353, WORD_LEN, 0},	// 70004224              
{0x0F12,0x0A9A, WORD_LEN, 0},	// 70004226              
{0x0F12,0x2308, WORD_LEN, 0},	// 70004228              
{0x0F12,0x5ECB, WORD_LEN, 0},	// 7000422A              
{0x0F12,0x18D1, WORD_LEN, 0},	// 7000422C              
{0x0F12,0x02C9, WORD_LEN, 0},	// 7000422E              
{0x0F12,0x0C09, WORD_LEN, 0},	// 70004230              
{0x0F12,0x2701, WORD_LEN, 0},	// 70004232              
{0x0F12,0x003A, WORD_LEN, 0},	// 70004234              
{0x0F12,0x40AA, WORD_LEN, 0},	// 70004236              
{0x0F12,0x9200, WORD_LEN, 0},	// 70004238              
{0x0F12,0x002A, WORD_LEN, 0},	// 7000423A              
{0x0F12,0x3A10, WORD_LEN, 0},	// 7000423C              
{0x0F12,0x4097, WORD_LEN, 0},	// 7000423E              
{0x0F12,0x2D10, WORD_LEN, 0},	// 70004240              
{0x0F12,0xDA06, WORD_LEN, 0},	// 70004242              
{0x0F12,0x4A25, WORD_LEN, 0},	// 70004244              
{0x0F12,0x9B00, WORD_LEN, 0},	// 70004246              
{0x0F12,0x8812, WORD_LEN, 0},	// 70004248              
{0x0F12,0x439A, WORD_LEN, 0},	// 7000424A              
{0x0F12,0x4B23, WORD_LEN, 0},	// 7000424C              
{0x0F12,0x801A, WORD_LEN, 0},	// 7000424E              
{0x0F12,0xE003, WORD_LEN, 0},	// 70004250              
{0x0F12,0x4B22, WORD_LEN, 0},	// 70004252              
{0x0F12,0x885A, WORD_LEN, 0},	// 70004254              
{0x0F12,0x43BA, WORD_LEN, 0},	// 70004256              
{0x0F12,0x805A, WORD_LEN, 0},	// 70004258              
{0x0F12,0x0023, WORD_LEN, 0},	// 7000425A              
{0x0F12,0x0032, WORD_LEN, 0},	// 7000425C              
{0x0F12,0xF000, WORD_LEN, 0},	// 7000425E              
{0x0F12,0xF8EF, WORD_LEN, 0},	// 70004260              
{0x0F12,0x2D10, WORD_LEN, 0},	// 70004262              
{0x0F12,0xDA05, WORD_LEN, 0},	// 70004264              
{0x0F12,0x491D, WORD_LEN, 0},	// 70004266              
{0x0F12,0x9A00, WORD_LEN, 0},	// 70004268              
{0x0F12,0x8808, WORD_LEN, 0},	// 7000426A              
{0x0F12,0x4310, WORD_LEN, 0},	// 7000426C              
{0x0F12,0x8008, WORD_LEN, 0},	// 7000426E              
{0x0F12,0xE003, WORD_LEN, 0},	// 70004270              
{0x0F12,0x481A, WORD_LEN, 0},	// 70004272              
{0x0F12,0x8841, WORD_LEN, 0},	// 70004274              
{0x0F12,0x4339, WORD_LEN, 0},	// 70004276              
{0x0F12,0x8041, WORD_LEN, 0},	// 70004278              
{0x0F12,0x4D17, WORD_LEN, 0},	// 7000427A              
{0x0F12,0x2000, WORD_LEN, 0},	// 7000427C              
{0x0F12,0x3580, WORD_LEN, 0},	// 7000427E              
{0x0F12,0x88AA, WORD_LEN, 0},	// 70004280              
{0x0F12,0x5E30, WORD_LEN, 0},	// 70004282              
{0x0F12,0x2100, WORD_LEN, 0},	// 70004284              
{0x0F12,0xF000, WORD_LEN, 0},	// 70004286              
{0x0F12,0xF8FB, WORD_LEN, 0},	// 70004288              
{0x0F12,0x8030, WORD_LEN, 0},	// 7000428A              
{0x0F12,0x2000, WORD_LEN, 0},	// 7000428C              
{0x0F12,0x88AA, WORD_LEN, 0},	// 7000428E              
{0x0F12,0x5E20, WORD_LEN, 0},	// 70004290              
{0x0F12,0x2100, WORD_LEN, 0},	// 70004292              
{0x0F12,0xF000, WORD_LEN, 0},	// 70004294              
{0x0F12,0xF8F4, WORD_LEN, 0},	// 70004296              
{0x0F12,0x8020, WORD_LEN, 0},	// 70004298              
{0x0F12,0xE587, WORD_LEN, 0},	// 7000429A              
{0x0F12,0xB510, WORD_LEN, 0},	// 7000429C              
{0x0F12,0xF000, WORD_LEN, 0},	// 7000429E              
{0x0F12,0xF8F7, WORD_LEN, 0},	// 700042A0              
{0x0F12,0x4A0F, WORD_LEN, 0},	// 700042A2              
{0x0F12,0x8D50, WORD_LEN, 0},	// 700042A4              
{0x0F12,0x2800, WORD_LEN, 0},	// 700042A6              
{0x0F12,0xD007, WORD_LEN, 0},	// 700042A8              
{0x0F12,0x490A, WORD_LEN, 0},	// 700042AA              
{0x0F12,0x31C0, WORD_LEN, 0},	// 700042AC              
{0x0F12,0x684B, WORD_LEN, 0},	// 700042AE              
{0x0F12,0x490C, WORD_LEN, 0},	// 700042B0              
{0x0F12,0x4283, WORD_LEN, 0},	// 700042B2              
{0x0F12,0xD202, WORD_LEN, 0},	// 700042B4              
{0x0F12,0x8D90, WORD_LEN, 0},	// 700042B6              
{0x0F12,0x81C8, WORD_LEN, 0},	// 700042B8              
{0x0F12,0xE6A0, WORD_LEN, 0},	// 700042BA              
{0x0F12,0x8DD0, WORD_LEN, 0},	// 700042BC              
{0x0F12,0x81C8, WORD_LEN, 0},	// 700042BE              
{0x0F12,0xE69D, WORD_LEN, 0},	// 700042C0              
{0x0F12,0x0000, WORD_LEN, 0},	// 700042C2              
{0x0F12,0x2558, WORD_LEN, 0},	// 700042C4              
{0x0F12,0x7000, WORD_LEN, 0},	// 700042C6              
{0x0F12,0x2AB8, WORD_LEN, 0},	// 700042C8              
{0x0F12,0x7000, WORD_LEN, 0},	// 700042CA              
{0x0F12,0x145E, WORD_LEN, 0},	// 700042CC              
{0x0F12,0x7000, WORD_LEN, 0},	// 700042CE              
{0x0F12,0x2698, WORD_LEN, 0},	// 700042D0              
{0x0F12,0x7000, WORD_LEN, 0},	// 700042D2              
{0x0F12,0x2BB8, WORD_LEN, 0},	// 700042D4              
{0x0F12,0x7000, WORD_LEN, 0},	// 700042D6              
{0x0F12,0x2998, WORD_LEN, 0},	// 700042D8              
{0x0F12,0x7000, WORD_LEN, 0},	// 700042DA              
{0x0F12,0x1100, WORD_LEN, 0},	// 700042DC              
{0x0F12,0xD000, WORD_LEN, 0},	// 700042DE              
{0x0F12,0x4780, WORD_LEN, 0},	// 700042E0              
{0x0F12,0x7000, WORD_LEN, 0},	// 700042E2              
{0x0F12,0xE200, WORD_LEN, 0},	// 700042E4              
{0x0F12,0xD000, WORD_LEN, 0},	// 700042E6              
{0x0F12,0x4778, WORD_LEN, 0},	// 700042E8              
{0x0F12,0x46C0, WORD_LEN, 0},	// 700042EA              
{0x0F12,0xC000, WORD_LEN, 0},	// 700042EC              
{0x0F12,0xE59F, WORD_LEN, 0},	// 700042EE              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 700042F0              
{0x0F12,0xE12F, WORD_LEN, 0},	// 700042F2              
{0x0F12,0x1789, WORD_LEN, 0},	// 700042F4              
{0x0F12,0x0001, WORD_LEN, 0},	// 700042F6              
{0x0F12,0x4778, WORD_LEN, 0},	// 700042F8              
{0x0F12,0x46C0, WORD_LEN, 0},	// 700042FA              
{0x0F12,0xC000, WORD_LEN, 0},	// 700042FC              
{0x0F12,0xE59F, WORD_LEN, 0},	// 700042FE              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004300              
{0x0F12,0xE12F, WORD_LEN, 0},	// 70004302              
{0x0F12,0x16F1, WORD_LEN, 0},	// 70004304              
{0x0F12,0x0001, WORD_LEN, 0},	// 70004306              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004308              
{0x0F12,0x46C0, WORD_LEN, 0},	// 7000430A              
{0x0F12,0xC000, WORD_LEN, 0},	// 7000430C              
{0x0F12,0xE59F, WORD_LEN, 0},	// 7000430E              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004310              
{0x0F12,0xE12F, WORD_LEN, 0},	// 70004312              
{0x0F12,0xC3B1, WORD_LEN, 0},	// 70004314              
{0x0F12,0x0000, WORD_LEN, 0},	// 70004316              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004318              
{0x0F12,0x46C0, WORD_LEN, 0},	// 7000431A              
{0x0F12,0xC000, WORD_LEN, 0},	// 7000431C              
{0x0F12,0xE59F, WORD_LEN, 0},	// 7000431E              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004320              
{0x0F12,0xE12F, WORD_LEN, 0},	// 70004322              
{0x0F12,0xC36D, WORD_LEN, 0},	// 70004324              
{0x0F12,0x0000, WORD_LEN, 0},	// 70004326              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004328              
{0x0F12,0x46C0, WORD_LEN, 0},	// 7000432A              
{0x0F12,0xC000, WORD_LEN, 0},	// 7000432C              
{0x0F12,0xE59F, WORD_LEN, 0},	// 7000432E              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004330              
{0x0F12,0xE12F, WORD_LEN, 0},	// 70004332              
{0x0F12,0xF6D7, WORD_LEN, 0},	// 70004334              
{0x0F12,0x0000, WORD_LEN, 0},	// 70004336              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004338              
{0x0F12,0x46C0, WORD_LEN, 0},	// 7000433A              
{0x0F12,0xC000, WORD_LEN, 0},	// 7000433C              
{0x0F12,0xE59F, WORD_LEN, 0},	// 7000433E              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004340              
{0x0F12,0xE12F, WORD_LEN, 0},	// 70004342              
{0x0F12,0xB49D, WORD_LEN, 0},	// 70004344              
{0x0F12,0x0000, WORD_LEN, 0},	// 70004346              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004348              
{0x0F12,0x46C0, WORD_LEN, 0},	// 7000434A              
{0x0F12,0xC000, WORD_LEN, 0},	// 7000434C              
{0x0F12,0xE59F, WORD_LEN, 0},	// 7000434E              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004350              
{0x0F12,0xE12F, WORD_LEN, 0},	// 70004352              
{0x0F12,0x7EDF, WORD_LEN, 0},	// 70004354              
{0x0F12,0x0000, WORD_LEN, 0},	// 70004356              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004358              
{0x0F12,0x46C0, WORD_LEN, 0},	// 7000435A              
{0x0F12,0xC000, WORD_LEN, 0},	// 7000435C              
{0x0F12,0xE59F, WORD_LEN, 0},	// 7000435E              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004360              
{0x0F12,0xE12F, WORD_LEN, 0},	// 70004362              
{0x0F12,0x448D, WORD_LEN, 0},	// 70004364              
{0x0F12,0x0000, WORD_LEN, 0},	// 70004366              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004368              
{0x0F12,0x46C0, WORD_LEN, 0},	// 7000436A              
{0x0F12,0xF004, WORD_LEN, 0},	// 7000436C              
{0x0F12,0xE51F, WORD_LEN, 0},	// 7000436E              
{0x0F12,0x29EC, WORD_LEN, 0},	// 70004370              
{0x0F12,0x0001, WORD_LEN, 0},	// 70004372              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004374              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004376              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004378              
{0x0F12,0xE59F, WORD_LEN, 0},	// 7000437A              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 7000437C              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000437E              
{0x0F12,0x2EF1, WORD_LEN, 0},	// 70004380              
{0x0F12,0x0000, WORD_LEN, 0},	// 70004382              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004384              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004386              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004388              
{0x0F12,0xE59F, WORD_LEN, 0},	// 7000438A              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 7000438C              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000438E              
{0x0F12,0xEE03, WORD_LEN, 0},	// 70004390              
{0x0F12,0x0000, WORD_LEN, 0},	// 70004392              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004394              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004396              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004398              
{0x0F12,0xE59F, WORD_LEN, 0},	// 7000439A              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 7000439C              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000439E              
{0x0F12,0xA58B, WORD_LEN, 0},	// 700043A0              
{0x0F12,0x0000, WORD_LEN, 0},	// 700043A2              
{0x0F12,0x4778, WORD_LEN, 0},	// 700043A4              
{0x0F12,0x46C0, WORD_LEN, 0},	// 700043A6              
{0x0F12,0xC000, WORD_LEN, 0},	// 700043A8              
{0x0F12,0xE59F, WORD_LEN, 0},	// 700043AA              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 700043AC              
{0x0F12,0xE12F, WORD_LEN, 0},	// 700043AE              
{0x0F12,0x7C49, WORD_LEN, 0},	// 700043B0              
{0x0F12,0x0000, WORD_LEN, 0},	// 700043B2              
{0x0F12,0x4778, WORD_LEN, 0},	// 700043B4              
{0x0F12,0x46C0, WORD_LEN, 0},	// 700043B6              
{0x0F12,0xC000, WORD_LEN, 0},	// 700043B8              
{0x0F12,0xE59F, WORD_LEN, 0},	// 700043BA              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 700043BC              
{0x0F12,0xE12F, WORD_LEN, 0},	// 700043BE              
{0x0F12,0x7C63, WORD_LEN, 0},	// 700043C0              
{0x0F12,0x0000, WORD_LEN, 0},	// 700043C2              
{0x0F12,0x4778, WORD_LEN, 0},	// 700043C4              
{0x0F12,0x46C0, WORD_LEN, 0},	// 700043C6              
{0x0F12,0xC000, WORD_LEN, 0},	// 700043C8              
{0x0F12,0xE59F, WORD_LEN, 0},	// 700043CA              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 700043CC              
{0x0F12,0xE12F, WORD_LEN, 0},	// 700043CE              
{0x0F12,0x2DB7, WORD_LEN, 0},	// 700043D0              
{0x0F12,0x0000, WORD_LEN, 0},	// 700043D2              
{0x0F12,0x4778, WORD_LEN, 0},	// 700043D4              
{0x0F12,0x46C0, WORD_LEN, 0},	// 700043D6              
{0x0F12,0xC000, WORD_LEN, 0},	// 700043D8              
{0x0F12,0xE59F, WORD_LEN, 0},	// 700043DA              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 700043DC              
{0x0F12,0xE12F, WORD_LEN, 0},	// 700043DE              
{0x0F12,0xEB3D, WORD_LEN, 0},	// 700043E0              
{0x0F12,0x0000, WORD_LEN, 0},	// 700043E2              
{0x0F12,0x4778, WORD_LEN, 0},	// 700043E4              
{0x0F12,0x46C0, WORD_LEN, 0},	// 700043E6              
{0x0F12,0xC000, WORD_LEN, 0},	// 700043E8              
{0x0F12,0xE59F, WORD_LEN, 0},	// 700043EA              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 700043EC              
{0x0F12,0xE12F, WORD_LEN, 0},	// 700043EE              
{0x0F12,0xF061, WORD_LEN, 0},	// 700043F0              
{0x0F12,0x0000, WORD_LEN, 0},	// 700043F2              
{0x0F12,0x4778, WORD_LEN, 0},	// 700043F4              
{0x0F12,0x46C0, WORD_LEN, 0},	// 700043F6              
{0x0F12,0xC000, WORD_LEN, 0},	// 700043F8              
{0x0F12,0xE59F, WORD_LEN, 0},	// 700043FA              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 700043FC              
{0x0F12,0xE12F, WORD_LEN, 0},	// 700043FE              
{0x0F12,0xF0EF, WORD_LEN, 0},	// 70004400              
{0x0F12,0x0000, WORD_LEN, 0},	// 70004402              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004404              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004406              
{0x0F12,0xF004, WORD_LEN, 0},	// 70004408              
{0x0F12,0xE51F, WORD_LEN, 0},	// 7000440A              
{0x0F12,0x2824, WORD_LEN, 0},	// 7000440C              
{0x0F12,0x0001, WORD_LEN, 0},	// 7000440E              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004410              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004412              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004414              
{0x0F12,0xE59F, WORD_LEN, 0},	// 70004416              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004418              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000441A              
{0x0F12,0x8EDD, WORD_LEN, 0},	// 7000441C              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000441E              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004420              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004422              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004424              
{0x0F12,0xE59F, WORD_LEN, 0},	// 70004426              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004428              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000442A              
{0x0F12,0x8DCB, WORD_LEN, 0},	// 7000442C              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000442E              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004430              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004432              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004434              
{0x0F12,0xE59F, WORD_LEN, 0},	// 70004436              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004438              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000443A              
{0x0F12,0x8E17, WORD_LEN, 0},	// 7000443C              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000443E              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004440              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004442              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004444              
{0x0F12,0xE59F, WORD_LEN, 0},	// 70004446              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004448              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000444A              
{0x0F12,0x98C5, WORD_LEN, 0},	// 7000444C              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000444E              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004450              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004452              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004454              
{0x0F12,0xE59F, WORD_LEN, 0},	// 70004456              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004458              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000445A              
{0x0F12,0x7C7D, WORD_LEN, 0},	// 7000445C              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000445E              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004460              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004462              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004464              
{0x0F12,0xE59F, WORD_LEN, 0},	// 70004466              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004468              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000446A              
{0x0F12,0x7E31, WORD_LEN, 0},	// 7000446C              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000446E              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004470              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004472              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004474              
{0x0F12,0xE59F, WORD_LEN, 0},	// 70004476              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004478              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000447A              
{0x0F12,0x7EAB, WORD_LEN, 0},	// 7000447C              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000447E              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004480              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004482              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004484              
{0x0F12,0xE59F, WORD_LEN, 0},	// 70004486              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004488              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000448A              
{0x0F12,0x7501, WORD_LEN, 0},	// 7000448C              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000448E              
{0x0F12,0x4778, WORD_LEN, 0},	// 70004490              
{0x0F12,0x46C0, WORD_LEN, 0},	// 70004492              
{0x0F12,0xC000, WORD_LEN, 0},	// 70004494              
{0x0F12,0xE59F, WORD_LEN, 0},	// 70004496              
{0x0F12,0xFF1C, WORD_LEN, 0},	// 70004498              
{0x0F12,0xE12F, WORD_LEN, 0},	// 7000449A              
{0x0F12,0xF63F, WORD_LEN, 0},	// 7000449C              
{0x0F12,0x0000, WORD_LEN, 0},	// 7000449E      

// End of Patch Data(Last : 7000449Eh)
// Total Size 2472 (0x09A8)           
// Addr : 3AF8 , Size : 2470(9A6h)    

{0x0028,0xD000, WORD_LEN, 0},
{0x002A,0x1000, WORD_LEN, 0},
{0x0F12,0x0001, WORD_LEN, 0},

//==================================================================================
// 06.AF Setting
//==================================================================================

{0x0028,0x7000, WORD_LEN, 0},                                                                  
{0x002A,0x01FC, WORD_LEN, 0},                                                                  
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_IPRM_LedGpio                                      
{0x002A,0x01FE, WORD_LEN, 0},                                                                  
{0x0F12,0x0003, WORD_LEN, 0},	//REG_TC_IPRM_CM_Init_AfModeType VCM IIC                   
{0x0F12,0x0000, WORD_LEN, 0},	//REG_TC_IPRM_CM_Init_PwmConfig1                           
{0x002A,0x0204, WORD_LEN, 0},                                                                  
{0x0F12,0x0061, WORD_LEN, 0},	//REG_TC_IPRM_CM_Init_GpioConfig1 AF Enable GPIO 6	
{0x002A,0x020C, WORD_LEN, 0},                                                                  
{0x0F12,0x2F0C, WORD_LEN, 0},	//REG_TC_IPRM_CM_Init_Mi2cBits                             
{0x0F12,0x0190, WORD_LEN, 0},	//REG_TC_IPRM_CM_Init_Mi2cRateKhz IIC Speed                
{0x002A,0x0294, WORD_LEN, 0},                                                                  
{0x0F12,0x0100, WORD_LEN, 0},	//REG_TC_AF_FstWinStartX                                   
{0x0F12,0x00E3, WORD_LEN, 0},	//REG_TC_AF_FstWinStartY                                   
{0x0F12,0x0200, WORD_LEN, 0},	//REG_TC_AF_FstWinSizeX                                    
{0x0F12,0x0238, WORD_LEN, 0},	//REG_TC_AF_FstWinSizeY                                    
{0x0F12,0x01C6, WORD_LEN, 0},	//REG_TC_AF_ScndWinStartX                                  
{0x0F12,0x0166, WORD_LEN, 0},	//REG_TC_AF_ScndWinStartY                                  
{0x0F12,0x0074, WORD_LEN, 0},	//REG_TC_AF_ScndWinSizeX                                   
{0x0F12,0x0132, WORD_LEN, 0},	//REG_TC_AF_ScndWinSizeY                                   
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_AF_WinSizesUpdated                                
                                                                   
{0x002A,0x070E, WORD_LEN, 0},                                                                  
{0x0F12,0x00C0, WORD_LEN, 0},	//skl_af_StatOvlpExpFactor                                 
{0x002A,0x071E, WORD_LEN, 0},                                                                  
{0x0F12,0x0000, WORD_LEN, 0},	//skl_af_bAfStatOff                                        
{0x002A,0x163C, WORD_LEN, 0},                                                                  
{0x0F12,0x0000, WORD_LEN, 0},	//af_search_usAeStable                                     
{0x002A,0x1648, WORD_LEN, 0},                                                                  
{0x0F12,0x9002, WORD_LEN, 0},	//af_search_usSingleAfFlags                                
{0x002A,0x1652, WORD_LEN, 0},                                                                  
{0x0F12,0x0002, WORD_LEN, 0},	//af_search_usFinePeakCount                                
{0x0F12,0x0000, WORD_LEN, 0},	//af_search_usFineMaxScale                                 
{0x002A,0x15E0, WORD_LEN, 0},                                                                  
{0x0F12,0x0902, WORD_LEN, 0},	//af_pos_usFineStepNumSize                                 
                                                                   
{0x002A,0x164C, WORD_LEN, 0},                                                                  
{0x0F12,0x0003, WORD_LEN, 0},	//af_search_usMinPeakSamples                               
{0x002A,0x163E, WORD_LEN, 0},                                                                  
{0x0F12,0x00A0, WORD_LEN, 0},	//af_search_usPeakThr Full search (E5 90%)                 
{0x0F12,0x0098, WORD_LEN, 0},	//af_search_usPeakThrLow                                   
{0x002A,0x15D4, WORD_LEN, 0},                                                                  
{0x0F12,0x0000, WORD_LEN, 0},	//af_pos_usHomePos                                         
{0x0F12,0xD000, WORD_LEN, 0},	//af_pos_usLowConfPos                                      
{0x002A,0x169A, WORD_LEN, 0},                                                                  
{0x0F12,0xFF95, WORD_LEN, 0},	//af_search_usConfCheckOrder_1_                            
{0x002A,0x166A, WORD_LEN, 0},                                                                  
{0x0F12,0x0280, WORD_LEN, 0},	//af_search_usConfThr_4_                                   
{0x002A,0x1676, WORD_LEN, 0},	                                                           
{0x0F12,0x03A0, WORD_LEN, 0},	//af_search_usConfThr_10_                                  
{0x0F12,0x0320, WORD_LEN, 0},	//af_search_usConfThr_11_                                  
{0x002A,0x16BC, WORD_LEN, 0},	                                                           
{0x0F12,0x0030, WORD_LEN, 0},	//af_stat_usMinStatVal                                     
{0x002A,0x16E0, WORD_LEN, 0},	                                                           
{0x0F12,0x0060, WORD_LEN, 0},	//af_scene_usSceneLowNormBrThr                             
{0x002A,0x16D4, WORD_LEN, 0},	                                                           
{0x0F12,0x0010, WORD_LEN, 0},	//af_stat_usBpfThresh                                      
{0x002A,0x1656, WORD_LEN, 0},	                                                           
{0x0F12,0x0000, WORD_LEN, 0},	//af_search_usCapturePolicy                                
{0x002A,0x15E6, WORD_LEN, 0},                                                                  
{0x0F12,0x003C, WORD_LEN, 0},	//af_pos_usCaptureFixedPos                                 
                                                                   
{0x0F12,0x0018, WORD_LEN, 0},	//af_pos_usTableLastInd                                    
{0x0F12,0x002A, WORD_LEN, 0},	//af_pos_usTable_0_                                        
{0x0F12,0x0030, WORD_LEN, 0},	//af_pos_usTable_1_                                        
{0x0F12,0x0036, WORD_LEN, 0},	//af_pos_usTable_2_                                        
{0x0F12,0x003C, WORD_LEN, 0},	//af_pos_usTable_3_                                        
{0x0F12,0x0042, WORD_LEN, 0},	//af_pos_usTable_4_                                        
{0x0F12,0x0048, WORD_LEN, 0},	//af_pos_usTable_5_                                        
{0x0F12,0x004E, WORD_LEN, 0},	//af_pos_usTable_6_                                        
{0x0F12,0x0054, WORD_LEN, 0},	//af_pos_usTable_7_                                        
{0x0F12,0x005A, WORD_LEN, 0},	//af_pos_usTable_8_                                        
{0x0F12,0x0060, WORD_LEN, 0},	//af_pos_usTable_9_                                        
{0x0F12,0x0066, WORD_LEN, 0},	//af_pos_usTable_10                                        
{0x0F12,0x006C, WORD_LEN, 0},	//af_pos_usTable_11_                                       
{0x0F12,0x0072, WORD_LEN, 0},	//af_pos_usTable_12_                                       
{0x0F12,0x0078, WORD_LEN, 0},	//af_pos_usTable_13_                                       
{0x0F12,0x007E, WORD_LEN, 0},	//af_pos_usTable_14_                                       
{0x0F12,0x0084, WORD_LEN, 0},	//af_pos_usTable_15_                                       
{0x0F12,0x008A, WORD_LEN, 0},	//af_pos_usTable_16_                                       
{0x0F12,0x0090, WORD_LEN, 0},	//af_pos_usTable_17_                                       
{0x0F12,0x0096, WORD_LEN, 0},	//af_pos_usTable_18_                                       
{0x0F12,0x009C, WORD_LEN, 0},	//af_pos_usTable_19_                                       
{0x0F12,0x00A2, WORD_LEN, 0},	//af_pos_usTable_20_                                       
{0x0F12,0x00A8, WORD_LEN, 0},	//af_pos_usTable_21_                                       
{0x0F12,0x00AE, WORD_LEN, 0},	//af_pos_usTable_22_                                       
{0x0F12,0x00B4, WORD_LEN, 0},	//af_pos_usTable_23_                                       
{0x0F12,0x00BA, WORD_LEN, 0},	//af_pos_usTable_24_  
                                                                 
{0x002A,0x1722, WORD_LEN, 0},                                                                  
{0x0F12,0x8000, WORD_LEN, 0},	//afd_usParam_0_                                           
{0x0F12,0x0006, WORD_LEN, 0},	//afd_usParam_1_                                           
{0x0F12,0x3FF0, WORD_LEN, 0},	//afd_usParam_2_                                           
{0x0F12,0x03E8, WORD_LEN, 0},	//afd_usParam_3_                                           
{0x0F12,0x0000, WORD_LEN, 0},	//afd_usParam_4_                                           
{0x0F12,0x0018, WORD_LEN, 0},	//afd_usParam_5_                                           
{0x0F12,0x0010, WORD_LEN, 0},	//afd_usParam_6_                                           
{0x0F12,0x0010, WORD_LEN, 0},	//afd_usParam_7_                                           
{0x0F12,0x0040, WORD_LEN, 0},	//afd_usParam_8_                                           
{0x0F12,0x0080, WORD_LEN, 0},	//afd_usParam_9_                                           
{0x0F12,0x00C0, WORD_LEN, 0},	//afd_usParam_10_                                          
{0x0F12,0x00E0, WORD_LEN, 0},	//afd_usParam_11_                                          
                                                                   
{0x002A,0x028C, WORD_LEN, 0},                                                                  
{0x0F12,0x0003, WORD_LEN, 0},	//REG_TC_AF_AfCmd                                          

//==================================================================================
// 07.Clock Setting
//==================================================================================

//Input Clock (Mclk)	
{0x002A,0x01F8, WORD_LEN, 0},	
{0x0F12,0x5DC0, WORD_LEN, 0},	//REG_TC_IPRM_InClockLSBs
{0x002A,0x0212, WORD_LEN, 0},	                                     
{0x0F12,0x0000, WORD_LEN, 0},	//REG_TC_IPRM_UseNPviClocks          
{0x0F12,0x0002, WORD_LEN, 0},	//REG_TC_IPRM_UseNMipiClocks         
{0x0F12,0x0002, WORD_LEN, 0},	//REG_TC_IPRM_NumberOfMipiLanes

//System Clock & Output clock (Pclk)		
{0x002A,0x021A, WORD_LEN, 0},	
{0x0F12,0x34BC, WORD_LEN, 0},	//34BC REG_TC_IPRM_OpClk4KHz_0 
{0x0F12,0x4F1A, WORD_LEN, 0}, //4F1A	//REG_TC_IPRM_MinOutRate4KHz_0
{0x0F12,0x4F1A, WORD_LEN, 0}, //4F1A	//REG_TC_IPRM_MaxOutRate4KHz_0	

{0x002A,0x022C, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_IPRM_InitParamsUpdated

//==================================================================================
// 08.Input Size Setting
//==================================================================================
//Input Size	
{0x002A,0x0250, WORD_LEN, 0},                            	
{0x0F12,0x0A00, WORD_LEN, 0},	//REG_TC_GP_PrevReqInputWidth
{0x0F12,0x0780, WORD_LEN, 0},	//REG_TC_GP_PrevReqInputHeight
{0x0F12,0x0010, WORD_LEN, 0},	//REG_TC_GP_PrevInputWidthOfs
{0x0F12,0x000c, WORD_LEN, 0},	//REG_TC_GP_PrevInputHeightOfs
{0x0F12,0x0A00, WORD_LEN, 0},	//REG_TC_GP_CapReqInputWidth
{0x0F12,0x0780, WORD_LEN, 0},	//REG_TC_GP_CapReqInputHeight
{0x0F12,0x0010, WORD_LEN, 0},	//REG_TC_GP_CapInputWidthOfs
{0x0F12,0x000c, WORD_LEN, 0},	//REG_TC_GP_CapInputHeightOfs

{0x002A,0x0494, WORD_LEN, 0}, 
{0x0F12,0x0A00, WORD_LEN, 0},	//REG_TC_PZOOM_ZoomInputWidth    
{0x0F12,0x0780, WORD_LEN, 0},	//REG_TC_PZOOM_ZoomInputHeight   
{0x0F12,0x0000, WORD_LEN, 0},	//REG_TC_PZOOM_ZoomInputWidthOfs 
{0x0F12,0x0000, WORD_LEN, 0},	//REG_TC_PZOOM_ZoomInputHeightOfs
{0x0F12,0x0A00, WORD_LEN, 0},	//REG_TC_CZOOM_ZoomInputWidth    
{0x0F12,0x0780, WORD_LEN, 0},	//REG_TC_CZOOM_ZoomInputHeight   
{0x0F12,0x0000, WORD_LEN, 0},	//REG_TC_CZOOM_ZoomInputWidthOfs 
{0x0F12,0x0000, WORD_LEN, 0},	//REG_TC_CZOOM_ZoomInputHeightOfs

{0x002A,0x0262, WORD_LEN, 0},                                
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_GP_bUseReqInputInPre  
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_GP_bUseReqInputInCap  

// ztlee Add test i2c
{0xFFFF,0x0064, WORD_LEN, 150},

//==================================================================================
// 09.Preview & Capture Configration Setting
//==================================================================================
//Preview config[0] 640x480  7.5~15fps	
{0x002A,0x02A6, WORD_LEN, 0},	
{0x0F12,0x0500, WORD_LEN, 0},	//280 REG_0TC_PCFG_usWidth              	
{0x0F12,0x03C0, WORD_LEN, 0},	//1E0 REG_0TC_PCFG_usHeight             	
{0x0F12,0x0005, WORD_LEN, 0},	//REG_0TC_PCFG_Format	                
{0x0F12,0x4F1A, WORD_LEN, 0},	//REG_0TC_PCFG_usMaxOut4KHzRate     	
{0x0F12,0x4F1A, WORD_LEN, 0},	//REG_0TC_PCFG_usMinOut4KHzRate     	
{0x0F12,0x0100, WORD_LEN, 0},	//REG_0TC_PCFG_OutClkPerPix88       	
{0x0F12,0x0300, WORD_LEN, 0},	//REG_0TC_PCFG_uBpp88               	
{0x0F12,0x0002, WORD_LEN, 0},	//REG_0TC_PCFG_PVIMask              	
{0x0F12,0x0000, WORD_LEN, 0},	//REG_0TC_PCFG_OIFMask              	
{0x0F12,0x01E0, WORD_LEN, 0},	//REG_0TC_PCFG_usJpegPacketSize     	
{0x0F12,0x0000, WORD_LEN, 0},	//REG_0TC_PCFG_usJpegTotalPackets   	
{0x0F12,0x0000, WORD_LEN, 0},	//REG_0TC_PCFG_uClockInd            	
{0x0F12,0x0000, WORD_LEN, 0},	//REG_0TC_PCFG_usFrTimeType         	
{0x0F12,0x0001, WORD_LEN, 0},	//REG_0TC_PCFG_FrRateQualityType    	
{0x0F12,0x0535, WORD_LEN, 0},	//REG_0TC_PCFG_usMaxFrTimeMsecMult10	
//{0x0F12,0x014D, WORD_LEN, 0},	//29A REG_0TC_PCFG_usMinFrTimeMsecMult10	
{0x0F12,0x01F4, WORD_LEN, 0},	//29A REG_0TC_PCFG_usMinFrTimeMsecMult10	
{0x002A,0x02D0, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	//REG_0TC_PCFG_uPrevMirror
{0x0F12,0x0000, WORD_LEN, 0},	//REG_0TC_PCFG_uCaptureMirror

//Capture Config[0] 2560x1920   7.5~15fps                           	
{0x002A,0x0396, WORD_LEN, 0},                                     	
{0x0F12,0x0000, WORD_LEN, 0},	//00 REG_0TC_CCFG_uCaptureMode         
{0x0F12,0x0A00, WORD_LEN, 0},	//REG_0TC_CCFG_usWidth              
{0x0F12,0x0780, WORD_LEN, 0},	//REG_0TC_CCFG_usHeight             
{0x0F12,0x0005, WORD_LEN, 0},	//REG_0TC_CCFG_Format               
{0x0F12,0x4F1A, WORD_LEN, 0},	//REG_0TC_CCFG_usMaxOut4KHzRate     
{0x0F12,0x4F1A, WORD_LEN, 0},	//REG_0TC_CCFG_usMinOut4KHzRate     
{0x0F12,0x0100, WORD_LEN, 0},	//REG_0TC_CCFG_OutClkPerPix88       
{0x0F12,0x0300, WORD_LEN, 0},	//REG_0TC_CCFG_uBpp88               
{0x0F12,0x0002, WORD_LEN, 0},	//REG_0TC_CCFG_PVIMask              
{0x0F12,0x0000, WORD_LEN, 0},	//REG_0TC_CCFG_OIFMask              
{0x0F12,0x01E0, WORD_LEN, 0},	//REG_0TC_CCFG_usJpegPacketSize     
{0x0F12,0x0000, WORD_LEN, 0},	//REG_0TC_CCFG_usJpegTotalPackets   
{0x0F12,0x0000, WORD_LEN, 0},	//REG_0TC_CCFG_uClockInd            
{0x0F12,0x0002, WORD_LEN, 0},	//0 REG_0TC_CCFG_usFrTimeType         
{0x0F12,0x0002, WORD_LEN, 0},	//REG_0TC_CCFG_FrRateQualityType    
{0x0F12,0x0535, WORD_LEN, 0},	//REG_0TC_CCFG_usMaxFrTimeMsecMult10
{0x0F12,0x0000, WORD_LEN, 0},	//29A REG_0TC_CCFG_usMinFrTimeMsecMult10

{0xFFFF,0x0064, WORD_LEN, 100}, //p100

//==================================================================================
// 10.Auto Flicker Detection
//==================================================================================

{0x002A,0x0F30, WORD_LEN, 0}, 	
{0x0F12,0x0001, WORD_LEN, 0},   	//AFC_D_ConvAccelerPower 

// Auto Flicker (60Mhz start)	
{0x002A,0x0F2A, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	//AFC_Default BIT[0] 1:60Hz 0:50Hz
{0x002A,0x04E6, WORD_LEN, 0},	                                      
{0x0F12,0x077F, WORD_LEN, 0},	//REG_TC_DBG 7F: 60Hz  5F:50Hz

//==================================================================================
// 11.AE Setting
//==================================================================================

//AE Target	
{0x002A,0x1484, WORD_LEN, 0},	
{0x0F12,0x003C, WORD_LEN, 0},   	//TVAR_ae_BrAve

//ae_StatMode bit[3] BLC has to be bypassed to prevent AE weight change especially backlight scene 
{0x002A,0x148A, WORD_LEN, 0},	
{0x0F12,0x000F, WORD_LEN, 0},  	 //ae_StatMode

{0x002A,0x0588, WORD_LEN, 0},
{0x0F12,0x0000, WORD_LEN, 0},	//lt_uInitPostToleranceCnt

//AE_state                	
{0x002A,0x0544, WORD_LEN, 0},	
{0x0F12,0x0111, WORD_LEN, 0},  	 //lt_uLimitHigh
{0x0F12,0x00EF, WORD_LEN, 0},  	 //lt_uLimitLow

//AE Concept	
{0x002A,0x0608, WORD_LEN, 0},                                	
{0x0F12,0x0001, WORD_LEN, 0}, 	//lt_ExpGain_uSubsamplingmode
{0x0F12,0x0001, WORD_LEN, 0},	//lt_ExpGain_uNonSubsampling

//Exposure	
{0x002a,0x0610, WORD_LEN, 0},	//lt_ExpGain_ExpCurveGainMaxStr_0__ulExpIn_0
{0x0F12,0x0001, WORD_LEN, 0},
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0A3C, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0D05, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x4008, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x7000, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x9C00, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0xAD00, WORD_LEN, 0},	
{0x0F12,0x0001, WORD_LEN, 0},	
{0x0F12,0xF1D4, WORD_LEN, 0},	
{0x0F12,0x0002, WORD_LEN, 0},	
{0x0F12,0xDC00, WORD_LEN, 0},	
{0x0F12,0x0005, WORD_LEN, 0},	
{0x0F12,0xDC00, WORD_LEN, 0},	
{0x0F12,0x0005, WORD_LEN, 0},	

{0x002A,0x0638, WORD_LEN, 0},	//lt_ExpGain_ExpCurveGainMaxStr_0__ulExpOut_0_
{0x0F12,0x0001, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0A3C, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x0D05, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x3408, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x3408, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x6810, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0x8214, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0xC350, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0xC350, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	
{0x0F12,0xC350, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	

//Gain
{0x002A,0x060C, WORD_LEN, 0},	
{0x0F12,0x0540, WORD_LEN, 0},	//800 //lt_ExpGain_ExpCurveGainMaxStr
{0x0F12,0x0100, WORD_LEN, 0},	//lt_ExpGain_ExpCurveGainMaxStr_0__uMaxDigGain
{0x002A,0x05a2, WORD_LEN, 0},		
{0x0F12,0x1000, WORD_LEN, 0},	//lt_uMaxTotGain

// Lei Control	
{0x002A,0x06B8, WORD_LEN, 0},               	
{0x0F12,0x452C, WORD_LEN, 0},               	
{0x0F12,0x0005, WORD_LEN, 0},  	 //lt_uMaxLei

//==================================================================================
// 12.AE Weight (Normal)
//==================================================================================

{0x002A,0x1492, WORD_LEN, 0},	
{0x0F12,0x0100, WORD_LEN, 0},	//ae_WeightTbl_16[0]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[1]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[2]
{0x0F12,0x0001, WORD_LEN, 0},	//ae_WeightTbl_16[3]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[4]
{0x0F12,0x0201, WORD_LEN, 0},	//ae_WeightTbl_16[5]
{0x0F12,0x0102, WORD_LEN, 0},	//ae_WeightTbl_16[6]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[7]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[8]
{0x0F12,0x0202, WORD_LEN, 0},	//ae_WeightTbl_16[9]
{0x0F12,0x0202, WORD_LEN, 0},	//ae_WeightTbl_16[10]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[11]
{0x0F12,0x0201, WORD_LEN, 0},	//ae_WeightTbl_16[12]
{0x0F12,0x0302, WORD_LEN, 0},	//ae_WeightTbl_16[13]
{0x0F12,0x0203, WORD_LEN, 0},	//ae_WeightTbl_16[14]
{0x0F12,0x0102, WORD_LEN, 0},	//ae_WeightTbl_16[15]
{0x0F12,0x0201, WORD_LEN, 0},	//ae_WeightTbl_16[16]
{0x0F12,0x0302, WORD_LEN, 0},	//ae_WeightTbl_16[17]
{0x0F12,0x0203, WORD_LEN, 0},	//ae_WeightTbl_16[18]
{0x0F12,0x0102, WORD_LEN, 0},	//ae_WeightTbl_16[19]
{0x0F12,0x0201, WORD_LEN, 0},	//ae_WeightTbl_16[20]
{0x0F12,0x0202, WORD_LEN, 0},	//ae_WeightTbl_16[21]
{0x0F12,0x0202, WORD_LEN, 0},	//ae_WeightTbl_16[22]
{0x0F12,0x0102, WORD_LEN, 0},	//ae_WeightTbl_16[23]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[24]
{0x0F12,0x0202, WORD_LEN, 0},	//ae_WeightTbl_16[25]
{0x0F12,0x0202, WORD_LEN, 0},	//ae_WeightTbl_16[26]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[27]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[28]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[29]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[30]
{0x0F12,0x0101, WORD_LEN, 0},	//ae_WeightTbl_16[31]

//==================================================================================
// 13.Flash Setting
//==================================================================================

//{0x002A,0x0484, WORD_LEN, 0},	
//{0x0F12,0x0002, WORD_LEN, 0},	//capture flash on                         
//{0x002A,0x183A, WORD_LEN, 0},	                                           
//{0x0F12,0x0001, WORD_LEN, 0},	//one frame AE                             
//{0x002A,0x17F6, WORD_LEN, 0},	                                           
//{0x0F12,0x023C, WORD_LEN, 0},	//AWB R point                              
//{0x0F12,0x0248, WORD_LEN, 0},	//AWB B point                              
//{0x002A,0x1840, WORD_LEN, 0},	                                           
//{0x0F12,0x0001, WORD_LEN, 0},	// Fls AE tune start                       
//{0x0F12,0x0100, WORD_LEN, 0},	// fls_afl_FlsAFIn  Rin                    
//{0x0F12,0x0120, WORD_LEN, 0},	                                           
//{0x0F12,0x0180, WORD_LEN, 0},	                                           
//{0x0F12,0x0200, WORD_LEN, 0},	                                           
//{0x0F12,0x0400, WORD_LEN, 0},	                                           
//{0x0F12,0x0800, WORD_LEN, 0},	                                           
//{0x0F12,0x0A00, WORD_LEN, 0},	                                           
//{0x0F12,0x1000, WORD_LEN, 0},	                                           
//{0x0F12,0x0100, WORD_LEN, 0},	// fls_afl_FlsAFOut  Rout                  
//{0x0F12,0x00A0, WORD_LEN, 0},	                                           
//{0x0F12,0x0090, WORD_LEN, 0},	                                           
//{0x0F12,0x0080, WORD_LEN, 0},	                                           
//{0x0F12,0x0070, WORD_LEN, 0},	                                           
//{0x0F12,0x0045, WORD_LEN, 0},	                                           
//{0x0F12,0x0030, WORD_LEN, 0},	                                           
//{0x0F12,0x0010, WORD_LEN, 0},	                                           
//{0x002A,0x1884, WORD_LEN, 0},	                                           
//{0x0F12,0x0100, WORD_LEN, 0},	// fls_afl_FlsNBOut  flash NB default      
//{0x0F12,0x0100, WORD_LEN, 0},	                                           
//{0x0F12,0x0100, WORD_LEN, 0},	                                           
//{0x0F12,0x0100, WORD_LEN, 0},	                                           
//{0x0F12,0x0100, WORD_LEN, 0},	                                           
//{0x0F12,0x0100, WORD_LEN, 0},	                                           
//{0x0F12,0x0100, WORD_LEN, 0},	                                           
//{0x0F12,0x0100, WORD_LEN, 0},	                                           
//{0x002A,0x1826, WORD_LEN, 0},	                                           
//{0x0F12,0x0100, WORD_LEN, 0},	// fls_afl_FlashWP_Weight  flash NB default
//{0x0F12,0x00C0, WORD_LEN, 0},	                                           
//{0x0F12,0x0080, WORD_LEN, 0},	                                           
//{0x0F12,0x000A, WORD_LEN, 0},	                                           
//{0x0F12,0x0000, WORD_LEN, 0},	                                           
//{0x0F12,0x0030, WORD_LEN, 0},	// fls_afl_FlashWP_Weight  flash NB default
//{0x0F12,0x0040, WORD_LEN, 0},	
//{0x0F12,0x0048, WORD_LEN, 0},	
//{0x0F12,0x0050, WORD_LEN, 0},	
//{0x0F12,0x0060, WORD_LEN, 0},	
//{0x002A,0x4784, WORD_LEN, 0},	
//{0x0F12,0x00A0, WORD_LEN, 0},	// TNP_Regs_FlsWeightRIn  weight tune start in    
//{0x0F12,0x00C0, WORD_LEN, 0},	     
//{0x0F12,0x00D0, WORD_LEN, 0},	    
//{0x0F12,0x0100, WORD_LEN, 0},	      
//{0x0F12,0x0200, WORD_LEN, 0},	     
//{0x0F12,0x0300, WORD_LEN, 0},	     
//{0x0F12,0x0088, WORD_LEN, 0},	// TNP_Regs_FlsWeightROut  weight tune start out  
//{0x0F12,0x00B0, WORD_LEN, 0},	      
//{0x0F12,0x00C0, WORD_LEN, 0},	     
//{0x0F12,0x0100, WORD_LEN, 0},	    
//{0x0F12,0x0200, WORD_LEN, 0},	  
//{0x0F12,0x0300, WORD_LEN, 0},	 
//{0x002A,0x479C, WORD_LEN, 0},	   
//{0x0F12,0x0120, WORD_LEN, 0},	//Fls  BRIn
//{0x0F12,0x0150, WORD_LEN, 0},	      
//{0x0F12,0x0200, WORD_LEN, 0},	     
//{0x0F12,0x003C, WORD_LEN, 0},	// Fls  BROut
//{0x0F12,0x003B, WORD_LEN, 0},	    
//{0x0F12,0x0026, WORD_LEN, 0},	//brightness

//==================================================================================
// 14.AWB-BASIC setting
//==================================================================================

// AWB init Start point
{0x002A,0x145E, WORD_LEN, 0},
{0x0F12,0x0580, WORD_LEN, 0},	//awbb_GainsInit_0_
{0x0F12,0x0428, WORD_LEN, 0},	//awbb_GainsInit_1_
{0x0F12,0x0780, WORD_LEN, 0},	//awbb_GainsInit_2_

// AWB Convergence Speed
{0x002A,0x1464, WORD_LEN, 0},
{0x0F12,0x0008, WORD_LEN, 0},	//awbb_WpFilterMinThr
{0x0F12,0x0190, WORD_LEN, 0},	//awbb_WpFilterMaxThr
{0x0F12,0x00A0, WORD_LEN, 0},	//awbb_WpFilterCoef
{0x0F12,0x0004, WORD_LEN, 0},	//awbb_WpFilterSize		
{0x0F12,0x0002, WORD_LEN, 0},	//awbb_GridEnable	

{0x002A,0x144E, WORD_LEN, 0},			
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_RGainOff 		
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_BGainOff 		
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GGainOff 		
{0x0f12,0x00C2, WORD_LEN, 0},	//awbb_Alpha_Comp_Mode		
{0x0F12,0x0002, WORD_LEN, 0},	//awbb_Rpl_InvalidOutDoor		
{0x0F12,0x0001, WORD_LEN, 0},	//awbb_UseGrThrCorr		
{0x0F12,0x0074, WORD_LEN, 0},	//awbb_Use_Filters     		
{0x0F12,0x0001, WORD_LEN, 0},	//awbb_CorrectMinNumPatches		

// White Locus
{0x002A,0x11F0, WORD_LEN, 0},
{0x0F12,0x012C, WORD_LEN, 0},	//awbb_IntcR
{0x0F12,0x0121, WORD_LEN, 0},	//awbb_IntcB
{0x0F12,0x02DF, WORD_LEN, 0},	//awbb_GLocusR		
{0x0F12,0x0314, WORD_LEN, 0},	//awbb_GLocusB	

{0x002A,0x120E, WORD_LEN, 0},
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_MovingScale10
{0x0F12,0x05FD, WORD_LEN, 0},	//awbb_GamutWidthThr1
{0x0F12,0x036B, WORD_LEN, 0},	//awbb_GamutHeightThr1
{0x0F12,0x0020, WORD_LEN, 0},	//awbb_GamutWidthThr2
{0x0F12,0x001A, WORD_LEN, 0},	//awbb_GamutHeightThr2

{0x002A,0x1278, WORD_LEN, 0},
{0x0F12,0xFEF7, WORD_LEN, 0},	//awbb_SCDetectionMap_SEC_StartR_B
{0x0F12,0x0021, WORD_LEN, 0},	//awbb_SCDetectionMap_SEC_StepR_B
{0x0F12,0x07D0, WORD_LEN, 0},	//awbb_SCDetectionMap_SEC_SunnyNB
{0x0F12,0x07D0, WORD_LEN, 0},	//awbb_SCDetectionMap_SEC_StepNB
{0x0F12,0x01C8, WORD_LEN, 0},	//awbb_SCDetectionMap_SEC_LowTempR_B
{0x0F12,0x0096, WORD_LEN, 0},	//awbb_SCDetectionMap_SEC_SunnyNBZone
{0x0F12,0x0004, WORD_LEN, 0},	//awbb_SCDetectionMap_SEC_LowTempR_BZone

{0x002A,0x1224, WORD_LEN, 0},
{0x0F12,0x0032, WORD_LEN, 0},	//awbb_LowBr
{0x0F12,0x001E, WORD_LEN, 0},	//awbb_LowBr_NBzone
{0x0F12,0x00E2, WORD_LEN, 0},	//awbb_YThreshHigh
{0x0F12,0x0010, WORD_LEN, 0},	//awbb_YThreshLow_Norm
{0x0F12,0x0002, WORD_LEN, 0},	//awbb_YThreshLow_Low
{0x002A,0x2BA4, WORD_LEN, 0},
{0x0F12,0x0002, WORD_LEN, 0},	//Mon_AWB_ByPassMode

{0x002A,0x11FC, WORD_LEN, 0},			
{0x0F12,0x000C, WORD_LEN, 0},	//awbb_MinNumOfFinalPatches  		

{0x002A,0x1208, WORD_LEN, 0},  
{0x0F12,0x0020, WORD_LEN, 0},	//awbb_MinNumOfChromaclassifpatches

// Indoor Zone
{0x002A,0x101C, WORD_LEN, 0},
{0x0F12,0x0360, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_0__m_left
{0x0F12,0x036C, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_0__m_right
{0x0F12,0x0320, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_1__m_left
{0x0F12,0x038A, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_1__m_right
{0x0F12,0x02E8, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_2__m_left
{0x0F12,0x0380, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_2__m_right
{0x0F12,0x02BE, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_3__m_left
{0x0F12,0x035A, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_3__m_right
{0x0F12,0x0298, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_4__m_left
{0x0F12,0x0334, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_4__m_right
{0x0F12,0x0272, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_5__m_left
{0x0F12,0x030E, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_5__m_right
{0x0F12,0x024C, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_6__m_left
{0x0F12,0x02EA, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_6__m_right
{0x0F12,0x0230, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_7__m_left
{0x0F12,0x02CC, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_7__m_right
{0x0F12,0x0214, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_8__m_left
{0x0F12,0x02B0, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_8__m_right
{0x0F12,0x01F8, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_9__m_left
{0x0F12,0x0294, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_9__m_right
{0x0F12,0x01DC, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_10__m_left
{0x0F12,0x0278, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_10__m_right
{0x0F12,0x01C0, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_11__m_left
{0x0F12,0x0264, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_11__m_right
{0x0F12,0x01AA, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_12__m_left
{0x0F12,0x0250, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_12__m_right
{0x0F12,0x0196, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_13__m_left
{0x0F12,0x023C, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_13__m_right
{0x0F12,0x0180, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_14__m_left
{0x0F12,0x0228, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_14__m_right
{0x0F12,0x016C, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_15__m_left
{0x0F12,0x0214, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_15__m_right
{0x0F12,0x0168, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_16__m_left
{0x0F12,0x0200, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_16__m_right
{0x0F12,0x0172, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_17__m_left
{0x0F12,0x01EC, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_17__m_right
{0x0F12,0x019A, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_18__m_left
{0x0F12,0x01D8, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_18__m_right
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_19__m_left
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_IndoorGrZones_m_BGrid_19__m_right

{0x0F12,0x0005, WORD_LEN, 0},	//awbb_IndoorGrZones_m_GridStep
{0x002A,0x1070, WORD_LEN, 0},
{0x0F12,0x0013, WORD_LEN, 0},	//awbb_IndoorGrZones_ZInfo_m_GridSz
{0x002A,0x1074, WORD_LEN, 0},
{0x0F12,0x00EC, WORD_LEN, 0},	//awbb_IndoorGrZones_m_Boffs

// Outdoor Zone
{0x002A,0x1078, WORD_LEN, 0},
{0x0F12,0x0240, WORD_LEN, 0}, //0240 //0232	//awbb_OutdoorGrZones_m_BGrid_0__m_left
{0x0F12,0x025A, WORD_LEN, 0}, //025A //025A	//awbb_OutdoorGrZones_m_BGrid_0__m_right
{0x0F12,0x0234, WORD_LEN, 0}, //022C //021E	//awbb_OutdoorGrZones_m_BGrid_1__m_left
{0x0F12,0x0274, WORD_LEN, 0}, //0274 //0274	//awbb_OutdoorGrZones_m_BGrid_1__m_right
{0x0F12,0x0228, WORD_LEN, 0}, //021E //020E	//awbb_OutdoorGrZones_m_BGrid_2__m_left
{0x0F12,0x028E, WORD_LEN, 0}, //028E //028E	//awbb_OutdoorGrZones_m_BGrid_2__m_right
{0x0F12,0x0220, WORD_LEN, 0}, //0210 //0200	//awbb_OutdoorGrZones_m_BGrid_3__m_left
{0x0F12,0x0290, WORD_LEN, 0}, //0290 //0290	//awbb_OutdoorGrZones_m_BGrid_3__m_right
{0x0F12,0x0212, WORD_LEN, 0}, //0204 //01F4	//awbb_OutdoorGrZones_m_BGrid_4__m_left
{0x0F12,0x0286, WORD_LEN, 0}, //0286 //0286	//awbb_OutdoorGrZones_m_BGrid_4__m_right
{0x0F12,0x0202, WORD_LEN, 0}, //01FA //01E8	//awbb_OutdoorGrZones_m_BGrid_5__m_left
{0x0F12,0x027E, WORD_LEN, 0}, //027E //027E	//awbb_OutdoorGrZones_m_BGrid_5__m_right
{0x0F12,0x01F6, WORD_LEN, 0}, //01F2 //01DE	//awbb_OutdoorGrZones_m_BGrid_6__m_left
{0x0F12,0x0274, WORD_LEN, 0}, //0274 //0274	//awbb_OutdoorGrZones_m_BGrid_6__m_right
{0x0F12,0x01EC, WORD_LEN, 0}, //01EA //01D2	//awbb_OutdoorGrZones_m_BGrid_7__m_left
{0x0F12,0x0268, WORD_LEN, 0}, //0268 //0268	//awbb_OutdoorGrZones_m_BGrid_7__m_right
{0x0F12,0x01E6, WORD_LEN, 0}, //01E6 //01D0	//awbb_OutdoorGrZones_m_BGrid_8__m_left
{0x0F12,0x025E, WORD_LEN, 0}, //025E //025E	//awbb_OutdoorGrZones_m_BGrid_8__m_right
{0x0F12,0x01E4, WORD_LEN, 0}, //01E4 //01D6	//awbb_OutdoorGrZones_m_BGrid_9__m_left
{0x0F12,0x0252, WORD_LEN, 0}, //0252 //0252	//awbb_OutdoorGrZones_m_BGrid_9__m_right
{0x0F12,0x01E6, WORD_LEN, 0}, //01E6 //01E2	//awbb_OutdoorGrZones_m_BGrid_10__m_left
{0x0F12,0x0248, WORD_LEN, 0}, //0248 //0248	//awbb_OutdoorGrZones_m_BGrid_10__m_right
{0x0F12,0x01F4, WORD_LEN, 0}, //01F4 //01F4	//awbb_OutdoorGrZones_m_BGrid_11__m_left
{0x0F12,0x021A, WORD_LEN, 0}, //021A //021A	//awbb_OutdoorGrZones_m_BGrid_11__m_right
   
{0x0F12,0x0004, WORD_LEN, 0},	//awbb_OutdoorGrZones_m_GridStep
{0x002A,0x10AC, WORD_LEN, 0},
{0x0F12,0x000C, WORD_LEN, 0},	//awbb_OutdoorGrZones_ZInfo_m_GridSz
{0x002A,0x10B0, WORD_LEN, 0},
{0x0F12,0x01DA, WORD_LEN, 0},	//awbb_OutdoorGrZones_m_Boffs

// Low Brightness Zone
{0x002A,0x10B4, WORD_LEN, 0},
{0x0F12,0x0348, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_0__m_left
{0x0F12,0x03B6, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_0__m_right
{0x0F12,0x02B8, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_1__m_left
{0x0F12,0x03B6, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_1__m_right
{0x0F12,0x0258, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_2__m_left
{0x0F12,0x038E, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_2__m_right
{0x0F12,0x0212, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_3__m_left
{0x0F12,0x0348, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_3__m_right
{0x0F12,0x01CC, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_4__m_left
{0x0F12,0x030C, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_4__m_right
{0x0F12,0x01A2, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_5__m_left
{0x0F12,0x02D2, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_5__m_right
{0x0F12,0x0170, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_6__m_left
{0x0F12,0x02A6, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_6__m_right
{0x0F12,0x014C, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_7__m_left
{0x0F12,0x0280, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_7__m_right
{0x0F12,0x0128, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_8__m_left
{0x0F12,0x025C, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_8__m_right
{0x0F12,0x0146, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_9__m_left
{0x0F12,0x0236, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_9__m_right
{0x0F12,0x0164, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_10__m_left
{0x0F12,0x0212, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_10__m_right
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_11__m_left
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_LowBrGrZones_m_BGrid_11__m_right

{0x0F12,0x0006, WORD_LEN, 0},	//awbb_LowBrGrZones_m_GridStep
{0x002A,0x10E8, WORD_LEN, 0},
{0x0F12,0x000B, WORD_LEN, 0},	//awbb_LowBrGrZones_ZInfo_m_GridSz
{0x002A,0x10EC, WORD_LEN, 0},
{0x0F12,0x00D2, WORD_LEN, 0},	//awbb_LowBrGrZones_m_Boffs

// Low Temp. Zone
{0x002A,0x10F0, WORD_LEN, 0},
{0x0F12,0x039A, WORD_LEN, 0},
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_CrclLowT_R_c
{0x0F12,0x00FE, WORD_LEN, 0},
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_CrclLowT_B_c
{0x0F12,0x2284, WORD_LEN, 0},
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_CrclLowT_Rad_c   	

//AWB - GridCorrection			
{0x002A,0x1434, WORD_LEN, 0},
{0x0F12,0x02C1, WORD_LEN, 0},	//awbb_GridConst_1_0_
{0x0F12,0x033A, WORD_LEN, 0},	//awbb_GridConst_1_1_
{0x0F12,0x038A, WORD_LEN, 0},	//awbb_GridConst_1_2_
{0x0F12,0x101A, WORD_LEN, 0},	//awbb_GridConst_2_0_
{0x0F12,0x1075, WORD_LEN, 0},	//awbb_GridConst_2_1_
{0x0F12,0x113D, WORD_LEN, 0},	//awbb_GridConst_2_2_
{0x0F12,0x113F, WORD_LEN, 0},	//awbb_GridConst_2_3_
{0x0F12,0x11AF, WORD_LEN, 0},	//awbb_GridConst_2_4_
{0x0F12,0x11F0, WORD_LEN, 0},	//awbb_GridConst_2_5_
{0x0F12,0x00B2, WORD_LEN, 0},	//awbb_GridCoeff_R_1
{0x0F12,0x00B8, WORD_LEN, 0},	//awbb_GridCoeff_B_1
{0x0F12,0x00CA, WORD_LEN, 0},	//awbb_GridCoeff_R_2
{0x0F12,0x009D, WORD_LEN, 0},	//awbb_GridCoeff_B_2

// Indoor Grid Offset			
{0x002A,0x13A4, WORD_LEN, 0},
{0x0F12,0x0000, WORD_LEN, 0},	//D65 awbb_GridCorr_R_0__0_
{0x0F12,0xffd0, WORD_LEN, 0},	//D65 CW awbb_GridCorr_R_0__1_
{0x0F12,0xFFd0, WORD_LEN, 0},	//CW awbb_GridCorr_R_0__2_
{0x0F12,0xFFD0, WORD_LEN, 0},	//A awbb_GridCorr_R_0__3_
{0x0F12,0xFFD0, WORD_LEN, 0},	//A awbb_GridCorr_R_0__4_
{0x0F12,0xFFD0, WORD_LEN, 0},	//H awbb_GridCorr_R_0__5_

{0x0F12,0x0000, WORD_LEN, 0},	//D65awbb_GridCorr_R_1__0_
{0x0F12,0xffd0, WORD_LEN, 0},	//D65 CW awbb_GridCorr_R_1__1_
{0x0F12,0xFFd0, WORD_LEN, 0},	//CW awbb_GridCorr_R_1__2_
{0x0F12,0xFFD0, WORD_LEN, 0},	//A awbb_GridCorr_R_1__3_
{0x0F12,0xFFD0, WORD_LEN, 0},	//A awbb_GridCorr_R_1__4_
{0x0F12,0xFFD0, WORD_LEN, 0},	//H awbb_GridCorr_R_1__5_

{0x0F12,0x0000, WORD_LEN, 0},	//D65awbb_GridCorr_R_2__0_
{0x0F12,0xffd0, WORD_LEN, 0},	//D65 CW awbb_GridCorr_R_2__1_
{0x0F12,0xFFd0, WORD_LEN, 0},	//CW awbb_GridCorr_R_2__2_
{0x0F12,0xFFD0, WORD_LEN, 0},	//A awbb_GridCorr_R_2__3_
{0x0F12,0xFFD0, WORD_LEN, 0},	//A awbb_GridCorr_R_2__4_
{0x0F12,0xFFD0, WORD_LEN, 0},	//H awbb_GridCorr_R_2__5_

{0x0F12,0xFFD0, WORD_LEN, 0},	//awbb_GridCorr_B_0__0_
{0x0F12,0xFFf0, WORD_LEN, 0},	//awbb_GridCorr_B_0__1_
{0x0F12,0x0010, WORD_LEN, 0},	//awbb_GridCorr_B_0__2_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_0__3_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_0__4_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_0__5_

{0x0F12,0xFFD0, WORD_LEN, 0},	//awbb_GridCorr_B_1__0_
{0x0F12,0xFFf0, WORD_LEN, 0},	//awbb_GridCorr_B_1__1_
{0x0F12,0x0010, WORD_LEN, 0},	//awbb_GridCorr_B_1__2_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_1__3_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_1__4_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_1__5_

{0x0F12,0xFFD0, WORD_LEN, 0},	//awbb_GridCorr_B_2__0_
{0x0F12,0xFFf0, WORD_LEN, 0},	//awbb_GridCorr_B_2__1_
{0x0F12,0x0010, WORD_LEN, 0},	//awbb_GridCorr_B_2__2_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_2__3_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_2__4_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_2__5_

// Outdoor Grid Offset
{0x0F12,0xFFB0, WORD_LEN, 0},	//FFC0 awbb_GridCorr_R_Out_0__0_
{0x0F12,0xFFC0, WORD_LEN, 0},	//FFD0 awbb_GridCorr_R_Out_0__1_
{0x0F12,0xFFC0, WORD_LEN, 0},	//FFD0 awbb_GridCorr_R_Out_0__2_
{0x0F12,0xFFC0, WORD_LEN, 0},	//FFD0 awbb_GridCorr_R_Out_0__3_
{0x0F12,0x0000, WORD_LEN, 0},	//0000 awbb_GridCorr_R_Out_0__4_
{0x0F12,0x0000, WORD_LEN, 0},	//0000 awbb_GridCorr_R_Out_0__5_

{0x0F12,0xFFB0, WORD_LEN, 0},	//awbb_GridCorr_R_Out_1__0_
{0x0F12,0xFFC0, WORD_LEN, 0},	//awbb_GridCorr_R_Out_1__1_
{0x0F12,0xFFC0, WORD_LEN, 0},	//awbb_GridCorr_R_Out_1__2_
{0x0F12,0xFFC0, WORD_LEN, 0},	//awbb_GridCorr_R_Out_1__3_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_R_Out_1__4_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_R_Out_1__5_

{0x0F12,0xFFB0, WORD_LEN, 0},	//awbb_GridCorr_R_Out_2__0_
{0x0F12,0xFFC0, WORD_LEN, 0},	//awbb_GridCorr_R_Out_2__1_
{0x0F12,0xFFC0, WORD_LEN, 0},	//awbb_GridCorr_R_Out_2__2_
{0x0F12,0xFFC0, WORD_LEN, 0},	//awbb_GridCorr_R_Out_2__3_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_R_Out_2__4_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_R_Out_2__5_

{0x0F12,0x0080, WORD_LEN, 0}, //0020	//0010 awbb_GridCorr_B_Out_0__0_
{0x0F12,0xFFE0, WORD_LEN, 0}, //FFE0	//FFD0 awbb_GridCorr_B_Out_0__1_
{0x0F12,0xFFE0, WORD_LEN, 0}, //FFE0	//FFD0 awbb_GridCorr_B_Out_0__2_
{0x0F12,0xFFE0, WORD_LEN, 0},	//FFD0 awbb_GridCorr_B_Out_0__3_
{0x0F12,0x0000, WORD_LEN, 0},	//0000 awbb_GridCorr_B_Out_0__4_
{0x0F12,0x0000, WORD_LEN, 0},	//0000 awbb_GridCorr_B_Out_0__5_

{0x0F12,0x0080, WORD_LEN, 0}, //0020	//awbb_GridCorr_B_Out_1__0_
{0x0F12,0xFFE0, WORD_LEN, 0},	//awbb_GridCorr_B_Out_1__1_
{0x0F12,0xFFE0, WORD_LEN, 0},	//awbb_GridCorr_B_Out_1__2_
{0x0F12,0xFFE0, WORD_LEN, 0},	//awbb_GridCorr_B_Out_1__3_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_Out_1__4_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_Out_1__5_

{0x0F12,0x0080, WORD_LEN, 0},	//awbb_GridCorr_B_Out_2__0_
{0x0F12,0xFFE0, WORD_LEN, 0},	//awbb_GridCorr_B_Out_2__1_
{0x0F12,0xFFE0, WORD_LEN, 0},	//awbb_GridCorr_B_Out_2__2_
{0x0F12,0xFFE0, WORD_LEN, 0},	//awbb_GridCorr_B_Out_2__3_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_Out_2__4_
{0x0F12,0x0000, WORD_LEN, 0},	//awbb_GridCorr_B_Out_2__5_


//==================================================================================
// 15.CCM Setting
//==================================================================================

{0x002A,0x08A6, WORD_LEN, 0},	
{0x0F12,0x00C0, WORD_LEN, 0},	//SARR_AwbCcmCord[0]
{0x0F12,0x0100, WORD_LEN, 0},	//SARR_AwbCcmCord[1]
{0x0F12,0x0125, WORD_LEN, 0},	//SARR_AwbCcmCord[2]
{0x0F12,0x015F, WORD_LEN, 0},	//SARR_AwbCcmCord[3]
{0x0F12,0x017C, WORD_LEN, 0},	//SARR_AwbCcmCord[4]
{0x0F12,0x0194, WORD_LEN, 0},	//SARR_AwbCcmCord[5]

{0x002A,0x0898, WORD_LEN, 0},	
{0x0F12,0x4800, WORD_LEN, 0},	//TVAR_wbt_pBaseCcms
{0x0F12,0x7000, WORD_LEN, 0},	
{0x002A,0x08A0, WORD_LEN, 0},	
{0x0F12,0x48D8, WORD_LEN, 0},	//TVAR_wbt_pOutdoorCcm
{0x0F12,0x7000, WORD_LEN, 0},	

//Horizon	
{0x002A,0x4800, WORD_LEN, 0},	
{0x0F12,0x0210, WORD_LEN, 0}, //0210 //0206 //0208 //0208	//H TVAR_wbt_pBaseCcms[0]  
{0x0F12,0xFFC0, WORD_LEN, 0}, //FFC0 //FFC0 //FFB5 //FFB5	//TVAR_wbt_pBaseCcms[1]  
{0x0F12,0xFFCC, WORD_LEN, 0}, //FFCC //FFD7 //FFE8 //FFE8	//TVAR_wbt_pBaseCcms[2]  
{0x0F12,0xFF34, WORD_LEN, 0}, //FF37 //FF3C //FF20 //FF20	//TVAR_wbt_pBaseCcms[3]  
{0x0F12,0x01EB, WORD_LEN, 0}, //01DA //01D0 //01BF //01BF	//TVAR_wbt_pBaseCcms[4]  
{0x0F12,0xFF09, WORD_LEN, 0}, //FF1B //FF21 //FF53 //FF53	//TVAR_wbt_pBaseCcms[5]  
{0x0F12,0x001C, WORD_LEN, 0}, //001C //0022 //0022 //0022	//TVAR_wbt_pBaseCcms[6]  
{0x0F12,0xFFE4, WORD_LEN, 0}, //FFE4 //FFEA //FFEA //FFEA	//TVAR_wbt_pBaseCcms[7]  
{0x0F12,0x01CD, WORD_LEN, 0}, //01CD //01C2 //01C2 //01C2	//TVAR_wbt_pBaseCcms[8]  
{0x0F12,0x00D9, WORD_LEN, 0}, //00D4 //00D5 //00F3 //00C6	//TVAR_wbt_pBaseCcms[9]  
{0x0F12,0x00D9, WORD_LEN, 0}, //00C9 //00C0 //00BA //0095	//TVAR_wbt_pBaseCcms[10] 
{0x0F12,0xFE95, WORD_LEN, 0}, //FEAA //FEB6 //FEA7 //FEFD	//TVAR_wbt_pBaseCcms[11] 
{0x0F12,0x0208, WORD_LEN, 0}, //0208 //0208 //0206 //0206	//TVAR_wbt_pBaseCcms[12] 
{0x0F12,0xFF75, WORD_LEN, 0}, //FF75 //FF75 //FF7F //FF7F	//TVAR_wbt_pBaseCcms[13] 
{0x0F12,0x0199, WORD_LEN, 0}, //0199 //0199 //0191 //0191	//TVAR_wbt_pBaseCcms[14] 
{0x0F12,0xFF06, WORD_LEN, 0}, //FF06 //FF06 //FF06 //FF06	//TVAR_wbt_pBaseCcms[15] 
{0x0F12,0x01BA, WORD_LEN, 0}, //01BA //01BA //01BA //01BA	//TVAR_wbt_pBaseCcms[16] 
{0x0F12,0x0108, WORD_LEN, 0}, //0108 //0108 //0108 //0108	//TVAR_wbt_pBaseCcms[17] 

// INCA A                               	
{0x0F12,0x0210, WORD_LEN, 0}, //0210 //0206 //0208 //0208	//A TVAR_wbt_pBaseCcms[18] 
{0x0F12,0xFFC0, WORD_LEN, 0}, //FFC0 //FFC0 //FFB5 //FFB5	//TVAR_wbt_pBaseCcms[19] 
{0x0F12,0xFFCC, WORD_LEN, 0}, //FFCC //FFD7 //FFE8 //FFE8	//TVAR_wbt_pBaseCcms[20] 
{0x0F12,0xFF34, WORD_LEN, 0}, //FF37 //FF3C //FF20 //FF20	//TVAR_wbt_pBaseCcms[21] 
{0x0F12,0x01EB, WORD_LEN, 0}, //01DA //01D0 //01BF //01BF	//TVAR_wbt_pBaseCcms[22] 
{0x0F12,0xFF09, WORD_LEN, 0}, //FF1B //FF21 //FF53 //FF53	//TVAR_wbt_pBaseCcms[23] 
{0x0F12,0x001C, WORD_LEN, 0}, //001C //0022 //0022 //0022	//TVAR_wbt_pBaseCcms[24] 
{0x0F12,0xFFE4, WORD_LEN, 0}, //FFE4 //FFEA //FFEA //FFEA	//TVAR_wbt_pBaseCcms[25] 
{0x0F12,0x01CD, WORD_LEN, 0}, //01CD //01C2 //01C2 //01C2	//TVAR_wbt_pBaseCcms[26] 
{0x0F12,0x00D9, WORD_LEN, 0}, //00D4 //00D5 //00F3 //00C6	//TVAR_wbt_pBaseCcms[27] 
{0x0F12,0x00D9, WORD_LEN, 0}, //00C9 //00C0 //00BA //0095	//TVAR_wbt_pBaseCcms[28] 
{0x0F12,0xFE95, WORD_LEN, 0}, //FEAA //FEB6 //FEA7 //FEFD	//TVAR_wbt_pBaseCcms[29] 
{0x0F12,0x0208, WORD_LEN, 0}, //0208 //0208 //0206 //0206	//TVAR_wbt_pBaseCcms[30] 
{0x0F12,0xFF75, WORD_LEN, 0}, //FF75 //FF75 //FF7F //FF7F	//TVAR_wbt_pBaseCcms[31] 
{0x0F12,0x0199, WORD_LEN, 0}, //0199 //0199 //0191 //0191	//TVAR_wbt_pBaseCcms[32] 
{0x0F12,0xFF06, WORD_LEN, 0}, //FF06 //FF06 //FF06 //FF06	//TVAR_wbt_pBaseCcms[33] 
{0x0F12,0x01BA, WORD_LEN, 0}, //01BA //01BA //01BA //01BA	//TVAR_wbt_pBaseCcms[34] 
{0x0F12,0x0108, WORD_LEN, 0}, //0108 //0108 //0108 //0108	//TVAR_wbt_pBaseCcms[35] 

//Warm White                    	
{0x0F12,0x0227, WORD_LEN, 0}, //0222 //0216 //020F //0208	//CW TVAR_wbt_pBaseCcms[36] 
{0x0F12,0xFF95, WORD_LEN, 0}, //FFA7 //FFAA //FFB0 //FFB5	//TVAR_wbt_pBaseCcms[37] 
{0x0F12,0xFFE0, WORD_LEN, 0}, //FFD8 //FFE5 //FFEA //FFE8	//TVAR_wbt_pBaseCcms[38] 
{0x0F12,0xFED2, WORD_LEN, 0}, //FEED //FEF0 //FEEE //FF20	//TVAR_wbt_pBaseCcms[39] 
{0x0F12,0x0174, WORD_LEN, 0}, //016D //0163 //0157 //01BF	//TVAR_wbt_pBaseCcms[40] 
{0x0F12,0xFF1F, WORD_LEN, 0}, //FF07 //FF09 //FF13 //FF53	//TVAR_wbt_pBaseCcms[41] 
{0x0F12,0xFFCB, WORD_LEN, 0}, //FFCF //FFD8 //FFDD //0022	//TVAR_wbt_pBaseCcms[42] 
{0x0F12,0xFFCB, WORD_LEN, 0}, //FFCF //FFD8 //FFDD //FFEA	//TVAR_wbt_pBaseCcms[43] 
{0x0F12,0x01D6, WORD_LEN, 0}, //01D1 //01C4 //01BA //01C2	//TVAR_wbt_pBaseCcms[44] 
{0x0F12,0x00F8, WORD_LEN, 0}, //00F8 //00F8 //00F2 //00C6	//TVAR_wbt_pBaseCcms[45] 
{0x0F12,0x00D5, WORD_LEN, 0}, //00D5 //00D5 //00CA //0095	//TVAR_wbt_pBaseCcms[46] 
{0x0F12,0xFF34, WORD_LEN, 0}, //FF34 //FF34 //FF48 //FEFD	//TVAR_wbt_pBaseCcms[47] 
{0x0F12,0x0151, WORD_LEN, 0}, //0151 //0151 //0151 //0206	//TVAR_wbt_pBaseCcms[48] 
{0x0F12,0xFF50, WORD_LEN, 0}, //FF50 //FF50 //FF50 //FF7F	//TVAR_wbt_pBaseCcms[49] 
{0x0F12,0x0147, WORD_LEN, 0}, //0147 //0147 //0147 //0191	//TVAR_wbt_pBaseCcms[50] 
{0x0F12,0xFF75, WORD_LEN, 0}, //FF75 //FF75 //FF75 //FF06	//TVAR_wbt_pBaseCcms[51] 
{0x0F12,0x0187, WORD_LEN, 0}, //0187 //0187 //0187 //01BA	//TVAR_wbt_pBaseCcms[52] 
{0x0F12,0x01BF, WORD_LEN, 0}, //01BF //01BF //01BF //0108	//TVAR_wbt_pBaseCcms[53] 
 	
//Cool White	
{0x0F12,0x0227, WORD_LEN, 0}, //0222 //0216 //020F	//CW TVAR_wbt_pBaseCcms[54] 
{0x0F12,0xFF95, WORD_LEN, 0}, //FFA7 //FFAA //FFB0	//TVAR_wbt_pBaseCcms[55] 
{0x0F12,0xFFE0, WORD_LEN, 0}, //FFD8 //FFE5 //FFEA	//TVAR_wbt_pBaseCcms[56] 
{0x0F12,0xFED2, WORD_LEN, 0}, //FEED //FEF0 //FEEE	//TVAR_wbt_pBaseCcms[57] 
{0x0F12,0x0174, WORD_LEN, 0}, //016D //0163 //0157	//TVAR_wbt_pBaseCcms[58] 
{0x0F12,0xFF1F, WORD_LEN, 0}, //FF07 //FF09 //FF13	//TVAR_wbt_pBaseCcms[59] 
{0x0F12,0xFFCB, WORD_LEN, 0}, //FFCF //FFD8 //FFDD	//TVAR_wbt_pBaseCcms[60] 
{0x0F12,0xFFCB, WORD_LEN, 0}, //FFCF //FFD8 //FFDD	//TVAR_wbt_pBaseCcms[61] 
{0x0F12,0x01D6, WORD_LEN, 0}, //01D1 //01C4 //01BA	//TVAR_wbt_pBaseCcms[62] 
{0x0F12,0x00F8, WORD_LEN, 0}, //00F8 //00F8 //00F2	//TVAR_wbt_pBaseCcms[63] 
{0x0F12,0x00D5, WORD_LEN, 0}, //00D5 //00D5 //00CA	//TVAR_wbt_pBaseCcms[64] 
{0x0F12,0xFF34, WORD_LEN, 0}, //FF34 //FF34 //FF48	//TVAR_wbt_pBaseCcms[65] 
{0x0F12,0x0151, WORD_LEN, 0}, //0151 //0151 //0151	//TVAR_wbt_pBaseCcms[66] 
{0x0F12,0xFF50, WORD_LEN, 0}, //FF50 //FF50 //FF50	//TVAR_wbt_pBaseCcms[67] 
{0x0F12,0x0147, WORD_LEN, 0}, //0147 //0147 //0147	//TVAR_wbt_pBaseCcms[68] 
{0x0F12,0xFF75, WORD_LEN, 0}, //FF75 //FF75 //FF75	//TVAR_wbt_pBaseCcms[69] 
{0x0F12,0x0187, WORD_LEN, 0}, //0187 //0187 //0187	//TVAR_wbt_pBaseCcms[70] 
{0x0F12,0x01BF, WORD_LEN, 0}, //01BF //01BF //01BF	//TVAR_wbt_pBaseCcms[71] 
 	
//D50	  
{0x0F12,0x01EC, WORD_LEN, 0}, //0200 //0204 //0204	//TVAR_wbt_pBaseCcms[72]  
{0x0F12,0xFFB2, WORD_LEN, 0}, //FFA8 //FFB2 //FFB2	//TVAR_wbt_pBaseCcms[73] 
{0x0F12,0x0007, WORD_LEN, 0}, //0002 //FFF5 //FFF5	//TVAR_wbt_pBaseCcms[74] 
{0x0F12,0xFEE1, WORD_LEN, 0}, //FEE1 //FEF1 //FEF1	//TVAR_wbt_pBaseCcms[75] 
{0x0F12,0x013A, WORD_LEN, 0}, //013A //014E //014E	//TVAR_wbt_pBaseCcms[76] 
{0x0F12,0xFF3C, WORD_LEN, 0}, //FF3C //FF18 //FF18	//TVAR_wbt_pBaseCcms[77] 
{0x0F12,0xFFDB, WORD_LEN, 0}, //FFDB //FFDB //FFE6	//TVAR_wbt_pBaseCcms[78] 
{0x0F12,0xFFDB, WORD_LEN, 0}, //FFDB //FFDB //FFDD	//TVAR_wbt_pBaseCcms[79] 
{0x0F12,0x01BE, WORD_LEN, 0}, //01BE //01BE //01B2	//TVAR_wbt_pBaseCcms[80] 
{0x0F12,0x00D3, WORD_LEN, 0}, //00F2 //00F2 //00F2	//TVAR_wbt_pBaseCcms[81] 
{0x0F12,0x00E4, WORD_LEN, 0}, //00CA //00CA //00CA	//TVAR_wbt_pBaseCcms[82] 
{0x0F12,0xFF4A, WORD_LEN, 0}, //FF48 //FF48 //FF48	//TVAR_wbt_pBaseCcms[83] 
{0x0F12,0x0151, WORD_LEN, 0}, //0151 //0151 //0151	//TVAR_wbt_pBaseCcms[84] 
{0x0F12,0xFF50, WORD_LEN, 0}, //FF50 //FF50 //FF50	//TVAR_wbt_pBaseCcms[85] 
{0x0F12,0x0147, WORD_LEN, 0}, //0147 //0147 //0147	//TVAR_wbt_pBaseCcms[86] 
{0x0F12,0xFF75, WORD_LEN, 0}, //FF75 //FF75 //FF75	//TVAR_wbt_pBaseCcms[87] 
{0x0F12,0x0187, WORD_LEN, 0}, //0187 //0187 //0187	//TVAR_wbt_pBaseCcms[88] 
{0x0F12,0x01BF, WORD_LEN, 0}, //01BF //01BF //01BF	//TVAR_wbt_pBaseCcms[89] 
 	
//D65                      	
{0x0F12,0x01EC, WORD_LEN, 0}, //0200 //0204 //0204	//d65 TVAR_wbt_pBaseCcms[90]  
{0x0F12,0xFFB2, WORD_LEN, 0}, //FFA8 //FFB2 //FFB2	//TVAR_wbt_pBaseCcms[91] 
{0x0F12,0x0007, WORD_LEN, 0}, //0002 //FFF5 //FFF5	//TVAR_wbt_pBaseCcms[92] 
{0x0F12,0xFEE1, WORD_LEN, 0}, //FEE1 //FEF1 //FEF1	//TVAR_wbt_pBaseCcms[93] 
{0x0F12,0x013A, WORD_LEN, 0}, //013A //014E //014E	//TVAR_wbt_pBaseCcms[94] 
{0x0F12,0xFF3C, WORD_LEN, 0}, //FF3C //FF18 //FF18	//TVAR_wbt_pBaseCcms[95] 
{0x0F12,0xFFDB, WORD_LEN, 0}, //FFDB //FFDB //FFE6	//TVAR_wbt_pBaseCcms[96] 
{0x0F12,0xFFDB, WORD_LEN, 0}, //FFDB //FFDB //FFDD	//TVAR_wbt_pBaseCcms[97] 
{0x0F12,0x01BE, WORD_LEN, 0}, //01BE //01BE //01B2	//TVAR_wbt_pBaseCcms[98] 
{0x0F12,0x00D3, WORD_LEN, 0}, //00F2 //00F2 //00F2	//TVAR_wbt_pBaseCcms[99] 
{0x0F12,0x00E4, WORD_LEN, 0}, //00CA //00CA //00CA	//TVAR_wbt_pBaseCcms[100]
{0x0F12,0xFF4A, WORD_LEN, 0}, //FF48 //FF48 //FF48	//TVAR_wbt_pBaseCcms[101]
{0x0F12,0x0151, WORD_LEN, 0}, //0151 //0151 //0151	//TVAR_wbt_pBaseCcms[102]
{0x0F12,0xFF50, WORD_LEN, 0}, //FF50 //FF50 //FF50	//TVAR_wbt_pBaseCcms[103]
{0x0F12,0x0147, WORD_LEN, 0}, //0147 //0147 //0147	//TVAR_wbt_pBaseCcms[104]
{0x0F12,0xFF75, WORD_LEN, 0}, //FF75 //FF75 //FF75	//TVAR_wbt_pBaseCcms[105]
{0x0F12,0x0187, WORD_LEN, 0}, //0187 //0187 //0187	//TVAR_wbt_pBaseCcms[106]
{0x0F12,0x01BF, WORD_LEN, 0}, //01BF //01BF //01BF	//TVAR_wbt_pBaseCcms[107]

//Outdoor	
{0x002A,0x48d8, WORD_LEN, 0},	
{0x0F12,0x01D4, WORD_LEN, 0}, //01DD //01E5 //01E5 //01E5	//TVAR_wbt_pOutdoorCcm[0] 
{0x0F12,0xFF9D, WORD_LEN, 0}, //FFB2 //FFA4 //FFA4 //FFA4	//TVAR_wbt_pOutdoorCcm[1] 
{0x0F12,0xFFE8, WORD_LEN, 0}, //FFD3 //FFDC //FFDC //FFDC	//TVAR_wbt_pOutdoorCcm[2] 
{0x0F12,0xFEA9, WORD_LEN, 0}, //FEA9 //FE90 //FE90 //FE90	//TVAR_wbt_pOutdoorCcm[3] 
{0x0F12,0x0143, WORD_LEN, 0}, //0143 //013F //013F //013F	//TVAR_wbt_pOutdoorCcm[4] 
{0x0F12,0xFF01, WORD_LEN, 0}, //FF01 //FF1B //FF1B //FF1B	//TVAR_wbt_pOutdoorCcm[5] 
{0x0F12,0xFFB7, WORD_LEN, 0}, //FFB7 //FFD2 //FFD2 //FFD2	//TVAR_wbt_pOutdoorCcm[6] 
{0x0F12,0xFFF5, WORD_LEN, 0}, //FFF5 //FFDF //FFDF //FFDF	//TVAR_wbt_pOutdoorCcm[7] 
{0x0F12,0x0237, WORD_LEN, 0}, //0237 //0236 //0236 //0236	//TVAR_wbt_pOutdoorCcm[8] 
{0x0F12,0x00B9, WORD_LEN, 0}, //00AF //00C3 //011C //00EC	//TVAR_wbt_pOutdoorCcm[9] 
{0x0F12,0x0126, WORD_LEN, 0}, //012B //011C //00C3 //00F8	//TVAR_wbt_pOutdoorCcm[10]
{0x0F12,0xFF33, WORD_LEN, 0}, //FF3D //FF39 //FF39 //FF34	//TVAR_wbt_pOutdoorCcm[11]
{0x0F12,0x01CE, WORD_LEN, 0}, //01CE //01CE //01CE //01CE	//TVAR_wbt_pOutdoorCcm[12]
{0x0F12,0xFF83, WORD_LEN, 0}, //FF83 //FF83 //FF83 //FF83	//TVAR_wbt_pOutdoorCcm[13]
{0x0F12,0x0195, WORD_LEN, 0}, //0195 //0195 //0195 //0195	//TVAR_wbt_pOutdoorCcm[14]
{0x0F12,0xFEF3, WORD_LEN, 0}, //FEF3 //FEF3 //FEF3 //FEF3	//TVAR_wbt_pOutdoorCcm[15]
{0x0F12,0x014F, WORD_LEN, 0}, //0126 //0126 //0126 //0126	//TVAR_wbt_pOutdoorCcm[16]
{0x0F12,0x0137, WORD_LEN, 0}, //0162 //0162 //0162 //0162	//TVAR_wbt_pOutdoorCcm[17]


//==================================================================================
// 16.GAMMA
//==================================================================================

{0x002A,0x0734, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][0] 
{0x0F12,0x000A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][1] 
{0x0F12,0x0016, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][2] 
{0x0F12,0x0030, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][3] 
{0x0F12,0x0066, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][4] 
{0x0F12,0x00D5, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][5] 
{0x0F12,0x0138, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][6] 
{0x0F12,0x0163, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][7] 
{0x0F12,0x0189, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][8] 
{0x0F12,0x01C6, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][9] 
{0x0F12,0x01F8, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][10]
{0x0F12,0x0222, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][11]
{0x0F12,0x0247, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][12]
{0x0F12,0x0282, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][13]
{0x0F12,0x02B5, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][14]
{0x0F12,0x030F, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][15]
{0x0F12,0x035F, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][16]
{0x0F12,0x03A2, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][17]
{0x0F12,0x03D8, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][18]
{0x0F12,0x03FF, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[0][19]

{0x0F12,0x0000, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][0] 
{0x0F12,0x000A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][1] 
{0x0F12,0x0016, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][2] 
{0x0F12,0x0030, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][3] 
{0x0F12,0x0066, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][4] 
{0x0F12,0x00D5, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][5] 
{0x0F12,0x0138, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][6] 
{0x0F12,0x0163, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][7] 
{0x0F12,0x0189, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][8] 
{0x0F12,0x01C6, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][9] 
{0x0F12,0x01F8, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][10]
{0x0F12,0x0222, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][11]
{0x0F12,0x0247, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][12]
{0x0F12,0x0282, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][13]
{0x0F12,0x02B5, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][14]
{0x0F12,0x030F, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][15]
{0x0F12,0x035F, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][16]
{0x0F12,0x03A2, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][17]
{0x0F12,0x03D8, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][18]
{0x0F12,0x03FF, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[1][19]

{0x0F12,0x0000, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][0] 
{0x0F12,0x000A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][1] 
{0x0F12,0x0016, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][2] 
{0x0F12,0x0030, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][3] 
{0x0F12,0x0066, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][4] 
{0x0F12,0x00D5, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][5] 
{0x0F12,0x0138, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][6] 
{0x0F12,0x0163, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][7] 
{0x0F12,0x0189, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][8] 
{0x0F12,0x01C6, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][9] 
{0x0F12,0x01F8, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][10]
{0x0F12,0x0222, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][11]
{0x0F12,0x0247, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][12]
{0x0F12,0x0282, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][13]
{0x0F12,0x02B5, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][14]
{0x0F12,0x030F, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][15]
{0x0F12,0x035F, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][16]
{0x0F12,0x03A2, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][17]
{0x0F12,0x03D8, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][18]
{0x0F12,0x03FF, WORD_LEN, 0},	//saRR_usDualGammaLutRGBIndoor[2][19]

{0x0F12,0x0000, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][0] 
{0x0F12,0x000C, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][1] 
{0x0F12,0x0020, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][2] 
{0x0F12,0x0050, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][3] 
{0x0F12,0x0098, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][4] 
{0x0F12,0x00FC, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][5] 
{0x0F12,0x0148, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][6] 
{0x0F12,0x0168, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][7] 
{0x0F12,0x0185, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][8] 
{0x0F12,0x01C1, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][9] 
{0x0F12,0x01F3, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][10]
{0x0F12,0x0220, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][11]
{0x0F12,0x024A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][12]
{0x0F12,0x0291, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][13]
{0x0F12,0x02D0, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][14]
{0x0F12,0x032A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][15]
{0x0F12,0x036A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][16]
{0x0F12,0x039F, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][17]
{0x0F12,0x03CC, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][18]
{0x0F12,0x03F9, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[0][19]

{0x0F12,0x0000, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][0] 
{0x0F12,0x000C, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][1] 
{0x0F12,0x0020, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][2] 
{0x0F12,0x0050, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][3] 
{0x0F12,0x0098, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][4] 
{0x0F12,0x00FC, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][5] 
{0x0F12,0x0148, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][6] 
{0x0F12,0x0168, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][7] 
{0x0F12,0x0185, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][8] 
{0x0F12,0x01C1, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][9] 
{0x0F12,0x01F3, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][10]
{0x0F12,0x0220, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][11]
{0x0F12,0x024A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][12]
{0x0F12,0x0291, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][13]
{0x0F12,0x02D0, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][14]
{0x0F12,0x032A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][15]
{0x0F12,0x036A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][16]
{0x0F12,0x039F, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][17]
{0x0F12,0x03CC, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][18]
{0x0F12,0x03F9, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[1][19]

{0x0F12,0x0000, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][0] 
{0x0F12,0x000C, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][1] 
{0x0F12,0x0020, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][2] 
{0x0F12,0x0050, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][3] 
{0x0F12,0x0098, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][4] 
{0x0F12,0x00FC, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][5] 
{0x0F12,0x0148, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][6] 
{0x0F12,0x0168, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][7] 
{0x0F12,0x0185, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][8] 
{0x0F12,0x01C1, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][9] 
{0x0F12,0x01F3, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][10]
{0x0F12,0x0220, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][11]
{0x0F12,0x024A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][12]
{0x0F12,0x0291, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][13]
{0x0F12,0x02D0, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][14]
{0x0F12,0x032A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][15]
{0x0F12,0x036A, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][16]
{0x0F12,0x039F, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][17]
{0x0F12,0x03CC, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][18]
{0x0F12,0x03F9, WORD_LEN, 0},	//saRR_usDualGammaLutRGBOutdoor[2][19]


//==================================================================================
// 17.AFIT
//==================================================================================
{0x002A,0x0944, WORD_LEN, 0},	
{0x0F12,0x0050, WORD_LEN, 0},	//afit_uNoiseIndInDoor
{0x0F12,0x00B0, WORD_LEN, 0},	//afit_uNoiseIndInDoor
{0x0F12,0x0196, WORD_LEN, 0},	//afit_uNoiseIndInDoor
{0x0F12,0x0245, WORD_LEN, 0},	//afit_uNoiseIndInDoor
{0x0F12,0x0300, WORD_LEN, 0},	//afit_uNoiseIndInDoor

{0x002A,0x0938, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	// on/off AFIT by NB option
{0x0F12,0x0014, WORD_LEN, 0},	//SARR_uNormBrInDoor
{0x0F12,0x00D2, WORD_LEN, 0},	//SARR_uNormBrInDoor
{0x0F12,0x0384, WORD_LEN, 0},	//SARR_uNormBrInDoor
{0x0F12,0x07D0, WORD_LEN, 0},	//SARR_uNormBrInDoor
{0x0F12,0x1388, WORD_LEN, 0},	//SARR_uNormBrInDoor

{0x002A,0x0976, WORD_LEN, 0},	
{0x0F12,0x0070, WORD_LEN, 0},	//afit_usGamutTh       
{0x0F12,0x0005, WORD_LEN, 0},	//afit_usNeargrayOffset
{0x0F12,0x0000, WORD_LEN, 0},	//afit_bUseSenBpr         
{0x0F12,0x01CC, WORD_LEN, 0},	//afit_usBprThr_0_        
{0x0F12,0x01CC, WORD_LEN, 0},	//afit_usBprThr_1_        
{0x0F12,0x01CC, WORD_LEN, 0},	//afit_usBprThr_2_        
{0x0F12,0x01CC, WORD_LEN, 0},	//afit_usBprThr_3_        
{0x0F12,0x01CC, WORD_LEN, 0},	//afit_usBprThr_4_        
{0x0F12,0x0180, WORD_LEN, 0},	//afit_NIContrastAFITValue
{0x0F12,0x0196, WORD_LEN, 0},	//afit_NIContrastTh       

{0x002A,0x098C, WORD_LEN, 0},	
{0x0F12,0x0000, WORD_LEN, 0},	//7000098C//AFIT16_BRIGHTNESS           
{0x0F12,0x0000, WORD_LEN, 0},	//7000098E//AFIT16_CONTRAST                                                        
{0x0F12,0x0000, WORD_LEN, 0},	//70000990//AFIT16_SATURATION                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000992//AFIT16_SHARP_BLUR                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000994//AFIT16_GLAMOUR                                                         
{0x0F12,0x00C0, WORD_LEN, 0},	//70000996//AFIT16_bnr_edge_high                                                   
{0x0F12,0x0064, WORD_LEN, 0},	//70000998//AFIT16_postdmsc_iLowBright                                             
{0x0F12,0x0384, WORD_LEN, 0},	//7000099A//AFIT16_postdmsc_iHighBright                                            
{0x0F12,0x005F, WORD_LEN, 0},	//7000099C//AFIT16_postdmsc_iLowSat                                                
{0x0F12,0x01F4, WORD_LEN, 0},	//7000099E//AFIT16_postdmsc_iHighSat                                               
{0x0F12,0x0070, WORD_LEN, 0},	//700009A0//AFIT16_postdmsc_iTune                                                  
{0x0F12,0x0040, WORD_LEN, 0},	//700009A2//AFIT16_yuvemix_mNegRanges_0                                            
{0x0F12,0x00A0, WORD_LEN, 0},	//700009A4//AFIT16_yuvemix_mNegRanges_1                                            
{0x0F12,0x0100, WORD_LEN, 0},	//700009A6//AFIT16_yuvemix_mNegRanges_2                                            
{0x0F12,0x0010, WORD_LEN, 0},	//700009A8//AFIT16_yuvemix_mPosRanges_0                                            
{0x0F12,0x0040, WORD_LEN, 0},	//700009AA//AFIT16_yuvemix_mPosRanges_1                                            
{0x0F12,0x00A0, WORD_LEN, 0},	//700009AC//AFIT16_yuvemix_mPosRanges_2                                            
{0x0F12,0x1430, WORD_LEN, 0},	//700009AE//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                        
{0x0F12,0x0201, WORD_LEN, 0},	//700009B0//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                   
{0x0F12,0x0204, WORD_LEN, 0},	//700009B2//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh              
{0x0F12,0x3604, WORD_LEN, 0},	//700009B4//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                  
{0x0F12,0x032A, WORD_LEN, 0},	//700009B6//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                 
{0x0F12,0x0403, WORD_LEN, 0},	//700009B8//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin              
{0x0F12,0x1B06, WORD_LEN, 0},	//700009BA//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                
{0x0F12,0x6015, WORD_LEN, 0},	//700009BC//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH           
{0x0F12,0x00C0, WORD_LEN, 0},	//700009BE//AFIT8_bnr_iNormalizedSTD_Limit   [7:0] AFIT8_bnr_iDirNRTune            
{0x0F12,0x6080, WORD_LEN, 0},	//700009C0//AFIT8_bnr_iDirMinThres   [7:0] AFIT8_bnr_iDirFltDiffThresHigh          
{0x0F12,0x4080, WORD_LEN, 0},	//700009C2//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh    
{0x0F12,0x0640, WORD_LEN, 0},	//700009C4//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed     
{0x0F12,0x0306, WORD_LEN, 0},	//700009C6//AFIT8_bnr_iHighMaxSlopeAllowed   [7:0] AFIT8_bnr_iLowSlopeThresh       
{0x0F12,0x2003, WORD_LEN, 0},	//700009C8//AFIT8_bnr_iHighSlopeThresh   [7:0] AFIT8_bnr_iSlopenessTH              
{0x0F12,0xFF01, WORD_LEN, 0},	//700009CA//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit         
{0x0F12,0x0000, WORD_LEN, 0},	//700009CC//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2              
{0x0F12,0x0400, WORD_LEN, 0},	//700009CE//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                   
{0x0F12,0x365A, WORD_LEN, 0},	//700009D0//AFIT8_bnr_iRadialLimit   [7:0] AFIT8_ee_iFSMagThLow                    
{0x0F12,0x102A, WORD_LEN, 0},	//700009D2//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                     
{0x0F12,0x000B, WORD_LEN, 0},	//700009D4//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                        
{0x0F12,0x0600, WORD_LEN, 0},	//700009D6//AFIT8_ee_iFSThHigh   [7:0] AFIT8_ee_iFSmagPower                        
{0x0F12,0x5A0F, WORD_LEN, 0},	//700009D8//AFIT8_ee_iFSVarCountTh   [7:0] AFIT8_ee_iRadialLimit                   
{0x0F12,0x0505, WORD_LEN, 0},	//700009DA//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                
{0x0F12,0x1802, WORD_LEN, 0},	//700009DC//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                        
{0x0F12,0x0000, WORD_LEN, 0},	//700009DE//AFIT8_ee_iROADSubMaxNR   [7:0] AFIT8_ee_iROADSubThres                  
{0x0F12,0x2006, WORD_LEN, 0},	//700009E0//AFIT8_ee_iROADNeiThres   [7:0] AFIT8_ee_iROADNeiMaxNR                  
{0x0F12,0x3028, WORD_LEN, 0},	//700009E2//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                   
{0x0F12,0x0418, WORD_LEN, 0},	//700009E4//AFIT8_ee_iWSharpen   [7:0] AFIT8_ee_iMShThresh                         
{0x0F12,0x0101, WORD_LEN, 0},	//700009E6//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                   
{0x0F12,0x0800, WORD_LEN, 0},	//700009E8//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                  
{0x0F12,0x1804, WORD_LEN, 0},	//700009EA//AFIT8_ee_iReduceEdgeThresh   [7:0] AFIT8_dmsc_iEnhThresh               
{0x0F12,0x4008, WORD_LEN, 0},	//700009EC//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                
{0x0F12,0x0540, WORD_LEN, 0},	//700009EE//AFIT8_dmsc_iDemBlurLow   [7:0] AFIT8_dmsc_iDemBlurRange                
{0x0F12,0x8006, WORD_LEN, 0},	//700009F0//AFIT8_dmsc_iDecisionThresh   [7:0] AFIT8_dmsc_iCentGrad                
{0x0F12,0x0020, WORD_LEN, 0},	//700009F2//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                 
{0x0F12,0x0000, WORD_LEN, 0},	//700009F4//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh          
{0x0F12,0x1800, WORD_LEN, 0},	//700009F6//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat              
{0x0F12,0x0000, WORD_LEN, 0},	//700009F8//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit           
{0x0F12,0x1E10, WORD_LEN, 0},	//700009FA//AFIT8_postdmsc_iBCoeff   [7:0] AFIT8_postdmsc_iGCoeff                  
{0x0F12,0x000B, WORD_LEN, 0},	//700009FC//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0            
{0x0F12,0x0607, WORD_LEN, 0},	//700009FE//AFIT8_yuvemix_mNegSlopes_1   [7:0] AFIT8_yuvemix_mNegSlopes_2          
{0x0F12,0x0005, WORD_LEN, 0},	//70000A00//AFIT8_yuvemix_mNegSlopes_3   [7:0] AFIT8_yuvemix_mPosSlopes_0          
{0x0F12,0x0607, WORD_LEN, 0},	//70000A02//AFIT8_yuvemix_mPosSlopes_1   [7:0] AFIT8_yuvemix_mPosSlopes_2          
{0x0F12,0x0405, WORD_LEN, 0},	//70000A04//AFIT8_yuvemix_mPosSlopes_3   [7:0] AFIT8_yuviirnr_iXSupportY           
{0x0F12,0x0205, WORD_LEN, 0},	//70000A06//AFIT8_yuviirnr_iXSupportUV   [7:0] AFIT8_yuviirnr_iLowYNorm            
{0x0F12,0x0304, WORD_LEN, 0},	//70000A08//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm            
{0x0F12,0x0409, WORD_LEN, 0},	//70000A0A//AFIT8_yuviirnr_iHighUVNorm   [7:0] AFIT8_yuviirnr_iYNormShift          
{0x0F12,0x0306, WORD_LEN, 0},	//70000A0C//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y       
{0x0F12,0x0407, WORD_LEN, 0},	//70000A0E//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y    
{0x0F12,0x1C04, WORD_LEN, 0},	//70000A10//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV   
{0x0F12,0x0214, WORD_LEN, 0},	//70000A12//AFIT8_yuviirnr_iDiffThreshH_UV   [7:0] AFIT8_yuviirnr_iMaxThreshL_Y    
{0x0F12,0x1002, WORD_LEN, 0},	//70000A14//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV     
{0x0F12,0x0610, WORD_LEN, 0},	//70000A16//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL     
{0x0F12,0x1A02, WORD_LEN, 0},	//70000A18//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL     
{0x0F12,0x4A18, WORD_LEN, 0},	//70000A1A//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower     
{0x0F12,0x0080, WORD_LEN, 0},	//70000A1C//AFIT8_RGBGamma2_iLinearity   [7:0] AFIT8_RGBGamma2_iDarkReduce         
{0x0F12,0x0348, WORD_LEN, 0},	//70000A1E//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset             
{0x0F12,0x0180, WORD_LEN, 0},	//70000A20//AFIT8_RGB2YUV_iRGBGain   [7:0] AFIT8_bnr_nClustLevel_H                 
{0x0F12,0x0A0A, WORD_LEN, 0},	//70000A22//AFIT8_bnr_iClustMulT_H   [7:0] AFIT8_bnr_iClustMulT_C                  
{0x0F12,0x0101, WORD_LEN, 0},	//70000A24//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C              
{0x0F12,0x2A36, WORD_LEN, 0},	//70000A26//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh               
{0x0F12,0x6024, WORD_LEN, 0},	//70000A28//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower               
{0x0F12,0x2A36, WORD_LEN, 0},	//70000A2A//AFIT8_ee_iLowShDenoise   [7:0] AFIT8_ee_iHighShDenoise                 
{0x0F12,0xFFFF, WORD_LEN, 0},	//70000A2C//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp               
{0x0F12,0x0808, WORD_LEN, 0},	//70000A2E//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope          
{0x0F12,0x0A01, WORD_LEN, 0},	//70000A30//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin         
{0x0F12,0x010A, WORD_LEN, 0},	//70000A32//AFIT8_bnr_iClustMulT_C_Bin   [7:0] AFIT8_bnr_iClustThresh_H_Bin        
{0x0F12,0x3601, WORD_LEN, 0},	//70000A34//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin       
{0x0F12,0x242A, WORD_LEN, 0},	//70000A36//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin       
{0x0F12,0x3660, WORD_LEN, 0},	//70000A38//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin        
{0x0F12,0xFF2A, WORD_LEN, 0},	//70000A3A//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin        
{0x0F12,0x08FF, WORD_LEN, 0},	//70000A3C//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin   
{0x0F12,0x0008, WORD_LEN, 0},	//70000A3E//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                    
{0x0F12,0x0001, WORD_LEN, 0},	//70000A40//AFITB_bnr_nClustLevel_C      [0]                                       	
{0x0F12,0x0000, WORD_LEN, 0},	//70000A42//AFIT16_BRIGHTNESS           
{0x0F12,0x0000, WORD_LEN, 0},	//70000A44//AFIT16_CONTRAST                                                        
{0x0F12,0x0000, WORD_LEN, 0},	//70000A46//AFIT16_SATURATION                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000A48//AFIT16_SHARP_BLUR                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000A4A//AFIT16_GLAMOUR                                                         
{0x0F12,0x00C0, WORD_LEN, 0},	//70000A4C//AFIT16_bnr_edge_high                                                   
{0x0F12,0x0064, WORD_LEN, 0},	//70000A4E//AFIT16_postdmsc_iLowBright                                             
{0x0F12,0x0384, WORD_LEN, 0},	//70000A50//AFIT16_postdmsc_iHighBright                                            
{0x0F12,0x0051, WORD_LEN, 0},	//70000A52//AFIT16_postdmsc_iLowSat                                                
{0x0F12,0x01F4, WORD_LEN, 0},	//70000A54//AFIT16_postdmsc_iHighSat                                               
{0x0F12,0x0070, WORD_LEN, 0},	//70000A56//AFIT16_postdmsc_iTune                                                  
{0x0F12,0x0040, WORD_LEN, 0},	//70000A58//AFIT16_yuvemix_mNegRanges_0                                            
{0x0F12,0x00A0, WORD_LEN, 0},	//70000A5A//AFIT16_yuvemix_mNegRanges_1                                            
{0x0F12,0x0100, WORD_LEN, 0},	//70000A5C//AFIT16_yuvemix_mNegRanges_2                                            
{0x0F12,0x0010, WORD_LEN, 0},	//70000A5E//AFIT16_yuvemix_mPosRanges_0                                            
{0x0F12,0x0060, WORD_LEN, 0},	//70000A60//AFIT16_yuvemix_mPosRanges_1                                            
{0x0F12,0x0100, WORD_LEN, 0},	//70000A62//AFIT16_yuvemix_mPosRanges_2                                            
{0x0F12,0x1430, WORD_LEN, 0},	//70000A64//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                        
{0x0F12,0x0201, WORD_LEN, 0},	//70000A66//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                   
{0x0F12,0x0204, WORD_LEN, 0},	//70000A68//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh              
{0x0F12,0x2404, WORD_LEN, 0},	//70000A6A//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                  
{0x0F12,0x031B, WORD_LEN, 0},	//70000A6C//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                 
{0x0F12,0x0103, WORD_LEN, 0},	//70000A6E//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin              
{0x0F12,0x1205, WORD_LEN, 0},	//70000A70//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                
{0x0F12,0x400D, WORD_LEN, 0},	//70000A72//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH           
{0x0F12,0x0080, WORD_LEN, 0},	//70000A74//AFIT8_bnr_iNormalizedSTD_Limit   [7:0] AFIT8_bnr_iDirNRTune            
{0x0F12,0x2080, WORD_LEN, 0},	//70000A76//AFIT8_bnr_iDirMinThres   [7:0] AFIT8_bnr_iDirFltDiffThresHigh          
{0x0F12,0x3040, WORD_LEN, 0},	//70000A78//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh    
{0x0F12,0x0630, WORD_LEN, 0},	//70000A7A//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed     
{0x0F12,0x0306, WORD_LEN, 0},	//70000A7C//AFIT8_bnr_iHighMaxSlopeAllowed   [7:0] AFIT8_bnr_iLowSlopeThresh       
{0x0F12,0x2003, WORD_LEN, 0},	//70000A7E//AFIT8_bnr_iHighSlopeThresh   [7:0] AFIT8_bnr_iSlopenessTH              
{0x0F12,0xFF01, WORD_LEN, 0},	//70000A80//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit         
{0x0F12,0x0404, WORD_LEN, 0},	//70000A82//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2              
{0x0F12,0x0300, WORD_LEN, 0},	//70000A84//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                   
{0x0F12,0x245A, WORD_LEN, 0},	//70000A86//AFIT8_bnr_iRadialLimit   [7:0] AFIT8_ee_iFSMagThLow                    
{0x0F12,0x1018, WORD_LEN, 0},	//70000A88//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                     
{0x0F12,0x000B, WORD_LEN, 0},	//70000A8A//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                        
{0x0F12,0x0B00, WORD_LEN, 0},	//70000A8C//AFIT8_ee_iFSThHigh   [7:0] AFIT8_ee_iFSmagPower                        
{0x0F12,0x5A0F, WORD_LEN, 0},	//70000A8E//AFIT8_ee_iFSVarCountTh   [7:0] AFIT8_ee_iRadialLimit                   
{0x0F12,0x0505, WORD_LEN, 0},	//70000A90//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                
{0x0F12,0x1802, WORD_LEN, 0},	//70000A92//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                        
{0x0F12,0x0000, WORD_LEN, 0},	//70000A94//AFIT8_ee_iROADSubMaxNR   [7:0] AFIT8_ee_iROADSubThres                  
{0x0F12,0x2006, WORD_LEN, 0},	//70000A96//AFIT8_ee_iROADNeiThres   [7:0] AFIT8_ee_iROADNeiMaxNR                  
{0x0F12,0x3428, WORD_LEN, 0},	//70000A98//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                   
{0x0F12,0x041C, WORD_LEN, 0},	//70000A9A//AFIT8_ee_iWSharpen   [7:0] AFIT8_ee_iMShThresh                         
{0x0F12,0x0101, WORD_LEN, 0},	//70000A9C//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                   
{0x0F12,0x0800, WORD_LEN, 0},	//70000A9E//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                  
{0x0F12,0x1004, WORD_LEN, 0},	//70000AA0//AFIT8_ee_iReduceEdgeThresh   [7:0] AFIT8_dmsc_iEnhThresh               
{0x0F12,0x4008, WORD_LEN, 0},	//70000AA2//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                
{0x0F12,0x0540, WORD_LEN, 0},	//70000AA4//AFIT8_dmsc_iDemBlurLow   [7:0] AFIT8_dmsc_iDemBlurRange                
{0x0F12,0x8006, WORD_LEN, 0},	//70000AA6//AFIT8_dmsc_iDecisionThresh   [7:0] AFIT8_dmsc_iCentGrad                
{0x0F12,0x0020, WORD_LEN, 0},	//70000AA8//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                 
{0x0F12,0x0000, WORD_LEN, 0},	//70000AAA//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh          
{0x0F12,0x1800, WORD_LEN, 0},	//70000AAC//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat              
{0x0F12,0x0000, WORD_LEN, 0},	//70000AAE//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit           
{0x0F12,0x1E10, WORD_LEN, 0},	//70000AB0//AFIT8_postdmsc_iBCoeff   [7:0] AFIT8_postdmsc_iGCoeff                  
{0x0F12,0x000B, WORD_LEN, 0},	//70000AB2//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0            
{0x0F12,0x0607, WORD_LEN, 0},	//70000AB4//AFIT8_yuvemix_mNegSlopes_1   [7:0] AFIT8_yuvemix_mNegSlopes_2          
{0x0F12,0x0005, WORD_LEN, 0},	//70000AB6//AFIT8_yuvemix_mNegSlopes_3   [7:0] AFIT8_yuvemix_mPosSlopes_0          
{0x0F12,0x0607, WORD_LEN, 0},	//70000AB8//AFIT8_yuvemix_mPosSlopes_1   [7:0] AFIT8_yuvemix_mPosSlopes_2          
{0x0F12,0x0405, WORD_LEN, 0},	//70000ABA//AFIT8_yuvemix_mPosSlopes_3   [7:0] AFIT8_yuviirnr_iXSupportY           
{0x0F12,0x0205, WORD_LEN, 0},	//70000ABC//AFIT8_yuviirnr_iXSupportUV   [7:0] AFIT8_yuviirnr_iLowYNorm            
{0x0F12,0x0304, WORD_LEN, 0},	//70000ABE//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm            
{0x0F12,0x0409, WORD_LEN, 0},	//70000AC0//AFIT8_yuviirnr_iHighUVNorm   [7:0] AFIT8_yuviirnr_iYNormShift          
{0x0F12,0x0306, WORD_LEN, 0},	//70000AC2//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y       
{0x0F12,0x0407, WORD_LEN, 0},	//70000AC4//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y    
{0x0F12,0x1F04, WORD_LEN, 0},	//70000AC6//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV   
{0x0F12,0x0218, WORD_LEN, 0},	//70000AC8//AFIT8_yuviirnr_iDiffThreshH_UV   [7:0] AFIT8_yuviirnr_iMaxThreshL_Y    
{0x0F12,0x1102, WORD_LEN, 0},	//70000ACA//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV     
{0x0F12,0x0611, WORD_LEN, 0},	//70000ACC//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL     
{0x0F12,0x1A02, WORD_LEN, 0},	//70000ACE//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL     
{0x0F12,0x8018, WORD_LEN, 0},	//70000AD0//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower     
{0x0F12,0x0080, WORD_LEN, 0},	//70000AD2//AFIT8_RGBGamma2_iLinearity   [7:0] AFIT8_RGBGamma2_iDarkReduce         
{0x0F12,0x0380, WORD_LEN, 0},	//70000AD4//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset             
{0x0F12,0x0180, WORD_LEN, 0},	//70000AD6//AFIT8_RGB2YUV_iRGBGain   [7:0] AFIT8_bnr_nClustLevel_H                 
{0x0F12,0x0A0A, WORD_LEN, 0},	//70000AD8//AFIT8_bnr_iClustMulT_H   [7:0] AFIT8_bnr_iClustMulT_C                  
{0x0F12,0x0101, WORD_LEN, 0},	//70000ADA//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C              
{0x0F12,0x1B24, WORD_LEN, 0},	//70000ADC//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh               
{0x0F12,0x6024, WORD_LEN, 0},	//70000ADE//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower               
{0x0F12,0x1D22, WORD_LEN, 0},	//70000AE0//AFIT8_ee_iLowShDenoise   [7:0] AFIT8_ee_iHighShDenoise                 
{0x0F12,0xFFFF, WORD_LEN, 0},	//70000AE2//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp               
{0x0F12,0x0808, WORD_LEN, 0},	//70000AE4//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope          
{0x0F12,0x0A01, WORD_LEN, 0},	//70000AE6//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin         
{0x0F12,0x010A, WORD_LEN, 0},	//70000AE8//AFIT8_bnr_iClustMulT_C_Bin   [7:0] AFIT8_bnr_iClustThresh_H_Bin        
{0x0F12,0x2401, WORD_LEN, 0},	//70000AEA//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin       
{0x0F12,0x241B, WORD_LEN, 0},	//70000AEC//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin       
{0x0F12,0x1E60, WORD_LEN, 0},	//70000AEE//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin        
{0x0F12,0xFF18, WORD_LEN, 0},	//70000AF0//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin        
{0x0F12,0x08FF, WORD_LEN, 0},	//70000AF2//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin   
{0x0F12,0x0008, WORD_LEN, 0},	//70000AF4//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                    
{0x0F12,0x0001, WORD_LEN, 0},	//70000AF6//AFITB_bnr_nClustLevel_C      [0]                                       	
{0x0F12,0x0000, WORD_LEN, 0},	//70000AF8//AFIT16_BRIGHTNESS           
{0x0F12,0x0000, WORD_LEN, 0},	//70000AFA//AFIT16_CONTRAST                                                        
{0x0F12,0x0000, WORD_LEN, 0},	//70000AFC//AFIT16_SATURATION                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000AFE//AFIT16_SHARP_BLUR                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000B00//AFIT16_GLAMOUR                                                         
{0x0F12,0x00C0, WORD_LEN, 0},	//70000B02//AFIT16_bnr_edge_high                                                   
{0x0F12,0x0064, WORD_LEN, 0},	//70000B04//AFIT16_postdmsc_iLowBright                                             
{0x0F12,0x0384, WORD_LEN, 0},	//70000B06//AFIT16_postdmsc_iHighBright                                            
{0x0F12,0x0043, WORD_LEN, 0},	//70000B08//AFIT16_postdmsc_iLowSat                                                
{0x0F12,0x01F4, WORD_LEN, 0},	//70000B0A//AFIT16_postdmsc_iHighSat                                               
{0x0F12,0x0070, WORD_LEN, 0},	//70000B0C//AFIT16_postdmsc_iTune                                                  
{0x0F12,0x0040, WORD_LEN, 0},	//70000B0E//AFIT16_yuvemix_mNegRanges_0                                            
{0x0F12,0x00A0, WORD_LEN, 0},	//70000B10//AFIT16_yuvemix_mNegRanges_1                                            
{0x0F12,0x0100, WORD_LEN, 0},	//70000B12//AFIT16_yuvemix_mNegRanges_2                                            
{0x0F12,0x0010, WORD_LEN, 0},	//70000B14//AFIT16_yuvemix_mPosRanges_0                                            
{0x0F12,0x0060, WORD_LEN, 0},	//70000B16//AFIT16_yuvemix_mPosRanges_1                                            
{0x0F12,0x0100, WORD_LEN, 0},	//70000B18//AFIT16_yuvemix_mPosRanges_2                                            
{0x0F12,0x1430, WORD_LEN, 0},	//70000B1A//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                        
{0x0F12,0x0201, WORD_LEN, 0},	//70000B1C//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                   
{0x0F12,0x0204, WORD_LEN, 0},	//70000B1E//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh              
{0x0F12,0x1B04, WORD_LEN, 0},	//70000B20//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                  
{0x0F12,0x0312, WORD_LEN, 0},	//70000B22//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                 
{0x0F12,0x0003, WORD_LEN, 0},	//70000B24//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin              
{0x0F12,0x0C03, WORD_LEN, 0},	//70000B26//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                
{0x0F12,0x2806, WORD_LEN, 0},	//70000B28//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH           
{0x0F12,0x0060, WORD_LEN, 0},	//70000B2A//AFIT8_bnr_iNormalizedSTD_Limit   [7:0] AFIT8_bnr_iDirNRTune            
{0x0F12,0x1580, WORD_LEN, 0},	//70000B2C//AFIT8_bnr_iDirMinThres   [7:0] AFIT8_bnr_iDirFltDiffThresHigh          
{0x0F12,0x2020, WORD_LEN, 0},	//70000B2E//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh    
{0x0F12,0x0620, WORD_LEN, 0},	//70000B30//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed     
{0x0F12,0x0306, WORD_LEN, 0},	//70000B32//AFIT8_bnr_iHighMaxSlopeAllowed   [7:0] AFIT8_bnr_iLowSlopeThresh       
{0x0F12,0x2003, WORD_LEN, 0},	//70000B34//AFIT8_bnr_iHighSlopeThresh   [7:0] AFIT8_bnr_iSlopenessTH              
{0x0F12,0xFF01, WORD_LEN, 0},	//70000B36//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit         
{0x0F12,0x0404, WORD_LEN, 0},	//70000B38//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2              
{0x0F12,0x0300, WORD_LEN, 0},	//70000B3A//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                   
{0x0F12,0x145A, WORD_LEN, 0},	//70000B3C//AFIT8_bnr_iRadialLimit   [7:0] AFIT8_ee_iFSMagThLow                    
{0x0F12,0x1010, WORD_LEN, 0},	//70000B3E//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                     
{0x0F12,0x000B, WORD_LEN, 0},	//70000B40//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                        
{0x0F12,0x0E00, WORD_LEN, 0},	//70000B42//AFIT8_ee_iFSThHigh   [7:0] AFIT8_ee_iFSmagPower                        
{0x0F12,0x5A0F, WORD_LEN, 0},	//70000B44//AFIT8_ee_iFSVarCountTh   [7:0] AFIT8_ee_iRadialLimit                   
{0x0F12,0x0504, WORD_LEN, 0},	//70000B46//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                
{0x0F12,0x1802, WORD_LEN, 0},	//70000B48//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                        
{0x0F12,0x0000, WORD_LEN, 0},	//70000B4A//AFIT8_ee_iROADSubMaxNR   [7:0] AFIT8_ee_iROADSubThres                  
{0x0F12,0x2006, WORD_LEN, 0},	//70000B4C//AFIT8_ee_iROADNeiThres   [7:0] AFIT8_ee_iROADNeiMaxNR                  
{0x0F12,0x3828, WORD_LEN, 0},	//70000B4E//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                   
{0x0F12,0x0428, WORD_LEN, 0},	//70000B50//AFIT8_ee_iWSharpen   [7:0] AFIT8_ee_iMShThresh                         
{0x0F12,0x0101, WORD_LEN, 0},	//70000B52//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                   
{0x0F12,0x8000, WORD_LEN, 0},	//70000B54//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                  
{0x0F12,0x0A04, WORD_LEN, 0},	//70000B56//AFIT8_ee_iReduceEdgeThresh   [7:0] AFIT8_dmsc_iEnhThresh               
{0x0F12,0x4008, WORD_LEN, 0},	//70000B58//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                
{0x0F12,0x0540, WORD_LEN, 0},	//70000B5A//AFIT8_dmsc_iDemBlurLow   [7:0] AFIT8_dmsc_iDemBlurRange                
{0x0F12,0x8006, WORD_LEN, 0},	//70000B5C//AFIT8_dmsc_iDecisionThresh   [7:0] AFIT8_dmsc_iCentGrad                
{0x0F12,0x0020, WORD_LEN, 0},	//70000B5E//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                 
{0x0F12,0x0000, WORD_LEN, 0},	//70000B60//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh          
{0x0F12,0x1800, WORD_LEN, 0},	//70000B62//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat              
{0x0F12,0x0000, WORD_LEN, 0},	//70000B64//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit           
{0x0F12,0x1E10, WORD_LEN, 0},	//70000B66//AFIT8_postdmsc_iBCoeff   [7:0] AFIT8_postdmsc_iGCoeff                  
{0x0F12,0x000B, WORD_LEN, 0},	//70000B68//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0            
{0x0F12,0x0607, WORD_LEN, 0},	//70000B6A//AFIT8_yuvemix_mNegSlopes_1   [7:0] AFIT8_yuvemix_mNegSlopes_2          
{0x0F12,0x0005, WORD_LEN, 0},	//70000B6C//AFIT8_yuvemix_mNegSlopes_3   [7:0] AFIT8_yuvemix_mPosSlopes_0          
{0x0F12,0x0607, WORD_LEN, 0},	//70000B6E//AFIT8_yuvemix_mPosSlopes_1   [7:0] AFIT8_yuvemix_mPosSlopes_2          
{0x0F12,0x0405, WORD_LEN, 0},	//70000B70//AFIT8_yuvemix_mPosSlopes_3   [7:0] AFIT8_yuviirnr_iXSupportY           
{0x0F12,0x0207, WORD_LEN, 0},	//70000B72//AFIT8_yuviirnr_iXSupportUV   [7:0] AFIT8_yuviirnr_iLowYNorm            
{0x0F12,0x0304, WORD_LEN, 0},	//70000B74//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm            
{0x0F12,0x0409, WORD_LEN, 0},	//70000B76//AFIT8_yuviirnr_iHighUVNorm   [7:0] AFIT8_yuviirnr_iYNormShift          
{0x0F12,0x0306, WORD_LEN, 0},	//70000B78//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y       
{0x0F12,0x0407, WORD_LEN, 0},	//70000B7A//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y    
{0x0F12,0x2404, WORD_LEN, 0},	//70000B7C//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV   
{0x0F12,0x0221, WORD_LEN, 0},	//70000B7E//AFIT8_yuviirnr_iDiffThreshH_UV   [7:0] AFIT8_yuviirnr_iMaxThreshL_Y    
{0x0F12,0x1202, WORD_LEN, 0},	//70000B80//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV     
{0x0F12,0x0613, WORD_LEN, 0},	//70000B82//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL     
{0x0F12,0x1A02, WORD_LEN, 0},	//70000B84//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL     
{0x0F12,0x8018, WORD_LEN, 0},	//70000B86//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower     
{0x0F12,0x0080, WORD_LEN, 0},	//70000B88//AFIT8_RGBGamma2_iLinearity   [7:0] AFIT8_RGBGamma2_iDarkReduce         
{0x0F12,0x0080, WORD_LEN, 0},	//70000B8A//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset             
{0x0F12,0x0180, WORD_LEN, 0},	//70000B8C//AFIT8_RGB2YUV_iRGBGain   [7:0] AFIT8_bnr_nClustLevel_H                 
{0x0F12,0x0A0A, WORD_LEN, 0},	//70000B8E//AFIT8_bnr_iClustMulT_H   [7:0] AFIT8_bnr_iClustMulT_C                  
{0x0F12,0x0101, WORD_LEN, 0},	//70000B90//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C              
{0x0F12,0x141D, WORD_LEN, 0},	//70000B92//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh               
{0x0F12,0x6024, WORD_LEN, 0},	//70000B94//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower               
{0x0F12,0x0C0C, WORD_LEN, 0},	//70000B96//AFIT8_ee_iLowShDenoise   [7:0] AFIT8_ee_iHighShDenoise                 
{0x0F12,0xFFFF, WORD_LEN, 0},	//70000B98//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp               
{0x0F12,0x0808, WORD_LEN, 0},	//70000B9A//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope          
{0x0F12,0x0A01, WORD_LEN, 0},	//70000B9C//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin         
{0x0F12,0x010A, WORD_LEN, 0},	//70000B9E//AFIT8_bnr_iClustMulT_C_Bin   [7:0] AFIT8_bnr_iClustThresh_H_Bin        
{0x0F12,0x1B01, WORD_LEN, 0},	//70000BA0//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin       
{0x0F12,0x2412, WORD_LEN, 0},	//70000BA2//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin       
{0x0F12,0x0C60, WORD_LEN, 0},	//70000BA4//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin        
{0x0F12,0xFF0C, WORD_LEN, 0},	//70000BA6//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin        
{0x0F12,0x08FF, WORD_LEN, 0},	//70000BA8//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin   
{0x0F12,0x0008, WORD_LEN, 0},	//70000BAA//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                    
{0x0F12,0x0001, WORD_LEN, 0},	//70000BAC//AFITB_bnr_nClustLevel_C      [0]                                       	
{0x0F12,0x0000, WORD_LEN, 0},	//70000BAE//AFIT16_BRIGHTNESS           
{0x0F12,0x0000, WORD_LEN, 0},	//70000BB0//AFIT16_CONTRAST                                                        
{0x0F12,0x0000, WORD_LEN, 0},	//70000BB2//AFIT16_SATURATION                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000BB4//AFIT16_SHARP_BLUR                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000BB6//AFIT16_GLAMOUR                                                         
{0x0F12,0x00C0, WORD_LEN, 0},	//70000BB8//AFIT16_bnr_edge_high                                                   
{0x0F12,0x0064, WORD_LEN, 0},	//70000BBA//AFIT16_postdmsc_iLowBright                                             
{0x0F12,0x0384, WORD_LEN, 0},	//70000BBC//AFIT16_postdmsc_iHighBright                                            
{0x0F12,0x0032, WORD_LEN, 0},	//70000BBE//AFIT16_postdmsc_iLowSat                                                
{0x0F12,0x01F4, WORD_LEN, 0},	//70000BC0//AFIT16_postdmsc_iHighSat                                               
{0x0F12,0x0070, WORD_LEN, 0},	//70000BC2//AFIT16_postdmsc_iTune                                                  
{0x0F12,0x0040, WORD_LEN, 0},	//70000BC4//AFIT16_yuvemix_mNegRanges_0                                            
{0x0F12,0x00A0, WORD_LEN, 0},	//70000BC6//AFIT16_yuvemix_mNegRanges_1                                            
{0x0F12,0x0100, WORD_LEN, 0},	//70000BC8//AFIT16_yuvemix_mNegRanges_2                                            
{0x0F12,0x0010, WORD_LEN, 0},	//70000BCA//AFIT16_yuvemix_mPosRanges_0                                            
{0x0F12,0x0060, WORD_LEN, 0},	//70000BCC//AFIT16_yuvemix_mPosRanges_1                                            
{0x0F12,0x0100, WORD_LEN, 0},	//70000BCE//AFIT16_yuvemix_mPosRanges_2                                            
{0x0F12,0x1430, WORD_LEN, 0},	//70000BD0//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                        
{0x0F12,0x0201, WORD_LEN, 0},	//70000BD2//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                   
{0x0F12,0x0204, WORD_LEN, 0},	//70000BD4//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh              
{0x0F12,0x1504, WORD_LEN, 0},	//70000BD6//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                  
{0x0F12,0x030F, WORD_LEN, 0},	//70000BD8//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                 
{0x0F12,0x0003, WORD_LEN, 0},	//70000BDA//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin              
{0x0F12,0x0902, WORD_LEN, 0},	//70000BDC//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                
{0x0F12,0x2004, WORD_LEN, 0},	//70000BDE//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH           
{0x0F12,0x0050, WORD_LEN, 0},	//70000BE0//AFIT8_bnr_iNormalizedSTD_Limit   [7:0] AFIT8_bnr_iDirNRTune            
{0x0F12,0x1140, WORD_LEN, 0},	//70000BE2//AFIT8_bnr_iDirMinThres   [7:0] AFIT8_bnr_iDirFltDiffThresHigh          
{0x0F12,0x201C, WORD_LEN, 0},	//70000BE4//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh    
{0x0F12,0x0620, WORD_LEN, 0},	//70000BE6//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed     
{0x0F12,0x0306, WORD_LEN, 0},	//70000BE8//AFIT8_bnr_iHighMaxSlopeAllowed   [7:0] AFIT8_bnr_iLowSlopeThresh       
{0x0F12,0x2003, WORD_LEN, 0},	//70000BEA//AFIT8_bnr_iHighSlopeThresh   [7:0] AFIT8_bnr_iSlopenessTH              
{0x0F12,0xFF01, WORD_LEN, 0},	//70000BEC//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit         
{0x0F12,0x0404, WORD_LEN, 0},	//70000BEE//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2              
{0x0F12,0x0300, WORD_LEN, 0},	//70000BF0//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                   
{0x0F12,0x145A, WORD_LEN, 0},	//70000BF2//AFIT8_bnr_iRadialLimit   [7:0] AFIT8_ee_iFSMagThLow                    
{0x0F12,0x1010, WORD_LEN, 0},	//70000BF4//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                     
{0x0F12,0x000B, WORD_LEN, 0},	//70000BF6//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                        
{0x0F12,0x1000, WORD_LEN, 0},	//70000BF8//AFIT8_ee_iFSThHigh   [7:0] AFIT8_ee_iFSmagPower                        
{0x0F12,0x5A0F, WORD_LEN, 0},	//70000BFA//AFIT8_ee_iFSVarCountTh   [7:0] AFIT8_ee_iRadialLimit                   
{0x0F12,0x0503, WORD_LEN, 0},	//70000BFC//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                
{0x0F12,0x1802, WORD_LEN, 0},	//70000BFE//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                        
{0x0F12,0x0000, WORD_LEN, 0},	//70000C00//AFIT8_ee_iROADSubMaxNR   [7:0] AFIT8_ee_iROADSubThres                  
{0x0F12,0x2006, WORD_LEN, 0},	//70000C02//AFIT8_ee_iROADNeiThres   [7:0] AFIT8_ee_iROADNeiMaxNR                  
{0x0F12,0x3C28, WORD_LEN, 0},	//70000C04//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                   
{0x0F12,0x042C, WORD_LEN, 0},	//70000C06//AFIT8_ee_iWSharpen   [7:0] AFIT8_ee_iMShThresh                         
{0x0F12,0x0101, WORD_LEN, 0},	//70000C08//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                   
{0x0F12,0xFF00, WORD_LEN, 0},	//70000C0A//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                  
{0x0F12,0x0904, WORD_LEN, 0},	//70000C0C//AFIT8_ee_iReduceEdgeThresh   [7:0] AFIT8_dmsc_iEnhThresh               
{0x0F12,0x4008, WORD_LEN, 0},	//70000C0E//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                
{0x0F12,0x0540, WORD_LEN, 0},	//70000C10//AFIT8_dmsc_iDemBlurLow   [7:0] AFIT8_dmsc_iDemBlurRange                
{0x0F12,0x8006, WORD_LEN, 0},	//70000C12//AFIT8_dmsc_iDecisionThresh   [7:0] AFIT8_dmsc_iCentGrad                
{0x0F12,0x0020, WORD_LEN, 0},	//70000C14//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                 
{0x0F12,0x0000, WORD_LEN, 0},	//70000C16//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh          
{0x0F12,0x1800, WORD_LEN, 0},	//70000C18//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat              
{0x0F12,0x0000, WORD_LEN, 0},	//70000C1A//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit           
{0x0F12,0x1E10, WORD_LEN, 0},	//70000C1C//AFIT8_postdmsc_iBCoeff   [7:0] AFIT8_postdmsc_iGCoeff                  
{0x0F12,0x000B, WORD_LEN, 0},	//70000C1E//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0            
{0x0F12,0x0607, WORD_LEN, 0},	//70000C20//AFIT8_yuvemix_mNegSlopes_1   [7:0] AFIT8_yuvemix_mNegSlopes_2          
{0x0F12,0x0005, WORD_LEN, 0},	//70000C22//AFIT8_yuvemix_mNegSlopes_3   [7:0] AFIT8_yuvemix_mPosSlopes_0          
{0x0F12,0x0607, WORD_LEN, 0},	//70000C24//AFIT8_yuvemix_mPosSlopes_1   [7:0] AFIT8_yuvemix_mPosSlopes_2          
{0x0F12,0x0405, WORD_LEN, 0},	//70000C26//AFIT8_yuvemix_mPosSlopes_3   [7:0] AFIT8_yuviirnr_iXSupportY           
{0x0F12,0x0206, WORD_LEN, 0},	//70000C28//AFIT8_yuviirnr_iXSupportUV   [7:0] AFIT8_yuviirnr_iLowYNorm            
{0x0F12,0x0304, WORD_LEN, 0},	//70000C2A//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm            
{0x0F12,0x0409, WORD_LEN, 0},	//70000C2C//AFIT8_yuviirnr_iHighUVNorm   [7:0] AFIT8_yuviirnr_iYNormShift          
{0x0F12,0x0305, WORD_LEN, 0},	//70000C2E//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y       
{0x0F12,0x0406, WORD_LEN, 0},	//70000C30//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y    
{0x0F12,0x2804, WORD_LEN, 0},	//70000C32//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV   
{0x0F12,0x0228, WORD_LEN, 0},	//70000C34//AFIT8_yuviirnr_iDiffThreshH_UV   [7:0] AFIT8_yuviirnr_iMaxThreshL_Y    
{0x0F12,0x1402, WORD_LEN, 0},	//70000C36//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV     
{0x0F12,0x0618, WORD_LEN, 0},	//70000C38//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL     
{0x0F12,0x1A02, WORD_LEN, 0},	//70000C3A//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL     
{0x0F12,0x8018, WORD_LEN, 0},	//70000C3C//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower     
{0x0F12,0x0080, WORD_LEN, 0},	//70000C3E//AFIT8_RGBGamma2_iLinearity   [7:0] AFIT8_RGBGamma2_iDarkReduce         
{0x0F12,0x0080, WORD_LEN, 0},	//70000C40//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset             
{0x0F12,0x0180, WORD_LEN, 0},	//70000C42//AFIT8_RGB2YUV_iRGBGain   [7:0] AFIT8_bnr_nClustLevel_H                 
{0x0F12,0x0A0A, WORD_LEN, 0},	//70000C44//AFIT8_bnr_iClustMulT_H   [7:0] AFIT8_bnr_iClustMulT_C                  
{0x0F12,0x0101, WORD_LEN, 0},	//70000C46//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C              
{0x0F12,0x1117, WORD_LEN, 0},	//70000C48//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh               
{0x0F12,0x6024, WORD_LEN, 0},	//70000C4A//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower               
{0x0F12,0x0A0A, WORD_LEN, 0},	//70000C4C//AFIT8_ee_iLowShDenoise   [7:0] AFIT8_ee_iHighShDenoise                 
{0x0F12,0xFFFF, WORD_LEN, 0},	//70000C4E//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp               
{0x0F12,0x0808, WORD_LEN, 0},	//70000C50//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope          
{0x0F12,0x0A01, WORD_LEN, 0},	//70000C52//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin         
{0x0F12,0x010A, WORD_LEN, 0},	//70000C54//AFIT8_bnr_iClustMulT_C_Bin   [7:0] AFIT8_bnr_iClustThresh_H_Bin        
{0x0F12,0x1501, WORD_LEN, 0},	//70000C56//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin       
{0x0F12,0x240F, WORD_LEN, 0},	//70000C58//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin       
{0x0F12,0x0A60, WORD_LEN, 0},	//70000C5A//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin        
{0x0F12,0xFF0A, WORD_LEN, 0},	//70000C5C//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin        
{0x0F12,0x08FF, WORD_LEN, 0},	//70000C5E//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin   
{0x0F12,0x0008, WORD_LEN, 0},	//70000C60//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                    
{0x0F12,0x0001, WORD_LEN, 0},	//70000C62//AFITB_bnr_nClustLevel_C      [0]                                       	
{0x0F12,0x0000, WORD_LEN, 0},	//70000C64//AFIT16_BRIGHTNESS           
{0x0F12,0x0000, WORD_LEN, 0},	//70000C66//AFIT16_CONTRAST                                                        
{0x0F12,0x0000, WORD_LEN, 0},	//70000C68//AFIT16_SATURATION                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000C6A//AFIT16_SHARP_BLUR                                                      
{0x0F12,0x0000, WORD_LEN, 0},	//70000C6C//AFIT16_GLAMOUR                                                         
{0x0F12,0x00C0, WORD_LEN, 0},	//70000C6E//AFIT16_bnr_edge_high                                                   
{0x0F12,0x0064, WORD_LEN, 0},	//70000C70//AFIT16_postdmsc_iLowBright                                             
{0x0F12,0x0384, WORD_LEN, 0},	//70000C72//AFIT16_postdmsc_iHighBright                                            
{0x0F12,0x0032, WORD_LEN, 0},	//70000C74//AFIT16_postdmsc_iLowSat                                                
{0x0F12,0x01F4, WORD_LEN, 0},	//70000C76//AFIT16_postdmsc_iHighSat                                               
{0x0F12,0x0070, WORD_LEN, 0},	//70000C78//AFIT16_postdmsc_iTune                                                  
{0x0F12,0x0040, WORD_LEN, 0},	//70000C7A//AFIT16_yuvemix_mNegRanges_0                                            
{0x0F12,0x00A0, WORD_LEN, 0},	//70000C7C//AFIT16_yuvemix_mNegRanges_1                                            
{0x0F12,0x0100, WORD_LEN, 0},	//70000C7E//AFIT16_yuvemix_mNegRanges_2                                            
{0x0F12,0x0010, WORD_LEN, 0},	//70000C80//AFIT16_yuvemix_mPosRanges_0                                            
{0x0F12,0x0060, WORD_LEN, 0},	//70000C82//AFIT16_yuvemix_mPosRanges_1                                            
{0x0F12,0x0100, WORD_LEN, 0},	//70000C84//AFIT16_yuvemix_mPosRanges_2                                            
{0x0F12,0x1430, WORD_LEN, 0},	//70000C86//AFIT8_bnr_edge_low  [7:0] AFIT8_bnr_repl_thresh                        
{0x0F12,0x0201, WORD_LEN, 0},	//70000C88//AFIT8_bnr_repl_force  [7:0] AFIT8_bnr_iHotThreshHigh                   
{0x0F12,0x0204, WORD_LEN, 0},	//70000C8A//AFIT8_bnr_iHotThreshLow   [7:0] AFIT8_bnr_iColdThreshHigh              
{0x0F12,0x0F04, WORD_LEN, 0},	//70000C8C//AFIT8_bnr_iColdThreshLow   [7:0] AFIT8_bnr_DispTH_Low                  
{0x0F12,0x030C, WORD_LEN, 0},	//70000C8E//AFIT8_bnr_DispTH_High   [7:0] AFIT8_bnr_DISP_Limit_Low                 
{0x0F12,0x0003, WORD_LEN, 0},	//70000C90//AFIT8_bnr_DISP_Limit_High   [7:0] AFIT8_bnr_iDistSigmaMin              
{0x0F12,0x0602, WORD_LEN, 0},	//70000C92//AFIT8_bnr_iDistSigmaMax   [7:0] AFIT8_bnr_iDiffSigmaLow                
{0x0F12,0x1803, WORD_LEN, 0},	//70000C94//AFIT8_bnr_iDiffSigmaHigh   [7:0] AFIT8_bnr_iNormalizedSTD_TH           
{0x0F12,0x0040, WORD_LEN, 0},	//70000C96//AFIT8_bnr_iNormalizedSTD_Limit   [7:0] AFIT8_bnr_iDirNRTune            
{0x0F12,0x0E20, WORD_LEN, 0},	//70000C98//AFIT8_bnr_iDirMinThres   [7:0] AFIT8_bnr_iDirFltDiffThresHigh          
{0x0F12,0x2018, WORD_LEN, 0},	//70000C9A//AFIT8_bnr_iDirFltDiffThresLow   [7:0] AFIT8_bnr_iDirSmoothPowerHigh    
{0x0F12,0x0620, WORD_LEN, 0},	//70000C9C//AFIT8_bnr_iDirSmoothPowerLow   [7:0] AFIT8_bnr_iLowMaxSlopeAllowed     
{0x0F12,0x0306, WORD_LEN, 0},	//70000C9E//AFIT8_bnr_iHighMaxSlopeAllowed   [7:0] AFIT8_bnr_iLowSlopeThresh       
{0x0F12,0x2003, WORD_LEN, 0},	//70000CA0//AFIT8_bnr_iHighSlopeThresh   [7:0] AFIT8_bnr_iSlopenessTH              
{0x0F12,0xFF01, WORD_LEN, 0},	//70000CA2//AFIT8_bnr_iSlopeBlurStrength   [7:0] AFIT8_bnr_iSlopenessLimit         
{0x0F12,0x0404, WORD_LEN, 0},	//70000CA4//AFIT8_bnr_AddNoisePower1   [7:0] AFIT8_bnr_AddNoisePower2              
{0x0F12,0x0200, WORD_LEN, 0},	//70000CA6//AFIT8_bnr_iRadialTune   [7:0] AFIT8_bnr_iRadialPower                   
{0x0F12,0x145A, WORD_LEN, 0},	//70000CA8//AFIT8_bnr_iRadialLimit   [7:0] AFIT8_ee_iFSMagThLow                    
{0x0F12,0x1010, WORD_LEN, 0},	//70000CAA//AFIT8_ee_iFSMagThHigh   [7:0] AFIT8_ee_iFSVarThLow                     
{0x0F12,0x000B, WORD_LEN, 0},	//70000CAC//AFIT8_ee_iFSVarThHigh   [7:0] AFIT8_ee_iFSThLow                        
{0x0F12,0x1200, WORD_LEN, 0},	//70000CAE//AFIT8_ee_iFSThHigh   [7:0] AFIT8_ee_iFSmagPower                        
{0x0F12,0x5A0F, WORD_LEN, 0},	//70000CB0//AFIT8_ee_iFSVarCountTh   [7:0] AFIT8_ee_iRadialLimit                   
{0x0F12,0x0502, WORD_LEN, 0},	//70000CB2//AFIT8_ee_iRadialPower   [7:0] AFIT8_ee_iSmoothEdgeSlope                
{0x0F12,0x1802, WORD_LEN, 0},	//70000CB4//AFIT8_ee_iROADThres   [7:0] AFIT8_ee_iROADMaxNR                        
{0x0F12,0x0000, WORD_LEN, 0},	//70000CB6//AFIT8_ee_iROADSubMaxNR   [7:0] AFIT8_ee_iROADSubThres                  
{0x0F12,0x2006, WORD_LEN, 0},	//70000CB8//AFIT8_ee_iROADNeiThres   [7:0] AFIT8_ee_iROADNeiMaxNR                  
{0x0F12,0x4028, WORD_LEN, 0},	//70000CBA//AFIT8_ee_iSmoothEdgeThres   [7:0] AFIT8_ee_iMSharpen                   
{0x0F12,0x0430, WORD_LEN, 0},	//70000CBC//AFIT8_ee_iWSharpen   [7:0] AFIT8_ee_iMShThresh                         
{0x0F12,0x0101, WORD_LEN, 0},	//70000CBE//AFIT8_ee_iWShThresh   [7:0] AFIT8_ee_iReduceNegative                   
{0x0F12,0xFF00, WORD_LEN, 0},	//70000CC0//AFIT8_ee_iEmbossCentAdd   [7:0] AFIT8_ee_iShDespeckle                  
{0x0F12,0x0804, WORD_LEN, 0},	//70000CC2//AFIT8_ee_iReduceEdgeThresh   [7:0] AFIT8_dmsc_iEnhThresh               
{0x0F12,0x4008, WORD_LEN, 0},	//70000CC4//AFIT8_dmsc_iDesatThresh   [7:0] AFIT8_dmsc_iDemBlurHigh                
{0x0F12,0x0540, WORD_LEN, 0},	//70000CC6//AFIT8_dmsc_iDemBlurLow   [7:0] AFIT8_dmsc_iDemBlurRange                
{0x0F12,0x8006, WORD_LEN, 0},	//70000CC8//AFIT8_dmsc_iDecisionThresh   [7:0] AFIT8_dmsc_iCentGrad                
{0x0F12,0x0020, WORD_LEN, 0},	//70000CCA//AFIT8_dmsc_iMonochrom   [7:0] AFIT8_dmsc_iGBDenoiseVal                 
{0x0F12,0x0000, WORD_LEN, 0},	//70000CCC//AFIT8_dmsc_iGRDenoiseVal   [7:0] AFIT8_dmsc_iEdgeDesatThrHigh          
{0x0F12,0x1800, WORD_LEN, 0},	//70000CCE//AFIT8_dmsc_iEdgeDesatThrLow   [7:0] AFIT8_dmsc_iEdgeDesat              
{0x0F12,0x0000, WORD_LEN, 0},	//70000CD0//AFIT8_dmsc_iNearGrayDesat   [7:0] AFIT8_dmsc_iEdgeDesatLimit           
{0x0F12,0x1E10, WORD_LEN, 0},	//70000CD2//AFIT8_postdmsc_iBCoeff   [7:0] AFIT8_postdmsc_iGCoeff                  
{0x0F12,0x000B, WORD_LEN, 0},	//70000CD4//AFIT8_postdmsc_iWideMult   [7:0] AFIT8_yuvemix_mNegSlopes_0            
{0x0F12,0x0607, WORD_LEN, 0},	//70000CD6//AFIT8_yuvemix_mNegSlopes_1   [7:0] AFIT8_yuvemix_mNegSlopes_2          
{0x0F12,0x0005, WORD_LEN, 0},	//70000CD8//AFIT8_yuvemix_mNegSlopes_3   [7:0] AFIT8_yuvemix_mPosSlopes_0          
{0x0F12,0x0607, WORD_LEN, 0},	//70000CDA//AFIT8_yuvemix_mPosSlopes_1   [7:0] AFIT8_yuvemix_mPosSlopes_2          
{0x0F12,0x0405, WORD_LEN, 0},	//70000CDC//AFIT8_yuvemix_mPosSlopes_3   [7:0] AFIT8_yuviirnr_iXSupportY           
{0x0F12,0x0205, WORD_LEN, 0},	//70000CDE//AFIT8_yuviirnr_iXSupportUV   [7:0] AFIT8_yuviirnr_iLowYNorm            
{0x0F12,0x0304, WORD_LEN, 0},	//70000CE0//AFIT8_yuviirnr_iHighYNorm   [7:0] AFIT8_yuviirnr_iLowUVNorm            
{0x0F12,0x0409, WORD_LEN, 0},	//70000CE2//AFIT8_yuviirnr_iHighUVNorm   [7:0] AFIT8_yuviirnr_iYNormShift          
{0x0F12,0x0306, WORD_LEN, 0},	//70000CE4//AFIT8_yuviirnr_iUVNormShift   [7:0] AFIT8_yuviirnr_iVertLength_Y       
{0x0F12,0x0407, WORD_LEN, 0},	//70000CE6//AFIT8_yuviirnr_iVertLength_UV   [7:0] AFIT8_yuviirnr_iDiffThreshL_Y    
{0x0F12,0x2C04, WORD_LEN, 0},	//70000CE8//AFIT8_yuviirnr_iDiffThreshH_Y   [7:0] AFIT8_yuviirnr_iDiffThreshL_UV   
{0x0F12,0x022C, WORD_LEN, 0},	//70000CEA//AFIT8_yuviirnr_iDiffThreshH_UV   [7:0] AFIT8_yuviirnr_iMaxThreshL_Y    
{0x0F12,0x1402, WORD_LEN, 0},	//70000CEC//AFIT8_yuviirnr_iMaxThreshH_Y   [7:0] AFIT8_yuviirnr_iMaxThreshL_UV     
{0x0F12,0x0618, WORD_LEN, 0},	//70000CEE//AFIT8_yuviirnr_iMaxThreshH_UV   [7:0] AFIT8_yuviirnr_iYNRStrengthL     
{0x0F12,0x1A02, WORD_LEN, 0},	//70000CF0//AFIT8_yuviirnr_iYNRStrengthH   [7:0] AFIT8_yuviirnr_iUVNRStrengthL     
{0x0F12,0x8018, WORD_LEN, 0},	//70000CF2//AFIT8_yuviirnr_iUVNRStrengthH   [7:0] AFIT8_byr_gras_iShadingPower     
{0x0F12,0x0080, WORD_LEN, 0},	//70000CF4//AFIT8_RGBGamma2_iLinearity   [7:0] AFIT8_RGBGamma2_iDarkReduce         
{0x0F12,0x0080, WORD_LEN, 0},	//70000CF6//AFIT8_ccm_oscar_iSaturation   [7:0] AFIT8_RGB2YUV_iYOffset             
{0x0F12,0x0180, WORD_LEN, 0},	//70000CF8//AFIT8_RGB2YUV_iRGBGain   [7:0] AFIT8_bnr_nClustLevel_H                 
{0x0F12,0x0A0A, WORD_LEN, 0},	//70000CFA//AFIT8_bnr_iClustMulT_H   [7:0] AFIT8_bnr_iClustMulT_C                  
{0x0F12,0x0101, WORD_LEN, 0},	//70000CFC//AFIT8_bnr_iClustThresh_H   [7:0] AFIT8_bnr_iClustThresh_C              
{0x0F12,0x0C0F, WORD_LEN, 0},	//70000CFE//AFIT8_bnr_iDenThreshLow   [7:0] AFIT8_bnr_iDenThreshHigh               
{0x0F12,0x6024, WORD_LEN, 0},	//70000D00//AFIT8_ee_iLowSharpPower   [7:0] AFIT8_ee_iHighSharpPower               
{0x0F12,0x0808, WORD_LEN, 0},	//70000D02//AFIT8_ee_iLowShDenoise   [7:0] AFIT8_ee_iHighShDenoise                 
{0x0F12,0xFFFF, WORD_LEN, 0},	//70000D04//AFIT8_ee_iLowSharpClamp   [7:0] AFIT8_ee_iHighSharpClamp               
{0x0F12,0x0808, WORD_LEN, 0},	//70000D06//AFIT8_ee_iReduceEdgeMinMult   [7:0] AFIT8_ee_iReduceEdgeSlope          
{0x0F12,0x0A01, WORD_LEN, 0},	//70000D08//AFIT8_bnr_nClustLevel_H_Bin   [7:0] AFIT8_bnr_iClustMulT_H_Bin         
{0x0F12,0x010A, WORD_LEN, 0},	//70000D0A//AFIT8_bnr_iClustMulT_C_Bin   [7:0] AFIT8_bnr_iClustThresh_H_Bin        
{0x0F12,0x0F01, WORD_LEN, 0},	//70000D0C//AFIT8_bnr_iClustThresh_C_Bin   [7:0] AFIT8_bnr_iDenThreshLow_Bin       
{0x0F12,0x240C, WORD_LEN, 0},	//70000D0E//AFIT8_bnr_iDenThreshHigh_Bin   [7:0] AFIT8_ee_iLowSharpPower_Bin       
{0x0F12,0x0860, WORD_LEN, 0},	//70000D10//AFIT8_ee_iHighSharpPower_Bin   [7:0] AFIT8_ee_iLowShDenoise_Bin        
{0x0F12,0xFF08, WORD_LEN, 0},	//70000D12//AFIT8_ee_iHighShDenoise_Bin   [7:0] AFIT8_ee_iLowSharpClamp_Bin        
{0x0F12,0x08FF, WORD_LEN, 0},	//70000D14//AFIT8_ee_iHighSharpClamp_Bin   [7:0] AFIT8_ee_iReduceEdgeMinMult_Bin   
{0x0F12,0x0008, WORD_LEN, 0},	//70000D16//AFIT8_ee_iReduceEdgeSlope_Bin [7:0]                                    
{0x0F12,0x0001, WORD_LEN, 0},	//70000D18//AFITB_bnr_nClustLevel_C      [0]                                       

{0x0F12,0x23CE, WORD_LEN, 0},	//70000D19//ConstAfitBaseVals
{0x0F12,0xFDC8, WORD_LEN, 0},	//70000D1A//ConstAfitBaseVals
{0x0F12,0x112E, WORD_LEN, 0},	//70000D1B//ConstAfitBaseVals
{0x0F12,0x93A5, WORD_LEN, 0},	//70000D1C//ConstAfitBaseVals
{0x0F12,0xFE67, WORD_LEN, 0},	//70000D1D//ConstAfitBaseVals
{0x0F12,0x0000, WORD_LEN, 0},	//70000D1E//ConstAfitBaseVals

//==================================================================================
// 18.JPEG Thumnail Setting
//==================================================================================

//{0x002A,0x0478, WORD_LEN, 0},	
//{0x0F12,0x005F, WORD_LEN, 0},	//REG_TC_BRC_usPrevQuality    
//{0x0F12,0x005F, WORD_LEN, 0},	//REG_TC_BRC_usCaptureQuality 
//{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_THUMB_Thumb_bActive  
//{0x0F12,0x0280, WORD_LEN, 0},	//REG_TC_THUMB_Thumb_uWidth   
//{0x0F12,0x01E0, WORD_LEN, 0},	//REG_TC_THUMB_Thumb_uHeight  
//{0x0F12,0x0005, WORD_LEN, 0},	//REG_TC_THUMB_Thumb_Format   

//{0x002A,0x17DC, WORD_LEN, 0},	
//{0x0F12,0x0054, WORD_LEN, 0},	//jpeg_ManualMBCV                
//{0x002A,0x1AE4, WORD_LEN, 0},	                                 
//{0x0F12,0x001C, WORD_LEN, 0},	//senHal_bExtraAddLine           
//{0x002A,0x0284, WORD_LEN, 0},	                                 
//{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_GP_bBypassScalerJpg     
//{0x002A,0x028A, WORD_LEN, 0},	                                 
//{0x0F12,0x0000, WORD_LEN, 0},	//REG_TC_GP_bUse1FrameCaptureMode

//{0x002A,0x1CC2, WORD_LEN, 0},	//DRx_uDRxWeight for AutoCont function  
//{0x0F12,0x0100, WORD_LEN, 0},                                           
//{0x0F12,0x0100, WORD_LEN, 0},                                           
//{0x0F12,0x0100, WORD_LEN, 0},                                           
//{0x0F12,0x0100, WORD_LEN, 0},                                           

//==================================================================================
// 19.Select Cofigration Display
//==================================================================================
//PREVIEW                                                                        		
{0xFCFC,0xD000, WORD_LEN, 0},                                    		
{0x0028,0x7000, WORD_LEN, 0},                                		                               		
{0x002A,0x0266, WORD_LEN, 0},   
{0x0f12,0x0000, WORD_LEN, 0},	//REG_TC_GP_ActivePrevConfig        		
{0x002A,0x026A, WORD_LEN, 0},	                                   
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_GP_PrevOpenAfterChange     		
{0x002A,0x024E, WORD_LEN, 0},	                                   
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_GP_NewConfigSync           		
{0x002A,0x0268, WORD_LEN, 0},	                                   
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_GP_PrevConfigChanged       		
{0x002A,0x023E, WORD_LEN, 0},	                                   
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_GP_EnablePreview           	
{0x0F12,0x0001, WORD_LEN, 0},	//REG_TC_GP_EnablePreviewChanged 

{0xFFFF,0x0096, WORD_LEN, 150}, //p150
//===================================================================================
// 20.ISSUE
//===================================================================================

};

static const struct s5k4ecgx_i2c_reg_conf const s5k4ecgx_preview_cfg[] = {
	{0xFCFC,0xD000, WORD_LEN, 0}, // 
	{0x0028,0x7000, WORD_LEN, 0}, // 
	{0x002A,0x0266, WORD_LEN, 0}, // 
	{0x0F12,0x0000, WORD_LEN, 0}, // /#REG_TC_GP_ActivePrevConfig
	{0x0F12,0x0001, WORD_LEN, 0}, // /
	{0x002A,0x0242, WORD_LEN, 0}, // 
	{0x0F12,0x0000, WORD_LEN, 0}, // /#REG_TC_GP_EnableCapture
	{0x002A,0x024E, WORD_LEN, 0}, // 
	{0x0F12,0x0001, WORD_LEN, 0}, // /#REG_TC_GP_NewConfigSync
	{0x002A,0x0244, WORD_LEN, 0}, // 
	{0x0F12,0x0001, WORD_LEN, 150}, // /REG_TC_GP_EnableCaptureChanged
};

static const struct s5k4ecgx_i2c_reg_conf const s5k4ecgx_snapshot_cfg[] = {
	{0xFCFC,0xD000, WORD_LEN, 0}, // 
	{0x0028,0x7000, WORD_LEN, 0}, // 
	{0x002A,0x026E, WORD_LEN, 0}, // 
	{0x0F12,0x0000, WORD_LEN, 0}, // /#REG_TC_GP_ActiveCapConfig
	{0x0F12,0x0001, WORD_LEN, 0}, // /
	{0x002A,0x0242, WORD_LEN, 0}, // 
	{0x0F12,0x0001, WORD_LEN, 0}, // /#REG_TC_GP_EnableCapture
	{0x002A,0x024E, WORD_LEN, 0}, // 
	{0x0F12,0x0001, WORD_LEN, 0}, // /#REG_TC_GP_NewConfigSync
	{0x002A,0x0244, WORD_LEN, 0}, // 
	{0x0F12,0x0001, WORD_LEN, 150}, // /#REG_TC_GP_EnableCaptureChanged
};

static const struct s5k4ecgx_i2c_reg_conf const s5k4ecgx_autofocus_cfg[] = {
/*
//Infinity Position
	{0xFCFC,0xD000, WORD_LEN, 0},
	{0x0028,0x7000, WORD_LEN, 0},
	{0x002A,0x028E, WORD_LEN, 0}, 
	{0x0F12,0x0000, WORD_LEN, 0}, 
	{0x002A,0x028C, WORD_LEN, 0},
	{0x0F12,0x0004, WORD_LEN, 0},    
	{0xFFFF,0x0096, WORD_LEN, 150}, //P150  
	{0x002A,0x1648, WORD_LEN, 0},
	{0x0F12,0x9002, WORD_LEN, 0}, 
*/
	
	{0xFCFC,0xD000, WORD_LEN, 0}, // 
	{0x0028,0x7000, WORD_LEN, 150}, // 
	{0x002A,0x028E, WORD_LEN, 0}, // 
	{0x0F12,0x0000, WORD_LEN, 0}, // /#REG_TC_AF_AfCmdParam
	{0x002A,0x028C, WORD_LEN, 0}, // 
	{0x0F12,0x0005, WORD_LEN, 300}, // /#REG_TC_AF_AfCmd
// Read Register 70002EEE
	{0xFCFC,0xD000, WORD_LEN, 0},
	{0x002C,0x7000, WORD_LEN, 0},
	{0x2EEE,0xFFFF, POLLING_LEN, 0}, // SEQ_CMD
//	{0x0002,0x9614, POLLING_LEN, 0}, // SEQ_CMD
	{0x0002,0x9614, POLLING_LEN, 0}, // SEQ_CMD


// Read Register 70002007
	{0xFCFC,0xD000, WORD_LEN, 0},
	{0x002C,0x7000, WORD_LEN, 150},
	{0x2007,0xFF00, POLLING_LEN, 0}, // SEQ_CMD
//	{0x0000,0x9614, POLLING_LEN, 0}, // SEQ_CMD
	{0x0000,0x9614, POLLING_LEN, 0}, // SEQ_CMD
};


static const struct s5k4ecgx_i2c_reg_conf const s5k4ecgx_brightness_cfg[][S5K4ECGX_CFG_BRIGHT_MAX_PARAMS] = {
//EV_Minus_4
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1484, WORD_LEN, 0},
		{0x0F12,0x0024, WORD_LEN, 0},
	},
//EV_Minus_3
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1484, WORD_LEN, 0},
		{0x0F12,0x002A, WORD_LEN, 0},
	},
//EV_Minus_2
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1484, WORD_LEN, 0},
		{0x0F12,0x0030, WORD_LEN, 0},
	},
//EV_Minus_1
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1484, WORD_LEN, 0},
		{0x0F12,0x0036, WORD_LEN, 0},
	},
//EV_Default 
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1484, WORD_LEN, 0},
		{0x0F12,0x003C, WORD_LEN, 0},
	},
//EV_Plus_1
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1484, WORD_LEN, 0},
		{0x0F12,0x0047, WORD_LEN, 0},
	},
//EV_Plus_2
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1484, WORD_LEN, 0},
		{0x0F12,0x0052, WORD_LEN, 0},
	},
//EV_Plus_3
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1484, WORD_LEN, 0},
		{0x0F12,0x0057, WORD_LEN, 0},
	},
//EV_Plus_4
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1484, WORD_LEN, 0},
		{0x0F12,0x0068, WORD_LEN, 0},
	},
};

static const struct s5k4ecgx_i2c_reg_conf const s5k4ecgx_wb_cfg[][S5K4ECGX_CFG_WB_MAX_PARAMS] = {
//WB_Auto  
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x04E6, WORD_LEN, 0},
		{0x0F12,0x077F, WORD_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
	},
//WB_custom
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x04E6, WORD_LEN, 0},
		{0x0F12,0x077F, WORD_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
		{0x0000,0x0000, ZERO_LEN, 0},
	},
//WB_Tungsten
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x04E6, WORD_LEN, 0},
		{0x0F12,0x0777, WORD_LEN, 0},
		{0x002A,0x04BA, WORD_LEN, 0},
		{0x0F12,0x0430, WORD_LEN, 0},
		{0x002A,0x04BE, WORD_LEN, 0},
		{0x0F12,0x03D0, WORD_LEN, 0},
		{0x002A,0x04C2, WORD_LEN, 0},
		{0x0F12,0x09F0, WORD_LEN, 0},
		{0x002A,0x04C6, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
//WB_Fluorescent
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x04E6, WORD_LEN, 0}, 
		{0x0F12,0x0777, WORD_LEN, 0},
		{0x002A,0x04BA, WORD_LEN, 0},
		{0x0F12,0x0600, WORD_LEN, 0},
		{0x002A,0x04BE, WORD_LEN, 0},
		{0x0F12,0x0440, WORD_LEN, 0},
		{0x002A,0x04C2, WORD_LEN, 0},
		{0x0F12,0x09E0, WORD_LEN, 0},
		{0x002A,0x04C6, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
//WB_Sunny  
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x04E6, WORD_LEN, 0},
		{0x0F12,0x0777, WORD_LEN, 0},
		{0x002A,0x04BA, WORD_LEN, 0},
		{0x0F12,0x0620, WORD_LEN, 0},
		{0x002A,0x04BE, WORD_LEN, 0},
		{0x0F12,0x03D0, WORD_LEN, 0},
		{0x002A,0x04C2, WORD_LEN, 0},
		{0x0F12,0x0580, WORD_LEN, 0},
		{0x002A,0x04C6, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
//WB_Cloudy 
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x04E6, WORD_LEN, 0},
		{0x0F12,0x0777, WORD_LEN, 0},
		{0x002A,0x04BA, WORD_LEN, 0},
		{0x0F12,0x06D0, WORD_LEN, 0},
		{0x002A,0x04BE, WORD_LEN, 0},
		{0x0F12,0x03D0, WORD_LEN, 0},
		{0x002A,0x04C2, WORD_LEN, 0},
		{0x0F12,0x0520, WORD_LEN, 0},
		{0x002A,0x04C6, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
};

static const struct s5k4ecgx_i2c_reg_conf const s5k4ecgx_effect_cfg[][S5K4ECGX_CFG_EFFECT_MAX_PARAMS] ={
//Effect_Normal      
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x023C, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
	},
//Effect_Black_White(Mono)
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x023C, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
//Effect_Negative
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x023C, WORD_LEN, 0},
		{0x0F12,0x0003, WORD_LEN, 0},
	},
//Effect_Solarization
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x023C, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0},
	},
//Effect_Sepia
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x023C, WORD_LEN, 0},
		{0x0F12,0x0004, WORD_LEN, 0},
	},
// posterize 미지원
	{
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
	},
//whiteboard 미지원
	{
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
	},
// blackboard 미지원
	{
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
	},
// aqua 미지원
	{
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
		{0x0000, 0x0000, ZERO_LEN, 0},
	},
};

static const struct s5k4ecgx_i2c_reg_conf const s5k4ecgx_focus_step_settings_cfg[][S5K4ECGX_CFG_FOCUS_STEP_MAX_PARAMS] ={
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x028E, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x028C, WORD_LEN, 0},
		{0x0F12,0x0004, WORD_LEN, 0},
		{0xFFFF,0x0096, WORD_LEN, 150}, //P150  
		{0x002A,0x1648, WORD_LEN, 0},
		{0x0F12,0x9002, WORD_LEN, 0},
	},
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x028E, WORD_LEN, 0},
		{0x0F12,0x00D0, WORD_LEN, 0},
		{0x002A,0x028C, WORD_LEN, 0},
		{0x0F12,0x0004, WORD_LEN, 0},
		{0xFFFF,0x0096, WORD_LEN, 150}, //P150 
		{0x002A,0x1648, WORD_LEN, 0},
		{0x0F12,0x9002, WORD_LEN, 0},
	},
};

static const struct s5k4ecgx_i2c_reg_conf const s5k4ecgx_preview_fps_settings_cfg[][S5K4ECGX_CFG_PREVIEW_FPS_MAX_PARAMS] ={
// 7 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0594, WORD_LEN, 0},
		{0x0F12,0x0594, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 8 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x04E2, WORD_LEN, 0},
		{0x0F12,0x04E2, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 9 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0457, WORD_LEN, 0},
		{0x0F12,0x0457, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 10 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x03E8, WORD_LEN, 0},
		{0x0F12,0x03E8, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 11 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x038D, WORD_LEN, 0},
		{0x0F12,0x038D, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 12 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0341, WORD_LEN, 0},
		{0x0F12,0x0341, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
//13 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0301, WORD_LEN, 0},
		{0x0F12,0x0301, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 14 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x02CA, WORD_LEN, 0},
		{0x0F12,0x02CA, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 15 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x029A, WORD_LEN, 0},
		{0x0F12,0x029A, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 16 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0271, WORD_LEN, 0},
		{0x0F12,0x0271, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 17 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x024C, WORD_LEN, 0},
		{0x0F12,0x024C, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
//18 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x022B, WORD_LEN, 0},
		{0x0F12,0x022B, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 19 fps
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x020E, WORD_LEN, 0},
		{0x0F12,0x020E, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 20 pfs
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x01F4, WORD_LEN, 0},
		{0x0F12,0x01F4, WORD_LEN, 0},

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
// 21 fps
	{
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},

		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
	},
// 22 fps
	{
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},

		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
	},
// 23 fps
	{
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},

		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
	},
// 24 fps
	{
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},

		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
	},
// 25 fps
	{
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},

		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
	},
// 26 fps
	{
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},

		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
	},
// 27 fps
	{
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},

		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
	},
// 28 fps
	{
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},

		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
	},
// 29 fps
	{
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},

		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
	},
// 30 fps ( variable 5~20fps code )
	{
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x02B4, WORD_LEN, 0}, 
		{0x0F12,0x0002, WORD_LEN, 0}, //REG_0TC_PCFG_PVIMask
		{0x002A,0x02BE, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0535, WORD_LEN, 0}, //20fps
		{0x0F12,0x01F4, WORD_LEN, 0}, //7.5fps

		{0x002A,0x0266, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x026A, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x024E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x0268, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
	},
};


struct s5k4ecgx_reg s5k4ecgx_regs = {

	.init_parm = s5k4ecgx_init_parm,
	.init_parm_size = ARRAY_SIZE(s5k4ecgx_init_parm),

	.preview_cfg_settings = s5k4ecgx_preview_cfg,
	.preview_cfg_settings_size = ARRAY_SIZE(s5k4ecgx_preview_cfg),

	.snapshot_cfg_settings = s5k4ecgx_snapshot_cfg,
	.snapshot_cfg_settings_size = ARRAY_SIZE(s5k4ecgx_snapshot_cfg),

	.autofocus_cfg_settings = s5k4ecgx_autofocus_cfg,
	.autofocus_cfg_settings_size = ARRAY_SIZE(s5k4ecgx_autofocus_cfg),

	.bright_cfg_settings = s5k4ecgx_brightness_cfg,
	.bright_cfg_settings_size = S5K4ECGX_CFG_BRIGHT_MAX_PARAMS,

	.wb_cfg_settings = s5k4ecgx_wb_cfg,
	.wb_cfg_settings_size = S5K4ECGX_CFG_WB_MAX_PARAMS,

	.effect_cfg_settings = s5k4ecgx_effect_cfg,
	.effect_cfg_settings_size = S5K4ECGX_CFG_EFFECT_MAX_PARAMS,

	.focus_step_cfg_settings =s5k4ecgx_focus_step_settings_cfg,
	.focus_step_cfg_settings_size = S5K4ECGX_CFG_FOCUS_STEP_MAX_PARAMS,

//preview frame rate
#if 0
	.preview_fps_cfg_settings = s5k4ecgx_preview_fps_settings_cfg,
	.preview_fps_cfg_settings_size = S5K4ECGX_CFG_PREVIEW_FPS_MAX_PARAMS,
#endif
};
