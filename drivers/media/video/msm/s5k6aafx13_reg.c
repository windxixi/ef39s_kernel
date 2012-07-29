/* Copyright (c) 2010, PANTECH. All rights reserved.
 */
 
#include "s5k6aafx13.h"

/* register configration */
static const struct s5k6aafx13_i2c_reg_conf s5k6aafx13_init_parm[] =
{		
#ifdef BURST_MODE_INIT//20110512 tune
    {0x0010,0x0001, WORD_LEN, 0}, // Re{0xet                                               //
    {0x0004,0x0000, WORD_LEN, 0}, // Di{0xable Auto Addre{0x{0x Increment : 0 Chunghwan Park   //
    {0x1030,0x0000, WORD_LEN, 0}, // Clear ho{0xt interrupt {0xo main will wait              //
    {0x0014,0x0001, WORD_LEN, 100}, // ARM go                                              //
    //{0xFFFF,0x0064, WORD_LEN, 0}, //p100 
    {0xFCFC,0xD000, WORD_LEN, 0},
    {0x0004,0x0001, WORD_LEN, 0},   // en{0xable Auto Addre{0x{0x Increment : 1 //
    {0x0028,0x7000, WORD_LEN, 0},
    {0x002A,0x1D60, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_1, 0},
    ///////////////////////////////////////////////////////    
    {0x0004,0x0000, WORD_LEN, 0}, //  2. Analog {0xetting   //
    {0xF454,0x0001, WORD_LEN, 0}, // ADC {0xat = 750mV(50h), NTG = -0.8V(10h), {0xaturation margin low limit = 732L{0xB //
    {0xF418,0x0050, WORD_LEN, 0},
    {0xF43E,0x0010, WORD_LEN, 0},
    {0x0004,0x0001, WORD_LEN, 0}, //Di{0xable Auto Addre{0x{0x Increment : 1//
    {0x002A,0x112A, WORD_LEN, 0}, // Analog {0xetting{0x // //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[8]   //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x1132, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[12]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x113E, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[18]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x115C, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[33]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x1164, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[37]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x1174, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[45]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x1178, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[47]  //
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x077A, WORD_LEN, 0}, // m{0xm_uOff{0xetNoBin //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_2, 0},
    ///////////////////////////////////////////////////////    
    {0x002A,0x07A2, WORD_LEN, 0}, // m{0xm_{0xAnalogOff{0xet //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_3, 0},
    ///////////////////////////////////////////////////////
    {0x002A,0x07B6, WORD_LEN, 0}, // m{0xm_NonLinearOf{0xOutput //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_4, 0},
    ///////////////////////////////////////////////////////  
    {0x0028,0x7000, WORD_LEN, 0}, //E{0xD Check Code add_100505//
    {0x002A,0x0132, WORD_LEN, 0},
    {0x0F12,0xAAAA, WORD_LEN, 0},
    {0x002A,0x1000, WORD_LEN, 0}, // 3. AE & AE weight ////TVAR_ae_BrAve                                    //
    {0x0F12,0x003C, WORD_LEN, 0}, //35  //
    {0x002A,0x0474, WORD_LEN, 0},
    {0x0F12,0x0112, WORD_LEN, 0}, //lt_uLimitHigh    //
    {0x0F12,0x00EF, WORD_LEN, 0}, //lt_uLimitLow     //
    {0x002A,0x1006, WORD_LEN, 0},
    {0x0F12,0x001F, WORD_LEN, 0}, //ae_{0xtatMode//
    {0x002A,0x108E, WORD_LEN, 0}, //{0xARR_IllumType//
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_5, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0488, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_6, 0},
    ///////////////////////////////////////////////////////      
    {0x002A,0x2360, WORD_LEN, 0}, //2174 AWBBTune_EVT4_uMaxExp3      //
    {0x0F12,0xF424, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_7, 0},
    ///////////////////////////////////////////////////////      
    {0x002A,0x2364, WORD_LEN, 0}, //2178 AWBBTune_EVT4_uCapMaxExp3   //
    {0x0F12,0xF424, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0498, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux {0xhutter       //
    {0x0F12,0x0310, WORD_LEN, 0}, //lt_uMaxAnGain2                                     //
    {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
    {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
    {0x002A,0x049C, WORD_LEN, 0}, //                                                    //
    {0x0F12,0x0100, WORD_LEN, 0}, //lt_uMaxDigGain                                      //
    {0x002A,0x235C, WORD_LEN, 0}, //002A2170
    {0x0F12,0x0002, WORD_LEN, 0},  //1 #AWBBTune_EVT4_uMinCoar{0xe
    {0x0F12,0x0090, WORD_LEN, 0},  //#AWBBTune_EVT4_uMinFine
    {0x002A,0x100e, WORD_LEN, 0}, //ae_WeightTbl_16//
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_8, 0},
    ///////////////////////////////////////////////////////          
    {0x002A,0x0f4c, WORD_LEN, 0}, //brightne{0x{0x //
    {0x0F12,0x02b0, WORD_LEN, 0}, //180,       //
    {0x002A,0x0f52, WORD_LEN, 0}, //           //
    {0x0F12,0x02f0, WORD_LEN, 0}, //180,       //
    {0x002A,0x0754, WORD_LEN, 0}, // 4. {0xhading (GA{0x) // //TVAR_a{0xh_pGA{0x//
    {0x0F12,0x247C, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x002A,0x247C, WORD_LEN, 0}, //    param_{0xtart   TVAR_a{0xh_pGA{0x //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_9, 0},
    ///////////////////////////////////////////////////////      
    {0x002A,0x0704, WORD_LEN, 0}, // 5. {0xhading Alpha //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_10, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x075A, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_11, 0},
    ///////////////////////////////////////////////////////      
    {0x002A,0x04C8, WORD_LEN, 0}, // 6. Gamma // // param_{0xtart   {0xARR_u{0xGammaLutRGBIndoor //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_12, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0C50, WORD_LEN, 0}, // 7. AWB // // param_{0xtart awbb_IndoorGrZone{0x_m_BGrid //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_13, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0CF8, WORD_LEN, 0}, 
    {0x0F12,0x010F, WORD_LEN, 0}, //awbb_IndoorGrZone{0x_m_Boff{0x//
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0D84, WORD_LEN, 0}, // param_{0xtart  awbb_LowBrGrZone{0x_m_BGrid //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_14, 0},
    ///////////////////////////////////////////////////////      
    {0x002A,0x0DF0, WORD_LEN, 0},
    {0x0F12,0x0081, WORD_LEN, 0}, //awbb_LowBrGrZone{0x_m_Boff{0x//
    {0x0F12,0x0000, WORD_LEN, 0}, // param_{0xtart  awbb_OutdoorGrZone{0x_m_BGrid //
    {0x002A,0x0D08, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_15, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0D70, WORD_LEN, 0},
    {0x0F12,0x000F, WORD_LEN, 0}, //0011//
    {0x002A,0x0D74, WORD_LEN, 0},
    {0x0F12,0x0207, WORD_LEN, 0}, //1f //2F, awbb_OutdoorGrZone{0x_m_Boff{0x//
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0E00, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_16, 0},
    ///////////////////////////////////////////////////////      
    {0x002A,0x0E1A, WORD_LEN, 0},
    {0x0F12,0x012F, WORD_LEN, 0},
    {0x0F12,0x0120, WORD_LEN, 0},
    {0x002A,0x0E68, WORD_LEN, 0}, // awbb_LowTempRB //
    {0x0F12,0x04F2, WORD_LEN, 0},
    {0x002A,0x0D78, WORD_LEN, 0},
    {0x0F12,0x0020, WORD_LEN, 0}, //AWB min.                                       //
    {0x002A,0x0D80, WORD_LEN, 0},
    {0x0F12,0x00E0, WORD_LEN, 0}, //AWB Max.                                       //
    {0x002A,0x0E40, WORD_LEN, 0}, //awbb_U{0xe_Filter{0x                            //
    {0x0F12,0x0061, WORD_LEN, 0}, //AWB option                                 //
    {0x002A,0x0EE4, WORD_LEN, 0},
    {0x0F12,0x0003, WORD_LEN, 0}, //awbb_OutdoorFltr{0xz                          //
    {0x002A,0x0E3C, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0}, //awbb_U{0xe_InvalidOutDoor                     //
    {0x002A,0x0F3A, WORD_LEN, 0},
    {0x0F12,0x024C, WORD_LEN, 0}, //awbb_OutdoorWP_r       //
    {0x0F12,0x0290, WORD_LEN, 0}, //awbb_OutdoorWP_b       //
    {0x002A,0x0E46, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_17, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0E5E, WORD_LEN, 0},
    {0x0F12,0x071A, WORD_LEN, 0}, //awbb_GamutWidthThr1                         //
    {0x0F12,0x03A4, WORD_LEN, 0},
    {0x002A,0x0E50, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_18, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0E36, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0}, //awbb_ByPa{0x{0x_LowTempMode                     //  
    {0x002a,0x0e18, WORD_LEN, 0},
    {0x0f12,0x0000, WORD_LEN, 0}, // 32 awbb_dark  //
    {0x002A,0x0E3A, WORD_LEN, 0}, // AWB etc //
    {0x0F12,0x02C2, WORD_LEN, 0}, //awbb_Alpha_Comp_Mode                   //
    {0x002A,0x0F12, WORD_LEN, 0},
    {0x0F12,0x02C9, WORD_LEN, 0}, //awbb_GLocu{0xR                           //
    {0x0F12,0x033F, WORD_LEN, 0}, //awbb_GLocu{0xB                           //
    {0x002A,0x0E1A, WORD_LEN, 0},
    {0x0F12,0x0138, WORD_LEN, 0}, //awbb_IntcR                             //
    {0x002A,0x236C, WORD_LEN, 0}, //2180//
    {0x0F12,0x0000, WORD_LEN, 0}, //AWBBTune_EVT4_uInitPo{0xtToleranceCnt    //
    {0x002A,0x0c48, WORD_LEN, 0}, // AWB {0xtart Point // //awbb_Gain{0xInit //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_19, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0E42, WORD_LEN, 0}, // 8. Grid Correction //
    {0x0F12,0x0002, WORD_LEN, 0},
    {0x002A,0x0EE0, WORD_LEN, 0},
    {0x0F12,0x00B5, WORD_LEN, 0}, //awbb_GridCoeff_R_2                                          //
    {0x0F12,0x00B5, WORD_LEN, 0}, //awbb_GridCoeff_B_2                                          //
    {0x002A,0x0ED0, WORD_LEN, 0}, //                                                            //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_20, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0ECA, WORD_LEN, 0}, //                                                            //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_21, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0E82, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_22, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x06D4, WORD_LEN, 0}, // 9. CCM //
    {0x0F12,0x2380, WORD_LEN, 0}, //TVAR_wbt_pOutdoorCcm//
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x002A,0x06CC, WORD_LEN, 0},
    {0x0F12,0x23A4, WORD_LEN, 0}, //TVAR_wbt_pBa{0xeCcm{0x  //
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x002A,0x06E8, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_23, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x06DA, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_24, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x23A4, WORD_LEN, 0}, //    par,0xam_{},0xtart  TVAR_wbt_pBa{0xeCcm{0x //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_25, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x2380, WORD_LEN, 0}, //    par,0xam_{},0xtart  TVAR_wbt_pOutdoorCcm
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_26, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x07E8, WORD_LEN, 0}, // 10. AFIT // // param_{0xtart   {0xARR_uNormBrInDoor //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_27, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x07D0, WORD_LEN, 0}, //    param_{0xtart   afit_uNoi{0xeIndInDoor //
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_28, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x07E6, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0}, //afit_bU{0xeNoi{0xeInd //
    {0x002A,0x0828, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_29, 0},
    ///////////////////////////////////////////////////////  
    {0x1000,0x0001, WORD_LEN, 10}, // 11. Flicker, CLK //  //{0xet ho{0xt interrupt {0xo main {0xtart run //
    //{0xFFFF,0x000A, WORD_LEN, 0}, //p10 //W,0xait1},0m{0xec                           //
    {0x002A,0x0400, WORD_LEN, 0},// End ,0xu{0x},er init {0xcript
    {0x0F12,0x007F, WORD_LEN, 0}, //5f
    {0x002A,0x03DC, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},//clk {,0x0xet},ting{0x
    {0x002A,0x01B8, WORD_LEN, 0},
    {0x0F12,0x5DC0, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x01C6, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0}, //pvi clock
    {0x0F12,0x0001, WORD_LEN, 0}, //mipi clock
    {0x002A,0x01CC, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_30, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x01E0, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 100},
    //{0xFFFF,0x0064, WORD_LEN, 0}, //p100   
    {0x002A,0x0242, WORD_LEN, 0},//PREVI,0xEW C},ONFIGURATION 0 ({0xXGA, YUV, 15fp{0x)
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_31, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x024E, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0}, //clock 1 사용
    {0x002A,0x0248, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_32, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0252, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x0250, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0}, 
    {0x002A,0x0262, WORD_LEN, 0},
    {0x0F12,0x0003, WORD_LEN, 0}, //REG_0TC_PCFG_uPrevMirror   [0]x [1]y [2]xy                                 //
    {0x0F12,0x0003, WORD_LEN, 0}, //REG_0TC_PCFG_uCaptureMirror 
    {0x002A,0x0254, WORD_LEN, 0},
    {0x0F12,0x0535, WORD_LEN, 0},
    {0x0F12,0x029A, WORD_LEN, 0},
    {0x002A,0x0268, WORD_LEN, 0}, //42
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_33, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0274, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x026E, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_34, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x0278, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x0276, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},
    {0x002A,0x0288, WORD_LEN, 0},
    {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
    {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
    {0x002A,0x027A, WORD_LEN, 0},
    {0x0F12,0x0594, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x030E, WORD_LEN, 0}, //CAPTU,0xRE C},ONFIGURATION 0 
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_35, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x031A, WORD_LEN, 0},                        
    {0x0F12,0x0001, WORD_LEN, 0},  //REG_0TC_CCFG_uClockInd
    {0x002A,0x0314, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_36, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x031E, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},  //REG_0TC_CCFG_FrRateQualityType
    {0x002A,0x031C, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},  //REG_0TC_CCFG_u{0xFrTimeType
    {0x002A,0x0320, WORD_LEN, 0},
    {0x0F12,0x0594, WORD_LEN, 0},  //4e2 535 REG_0TC_CCFG_u{0xMaxFrTimeM{0xecMult10 //Don't change!
    {0x0F12,0x0000, WORD_LEN, 0},  //REG_0TC_CCFG_u{0xMinFrTimeM{0xecMult10 //Don't change!
    {0x002A,0x0226, WORD_LEN, 0},     //REG_TC_GP_CapConfigChanged             
    {0x0F12,0x0001, WORD_LEN, 0},   
    {0x002A,0x10EE, WORD_LEN, 0}, //{0xenHal_uMinCol{0xNoBin *FACTORY ONLY *No Delete//
    {0x0F12,0x097a, WORD_LEN, 0}, //886 //97a //097a //0886 //97a REG_TC_GP_Input{0xChangeReque{0xt/REG_TC_GP_PrevConfigChanged/REG_TC_GP_CapConfigChanged //
    {0x002A,0x03B6, WORD_LEN, 0}, //mipi
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x03FA, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_37, 0},
    ///////////////////////////////////////////////////////  
    {0x002A,0x021C, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0220, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x01F8, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x021E, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x01F0, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x01FA, WORD_LEN, 0}, // Input Crop   
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_38, 0},
    ///////////////////////////////////////////////////////      
    {0x002A,0x020A, WORD_LEN, 0},
    ///////////////////////////////////////////////////////
    { 0, 0, BURST_39, 0},
    /////////////////////////////////////////////////////// 
    {0xFFFF,0x0064, WORD_LEN, 100} //p100
#else
#if (1)//update
    {0x0010,0x0001, WORD_LEN, 0}, // Re{0xet                                               //
    {0x0004,0x0000, WORD_LEN, 0}, // Di{0xable Auto Addre{0x{0x Increment : 0 Chunghwan Park   //
    {0x1030,0x0000, WORD_LEN, 0}, // Clear ho{0xt interrupt {0xo main will wait              //
    {0x0014,0x0001, WORD_LEN, 100}, // ARM go                                              //
    //{0xFFFF,0x0064, WORD_LEN, 0}, //p100 
    {0xFCFC,0xD000, WORD_LEN, 0},
    {0x0004,0x0001, WORD_LEN, 0},	// en{0xable Auto Addre{0x{0x Increment : 1 //
    {0x0028,0x7000, WORD_LEN, 0},
    {0x002A,0x1D60, WORD_LEN, 0},
    {0x0F12,0xB570, WORD_LEN, 0},
    {0x0F12,0x4936, WORD_LEN, 0},
    {0x0F12,0x4836, WORD_LEN, 0},
    {0x0F12,0x2205, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA4E, WORD_LEN, 0},
    {0x0F12,0x4935, WORD_LEN, 0},
    {0x0F12,0x2002, WORD_LEN, 0},
    {0x0F12,0x83C8, WORD_LEN, 0},
    {0x0F12,0x2001, WORD_LEN, 0},
    {0x0F12,0x3120, WORD_LEN, 0},
    {0x0F12,0x8088, WORD_LEN, 0},
    {0x0F12,0x4933, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0},
    {0x0F12,0x8008, WORD_LEN, 0},
    {0x0F12,0x4933, WORD_LEN, 0},
    {0x0F12,0x8048, WORD_LEN, 0},
    {0x0F12,0x4933, WORD_LEN, 0},
    {0x0F12,0x4833, WORD_LEN, 0},
    {0x0F12,0x2204, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA3E, WORD_LEN, 0},
    {0x0F12,0x4932, WORD_LEN, 0},
    {0x0F12,0x4833, WORD_LEN, 0},
    {0x0F12,0x2206, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA39, WORD_LEN, 0},
    {0x0F12,0x4932, WORD_LEN, 0},
    {0x0F12,0x4832, WORD_LEN, 0},
    {0x0F12,0x2207, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA34, WORD_LEN, 0},
    {0x0F12,0x4931, WORD_LEN, 0},
    {0x0F12,0x4832, WORD_LEN, 0},
    {0x0F12,0x2208, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA2F, WORD_LEN, 0},
    {0x0F12,0x4931, WORD_LEN, 0},
    {0x0F12,0x4831, WORD_LEN, 0},
    {0x0F12,0x2209, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA2A, WORD_LEN, 0},
    {0x0F12,0x4930, WORD_LEN, 0},
    {0x0F12,0x4831, WORD_LEN, 0},
    {0x0F12,0x220A, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA25, WORD_LEN, 0},
    {0x0F12,0x4930, WORD_LEN, 0},
    {0x0F12,0x4830, WORD_LEN, 0},
    {0x0F12,0x220B, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA20, WORD_LEN, 0},
    {0x0F12,0x482F, WORD_LEN, 0},
    {0x0F12,0x4930, WORD_LEN, 0},
    {0x0F12,0x6108, WORD_LEN, 0},
    {0x0F12,0x4830, WORD_LEN, 0},
    {0x0F12,0x39FF, WORD_LEN, 0},
    {0x0F12,0x3901, WORD_LEN, 0},
    {0x0F12,0x6748, WORD_LEN, 0},
    {0x0F12,0x482F, WORD_LEN, 0},
    {0x0F12,0x1C0A, WORD_LEN, 0},
    {0x0F12,0x32C0, WORD_LEN, 0},
    {0x0F12,0x6390, WORD_LEN, 0},
    {0x0F12,0x482E, WORD_LEN, 0},
    {0x0F12,0x6708, WORD_LEN, 0},
    {0x0F12,0x491A, WORD_LEN, 0},
    {0x0F12,0x482D, WORD_LEN, 0},
    {0x0F12,0x3108, WORD_LEN, 0},
    {0x0F12,0x60C1, WORD_LEN, 0},
    {0x0F12,0x6882, WORD_LEN, 0},
    {0x0F12,0x1A51, WORD_LEN, 0},
    {0x0F12,0x8201, WORD_LEN, 0},
    {0x0F12,0x4C2B, WORD_LEN, 0},
    {0x0F12,0x2607, WORD_LEN, 0},
    {0x0F12,0x6821, WORD_LEN, 0},
    {0x0F12,0x0736, WORD_LEN, 0},
    {0x0F12,0x42B1, WORD_LEN, 0},
    {0x0F12,0xDA05, WORD_LEN, 0},
    {0x0F12,0x4829, WORD_LEN, 0},
    {0x0F12,0x22D8, WORD_LEN, 0},
    {0x0F12,0x1C05, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA09, WORD_LEN, 0},
    {0x0F12,0x6025, WORD_LEN, 0},
    {0x0F12,0x68A1, WORD_LEN, 0},
    {0x0F12,0x42B1, WORD_LEN, 0},
    {0x0F12,0xDA07, WORD_LEN, 0},
    {0x0F12,0x4825, WORD_LEN, 0},
    {0x0F12,0x2224, WORD_LEN, 0},
    {0x0F12,0x3824, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xFA00, WORD_LEN, 0},
    {0x0F12,0x4822, WORD_LEN, 0},
    {0x0F12,0x3824, WORD_LEN, 0},
    {0x0F12,0x60A0, WORD_LEN, 0},
    {0x0F12,0x4D22, WORD_LEN, 0},
    {0x0F12,0x6D29, WORD_LEN, 0},
    {0x0F12,0x42B1, WORD_LEN, 0},
    {0x0F12,0xDA07, WORD_LEN, 0},
    {0x0F12,0x481F, WORD_LEN, 0},
    {0x0F12,0x228F, WORD_LEN, 0},
    {0x0F12,0x00D2, WORD_LEN, 0},
    {0x0F12,0x30D8, WORD_LEN, 0},
    {0x0F12,0x1C04, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF9F2, WORD_LEN, 0},
    {0x0F12,0x652C, WORD_LEN, 0},
    {0x0F12,0xBC70, WORD_LEN, 0},
    {0x0F12,0xBC08, WORD_LEN, 0},
    {0x0F12,0x4718, WORD_LEN, 0},
    {0x0F12,0x218B, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x127B, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0398, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x1376, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x2370, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x1F0D, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x890D, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x1F2F, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x27A9, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x1FE1, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x27C5, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x2043, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x285F, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x2003, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x28FF, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x20CD, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x6181, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x20EF, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x6663, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x2123, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x0100, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x1EC1, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x1EAD, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x1F79, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x04AC, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x06CC, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x23A4, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x0704, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0xB510, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF9B9, WORD_LEN, 0},
    {0x0F12,0x48C3, WORD_LEN, 0},
    {0x0F12,0x49C3, WORD_LEN, 0},
    {0x0F12,0x8800, WORD_LEN, 0},
    {0x0F12,0x8048, WORD_LEN, 0},
    {0x0F12,0xBC10, WORD_LEN, 0},
    {0x0F12,0xBC08, WORD_LEN, 0},
    {0x0F12,0x4718, WORD_LEN, 0},
    {0x0F12,0xB5F8, WORD_LEN, 0},
    {0x0F12,0x1C06, WORD_LEN, 0},
    {0x0F12,0x4DC0, WORD_LEN, 0},
    {0x0F12,0x68AC, WORD_LEN, 0},
    {0x0F12,0x1C30, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF9B3, WORD_LEN, 0},
    {0x0F12,0x68A9, WORD_LEN, 0},
    {0x0F12,0x4ABC, WORD_LEN, 0},
    {0x0F12,0x42A1, WORD_LEN, 0},
    {0x0F12,0xD003, WORD_LEN, 0},
    {0x0F12,0x4BBD, WORD_LEN, 0},
    {0x0F12,0x8A1B, WORD_LEN, 0},
    {0x0F12,0x3301, WORD_LEN, 0},
    {0x0F12,0x8013, WORD_LEN, 0},
    {0x0F12,0x8813, WORD_LEN, 0},
    {0x0F12,0x1C14, WORD_LEN, 0},
    {0x0F12,0x2B00, WORD_LEN, 0},
    {0x0F12,0xD00F, WORD_LEN, 0},
    {0x0F12,0x2201, WORD_LEN, 0},
    {0x0F12,0x4281, WORD_LEN, 0},
    {0x0F12,0xD003, WORD_LEN, 0},
    {0x0F12,0x8C2F, WORD_LEN, 0},
    {0x0F12,0x42B9, WORD_LEN, 0},
    {0x0F12,0xD300, WORD_LEN, 0},
    {0x0F12,0x2200, WORD_LEN, 0},
    {0x0F12,0x60AE, WORD_LEN, 0},
    {0x0F12,0x2A00, WORD_LEN, 0},
    {0x0F12,0xD003, WORD_LEN, 0},
    {0x0F12,0x8C28, WORD_LEN, 0},
    {0x0F12,0x42B0, WORD_LEN, 0},
    {0x0F12,0xD800, WORD_LEN, 0},
    {0x0F12,0x1C30, WORD_LEN, 0},
    {0x0F12,0x1E59, WORD_LEN, 0},
    {0x0F12,0x8021, WORD_LEN, 0},
    {0x0F12,0xBCF8, WORD_LEN, 0},
    {0x0F12,0xBC08, WORD_LEN, 0},
    {0x0F12,0x4718, WORD_LEN, 0},
    {0x0F12,0xB510, WORD_LEN, 0},
    {0x0F12,0x1C04, WORD_LEN, 0},
    {0x0F12,0x48AF, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF997, WORD_LEN, 0},
    {0x0F12,0x4AAD, WORD_LEN, 0},
    {0x0F12,0x4BAE, WORD_LEN, 0},
    {0x0F12,0x8811, WORD_LEN, 0},
    {0x0F12,0x885B, WORD_LEN, 0},
    {0x0F12,0x8852, WORD_LEN, 0},
    {0x0F12,0x4359, WORD_LEN, 0},
    {0x0F12,0x1889, WORD_LEN, 0},
    {0x0F12,0x4288, WORD_LEN, 0},
    {0x0F12,0xD800, WORD_LEN, 0},
    {0x0F12,0x1C08, WORD_LEN, 0},
    {0x0F12,0x6020, WORD_LEN, 0},
    {0x0F12,0xE7C5, WORD_LEN, 0},
    {0x0F12,0xB570, WORD_LEN, 0},
    {0x0F12,0x1C05, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF98F, WORD_LEN, 0},
    {0x0F12,0x49A5, WORD_LEN, 0},
    {0x0F12,0x8989, WORD_LEN, 0},
    {0x0F12,0x4348, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0},
    {0x0F12,0x0C00, WORD_LEN, 0},
    {0x0F12,0x2101, WORD_LEN, 0},
    {0x0F12,0x0349, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF98E, WORD_LEN, 0},
    {0x0F12,0x1C04, WORD_LEN, 0},
    {0x0F12,0x489F, WORD_LEN, 0},
    {0x0F12,0x8F80, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF991, WORD_LEN, 0},
    {0x0F12,0x1C01, WORD_LEN, 0},
    {0x0F12,0x20FF, WORD_LEN, 0},
    {0x0F12,0x43C0, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF994, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF998, WORD_LEN, 0},
    {0x0F12,0x1C01, WORD_LEN, 0},
    {0x0F12,0x4898, WORD_LEN, 0},
    {0x0F12,0x8840, WORD_LEN, 0},
    {0x0F12,0x4360, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0},
    {0x0F12,0x0C00, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF97A, WORD_LEN, 0},
    {0x0F12,0x6028, WORD_LEN, 0},
    {0x0F12,0xBC70, WORD_LEN, 0},
    {0x0F12,0xBC08, WORD_LEN, 0},
    {0x0F12,0x4718, WORD_LEN, 0},
    {0x0F12,0xB5F1, WORD_LEN, 0},
    {0x0F12,0xB082, WORD_LEN, 0},
    {0x0F12,0x4D96, WORD_LEN, 0},
    {0x0F12,0x4E91, WORD_LEN, 0},
    {0x0F12,0x88A8, WORD_LEN, 0},
    {0x0F12,0x1C2C, WORD_LEN, 0},
    {0x0F12,0x3420, WORD_LEN, 0},
    {0x0F12,0x4F90, WORD_LEN, 0},
    {0x0F12,0x2800, WORD_LEN, 0},
    {0x0F12,0xD018, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF988, WORD_LEN, 0},
    {0x0F12,0x9001, WORD_LEN, 0},
    {0x0F12,0x9802, WORD_LEN, 0},
    {0x0F12,0x6B39, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF974, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF978, WORD_LEN, 0},
    {0x0F12,0x9901, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF95F, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0x8871, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF96A, WORD_LEN, 0},
    {0x0F12,0x0400, WORD_LEN, 0},
    {0x0F12,0x0C00, WORD_LEN, 0},
    {0x0F12,0x21FF, WORD_LEN, 0},
    {0x0F12,0x3101, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF97A, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0x88E8, WORD_LEN, 0},
    {0x0F12,0x2800, WORD_LEN, 0},
    {0x0F12,0xD00A, WORD_LEN, 0},
    {0x0F12,0x4980, WORD_LEN, 0},
    {0x0F12,0x8820, WORD_LEN, 0},
    {0x0F12,0x3128, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF979, WORD_LEN, 0},
    {0x0F12,0x8D38, WORD_LEN, 0},
    {0x0F12,0x8871, WORD_LEN, 0},
    {0x0F12,0x4348, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0},
    {0x0F12,0x0C00, WORD_LEN, 0},
    {0x0F12,0x8538, WORD_LEN, 0},
    {0x0F12,0xBCFE, WORD_LEN, 0},
    {0x0F12,0xBC08, WORD_LEN, 0},
    {0x0F12,0x4718, WORD_LEN, 0},
    {0x0F12,0xB510, WORD_LEN, 0},
    {0x0F12,0x1C04, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF974, WORD_LEN, 0},
    {0x0F12,0x6821, WORD_LEN, 0},
    {0x0F12,0x0409, WORD_LEN, 0},
    {0x0F12,0x0C09, WORD_LEN, 0},
    {0x0F12,0x1A40, WORD_LEN, 0},
    {0x0F12,0x4976, WORD_LEN, 0},
    {0x0F12,0x6849, WORD_LEN, 0},
    {0x0F12,0x4281, WORD_LEN, 0},
    {0x0F12,0xD800, WORD_LEN, 0},
    {0x0F12,0x1C08, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF971, WORD_LEN, 0},
    {0x0F12,0x6020, WORD_LEN, 0},
    {0x0F12,0xE75B, WORD_LEN, 0},
    {0x0F12,0xB570, WORD_LEN, 0},
    {0x0F12,0x6801, WORD_LEN, 0},
    {0x0F12,0x040D, WORD_LEN, 0},
    {0x0F12,0x0C2D, WORD_LEN, 0},
    {0x0F12,0x6844, WORD_LEN, 0},
    {0x0F12,0x486F, WORD_LEN, 0},
    {0x0F12,0x8981, WORD_LEN, 0},
    {0x0F12,0x1C28, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF927, WORD_LEN, 0},
    {0x0F12,0x8060, WORD_LEN, 0},
    {0x0F12,0x4970, WORD_LEN, 0},
    {0x0F12,0x69C9, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF968, WORD_LEN, 0},
    {0x0F12,0x1C01, WORD_LEN, 0},
    {0x0F12,0x80A0, WORD_LEN, 0},
    {0x0F12,0x0228, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF92D, WORD_LEN, 0},
    {0x0F12,0x0400, WORD_LEN, 0},
    {0x0F12,0x0C00, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0x496B, WORD_LEN, 0},
    {0x0F12,0x2300, WORD_LEN, 0},
    {0x0F12,0x5EC9, WORD_LEN, 0},
    {0x0F12,0x4288, WORD_LEN, 0},
    {0x0F12,0xDA02, WORD_LEN, 0},
    {0x0F12,0x20FF, WORD_LEN, 0},
    {0x0F12,0x3001, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0xE797, WORD_LEN, 0},
    {0x0F12,0xB5F8, WORD_LEN, 0},
    {0x0F12,0x1C04, WORD_LEN, 0},
    {0x0F12,0x4867, WORD_LEN, 0},
    {0x0F12,0x4E65, WORD_LEN, 0},
    {0x0F12,0x7800, WORD_LEN, 0},
    {0x0F12,0x6AB7, WORD_LEN, 0},
    {0x0F12,0x2800, WORD_LEN, 0},
    {0x0F12,0xD100, WORD_LEN, 0},
    {0x0F12,0x6A37, WORD_LEN, 0},
    {0x0F12,0x495D, WORD_LEN, 0},
    {0x0F12,0x2800, WORD_LEN, 0},
    {0x0F12,0x688D, WORD_LEN, 0},
    {0x0F12,0xD100, WORD_LEN, 0},
    {0x0F12,0x684D, WORD_LEN, 0},
    {0x0F12,0x4859, WORD_LEN, 0},
    {0x0F12,0x8841, WORD_LEN, 0},
    {0x0F12,0x6820, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF94B, WORD_LEN, 0},
    {0x0F12,0x8DF1, WORD_LEN, 0},
    {0x0F12,0x434F, WORD_LEN, 0},
    {0x0F12,0x0A3A, WORD_LEN, 0},
    {0x0F12,0x4282, WORD_LEN, 0},
    {0x0F12,0xD30C, WORD_LEN, 0},
    {0x0F12,0x4D5C, WORD_LEN, 0},
    {0x0F12,0x26FF, WORD_LEN, 0},
    {0x0F12,0x8829, WORD_LEN, 0},
    {0x0F12,0x3601, WORD_LEN, 0},
    {0x0F12,0x43B1, WORD_LEN, 0},
    {0x0F12,0x8029, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF944, WORD_LEN, 0},
    {0x0F12,0x6020, WORD_LEN, 0},
    {0x0F12,0x8828, WORD_LEN, 0},
    {0x0F12,0x4330, WORD_LEN, 0},
    {0x0F12,0x8028, WORD_LEN, 0},
    {0x0F12,0xE73B, WORD_LEN, 0},
    {0x0F12,0x1C0A, WORD_LEN, 0},
    {0x0F12,0x436A, WORD_LEN, 0},
    {0x0F12,0x0A12, WORD_LEN, 0},
    {0x0F12,0x4282, WORD_LEN, 0},
    {0x0F12,0xD304, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF8F3, WORD_LEN, 0},
    {0x0F12,0x6020, WORD_LEN, 0},
    {0x0F12,0xE7F4, WORD_LEN, 0},
    {0x0F12,0x6025, WORD_LEN, 0},
    {0x0F12,0xE7F2, WORD_LEN, 0},
    {0x0F12,0xB410, WORD_LEN, 0},
    {0x0F12,0x4848, WORD_LEN, 0},
    {0x0F12,0x4950, WORD_LEN, 0},
    {0x0F12,0x89C0, WORD_LEN, 0},
    {0x0F12,0x2316, WORD_LEN, 0},
    {0x0F12,0x5ECC, WORD_LEN, 0},
    {0x0F12,0x1C02, WORD_LEN, 0},
    {0x0F12,0x42A0, WORD_LEN, 0},
    {0x0F12,0xDC00, WORD_LEN, 0},
    {0x0F12,0x1C22, WORD_LEN, 0},
    {0x0F12,0x82CA, WORD_LEN, 0},
    {0x0F12,0x2318, WORD_LEN, 0},
    {0x0F12,0x5ECA, WORD_LEN, 0},
    {0x0F12,0x4290, WORD_LEN, 0},
    {0x0F12,0xDC00, WORD_LEN, 0},
    {0x0F12,0x1C10, WORD_LEN, 0},
    {0x0F12,0x8308, WORD_LEN, 0},
    {0x0F12,0xBC10, WORD_LEN, 0},
    {0x0F12,0x4770, WORD_LEN, 0},
    {0x0F12,0xB570, WORD_LEN, 0},
    {0x0F12,0x1C06, WORD_LEN, 0},
    {0x0F12,0x4C45, WORD_LEN, 0},
    {0x0F12,0x2501, WORD_LEN, 0},
    {0x0F12,0x8820, WORD_LEN, 0},
    {0x0F12,0x02AD, WORD_LEN, 0},
    {0x0F12,0x43A8, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF91E, WORD_LEN, 0},
    {0x0F12,0x6030, WORD_LEN, 0},
    {0x0F12,0xF7FF, WORD_LEN, 0},
    {0x0F12,0xFFE0, WORD_LEN, 0},
    {0x0F12,0x8820, WORD_LEN, 0},
    {0x0F12,0x4328, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0xE741, WORD_LEN, 0},
    {0x0F12,0xB570, WORD_LEN, 0},
    {0x0F12,0x4C3D, WORD_LEN, 0},
    {0x0F12,0x2501, WORD_LEN, 0},
    {0x0F12,0x8820, WORD_LEN, 0},
    {0x0F12,0x02ED, WORD_LEN, 0},
    {0x0F12,0x43A8, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF916, WORD_LEN, 0},
    {0x0F12,0xF7FF, WORD_LEN, 0},
    {0x0F12,0xFFD1, WORD_LEN, 0},
    {0x0F12,0x8820, WORD_LEN, 0},
    {0x0F12,0x4328, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0xE732, WORD_LEN, 0},
    {0x0F12,0x230D, WORD_LEN, 0},
    {0x0F12,0x071B, WORD_LEN, 0},
    {0x0F12,0x18C3, WORD_LEN, 0},
    {0x0F12,0x8818, WORD_LEN, 0},
    {0x0F12,0x2A00, WORD_LEN, 0},
    {0x0F12,0xD001, WORD_LEN, 0},
    {0x0F12,0x4308, WORD_LEN, 0},
    {0x0F12,0xE000, WORD_LEN, 0},
    {0x0F12,0x4388, WORD_LEN, 0},
    {0x0F12,0x8018, WORD_LEN, 0},
    {0x0F12,0x4770, WORD_LEN, 0},
    {0x0F12,0xB570, WORD_LEN, 0},
    {0x0F12,0x2402, WORD_LEN, 0},
    {0x0F12,0x4932, WORD_LEN, 0},
    {0x0F12,0x8809, WORD_LEN, 0},
    {0x0F12,0x078A, WORD_LEN, 0},
    {0x0F12,0xD500, WORD_LEN, 0},
    {0x0F12,0x2406, WORD_LEN, 0},
    {0x0F12,0x2900, WORD_LEN, 0},
    {0x0F12,0xD01F, WORD_LEN, 0},
    {0x0F12,0x1C02, WORD_LEN, 0},
    {0x0F12,0x207D, WORD_LEN, 0},
    {0x0F12,0x00C0, WORD_LEN, 0},
    {0x0F12,0x2600, WORD_LEN, 0},
    {0x0F12,0x4D2D, WORD_LEN, 0},
    {0x0F12,0x2A00, WORD_LEN, 0},
    {0x0F12,0xD019, WORD_LEN, 0},
    {0x0F12,0x2101, WORD_LEN, 0},
    {0x0F12,0x8229, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF8F9, WORD_LEN, 0},
    {0x0F12,0x2200, WORD_LEN, 0},
    {0x0F12,0x2101, WORD_LEN, 0},
    {0x0F12,0x482A, WORD_LEN, 0},
    {0x0F12,0x0309, WORD_LEN, 0},
    {0x0F12,0xF7FF, WORD_LEN, 0},
    {0x0F12,0xFFDB, WORD_LEN, 0},
    {0x0F12,0x2008, WORD_LEN, 0},
    {0x0F12,0x4304, WORD_LEN, 0},
    {0x0F12,0x1C21, WORD_LEN, 0},
    {0x0F12,0x4C26, WORD_LEN, 0},
    {0x0F12,0x2200, WORD_LEN, 0},
    {0x0F12,0x3C14, WORD_LEN, 0},
    {0x0F12,0x1C20, WORD_LEN, 0},
    {0x0F12,0xF7FF, WORD_LEN, 0},
    {0x0F12,0xFFD2, WORD_LEN, 0},
    {0x0F12,0x2200, WORD_LEN, 0},
    {0x0F12,0x2121, WORD_LEN, 0},
    {0x0F12,0x1C20, WORD_LEN, 0},
    {0x0F12,0xF7FF, WORD_LEN, 0},
    {0x0F12,0xFFCD, WORD_LEN, 0},
    {0x0F12,0x802E, WORD_LEN, 0},
    {0x0F12,0xE6FD, WORD_LEN, 0},
    {0x0F12,0x822E, WORD_LEN, 0},
    {0x0F12,0x0789, WORD_LEN, 0},
    {0x0F12,0x0FC9, WORD_LEN, 0},
    {0x0F12,0x0089, WORD_LEN, 0},
    {0x0F12,0x223B, WORD_LEN, 0},
    {0x0F12,0x4311, WORD_LEN, 0},
    {0x0F12,0x8029, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF8DA, WORD_LEN, 0},
    {0x0F12,0xE7F4, WORD_LEN, 0},
    {0x0F12,0xB510, WORD_LEN, 0},
    {0x0F12,0x491B, WORD_LEN, 0},
    {0x0F12,0x8FC8, WORD_LEN, 0},
    {0x0F12,0x2800, WORD_LEN, 0},
    {0x0F12,0xD007, WORD_LEN, 0},
    {0x0F12,0x2000, WORD_LEN, 0},
    {0x0F12,0x87C8, WORD_LEN, 0},
    {0x0F12,0x8F88, WORD_LEN, 0},
    {0x0F12,0x4C19, WORD_LEN, 0},
    {0x0F12,0x2800, WORD_LEN, 0},
    {0x0F12,0xD002, WORD_LEN, 0},
    {0x0F12,0x2008, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0xE689, WORD_LEN, 0},
    {0x0F12,0x4815, WORD_LEN, 0},
    {0x0F12,0x3060, WORD_LEN, 0},
    {0x0F12,0x8900, WORD_LEN, 0},
    {0x0F12,0x2800, WORD_LEN, 0},
    {0x0F12,0xD103, WORD_LEN, 0},
    {0x0F12,0x4814, WORD_LEN, 0},
    {0x0F12,0x2101, WORD_LEN, 0},
    {0x0F12,0xF000, WORD_LEN, 0},
    {0x0F12,0xF8CA, WORD_LEN, 0},
    {0x0F12,0x2010, WORD_LEN, 0},
    {0x0F12,0x8020, WORD_LEN, 0},
    {0x0F12,0xE7F2, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x1376, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x2370, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x14D8, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x235C, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0xF4B0, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x1554, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x1AB8, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x0080, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x046C, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x0468, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x1100, WORD_LEN, 0},
    {0x0F12,0xD000, WORD_LEN, 0},
    {0x0F12,0x198C, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x0AC4, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0xB0A0, WORD_LEN, 0},
    {0x0F12,0xD000, WORD_LEN, 0},
    {0x0F12,0xB0B4, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x01B8, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x044E, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x0450, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x9CE7, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xF004, WORD_LEN, 0},
    {0x0F12,0xE51F, WORD_LEN, 0},
    {0x0F12,0x9FB8, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x14C1, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x27E1, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x88DF, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x275D, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x1ED3, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x27C5, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xF004, WORD_LEN, 0},
    {0x0F12,0xE51F, WORD_LEN, 0},
    {0x0F12,0xA144, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x1F87, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x27A9, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x1ECB, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x28FF, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x26F9, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x4027, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x9F03, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xF004, WORD_LEN, 0},
    {0x0F12,0xE51F, WORD_LEN, 0},
    {0x0F12,0x9D9C, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x285F, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x6181, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x6663, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x85D9, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x4778, WORD_LEN, 0},
    {0x0F12,0x46C0, WORD_LEN, 0},
    {0x0F12,0xC000, WORD_LEN, 0},
    {0x0F12,0xE59F, WORD_LEN, 0},
    {0x0F12,0xFF1C, WORD_LEN, 0},
    {0x0F12,0xE12F, WORD_LEN, 0},
    {0x0F12,0x2001, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0xE848, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x0F12,0xE848, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x0F12,0x0500, WORD_LEN, 0},
    {0x0F12,0x0064, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0}, // End T&P part //
    {0x0004,0x0000, WORD_LEN, 0}, //  2. Analog {0xetting   //
    {0xF454,0x0001, WORD_LEN, 0}, // ADC {0xat = 750mV(50h), NTG = -0.8V(10h), {0xaturation margin low limit = 732L{0xB //
    {0xF418,0x0050, WORD_LEN, 0},
    {0xF43E,0x0010, WORD_LEN, 0},
    {0x0004,0x0001, WORD_LEN, 0}, //Di{0xable Auto Addre{0x{0x Increment : 1//
    {0x002A,0x112A, WORD_LEN, 0}, // Analog {0xetting{0x // //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[8]   //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x1132, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[12]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x113E, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[18]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x115C, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[33]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x1164, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[37]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x1174, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[45]  //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                              //
    {0x002A,0x1178, WORD_LEN, 0}, //{0xenHal_{0xenReg{0xMode{0x3_p{0xenMode{0xReg{0xArray3[47]  //
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x077A, WORD_LEN, 0}, // m{0xm_uOff{0xetNoBin //
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x07A2, WORD_LEN, 0}, // m{0xm_{0xAnalogOff{0xet //
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x07B6, WORD_LEN, 0}, // m{0xm_NonLinearOf{0xOutput //
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},
    {0x0F12,0x0004, WORD_LEN, 0},
    {0x0F12,0x0004, WORD_LEN, 0},
    {0x0F12,0x0005, WORD_LEN, 0},
    {0x0F12,0x0005, WORD_LEN, 0},
    {0x0028,0x7000, WORD_LEN, 0}, //E{0xD Check Code add_100505//
    {0x002A,0x0132, WORD_LEN, 0},
    {0x0F12,0xAAAA, WORD_LEN, 0},
    {0x002A,0x1000, WORD_LEN, 0}, // 3. AE & AE weight ////TVAR_ae_BrAve                                    //
    {0x0F12,0x003C, WORD_LEN, 0}, //35  //
    {0x002A,0x0474, WORD_LEN, 0},
    {0x0F12,0x0112, WORD_LEN, 0}, //lt_uLimitHigh    //
    {0x0F12,0x00EF, WORD_LEN, 0}, //lt_uLimitLow     //
    {0x002A,0x1006, WORD_LEN, 0},
    {0x0F12,0x001F, WORD_LEN, 0}, //ae_{0xtatMode//
    {0x002A,0x108E, WORD_LEN, 0}, //{0xARR_IllumType//
    {0x0F12,0x00C7, WORD_LEN, 0},
    {0x0F12,0x00F7, WORD_LEN, 0},
    {0x0F12,0x0107, WORD_LEN, 0},
    {0x0F12,0x0142, WORD_LEN, 0},
    {0x0F12,0x017A, WORD_LEN, 0},
    {0x0F12,0x01A0, WORD_LEN, 0},
    {0x0F12,0x01B6, WORD_LEN, 0},
    {0x0F12,0x0100, WORD_LEN, 0}, //{0xARR_IllumTypeF	         //
    {0x0F12,0x0100, WORD_LEN, 0}, //0122                      //
    {0x0F12,0x0100, WORD_LEN, 0}, //0136                      //
    {0x0F12,0x0100, WORD_LEN, 0}, //00F6                      //
    {0x0F12,0x0100, WORD_LEN, 0}, //0100                      //
    {0x0F12,0x0100, WORD_LEN, 0}, //00FE                      //
    {0x0F12,0x0100, WORD_LEN, 0}, //0100                      //
    {0x002A,0x0488, WORD_LEN, 0},
    {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                   //
    {0x0F12,0xA316, WORD_LEN, 0}, //lt_uMaxExp2                        //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                   //
    {0x002A,0x2360, WORD_LEN, 0}, //2174 AWBBTune_EVT4_uMaxExp3      //
    {0x0F12,0xF424, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
    {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                   //
    {0x0F12,0xA316, WORD_LEN, 0}, //lt_uCapMaxExp2                     //
    {0x0F12,0x0000, WORD_LEN, 0}, //                                   //
    {0x002A,0x2364, WORD_LEN, 0}, //2178 AWBBTune_EVT4_uCapMaxExp3   //
    {0x0F12,0xF424, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0498, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux {0xhutter       //
    {0x0F12,0x0310, WORD_LEN, 0}, //lt_uMaxAnGain2 	                                   //
    {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
    {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
    {0x002A,0x049C, WORD_LEN, 0}, //                                                    //
    {0x0F12,0x0100, WORD_LEN, 0}, //lt_uMaxDigGain                                      //
    {0x002A,0x235C, WORD_LEN, 0}, //002A2170
    {0x0F12,0x0002, WORD_LEN, 0},  //1 #AWBBTune_EVT4_uMinCoar{0xe
    {0x0F12,0x0090, WORD_LEN, 0},  //#AWBBTune_EVT4_uMinFine
    {0x002A,0x100e, WORD_LEN, 0}, //ae_WeightTbl_16//
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0302, WORD_LEN, 0},
    {0x0F12,0x0203, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0403, WORD_LEN, 0},
    {0x0F12,0x0304, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0403, WORD_LEN, 0},
    {0x0F12,0x0304, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0302, WORD_LEN, 0},
    {0x0F12,0x0203, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0101, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0f4c, WORD_LEN, 0}, //brightne{0x{0x //
    {0x0F12,0x02b0, WORD_LEN, 0}, //180,       //
    {0x002A,0x0f52, WORD_LEN, 0}, //           //
    {0x0F12,0x02f0, WORD_LEN, 0}, //180,       //
    {0x002A,0x0754, WORD_LEN, 0}, // 4. {0xhading (GA{0x) // //TVAR_a{0xh_pGA{0x//
    {0x0F12,0x247C, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x002A,0x247C, WORD_LEN, 0}, //	param_{0xtart	TVAR_a{0xh_pGA{0x //
    {0x0F12,0x0141, WORD_LEN, 0}, //01D2 TVAR_a{0xh_pGA{0x[0]         //
    {0x0F12,0x010F, WORD_LEN, 0}, //0186 TVAR_a{0xh_pGA{0x[1]         //
    {0x0F12,0x00D9, WORD_LEN, 0}, //012A TVAR_a{0xh_pGA{0x[2]         //
    {0x0F12,0x00B4, WORD_LEN, 0}, //00E6 TVAR_a{0xh_pGA{0x[3]         //
    {0x0F12,0x009E, WORD_LEN, 0}, //00BC TVAR_a{0xh_pGA{0x[4]         //
    {0x0F12,0x0090, WORD_LEN, 0}, //00A2 TVAR_a{0xh_pGA{0x[5]         //
    {0x0F12,0x008A, WORD_LEN, 0}, //009B TVAR_a{0xh_pGA{0x[6]         //
    {0x0F12,0x008C, WORD_LEN, 0}, //00A1 TVAR_a{0xh_pGA{0x[7]         //
    {0x0F12,0x009A, WORD_LEN, 0}, //00B7 TVAR_a{0xh_pGA{0x[8]         //
    {0x0F12,0x00BB, WORD_LEN, 0}, //00E2 TVAR_a{0xh_pGA{0x[9]         //
    {0x0F12,0x00EC, WORD_LEN, 0}, //0122 TVAR_a{0xh_pGA{0x[10]        //
    {0x0F12,0x012C, WORD_LEN, 0}, //017E TVAR_a{0xh_pGA{0x[11]        //
    {0x0F12,0x0142, WORD_LEN, 0}, //01BF TVAR_a{0xh_pGA{0x[12]        //
    {0x0F12,0x0103, WORD_LEN, 0}, //019C TVAR_a{0xh_pGA{0x[13]        //
    {0x0F12,0x00C9, WORD_LEN, 0}, //013C TVAR_a{0xh_pGA{0x[14]        //
    {0x0F12,0x0096, WORD_LEN, 0}, //00E1 TVAR_a{0xh_pGA{0x[15]        //
    {0x0F12,0x0075, WORD_LEN, 0}, //00A7 TVAR_a{0xh_pGA{0x[16]        //
    {0x0F12,0x0062, WORD_LEN, 0}, //0082 TVAR_a{0xh_pGA{0x[17]        //
    {0x0F12,0x0057, WORD_LEN, 0}, //006D TVAR_a{0xh_pGA{0x[18]        //
    {0x0F12,0x0053, WORD_LEN, 0}, //0065 TVAR_a{0xh_pGA{0x[19]        //
    {0x0F12,0x0055, WORD_LEN, 0}, //006B TVAR_a{0xh_pGA{0x[20]        //
    {0x0F12,0x005E, WORD_LEN, 0}, //007E TVAR_a{0xh_pGA{0x[21]        //
    {0x0F12,0x0076, WORD_LEN, 0}, //00A1 TVAR_a{0xh_pGA{0x[22]        //
    {0x0F12,0x009E, WORD_LEN, 0}, //00DB TVAR_a{0xh_pGA{0x[23]        //
    {0x0F12,0x00E0, WORD_LEN, 0}, //0133 TVAR_a{0xh_pGA{0x[24]        //
    {0x0F12,0x0114, WORD_LEN, 0}, //018B TVAR_a{0xh_pGA{0x[25]        //
    {0x0F12,0x00CC, WORD_LEN, 0}, //015A TVAR_a{0xh_pGA{0x[26]        //
    {0x0F12,0x0091, WORD_LEN, 0}, //00FA TVAR_a{0xh_pGA{0x[27]        //
    {0x0F12,0x0062, WORD_LEN, 0}, //00A7 TVAR_a{0xh_pGA{0x[28]        //
    {0x0F12,0x0046, WORD_LEN, 0}, //0075 TVAR_a{0xh_pGA{0x[29]        //
    {0x0F12,0x0035, WORD_LEN, 0}, //0053 TVAR_a{0xh_pGA{0x[30]        //
    {0x0F12,0x002B, WORD_LEN, 0}, //0040 TVAR_a{0xh_pGA{0x[31]        //
    {0x0F12,0x0028, WORD_LEN, 0}, //0038 TVAR_a{0xh_pGA{0x[32]        //
    {0x0F12,0x002B, WORD_LEN, 0}, //003D TVAR_a{0xh_pGA{0x[33]        //
    {0x0F12,0x0036, WORD_LEN, 0}, //004F TVAR_a{0xh_pGA{0x[34]        //
    {0x0F12,0x0049, WORD_LEN, 0}, //006D TVAR_a{0xh_pGA{0x[35]        //
    {0x0F12,0x006D, WORD_LEN, 0}, //009F TVAR_a{0xh_pGA{0x[36]        //
    {0x0F12,0x00AC, WORD_LEN, 0}, //00EE TVAR_a{0xh_pGA{0x[37]        //
    {0x0F12,0x00EE, WORD_LEN, 0}, //0147 TVAR_a{0xh_pGA{0x[38]        //
    {0x0F12,0x00AA, WORD_LEN, 0}, //0129 TVAR_a{0xh_pGA{0x[39]        //
    {0x0F12,0x0072, WORD_LEN, 0}, //00CE TVAR_a{0xh_pGA{0x[40]        //
    {0x0F12,0x0046, WORD_LEN, 0}, //0085 TVAR_a{0xh_pGA{0x[41]        //
    {0x0F12,0x002C, WORD_LEN, 0}, //0055 TVAR_a{0xh_pGA{0x[42]        //
    {0x0F12,0x001B, WORD_LEN, 0}, //0035 TVAR_a{0xh_pGA{0x[43]        //
    {0x0F12,0x0010, WORD_LEN, 0}, //0020 TVAR_a{0xh_pGA{0x[44]        //
    {0x0F12,0x000D, WORD_LEN, 0}, //0018 TVAR_a{0xh_pGA{0x[45]        //
    {0x0F12,0x0013, WORD_LEN, 0}, //001E TVAR_a{0xh_pGA{0x[46]        //
    {0x0F12,0x0020, WORD_LEN, 0}, //0030 TVAR_a{0xh_pGA{0x[47]        //
    {0x0F12,0x0035, WORD_LEN, 0}, //004F TVAR_a{0xh_pGA{0x[48]        //
    {0x0F12,0x0056, WORD_LEN, 0}, //007B TVAR_a{0xh_pGA{0x[49]        //
    {0x0F12,0x0092, WORD_LEN, 0}, //00C3 TVAR_a{0xh_pGA{0x[50]        //
    {0x0F12,0x00D4, WORD_LEN, 0}, //0119 TVAR_a{0xh_pGA{0x[51]        //
    {0x0F12,0x009E, WORD_LEN, 0}, //010E TVAR_a{0xh_pGA{0x[52]        //
    {0x0F12,0x0066, WORD_LEN, 0}, //00B8 TVAR_a{0xh_pGA{0x[53]        //
    {0x0F12,0x003A, WORD_LEN, 0}, //0072 TVAR_a{0xh_pGA{0x[54]        //
    {0x0F12,0x001F, WORD_LEN, 0}, //0044 TVAR_a{0xh_pGA{0x[55]        //
    {0x0F12,0x000D, WORD_LEN, 0}, //0023 TVAR_a{0xh_pGA{0x[56]        //
    {0x0F12,0x0002, WORD_LEN, 0}, //000E TVAR_a{0xh_pGA{0x[57]        //
    {0x0F12,0x0000, WORD_LEN, 0}, //0005 TVAR_a{0xh_pGA{0x[58]        //
    {0x0F12,0x0007, WORD_LEN, 0}, //000C TVAR_a{0xh_pGA{0x[59]        //
    {0x0F12,0x0017, WORD_LEN, 0}, //001F TVAR_a{0xh_pGA{0x[60]        //
    {0x0F12,0x002D, WORD_LEN, 0}, //003E TVAR_a{0xh_pGA{0x[61]        //
    {0x0F12,0x004F, WORD_LEN, 0}, //0069 TVAR_a{0xh_pGA{0x[62]        //
    {0x0F12,0x0089, WORD_LEN, 0}, //00AC TVAR_a{0xh_pGA{0x[63]        //
    {0x0F12,0x00CA, WORD_LEN, 0}, //0101 TVAR_a{0xh_pGA{0x[64]        //
    {0x0F12,0x00A1, WORD_LEN, 0}, //0107 TVAR_a{0xh_pGA{0x[65]        //
    {0x0F12,0x0068, WORD_LEN, 0}, //00B2 TVAR_a{0xh_pGA{0x[66]        //
    {0x0F12,0x003B, WORD_LEN, 0}, //006F TVAR_a{0xh_pGA{0x[67]        //
    {0x0F12,0x001F, WORD_LEN, 0}, //0040 TVAR_a{0xh_pGA{0x[68]        //
    {0x0F12,0x000C, WORD_LEN, 0}, //001E TVAR_a{0xh_pGA{0x[69]        //
    {0x0F12,0x0001, WORD_LEN, 0}, //0008 TVAR_a{0xh_pGA{0x[70]        //
    {0x0F12,0x0000, WORD_LEN, 0}, //0000 TVAR_a{0xh_pGA{0x[71]        //
    {0x0F12,0x0008, WORD_LEN, 0}, //0007 TVAR_a{0xh_pGA{0x[72]        //
    {0x0F12,0x0019, WORD_LEN, 0}, //001B TVAR_a{0xh_pGA{0x[73]        //
    {0x0F12,0x0031, WORD_LEN, 0}, //003A TVAR_a{0xh_pGA{0x[74]        //
    {0x0F12,0x0052, WORD_LEN, 0}, //0065 TVAR_a{0xh_pGA{0x[75]        //
    {0x0F12,0x008C, WORD_LEN, 0}, //00A8 TVAR_a{0xh_pGA{0x[76]        //
    {0x0F12,0x00CF, WORD_LEN, 0}, //00FB TVAR_a{0xh_pGA{0x[77]        //
    {0x0F12,0x00B5, WORD_LEN, 0}, //0115 TVAR_a{0xh_pGA{0x[78]        //
    {0x0F12,0x0077, WORD_LEN, 0}, //00BC TVAR_a{0xh_pGA{0x[79]        //
    {0x0F12,0x0046, WORD_LEN, 0}, //0077 TVAR_a{0xh_pGA{0x[80]        //
    {0x0F12,0x0029, WORD_LEN, 0}, //0048 TVAR_a{0xh_pGA{0x[81]        //
    {0x0F12,0x0015, WORD_LEN, 0}, //0027 TVAR_a{0xh_pGA{0x[82]        //
    {0x0F12,0x000A, WORD_LEN, 0}, //0011 TVAR_a{0xh_pGA{0x[83]        //
    {0x0F12,0x0009, WORD_LEN, 0}, //0009 TVAR_a{0xh_pGA{0x[84]        //
    {0x0F12,0x0011, WORD_LEN, 0}, //000F TVAR_a{0xh_pGA{0x[85]        //
    {0x0F12,0x0024, WORD_LEN, 0}, //0023 TVAR_a{0xh_pGA{0x[86]        //
    {0x0F12,0x003D, WORD_LEN, 0}, //0043 TVAR_a{0xh_pGA{0x[87]        //
    {0x0F12,0x0062, WORD_LEN, 0}, //0070 TVAR_a{0xh_pGA{0x[88]        //
    {0x0F12,0x009F, WORD_LEN, 0}, //00B5 TVAR_a{0xh_pGA{0x[89]        //
    {0x0F12,0x00E5, WORD_LEN, 0}, //010A TVAR_a{0xh_pGA{0x[90]        //
    {0x0F12,0x00D9, WORD_LEN, 0}, //0138 TVAR_a{0xh_pGA{0x[91]        //
    {0x0F12,0x0095, WORD_LEN, 0}, //00D9 TVAR_a{0xh_pGA{0x[92]        //
    {0x0F12,0x005E, WORD_LEN, 0}, //008E TVAR_a{0xh_pGA{0x[93]        //
    {0x0F12,0x003E, WORD_LEN, 0}, //005E TVAR_a{0xh_pGA{0x[94]        //
    {0x0F12,0x002A, WORD_LEN, 0}, //003C TVAR_a{0xh_pGA{0x[95]        //
    {0x0F12,0x001F, WORD_LEN, 0}, //0027 TVAR_a{0xh_pGA{0x[96]        //
    {0x0F12,0x001E, WORD_LEN, 0}, //001F TVAR_a{0xh_pGA{0x[97]        //
    {0x0F12,0x0026, WORD_LEN, 0}, //0024 TVAR_a{0xh_pGA{0x[98]        //
    {0x0F12,0x0039, WORD_LEN, 0}, //0038 TVAR_a{0xh_pGA{0x[99]        //
    {0x0F12,0x0053, WORD_LEN, 0}, //0058 TVAR_a{0xh_pGA{0x[100]       //
    {0x0F12,0x007D, WORD_LEN, 0}, //0088 TVAR_a{0xh_pGA{0x[101]       //
    {0x0F12,0x00C1, WORD_LEN, 0}, //00D3 TVAR_a{0xh_pGA{0x[102]       //
    {0x0F12,0x010C, WORD_LEN, 0}, //012D TVAR_a{0xh_pGA{0x[103]       //
    {0x0F12,0x0109, WORD_LEN, 0}, //0171 TVAR_a{0xh_pGA{0x[104]       //
    {0x0F12,0x00C1, WORD_LEN, 0}, //010C TVAR_a{0xh_pGA{0x[105]       //
    {0x0F12,0x0084, WORD_LEN, 0}, //00B5 TVAR_a{0xh_pGA{0x[106]       //
    {0x0F12,0x005D, WORD_LEN, 0}, //007F TVAR_a{0xh_pGA{0x[107]       //
    {0x0F12,0x0048, WORD_LEN, 0}, //005D TVAR_a{0xh_pGA{0x[108]       //
    {0x0F12,0x003D, WORD_LEN, 0}, //0048 TVAR_a{0xh_pGA{0x[109]       //
    {0x0F12,0x003B, WORD_LEN, 0}, //0040 TVAR_a{0xh_pGA{0x[110]       //
    {0x0F12,0x0043, WORD_LEN, 0}, //0045 TVAR_a{0xh_pGA{0x[111]       //
    {0x0F12,0x0056, WORD_LEN, 0}, //0059 TVAR_a{0xh_pGA{0x[112]       //
    {0x0F12,0x0075, WORD_LEN, 0}, //007B TVAR_a{0xh_pGA{0x[113]       //
    {0x0F12,0x00A6, WORD_LEN, 0}, //00B1 TVAR_a{0xh_pGA{0x[114]       //
    {0x0F12,0x00F5, WORD_LEN, 0}, //0104 TVAR_a{0xh_pGA{0x[115]       //
    {0x0F12,0x0143, WORD_LEN, 0}, //0166 TVAR_a{0xh_pGA{0x[116]       //
    {0x0F12,0x0150, WORD_LEN, 0}, //01B9 TVAR_a{0xh_pGA{0x[117]       //
    {0x0F12,0x0105, WORD_LEN, 0}, //0152 TVAR_a{0xh_pGA{0x[118]       //
    {0x0F12,0x00BD, WORD_LEN, 0}, //00F3 TVAR_a{0xh_pGA{0x[119]       //
    {0x0F12,0x008F, WORD_LEN, 0}, //00B5 TVAR_a{0xh_pGA{0x[120]       //
    {0x0F12,0x0074, WORD_LEN, 0}, //008C TVAR_a{0xh_pGA{0x[121]       //
    {0x0F12,0x0067, WORD_LEN, 0}, //0075 TVAR_a{0xh_pGA{0x[122]       //
    {0x0F12,0x0064, WORD_LEN, 0}, //006E TVAR_a{0xh_pGA{0x[123]       //
    {0x0F12,0x006C, WORD_LEN, 0}, //0073 TVAR_a{0xh_pGA{0x[124]       //
    {0x0F12,0x0082, WORD_LEN, 0}, //0089 TVAR_a{0xh_pGA{0x[125]       //
    {0x0F12,0x00A8, WORD_LEN, 0}, //00B2 TVAR_a{0xh_pGA{0x[126]       //
    {0x0F12,0x00E3, WORD_LEN, 0}, //00F1 TVAR_a{0xh_pGA{0x[127]       //
    {0x0F12,0x013A, WORD_LEN, 0}, //014E TVAR_a{0xh_pGA{0x[128]       //
    {0x0F12,0x0174, WORD_LEN, 0}, //01A8 TVAR_a{0xh_pGA{0x[129]       //
    {0x0F12,0x0187, WORD_LEN, 0}, //01E4 TVAR_a{0xh_pGA{0x[130]       //
    {0x0F12,0x0152, WORD_LEN, 0}, //019F TVAR_a{0xh_pGA{0x[131]       //
    {0x0F12,0x0106, WORD_LEN, 0}, //013E TVAR_a{0xh_pGA{0x[132]       //
    {0x0F12,0x00D1, WORD_LEN, 0}, //00F5 TVAR_a{0xh_pGA{0x[133]       //
    {0x0F12,0x00B0, WORD_LEN, 0}, //00C6 TVAR_a{0xh_pGA{0x[134]       //
    {0x0F12,0x009D, WORD_LEN, 0}, //00AC TVAR_a{0xh_pGA{0x[135]       //
    {0x0F12,0x0098, WORD_LEN, 0}, //00A2 TVAR_a{0xh_pGA{0x[136]       //
    {0x0F12,0x00A0, WORD_LEN, 0}, //00AA TVAR_a{0xh_pGA{0x[137]       //
    {0x0F12,0x00B9, WORD_LEN, 0}, //00C3 TVAR_a{0xh_pGA{0x[138]       //
    {0x0F12,0x00E8, WORD_LEN, 0}, //00F3 TVAR_a{0xh_pGA{0x[139]       //
    {0x0F12,0x012C, WORD_LEN, 0}, //0138 TVAR_a{0xh_pGA{0x[140]       //
    {0x0F12,0x017A, WORD_LEN, 0}, //0193 TVAR_a{0xh_pGA{0x[141]       //
    {0x0F12,0x018D, WORD_LEN, 0}, //01DE TVAR_a{0xh_pGA{0x[142]       //
    {0x0F12,0x012F, WORD_LEN, 0}, //017C TVAR_a{0xh_pGA{0x[143]       //
    {0x0F12,0x00FC, WORD_LEN, 0}, //0139 TVAR_a{0xh_pGA{0x[144]       //
    {0x0F12,0x00C6, WORD_LEN, 0}, //00EC TVAR_a{0xh_pGA{0x[145]       //
    {0x0F12,0x00A3, WORD_LEN, 0}, //00B5 TVAR_a{0xh_pGA{0x[146]       //
    {0x0F12,0x008E, WORD_LEN, 0}, //0092 TVAR_a{0xh_pGA{0x[147]       //
    {0x0F12,0x0083, WORD_LEN, 0}, //007E TVAR_a{0xh_pGA{0x[148]       //
    {0x0F12,0x007E, WORD_LEN, 0}, //0077 TVAR_a{0xh_pGA{0x[149]       //
    {0x0F12,0x007F, WORD_LEN, 0}, //007B TVAR_a{0xh_pGA{0x[150]       //
    {0x0F12,0x008B, WORD_LEN, 0}, //008A TVAR_a{0xh_pGA{0x[151]       //
    {0x0F12,0x00A8, WORD_LEN, 0}, //00A7 TVAR_a{0xh_pGA{0x[152]       //
    {0x0F12,0x00D8, WORD_LEN, 0}, //00D8 TVAR_a{0xh_pGA{0x[153]       //
    {0x0F12,0x0115, WORD_LEN, 0}, //011F TVAR_a{0xh_pGA{0x[154]       //
    {0x0F12,0x0131, WORD_LEN, 0}, //015A TVAR_a{0xh_pGA{0x[155]       //
    {0x0F12,0x00ED, WORD_LEN, 0}, //014B TVAR_a{0xh_pGA{0x[156]       //
    {0x0F12,0x00B4, WORD_LEN, 0}, //00F9 TVAR_a{0xh_pGA{0x[157]       //
    {0x0F12,0x0085, WORD_LEN, 0}, //00AF TVAR_a{0xh_pGA{0x[158]       //
    {0x0F12,0x0069, WORD_LEN, 0}, //0081 TVAR_a{0xh_pGA{0x[159]       //
    {0x0F12,0x0059, WORD_LEN, 0}, //0064 TVAR_a{0xh_pGA{0x[160]       //
    {0x0F12,0x004E, WORD_LEN, 0}, //0054 TVAR_a{0xh_pGA{0x[161]       //
    {0x0F12,0x004C, WORD_LEN, 0}, //004E TVAR_a{0xh_pGA{0x[162]       //
    {0x0F12,0x004D, WORD_LEN, 0}, //0050 TVAR_a{0xh_pGA{0x[163]       //
    {0x0F12,0x0055, WORD_LEN, 0}, //005D TVAR_a{0xh_pGA{0x[164]       //
    {0x0F12,0x006A, WORD_LEN, 0}, //0075 TVAR_a{0xh_pGA{0x[165]       //
    {0x0F12,0x008D, WORD_LEN, 0}, //009F TVAR_a{0xh_pGA{0x[166]       //
    {0x0F12,0x00CA, WORD_LEN, 0}, //00E1 TVAR_a{0xh_pGA{0x[167]       //
    {0x0F12,0x00FF, WORD_LEN, 0}, //012C TVAR_a{0xh_pGA{0x[168]       //
    {0x0F12,0x00B7, WORD_LEN, 0}, //0116 TVAR_a{0xh_pGA{0x[169]       //
    {0x0F12,0x0082, WORD_LEN, 0}, //00C4 TVAR_a{0xh_pGA{0x[170]       //
    {0x0F12,0x0058, WORD_LEN, 0}, //0084 TVAR_a{0xh_pGA{0x[171]       //
    {0x0F12,0x003F, WORD_LEN, 0}, //005B TVAR_a{0xh_pGA{0x[172]       //
    {0x0F12,0x0031, WORD_LEN, 0}, //0042 TVAR_a{0xh_pGA{0x[173]       //
    {0x0F12,0x0028, WORD_LEN, 0}, //0032 TVAR_a{0xh_pGA{0x[174]       //
    {0x0F12,0x0025, WORD_LEN, 0}, //002B TVAR_a{0xh_pGA{0x[175]       //
    {0x0F12,0x0029, WORD_LEN, 0}, //002E TVAR_a{0xh_pGA{0x[176]       //
    {0x0F12,0x0032, WORD_LEN, 0}, //003A TVAR_a{0xh_pGA{0x[177]       //
    {0x0F12,0x0044, WORD_LEN, 0}, //004F TVAR_a{0xh_pGA{0x[178]       //
    {0x0F12,0x0064, WORD_LEN, 0}, //0072 TVAR_a{0xh_pGA{0x[179]       //
    {0x0F12,0x009D, WORD_LEN, 0}, //00AD TVAR_a{0xh_pGA{0x[180]       //
    {0x0F12,0x00DD, WORD_LEN, 0}, //00F8 TVAR_a{0xh_pGA{0x[181]       //
    {0x0F12,0x009A, WORD_LEN, 0}, //00EC TVAR_a{0xh_pGA{0x[182]       //
    {0x0F12,0x0066, WORD_LEN, 0}, //00A2 TVAR_a{0xh_pGA{0x[183]       //
    {0x0F12,0x003F, WORD_LEN, 0}, //0069 TVAR_a{0xh_pGA{0x[184]       //
    {0x0F12,0x0028, WORD_LEN, 0}, //0044 TVAR_a{0xh_pGA{0x[185]       //
    {0x0F12,0x0018, WORD_LEN, 0}, //002A TVAR_a{0xh_pGA{0x[186]       //
    {0x0F12,0x000E, WORD_LEN, 0}, //0019 TVAR_a{0xh_pGA{0x[187]       //
    {0x0F12,0x000C, WORD_LEN, 0}, //0012 TVAR_a{0xh_pGA{0x[188]       //
    {0x0F12,0x0011, WORD_LEN, 0}, //0015 TVAR_a{0xh_pGA{0x[189]       //
    {0x0F12,0x001F, WORD_LEN, 0}, //0022 TVAR_a{0xh_pGA{0x[190]       //
    {0x0F12,0x0032, WORD_LEN, 0}, //0037 TVAR_a{0xh_pGA{0x[191]       //
    {0x0F12,0x0050, WORD_LEN, 0}, //0057 TVAR_a{0xh_pGA{0x[192]       //
    {0x0F12,0x0086, WORD_LEN, 0}, //008C TVAR_a{0xh_pGA{0x[193]       //
    {0x0F12,0x00C7, WORD_LEN, 0}, //00D2 TVAR_a{0xh_pGA{0x[194]       //
    {0x0F12,0x008E, WORD_LEN, 0}, //00D6 TVAR_a{0xh_pGA{0x[195]       //
    {0x0F12,0x005B, WORD_LEN, 0}, //0090 TVAR_a{0xh_pGA{0x[196]       //
    {0x0F12,0x0034, WORD_LEN, 0}, //005A TVAR_a{0xh_pGA{0x[197]       //
    {0x0F12,0x001B, WORD_LEN, 0}, //0036 TVAR_a{0xh_pGA{0x[198]       //
    {0x0F12,0x000B, WORD_LEN, 0}, //001C TVAR_a{0xh_pGA{0x[199]       //
    {0x0F12,0x0001, WORD_LEN, 0}, //000A TVAR_a{0xh_pGA{0x[200]       //
    {0x0F12,0x0000, WORD_LEN, 0}, //0003 TVAR_a{0xh_pGA{0x[201]       //
    {0x0F12,0x0007, WORD_LEN, 0}, //0007 TVAR_a{0xh_pGA{0x[202]       //
    {0x0F12,0x0017, WORD_LEN, 0}, //0015 TVAR_a{0xh_pGA{0x[203]       //
    {0x0F12,0x002C, WORD_LEN, 0}, //002B TVAR_a{0xh_pGA{0x[204]       //
    {0x0F12,0x004A, WORD_LEN, 0}, //004A TVAR_a{0xh_pGA{0x[205]       //
    {0x0F12,0x007E, WORD_LEN, 0}, //007C TVAR_a{0xh_pGA{0x[206]       //
    {0x0F12,0x00BF, WORD_LEN, 0}, //00BF TVAR_a{0xh_pGA{0x[207]       //
    {0x0F12,0x0092, WORD_LEN, 0}, //00CF TVAR_a{0xh_pGA{0x[208]       //
    {0x0F12,0x005E, WORD_LEN, 0}, //008C TVAR_a{0xh_pGA{0x[209]       //
    {0x0F12,0x0035, WORD_LEN, 0}, //0057 TVAR_a{0xh_pGA{0x[210]       //
    {0x0F12,0x001B, WORD_LEN, 0}, //0033 TVAR_a{0xh_pGA{0x[211]       //
    {0x0F12,0x000A, WORD_LEN, 0}, //0019 TVAR_a{0xh_pGA{0x[212]       //
    {0x0F12,0x0001, WORD_LEN, 0}, //0007 TVAR_a{0xh_pGA{0x[213]       //
    {0x0F12,0x0001, WORD_LEN, 0}, //0000 TVAR_a{0xh_pGA{0x[214]       //
    {0x0F12,0x0008, WORD_LEN, 0}, //0004 TVAR_a{0xh_pGA{0x[215]       //
    {0x0F12,0x001A, WORD_LEN, 0}, //0012 TVAR_a{0xh_pGA{0x[216]       //
    {0x0F12,0x0030, WORD_LEN, 0}, //0029 TVAR_a{0xh_pGA{0x[217]       //
    {0x0F12,0x004F, WORD_LEN, 0}, //0048 TVAR_a{0xh_pGA{0x[218]       //
    {0x0F12,0x0083, WORD_LEN, 0}, //007A TVAR_a{0xh_pGA{0x[219]       //
    {0x0F12,0x00C4, WORD_LEN, 0}, //00BC TVAR_a{0xh_pGA{0x[220]       //
    {0x0F12,0x00A3, WORD_LEN, 0}, //00DB TVAR_a{0xh_pGA{0x[221]       //
    {0x0F12,0x006B, WORD_LEN, 0}, //0093 TVAR_a{0xh_pGA{0x[222]       //
    {0x0F12,0x0041, WORD_LEN, 0}, //005E TVAR_a{0xh_pGA{0x[223]       //
    {0x0F12,0x0026, WORD_LEN, 0}, //003B TVAR_a{0xh_pGA{0x[224]       //
    {0x0F12,0x0014, WORD_LEN, 0}, //0021 TVAR_a{0xh_pGA{0x[225]       //
    {0x0F12,0x000B, WORD_LEN, 0}, //000F TVAR_a{0xh_pGA{0x[226]       //
    {0x0F12,0x000A, WORD_LEN, 0}, //0008 TVAR_a{0xh_pGA{0x[227]       //
    {0x0F12,0x0013, WORD_LEN, 0}, //000C TVAR_a{0xh_pGA{0x[228]       //
    {0x0F12,0x0025, WORD_LEN, 0}, //001B TVAR_a{0xh_pGA{0x[229]       //
    {0x0F12,0x003D, WORD_LEN, 0}, //0031 TVAR_a{0xh_pGA{0x[230]       //
    {0x0F12,0x005F, WORD_LEN, 0}, //0052 TVAR_a{0xh_pGA{0x[231]       //
    {0x0F12,0x0095, WORD_LEN, 0}, //0084 TVAR_a{0xh_pGA{0x[232]       //
    {0x0F12,0x00D9, WORD_LEN, 0}, //00C8 TVAR_a{0xh_pGA{0x[233]       //
    {0x0F12,0x00C4, WORD_LEN, 0}, //00F8 TVAR_a{0xh_pGA{0x[234]       //
    {0x0F12,0x0087, WORD_LEN, 0}, //00AB TVAR_a{0xh_pGA{0x[235]       //
    {0x0F12,0x0057, WORD_LEN, 0}, //0071 TVAR_a{0xh_pGA{0x[236]       //
    {0x0F12,0x003B, WORD_LEN, 0}, //004D TVAR_a{0xh_pGA{0x[237]       //
    {0x0F12,0x0029, WORD_LEN, 0}, //0034 TVAR_a{0xh_pGA{0x[238]       //
    {0x0F12,0x0021, WORD_LEN, 0}, //0023 TVAR_a{0xh_pGA{0x[239]       //
    {0x0F12,0x0020, WORD_LEN, 0}, //001C TVAR_a{0xh_pGA{0x[240]       //
    {0x0F12,0x0029, WORD_LEN, 0}, //0020 TVAR_a{0xh_pGA{0x[241]       //
    {0x0F12,0x003B, WORD_LEN, 0}, //002E TVAR_a{0xh_pGA{0x[242]       //
    {0x0F12,0x0054, WORD_LEN, 0}, //0044 TVAR_a{0xh_pGA{0x[243]       //
    {0x0F12,0x007A, WORD_LEN, 0}, //0066 TVAR_a{0xh_pGA{0x[244]       //
    {0x0F12,0x00B6, WORD_LEN, 0}, //009D TVAR_a{0xh_pGA{0x[245]       //
    {0x0F12,0x00FF, WORD_LEN, 0}, //00E7 TVAR_a{0xh_pGA{0x[246]       //
    {0x0F12,0x00F4, WORD_LEN, 0}, //0125 TVAR_a{0xh_pGA{0x[247]       //
    {0x0F12,0x00B0, WORD_LEN, 0}, //00D2 TVAR_a{0xh_pGA{0x[248]       //
    {0x0F12,0x007A, WORD_LEN, 0}, //0090 TVAR_a{0xh_pGA{0x[249]       //
    {0x0F12,0x0059, WORD_LEN, 0}, //0069 TVAR_a{0xh_pGA{0x[250]       //
    {0x0F12,0x0048, WORD_LEN, 0}, //004F TVAR_a{0xh_pGA{0x[251]       //
    {0x0F12,0x0040, WORD_LEN, 0}, //003F TVAR_a{0xh_pGA{0x[252]       //
    {0x0F12,0x003F, WORD_LEN, 0}, //0038 TVAR_a{0xh_pGA{0x[253]       //
    {0x0F12,0x0048, WORD_LEN, 0}, //003C TVAR_a{0xh_pGA{0x[254]       //
    {0x0F12,0x005A, WORD_LEN, 0}, //0049 TVAR_a{0xh_pGA{0x[255]       //
    {0x0F12,0x0075, WORD_LEN, 0}, //0061 TVAR_a{0xh_pGA{0x[256]       //
    {0x0F12,0x00A0, WORD_LEN, 0}, //0087 TVAR_a{0xh_pGA{0x[257]       //
    {0x0F12,0x00E7, WORD_LEN, 0}, //00C4 TVAR_a{0xh_pGA{0x[258]       //
    {0x0F12,0x0135, WORD_LEN, 0}, //0114 TVAR_a{0xh_pGA{0x[259]       //
    {0x0F12,0x0137, WORD_LEN, 0}, //0162 TVAR_a{0xh_pGA{0x[260]       //
    {0x0F12,0x00EF, WORD_LEN, 0}, //010C TVAR_a{0xh_pGA{0x[261]       //
    {0x0F12,0x00AF, WORD_LEN, 0}, //00C3 TVAR_a{0xh_pGA{0x[262]       //
    {0x0F12,0x0088, WORD_LEN, 0}, //0093 TVAR_a{0xh_pGA{0x[263]       //
    {0x0F12,0x0072, WORD_LEN, 0}, //0074 TVAR_a{0xh_pGA{0x[264]       //
    {0x0F12,0x0068, WORD_LEN, 0}, //0064 TVAR_a{0xh_pGA{0x[265]       //
    {0x0F12,0x0067, WORD_LEN, 0}, //005D TVAR_a{0xh_pGA{0x[266]       //
    {0x0F12,0x006F, WORD_LEN, 0}, //0061 TVAR_a{0xh_pGA{0x[267]       //
    {0x0F12,0x0082, WORD_LEN, 0}, //0071 TVAR_a{0xh_pGA{0x[268]       //
    {0x0F12,0x00A4, WORD_LEN, 0}, //008C TVAR_a{0xh_pGA{0x[269]       //
    {0x0F12,0x00D8, WORD_LEN, 0}, //00BB TVAR_a{0xh_pGA{0x[270]       //
    {0x0F12,0x0129, WORD_LEN, 0}, //0102 TVAR_a{0xh_pGA{0x[271]       //
    {0x0F12,0x0161, WORD_LEN, 0}, //014D TVAR_a{0xh_pGA{0x[272]       //
    {0x0F12,0x016E, WORD_LEN, 0}, //018A TVAR_a{0xh_pGA{0x[273]       //
    {0x0F12,0x013A, WORD_LEN, 0}, //014F TVAR_a{0xh_pGA{0x[274]       //
    {0x0F12,0x00F3, WORD_LEN, 0}, //00FE TVAR_a{0xh_pGA{0x[275]       //
    {0x0F12,0x00C4, WORD_LEN, 0}, //00C5 TVAR_a{0xh_pGA{0x[276]       //
    {0x0F12,0x00A7, WORD_LEN, 0}, //00A2 TVAR_a{0xh_pGA{0x[277]       //
    {0x0F12,0x0099, WORD_LEN, 0}, //008E TVAR_a{0xh_pGA{0x[278]       //
    {0x0F12,0x0096, WORD_LEN, 0}, //0087 TVAR_a{0xh_pGA{0x[279]       //
    {0x0F12,0x009E, WORD_LEN, 0}, //008C TVAR_a{0xh_pGA{0x[280]       //
    {0x0F12,0x00B4, WORD_LEN, 0}, //009E TVAR_a{0xh_pGA{0x[281]       //
    {0x0F12,0x00DF, WORD_LEN, 0}, //00C0 TVAR_a{0xh_pGA{0x[282]       //
    {0x0F12,0x011F, WORD_LEN, 0}, //00F5 TVAR_a{0xh_pGA{0x[283]       //
    {0x0F12,0x0168, WORD_LEN, 0}, //013E TVAR_a{0xh_pGA{0x[284]       //
    {0x0F12,0x0178, WORD_LEN, 0}, //017D TVAR_a{0xh_pGA{0x[285]       //
    {0x0F12,0x0144, WORD_LEN, 0}, //0192 TVAR_a{0xh_pGA{0x[286]       //
    {0x0F12,0x010A, WORD_LEN, 0}, //0148 TVAR_a{0xh_pGA{0x[287]       //
    {0x0F12,0x00CE, WORD_LEN, 0}, //00F5 TVAR_a{0xh_pGA{0x[288]       //
    {0x0F12,0x00A8, WORD_LEN, 0}, //00BA TVAR_a{0xh_pGA{0x[289]       //
    {0x0F12,0x0091, WORD_LEN, 0}, //0095 TVAR_a{0xh_pGA{0x[290]       //
    {0x0F12,0x0084, WORD_LEN, 0}, //007F TVAR_a{0xh_pGA{0x[291]       //
    {0x0F12,0x007F, WORD_LEN, 0}, //0078 TVAR_a{0xh_pGA{0x[292]       //
    {0x0F12,0x0083, WORD_LEN, 0}, //007C TVAR_a{0xh_pGA{0x[293]       //
    {0x0F12,0x0092, WORD_LEN, 0}, //008F TVAR_a{0xh_pGA{0x[294]       //
    {0x0F12,0x00B6, WORD_LEN, 0}, //00B3 TVAR_a{0xh_pGA{0x[295]       //
    {0x0F12,0x00EC, WORD_LEN, 0}, //00E9 TVAR_a{0xh_pGA{0x[296]       //
    {0x0F12,0x0130, WORD_LEN, 0}, //013B TVAR_a{0xh_pGA{0x[297]       //
    {0x0F12,0x014F, WORD_LEN, 0}, //017E TVAR_a{0xh_pGA{0x[298]       //
    {0x0F12,0x0100, WORD_LEN, 0}, //015F TVAR_a{0xh_pGA{0x[299]       //
    {0x0F12,0x00C2, WORD_LEN, 0}, //0108 TVAR_a{0xh_pGA{0x[300]       //
    {0x0F12,0x008E, WORD_LEN, 0}, //00B9 TVAR_a{0xh_pGA{0x[301]       //
    {0x0F12,0x006E, WORD_LEN, 0}, //0087 TVAR_a{0xh_pGA{0x[302]       //
    {0x0F12,0x005B, WORD_LEN, 0}, //0069 TVAR_a{0xh_pGA{0x[303]       //
    {0x0F12,0x0050, WORD_LEN, 0}, //0056 TVAR_a{0xh_pGA{0x[304]       //
    {0x0F12,0x004D, WORD_LEN, 0}, //004F TVAR_a{0xh_pGA{0x[305]       //
    {0x0F12,0x0051, WORD_LEN, 0}, //0053 TVAR_a{0xh_pGA{0x[306]       //
    {0x0F12,0x005D, WORD_LEN, 0}, //0062 TVAR_a{0xh_pGA{0x[307]       //
    {0x0F12,0x0075, WORD_LEN, 0}, //007F TVAR_a{0xh_pGA{0x[308]       //
    {0x0F12,0x009F, WORD_LEN, 0}, //00AE TVAR_a{0xh_pGA{0x[309]       //
    {0x0F12,0x00E1, WORD_LEN, 0}, //00F8 TVAR_a{0xh_pGA{0x[310]       //
    {0x0F12,0x011D, WORD_LEN, 0}, //014C TVAR_a{0xh_pGA{0x[311]       //
    {0x0F12,0x00CB, WORD_LEN, 0}, //0129 TVAR_a{0xh_pGA{0x[312]       //
    {0x0F12,0x0090, WORD_LEN, 0}, //00D4 TVAR_a{0xh_pGA{0x[313]       //
    {0x0F12,0x0062, WORD_LEN, 0}, //008F TVAR_a{0xh_pGA{0x[314]       //
    {0x0F12,0x0046, WORD_LEN, 0}, //0063 TVAR_a{0xh_pGA{0x[315]       //
    {0x0F12,0x0035, WORD_LEN, 0}, //0047 TVAR_a{0xh_pGA{0x[316]       //
    {0x0F12,0x002A, WORD_LEN, 0}, //0034 TVAR_a{0xh_pGA{0x[317]       //
    {0x0F12,0x0027, WORD_LEN, 0}, //002D TVAR_a{0xh_pGA{0x[318]       //
    {0x0F12,0x002D, WORD_LEN, 0}, //0030 TVAR_a{0xh_pGA{0x[319]       //
    {0x0F12,0x0039, WORD_LEN, 0}, //003E TVAR_a{0xh_pGA{0x[320]       //
    {0x0F12,0x004F, WORD_LEN, 0}, //0057 TVAR_a{0xh_pGA{0x[321]       //
    {0x0F12,0x0072, WORD_LEN, 0}, //007F TVAR_a{0xh_pGA{0x[322]       //
    {0x0F12,0x00AF, WORD_LEN, 0}, //00BF TVAR_a{0xh_pGA{0x[323]       //
    {0x0F12,0x00F5, WORD_LEN, 0}, //0111 TVAR_a{0xh_pGA{0x[324]       //
    {0x0F12,0x00AE, WORD_LEN, 0}, //0100 TVAR_a{0xh_pGA{0x[325]       //
    {0x0F12,0x0075, WORD_LEN, 0}, //00B1 TVAR_a{0xh_pGA{0x[326]       //
    {0x0F12,0x004A, WORD_LEN, 0}, //0074 TVAR_a{0xh_pGA{0x[327]       //
    {0x0F12,0x002F, WORD_LEN, 0}, //004C TVAR_a{0xh_pGA{0x[328]       //
    {0x0F12,0x001C, WORD_LEN, 0}, //002F TVAR_a{0xh_pGA{0x[329]       //
    {0x0F12,0x0011, WORD_LEN, 0}, //001C TVAR_a{0xh_pGA{0x[330]       //
    {0x0F12,0x000E, WORD_LEN, 0}, //0013 TVAR_a{0xh_pGA{0x[331]       //
    {0x0F12,0x0014, WORD_LEN, 0}, //0017 TVAR_a{0xh_pGA{0x[332]       //
    {0x0F12,0x0024, WORD_LEN, 0}, //0026 TVAR_a{0xh_pGA{0x[333]       //
    {0x0F12,0x003A, WORD_LEN, 0}, //003E TVAR_a{0xh_pGA{0x[334]       //
    {0x0F12,0x005B, WORD_LEN, 0}, //0062 TVAR_a{0xh_pGA{0x[335]       //
    {0x0F12,0x0093, WORD_LEN, 0}, //0099 TVAR_a{0xh_pGA{0x[336]       //
    {0x0F12,0x00D8, WORD_LEN, 0}, //00E5 TVAR_a{0xh_pGA{0x[337]       //
    {0x0F12,0x00A2, WORD_LEN, 0}, //00E6 TVAR_a{0xh_pGA{0x[338]       //
    {0x0F12,0x006A, WORD_LEN, 0}, //009F TVAR_a{0xh_pGA{0x[339]       //
    {0x0F12,0x0040, WORD_LEN, 0}, //0066 TVAR_a{0xh_pGA{0x[340]       //
    {0x0F12,0x0023, WORD_LEN, 0}, //003F TVAR_a{0xh_pGA{0x[341]       //
    {0x0F12,0x000F, WORD_LEN, 0}, //0021 TVAR_a{0xh_pGA{0x[342]       //
    {0x0F12,0x0003, WORD_LEN, 0}, //000D TVAR_a{0xh_pGA{0x[343]       //
    {0x0F12,0x0001, WORD_LEN, 0}, //0004 TVAR_a{0xh_pGA{0x[344]       //
    {0x0F12,0x0008, WORD_LEN, 0}, //0008 TVAR_a{0xh_pGA{0x[345]       //
    {0x0F12,0x0019, WORD_LEN, 0}, //0016 TVAR_a{0xh_pGA{0x[346]       //
    {0x0F12,0x0030, WORD_LEN, 0}, //002F TVAR_a{0xh_pGA{0x[347]       //
    {0x0F12,0x0050, WORD_LEN, 0}, //0050 TVAR_a{0xh_pGA{0x[348]       //
    {0x0F12,0x0086, WORD_LEN, 0}, //0085 TVAR_a{0xh_pGA{0x[349]       //
    {0x0F12,0x00C7, WORD_LEN, 0}, //00CA TVAR_a{0xh_pGA{0x[350]       //
    {0x0F12,0x00A6, WORD_LEN, 0}, //00E2 TVAR_a{0xh_pGA{0x[351]       //
    {0x0F12,0x006D, WORD_LEN, 0}, //009A TVAR_a{0xh_pGA{0x[352]       //
    {0x0F12,0x0040, WORD_LEN, 0}, //0062 TVAR_a{0xh_pGA{0x[353]       //
    {0x0F12,0x0023, WORD_LEN, 0}, //003B TVAR_a{0xh_pGA{0x[354]       //
    {0x0F12,0x000F, WORD_LEN, 0}, //001D TVAR_a{0xh_pGA{0x[355]       //
    {0x0F12,0x0002, WORD_LEN, 0}, //0009 TVAR_a{0xh_pGA{0x[356]       //
    {0x0F12,0x0000, WORD_LEN, 0}, //0000 TVAR_a{0xh_pGA{0x[357]       //
    {0x0F12,0x0007, WORD_LEN, 0}, //0003 TVAR_a{0xh_pGA{0x[358]       //
    {0x0F12,0x0018, WORD_LEN, 0}, //0011 TVAR_a{0xh_pGA{0x[359]       //
    {0x0F12,0x002F, WORD_LEN, 0}, //0029 TVAR_a{0xh_pGA{0x[360]       //
    {0x0F12,0x0050, WORD_LEN, 0}, //0049 TVAR_a{0xh_pGA{0x[361]       //
    {0x0F12,0x0085, WORD_LEN, 0}, //007D TVAR_a{0xh_pGA{0x[362]       //
    {0x0F12,0x00C5, WORD_LEN, 0}, //00C0 TVAR_a{0xh_pGA{0x[363]       //
    {0x0F12,0x00B9, WORD_LEN, 0}, //00EE TVAR_a{0xh_pGA{0x[364]       //
    {0x0F12,0x007B, WORD_LEN, 0}, //00A1 TVAR_a{0xh_pGA{0x[365]       //
    {0x0F12,0x004C, WORD_LEN, 0}, //0069 TVAR_a{0xh_pGA{0x[366]       //
    {0x0F12,0x002D, WORD_LEN, 0}, //0043 TVAR_a{0xh_pGA{0x[367]       //
    {0x0F12,0x0018, WORD_LEN, 0}, //0025 TVAR_a{0xh_pGA{0x[368]       //
    {0x0F12,0x000B, WORD_LEN, 0}, //0010 TVAR_a{0xh_pGA{0x[369]       //
    {0x0F12,0x0008, WORD_LEN, 0}, //0007 TVAR_a{0xh_pGA{0x[370]       //
    {0x0F12,0x000F, WORD_LEN, 0}, //000A TVAR_a{0xh_pGA{0x[371]       //
    {0x0F12,0x0020, WORD_LEN, 0}, //0017 TVAR_a{0xh_pGA{0x[372]       //
    {0x0F12,0x0038, WORD_LEN, 0}, //002E TVAR_a{0xh_pGA{0x[373]       //
    {0x0F12,0x0059, WORD_LEN, 0}, //004F TVAR_a{0xh_pGA{0x[374]       //
    {0x0F12,0x0090, WORD_LEN, 0}, //0083 TVAR_a{0xh_pGA{0x[375]       //
    {0x0F12,0x00D3, WORD_LEN, 0}, //00C7 TVAR_a{0xh_pGA{0x[376]       //
    {0x0F12,0x00DA, WORD_LEN, 0}, //010B TVAR_a{0xh_pGA{0x[377]       //
    {0x0F12,0x0097, WORD_LEN, 0}, //00BA TVAR_a{0xh_pGA{0x[378]       //
    {0x0F12,0x0063, WORD_LEN, 0}, //007B TVAR_a{0xh_pGA{0x[379]       //
    {0x0F12,0x0042, WORD_LEN, 0}, //0054 TVAR_a{0xh_pGA{0x[380]       //
    {0x0F12,0x002C, WORD_LEN, 0}, //0037 TVAR_a{0xh_pGA{0x[381]       //
    {0x0F12,0x0020, WORD_LEN, 0}, //0023 TVAR_a{0xh_pGA{0x[382]       //
    {0x0F12,0x001C, WORD_LEN, 0}, //001A TVAR_a{0xh_pGA{0x[383]       //
    {0x0F12,0x0023, WORD_LEN, 0}, //001B TVAR_a{0xh_pGA{0x[384]       //
    {0x0F12,0x0032, WORD_LEN, 0}, //0027 TVAR_a{0xh_pGA{0x[385]       //
    {0x0F12,0x0049, WORD_LEN, 0}, //003D TVAR_a{0xh_pGA{0x[386]       //
    {0x0F12,0x006E, WORD_LEN, 0}, //005F TVAR_a{0xh_pGA{0x[387]       //
    {0x0F12,0x00AA, WORD_LEN, 0}, //0095 TVAR_a{0xh_pGA{0x[388]       //
    {0x0F12,0x00F0, WORD_LEN, 0}, //00DE TVAR_a{0xh_pGA{0x[389]       //
    {0x0F12,0x010C, WORD_LEN, 0}, //013A TVAR_a{0xh_pGA{0x[390]       //
    {0x0F12,0x00C1, WORD_LEN, 0}, //00E1 TVAR_a{0xh_pGA{0x[391]       //
    {0x0F12,0x0086, WORD_LEN, 0}, //009C TVAR_a{0xh_pGA{0x[392]       //
    {0x0F12,0x0061, WORD_LEN, 0}, //006F TVAR_a{0xh_pGA{0x[393]       //
    {0x0F12,0x004A, WORD_LEN, 0}, //0053 TVAR_a{0xh_pGA{0x[394]       //
    {0x0F12,0x003E, WORD_LEN, 0}, //003F TVAR_a{0xh_pGA{0x[395]       //
    {0x0F12,0x003A, WORD_LEN, 0}, //0035 TVAR_a{0xh_pGA{0x[396]       //
    {0x0F12,0x003F, WORD_LEN, 0}, //0036 TVAR_a{0xh_pGA{0x[397]       //
    {0x0F12,0x004E, WORD_LEN, 0}, //0042 TVAR_a{0xh_pGA{0x[398]       //
    {0x0F12,0x0067, WORD_LEN, 0}, //0058 TVAR_a{0xh_pGA{0x[399]       //
    {0x0F12,0x0090, WORD_LEN, 0}, //007D TVAR_a{0xh_pGA{0x[400]       //
    {0x0F12,0x00D5, WORD_LEN, 0}, //00B9 TVAR_a{0xh_pGA{0x[401]       //
    {0x0F12,0x011D, WORD_LEN, 0}, //0105 TVAR_a{0xh_pGA{0x[402]       //
    {0x0F12,0x0152, WORD_LEN, 0}, //0177 TVAR_a{0xh_pGA{0x[403]       //
    {0x0F12,0x0102, WORD_LEN, 0}, //011D TVAR_a{0xh_pGA{0x[404]       //
    {0x0F12,0x00BC, WORD_LEN, 0}, //00CE TVAR_a{0xh_pGA{0x[405]       //
    {0x0F12,0x008F, WORD_LEN, 0}, //009A TVAR_a{0xh_pGA{0x[406]       //
    {0x0F12,0x0075, WORD_LEN, 0}, //0078 TVAR_a{0xh_pGA{0x[407]       //
    {0x0F12,0x0066, WORD_LEN, 0}, //0064 TVAR_a{0xh_pGA{0x[408]       //
    {0x0F12,0x0061, WORD_LEN, 0}, //005B TVAR_a{0xh_pGA{0x[409]       //
    {0x0F12,0x0066, WORD_LEN, 0}, //005C TVAR_a{0xh_pGA{0x[410]       //
    {0x0F12,0x0076, WORD_LEN, 0}, //0068 TVAR_a{0xh_pGA{0x[411]       //
    {0x0F12,0x0093, WORD_LEN, 0}, //0082 TVAR_a{0xh_pGA{0x[412]       //
    {0x0F12,0x00C6, WORD_LEN, 0}, //00AD TVAR_a{0xh_pGA{0x[413]       //
    {0x0F12,0x0110, WORD_LEN, 0}, //00F1 TVAR_a{0xh_pGA{0x[414]       //
    {0x0F12,0x0146, WORD_LEN, 0}, //0137 TVAR_a{0xh_pGA{0x[415]       //
    {0x0F12,0x0188, WORD_LEN, 0}, //019F TVAR_a{0xh_pGA{0x[416]       //
    {0x0F12,0x014E, WORD_LEN, 0}, //0160 TVAR_a{0xh_pGA{0x[417]       //
    {0x0F12,0x0102, WORD_LEN, 0}, //010D TVAR_a{0xh_pGA{0x[418]       //
    {0x0F12,0x00CD, WORD_LEN, 0}, //00CF TVAR_a{0xh_pGA{0x[419]       //
    {0x0F12,0x00AD, WORD_LEN, 0}, //00A7 TVAR_a{0xh_pGA{0x[420]       //
    {0x0F12,0x0098, WORD_LEN, 0}, //0090 TVAR_a{0xh_pGA{0x[421]       //
    {0x0F12,0x0091, WORD_LEN, 0}, //0085 TVAR_a{0xh_pGA{0x[422]       //
    {0x0F12,0x0095, WORD_LEN, 0}, //0087 TVAR_a{0xh_pGA{0x[423]       //
    {0x0F12,0x00A8, WORD_LEN, 0}, //0097 TVAR_a{0xh_pGA{0x[424]       //
    {0x0F12,0x00CD, WORD_LEN, 0}, //00B6 TVAR_a{0xh_pGA{0x[425]       //
    {0x0F12,0x0107, WORD_LEN, 0}, //00E7 TVAR_a{0xh_pGA{0x[426]       //
    {0x0F12,0x014A, WORD_LEN, 0}, //0129 TVAR_a{0xh_pGA{0x[427]       //
    {0x0F12,0x015B, WORD_LEN, 0}, //0166 TVAR_a{0xh_pGA{0x[428]       //
    {0x0F12,0x00DC, WORD_LEN, 0}, //013A TVAR_a{0xh_pGA{0x[429]       //
    {0x0F12,0x00BA, WORD_LEN, 0}, //0102 TVAR_a{0xh_pGA{0x[430]       //
    {0x0F12,0x0093, WORD_LEN, 0}, //00C1 TVAR_a{0xh_pGA{0x[431]       //
    {0x0F12,0x007A, WORD_LEN, 0}, //0092 TVAR_a{0xh_pGA{0x[432]       //
    {0x0F12,0x006B, WORD_LEN, 0}, //0075 TVAR_a{0xh_pGA{0x[433]       //
    {0x0F12,0x0064, WORD_LEN, 0}, //0065 TVAR_a{0xh_pGA{0x[434]       //
    {0x0F12,0x0062, WORD_LEN, 0}, //0061 TVAR_a{0xh_pGA{0x[435]       //
    {0x0F12,0x0067, WORD_LEN, 0}, //0068 TVAR_a{0xh_pGA{0x[436]       //
    {0x0F12,0x0074, WORD_LEN, 0}, //007A TVAR_a{0xh_pGA{0x[437]       //
    {0x0F12,0x0090, WORD_LEN, 0}, //009C TVAR_a{0xh_pGA{0x[438]       //
    {0x0F12,0x00BC, WORD_LEN, 0}, //00CD TVAR_a{0xh_pGA{0x[439]       //
    {0x0F12,0x00F3, WORD_LEN, 0}, //0116 TVAR_a{0xh_pGA{0x[440]       //
    {0x0F12,0x0104, WORD_LEN, 0}, //014D TVAR_a{0xh_pGA{0x[441]       //
    {0x0F12,0x00AA, WORD_LEN, 0}, //0116 TVAR_a{0xh_pGA{0x[442]       //
    {0x0F12,0x0081, WORD_LEN, 0}, //00CE TVAR_a{0xh_pGA{0x[443]       //
    {0x0F12,0x005E, WORD_LEN, 0}, //008E TVAR_a{0xh_pGA{0x[444]       //
    {0x0F12,0x004B, WORD_LEN, 0}, //0068 TVAR_a{0xh_pGA{0x[445]       //
    {0x0F12,0x0041, WORD_LEN, 0}, //0050 TVAR_a{0xh_pGA{0x[446]       //
    {0x0F12,0x003C, WORD_LEN, 0}, //0044 TVAR_a{0xh_pGA{0x[447]       //
    {0x0F12,0x003C, WORD_LEN, 0}, //0040 TVAR_a{0xh_pGA{0x[448]       //
    {0x0F12,0x003F, WORD_LEN, 0}, //0045 TVAR_a{0xh_pGA{0x[449]       //
    {0x0F12,0x0048, WORD_LEN, 0}, //0054 TVAR_a{0xh_pGA{0x[450]       //
    {0x0F12,0x005A, WORD_LEN, 0}, //006F TVAR_a{0xh_pGA{0x[451]       //
    {0x0F12,0x007A, WORD_LEN, 0}, //009A TVAR_a{0xh_pGA{0x[452]       //
    {0x0F12,0x00B0, WORD_LEN, 0}, //00DC TVAR_a{0xh_pGA{0x[453]       //
    {0x0F12,0x00D7, WORD_LEN, 0}, //0127 TVAR_a{0xh_pGA{0x[454]       //
    {0x0F12,0x007E, WORD_LEN, 0}, //00E5 TVAR_a{0xh_pGA{0x[455]       //
    {0x0F12,0x0058, WORD_LEN, 0}, //00A0 TVAR_a{0xh_pGA{0x[456]       //
    {0x0F12,0x0039, WORD_LEN, 0}, //0068 TVAR_a{0xh_pGA{0x[457]       //
    {0x0F12,0x002A, WORD_LEN, 0}, //0047 TVAR_a{0xh_pGA{0x[458]       //
    {0x0F12,0x0022, WORD_LEN, 0}, //0033 TVAR_a{0xh_pGA{0x[459]       //
    {0x0F12,0x001E, WORD_LEN, 0}, //0027 TVAR_a{0xh_pGA{0x[460]       //
    {0x0F12,0x001E, WORD_LEN, 0}, //0023 TVAR_a{0xh_pGA{0x[461]       //
    {0x0F12,0x0022, WORD_LEN, 0}, //0028 TVAR_a{0xh_pGA{0x[462]       //
    {0x0F12,0x002A, WORD_LEN, 0}, //0036 TVAR_a{0xh_pGA{0x[463]       //
    {0x0F12,0x0038, WORD_LEN, 0}, //004B TVAR_a{0xh_pGA{0x[464]       //
    {0x0F12,0x0052, WORD_LEN, 0}, //006E TVAR_a{0xh_pGA{0x[465]       //
    {0x0F12,0x0081, WORD_LEN, 0}, //00A7 TVAR_a{0xh_pGA{0x[466]       //
    {0x0F12,0x00B4, WORD_LEN, 0}, //00EE TVAR_a{0xh_pGA{0x[467]       //
    {0x0F12,0x0065, WORD_LEN, 0}, //00C0 TVAR_a{0xh_pGA{0x[468]       //
    {0x0F12,0x0041, WORD_LEN, 0}, //0081 TVAR_a{0xh_pGA{0x[469]       //
    {0x0F12,0x0025, WORD_LEN, 0}, //0051 TVAR_a{0xh_pGA{0x[470]       //
    {0x0F12,0x0017, WORD_LEN, 0}, //0034 TVAR_a{0xh_pGA{0x[471]       //
    {0x0F12,0x000F, WORD_LEN, 0}, //0020 TVAR_a{0xh_pGA{0x[472]       //
    {0x0F12,0x000A, WORD_LEN, 0}, //0014 TVAR_a{0xh_pGA{0x[473]       //
    {0x0F12,0x000A, WORD_LEN, 0}, //000F TVAR_a{0xh_pGA{0x[474]       //
    {0x0F12,0x000F, WORD_LEN, 0}, //0014 TVAR_a{0xh_pGA{0x[475]       //
    {0x0F12,0x001A, WORD_LEN, 0}, //0020 TVAR_a{0xh_pGA{0x[476]       //
    {0x0F12,0x0026, WORD_LEN, 0}, //0034 TVAR_a{0xh_pGA{0x[477]       //
    {0x0F12,0x003D, WORD_LEN, 0}, //0052 TVAR_a{0xh_pGA{0x[478]       //
    {0x0F12,0x0068, WORD_LEN, 0}, //0083 TVAR_a{0xh_pGA{0x[479]       //
    {0x0F12,0x0099, WORD_LEN, 0}, //00C3 TVAR_a{0xh_pGA{0x[480]       //
    {0x0F12,0x005D, WORD_LEN, 0}, //00AD TVAR_a{0xh_pGA{0x[481]       //
    {0x0F12,0x0038, WORD_LEN, 0}, //0073 TVAR_a{0xh_pGA{0x[482]       //
    {0x0F12,0x001D, WORD_LEN, 0}, //0046 TVAR_a{0xh_pGA{0x[483]       //
    {0x0F12,0x000F, WORD_LEN, 0}, //002A TVAR_a{0xh_pGA{0x[484]       //
    {0x0F12,0x0006, WORD_LEN, 0}, //0016 TVAR_a{0xh_pGA{0x[485]       //
    {0x0F12,0x0000, WORD_LEN, 0}, //0008 TVAR_a{0xh_pGA{0x[486]       //
    {0x0F12,0x0000, WORD_LEN, 0}, //0004 TVAR_a{0xh_pGA{0x[487]       //
    {0x0F12,0x0006, WORD_LEN, 0}, //0008 TVAR_a{0xh_pGA{0x[488]       //
    {0x0F12,0x0010, WORD_LEN, 0}, //0014 TVAR_a{0xh_pGA{0x[489]       //
    {0x0F12,0x001D, WORD_LEN, 0}, //0026 TVAR_a{0xh_pGA{0x[490]       //
    {0x0F12,0x0031, WORD_LEN, 0}, //0040 TVAR_a{0xh_pGA{0x[491]       //
    {0x0F12,0x005A, WORD_LEN, 0}, //006D TVAR_a{0xh_pGA{0x[492]       //
    {0x0F12,0x0089, WORD_LEN, 0}, //00AA TVAR_a{0xh_pGA{0x[493]       //
    {0x0F12,0x005F, WORD_LEN, 0}, //00A8 TVAR_a{0xh_pGA{0x[494]       //
    {0x0F12,0x003A, WORD_LEN, 0}, //006F TVAR_a{0xh_pGA{0x[495]       //
    {0x0F12,0x001F, WORD_LEN, 0}, //0044 TVAR_a{0xh_pGA{0x[496]       //
    {0x0F12,0x000F, WORD_LEN, 0}, //0027 TVAR_a{0xh_pGA{0x[497]       //
    {0x0F12,0x0006, WORD_LEN, 0}, //0013 TVAR_a{0xh_pGA{0x[498]       //
    {0x0F12,0x0000, WORD_LEN, 0}, //0005 TVAR_a{0xh_pGA{0x[499]       //
    {0x0F12,0x0000, WORD_LEN, 0}, //0000 TVAR_a{0xh_pGA{0x[500]       //
    {0x0F12,0x0005, WORD_LEN, 0}, //0004 TVAR_a{0xh_pGA{0x[501]       //
    {0x0F12,0x000F, WORD_LEN, 0}, //000F TVAR_a{0xh_pGA{0x[502]       //
    {0x0F12,0x001B, WORD_LEN, 0}, //0020 TVAR_a{0xh_pGA{0x[503]       //
    {0x0F12,0x002F, WORD_LEN, 0}, //003A TVAR_a{0xh_pGA{0x[504]       //
    {0x0F12,0x0056, WORD_LEN, 0}, //0065 TVAR_a{0xh_pGA{0x[505]       //
    {0x0F12,0x0084, WORD_LEN, 0}, //009F TVAR_a{0xh_pGA{0x[506]       //
    {0x0F12,0x006F, WORD_LEN, 0}, //00B4 TVAR_a{0xh_pGA{0x[507]       //
    {0x0F12,0x0047, WORD_LEN, 0}, //0076 TVAR_a{0xh_pGA{0x[508]       //
    {0x0F12,0x0028, WORD_LEN, 0}, //004B TVAR_a{0xh_pGA{0x[509]       //
    {0x0F12,0x0018, WORD_LEN, 0}, //002E TVAR_a{0xh_pGA{0x[510]       //
    {0x0F12,0x000E, WORD_LEN, 0}, //001A TVAR_a{0xh_pGA{0x[511]       //
    {0x0F12,0x0008, WORD_LEN, 0}, //000D TVAR_a{0xh_pGA{0x[512]       //
    {0x0F12,0x0007, WORD_LEN, 0}, //0006 TVAR_a{0xh_pGA{0x[513]       //
    {0x0F12,0x000B, WORD_LEN, 0}, //000A TVAR_a{0xh_pGA{0x[514]       //
    {0x0F12,0x0014, WORD_LEN, 0}, //0014 TVAR_a{0xh_pGA{0x[515]       //
    {0x0F12,0x0021, WORD_LEN, 0}, //0024 TVAR_a{0xh_pGA{0x[516]       //
    {0x0F12,0x0035, WORD_LEN, 0}, //003E TVAR_a{0xh_pGA{0x[517]       //
    {0x0F12,0x005E, WORD_LEN, 0}, //0069 TVAR_a{0xh_pGA{0x[518]       //
    {0x0F12,0x008D, WORD_LEN, 0}, //00A3 TVAR_a{0xh_pGA{0x[519]       //
    {0x0F12,0x008C, WORD_LEN, 0}, //00CE TVAR_a{0xh_pGA{0x[520]       //
    {0x0F12,0x005F, WORD_LEN, 0}, //008D TVAR_a{0xh_pGA{0x[521]       //
    {0x0F12,0x003C, WORD_LEN, 0}, //005D TVAR_a{0xh_pGA{0x[522]       //
    {0x0F12,0x0029, WORD_LEN, 0}, //003E TVAR_a{0xh_pGA{0x[523]       //
    {0x0F12,0x001E, WORD_LEN, 0}, //002C TVAR_a{0xh_pGA{0x[524]       //
    {0x0F12,0x0019, WORD_LEN, 0}, //001D TVAR_a{0xh_pGA{0x[525]       //
    {0x0F12,0x0017, WORD_LEN, 0}, //0017 TVAR_a{0xh_pGA{0x[526]       //
    {0x0F12,0x001A, WORD_LEN, 0}, //0019 TVAR_a{0xh_pGA{0x[527]       //
    {0x0F12,0x0023, WORD_LEN, 0}, //0022 TVAR_a{0xh_pGA{0x[528]       //
    {0x0F12,0x002F, WORD_LEN, 0}, //0031 TVAR_a{0xh_pGA{0x[529]       //
    {0x0F12,0x0047, WORD_LEN, 0}, //004C TVAR_a{0xh_pGA{0x[530]       //
    {0x0F12,0x0073, WORD_LEN, 0}, //007B TVAR_a{0xh_pGA{0x[531]       //
    {0x0F12,0x00A5, WORD_LEN, 0}, //00B8 TVAR_a{0xh_pGA{0x[532]       //
    {0x0F12,0x00B3, WORD_LEN, 0}, //00F9 TVAR_a{0xh_pGA{0x[533]       //
    {0x0F12,0x0083, WORD_LEN, 0}, //00B2 TVAR_a{0xh_pGA{0x[534]       //
    {0x0F12,0x005A, WORD_LEN, 0}, //0079 TVAR_a{0xh_pGA{0x[535]       //
    {0x0F12,0x0043, WORD_LEN, 0}, //0059 TVAR_a{0xh_pGA{0x[536]       //
    {0x0F12,0x0037, WORD_LEN, 0}, //0044 TVAR_a{0xh_pGA{0x[537]       //
    {0x0F12,0x0031, WORD_LEN, 0}, //0036 TVAR_a{0xh_pGA{0x[538]       //
    {0x0F12,0x002F, WORD_LEN, 0}, //0030 TVAR_a{0xh_pGA{0x[539]       //
    {0x0F12,0x0031, WORD_LEN, 0}, //0031 TVAR_a{0xh_pGA{0x[540]       //
    {0x0F12,0x003A, WORD_LEN, 0}, //0039 TVAR_a{0xh_pGA{0x[541]       //
    {0x0F12,0x0048, WORD_LEN, 0}, //004A TVAR_a{0xh_pGA{0x[542]       //
    {0x0F12,0x0064, WORD_LEN, 0}, //0068 TVAR_a{0xh_pGA{0x[543]       //
    {0x0F12,0x0098, WORD_LEN, 0}, //009D TVAR_a{0xh_pGA{0x[544]       //
    {0x0F12,0x00CA, WORD_LEN, 0}, //00DD TVAR_a{0xh_pGA{0x[545]       //
    {0x0F12,0x00ED, WORD_LEN, 0}, //0131 TVAR_a{0xh_pGA{0x[546]       //
    {0x0F12,0x00BA, WORD_LEN, 0}, //00E9 TVAR_a{0xh_pGA{0x[547]       //
    {0x0F12,0x008A, WORD_LEN, 0}, //00A9 TVAR_a{0xh_pGA{0x[548]       //
    {0x0F12,0x006C, WORD_LEN, 0}, //0080 TVAR_a{0xh_pGA{0x[549]       //
    {0x0F12,0x005C, WORD_LEN, 0}, //0066 TVAR_a{0xh_pGA{0x[550]       //
    {0x0F12,0x0053, WORD_LEN, 0}, //0058 TVAR_a{0xh_pGA{0x[551]       //
    {0x0F12,0x0050, WORD_LEN, 0}, //0052 TVAR_a{0xh_pGA{0x[552]       //
    {0x0F12,0x0052, WORD_LEN, 0}, //0053 TVAR_a{0xh_pGA{0x[553]       //
    {0x0F12,0x005B, WORD_LEN, 0}, //005E TVAR_a{0xh_pGA{0x[554]       //
    {0x0F12,0x0070, WORD_LEN, 0}, //0073 TVAR_a{0xh_pGA{0x[555]       //
    {0x0F12,0x0094, WORD_LEN, 0}, //0097 TVAR_a{0xh_pGA{0x[556]       //
    {0x0F12,0x00CA, WORD_LEN, 0}, //00D2 TVAR_a{0xh_pGA{0x[557]       //
    {0x0F12,0x00EF, WORD_LEN, 0}, //0110 TVAR_a{0xh_pGA{0x[558]       //
    {0x0F12,0x011D, WORD_LEN, 0}, //0150 TVAR_a{0xh_pGA{0x[559]       //
    {0x0F12,0x00FB, WORD_LEN, 0}, //0124 TVAR_a{0xh_pGA{0x[560]       //
    {0x0F12,0x00C6, WORD_LEN, 0}, //00E0 TVAR_a{0xh_pGA{0x[561]       //
    {0x0F12,0x00A3, WORD_LEN, 0}, //00B0 TVAR_a{0xh_pGA{0x[562]       //
    {0x0F12,0x008C, WORD_LEN, 0}, //0090 TVAR_a{0xh_pGA{0x[563]       //
    {0x0F12,0x007F, WORD_LEN, 0}, //007E TVAR_a{0xh_pGA{0x[564]       //
    {0x0F12,0x0079, WORD_LEN, 0}, //0077 TVAR_a{0xh_pGA{0x[565]       //
    {0x0F12,0x007B, WORD_LEN, 0}, //0079 TVAR_a{0xh_pGA{0x[566]       //
    {0x0F12,0x0088, WORD_LEN, 0}, //0087 TVAR_a{0xh_pGA{0x[567]       //
    {0x0F12,0x00A2, WORD_LEN, 0}, //00A2 TVAR_a{0xh_pGA{0x[568]       //
    {0x0F12,0x00CC, WORD_LEN, 0}, //00CD TVAR_a{0xh_pGA{0x[569]       //
    {0x0F12,0x00FB, WORD_LEN, 0}, //0106 TVAR_a{0xh_pGA{0x[570]       //
    {0x0F12,0x010A, WORD_LEN, 0}, //0137 TVAR_a{0xh_pGA{0x[571]       //
    {0x002A,0x0704, WORD_LEN, 0}, // 5. {0xhading Alpha //
    {0x0F12,0x00ED, WORD_LEN, 0}, // param_{0xtart	TVAR_a{0xh_AwbA{0xhCord //
    {0x0F12,0x0124, WORD_LEN, 0},
    {0x0F12,0x012B, WORD_LEN, 0},
    {0x0F12,0x014A, WORD_LEN, 0},
    {0x0F12,0x0190, WORD_LEN, 0},
    {0x0F12,0x01B2, WORD_LEN, 0},
    {0x0F12,0x01C4, WORD_LEN, 0}, // param_end	TVAR_a{0xh_AwbA{0xhCord //
    {0x0F12,0x0120, WORD_LEN, 0}, //a 012B // a param_{0xtart	TVAR_a{0xh_GA{0xAlpha // //TVAR_a{0xh_GA{0xAlpha[0]    //
    {0x0F12,0x0100, WORD_LEN, 0}, //0106 //TVAR_a{0xh_GA{0xAlpha[1]    //
    {0x0F12,0x0100, WORD_LEN, 0}, //010A //TVAR_a{0xh_GA{0xAlpha[2]    //
    {0x0F12,0x0100, WORD_LEN, 0}, //00EE //TVAR_a{0xh_GA{0xAlpha[3]    //
    {0x0F12,0x0120, WORD_LEN, 0}, //a 012b //a 200 TVAR_a{0xh_GA{0xAlpha[4] 0507   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00FC //TVAR_a{0xh_GA{0xAlpha[5]    //
    {0x0F12,0x0100, WORD_LEN, 0}, //00FE //TVAR_a{0xh_GA{0xAlpha[6]    //
    {0x0F12,0x0100, WORD_LEN, 0}, //0100 //TVAR_a{0xh_GA{0xAlpha[7]    //
    {0x0F12,0x0100, WORD_LEN, 0}, //cw 011B //TVAR_a{0xh_GA{0xAlpha[8]    //
    {0x0F12,0x0100, WORD_LEN, 0}, //0107 //TVAR_a{0xh_GA{0xAlpha[9]    //
    {0x0F12,0x0100, WORD_LEN, 0}, //0109 //TVAR_a{0xh_GA{0xAlpha[10]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00FF //TVAR_a{0xh_GA{0xAlpha[11]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //D50? 00DB //TVAR_a{0xh_GA{0xAlpha[12]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00FF //TVAR_a{0xh_GA{0xAlpha[13]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //0100 //TVAR_a{0xh_GA{0xAlpha[14]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00FB //TVAR_a{0xh_GA{0xAlpha[15]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //d65 cw 0100 //TVAR_a{0xh_GA{0xAlpha[16]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //0103 //TVAR_a{0xh_GA{0xAlpha[17]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //0101 //TVAR_a{0xh_GA{0xAlpha[18]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //010C //TVAR_a{0xh_GA{0xAlpha[19]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //d65 0110 //TVAR_a{0xh_GA{0xAlpha[20]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00FD //TVAR_a{0xh_GA{0xAlpha[21]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00FD //TVAR_a{0xh_GA{0xAlpha[22]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //0109 //TVAR_a{0xh_GA{0xAlpha[23]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00D4 //TVAR_a{0xh_GA{0xAlpha[24]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00FB //TVAR_a{0xh_GA{0xAlpha[25]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00F8 //TVAR_a{0xh_GA{0xAlpha[26]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //0107 //	param_end	TVAR_a{0xh_GA{0xAlpha // //TVAR_a{0xh_GA{0xAlpha[27]   //
    {0x0F12,0x0100, WORD_LEN, 0}, //00F0 //	param_{0xtart	TVAR_a{0xh_GA{0xOutdoorAlpha // //00FB	  TVAR_a{0xh_GA{0xOutdoorAlpha[0] //
    {0x0F12,0x0100, WORD_LEN, 0}, //0103 //TVAR_a{0xh_GA{0xOutdoorAlpha[1]        //
    {0x0F12,0x0100, WORD_LEN, 0}, //0101 //TVAR_a{0xh_GA{0xOutdoorAlpha[2]        //
    {0x0F12,0x0100, WORD_LEN, 0}, //010C //	param_end	TVAR_a{0xh_GA{0xOutdoorAlpha // //TVAR_a{0xh_GA{0xOutdoorAlpha[3]        //
    {0x002A,0x075A, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0280, WORD_LEN, 0},
    {0x0F12,0x0200, WORD_LEN, 0},
    {0x0F12,0x000E, WORD_LEN, 0},
    {0x0F12,0x000F, WORD_LEN, 0},
    {0x002A,0x04C8, WORD_LEN, 0}, // 6. Gamma // // param_{0xtart	{0xARR_u{0xGammaLutRGBIndoor //
    {0x0F12,0x0000, WORD_LEN, 0}, //0000   //
    {0x0F12,0x0004, WORD_LEN, 0}, //0007   //
    {0x0F12,0x0008, WORD_LEN, 0}, //000D   //
    {0x0F12,0x001a, WORD_LEN, 0}, //0013   //
    {0x0F12,0x0068, WORD_LEN, 0}, //0079   //
    {0x0F12,0x00fe, WORD_LEN, 0}, //00FE   //
    {0x0F12,0x0159, WORD_LEN, 0}, //0159   //
    {0x0F12,0x01A1, WORD_LEN, 0}, //01A1   //
    {0x0F12,0x0210, WORD_LEN, 0}, //0210   //
    {0x0F12,0x0263, WORD_LEN, 0}, //0263   //
    {0x0F12,0x02D5, WORD_LEN, 0}, //02D5   //
    {0x0F12,0x0330, WORD_LEN, 0}, //0330   //
    {0x0F12,0x0377, WORD_LEN, 0}, //0377   //
    {0x0F12,0x03BE, WORD_LEN, 0}, //03BE   //
    {0x0F12,0x03F0, WORD_LEN, 0}, //03F0   //
    {0x0F12,0x0400, WORD_LEN, 0}, //0400   //
    {0x0F12,0x0000, WORD_LEN, 0}, //0000   //
    {0x0F12,0x0004, WORD_LEN, 0}, //0007   //
    {0x0F12,0x0008, WORD_LEN, 0}, //000D   //
    {0x0F12,0x001a, WORD_LEN, 0}, //0013   //
    {0x0F12,0x0068, WORD_LEN, 0}, //0079   //
    {0x0F12,0x00fe, WORD_LEN, 0}, //00FE   //
    {0x0F12,0x0159, WORD_LEN, 0}, //0159   //
    {0x0F12,0x01A1, WORD_LEN, 0}, //01A1   //
    {0x0F12,0x0210, WORD_LEN, 0}, //0210   //
    {0x0F12,0x0263, WORD_LEN, 0}, //0263   //
    {0x0F12,0x02D5, WORD_LEN, 0}, //02D5   //
    {0x0F12,0x0330, WORD_LEN, 0}, //0330   //
    {0x0F12,0x0377, WORD_LEN, 0}, //0377   //
    {0x0F12,0x03BE, WORD_LEN, 0}, //03BE   //
    {0x0F12,0x03F0, WORD_LEN, 0}, //03F0   //
    {0x0F12,0x0400, WORD_LEN, 0}, //0400   //
    {0x0F12,0x0000, WORD_LEN, 0}, //0000   //
    {0x0F12,0x0004, WORD_LEN, 0}, //0007   //
    {0x0F12,0x0008, WORD_LEN, 0}, //000D   //
    {0x0F12,0x001a, WORD_LEN, 0}, //0013   //
    {0x0F12,0x0068, WORD_LEN, 0}, //0079   //
    {0x0F12,0x00fe, WORD_LEN, 0}, //00FE   //
    {0x0F12,0x0159, WORD_LEN, 0}, //0159   //
    {0x0F12,0x01A1, WORD_LEN, 0}, //01A1   //
    {0x0F12,0x0210, WORD_LEN, 0}, //0210   //
    {0x0F12,0x0263, WORD_LEN, 0}, //0263   //
    {0x0F12,0x02D5, WORD_LEN, 0}, //02D5   //
    {0x0F12,0x0330, WORD_LEN, 0}, //0330   //
    {0x0F12,0x0377, WORD_LEN, 0}, //0377   //
    {0x0F12,0x03BE, WORD_LEN, 0}, //03BE   //
    {0x0F12,0x03F0, WORD_LEN, 0}, //03F0   //
    {0x0F12,0x0400, WORD_LEN, 0}, //	param_end	{0xARR_u{0xGammaLutRGBIndoor // //0400   //
    {0x002A,0x0C50, WORD_LEN, 0}, // 7. AWB // // param_{0xtart	awbb_IndoorGrZone{0x_m_BGrid //
    {0x0F12,0x03B8, WORD_LEN, 0}, //03B8  //
    {0x0F12,0x03C8, WORD_LEN, 0}, //03C8  //
    {0x0F12,0x0384, WORD_LEN, 0}, //0384  //
    {0x0F12,0x03D0, WORD_LEN, 0}, //03D0  //
    {0x0F12,0x035E, WORD_LEN, 0}, //035E  //
    {0x0F12,0x03CC, WORD_LEN, 0}, //03CC  //
    {0x0F12,0x033E, WORD_LEN, 0}, //033E  //
    {0x0F12,0x03B2, WORD_LEN, 0}, //03B2  //
    {0x0F12,0x0322, WORD_LEN, 0}, //0322  //
    {0x0F12,0x0396, WORD_LEN, 0}, //0396  //
    {0x0F12,0x030C, WORD_LEN, 0}, //030C  //
    {0x0F12,0x0380, WORD_LEN, 0}, //0380  //
    {0x0F12,0x02F8, WORD_LEN, 0}, //02F8  //
    {0x0F12,0x0368, WORD_LEN, 0}, //0368  //
    {0x0F12,0x02DC, WORD_LEN, 0}, //02DC  //
    {0x0F12,0x0352, WORD_LEN, 0}, //034A  //
    {0x0F12,0x02C2, WORD_LEN, 0}, //02C2  //
    {0x0F12,0x033C, WORD_LEN, 0}, //0336  //
    {0x0F12,0x02AE, WORD_LEN, 0}, //02AE  //
    {0x0F12,0x032A, WORD_LEN, 0}, //031E  //
    {0x0F12,0x029A, WORD_LEN, 0}, //029A  //
    {0x0F12,0x031C, WORD_LEN, 0}, //0306  //
    {0x0F12,0x028C, WORD_LEN, 0}, //028C  //
    {0x0F12,0x030A, WORD_LEN, 0}, //02F4  //
    {0x0F12,0x027C, WORD_LEN, 0}, //027C  //
    {0x0F12,0x02FC, WORD_LEN, 0}, //02DC  //
    {0x0F12,0x0264, WORD_LEN, 0}, //0264  //
    {0x0F12,0x02EC, WORD_LEN, 0}, //02C2  //
    {0x0F12,0x0252, WORD_LEN, 0}, //0252  //
    {0x0F12,0x02DE, WORD_LEN, 0}, //02AE  //
    {0x0F12,0x0246, WORD_LEN, 0}, //0246  //
    {0x0F12,0x02CC, WORD_LEN, 0}, //029C  //
    {0x0F12,0x023C, WORD_LEN, 0}, //023C  //
    {0x0F12,0x02C2, WORD_LEN, 0}, //028A  //
    {0x0F12,0x022E, WORD_LEN, 0}, //022E  //
    {0x0F12,0x02B4, WORD_LEN, 0}, //027E  //
    {0x0F12,0x0222, WORD_LEN, 0}, //0222  //
    {0x0F12,0x02A8, WORD_LEN, 0}, //0272  //
    {0x0F12,0x0212, WORD_LEN, 0}, //0212  //
    {0x0F12,0x029C, WORD_LEN, 0}, //0266  //
    {0x0F12,0x0202, WORD_LEN, 0}, //0202  //
    {0x0F12,0x0292, WORD_LEN, 0}, //025A  //
    {0x0F12,0x01FA, WORD_LEN, 0}, //01FA  //
    {0x0F12,0x0288, WORD_LEN, 0}, //0250  //
    {0x0F12,0x01EC, WORD_LEN, 0}, //01EC  //
    {0x0F12,0x027E, WORD_LEN, 0}, //0246  //
    {0x0F12,0x01E6, WORD_LEN, 0}, //01E6  //
    {0x0F12,0x0272, WORD_LEN, 0}, //023C  //
    {0x0F12,0x01DC, WORD_LEN, 0}, //01DC  //
    {0x0F12,0x0264, WORD_LEN, 0}, //0234  //
    {0x0F12,0x01D4, WORD_LEN, 0}, //01D4  //
    {0x0F12,0x0256, WORD_LEN, 0}, //0228  //
    {0x0F12,0x01CE, WORD_LEN, 0}, //01CE  //
    {0x0F12,0x0248, WORD_LEN, 0}, //0220  //
    {0x0F12,0x01C6, WORD_LEN, 0}, //01C6  //
    {0x0F12,0x023E, WORD_LEN, 0}, //0216  //
    {0x0F12,0x01C0, WORD_LEN, 0}, //01C0  //
    {0x0F12,0x022E, WORD_LEN, 0}, //020C  //
    {0x0F12,0x01BE, WORD_LEN, 0}, //01BE  //
    {0x0F12,0x0222, WORD_LEN, 0}, //01FC  //
    {0x0F12,0x01C4, WORD_LEN, 0}, //01C4  //
    {0x0F12,0x020E, WORD_LEN, 0}, //01EE  //
    {0x0F12,0x01D0, WORD_LEN, 0}, //01D0  //
    {0x0F12,0x01E0, WORD_LEN, 0}, //01E0  //
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
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0004, WORD_LEN, 0}, // param_end	awbb_IndoorGrZone{0x_m_BGrid ////awbb_IndoorGrZone{0x_m_Grid{0xtep//
    {0x0F12,0x0000, WORD_LEN, 0}, 
    {0x002A,0x0CF8, WORD_LEN, 0}, 
    {0x0F12,0x010F, WORD_LEN, 0}, //awbb_IndoorGrZone{0x_m_Boff{0x//
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0D84, WORD_LEN, 0}, // param_{0xtart	awbb_LowBrGrZone{0x_m_BGrid //
    {0x0F12,0x0406, WORD_LEN, 0},
    {0x0F12,0x0467, WORD_LEN, 0},
    {0x0F12,0x0371, WORD_LEN, 0},
    {0x0F12,0x04B0, WORD_LEN, 0},
    {0x0F12,0x02E5, WORD_LEN, 0},
    {0x0F12,0x0481, WORD_LEN, 0},
    {0x0F12,0x0298, WORD_LEN, 0},
    {0x0F12,0x042E, WORD_LEN, 0},
    {0x0F12,0x0260, WORD_LEN, 0},
    {0x0F12,0x03DE, WORD_LEN, 0},
    {0x0F12,0x022F, WORD_LEN, 0},
    {0x0F12,0x0391, WORD_LEN, 0},
    {0x0F12,0x0201, WORD_LEN, 0},
    {0x0F12,0x034D, WORD_LEN, 0},
    {0x0F12,0x01DA, WORD_LEN, 0},
    {0x0F12,0x0310, WORD_LEN, 0},
    {0x0F12,0x01B3, WORD_LEN, 0},
    {0x0F12,0x02D4, WORD_LEN, 0},
    {0x0F12,0x018F, WORD_LEN, 0},
    {0x0F12,0x0297, WORD_LEN, 0},
    {0x0F12,0x0181, WORD_LEN, 0},
    {0x0F12,0x0271, WORD_LEN, 0},
    {0x0F12,0x0181, WORD_LEN, 0},
    {0x0F12,0x022A, WORD_LEN, 0},
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
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0}, // param_end	awbb_LowBrGrZone{0x_m_BGrid //
    {0x0F12,0x0006, WORD_LEN, 0}, //awbb_LowBrGrZone{0x_m_Grid{0xtep//
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0DF0, WORD_LEN, 0},
    {0x0F12,0x0081, WORD_LEN, 0}, //awbb_LowBrGrZone{0x_m_Boff{0x//
    {0x0F12,0x0000, WORD_LEN, 0}, // param_{0xtart	awbb_OutdoorGrZone{0x_m_BGrid //
    {0x002A,0x0D08, WORD_LEN, 0},
    {0x0F12,0x0258, WORD_LEN, 0}, //250,	awbb_OutdoorGrZone{0x_m_BGrid[0]      //
    {0x0F12,0x0274, WORD_LEN, 0}, //260,	awbb_OutdoorGrZone{0x_m_BGrid[1]      //
    {0x0F12,0x0240, WORD_LEN, 0}, //245,	awbb_OutdoorGrZone{0x_m_BGrid[2]      //
    {0x0F12,0x027E, WORD_LEN, 0}, //26B,	awbb_OutdoorGrZone{0x_m_BGrid[3]      //
    {0x0F12,0x0234, WORD_LEN, 0}, //23A,	awbb_OutdoorGrZone{0x_m_BGrid[4]      //
    {0x0F12,0x0280, WORD_LEN, 0}, //26D,	awbb_OutdoorGrZone{0x_m_BGrid[5]      //
    {0x0F12,0x022A, WORD_LEN, 0}, //230,	awbb_OutdoorGrZone{0x_m_BGrid[6]      //
    {0x0F12,0x0282, WORD_LEN, 0}, //26F,	awbb_OutdoorGrZone{0x_m_BGrid[7]      //
    {0x0F12,0x021E, WORD_LEN, 0}, //226,	awbb_OutdoorGrZone{0x_m_BGrid[8]      //
    {0x0F12,0x0282, WORD_LEN, 0}, //26F,	awbb_OutdoorGrZone{0x_m_BGrid[9]      //
    {0x0F12,0x0214, WORD_LEN, 0}, //221,	awbb_OutdoorGrZone{0x_m_BGrid[10]     //
    {0x0F12,0x027E, WORD_LEN, 0}, //26A,	awbb_OutdoorGrZone{0x_m_BGrid[11]     //
    {0x0F12,0x020E, WORD_LEN, 0}, //21D,	awbb_OutdoorGrZone{0x_m_BGrid[12]     //
    {0x0F12,0x0278, WORD_LEN, 0}, //264,	awbb_OutdoorGrZone{0x_m_BGrid[13]     //
    {0x0F12,0x020A, WORD_LEN, 0}, //217,	awbb_OutdoorGrZone{0x_m_BGrid[14]     //
    {0x0F12,0x0274, WORD_LEN, 0}, //260,	awbb_OutdoorGrZone{0x_m_BGrid[15]     //
    {0x0F12,0x0204, WORD_LEN, 0}, //210,	awbb_OutdoorGrZone{0x_m_BGrid[16]     //
    {0x0F12,0x026C, WORD_LEN, 0}, //259,	awbb_OutdoorGrZone{0x_m_BGrid[17]     //
    {0x0F12,0x0200, WORD_LEN, 0}, //20A,	awbb_OutdoorGrZone{0x_m_BGrid[18]     //
    {0x0F12,0x0264, WORD_LEN, 0}, //251,	awbb_OutdoorGrZone{0x_m_BGrid[19]     //
    {0x0F12,0x01FE, WORD_LEN, 0}, //204,	awbb_OutdoorGrZone{0x_m_BGrid[20]     //
    {0x0F12,0x025E, WORD_LEN, 0}, //24B,	awbb_OutdoorGrZone{0x_m_BGrid[21]     //
    {0x0F12,0x01FE, WORD_LEN, 0}, //1FD,	awbb_OutdoorGrZone{0x_m_BGrid[22]     //
    {0x0F12,0x0258, WORD_LEN, 0}, //245,	awbb_OutdoorGrZone{0x_m_BGrid[23]     //
    {0x0F12,0x01FE, WORD_LEN, 0}, //1F6,	awbb_OutdoorGrZone{0x_m_BGrid[24]     //
    {0x0F12,0x0252, WORD_LEN, 0}, //23E,	awbb_OutdoorGrZone{0x_m_BGrid[25]     //
    {0x0F12,0x0200, WORD_LEN, 0}, //1F1,	awbb_OutdoorGrZone{0x_m_BGrid[26]     //
    {0x0F12,0x0238, WORD_LEN, 0}, //238,	awbb_OutdoorGrZone{0x_m_BGrid[27]     //
    {0x0F12,0x0204, WORD_LEN, 0}, //1EC,	awbb_OutdoorGrZone{0x_m_BGrid[28]     //
    {0x0F12,0x0224, WORD_LEN, 0}, //22F,	awbb_OutdoorGrZone{0x_m_BGrid[29]     //
    {0x0F12,0x0000, WORD_LEN, 0}, //1E9,	awbb_OutdoorGrZone{0x_m_BGrid[30]     //
    {0x0F12,0x0000, WORD_LEN, 0}, //219,	awbb_OutdoorGrZone{0x_m_BGrid[31]     //
    {0x0F12,0x0000, WORD_LEN, 0}, //1EF,	awbb_OutdoorGrZone{0x_m_BGrid[32]     //
    {0x0F12,0x0000, WORD_LEN, 0}, //1F8,	awbb_OutdoorGrZone{0x_m_BGrid[33]     //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[34]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[35]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[36]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[37]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[38]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[39]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[40]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[41]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[42]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[43]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[44]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[45]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[46]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[47]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[48]            //
    {0x0F12,0x0000, WORD_LEN, 0}, //	param_end	awbb_OutdoorGrZone{0x_m_BGrid // //awbb_OutdoorGrZone{0x_m_BGrid[49]            //
    {0x0F12,0x0003, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_Grid{0xtep//
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0D70, WORD_LEN, 0},
    {0x0F12,0x000F, WORD_LEN, 0}, //0011//
    {0x002A,0x0D74, WORD_LEN, 0},
    {0x0F12,0x021f, WORD_LEN, 0}, //2F, awbb_OutdoorGrZone{0x_m_Boff{0x//
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0E00, WORD_LEN, 0},
    {0x0F12,0x034A, WORD_LEN, 0}, //awbb_CrclLowT_R_c                //
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0176, WORD_LEN, 0}, //awbb_CrclLowT_B_c                //
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x71B8, WORD_LEN, 0}, //awbb_CrclLowT_Rad_c              //
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0E1A, WORD_LEN, 0},
    {0x0F12,0x012F, WORD_LEN, 0},
    {0x0F12,0x0120, WORD_LEN, 0},
    {0x002A,0x0E68, WORD_LEN, 0}, // awbb_LowTempRB //
    {0x0F12,0x04F2, WORD_LEN, 0},
    {0x002A,0x0D78, WORD_LEN, 0},
    {0x0F12,0x0020, WORD_LEN, 0}, //AWB min.	                                   //
    {0x002A,0x0D80, WORD_LEN, 0},
    {0x0F12,0x00E0, WORD_LEN, 0}, //AWB Max.	                                   //
    {0x002A,0x0E40, WORD_LEN, 0}, //awbb_U{0xe_Filter{0x                            //
    {0x0F12,0x0061, WORD_LEN, 0}, //AWB option	                               //
    {0x002A,0x0EE4, WORD_LEN, 0},
    {0x0F12,0x0003, WORD_LEN, 0}, //awbb_OutdoorFltr{0xz                          //
    {0x002A,0x0E3C, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0}, //awbb_U{0xe_InvalidOutDoor                     //
    {0x002A,0x0F3A, WORD_LEN, 0},
    {0x0F12,0x024C, WORD_LEN, 0}, //awbb_OutdoorWP_r	   //
    {0x0F12,0x0290, WORD_LEN, 0}, //awbb_OutdoorWP_b	   //
    {0x002A,0x0E46, WORD_LEN, 0},
    {0x0F12,0x0bb8, WORD_LEN, 0}, //12c0 //0bb8 //awbb_{0xunnyBr	  	                       //
    {0x0F12,0x0096, WORD_LEN, 0}, //awbb_{0xunny_NBzone  	                   //
    {0x0F12,0x0BB8, WORD_LEN, 0}, //12c0 //0BB8 //awbb_CloudyBr	                       //
    {0x002A,0x0E5E, WORD_LEN, 0},
    {0x0F12,0x071A, WORD_LEN, 0}, //awbb_GamutWidthThr1                         //
    {0x0F12,0x03A4, WORD_LEN, 0},
    {0x002A,0x0E50, WORD_LEN, 0},
    {0x0F12,0x001B, WORD_LEN, 0}, //awbb_MacbethGamut_WidthZone                 //
    {0x0F12,0x000E, WORD_LEN, 0},
    {0x0F12,0x0008, WORD_LEN, 0},
    {0x0F12,0x0004, WORD_LEN, 0},
    {0x002A,0x0E36, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0}, //awbb_ByPa{0x{0x_LowTempMode                     //  
    {0x002a,0x0e18, WORD_LEN, 0},
    {0x0f12,0x0000, WORD_LEN, 0}, // 32 awbb_dark  //
    {0x002A,0x0E3A, WORD_LEN, 0}, // AWB etc //
    {0x0F12,0x02C2, WORD_LEN, 0}, //awbb_Alpha_Comp_Mode                   //
    {0x002A,0x0F12, WORD_LEN, 0},
    {0x0F12,0x02C9, WORD_LEN, 0}, //awbb_GLocu{0xR                           //
    {0x0F12,0x033F, WORD_LEN, 0}, //awbb_GLocu{0xB                           //
    {0x002A,0x0E1A, WORD_LEN, 0},
    {0x0F12,0x0138, WORD_LEN, 0}, //awbb_IntcR                             //
    {0x002A,0x236C, WORD_LEN, 0}, //2180//
    {0x0F12,0x0000, WORD_LEN, 0}, //AWBBTune_EVT4_uInitPo{0xtToleranceCnt    //
    {0x002A,0x0c48, WORD_LEN, 0}, // AWB {0xtart Point // //awbb_Gain{0xInit //
    {0x0F12,0x053C, WORD_LEN, 0}, //R Gain         //
    {0x0F12,0x03b0, WORD_LEN, 0}, //400,           //
    {0x0F12,0x055C, WORD_LEN, 0}, //B Gain         //
    {0x002A,0x0E42, WORD_LEN, 0}, // 8. Grid Correction //
    {0x0F12,0x0002, WORD_LEN, 0},
    {0x002A,0x0EE0, WORD_LEN, 0},
    {0x0F12,0x00B5, WORD_LEN, 0}, //awbb_GridCoeff_R_2                                          //
    {0x0F12,0x00B5, WORD_LEN, 0}, //awbb_GridCoeff_B_2                                          //
    {0x002A,0x0ED0, WORD_LEN, 0}, //                                                            //
    {0x0F12,0x0EC8, WORD_LEN, 0}, //awbb_GridCon{0xt_2[0]                                         //
    {0x0F12,0x1022, WORD_LEN, 0}, //awbb_GridCon{0xt_2[1]                                         //
    {0x0F12,0x10BB, WORD_LEN, 0}, //awbb_GridCon{0xt_2[2]                                         //
    {0x0F12,0x10C9, WORD_LEN, 0}, //awbb_GridCon{0xt_2[3]                                         //
    {0x0F12,0x1149, WORD_LEN, 0}, //awbb_GridCon{0xt_2[4]                                         //
    {0x0F12,0x11FD, WORD_LEN, 0}, //awbb_GridCon{0xt_2[5]                                         //
    {0x0F12,0x00B8, WORD_LEN, 0}, //awbb_GridCoeff_R_1                                          //
    {0x0F12,0x00B2, WORD_LEN, 0}, //awbb_GridCoeff_B_1                                          //
    {0x002A,0x0ECA, WORD_LEN, 0}, //                                                            //
    {0x0F12,0x029A, WORD_LEN, 0}, //awbb_GridCon{0xt_1[0]                                         //
    {0x0F12,0x0344, WORD_LEN, 0}, //awbb_GridCon{0xt_1[1]                                         //
    {0x0F12,0x03FB, WORD_LEN, 0}, //awbb_GridCon{0xt_1[2]                                         //
    {0x002A,0x0E82, WORD_LEN, 0},
    {0x0F12,0xfff0, WORD_LEN, 0}, //e0 //ffe0 //00000000 //0000 //0000 //0028  //awbb_GridCorr_R[0][0] //d65
    {0x0F12,0xfff0, WORD_LEN, 0}, //d0 //ffd0 //0000 //0000 //0000 //0028  //awbb_GridCorr_R[0][1]  //d50
    {0x0F12,0x0000, WORD_LEN, 0}, //cw 0010 //0000 //0000 //0000  //awbb_GridCorr_R[0][2]  //cw
    {0x0F12,0xffc8, WORD_LEN, 0}, //ffe0 0010 //0000 //0000 //0000  //awbb_GridCorr_R[0][3]  //ww
    {0x0F12,0xffc8, WORD_LEN, 0}, //ffe0 ff80 //70 //70 //ff70 //90 //ff70 //ffa0 //ffd0 //ff90 //FFC0 //0000  //awbb_GridCorr_R[0][4]  //a 
    {0x0F12,0xffc8, WORD_LEN, 0}, //ffe0 ff90 //70 //80 //ff70 //ffa0 //ffd0 //0090 //00C8 //58 //a8 //C8  //awbb_GridCorr_R[0][5] //h   vt 100411
    {0x0F12,0xfff0, WORD_LEN, 0}, //e0 //ffe0 //ffe0 //00000000 //0000 //0000 //0028  //awbb_GridCorr_R[1][0]
    {0x0F12,0xfff0, WORD_LEN, 0}, //d0 //ffd0 //0000 //0000 //0000 //0028  //awbb_GridCorr_R[1][1]
    {0x0F12,0x0000, WORD_LEN, 0}, //cw 0010 //0000 //0000 //0000  //awbb_GridCorr_R[1][2]
    {0x0F12,0xffc8, WORD_LEN, 0}, //0010 //0000 //0000 //0000  //awbb_GridCorr_R[1][3]
    {0x0F12,0xffc8, WORD_LEN, 0}, //ff80 //70 //70 //ff70 //90 //ff70 //ffa0 //ffd0 //ff90 //FFC0 //0000  //awbb_GridCorr_R[1][4]
    {0x0F12,0xffc8, WORD_LEN, 0}, //ff90//70 //80 //ff70 //ff70 //ffa0 //ffd0 //0090 //00C8 //58 //a8 //C8  //awbb_GridCorr_R[1][5]
    {0x0F12,0xfff0, WORD_LEN, 0}, //e0 //ffe0 //ffe0 //00000000 //0000 //0000 //0028  //awbb_GridCorr_R[2][0]
    {0x0F12,0xfff0, WORD_LEN, 0}, //d0 //ffd0 //0000 //0000 //0000 //0028  //awbb_GridCorr_R[2][1]
    {0x0F12,0x0000, WORD_LEN, 0}, //cw 0010 //0000 //0000 //0000  //awbb_GridCorr_R[2][2]
    {0x0F12,0xffc8, WORD_LEN, 0}, //0010 //0000 //0000 //0000  //awbb_GridCorr_R[2][3]
    {0x0F12,0xffc8, WORD_LEN, 0}, //ff80 //70 //70 //ff70 //90 //ff70 //ffa0 //ffd0 //ff90 //FFC0 //0000  //awbb_GridCorr_R[2][4]
    {0x0F12,0xffc8, WORD_LEN, 0}, //ff90//70 //80 //ff70 //ff70 //ffa0 //ffd0 //0090 //00C8 //58 //a8 //C8  //awbb_GridCorr_R[2][5]
    {0x0F12,0x0020, WORD_LEN, 0}, //10 //0010 //FFE0 //FFE0 //FFE0 //FF9C //FFC4  //awbb_GridCorr_B[0][0]
    {0x0F12,0x0000, WORD_LEN, 0}, //E0 //FFE0 //FFE0 //FFE0 //FF9C //FFC4  //awbb_GridCorr_B[0][1]
    {0x0F12,0x0050, WORD_LEN, 0}, //0010 //0010 //0000 //0000 //0000 //0000  //awbb_GridCorr_B[0][2]
    {0x0F12,0x0080, WORD_LEN, 0}, //0020 //0010 //0000 //0000 //0000 ///awbb_GridCorr_B[0][3]
    {0x0F12,0x0080, WORD_LEN, 0}, //0090 //0080 //0040 //0030 //0000 //FF6A //6A  //awbb_GridCorr_B[0][4]
    {0x0F12,0x0080, WORD_LEN, 0}, //0090 //0080 //0040 //FD80 //FD44 //fe00 //FD84 //44  //awbb_GridCorr_B[0][5]
    {0x0F12,0x0020, WORD_LEN, 0}, //10 //0010 //0010 //FFE0 //FFE0 //FFE0 //FF9C //FFC4  //awbb_GridCorr_B[1][0]
    {0x0F12,0x0000, WORD_LEN, 0}, //E0 //FFE0 //FFE0 //FFE0 //FFE0 //FF9C //FFC4  //awbb_GridCorr_B[1][1]
    {0x0F12,0x0050, WORD_LEN, 0}, //0010 //0010 //0000 //0000 //0000 //0000 //0000  //awbb_GridCorr_B[1][2]
    {0x0F12,0x0080, WORD_LEN, 0}, //0020 //0010 //0000 //0000 //0000 //0000  //awbb_GridCorr_B[1][3]
    {0x0F12,0x0080, WORD_LEN, 0}, //0090 //0090 //0080 //0040 //0030 //0000 //FF6A //6A  //awbb_GridCorr_B[1][4]
    {0x0F12,0x0080, WORD_LEN, 0}, //0090 //0090 //0080 //0040 //FD80 //FD44 //FD44 //fe00 //FD84 //44  //awbb_GridCorr_B[1][5]
    {0x0F12,0x0020, WORD_LEN, 0}, //10 //0010 //0010 //FFE0 //FFE0 //FFE0 //FF9C //FFC4  //awbb_GridCorr_B[2][0]
    {0x0F12,0x0000, WORD_LEN, 0}, //E0 //FFE0 //FFE0 //FFE0 //FFE0 //FF9C //FFC4  //awbb_GridCorr_B[2][1]
    {0x0F12,0x0050, WORD_LEN, 0}, //0010 //0010 //0000 //0000 //0000 //0000 //0000  //awbb_GridCorr_B[2][2]
    {0x0F12,0x0080, WORD_LEN, 0}, //0020 //0010 //0000 //0000 //0000 //0000  //awbb_GridCorr_B[2][3]
    {0x0F12,0x0080, WORD_LEN, 0}, //0090 //0090 //0080 //0040 //0030 //0000 //FF6A //6A  //awbb_GridCorr_B[2][4]
    {0x0F12,0x0080, WORD_LEN, 0}, //0090 //0090 //0080 //0040 //FD80 //FD44 //FD44 //fe00 //FD84 //44  //awbb_GridCorr_B[2][5]
    {0x002A,0x06D4, WORD_LEN, 0}, // 9. CCM //
    {0x0F12,0x2380, WORD_LEN, 0}, //TVAR_wbt_pOutdoorCcm//
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x002A,0x06CC, WORD_LEN, 0},
    {0x0F12,0x23A4, WORD_LEN, 0}, //TVAR_wbt_pBa{0xeCcm{0x  //
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x002A,0x06E8, WORD_LEN, 0},
    {0x0F12,0x23A4, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x23C8, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x23EC, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x2410, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x2434, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x0F12,0x2458, WORD_LEN, 0},
    {0x0F12,0x7000, WORD_LEN, 0},
    {0x002A,0x06DA, WORD_LEN, 0},
    {0x0F12,0x00BF, WORD_LEN, 0}, //{0xARR_AwbCcmCord[0]  2   700006DA   //
    {0x0F12,0x00E6, WORD_LEN, 0}, //{0xARR_AwbCcmCord[1]  2   700006DC   //
    {0x0F12,0x00F2, WORD_LEN, 0}, //{0xARR_AwbCcmCord[2]  2   700006DE   //
    {0x0F12,0x0143, WORD_LEN, 0}, //{0xARR_AwbCcmCord[3]  2   700006E0   //
    {0x0F12,0x0178, WORD_LEN, 0}, //{0xARR_AwbCcmCord[4]  2   700006E2   //
    {0x0F12,0x01A3, WORD_LEN, 0}, //{0xARR_AwbCcmCord[5]                 //
    {0x002A,0x23A4, WORD_LEN, 0}, //	par,0xam_{},0xtart	TVAR_wbt_pBa{0xeCcm{0x //
    {0x0F12,0x01DD, WORD_LEN, 0}, //01D3 //01ED //021A //0200 //01FA //01FA	  	//H//23A4
    {0x0F12,0xFFAE, WORD_LEN, 0}, //FFB4 //FFA8 //FFA5 //FFB4 //FFB9 //FFB9 
    {0x0F12,0xFFE0, WORD_LEN, 0}, //FFE4 //FFDC //FFEA //FFF5 //FFF8 //FFF8 
    {0x0F12,0x027B, WORD_LEN, 0}, //024C //01CE //0186 //0160 //0116 //00EC 
    {0x0F12,0x012E, WORD_LEN, 0}, //011E //013F //00F9 //00E5 //00BD //00EB 
    {0x0F12,0xFDC2, WORD_LEN, 0}, //FE01 //FE53 //FE8E //FEC9 //FF38 //FF33 
    {0x0F12,0xFEBC, WORD_LEN, 0}, //FECC //FEDC //FED2 //FEED //FF23 //FF23 
    {0x0F12,0x02A1, WORD_LEN, 0}, //0287 //0265 //0234 //0208 //01AB //01AB 
    {0x0F12,0xFF3A, WORD_LEN, 0}, //FF44 //FF50 //FF4F //FF60 //FF81 //FF81 
    {0x0F12,0xFEE2, WORD_LEN, 0}, //FEF1 //FF22 //FEF8 //FF07 //FF0D //FF0D 
    {0x0F12,0x019C, WORD_LEN, 0}, //0192 //0170 //0179 //016F //0169 //0169 
    {0x0F12,0x011A, WORD_LEN, 0}, //0115 //00FF //00E6 //00E1 //00DE //00DE 
    {0x0F12,0xFF79, WORD_LEN, 0}, //FFA0 //FFDA //FFDF //FFE8 //FFEF //FFEF 
    {0x0F12,0xFFB6, WORD_LEN, 0}, //FFAD //FFB8 //FFB4 //FFC0 //FFCA //FFCA 
    {0x0F12,0x01D6, WORD_LEN, 0}, //01B8 //0171 //0170 //015B //014D //014D 
    {0x0F12,0x01A7, WORD_LEN, 0}, //0197 //01AA //01D6 //01C6 //01C3 //01C3 
    {0x0F12,0xFF4D, WORD_LEN, 0}, //FF7C //FF62 //FF5C //FF76 //FF7E //FF7E 
    {0x0F12,0x0179, WORD_LEN, 0}, //015A //0166 //017A //0170 //016F //016F 
    {0x0F12,0x01DD, WORD_LEN, 0}, //01D3 //01ED //021A //0200 //01FA //01FA    //A//23C8
    {0x0F12,0xFFAE, WORD_LEN, 0}, //FFB4 //FFA8 //FFA5 //FFB4 //FFB9 //FFB9
    {0x0F12,0xFFE0, WORD_LEN, 0}, //FFE4 //FFDC //FFEA //FFF5 //FFF8 //FFF8
    {0x0F12,0x027B, WORD_LEN, 0}, //024C //01CE //0186 //0160 //0116 //00EC
    {0x0F12,0x012E, WORD_LEN, 0}, //011E //013F //00F9 //00E5 //00BD //00EB
    {0x0F12,0xFDC2, WORD_LEN, 0}, //FE01 //FE53 //FE8E //FEC9 //FF38 //FF33
    {0x0F12,0xFEBC, WORD_LEN, 0}, //FECC //FEDC //FED2 //FEED //FF23 //FF23
    {0x0F12,0x02A1, WORD_LEN, 0}, //0287 //0265 //0234 //0208 //01AB //01AB
    {0x0F12,0xFF3A, WORD_LEN, 0}, //FF44 //FF50 //FF4F //FF60 //FF81 //FF81
    {0x0F12,0xFEE2, WORD_LEN, 0}, //FEF1 //FF22 //FEF8 //FF07 //FF0D //FF0D
    {0x0F12,0x019C, WORD_LEN, 0}, //0192 //0170 //0179 //016F //0169 //0169
    {0x0F12,0x011A, WORD_LEN, 0}, //0115 //00FF //00E6 //00E1 //00DE //00DE
    {0x0F12,0xFF79, WORD_LEN, 0}, //FFA0 //FFDA //FFDF //FFE8 //FFEF //FFEF
    {0x0F12,0xFFB6, WORD_LEN, 0}, //FFAD //FFB8 //FFB4 //FFC0 //FFCA //FFCA
    {0x0F12,0x01D6, WORD_LEN, 0}, //01B8 //0171 //0170 //015B //014D //014D
    {0x0F12,0x01A7, WORD_LEN, 0}, //0197 //01AA //01D6 //01C6 //01C3 //01C3
    {0x0F12,0xFF4D, WORD_LEN, 0}, //FF7C //FF62 //FF5C //FF76 //FF7E //FF7E
    {0x0F12,0x0179, WORD_LEN, 0}, //015A //0166 //017A //0170 //016F //016F
    {0x0F12,0x01FA, WORD_LEN, 0}, //01DD //01D3 //01ED //021A //0200 //01FA //01FA	//WW//23EC
    {0x0F12,0xFFB9, WORD_LEN, 0}, //FFAE //FFB4 //FFA8 //FFA5 //FFB4 //FFB9 //FFB9
    {0x0F12,0xFFF8, WORD_LEN, 0}, //FFE0 //FFE4 //FFDC //FFEA //FFF5 //FFF8 //FFF8
    {0x0F12,0x0116, WORD_LEN, 0}, //027B //024C //01CE //0186 //0160 //0116 //00EC
    {0x0F12,0x00BD, WORD_LEN, 0}, //012E //011E //013F //00F9 //00E5 //00BD //00EB
    {0x0F12,0xFF38, WORD_LEN, 0}, //FDC2 //FE01 //FE53 //FE8E //FEC9 //FF38 //FF33
    {0x0F12,0xFF23, WORD_LEN, 0}, //FEBC //FECC //FEDC //FED2 //FEED //FF23 //FF23
    {0x0F12,0x01AB, WORD_LEN, 0}, //02A1 //0287 //0265 //0234 //0208 //01AB //01AB
    {0x0F12,0xFF81, WORD_LEN, 0}, //FF3A //FF44 //FF50 //FF4F //FF60 //FF81 //FF81
    {0x0F12,0xFF0D, WORD_LEN, 0}, //FEE2 //FEF1 //FF22 //FEF8 //FF07 //FF0D //FF0D
    {0x0F12,0x0169, WORD_LEN, 0}, //019C //0192 //0170 //0179 //016F //0169 //0169
    {0x0F12,0x00DE, WORD_LEN, 0}, //011A //0115 //00FF //00E6 //00E1 //00DE //00DE
    {0x0F12,0xFFEF, WORD_LEN, 0}, //FF79 //FFA0 //FFDA //FFDF //FFE8 //FFEF //FFEF
    {0x0F12,0xFFCA, WORD_LEN, 0}, //FFB6 //FFAD //FFB8 //FFB4 //FFC0 //FFCA //FFCA
    {0x0F12,0x014D, WORD_LEN, 0}, //01D6 //01B8 //0171 //0170 //015B //014D //014D
    {0x0F12,0x01C3, WORD_LEN, 0}, //01A7 //0197 //01AA //01D6 //01C6 //01C3 //01C3
    {0x0F12,0xFF7E, WORD_LEN, 0}, //FF4D //FF7C //FF62 //FF5C //FF76 //FF7E //FF7E
    {0x0F12,0x016F, WORD_LEN, 0}, //0179 //015A //0166 //017A //0170 //016F //016F
    {0x0F12,0x01F9, WORD_LEN, 0}, //01B7	//CW//2410
    {0x0F12,0xFFB8, WORD_LEN, 0}, //FFD0
    {0x0F12,0xFFF7, WORD_LEN, 0}, //0019
    {0x0F12,0x0130, WORD_LEN, 0}, //00C5
    {0x0F12,0x00CB, WORD_LEN, 0}, //00E3
    {0x0F12,0xFF0F, WORD_LEN, 0}, //FF5D
    {0x0F12,0xFF15, WORD_LEN, 0}, //FF3E
    {0x0F12,0x01CE, WORD_LEN, 0}, //0186
    {0x0F12,0xFF71, WORD_LEN, 0}, //FF8C
    {0x0F12,0xFF0E, WORD_LEN, 0}, //FF0E
    {0x0F12,0x016A, WORD_LEN, 0}, //016A
    {0x0F12,0x00DF, WORD_LEN, 0}, //00DF
    {0x0F12,0xFFEE, WORD_LEN, 0}, //FFEE
    {0x0F12,0xFFC9, WORD_LEN, 0}, //FFC9
    {0x0F12,0x014C, WORD_LEN, 0}, //014C
    {0x0F12,0x01C1, WORD_LEN, 0}, //01C1
    {0x0F12,0xFF7C, WORD_LEN, 0}, //FF7C
    {0x0F12,0x016D, WORD_LEN, 0}, //016D
    {0x0F12,0x01E2, WORD_LEN, 0}, //01F9 //01E0 //01B7	//D50//2434
    {0x0F12,0xFFB8, WORD_LEN, 0}, //FFB8 //FFB9 //FFD0
    {0x0F12,0x000B, WORD_LEN, 0}, //FFF7 //000C //0019
    {0x0F12,0x00CB, WORD_LEN, 0}, //0130 //00CB //00C5
    {0x0F12,0x00F3, WORD_LEN, 0}, //00CB //00F3 //00E3
    {0x0F12,0xFF4C, WORD_LEN, 0}, //FF0F //FF4C //FF5D
    {0x0F12,0xFF66, WORD_LEN, 0}, //FF15 //ff20 //FF33 //FF3E
    {0x0F12,0x01AB, WORD_LEN, 0}, //01CE //01bb //019C //0186
    {0x0F12,0xFF43, WORD_LEN, 0}, //FF71 //ff79 //FF85 //FF8C
    {0x0F12,0xFF0E, WORD_LEN, 0}, //FF0E //FF0E //FF0E
    {0x0F12,0x016A, WORD_LEN, 0}, //016A //016A //016A
    {0x0F12,0x00DF, WORD_LEN, 0}, //00DF //00DF //00DF
    {0x0F12,0xFFE8, WORD_LEN, 0}, //FFEE //FFEE //FFEE
    {0x0F12,0xFFC1, WORD_LEN, 0}, //FFC9 //FFC9 //FFC9
    {0x0F12,0x015A, WORD_LEN, 0}, //014C //014C //014C
    {0x0F12,0x01C1, WORD_LEN, 0}, //01C1 //01C1 //01C1
    {0x0F12,0xFF7C, WORD_LEN, 0}, //FF7C //FF7C //FF7C
    {0x0F12,0x016D, WORD_LEN, 0}, //016D //016D //016D
    {0x0F12,0x01E2, WORD_LEN, 0}, //01F9 //01E0 //01B7	//D65//2458
    {0x0F12,0xFFB8, WORD_LEN, 0}, //FFB8 //FFB9 //FFD0
    {0x0F12,0x000B, WORD_LEN, 0}, //FFF7 //000C //0019
    {0x0F12,0x00CB, WORD_LEN, 0}, //0130 //00CB //00C5
    {0x0F12,0x00F3, WORD_LEN, 0}, //00CB //00F3 //00E3
    {0x0F12,0xFF4C, WORD_LEN, 0}, //FF0F //FF4C //FF5D
    {0x0F12,0xFF66, WORD_LEN, 0}, //FF15 //ff20 //FF33 //FF3E
    {0x0F12,0x01AB, WORD_LEN, 0}, //01CE //01bb //019C //0186
    {0x0F12,0xFF43, WORD_LEN, 0}, //FF71 //ff79 //FF85 //FF8C
    {0x0F12,0xFF0E, WORD_LEN, 0}, //FF0E //FF0E //FF0E
    {0x0F12,0x016A, WORD_LEN, 0}, //016A //016A //016A
    {0x0F12,0x00DF, WORD_LEN, 0}, //00DF //00DF //00DF
    {0x0F12,0xFFE8, WORD_LEN, 0}, //FFEE //FFEE //FFEE
    {0x0F12,0xFFC1, WORD_LEN, 0}, //FFC9 //FFC9 //FFC9
    {0x0F12,0x015A, WORD_LEN, 0}, //014C //014C //014C
    {0x0F12,0x01C1, WORD_LEN, 0}, //01C1 //01C1 //01C1
    {0x0F12,0xFF7C, WORD_LEN, 0}, //FF7C //FF7C //FF7C
    {0x0F12,0x016D, WORD_LEN, 0}, //016D //016D //016D //	par,0xam_e},nd	TVAR_wbt_pBa{0xeCcm{0x
    {0x002A,0x2380, WORD_LEN, 0}, //	par,0xam_{},0xtart	TVAR_wbt_pOutdoorCcm
    {0x0F12,0x01B7, WORD_LEN, 0}, //01F2	//01F2	//019D  //TVAR_wbt_pOutdoorCcm[0] //{0xelf
    {0x0F12,0xFFBC, WORD_LEN, 0}, //FFC3	//FFc3	//FFC8  //TVAR_wbt_pOutdoorCcm[1]
    {0x0F12,0x0011, WORD_LEN, 0}, //FFE3	//FFe3	//000C  //TVAR_wbt_pOutdoorCcm[2]
    {0x0F12,0x00FC, WORD_LEN, 0}, //00F9	//00F9	//008D  //TVAR_wbt_pOutdoorCcm[3]
    {0x0F12,0x013B, WORD_LEN, 0}, //013F	//013F	//00CD  //TVAR_wbt_pOutdoorCcm[4]
    {0x0F12,0xFF84, WORD_LEN, 0}, //FF6E	//FF6E	//FF3F  //TVAR_wbt_pOutdoorCcm[5]
    {0x0F12,0xFED9, WORD_LEN, 0}, //FEBB	//FEbb	//FEDD  //TVAR_wbt_pOutdoorCcm[6]
    {0x0F12,0x01E6, WORD_LEN, 0}, //01F2	//01F2	//01C6  //TVAR_wbt_pOutdoorCcm[7]
    {0x0F12,0xFF16, WORD_LEN, 0}, //FEFA	//FEFA	//FF77  //TVAR_wbt_pOutdoorCcm[8]
    {0x0F12,0xFF4A, WORD_LEN, 0}, //FF37	//FF37	//FEAB  //TVAR_wbt_pOutdoorCcm[9]
    {0x0F12,0x0179, WORD_LEN, 0}, //01A2	//01A2	//015D  //TVAR_wbt_pOutdoorCcm[10]
    {0x0F12,0x014F, WORD_LEN, 0}, //0126	//0126	//0082  //TVAR_wbt_pOutdoorCcm[11]
    {0x0F12,0xFFC2, WORD_LEN, 0}, //FFE0	//FFE0	//FFCA  //TVAR_wbt_pOutdoorCcm[12]
    {0x0F12,0xFF99, WORD_LEN, 0}, //FFBF	//FFBF	//FFA2  //TVAR_wbt_pOutdoorCcm[13]
    {0x0F12,0x0219, WORD_LEN, 0}, //01E6	//01E6	//016F  //TVAR_wbt_pOutdoorCcm[14]
    {0x0F12,0x0172, WORD_LEN, 0}, //0186	//0186	//0171  //TVAR_wbt_pOutdoorCcm[15]
    {0x0F12,0xFF51, WORD_LEN, 0}, //FF4B	//FF4B	//FF35  //TVAR_wbt_pOutdoorCcm[16]
    {0x0F12,0x019B, WORD_LEN, 0}, //01B1	//01B1	//013E  //TVAR_wbt_pOutdoorCcm[17]//	par,0xam_e},nd	TVAR_wbt_pOutdoorCcm
    {0x002A,0x07E8, WORD_LEN, 0}, // 10. AFIT // // param_{0xtart	{0xARR_uNormBrInDoor //
    {0x0F12,0x0016, WORD_LEN, 0}, //000A	{0xARR_uNormBrInDoor[0] //
    {0x0F12,0x0028, WORD_LEN, 0}, //0019	{0xARR_uNormBrInDoor[1] //
    {0x0F12,0x0096, WORD_LEN, 0}, //0096	{0xARR_uNormBrInDoor[2] //
    {0x0F12,0x01F4, WORD_LEN, 0}, //01F4	{0xARR_uNormBrInDoor[3] //
    {0x0F12,0x07D0, WORD_LEN, 0}, // param_end	{0xARR_uNormBrInDoor // //07D0	{0xARR_uNormBrInDoor[4] //
    {0x002A,0x07D0, WORD_LEN, 0}, //	param_{0xtart	afit_uNoi{0xeIndInDoor //
    {0x0F12,0x0030, WORD_LEN, 0}, //afit_uNoi{0xeIndInDoor[0] //
    {0x0F12,0x0046, WORD_LEN, 0}, //afit_uNoi{0xeIndInDoor[1] //
    {0x0F12,0x0088, WORD_LEN, 0}, //afit_uNoi{0xeIndInDoor[2] //
    {0x0F12,0x0205, WORD_LEN, 0}, //afit_uNoi{0xeIndInDoor[3] //
    {0x0F12,0x02BC, WORD_LEN, 0}, //	param_end	afit_uNoi{0xeIndInDoor // //afit_uNoi{0xeIndInDoor[4] //
    {0x002A,0x07E6, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0}, //afit_bU{0xeNoi{0xeInd //
    {0x002A,0x0828, WORD_LEN, 0},
    {0x0F12,0x0010, WORD_LEN, 0},	//06 10 10	//TVAR_afit_pBa{0xeVal{0x[0]     70000828  //BRIGHTNE{0x{0x                                                                
    {0x0F12,0x0014, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[1]     7000082A  //CONTRA{0xT                                                                  
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[2]     7000082C  //{0xATURATION                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[3]     7000082E  //{0xHARP_BLUR                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[4]     70000830  //GLAMOUR                                                                   
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[5]     70000832  //Di{0xparity_i{0xat{0xat                                                         
    {0x0F12,0x0061, WORD_LEN, 0}, //21	//TVAR_afit_pBa{0xeVal{0x[6]     70000834  //Denoi{0xe1_iYDenThre{0xhLow                                                   
    {0x0F12,0x0030, WORD_LEN, 0}, //28	//TVAR_afit_pBa{0xeVal{0x[7]     70000836  //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
    {0x0F12,0x0061, WORD_LEN, 0}, //50	//TVAR_afit_pBa{0xeVal{0x[8]     70000838  //Denoi{0xe1_iYDenThre{0xhHigh                                                  
    {0x0F12,0x00FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[9]     7000083A  //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
    {0x0F12,0x0129, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[10]    7000083C  //Denoi{0xe1_iLowWWideThre{0xh                                                  
    {0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[11]    7000083E  //Denoi{0xe1_iHighWWideThre{0xh                                                 
    {0x0F12,0x0028, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[12]    70000840  //Denoi{0xe1_iLowWideThre{0xh                                                   
    {0x0F12,0x0028, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[13]    70000842  //Denoi{0xe1_iHighWideThre{0xh                                                  
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[14]    70000844  //Denoi{0xe1_i{0xat{0xat                                                          
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[15]    70000846  //Demo{0xaic4_iHy{0xtGrayLow                                                    
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[16]    70000848  //Demo{0xaic4_iHy{0xtGrayHigh                                                   
    {0x0F12,0x0344, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[17]    7000084A  //UVDenoi{0xe_iYLowThre{0xh                                                     
    {0x0F12,0x033A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[18]    7000084C  //UVDenoi{0xe_iYHighThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[19]    7000084E  //UVDenoi{0xe_iUVLowThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[20]    70000850  //UVDenoi{0xe_iUVHighThre{0xh                                                   
    {0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[21]    70000852  //D{0xMix1_iLowLimit_Wide                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[22]    70000854  //D{0xMix1_iLowLimit_Wide_Bin                                                 
    {0x0F12,0x001E, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[23]    70000856  //D{0xMix1_iHighLimit_Wide                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[24]    70000858  //D{0xMix1_iHighLimit_Wide_Bin                                                
    {0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[25]    7000085A  //D{0xMix1_iLowLimit_Fine                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[26]    7000085C  //D{0xMix1_iLowLimit_Fine_Bin                                                 
    {0x0F12,0x0010, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[27]    7000085E  //D{0xMix1_iHighLimit_Fine                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[28]    70000860  //D{0xMix1_iHighLimit_Fine_Bin                                                
    {0x0F12,0x0106, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[29]    70000862  //D{0xMix1_iRGBOff{0xet                                                         
    {0x0F12,0x006F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[30]    70000864  //D{0xMix1_iDemClamp                                                          
    {0x0F12,0x0C0F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[31]    70000866  //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
    {0x0F12,0x0C0F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[32]    70000868  //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
    {0x0F12,0x0303, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[33]    7000086A  //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
    {0x0F12,0x0303, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[34]    7000086C  //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
    {0x0F12,0x140A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[35]    7000086E  //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
    {0x0F12,0x140A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[36]    70000870  //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
    {0x0F12,0x2828, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[37]    70000872  //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[38]    70000874  //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
    {0x0F12,0x020A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[39]    70000876  //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
    {0x0F12,0x0480, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[40]    70000878  //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
    {0x0F12,0x0E08, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[41]    7000087A  //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
    {0x0F12,0x03FF, WORD_LEN, 0}, //30A	//TVAR_afit_pBa{0xeVal{0x[42]    7000087C  //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
    {0x0F12,0x0A03, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[43]    7000087E  //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
    {0x0F12,0x0A11, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[44]    70000880  //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
    {0x0F12,0x000F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[45]    70000882  //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
    {0x0F12,0x0500, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[46]    70000884  //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
    {0x0F12,0x0900, WORD_LEN, 0}, //914	//TVAR_afit_pBa{0xeVal{0x[47]    70000886  //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
    {0x0F12,0x0012, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[48]    70000888  //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[49]    7000088A  //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
    {0x0F12,0x0005, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[50]    7000088C  //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[51]    7000088E  //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[52]    70000890  //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[53]    70000892  //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[54]    70000894  //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
    {0x0F12,0x0A00, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[55]    70000896  //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
    {0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[56]    70000898  //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
    {0x0F12,0x014C, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[57]    7000089A  //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
    {0x0F12,0x014D, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[58]    7000089C  //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
    {0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[59]    7000089E  //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
    {0x0F12,0x8020, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[60]    700008A0  //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[61]    700008A2  //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
    {0x0F12,0x0018, WORD_LEN, 0}, //14 23 10 22 //20 //1E //A	//TVAR_afit_pBa{0xeVal{0x[62]    700008A4  //RGB2YUV_iYOff{0xet                                                          
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[63]    700008A6            //BRIGHTNE{0x{0x                                                                
    {0x0F12,0x0028, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[64]    700008A8            //CONTRA{0xT                                                                  
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[65]    700008AA            //{0xATURATION                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[66]    700008AC            //{0xHARP_BLUR                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[67]    700008AE            //GLAMOUR                                                                   
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[68]    700008B0    //Di{0xparity_i{0xat{0xat                                                         
    {0x0F12,0x0020, WORD_LEN, 0}, //0C	//0020	//TVAR_afit_pBa{0xeVal{0x[69]    700008B2    //Denoi{0xe1_iYDenThre{0xhLow                                                   
    {0x0F12,0x000E, WORD_LEN, 0},	//000E	//TVAR_afit_pBa{0xeVal{0x[70]    700008B4    //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
    {0x0F12,0x0050, WORD_LEN, 0},	//0080	//TVAR_afit_pBa{0xeVal{0x[71]    700008B6    //Denoi{0xe1_iYDenThre{0xhHigh                                                  
    {0x0F12,0x00FF, WORD_LEN, 0},	//00FF	//TVAR_afit_pBa{0xeVal{0x[72]    700008B8    //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
    {0x0F12,0x0129, WORD_LEN, 0},	//0129	//TVAR_afit_pBa{0xeVal{0x[73]    700008BA    //Denoi{0xe1_iLowWWideThre{0xh                                                  
    {0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[74]    700008BC    //Denoi{0xe1_iHighWWideThre{0xh                                                 
    {0x0F12,0x0028, WORD_LEN, 0},	//0028	//TVAR_afit_pBa{0xeVal{0x[75]    700008BE    //Denoi{0xe1_iLowWideThre{0xh                                                   
    {0x0F12,0x0028, WORD_LEN, 0},	//0028	//TVAR_afit_pBa{0xeVal{0x[76]    700008C0    //Denoi{0xe1_iHighWideThre{0xh                                                  
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[77]    700008C2    //Denoi{0xe1_i{0xat{0xat                                                          
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[78]    700008C4    //Demo{0xaic4_iHy{0xtGrayLow                                                    
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[79]    700008C6    //Demo{0xaic4_iHy{0xtGrayHigh                                                   
    {0x0F12,0x0114, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[80]    700008C8    //UVDenoi{0xe_iYLowThre{0xh                                                     
    {0x0F12,0x020A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[81]    700008CA    //UVDenoi{0xe_iYHighThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[82]    700008CC    //UVDenoi{0xe_iUVLowThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[83]    700008CE    //UVDenoi{0xe_iUVHighThre{0xh                                                   
    {0x0F12,0x0018, WORD_LEN, 0},	//000a	//TVAR_afit_pBa{0xeVal{0x[84]    700008D0    //D{0xMix1_iLowLimit_Wide                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[85]    700008D2    //D{0xMix1_iLowLimit_Wide_Bin                                                 
    {0x0F12,0x000A, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[86]    700008D4    //D{0xMix1_iHighLimit_Wide                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[87]    700008D6    //D{0xMix1_iHighLimit_Wide_Bin                                                
    {0x0F12,0x0028, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[88]    700008D8    //D{0xMix1_iLowLimit_Fine                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[89]    700008DA    //D{0xMix1_iLowLimit_Fine_Bin                                                 
    {0x0F12,0x0010, WORD_LEN, 0},	//00A0	//TVAR_afit_pBa{0xeVal{0x[90]    700008DC    //D{0xMix1_iHighLimit_Fine                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[91]    700008DE    //D{0xMix1_iHighLimit_Fine_Bin                                                
    {0x0F12,0x0106, WORD_LEN, 0},	//0106	//TVAR_afit_pBa{0xeVal{0x[92]    700008E0    //D{0xMix1_iRGBOff{0xet                                                         
    {0x0F12,0x006F, WORD_LEN, 0},	//006F	//TVAR_afit_pBa{0xeVal{0x[93]    700008E2    //D{0xMix1_iDemClamp                                                          
    {0x0F12,0x050F, WORD_LEN, 0},	//050F	//TVAR_afit_pBa{0xeVal{0x[94]    700008E4    //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
    {0x0F12,0x0A0F, WORD_LEN, 0},	//0A0F	//TVAR_afit_pBa{0xeVal{0x[95]    700008E6    //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
    {0x0F12,0x0203, WORD_LEN, 0},	//0203	//TVAR_afit_pBa{0xeVal{0x[96]    700008E8    //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
    {0x0F12,0x0303, WORD_LEN, 0},	//0203	//TVAR_afit_pBa{0xeVal{0x[97]    700008EA    //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
    {0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[98]    700008EC    //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
    {0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[99]    700008EE    //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
    {0x0F12,0x2828, WORD_LEN, 0},	//2828	//TVAR_afit_pBa{0xeVal{0x[100]   700008F0    //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[101]   700008F2    //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
    {0x0F12,0x020A, WORD_LEN, 0},	//020A	//TVAR_afit_pBa{0xeVal{0x[102]   700008F4    //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
    {0x0F12,0x0480, WORD_LEN, 0},	//0480	//TVAR_afit_pBa{0xeVal{0x[103]   700008F6    //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
    {0x0F12,0x0E08, WORD_LEN, 0},	//0E08	//TVAR_afit_pBa{0xeVal{0x[104]   700008F8    //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
    {0x0F12,0x030A, WORD_LEN, 0},	//020A	//TVAR_afit_pBa{0xeVal{0x[105]   700008FA    //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
    {0x0F12,0x1403, WORD_LEN, 0},	//0A03	//TVAR_afit_pBa{0xeVal{0x[106]   700008FC    //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
    {0x0F12,0x0A11, WORD_LEN, 0},	//0A11	//TVAR_afit_pBa{0xeVal{0x[107]   700008FE    //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
    {0x0F12,0x0A0F, WORD_LEN, 0},	//0A0F	//TVAR_afit_pBa{0xeVal{0x[108]   70000900    //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
    {0x0F12,0x050A, WORD_LEN, 0},	//050A	//TVAR_afit_pBa{0xeVal{0x[109]   70000902    //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
    {0x0F12,0x1014, WORD_LEN, 0}, //1E	//101E	//TVAR_afit_pBa{0xeVal{0x[110]   70000904    //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
    {0x0F12,0x101E, WORD_LEN, 0},	//101E	//TVAR_afit_pBa{0xeVal{0x[111]   70000906    //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
    {0x0F12,0x0A08, WORD_LEN, 0},	//3030	//TVAR_afit_pBa{0xeVal{0x[112]   70000908    //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
    {0x0F12,0x0005, WORD_LEN, 0},	//0005	//TVAR_afit_pBa{0xeVal{0x[113]   7000090A    //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
    {0x0F12,0x0400, WORD_LEN, 0},	//0400	//TVAR_afit_pBa{0xeVal{0x[114]   7000090C    //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
    {0x0F12,0x0400, WORD_LEN, 0},	//0400	//TVAR_afit_pBa{0xeVal{0x[115]   7000090E    //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[116]   70000910    //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[117]   70000912    //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
    {0x0F12,0x0A00, WORD_LEN, 0},	//0A00	//TVAR_afit_pBa{0xeVal{0x[118]   70000914    //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
    {0x0F12,0x000A, WORD_LEN, 0},	//100A	//TVAR_afit_pBa{0xeVal{0x[119]   70000916    //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[120]   70000918            //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
    {0x0F12,0x0154, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[121]   7000091A            //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
    {0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[122]   7000091C            //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
    {0x0F12,0x8020, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[123]   7000091E            //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[124]   70000920            //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
    {0x0F12,0x0000, WORD_LEN, 0}, //1f 00	//TVAR_afit_pBa{0xeVal{0x[125]   70000922            //RGB2YUV_iYOff{0xet                                                          
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[126]   70000924            //BRIGHTNE{0x{0x                                                                
    {0x0F12,0x0024, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[127]   70000926            //CONTRA{0xT                                                                  
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[128]   70000928            //{0xATURATION                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[129]   7000092A            //{0xHARP_BLUR                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[130]   7000092C            //GLAMOUR                                                                   
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[131]   7000092E    //Di{0xparity_i{0xat{0xat                                                         
    {0x0F12,0x000E, WORD_LEN, 0},   //e 08	//000E	//TVAR_afit_pBa{0xeVal{0x[132]   70000930    //Denoi{0xe1_iYDenThre{0xhLow                                                   
    {0x0F12,0x0006, WORD_LEN, 0},	//0006	//TVAR_afit_pBa{0xeVal{0x[133]   70000932    //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
    {0x0F12,0x0050, WORD_LEN, 0},	//50 0064	//TVAR_afit_pBa{0xeVal{0x[134]   70000934    //Denoi{0xe1_iYDenThre{0xhHigh                                                  
    {0x0F12,0x0050, WORD_LEN, 0},	//0050	//TVAR_afit_pBa{0xeVal{0x[135]   70000936    //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
    {0x0F12,0x0002, WORD_LEN, 0},	//0002	//TVAR_afit_pBa{0xeVal{0x[136]   70000938    //Denoi{0xe1_iLowWWideThre{0xh                                                  
    {0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[137]   7000093A    //Denoi{0xe1_iHighWWideThre{0xh                                                 
    {0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[138]   7000093C    //Denoi{0xe1_iLowWideThre{0xh                                                   
    {0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[139]   7000093E    //Denoi{0xe1_iHighWideThre{0xh                                                  
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[140]   70000940    //Denoi{0xe1_i{0xat{0xat                                                          
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[141]   70000942    //Demo{0xaic4_iHy{0xtGrayLow                                                    
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[142]   70000944    //Demo{0xaic4_iHy{0xtGrayHigh                                                   
    {0x0F12,0x0014, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[143]   70000946    //UVDenoi{0xe_iYLowThre{0xh                                                     
    {0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[144]   70000948    //UVDenoi{0xe_iYHighThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[145]   7000094A    //UVDenoi{0xe_iUVLowThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[146]   7000094C    //UVDenoi{0xe_iUVHighThre{0xh                                                   
    {0x0F12,0x001C, WORD_LEN, 0},	//000a	//TVAR_afit_pBa{0xeVal{0x[147]   7000094E    //D{0xMix1_iLowLimit_Wide                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[148]   70000950    //D{0xMix1_iLowLimit_Wide_Bin                                                 
    {0x0F12,0x000A, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[149]   70000952    //D{0xMix1_iHighLimit_Wide                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[150]   70000954    //D{0xMix1_iHighLimit_Wide_Bin                                                
    {0x0F12,0x0028, WORD_LEN, 0},	//0050	//TVAR_afit_pBa{0xeVal{0x[151]   70000956    //D{0xMix1_iLowLimit_Fine                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[152]   70000958    //D{0xMix1_iLowLimit_Fine_Bin                                                 
    {0x0F12,0x0010, WORD_LEN, 0},	//0010	//TVAR_afit_pBa{0xeVal{0x[153]   7000095A    //D{0xMix1_iHighLimit_Fine                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[154]   7000095C    //D{0xMix1_iHighLimit_Fine_Bin                                                
    {0x0F12,0x0106, WORD_LEN, 0},	//0106	//TVAR_afit_pBa{0xeVal{0x[155]   7000095E    //D{0xMix1_iRGBOff{0xet                                                         
    {0x0F12,0x006F, WORD_LEN, 0},	//006F	//TVAR_afit_pBa{0xeVal{0x[156]   70000960    //D{0xMix1_iDemClamp                                                          
    {0x0F12,0x0205, WORD_LEN, 0},	//020A	//TVAR_afit_pBa{0xeVal{0x[157]   70000962    //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
    {0x0F12,0x0505, WORD_LEN, 0},	//050A	//TVAR_afit_pBa{0xeVal{0x[158]   70000964    //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
    {0x0F12,0x0101, WORD_LEN, 0},	//0101	//TVAR_afit_pBa{0xeVal{0x[159]   70000966    //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
    {0x0F12,0x0202, WORD_LEN, 0},	//0102	//TVAR_afit_pBa{0xeVal{0x[160]   70000968    //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
    {0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[161]   7000096A    //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
    {0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[162]   7000096C    //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
    {0x0F12,0x2828, WORD_LEN, 0},	//2828	//TVAR_afit_pBa{0xeVal{0x[163]   7000096E    //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
    {0x0F12,0x0606, WORD_LEN, 0},	//0606	//TVAR_afit_pBa{0xeVal{0x[164]   70000970    //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
    {0x0F12,0x0205, WORD_LEN, 0},	//0205	//TVAR_afit_pBa{0xeVal{0x[165]   70000972    //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
    {0x0F12,0x0480, WORD_LEN, 0},	//0480	//TVAR_afit_pBa{0xeVal{0x[166]   70000974    //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
    {0x0F12,0x000A, WORD_LEN, 0},	//000F	//TVAR_afit_pBa{0xeVal{0x[167]   70000976    //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
    {0x0F12,0x0005, WORD_LEN, 0},	//0005	//TVAR_afit_pBa{0xeVal{0x[168]   70000978    //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
    {0x0F12,0x1903, WORD_LEN, 0},	//1903	//TVAR_afit_pBa{0xeVal{0x[169]   7000097A    //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
    {0x0F12,0x0F11, WORD_LEN, 0}, //1911	//1911	//TVAR_afit_pBa{0xeVal{0x[170]   7000097C    //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
    {0x0F12,0x0A0F, WORD_LEN, 0},	//0A0F	//TVAR_afit_pBa{0xeVal{0x[171]   7000097E    //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
    {0x0F12,0x050A, WORD_LEN, 0},	//050A	//TVAR_afit_pBa{0xeVal{0x[172]   70000980    //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
    {0x0F12,0x2014, WORD_LEN, 0}, //2028	//2028	//TVAR_afit_pBa{0xeVal{0x[173]   70000982    //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
    {0x0F12,0x201E, WORD_LEN, 0}, //2028	//2028	//TVAR_afit_pBa{0xeVal{0x[174]   70000984    //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
    {0x0F12,0x0A08, WORD_LEN, 0},	//2000	//TVAR_afit_pBa{0xeVal{0x[175]   70000986    //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
    {0x0F12,0x0007, WORD_LEN, 0},	//0007	//TVAR_afit_pBa{0xeVal{0x[176]   70000988    //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
    {0x0F12,0x0403, WORD_LEN, 0},	//0403	//TVAR_afit_pBa{0xeVal{0x[177]   7000098A    //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
    {0x0F12,0x0402, WORD_LEN, 0},	//0402	//TVAR_afit_pBa{0xeVal{0x[178]   7000098C    //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[179]   7000098E    //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
    {0x0F12,0x0203, WORD_LEN, 0},	//0203	//TVAR_afit_pBa{0xeVal{0x[180]   70000990    //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[181]   70000992    //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
    {0x0F12,0x0006, WORD_LEN, 0},	//1006	//TVAR_afit_pBa{0xeVal{0x[182]   70000994    //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[183]   70000996            //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
    {0x0F12,0x0173, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[184]   70000998            //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
    {0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[185]   7000099A            //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
    {0x0F12,0x8030, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[186]   7000099C            //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[187]   7000099E            //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[188]   700009A0            //RGB2YUV_iYOff{0xet                                                          
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[189]   700009A2            //BRIGHTNE{0x{0x                                                                
    {0x0F12,0x0014, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[190]   700009A4            //CONTRA{0xT                                                                  
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[191]   700009A6            //{0xATURATION                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[192]   700009A8            //{0xHARP_BLUR                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[193]   700009AA            //GLAMOUR                                                                   
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[194]   700009AC    //Di{0xparity_i{0xat{0xat                                                         
    {0x0F12,0x000E, WORD_LEN, 0},   //e 08	//000E	//TVAR_afit_pBa{0xeVal{0x[195]   700009AE    //Denoi{0xe1_iYDenThre{0xhLow                                                   
    {0x0F12,0x0006, WORD_LEN, 0},	//0006	//TVAR_afit_pBa{0xeVal{0x[196]   700009B0    //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
    {0x0F12,0x0050, WORD_LEN, 0},	//50 0064	//TVAR_afit_pBa{0xeVal{0x[197]   700009B2    //Denoi{0xe1_iYDenThre{0xhHigh                                                  
    {0x0F12,0x0050, WORD_LEN, 0},	//0050	//TVAR_afit_pBa{0xeVal{0x[198]   700009B4    //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
    {0x0F12,0x0002, WORD_LEN, 0},	//0002	//TVAR_afit_pBa{0xeVal{0x[199]   700009B6    //Denoi{0xe1_iLowWWideThre{0xh                                                  
    {0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[200]   700009B8    //Denoi{0xe1_iHighWWideThre{0xh                                                 
    {0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[201]   700009BA    //Denoi{0xe1_iLowWideThre{0xh                                                   
    {0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[202]   700009BC    //Denoi{0xe1_iHighWideThre{0xh                                                  
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[203]   700009BE    //Denoi{0xe1_i{0xat{0xat                                                          
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[204]   700009C0    //Demo{0xaic4_iHy{0xtGrayLow                                                    
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[205]   700009C2    //Demo{0xaic4_iHy{0xtGrayHigh                                                   
    {0x0F12,0x0014, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[206]   700009C4    //UVDenoi{0xe_iYLowThre{0xh                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[207]   700009C6    //UVDenoi{0xe_iYHighThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[208]   700009C8    //UVDenoi{0xe_iUVLowThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[209]   700009CA    //UVDenoi{0xe_iUVHighThre{0xh                                                   
    {0x0F12,0x001C, WORD_LEN, 0},	//000a	//TVAR_afit_pBa{0xeVal{0x[210]   700009CC    //D{0xMix1_iLowLimit_Wide                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[211]   700009CE    //D{0xMix1_iLowLimit_Wide_Bin                                                 
    {0x0F12,0x000A, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[212]   700009D0    //D{0xMix1_iHighLimit_Wide                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[213]   700009D2    //D{0xMix1_iHighLimit_Wide_Bin                                                
    {0x0F12,0x0028, WORD_LEN, 0},	//0050	//TVAR_afit_pBa{0xeVal{0x[214]   700009D4    //D{0xMix1_iLowLimit_Fine                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[215]   700009D6    //D{0xMix1_iLowLimit_Fine_Bin                                                 
    {0x0F12,0x0010, WORD_LEN, 0},	//0010	//TVAR_afit_pBa{0xeVal{0x[216]   700009D8    //D{0xMix1_iHighLimit_Fine                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[217]   700009DA    //D{0xMix1_iHighLimit_Fine_Bin                                                
    {0x0F12,0x0106, WORD_LEN, 0},	//0106	//TVAR_afit_pBa{0xeVal{0x[218]   700009DC    //D{0xMix1_iRGBOff{0xet                                                         
    {0x0F12,0x006F, WORD_LEN, 0},	//006F	//TVAR_afit_pBa{0xeVal{0x[219]   700009DE    //D{0xMix1_iDemClamp                                                          
    {0x0F12,0x0205, WORD_LEN, 0},	//0205	//TVAR_afit_pBa{0xeVal{0x[220]   700009E0    //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
    {0x0F12,0x0505, WORD_LEN, 0},	//0505	//TVAR_afit_pBa{0xeVal{0x[221]   700009E2    //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
    {0x0F12,0x0101, WORD_LEN, 0},	//0101	//TVAR_afit_pBa{0xeVal{0x[222]   700009E4    //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
    {0x0F12,0x0202, WORD_LEN, 0},	//0102	//TVAR_afit_pBa{0xeVal{0x[223]   700009E6    //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
    {0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[224]   700009E8    //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
    {0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[225]   700009EA    //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
    {0x0F12,0x2828, WORD_LEN, 0},	//2828	//TVAR_afit_pBa{0xeVal{0x[226]   700009EC    //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
    {0x0F12,0x0606, WORD_LEN, 0},	//0606	//TVAR_afit_pBa{0xeVal{0x[227]   700009EE    //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
    {0x0F12,0x0205, WORD_LEN, 0},	//0205	//TVAR_afit_pBa{0xeVal{0x[228]   700009F0    //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
    {0x0F12,0x0480, WORD_LEN, 0},	//0480	//TVAR_afit_pBa{0xeVal{0x[229]   700009F2    //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
    {0x0F12,0x000A, WORD_LEN, 0},	//000F	//TVAR_afit_pBa{0xeVal{0x[230]   700009F4    //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
    {0x0F12,0x0005, WORD_LEN, 0},	//0005	//TVAR_afit_pBa{0xeVal{0x[231]   700009F6    //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
    {0x0F12,0x1903, WORD_LEN, 0},	//1903	//TVAR_afit_pBa{0xeVal{0x[232]   700009F8    //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
    {0x0F12,0x0F11, WORD_LEN, 0}, //1911	//1911	//TVAR_afit_pBa{0xeVal{0x[233]   700009FA    //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
    {0x0F12,0x0A0F, WORD_LEN, 0},	//0A0F	//TVAR_afit_pBa{0xeVal{0x[234]   700009FC    //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
    {0x0F12,0x050A, WORD_LEN, 0},	//050A	//TVAR_afit_pBa{0xeVal{0x[235]   700009FE    //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
    {0x0F12,0x2014, WORD_LEN, 0}, //2028	//2028	//TVAR_afit_pBa{0xeVal{0x[236]   70000A00    //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
    {0x0F12,0x201E, WORD_LEN, 0}, //2028	//2028	//TVAR_afit_pBa{0xeVal{0x[237]   70000A02    //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
    {0x0F12,0x0A08, WORD_LEN, 0},	//2000	//TVAR_afit_pBa{0xeVal{0x[238]   70000A04    //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
    {0x0F12,0x0007, WORD_LEN, 0},	//0007	//TVAR_afit_pBa{0xeVal{0x[239]   70000A06    //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
    {0x0F12,0x0403, WORD_LEN, 0},	//0403	//TVAR_afit_pBa{0xeVal{0x[240]   70000A08    //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
    {0x0F12,0x0402, WORD_LEN, 0},	//0402	//TVAR_afit_pBa{0xeVal{0x[241]   70000A0A    //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[242]   70000A0C    //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
    {0x0F12,0x0203, WORD_LEN, 0},	//0203	//TVAR_afit_pBa{0xeVal{0x[243]   70000A0E    //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
    {0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[244]   70000A10    //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
    {0x0F12,0x0006, WORD_LEN, 0},	//1006	//TVAR_afit_pBa{0xeVal{0x[245]   70000A12    //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[246]   70000A14            //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[247]   70000A16            //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
    {0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[248]   70000A18            //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
    {0x0F12,0x8030, WORD_LEN, 0},	//30 TVAR_afit_pBa{0xeVal{0x[249]   70000A1A            //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[250]   70000A1C            //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[251]   70000A1E            //RGB2YUV_iYOff{0xet                                                          
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[252]   70000A20                //BRIGHTNE{0x{0x                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[253]   70000A22                //CONTRA{0xT                                                                  
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[254]   70000A24                //{0xATURATION                                                                
    {0x0F12,0x0014, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[255]   70000A26                //{0xHARP_BLUR                                                                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[256]   70000A28                //GLAMOUR                                                                   
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[257]   70000A2A                //Di{0xparity_i{0xat{0xat                                                         
    {0x0F12,0x000e, WORD_LEN, 0},	//0e TVAR_afit_pBa{0xeVal{0x[258]   70000A2C                //Denoi{0xe1_iYDenThre{0xhLow                                                   
    {0x0F12,0x0006, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[259]   70000A2E                //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
    {0x0F12,0x0020, WORD_LEN, 0},	//20 TVAR_afit_pBa{0xeVal{0x[260]   70000A30                //Denoi{0xe1_iYDenThre{0xhHigh                                                  
    {0x0F12,0x0050, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[261]   70000A32                //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
    {0x0F12,0x0002, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[262]   70000A34                //Denoi{0xe1_iLowWWideThre{0xh                                                  
    {0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[263]   70000A36                //Denoi{0xe1_iHighWWideThre{0xh                                                 
    {0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[264]   70000A38                //Denoi{0xe1_iLowWideThre{0xh                                                   
    {0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[265]   70000A3A                //Denoi{0xe1_iHighWideThre{0xh                                                  
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[266]   70000A3C                //Denoi{0xe1_i{0xat{0xat                                                          
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[267]   70000A3E                //Demo{0xaic4_iHy{0xtGrayLow                                                    
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[268]   70000A40                //Demo{0xaic4_iHy{0xtGrayHigh                                                   
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[269]   70000A42                //UVDenoi{0xe_iYLowThre{0xh                                                     
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[270]   70000A44                //UVDenoi{0xe_iYHighThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[271]   70000A46                //UVDenoi{0xe_iUVLowThre{0xh                                                    
    {0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[272]   70000A48                //UVDenoi{0xe_iUVHighThre{0xh                                                   
    {0x0F12,0x0014, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[273]   70000A4A                //D{0xMix1_iLowLimit_Wide                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[274]   70000A4C                //D{0xMix1_iLowLimit_Wide_Bin                                                 
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[275]   70000A4E                //D{0xMix1_iHighLimit_Wide                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[276]   70000A50                //D{0xMix1_iHighLimit_Wide_Bin                                                
    {0x0F12,0x0020, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[277]   70000A52                //D{0xMix1_iLowLimit_Fine                                                     
    {0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[278]   70000A54                //D{0xMix1_iLowLimit_Fine_Bin                                                 
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[279]   70000A56                //D{0xMix1_iHighLimit_Fine                                                    
    {0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[280]   70000A58                //D{0xMix1_iHighLimit_Fine_Bin                                                
    {0x0F12,0x0106, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[281]   70000A5A                //D{0xMix1_iRGBOff{0xet                                                         
    {0x0F12,0x006F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[282]   70000A5C                //D{0xMix1_iDemClamp                                                          
    {0x0F12,0x0202, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[283]   70000A5E                //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
    {0x0F12,0x0502, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[284]   70000A60                //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
    {0x0F12,0x0101, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[285]   70000A62                //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
    {0x0F12,0x0202, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[286]   70000A64                //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
    {0x0F12,0x140A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[287]   70000A66                //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
    {0x0F12,0x140A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[288]   70000A68                //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
    {0x0F12,0x2828, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[289]   70000A6A                //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
    {0x0F12,0x0606, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[290]   70000A6C                //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
    {0x0F12,0x0205, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[291]   70000A6E                //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
    {0x0F12,0x0880, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[292]   70000A70                //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
    {0x0F12,0x000F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[293]   70000A72                //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
    {0x0F12,0x0005, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[294]   70000A74                //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
    {0x0F12,0x1903, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[295]   70000A76                //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
    {0x0F12,0x0F11, WORD_LEN, 0}, //1911	//TVAR_afit_pBa{0xeVal{0x[296]   70000A78                //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
    {0x0F12,0x0A0F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[297]   70000A7A                //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
    {0x0F12,0x050A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[298]   70000A7C                //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
    {0x0F12,0x2014, WORD_LEN, 0}, //2020	//TVAR_afit_pBa{0xeVal{0x[299]   70000A7E                //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
    {0x0F12,0x201E, WORD_LEN, 0}, //2020	//TVAR_afit_pBa{0xeVal{0x[300]   70000A80                //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
    {0x0F12,0x0A08, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[301]   70000A82                //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
    {0x0F12,0x0007, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[302]   70000A84                //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
    {0x0F12,0x0408, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[303]   70000A86                //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
    {0x0F12,0x0406, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[304]   70000A88                //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[305]   70000A8A                //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
    {0x0F12,0x0608, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[306]   70000A8C                //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[307]   70000A8E                //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
    {0x0F12,0x0006, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[308]   70000A90                //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[309]   70000A92 //180 173 164  //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[310]   70000A94 //Linearity    //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
    {0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[311]   70000A96                //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
    {0x0F12,0x803b, WORD_LEN, 0},	//40 TVAR_afit_pBa{0xeVal{0x[312]   70000A98                //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
    {0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[313]   70000A9A                //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
    {0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[314]   70000A9C                //RGB2YUV_iYOff{0xet              //	par,0xam_e},nd	TVAR_afit_pBa{0xeVal{0x //	par,0xam_{},0xtart	afit_pCon{0xtBa{0xeVal{0x
    {0x0F12,0x00FF, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[0]                            //Denoi{0xe1_iUVDenThre{0xhLow                             
    {0x0F12,0x00FF, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[1]                            //Denoi{0xe1_iUVDenThre{0xhHigh                            
    {0x0F12,0x0800, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[2]                            //Denoi{0xe1_{0xen{0xor_width                                
    {0x0F12,0x0600, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[3]                            //Denoi{0xe1_{0xen{0xor_height                               
    {0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[4]                            //Denoi{0xe1_{0xtart_x                                     
    {0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[5]                            //Denoi{0xe1_{0xtart_y                                     
    {0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[6]                            //"Denoi{0xe1_iYDen{0xmoothDenoi{0xe1_iW{0xharp  "             
    {0x0F12,0x0300, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[7]                            //"Denoi{0xe1_iWW{0xharp Denoi{0xe1_iRadialTune  "           
    {0x0F12,0x0002, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[8]                            //"Denoi{0xe1_iOutputBrightne{0x{0xDenoi{0xe1_binning_x  "     
    {0x0F12,0x0400, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[9]                            //"Denoi{0xe1_binning_yDemo{0xaic4_iFDeriv  "              
    {0x0F12,0x0106, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[10]                           //"Demo{0xaic4_iFDerivNeiDemo{0xaic4_i{0xDeriv  "            
    {0x0F12,0x0005, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[11]                           //"Demo{0xaic4_i{0xDerivNeiDemo{0xaic4_iEnhancerG  "         
    {0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[12]                           //"Demo{0xaic4_iEnhancerRBDemo{0xaic4_iEnhancerV  "        
    {0x0F12,0x0703, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[13]                           //"Demo{0xaic4_iDeci{0xionThre{0xhDemo{0xaic4_iDe{0xatThre{0xh"    
    {0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[14]                           //Demo{0xaic4_iBypa{0x{0x{0xelect                              
    {0x0F12,0xFFD6, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[15]
    {0x0F12,0x53C1, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[16]//hy{0x off : 4341
    {0x0F12,0xE1FE, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[17]//mixer on :E0FA
    {0x0F12,0x0001, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[18]//	par,0xam_e},nd	afit_pCon{0xtBa{0xeVal{0x
    {0x1000,0x0001, WORD_LEN, 10}, // 11. Flicker, CLK //  //{0xet ho{0xt interrupt {0xo main {0xtart run //
    //{0xFFFF,0x000A, WORD_LEN, 0}, //p10 //W,0xait1},0m{0xec                           //
    {0x002A,0x0400, WORD_LEN, 0},// End ,0xu{0x},er init {0xcript
    {0x0F12,0x007F, WORD_LEN, 0}, //5f
    {0x002A,0x03DC, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},//clk {,0x0xet},ting{0x
    {0x002A,0x01B8, WORD_LEN, 0},
    {0x0F12,0x5DC0, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x01C6, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0}, //pvi clock
    {0x0F12,0x0001, WORD_LEN, 0}, //mipi clock
    {0x002A,0x01CC, WORD_LEN, 0},
    {0x0F12,0x1f40, WORD_LEN, 0}, //1f40 //1770 //{0xy{0xtem clk 24m
    {0x0F12,0x2EE0, WORD_LEN, 0}, //pclk 48m  //MIPI clk=pclk*8
    {0x0F12,0x2EE0, WORD_LEN, 0}, 
    {0x0F12,0x1f40, WORD_LEN, 0}, //1f40 //1770 //{0xy{0xtem clk 24m
    {0x0F12,0x2EE0, WORD_LEN, 0}, //pclk 48m  //MIPI clk=pclk*8
    {0x0F12,0x2EE0, WORD_LEN, 0},
    {0x002A,0x01E0, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 100},
    //{0xFFFF,0x0064, WORD_LEN, 0}, //p100   
    {0x002A,0x0242, WORD_LEN, 0},//PREVI,0xEW C},ONFIGURATION 0 ({0xXGA, YUV, 15fp{0x)
    {0x0F12,0x0280, WORD_LEN, 0},	//1280
    {0x0F12,0x01e0, WORD_LEN, 0},	//200 1024
    {0x0F12,0x0005, WORD_LEN, 0},	//YUV
    {0x002A,0x024E, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0}, //clock 1 사용
    {0x002A,0x0248, WORD_LEN, 0},
    {0x0F12,0x2EE0, WORD_LEN, 0},
    {0x0F12,0x2EE0, WORD_LEN, 0},
    {0x0F12,0x0042, WORD_LEN, 0},
    {0x002A,0x0252, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x0250, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0}, 
    {0x002A,0x0262, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0}, //REG_0TC_PCFG_uPrevMirror   [0]x [1]y [2]xy                                 //
    {0x0F12,0x0000, WORD_LEN, 0}, //REG_0TC_PCFG_uCaptureMirror 
    {0x002A,0x0254, WORD_LEN, 0},
    {0x0F12,0x0535, WORD_LEN, 0},
    {0x0F12,0x029A, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0}, //PREVI,0xEW C},ONFIGURATION 1 ({0xXGA, YUV, 15fp{0x)
    {0x0F12,0x0500, WORD_LEN, 0},	//1280
    {0x0F12,0x0400, WORD_LEN, 0},	//1024
    {0x0F12,0x0005, WORD_LEN, 0},	//YUV
    {0x002A,0x0274, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x026E, WORD_LEN, 0},
    {0x0F12,0x2EE0, WORD_LEN, 0},
    {0x0F12,0x2EE0, WORD_LEN, 0},
    {0x0F12,0x0042, WORD_LEN, 0},
    {0x002A,0x0278, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},
    {0x002A,0x0276, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},
    {0x002A,0x0288, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
    {0x0F12,0x0000, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
    {0x002A,0x027A, WORD_LEN, 0},
    {0x0F12,0x0535, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x030E, WORD_LEN, 0}, //CAPTU,0xRE C},ONFIGURATION 0 
    {0x0F12,0x0500, WORD_LEN, 0},  //REG_0TC_CCFG_u{0xWidth	//1280
    {0x0F12,0x03c0, WORD_LEN, 0},  //400 REG_0TC_CCFG_u{0xHeight	//960
    {0x0F12,0x0005, WORD_LEN, 0},  //REG_0TC_PCFG_Format	//YUV
    {0x002A,0x031A, WORD_LEN, 0},                        
    {0x0F12,0x0001, WORD_LEN, 0},  //REG_0TC_CCFG_uClockInd
    {0x002A,0x0314, WORD_LEN, 0},
    {0x0F12,0x2EE0, WORD_LEN, 0}, //1770 //2200 //1800 //1770  //REG_0TC_CCFG_u{0xMaxOut4KHzRate
    {0x0F12,0x2EE0, WORD_LEN, 0}, //1770 //2200 //1800 //1770  //REG_0TC_CCFG_u{0xMinOut4KHzRate
    {0x0F12,0x0042, WORD_LEN, 0},  //REG_0TC_CCFG_PVIMa{0xk
    {0x002A,0x031E, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},  //REG_0TC_CCFG_FrRateQualityType
    {0x002A,0x031C, WORD_LEN, 0},
    {0x0F12,0x0002, WORD_LEN, 0},  //REG_0TC_CCFG_u{0xFrTimeType
    {0x002A,0x0320, WORD_LEN, 0},
    {0x0F12,0x0594, WORD_LEN, 0},  //4e2 535 REG_0TC_CCFG_u{0xMaxFrTimeM{0xecMult10	//Don't change!
    {0x0F12,0x0000, WORD_LEN, 0},  //REG_0TC_CCFG_u{0xMinFrTimeM{0xecMult10	//Don't change!
    {0x002A,0x0226, WORD_LEN, 0},     //REG_TC_GP_CapConfigChanged             
    {0x0F12,0x0001, WORD_LEN, 0},   
    {0x002A,0x10EE, WORD_LEN, 0}, //{0xenHal_uMinCol{0xNoBin *FACTORY ONLY *No Delete//
    {0x0F12,0x097a, WORD_LEN, 0}, //886 //97a //097a //0886 //97a REG_TC_GP_Input{0xChangeReque{0xt/REG_TC_GP_PrevConfigChanged/REG_TC_GP_CapConfigChanged //
    {0x002A,0x03B6, WORD_LEN, 0}, //mipi
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x03FA, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x0F12,0x00C3, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x021C, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0220, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x01F8, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x021E, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x01F0, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x020A, WORD_LEN, 0}, // change InPut //
    {0x0F12,0x0500, WORD_LEN, 0}, //REG_TC_GP_PrevZoomReqInputWidth     //
    {0x0F12,0x03c0, WORD_LEN, 0}, //REG_TC_GP_PrevZoomReqInputHeight    //
    {0x0F12,0x0000, WORD_LEN, 0}, //REG_TC_GP_PrevZoomReqInputWidthOf{0x  //
    {0x0F12,0x0020, WORD_LEN, 0}, //REG_TC_GP_PrevZoomReqInputHeightOf{0x //
    {0x0F12,0x0500, WORD_LEN, 0}, //REG_TC_GP_CapZoomReqInputWidth      //
    {0x0F12,0x03C0, WORD_LEN, 0}, //REG_TC_GP_CapZoomReqInputHeight     //
    {0x0F12,0x0000, WORD_LEN, 0}, //REG_TC_GP_CapZoomReqInputWidthOf{0x   //
    {0x0F12,0x0020, WORD_LEN, 0}, //REG_TC_GP_CapZoomReqInputHeightOf{0x  //
    {0x0F12,0x0001, WORD_LEN, 100} //REG_TC_GP_Input{0xChangeReque{0xt//
    //{0xFFFF,0x0064, WORD_LEN, 0}, //p100
#else
	{0x0010,0x0001, WORD_LEN, 0}, // Reset                                               //
	{0x0004,0x0000, WORD_LEN, 0}, // Disable Auto Address Increment : 0 Chunghwan Park   //
	{0x1030,0x0000, WORD_LEN, 0}, // Clear host interrupt so main will wait              //
	{0x0014,0x0001, WORD_LEN, 0}, // ARM go                                              //
	{0xFFFF,0x0064, WORD_LEN, 0}, //Wait100ms//       
	{0xFCFC,0xD000, WORD_LEN, 0},
	{0x0004,0x0001, WORD_LEN, 0},	// ensable Auto Address Increment : 1 //
	{0x0028,0x7000, WORD_LEN, 0},
	{0x002A,0x1D60, WORD_LEN, 0},
	{0x0F12,0xB570, WORD_LEN, 0},
	{0x0F12,0x4936, WORD_LEN, 0},
	{0x0F12,0x4836, WORD_LEN, 0},
	{0x0F12,0x2205, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA4E, WORD_LEN, 0},
	{0x0F12,0x4935, WORD_LEN, 0},
	{0x0F12,0x2002, WORD_LEN, 0},
	{0x0F12,0x83C8, WORD_LEN, 0},
	{0x0F12,0x2001, WORD_LEN, 0},
	{0x0F12,0x3120, WORD_LEN, 0},
	{0x0F12,0x8088, WORD_LEN, 0},
	{0x0F12,0x4933, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0},
	{0x0F12,0x8008, WORD_LEN, 0},
	{0x0F12,0x4933, WORD_LEN, 0},
	{0x0F12,0x8048, WORD_LEN, 0},
	{0x0F12,0x4933, WORD_LEN, 0},
	{0x0F12,0x4833, WORD_LEN, 0},
	{0x0F12,0x2204, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA3E, WORD_LEN, 0},
	{0x0F12,0x4932, WORD_LEN, 0},
	{0x0F12,0x4833, WORD_LEN, 0},
	{0x0F12,0x2206, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA39, WORD_LEN, 0},
	{0x0F12,0x4932, WORD_LEN, 0},
	{0x0F12,0x4832, WORD_LEN, 0},
	{0x0F12,0x2207, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA34, WORD_LEN, 0},
	{0x0F12,0x4931, WORD_LEN, 0},
	{0x0F12,0x4832, WORD_LEN, 0},
	{0x0F12,0x2208, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA2F, WORD_LEN, 0},
	{0x0F12,0x4931, WORD_LEN, 0},
	{0x0F12,0x4831, WORD_LEN, 0},
	{0x0F12,0x2209, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA2A, WORD_LEN, 0},
	{0x0F12,0x4930, WORD_LEN, 0},
	{0x0F12,0x4831, WORD_LEN, 0},
	{0x0F12,0x220A, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA25, WORD_LEN, 0},
	{0x0F12,0x4930, WORD_LEN, 0},
	{0x0F12,0x4830, WORD_LEN, 0},
	{0x0F12,0x220B, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA20, WORD_LEN, 0},
	{0x0F12,0x482F, WORD_LEN, 0},
	{0x0F12,0x4930, WORD_LEN, 0},
	{0x0F12,0x6108, WORD_LEN, 0},
	{0x0F12,0x4830, WORD_LEN, 0},
	{0x0F12,0x39FF, WORD_LEN, 0},
	{0x0F12,0x3901, WORD_LEN, 0},
	{0x0F12,0x6748, WORD_LEN, 0},
	{0x0F12,0x482F, WORD_LEN, 0},
	{0x0F12,0x1C0A, WORD_LEN, 0},
	{0x0F12,0x32C0, WORD_LEN, 0},
	{0x0F12,0x6390, WORD_LEN, 0},
	{0x0F12,0x482E, WORD_LEN, 0},
	{0x0F12,0x6708, WORD_LEN, 0},
	{0x0F12,0x491A, WORD_LEN, 0},
	{0x0F12,0x482D, WORD_LEN, 0},
	{0x0F12,0x3108, WORD_LEN, 0},
	{0x0F12,0x60C1, WORD_LEN, 0},
	{0x0F12,0x6882, WORD_LEN, 0},
	{0x0F12,0x1A51, WORD_LEN, 0},
	{0x0F12,0x8201, WORD_LEN, 0},
	{0x0F12,0x4C2B, WORD_LEN, 0},
	{0x0F12,0x2607, WORD_LEN, 0},
	{0x0F12,0x6821, WORD_LEN, 0},
	{0x0F12,0x0736, WORD_LEN, 0},
	{0x0F12,0x42B1, WORD_LEN, 0},
	{0x0F12,0xDA05, WORD_LEN, 0},
	{0x0F12,0x4829, WORD_LEN, 0},
	{0x0F12,0x22D8, WORD_LEN, 0},
	{0x0F12,0x1C05, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA09, WORD_LEN, 0},
	{0x0F12,0x6025, WORD_LEN, 0},
	{0x0F12,0x68A1, WORD_LEN, 0},
	{0x0F12,0x42B1, WORD_LEN, 0},
	{0x0F12,0xDA07, WORD_LEN, 0},
	{0x0F12,0x4825, WORD_LEN, 0},
	{0x0F12,0x2224, WORD_LEN, 0},
	{0x0F12,0x3824, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xFA00, WORD_LEN, 0},
	{0x0F12,0x4822, WORD_LEN, 0},
	{0x0F12,0x3824, WORD_LEN, 0},
	{0x0F12,0x60A0, WORD_LEN, 0},
	{0x0F12,0x4D22, WORD_LEN, 0},
	{0x0F12,0x6D29, WORD_LEN, 0},
	{0x0F12,0x42B1, WORD_LEN, 0},
	{0x0F12,0xDA07, WORD_LEN, 0},
	{0x0F12,0x481F, WORD_LEN, 0},
	{0x0F12,0x228F, WORD_LEN, 0},
	{0x0F12,0x00D2, WORD_LEN, 0},
	{0x0F12,0x30D8, WORD_LEN, 0},
	{0x0F12,0x1C04, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF9F2, WORD_LEN, 0},
	{0x0F12,0x652C, WORD_LEN, 0},
	{0x0F12,0xBC70, WORD_LEN, 0},
	{0x0F12,0xBC08, WORD_LEN, 0},
	{0x0F12,0x4718, WORD_LEN, 0},
	{0x0F12,0x218B, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x127B, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0398, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x1376, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x2370, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x1F0D, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x890D, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x1F2F, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x27A9, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x1FE1, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x27C5, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x2043, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x285F, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x2003, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x28FF, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x20CD, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x6181, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x20EF, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x6663, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x2123, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x0100, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x1EC1, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x1EAD, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x1F79, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x04AC, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x06CC, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x23A4, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x0704, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0xB510, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF9B9, WORD_LEN, 0},
	{0x0F12,0x48C3, WORD_LEN, 0},
	{0x0F12,0x49C3, WORD_LEN, 0},
	{0x0F12,0x8800, WORD_LEN, 0},
	{0x0F12,0x8048, WORD_LEN, 0},
	{0x0F12,0xBC10, WORD_LEN, 0},
	{0x0F12,0xBC08, WORD_LEN, 0},
	{0x0F12,0x4718, WORD_LEN, 0},
	{0x0F12,0xB5F8, WORD_LEN, 0},
	{0x0F12,0x1C06, WORD_LEN, 0},
	{0x0F12,0x4DC0, WORD_LEN, 0},
	{0x0F12,0x68AC, WORD_LEN, 0},
	{0x0F12,0x1C30, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF9B3, WORD_LEN, 0},
	{0x0F12,0x68A9, WORD_LEN, 0},
	{0x0F12,0x4ABC, WORD_LEN, 0},
	{0x0F12,0x42A1, WORD_LEN, 0},
	{0x0F12,0xD003, WORD_LEN, 0},
	{0x0F12,0x4BBD, WORD_LEN, 0},
	{0x0F12,0x8A1B, WORD_LEN, 0},
	{0x0F12,0x3301, WORD_LEN, 0},
	{0x0F12,0x8013, WORD_LEN, 0},
	{0x0F12,0x8813, WORD_LEN, 0},
	{0x0F12,0x1C14, WORD_LEN, 0},
	{0x0F12,0x2B00, WORD_LEN, 0},
	{0x0F12,0xD00F, WORD_LEN, 0},
	{0x0F12,0x2201, WORD_LEN, 0},
	{0x0F12,0x4281, WORD_LEN, 0},
	{0x0F12,0xD003, WORD_LEN, 0},
	{0x0F12,0x8C2F, WORD_LEN, 0},
	{0x0F12,0x42B9, WORD_LEN, 0},
	{0x0F12,0xD300, WORD_LEN, 0},
	{0x0F12,0x2200, WORD_LEN, 0},
	{0x0F12,0x60AE, WORD_LEN, 0},
	{0x0F12,0x2A00, WORD_LEN, 0},
	{0x0F12,0xD003, WORD_LEN, 0},
	{0x0F12,0x8C28, WORD_LEN, 0},
	{0x0F12,0x42B0, WORD_LEN, 0},
	{0x0F12,0xD800, WORD_LEN, 0},
	{0x0F12,0x1C30, WORD_LEN, 0},
	{0x0F12,0x1E59, WORD_LEN, 0},
	{0x0F12,0x8021, WORD_LEN, 0},
	{0x0F12,0xBCF8, WORD_LEN, 0},
	{0x0F12,0xBC08, WORD_LEN, 0},
	{0x0F12,0x4718, WORD_LEN, 0},
	{0x0F12,0xB510, WORD_LEN, 0},
	{0x0F12,0x1C04, WORD_LEN, 0},
	{0x0F12,0x48AF, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF997, WORD_LEN, 0},
	{0x0F12,0x4AAD, WORD_LEN, 0},
	{0x0F12,0x4BAE, WORD_LEN, 0},
	{0x0F12,0x8811, WORD_LEN, 0},
	{0x0F12,0x885B, WORD_LEN, 0},
	{0x0F12,0x8852, WORD_LEN, 0},
	{0x0F12,0x4359, WORD_LEN, 0},
	{0x0F12,0x1889, WORD_LEN, 0},
	{0x0F12,0x4288, WORD_LEN, 0},
	{0x0F12,0xD800, WORD_LEN, 0},
	{0x0F12,0x1C08, WORD_LEN, 0},
	{0x0F12,0x6020, WORD_LEN, 0},
	{0x0F12,0xE7C5, WORD_LEN, 0},
	{0x0F12,0xB570, WORD_LEN, 0},
	{0x0F12,0x1C05, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF98F, WORD_LEN, 0},
	{0x0F12,0x49A5, WORD_LEN, 0},
	{0x0F12,0x8989, WORD_LEN, 0},
	{0x0F12,0x4348, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0},
	{0x0F12,0x0C00, WORD_LEN, 0},
	{0x0F12,0x2101, WORD_LEN, 0},
	{0x0F12,0x0349, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF98E, WORD_LEN, 0},
	{0x0F12,0x1C04, WORD_LEN, 0},
	{0x0F12,0x489F, WORD_LEN, 0},
	{0x0F12,0x8F80, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF991, WORD_LEN, 0},
	{0x0F12,0x1C01, WORD_LEN, 0},
	{0x0F12,0x20FF, WORD_LEN, 0},
	{0x0F12,0x43C0, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF994, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF998, WORD_LEN, 0},
	{0x0F12,0x1C01, WORD_LEN, 0},
	{0x0F12,0x4898, WORD_LEN, 0},
	{0x0F12,0x8840, WORD_LEN, 0},
	{0x0F12,0x4360, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0},
	{0x0F12,0x0C00, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF97A, WORD_LEN, 0},
	{0x0F12,0x6028, WORD_LEN, 0},
	{0x0F12,0xBC70, WORD_LEN, 0},
	{0x0F12,0xBC08, WORD_LEN, 0},
	{0x0F12,0x4718, WORD_LEN, 0},
	{0x0F12,0xB5F1, WORD_LEN, 0},
	{0x0F12,0xB082, WORD_LEN, 0},
	{0x0F12,0x4D96, WORD_LEN, 0},
	{0x0F12,0x4E91, WORD_LEN, 0},
	{0x0F12,0x88A8, WORD_LEN, 0},
	{0x0F12,0x1C2C, WORD_LEN, 0},
	{0x0F12,0x3420, WORD_LEN, 0},
	{0x0F12,0x4F90, WORD_LEN, 0},
	{0x0F12,0x2800, WORD_LEN, 0},
	{0x0F12,0xD018, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF988, WORD_LEN, 0},
	{0x0F12,0x9001, WORD_LEN, 0},
	{0x0F12,0x9802, WORD_LEN, 0},
	{0x0F12,0x6B39, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF974, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF978, WORD_LEN, 0},
	{0x0F12,0x9901, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF95F, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0x8871, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF96A, WORD_LEN, 0},
	{0x0F12,0x0400, WORD_LEN, 0},
	{0x0F12,0x0C00, WORD_LEN, 0},
	{0x0F12,0x21FF, WORD_LEN, 0},
	{0x0F12,0x3101, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF97A, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0x88E8, WORD_LEN, 0},
	{0x0F12,0x2800, WORD_LEN, 0},
	{0x0F12,0xD00A, WORD_LEN, 0},
	{0x0F12,0x4980, WORD_LEN, 0},
	{0x0F12,0x8820, WORD_LEN, 0},
	{0x0F12,0x3128, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF979, WORD_LEN, 0},
	{0x0F12,0x8D38, WORD_LEN, 0},
	{0x0F12,0x8871, WORD_LEN, 0},
	{0x0F12,0x4348, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0},
	{0x0F12,0x0C00, WORD_LEN, 0},
	{0x0F12,0x8538, WORD_LEN, 0},
	{0x0F12,0xBCFE, WORD_LEN, 0},
	{0x0F12,0xBC08, WORD_LEN, 0},
	{0x0F12,0x4718, WORD_LEN, 0},
	{0x0F12,0xB510, WORD_LEN, 0},
	{0x0F12,0x1C04, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF974, WORD_LEN, 0},
	{0x0F12,0x6821, WORD_LEN, 0},
	{0x0F12,0x0409, WORD_LEN, 0},
	{0x0F12,0x0C09, WORD_LEN, 0},
	{0x0F12,0x1A40, WORD_LEN, 0},
	{0x0F12,0x4976, WORD_LEN, 0},
	{0x0F12,0x6849, WORD_LEN, 0},
	{0x0F12,0x4281, WORD_LEN, 0},
	{0x0F12,0xD800, WORD_LEN, 0},
	{0x0F12,0x1C08, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF971, WORD_LEN, 0},
	{0x0F12,0x6020, WORD_LEN, 0},
	{0x0F12,0xE75B, WORD_LEN, 0},
	{0x0F12,0xB570, WORD_LEN, 0},
	{0x0F12,0x6801, WORD_LEN, 0},
	{0x0F12,0x040D, WORD_LEN, 0},
	{0x0F12,0x0C2D, WORD_LEN, 0},
	{0x0F12,0x6844, WORD_LEN, 0},
	{0x0F12,0x486F, WORD_LEN, 0},
	{0x0F12,0x8981, WORD_LEN, 0},
	{0x0F12,0x1C28, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF927, WORD_LEN, 0},
	{0x0F12,0x8060, WORD_LEN, 0},
	{0x0F12,0x4970, WORD_LEN, 0},
	{0x0F12,0x69C9, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF968, WORD_LEN, 0},
	{0x0F12,0x1C01, WORD_LEN, 0},
	{0x0F12,0x80A0, WORD_LEN, 0},
	{0x0F12,0x0228, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF92D, WORD_LEN, 0},
	{0x0F12,0x0400, WORD_LEN, 0},
	{0x0F12,0x0C00, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0x496B, WORD_LEN, 0},
	{0x0F12,0x2300, WORD_LEN, 0},
	{0x0F12,0x5EC9, WORD_LEN, 0},
	{0x0F12,0x4288, WORD_LEN, 0},
	{0x0F12,0xDA02, WORD_LEN, 0},
	{0x0F12,0x20FF, WORD_LEN, 0},
	{0x0F12,0x3001, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0xE797, WORD_LEN, 0},
	{0x0F12,0xB5F8, WORD_LEN, 0},
	{0x0F12,0x1C04, WORD_LEN, 0},
	{0x0F12,0x4867, WORD_LEN, 0},
	{0x0F12,0x4E65, WORD_LEN, 0},
	{0x0F12,0x7800, WORD_LEN, 0},
	{0x0F12,0x6AB7, WORD_LEN, 0},
	{0x0F12,0x2800, WORD_LEN, 0},
	{0x0F12,0xD100, WORD_LEN, 0},
	{0x0F12,0x6A37, WORD_LEN, 0},
	{0x0F12,0x495D, WORD_LEN, 0},
	{0x0F12,0x2800, WORD_LEN, 0},
	{0x0F12,0x688D, WORD_LEN, 0},
	{0x0F12,0xD100, WORD_LEN, 0},
	{0x0F12,0x684D, WORD_LEN, 0},
	{0x0F12,0x4859, WORD_LEN, 0},
	{0x0F12,0x8841, WORD_LEN, 0},
	{0x0F12,0x6820, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF94B, WORD_LEN, 0},
	{0x0F12,0x8DF1, WORD_LEN, 0},
	{0x0F12,0x434F, WORD_LEN, 0},
	{0x0F12,0x0A3A, WORD_LEN, 0},
	{0x0F12,0x4282, WORD_LEN, 0},
	{0x0F12,0xD30C, WORD_LEN, 0},
	{0x0F12,0x4D5C, WORD_LEN, 0},
	{0x0F12,0x26FF, WORD_LEN, 0},
	{0x0F12,0x8829, WORD_LEN, 0},
	{0x0F12,0x3601, WORD_LEN, 0},
	{0x0F12,0x43B1, WORD_LEN, 0},
	{0x0F12,0x8029, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF944, WORD_LEN, 0},
	{0x0F12,0x6020, WORD_LEN, 0},
	{0x0F12,0x8828, WORD_LEN, 0},
	{0x0F12,0x4330, WORD_LEN, 0},
	{0x0F12,0x8028, WORD_LEN, 0},
	{0x0F12,0xE73B, WORD_LEN, 0},
	{0x0F12,0x1C0A, WORD_LEN, 0},
	{0x0F12,0x436A, WORD_LEN, 0},
	{0x0F12,0x0A12, WORD_LEN, 0},
	{0x0F12,0x4282, WORD_LEN, 0},
	{0x0F12,0xD304, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF8F3, WORD_LEN, 0},
	{0x0F12,0x6020, WORD_LEN, 0},
	{0x0F12,0xE7F4, WORD_LEN, 0},
	{0x0F12,0x6025, WORD_LEN, 0},
	{0x0F12,0xE7F2, WORD_LEN, 0},
	{0x0F12,0xB410, WORD_LEN, 0},
	{0x0F12,0x4848, WORD_LEN, 0},
	{0x0F12,0x4950, WORD_LEN, 0},
	{0x0F12,0x89C0, WORD_LEN, 0},
	{0x0F12,0x2316, WORD_LEN, 0},
	{0x0F12,0x5ECC, WORD_LEN, 0},
	{0x0F12,0x1C02, WORD_LEN, 0},
	{0x0F12,0x42A0, WORD_LEN, 0},
	{0x0F12,0xDC00, WORD_LEN, 0},
	{0x0F12,0x1C22, WORD_LEN, 0},
	{0x0F12,0x82CA, WORD_LEN, 0},
	{0x0F12,0x2318, WORD_LEN, 0},
	{0x0F12,0x5ECA, WORD_LEN, 0},
	{0x0F12,0x4290, WORD_LEN, 0},
	{0x0F12,0xDC00, WORD_LEN, 0},
	{0x0F12,0x1C10, WORD_LEN, 0},
	{0x0F12,0x8308, WORD_LEN, 0},
	{0x0F12,0xBC10, WORD_LEN, 0},
	{0x0F12,0x4770, WORD_LEN, 0},
	{0x0F12,0xB570, WORD_LEN, 0},
	{0x0F12,0x1C06, WORD_LEN, 0},
	{0x0F12,0x4C45, WORD_LEN, 0},
	{0x0F12,0x2501, WORD_LEN, 0},
	{0x0F12,0x8820, WORD_LEN, 0},
	{0x0F12,0x02AD, WORD_LEN, 0},
	{0x0F12,0x43A8, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF91E, WORD_LEN, 0},
	{0x0F12,0x6030, WORD_LEN, 0},
	{0x0F12,0xF7FF, WORD_LEN, 0},
	{0x0F12,0xFFE0, WORD_LEN, 0},
	{0x0F12,0x8820, WORD_LEN, 0},
	{0x0F12,0x4328, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0xE741, WORD_LEN, 0},
	{0x0F12,0xB570, WORD_LEN, 0},
	{0x0F12,0x4C3D, WORD_LEN, 0},
	{0x0F12,0x2501, WORD_LEN, 0},
	{0x0F12,0x8820, WORD_LEN, 0},
	{0x0F12,0x02ED, WORD_LEN, 0},
	{0x0F12,0x43A8, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF916, WORD_LEN, 0},
	{0x0F12,0xF7FF, WORD_LEN, 0},
	{0x0F12,0xFFD1, WORD_LEN, 0},
	{0x0F12,0x8820, WORD_LEN, 0},
	{0x0F12,0x4328, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0xE732, WORD_LEN, 0},
	{0x0F12,0x230D, WORD_LEN, 0},
	{0x0F12,0x071B, WORD_LEN, 0},
	{0x0F12,0x18C3, WORD_LEN, 0},
	{0x0F12,0x8818, WORD_LEN, 0},
	{0x0F12,0x2A00, WORD_LEN, 0},
	{0x0F12,0xD001, WORD_LEN, 0},
	{0x0F12,0x4308, WORD_LEN, 0},
	{0x0F12,0xE000, WORD_LEN, 0},
	{0x0F12,0x4388, WORD_LEN, 0},
	{0x0F12,0x8018, WORD_LEN, 0},
	{0x0F12,0x4770, WORD_LEN, 0},
	{0x0F12,0xB570, WORD_LEN, 0},
	{0x0F12,0x2402, WORD_LEN, 0},
	{0x0F12,0x4932, WORD_LEN, 0},
	{0x0F12,0x8809, WORD_LEN, 0},
	{0x0F12,0x078A, WORD_LEN, 0},
	{0x0F12,0xD500, WORD_LEN, 0},
	{0x0F12,0x2406, WORD_LEN, 0},
	{0x0F12,0x2900, WORD_LEN, 0},
	{0x0F12,0xD01F, WORD_LEN, 0},
	{0x0F12,0x1C02, WORD_LEN, 0},
	{0x0F12,0x207D, WORD_LEN, 0},
	{0x0F12,0x00C0, WORD_LEN, 0},
	{0x0F12,0x2600, WORD_LEN, 0},
	{0x0F12,0x4D2D, WORD_LEN, 0},
	{0x0F12,0x2A00, WORD_LEN, 0},
	{0x0F12,0xD019, WORD_LEN, 0},
	{0x0F12,0x2101, WORD_LEN, 0},
	{0x0F12,0x8229, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF8F9, WORD_LEN, 0},
	{0x0F12,0x2200, WORD_LEN, 0},
	{0x0F12,0x2101, WORD_LEN, 0},
	{0x0F12,0x482A, WORD_LEN, 0},
	{0x0F12,0x0309, WORD_LEN, 0},
	{0x0F12,0xF7FF, WORD_LEN, 0},
	{0x0F12,0xFFDB, WORD_LEN, 0},
	{0x0F12,0x2008, WORD_LEN, 0},
	{0x0F12,0x4304, WORD_LEN, 0},
	{0x0F12,0x1C21, WORD_LEN, 0},
	{0x0F12,0x4C26, WORD_LEN, 0},
	{0x0F12,0x2200, WORD_LEN, 0},
	{0x0F12,0x3C14, WORD_LEN, 0},
	{0x0F12,0x1C20, WORD_LEN, 0},
	{0x0F12,0xF7FF, WORD_LEN, 0},
	{0x0F12,0xFFD2, WORD_LEN, 0},
	{0x0F12,0x2200, WORD_LEN, 0},
	{0x0F12,0x2121, WORD_LEN, 0},
	{0x0F12,0x1C20, WORD_LEN, 0},
	{0x0F12,0xF7FF, WORD_LEN, 0},
	{0x0F12,0xFFCD, WORD_LEN, 0},
	{0x0F12,0x802E, WORD_LEN, 0},
	{0x0F12,0xE6FD, WORD_LEN, 0},
	{0x0F12,0x822E, WORD_LEN, 0},
	{0x0F12,0x0789, WORD_LEN, 0},
	{0x0F12,0x0FC9, WORD_LEN, 0},
	{0x0F12,0x0089, WORD_LEN, 0},
	{0x0F12,0x223B, WORD_LEN, 0},
	{0x0F12,0x4311, WORD_LEN, 0},
	{0x0F12,0x8029, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF8DA, WORD_LEN, 0},
	{0x0F12,0xE7F4, WORD_LEN, 0},
	{0x0F12,0xB510, WORD_LEN, 0},
	{0x0F12,0x491B, WORD_LEN, 0},
	{0x0F12,0x8FC8, WORD_LEN, 0},
	{0x0F12,0x2800, WORD_LEN, 0},
	{0x0F12,0xD007, WORD_LEN, 0},
	{0x0F12,0x2000, WORD_LEN, 0},
	{0x0F12,0x87C8, WORD_LEN, 0},
	{0x0F12,0x8F88, WORD_LEN, 0},
	{0x0F12,0x4C19, WORD_LEN, 0},
	{0x0F12,0x2800, WORD_LEN, 0},
	{0x0F12,0xD002, WORD_LEN, 0},
	{0x0F12,0x2008, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0xE689, WORD_LEN, 0},
	{0x0F12,0x4815, WORD_LEN, 0},
	{0x0F12,0x3060, WORD_LEN, 0},
	{0x0F12,0x8900, WORD_LEN, 0},
	{0x0F12,0x2800, WORD_LEN, 0},
	{0x0F12,0xD103, WORD_LEN, 0},
	{0x0F12,0x4814, WORD_LEN, 0},
	{0x0F12,0x2101, WORD_LEN, 0},
	{0x0F12,0xF000, WORD_LEN, 0},
	{0x0F12,0xF8CA, WORD_LEN, 0},
	{0x0F12,0x2010, WORD_LEN, 0},
	{0x0F12,0x8020, WORD_LEN, 0},
	{0x0F12,0xE7F2, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x1376, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x2370, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x14D8, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x235C, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0xF4B0, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x1554, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x1AB8, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x0080, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x046C, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x0468, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x1100, WORD_LEN, 0},
	{0x0F12,0xD000, WORD_LEN, 0},
	{0x0F12,0x198C, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x0AC4, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0xB0A0, WORD_LEN, 0},
	{0x0F12,0xD000, WORD_LEN, 0},
	{0x0F12,0xB0B4, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x01B8, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x044E, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x0450, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x9CE7, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xF004, WORD_LEN, 0},
	{0x0F12,0xE51F, WORD_LEN, 0},
	{0x0F12,0x9FB8, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x14C1, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x27E1, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x88DF, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x275D, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x1ED3, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x27C5, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xF004, WORD_LEN, 0},
	{0x0F12,0xE51F, WORD_LEN, 0},
	{0x0F12,0xA144, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x1F87, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x27A9, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x1ECB, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x28FF, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x26F9, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x4027, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x9F03, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xF004, WORD_LEN, 0},
	{0x0F12,0xE51F, WORD_LEN, 0},
	{0x0F12,0x9D9C, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x285F, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x6181, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x6663, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x85D9, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x4778, WORD_LEN, 0},
	{0x0F12,0x46C0, WORD_LEN, 0},
	{0x0F12,0xC000, WORD_LEN, 0},
	{0x0F12,0xE59F, WORD_LEN, 0},
	{0x0F12,0xFF1C, WORD_LEN, 0},
	{0x0F12,0xE12F, WORD_LEN, 0},
	{0x0F12,0x2001, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0xE848, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x0F12,0xE848, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x0F12,0x0500, WORD_LEN, 0},
	{0x0F12,0x0064, WORD_LEN, 0},
	{0x0F12,0x0002, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0}, // End T&P part //
	{0x0004,0x0000, WORD_LEN, 0}, //  2. Analog Setting   //       
	{0xF454,0x0001, WORD_LEN, 0}, // ADC sat = 750mV(50h), NTG = -0.8V(10h), Saturation margin low limit = 732LSB //
	{0xF418,0x0050, WORD_LEN, 0},
	{0xF43E,0x0010, WORD_LEN, 0},
	{0x0004,0x0001, WORD_LEN, 0}, //Disable Auto Address Increment : 1//
	{0x002A,0x112A, WORD_LEN, 0}, // Analog Settings // //senHal_SenRegsModes3_pSenModesRegsArray3[8]   //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                              //
	{0x002A,0x1132, WORD_LEN, 0}, //senHal_SenRegsModes3_pSenModesRegsArray3[12]  //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                              //
	{0x002A,0x113E, WORD_LEN, 0}, //senHal_SenRegsModes3_pSenModesRegsArray3[18]  //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                              //
	{0x002A,0x115C, WORD_LEN, 0}, //senHal_SenRegsModes3_pSenModesRegsArray3[33]  //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                              //
	{0x002A,0x1164, WORD_LEN, 0}, //senHal_SenRegsModes3_pSenModesRegsArray3[37]  //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                              //
	{0x002A,0x1174, WORD_LEN, 0}, //senHal_SenRegsModes3_pSenModesRegsArray3[45]  //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                              //
	{0x002A,0x1178, WORD_LEN, 0}, //senHal_SenRegsModes3_pSenModesRegsArray3[47]  //
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x002A,0x077A, WORD_LEN, 0}, // msm_uOffsetNoBin //
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x002A,0x07A2, WORD_LEN, 0}, // msm_sAnalogOffset //
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x002A,0x07B6, WORD_LEN, 0}, // msm_NonLinearOfsOutput //
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0002, WORD_LEN, 0},
	{0x0F12,0x0004, WORD_LEN, 0},
	{0x0F12,0x0004, WORD_LEN, 0},
	{0x0F12,0x0005, WORD_LEN, 0},
	{0x0F12,0x0005, WORD_LEN, 0},       
	{0x0028,0x7000, WORD_LEN, 0}, //ESD Check Code add_100505//
	{0x002A,0x0132, WORD_LEN, 0},
	{0x0F12,0xAAAA, WORD_LEN, 0},
	{0x002A,0x1000, WORD_LEN, 0}, // 3. AE & AE weight ////TVAR_ae_BrAve                                    //
	{0x0F12,0x0036, WORD_LEN, 0}, //35  //       
	{0x002A,0x0474, WORD_LEN, 0},
	{0x0F12,0x0112, WORD_LEN, 0}, //lt_uLimitHigh    //
	{0x0F12,0x00EF, WORD_LEN, 0}, //lt_uLimitLow     //
	{0x002A,0x1006, WORD_LEN, 0},
	{0x0F12,0x001F, WORD_LEN, 0}, //ae_{0xtatMode//       
	{0x002A,0x108E, WORD_LEN, 0}, //{0xARR_IllumType//
	{0x0F12,0x00C7, WORD_LEN, 0},
	{0x0F12,0x00F7, WORD_LEN, 0},
	{0x0F12,0x0107, WORD_LEN, 0},
	{0x0F12,0x0142, WORD_LEN, 0},
	{0x0F12,0x017A, WORD_LEN, 0},
	{0x0F12,0x01A0, WORD_LEN, 0},
	{0x0F12,0x01B6, WORD_LEN, 0},       
	{0x0F12,0x0100, WORD_LEN, 0}, //{0xARR_IllumTypeF	         //
	{0x0F12,0x0100, WORD_LEN, 0}, //0122                      //
	{0x0F12,0x0100, WORD_LEN, 0}, //0136                      //
	{0x0F12,0x0100, WORD_LEN, 0}, //00F6                      //
	{0x0F12,0x0100, WORD_LEN, 0}, //0100                      //
	{0x0F12,0x0100, WORD_LEN, 0}, //00FE                      //
	{0x0F12,0x0100, WORD_LEN, 0}, //0100                      //       
	{0x002A,0x0488, WORD_LEN, 0},
	{0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                   //
	{0x0F12,0xA316, WORD_LEN, 0}, //lt_uMaxExp2                        //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                   //
	{0x002A,0x2360, WORD_LEN, 0}, //2174 AWBBTune_EVT4_uMaxExp3      //
	{0x0F12,0xF424, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
	{0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                   //
	{0x0F12,0xA316, WORD_LEN, 0}, //lt_uCapMaxExp2                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //                                   //
	{0x002A,0x2364, WORD_LEN, 0}, //2178 AWBBTune_EVT4_uCapMaxExp3   //
	{0x0F12,0xF424, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x002A,0x0498, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux {0xhutter       //
	{0x0F12,0x0310, WORD_LEN, 0}, //lt_uMaxAnGain2 	                                   //
	{0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
	{0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
	{0x002A,0x049C, WORD_LEN, 0}, //                                                    //
	{0x0F12,0x0100, WORD_LEN, 0}, //lt_uMaxDigGain                                      //       
	{0x002A,0x235C, WORD_LEN, 0}, //2170                                              //
	{0x0F12,0x0002, WORD_LEN, 0}, //0001 AWBBTune_EVT4_uMinCoar{0xe                       //
	{0x0F12,0x0090, WORD_LEN, 0}, //AWBBTune_EVT4_uMinFine                              //       
	{0x002A,0x100e, WORD_LEN, 0}, //ae_WeightTbl_16//
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0302, WORD_LEN, 0},
	{0x0F12,0x0203, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0403, WORD_LEN, 0},
	{0x0F12,0x0304, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0403, WORD_LEN, 0},
	{0x0F12,0x0304, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0302, WORD_LEN, 0},
	{0x0F12,0x0203, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0101, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x002A,0x0f4c, WORD_LEN, 0}, //brightne{0x{0x //
	{0x0F12,0x02b0, WORD_LEN, 0}, //180,       //
	{0x002A,0x0f52, WORD_LEN, 0}, //           //
	{0x0F12,0x02f0, WORD_LEN, 0}, //180,       //
	{0x002A,0x0754, WORD_LEN, 0}, // 4. {0xhading (GA{0x) // //TVAR_a{0xh_pGA{0x//
	{0x0F12,0x247C, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},     
	{0x002A,0x247C, WORD_LEN, 0}, //	param_{0xtart	TVAR_a{0xh_pGA{0x //
	{0x0F12,0x0141, WORD_LEN, 0}, //TVAR_ash_pGA{0x[0]         //
	{0x0F12,0x010F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[1]         //
	{0x0F12,0x00D9, WORD_LEN, 0}, //TVAR_ash_pGA{0x[2]         //
	{0x0F12,0x00B4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[3]         //
	{0x0F12,0x009E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[4]         //
	{0x0F12,0x0090, WORD_LEN, 0}, //TVAR_ash_pGA{0x[5]         //
	{0x0F12,0x008A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[6]         //
	{0x0F12,0x008C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[7]         //
	{0x0F12,0x009A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[8]         //
	{0x0F12,0x00BB, WORD_LEN, 0}, //TVAR_ash_pGA{0x[9]         //
	{0x0F12,0x00EC, WORD_LEN, 0}, //TVAR_ash_pGA{0x[10]        //
	{0x0F12,0x012C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[11]        //
	{0x0F12,0x0142, WORD_LEN, 0}, //TVAR_ash_pGA{0x[12]        //
	{0x0F12,0x0103, WORD_LEN, 0}, //TVAR_ash_pGA{0x[13]        //
	{0x0F12,0x00C9, WORD_LEN, 0}, //TVAR_ash_pGA{0x[14]        //
	{0x0F12,0x0096, WORD_LEN, 0}, //TVAR_ash_pGA{0x[15]        //
	{0x0F12,0x0075, WORD_LEN, 0}, //TVAR_ash_pGA{0x[16]        //
	{0x0F12,0x0062, WORD_LEN, 0}, //TVAR_ash_pGA{0x[17]        //
	{0x0F12,0x0057, WORD_LEN, 0}, //TVAR_ash_pGA{0x[18]        //
	{0x0F12,0x0053, WORD_LEN, 0}, //TVAR_ash_pGA{0x[19]        //
	{0x0F12,0x0055, WORD_LEN, 0}, //TVAR_ash_pGA{0x[20]        //
	{0x0F12,0x005E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[21]        //
	{0x0F12,0x0076, WORD_LEN, 0}, //TVAR_ash_pGA{0x[22]        //
	{0x0F12,0x009E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[23]        //
	{0x0F12,0x00E0, WORD_LEN, 0}, //TVAR_ash_pGA{0x[24]        //
	{0x0F12,0x0114, WORD_LEN, 0}, //TVAR_ash_pGA{0x[25]        //
	{0x0F12,0x00CC, WORD_LEN, 0}, //TVAR_ash_pGA{0x[26]        //
	{0x0F12,0x0091, WORD_LEN, 0}, //TVAR_ash_pGA{0x[27]        //
	{0x0F12,0x0062, WORD_LEN, 0}, //TVAR_ash_pGA{0x[28]        //
	{0x0F12,0x0046, WORD_LEN, 0}, //TVAR_ash_pGA{0x[29]        //
	{0x0F12,0x0035, WORD_LEN, 0}, //TVAR_ash_pGA{0x[30]        //
	{0x0F12,0x002B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[31]        //
	{0x0F12,0x0028, WORD_LEN, 0}, //TVAR_ash_pGA{0x[32]        //
	{0x0F12,0x002B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[33]        //
	{0x0F12,0x0036, WORD_LEN, 0}, //TVAR_ash_pGA{0x[34]        //
	{0x0F12,0x0049, WORD_LEN, 0}, //TVAR_ash_pGA{0x[35]        //
	{0x0F12,0x006D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[36]        //
	{0x0F12,0x00AC, WORD_LEN, 0}, //TVAR_ash_pGA{0x[37]        //
	{0x0F12,0x00EE, WORD_LEN, 0}, //TVAR_ash_pGA{0x[38]        //
	{0x0F12,0x00AA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[39]        //
	{0x0F12,0x0072, WORD_LEN, 0}, //TVAR_ash_pGA{0x[40]        //
	{0x0F12,0x0046, WORD_LEN, 0}, //TVAR_ash_pGA{0x[41]        //
	{0x0F12,0x002C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[42]        //
	{0x0F12,0x001B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[43]        //
	{0x0F12,0x0010, WORD_LEN, 0}, //TVAR_ash_pGA{0x[44]        //
	{0x0F12,0x000D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[45]        //
	{0x0F12,0x0013, WORD_LEN, 0}, //TVAR_ash_pGA{0x[46]        //
	{0x0F12,0x0020, WORD_LEN, 0}, //TVAR_ash_pGA{0x[47]        //
	{0x0F12,0x0035, WORD_LEN, 0}, //TVAR_ash_pGA{0x[48]        //
	{0x0F12,0x0056, WORD_LEN, 0}, //TVAR_ash_pGA{0x[49]        //
	{0x0F12,0x0092, WORD_LEN, 0}, //TVAR_ash_pGA{0x[50]        //
	{0x0F12,0x00D4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[51]        //
	{0x0F12,0x009E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[52]        //
	{0x0F12,0x0066, WORD_LEN, 0}, //TVAR_ash_pGA{0x[53]        //
	{0x0F12,0x003A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[54]        //
	{0x0F12,0x001F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[55]        //
	{0x0F12,0x000D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[56]        //
	{0x0F12,0x0002, WORD_LEN, 0}, //TVAR_ash_pGA{0x[57]        //
	{0x0F12,0x0000, WORD_LEN, 0}, //TVAR_ash_pGA{0x[58]        //
	{0x0F12,0x0007, WORD_LEN, 0}, //TVAR_ash_pGA{0x[59]        //
	{0x0F12,0x0017, WORD_LEN, 0}, //TVAR_ash_pGA{0x[60]        //
	{0x0F12,0x002D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[61]        //
	{0x0F12,0x004F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[62]        //
	{0x0F12,0x0089, WORD_LEN, 0}, //TVAR_ash_pGA{0x[63]        //
	{0x0F12,0x00CA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[64]        //
	{0x0F12,0x00A1, WORD_LEN, 0}, //TVAR_ash_pGA{0x[65]        //
	{0x0F12,0x0068, WORD_LEN, 0}, //TVAR_ash_pGA{0x[66]        //
	{0x0F12,0x003B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[67]        //
	{0x0F12,0x001F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[68]        //
	{0x0F12,0x000C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[69]        //
	{0x0F12,0x0001, WORD_LEN, 0}, //TVAR_ash_pGA{0x[70]        //
	{0x0F12,0x0000, WORD_LEN, 0}, //TVAR_ash_pGA{0x[71]        //
	{0x0F12,0x0008, WORD_LEN, 0}, //TVAR_ash_pGA{0x[72]        //
	{0x0F12,0x0019, WORD_LEN, 0}, //TVAR_ash_pGA{0x[73]        //
	{0x0F12,0x0031, WORD_LEN, 0}, //TVAR_ash_pGA{0x[74]        //
	{0x0F12,0x0052, WORD_LEN, 0}, //TVAR_ash_pGA{0x[75]        //
	{0x0F12,0x008C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[76]        //
	{0x0F12,0x00CF, WORD_LEN, 0}, //TVAR_ash_pGA{0x[77]        //
	{0x0F12,0x00B5, WORD_LEN, 0}, //TVAR_ash_pGA{0x[78]        //
	{0x0F12,0x0077, WORD_LEN, 0}, //TVAR_ash_pGA{0x[79]        //
	{0x0F12,0x0046, WORD_LEN, 0}, //TVAR_ash_pGA{0x[80]        //
	{0x0F12,0x0029, WORD_LEN, 0}, //TVAR_ash_pGA{0x[81]        //
	{0x0F12,0x0015, WORD_LEN, 0}, //TVAR_ash_pGA{0x[82]        //
	{0x0F12,0x000A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[83]        //
	{0x0F12,0x0009, WORD_LEN, 0}, //TVAR_ash_pGA{0x[84]        //
	{0x0F12,0x0011, WORD_LEN, 0}, //TVAR_ash_pGA{0x[85]        //
	{0x0F12,0x0024, WORD_LEN, 0}, //TVAR_ash_pGA{0x[86]        //
	{0x0F12,0x003D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[87]        //
	{0x0F12,0x0062, WORD_LEN, 0}, //TVAR_ash_pGA{0x[88]        //
	{0x0F12,0x009F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[89]        //
	{0x0F12,0x00E5, WORD_LEN, 0}, //TVAR_ash_pGA{0x[90]        //
	{0x0F12,0x00D9, WORD_LEN, 0}, //TVAR_ash_pGA{0x[91]        //
	{0x0F12,0x0095, WORD_LEN, 0}, //TVAR_ash_pGA{0x[92]        //
	{0x0F12,0x005E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[93]        //
	{0x0F12,0x003E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[94]        //
	{0x0F12,0x002A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[95]        //
	{0x0F12,0x001F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[96]        //
	{0x0F12,0x001E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[97]        //
	{0x0F12,0x0026, WORD_LEN, 0}, //TVAR_ash_pGA{0x[98]        //
	{0x0F12,0x0039, WORD_LEN, 0}, //TVAR_ash_pGA{0x[99]        //
	{0x0F12,0x0053, WORD_LEN, 0}, //TVAR_ash_pGA{0x[100]       //
	{0x0F12,0x007D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[101]       //
	{0x0F12,0x00C1, WORD_LEN, 0}, //TVAR_ash_pGA{0x[102]       //
	{0x0F12,0x010C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[103]       //
	{0x0F12,0x0109, WORD_LEN, 0}, //TVAR_ash_pGA{0x[104]       //
	{0x0F12,0x00C1, WORD_LEN, 0}, //TVAR_ash_pGA{0x[105]       //
	{0x0F12,0x0084, WORD_LEN, 0}, //TVAR_ash_pGA{0x[106]       //
	{0x0F12,0x005D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[107]       //
	{0x0F12,0x0048, WORD_LEN, 0}, //TVAR_ash_pGA{0x[108]       //
	{0x0F12,0x003D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[109]       //
	{0x0F12,0x003B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[110]       //
	{0x0F12,0x0043, WORD_LEN, 0}, //TVAR_ash_pGA{0x[111]       //
	{0x0F12,0x0056, WORD_LEN, 0}, //TVAR_ash_pGA{0x[112]       //
	{0x0F12,0x0075, WORD_LEN, 0}, //TVAR_ash_pGA{0x[113]       //
	{0x0F12,0x00A6, WORD_LEN, 0}, //TVAR_ash_pGA{0x[114]       //
	{0x0F12,0x00F5, WORD_LEN, 0}, //TVAR_ash_pGA{0x[115]       //
	{0x0F12,0x0143, WORD_LEN, 0}, //TVAR_ash_pGA{0x[116]       //
	{0x0F12,0x0150, WORD_LEN, 0}, //TVAR_ash_pGA{0x[117]       //
	{0x0F12,0x0105, WORD_LEN, 0}, //TVAR_ash_pGA{0x[118]       //
	{0x0F12,0x00BD, WORD_LEN, 0}, //TVAR_ash_pGA{0x[119]       //
	{0x0F12,0x008F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[120]       //
	{0x0F12,0x0074, WORD_LEN, 0}, //TVAR_ash_pGA{0x[121]       //
	{0x0F12,0x0067, WORD_LEN, 0}, //TVAR_ash_pGA{0x[122]       //
	{0x0F12,0x0064, WORD_LEN, 0}, //TVAR_ash_pGA{0x[123]       //
	{0x0F12,0x006C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[124]       //
	{0x0F12,0x0082, WORD_LEN, 0}, //TVAR_ash_pGA{0x[125]       //
	{0x0F12,0x00A8, WORD_LEN, 0}, //TVAR_ash_pGA{0x[126]       //
	{0x0F12,0x00E3, WORD_LEN, 0}, //TVAR_ash_pGA{0x[127]       //
	{0x0F12,0x013A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[128]       //
	{0x0F12,0x0174, WORD_LEN, 0}, //TVAR_ash_pGA{0x[129]       //
	{0x0F12,0x0187, WORD_LEN, 0}, //TVAR_ash_pGA{0x[130]       //
	{0x0F12,0x0152, WORD_LEN, 0}, //TVAR_ash_pGA{0x[131]       //
	{0x0F12,0x0106, WORD_LEN, 0}, //TVAR_ash_pGA{0x[132]       //
	{0x0F12,0x00D1, WORD_LEN, 0}, //TVAR_ash_pGA{0x[133]       //
	{0x0F12,0x00B0, WORD_LEN, 0}, //TVAR_ash_pGA{0x[134]       //
	{0x0F12,0x009D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[135]       //
	{0x0F12,0x0098, WORD_LEN, 0}, //TVAR_ash_pGA{0x[136]       //
	{0x0F12,0x00A0, WORD_LEN, 0}, //TVAR_ash_pGA{0x[137]       //
	{0x0F12,0x00B9, WORD_LEN, 0}, //TVAR_ash_pGA{0x[138]       //
	{0x0F12,0x00E8, WORD_LEN, 0}, //TVAR_ash_pGA{0x[139]       //
	{0x0F12,0x012C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[140]       //
	{0x0F12,0x017A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[141]       //
	{0x0F12,0x018D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[142]       //
	{0x0F12,0x012F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[143]       //
	{0x0F12,0x00FC, WORD_LEN, 0}, //TVAR_ash_pGA{0x[144]       //
	{0x0F12,0x00C6, WORD_LEN, 0}, //TVAR_ash_pGA{0x[145]       //
	{0x0F12,0x00A3, WORD_LEN, 0}, //TVAR_ash_pGA{0x[146]       //
	{0x0F12,0x008E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[147]       //
	{0x0F12,0x0083, WORD_LEN, 0}, //TVAR_ash_pGA{0x[148]       //
	{0x0F12,0x007E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[149]       //
	{0x0F12,0x007F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[150]       //
	{0x0F12,0x008B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[151]       //
	{0x0F12,0x00A8, WORD_LEN, 0}, //TVAR_ash_pGA{0x[152]       //
	{0x0F12,0x00D8, WORD_LEN, 0}, //TVAR_ash_pGA{0x[153]       //
	{0x0F12,0x0115, WORD_LEN, 0}, //TVAR_ash_pGA{0x[154]       //
	{0x0F12,0x0131, WORD_LEN, 0}, //TVAR_ash_pGA{0x[155]       //
	{0x0F12,0x00ED, WORD_LEN, 0}, //TVAR_ash_pGA{0x[156]       //
	{0x0F12,0x00B4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[157]       //
	{0x0F12,0x0085, WORD_LEN, 0}, //TVAR_ash_pGA{0x[158]       //
	{0x0F12,0x0069, WORD_LEN, 0}, //TVAR_ash_pGA{0x[159]       //
	{0x0F12,0x0059, WORD_LEN, 0}, //TVAR_ash_pGA{0x[160]       //
	{0x0F12,0x004E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[161]       //
	{0x0F12,0x004C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[162]       //
	{0x0F12,0x004D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[163]       //
	{0x0F12,0x0055, WORD_LEN, 0}, //TVAR_ash_pGA{0x[164]       //
	{0x0F12,0x006A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[165]       //
	{0x0F12,0x008D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[166]       //
	{0x0F12,0x00CA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[167]       //
	{0x0F12,0x00FF, WORD_LEN, 0}, //TVAR_ash_pGA{0x[168]       //
	{0x0F12,0x00B7, WORD_LEN, 0}, //TVAR_ash_pGA{0x[169]       //
	{0x0F12,0x0082, WORD_LEN, 0}, //TVAR_ash_pGA{0x[170]       //
	{0x0F12,0x0058, WORD_LEN, 0}, //TVAR_ash_pGA{0x[171]       //
	{0x0F12,0x003F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[172]       //
	{0x0F12,0x0031, WORD_LEN, 0}, //TVAR_ash_pGA{0x[173]       //
	{0x0F12,0x0028, WORD_LEN, 0}, //TVAR_ash_pGA{0x[174]       //
	{0x0F12,0x0025, WORD_LEN, 0}, //TVAR_ash_pGA{0x[175]       //
	{0x0F12,0x0029, WORD_LEN, 0}, //TVAR_ash_pGA{0x[176]       //
	{0x0F12,0x0032, WORD_LEN, 0}, //TVAR_ash_pGA{0x[177]       //
	{0x0F12,0x0044, WORD_LEN, 0}, //TVAR_ash_pGA{0x[178]       //
	{0x0F12,0x0064, WORD_LEN, 0}, //TVAR_ash_pGA{0x[179]       //
	{0x0F12,0x009D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[180]       //
	{0x0F12,0x00DD, WORD_LEN, 0}, //TVAR_ash_pGA{0x[181]       //
	{0x0F12,0x009A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[182]       //
	{0x0F12,0x0066, WORD_LEN, 0}, //TVAR_ash_pGA{0x[183]       //
	{0x0F12,0x003F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[184]       //
	{0x0F12,0x0028, WORD_LEN, 0}, //TVAR_ash_pGA{0x[185]       //
	{0x0F12,0x0018, WORD_LEN, 0}, //TVAR_ash_pGA{0x[186]       //
	{0x0F12,0x000E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[187]       //
	{0x0F12,0x000C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[188]       //
	{0x0F12,0x0011, WORD_LEN, 0}, //TVAR_ash_pGA{0x[189]       //
	{0x0F12,0x001F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[190]       //
	{0x0F12,0x0032, WORD_LEN, 0}, //TVAR_ash_pGA{0x[191]       //
	{0x0F12,0x0050, WORD_LEN, 0}, //TVAR_ash_pGA{0x[192]       //
	{0x0F12,0x0086, WORD_LEN, 0}, //TVAR_ash_pGA{0x[193]       //
	{0x0F12,0x00C7, WORD_LEN, 0}, //TVAR_ash_pGA{0x[194]       //
	{0x0F12,0x008E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[195]       //
	{0x0F12,0x005B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[196]       //
	{0x0F12,0x0034, WORD_LEN, 0}, //TVAR_ash_pGA{0x[197]       //
	{0x0F12,0x001B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[198]       //
	{0x0F12,0x000B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[199]       //
	{0x0F12,0x0001, WORD_LEN, 0}, //TVAR_ash_pGA{0x[200]       //
	{0x0F12,0x0000, WORD_LEN, 0}, //TVAR_ash_pGA{0x[201]       //
	{0x0F12,0x0007, WORD_LEN, 0}, //TVAR_ash_pGA{0x[202]       //
	{0x0F12,0x0017, WORD_LEN, 0}, //TVAR_ash_pGA{0x[203]       //
	{0x0F12,0x002C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[204]       //
	{0x0F12,0x004A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[205]       //
	{0x0F12,0x007E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[206]       //
	{0x0F12,0x00BF, WORD_LEN, 0}, //TVAR_ash_pGA{0x[207]       //
	{0x0F12,0x0092, WORD_LEN, 0}, //TVAR_ash_pGA{0x[208]       //
	{0x0F12,0x005E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[209]       //
	{0x0F12,0x0035, WORD_LEN, 0}, //TVAR_ash_pGA{0x[210]       //
	{0x0F12,0x001B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[211]       //
	{0x0F12,0x000A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[212]       //
	{0x0F12,0x0001, WORD_LEN, 0}, //TVAR_ash_pGA{0x[213]       //
	{0x0F12,0x0001, WORD_LEN, 0}, //TVAR_ash_pGA{0x[214]       //
	{0x0F12,0x0008, WORD_LEN, 0}, //TVAR_ash_pGA{0x[215]       //
	{0x0F12,0x001A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[216]       //
	{0x0F12,0x0030, WORD_LEN, 0}, //TVAR_ash_pGA{0x[217]       //
	{0x0F12,0x004F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[218]       //
	{0x0F12,0x0083, WORD_LEN, 0}, //TVAR_ash_pGA{0x[219]       //
	{0x0F12,0x00C4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[220]       //
	{0x0F12,0x00A3, WORD_LEN, 0}, //TVAR_ash_pGA{0x[221]       //
	{0x0F12,0x006B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[222]       //
	{0x0F12,0x0041, WORD_LEN, 0}, //TVAR_ash_pGA{0x[223]       //
	{0x0F12,0x0026, WORD_LEN, 0}, //TVAR_ash_pGA{0x[224]       //
	{0x0F12,0x0014, WORD_LEN, 0}, //TVAR_ash_pGA{0x[225]       //
	{0x0F12,0x000B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[226]       //
	{0x0F12,0x000A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[227]       //
	{0x0F12,0x0013, WORD_LEN, 0}, //TVAR_ash_pGA{0x[228]       //
	{0x0F12,0x0025, WORD_LEN, 0}, //TVAR_ash_pGA{0x[229]       //
	{0x0F12,0x003D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[230]       //
	{0x0F12,0x005F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[231]       //
	{0x0F12,0x0095, WORD_LEN, 0}, //TVAR_ash_pGA{0x[232]       //
	{0x0F12,0x00D9, WORD_LEN, 0}, //TVAR_ash_pGA{0x[233]       //
	{0x0F12,0x00C4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[234]       //
	{0x0F12,0x0087, WORD_LEN, 0}, //TVAR_ash_pGA{0x[235]       //
	{0x0F12,0x0057, WORD_LEN, 0}, //TVAR_ash_pGA{0x[236]       //
	{0x0F12,0x003B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[237]       //
	{0x0F12,0x0029, WORD_LEN, 0}, //TVAR_ash_pGA{0x[238]       //
	{0x0F12,0x0021, WORD_LEN, 0}, //TVAR_ash_pGA{0x[239]       //
	{0x0F12,0x0020, WORD_LEN, 0}, //TVAR_ash_pGA{0x[240]       //
	{0x0F12,0x0029, WORD_LEN, 0}, //TVAR_ash_pGA{0x[241]       //
	{0x0F12,0x003B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[242]       //
	{0x0F12,0x0054, WORD_LEN, 0}, //TVAR_ash_pGA{0x[243]       //
	{0x0F12,0x007A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[244]       //
	{0x0F12,0x00B6, WORD_LEN, 0}, //TVAR_ash_pGA{0x[245]       //
	{0x0F12,0x00FF, WORD_LEN, 0}, //TVAR_ash_pGA{0x[246]       //
	{0x0F12,0x00F4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[247]       //
	{0x0F12,0x00B0, WORD_LEN, 0}, //TVAR_ash_pGA{0x[248]       //
	{0x0F12,0x007A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[249]       //
	{0x0F12,0x0059, WORD_LEN, 0}, //TVAR_ash_pGA{0x[250]       //
	{0x0F12,0x0048, WORD_LEN, 0}, //TVAR_ash_pGA{0x[251]       //
	{0x0F12,0x0040, WORD_LEN, 0}, //TVAR_ash_pGA{0x[252]       //
	{0x0F12,0x003F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[253]       //
	{0x0F12,0x0048, WORD_LEN, 0}, //TVAR_ash_pGA{0x[254]       //
	{0x0F12,0x005A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[255]       //
	{0x0F12,0x0075, WORD_LEN, 0}, //TVAR_ash_pGA{0x[256]       //
	{0x0F12,0x00A0, WORD_LEN, 0}, //TVAR_ash_pGA{0x[257]       //
	{0x0F12,0x00E7, WORD_LEN, 0}, //TVAR_ash_pGA{0x[258]       //
	{0x0F12,0x0135, WORD_LEN, 0}, //TVAR_ash_pGA{0x[259]       //
	{0x0F12,0x0137, WORD_LEN, 0}, //TVAR_ash_pGA{0x[260]       //
	{0x0F12,0x00EF, WORD_LEN, 0}, //TVAR_ash_pGA{0x[261]       //
	{0x0F12,0x00AF, WORD_LEN, 0}, //TVAR_ash_pGA{0x[262]       //
	{0x0F12,0x0088, WORD_LEN, 0}, //TVAR_ash_pGA{0x[263]       //
	{0x0F12,0x0072, WORD_LEN, 0}, //TVAR_ash_pGA{0x[264]       //
	{0x0F12,0x0068, WORD_LEN, 0}, //TVAR_ash_pGA{0x[265]       //
	{0x0F12,0x0067, WORD_LEN, 0}, //TVAR_ash_pGA{0x[266]       //
	{0x0F12,0x006F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[267]       //
	{0x0F12,0x0082, WORD_LEN, 0}, //TVAR_ash_pGA{0x[268]       //
	{0x0F12,0x00A4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[269]       //
	{0x0F12,0x00D8, WORD_LEN, 0}, //TVAR_ash_pGA{0x[270]       //
	{0x0F12,0x0129, WORD_LEN, 0}, //TVAR_ash_pGA{0x[271]       //
	{0x0F12,0x0161, WORD_LEN, 0}, //TVAR_ash_pGA{0x[272]       //
	{0x0F12,0x016E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[273]       //
	{0x0F12,0x013A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[274]       //
	{0x0F12,0x00F3, WORD_LEN, 0}, //TVAR_ash_pGA{0x[275]       //
	{0x0F12,0x00C4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[276]       //
	{0x0F12,0x00A7, WORD_LEN, 0}, //TVAR_ash_pGA{0x[277]       //
	{0x0F12,0x0099, WORD_LEN, 0}, //TVAR_ash_pGA{0x[278]       //
	{0x0F12,0x0096, WORD_LEN, 0}, //TVAR_ash_pGA{0x[279]       //
	{0x0F12,0x009E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[280]       //
	{0x0F12,0x00B4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[281]       //
	{0x0F12,0x00DF, WORD_LEN, 0}, //TVAR_ash_pGA{0x[282]       //
	{0x0F12,0x011F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[283]       //
	{0x0F12,0x0168, WORD_LEN, 0}, //TVAR_ash_pGA{0x[284]       //
	{0x0F12,0x0178, WORD_LEN, 0}, //TVAR_ash_pGA{0x[285]       //
	{0x0F12,0x0144, WORD_LEN, 0}, //TVAR_ash_pGA{0x[286]       //
	{0x0F12,0x010A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[287]       //
	{0x0F12,0x00CE, WORD_LEN, 0}, //TVAR_ash_pGA{0x[288]       //
	{0x0F12,0x00A8, WORD_LEN, 0}, //TVAR_ash_pGA{0x[289]       //
	{0x0F12,0x0091, WORD_LEN, 0}, //TVAR_ash_pGA{0x[290]       //
	{0x0F12,0x0084, WORD_LEN, 0}, //TVAR_ash_pGA{0x[291]       //
	{0x0F12,0x007F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[292]       //
	{0x0F12,0x0083, WORD_LEN, 0}, //TVAR_ash_pGA{0x[293]       //
	{0x0F12,0x0092, WORD_LEN, 0}, //TVAR_ash_pGA{0x[294]       //
	{0x0F12,0x00B6, WORD_LEN, 0}, //TVAR_ash_pGA{0x[295]       //
	{0x0F12,0x00EC, WORD_LEN, 0}, //TVAR_ash_pGA{0x[296]       //
	{0x0F12,0x0130, WORD_LEN, 0}, //TVAR_ash_pGA{0x[297]       //
	{0x0F12,0x014F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[298]       //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_ash_pGA{0x[299]       //
	{0x0F12,0x00C2, WORD_LEN, 0}, //TVAR_ash_pGA{0x[300]       //
	{0x0F12,0x008E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[301]       //
	{0x0F12,0x006E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[302]       //
	{0x0F12,0x005B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[303]       //
	{0x0F12,0x0050, WORD_LEN, 0}, //TVAR_ash_pGA{0x[304]       //
	{0x0F12,0x004D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[305]       //
	{0x0F12,0x0051, WORD_LEN, 0}, //TVAR_ash_pGA{0x[306]       //
	{0x0F12,0x005D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[307]       //
	{0x0F12,0x0075, WORD_LEN, 0}, //TVAR_ash_pGA{0x[308]       //
	{0x0F12,0x009F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[309]       //
	{0x0F12,0x00E1, WORD_LEN, 0}, //TVAR_ash_pGA{0x[310]       //
	{0x0F12,0x011D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[311]       //
	{0x0F12,0x00CB, WORD_LEN, 0}, //TVAR_ash_pGA{0x[312]       //
	{0x0F12,0x0090, WORD_LEN, 0}, //TVAR_ash_pGA{0x[313]       //
	{0x0F12,0x0062, WORD_LEN, 0}, //TVAR_ash_pGA{0x[314]       //
	{0x0F12,0x0046, WORD_LEN, 0}, //TVAR_ash_pGA{0x[315]       //
	{0x0F12,0x0035, WORD_LEN, 0}, //TVAR_ash_pGA{0x[316]       //
	{0x0F12,0x002A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[317]       //
	{0x0F12,0x0027, WORD_LEN, 0}, //TVAR_ash_pGA{0x[318]       //
	{0x0F12,0x002D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[319]       //
	{0x0F12,0x0039, WORD_LEN, 0}, //TVAR_ash_pGA{0x[320]       //
	{0x0F12,0x004F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[321]       //
	{0x0F12,0x0072, WORD_LEN, 0}, //TVAR_ash_pGA{0x[322]       //
	{0x0F12,0x00AF, WORD_LEN, 0}, //TVAR_ash_pGA{0x[323]       //
	{0x0F12,0x00F5, WORD_LEN, 0}, //TVAR_ash_pGA{0x[324]       //
	{0x0F12,0x00AE, WORD_LEN, 0}, //TVAR_ash_pGA{0x[325]       //
	{0x0F12,0x0075, WORD_LEN, 0}, //TVAR_ash_pGA{0x[326]       //
	{0x0F12,0x004A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[327]       //
	{0x0F12,0x002F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[328]       //
	{0x0F12,0x001C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[329]       //
	{0x0F12,0x0011, WORD_LEN, 0}, //TVAR_ash_pGA{0x[330]       //
	{0x0F12,0x000E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[331]       //
	{0x0F12,0x0014, WORD_LEN, 0}, //TVAR_ash_pGA{0x[332]       //
	{0x0F12,0x0024, WORD_LEN, 0}, //TVAR_ash_pGA{0x[333]       //
	{0x0F12,0x003A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[334]       //
	{0x0F12,0x005B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[335]       //
	{0x0F12,0x0093, WORD_LEN, 0}, //TVAR_ash_pGA{0x[336]       //
	{0x0F12,0x00D8, WORD_LEN, 0}, //TVAR_ash_pGA{0x[337]       //
	{0x0F12,0x00A2, WORD_LEN, 0}, //TVAR_ash_pGA{0x[338]       //
	{0x0F12,0x006A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[339]       //
	{0x0F12,0x0040, WORD_LEN, 0}, //TVAR_ash_pGA{0x[340]       //
	{0x0F12,0x0023, WORD_LEN, 0}, //TVAR_ash_pGA{0x[341]       //
	{0x0F12,0x000F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[342]       //
	{0x0F12,0x0003, WORD_LEN, 0}, //TVAR_ash_pGA{0x[343]       //
	{0x0F12,0x0001, WORD_LEN, 0}, //TVAR_ash_pGA{0x[344]       //
	{0x0F12,0x0008, WORD_LEN, 0}, //TVAR_ash_pGA{0x[345]       //
	{0x0F12,0x0019, WORD_LEN, 0}, //TVAR_ash_pGA{0x[346]       //
	{0x0F12,0x0030, WORD_LEN, 0}, //TVAR_ash_pGA{0x[347]       //
	{0x0F12,0x0050, WORD_LEN, 0}, //TVAR_ash_pGA{0x[348]       //
	{0x0F12,0x0086, WORD_LEN, 0}, //TVAR_ash_pGA{0x[349]       //
	{0x0F12,0x00C7, WORD_LEN, 0}, //TVAR_ash_pGA{0x[350]       //
	{0x0F12,0x00A6, WORD_LEN, 0}, //TVAR_ash_pGA{0x[351]       //
	{0x0F12,0x006D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[352]       //
	{0x0F12,0x0040, WORD_LEN, 0}, //TVAR_ash_pGA{0x[353]       //
	{0x0F12,0x0023, WORD_LEN, 0}, //TVAR_ash_pGA{0x[354]       //
	{0x0F12,0x000F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[355]       //
	{0x0F12,0x0002, WORD_LEN, 0}, //TVAR_ash_pGA{0x[356]       //
	{0x0F12,0x0000, WORD_LEN, 0}, //TVAR_ash_pGA{0x[357]       //
	{0x0F12,0x0007, WORD_LEN, 0}, //TVAR_ash_pGA{0x[358]       //
	{0x0F12,0x0018, WORD_LEN, 0}, //TVAR_ash_pGA{0x[359]       //
	{0x0F12,0x002F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[360]       //
	{0x0F12,0x0050, WORD_LEN, 0}, //TVAR_ash_pGA{0x[361]       //
	{0x0F12,0x0085, WORD_LEN, 0}, //TVAR_ash_pGA{0x[362]       //
	{0x0F12,0x00C5, WORD_LEN, 0}, //TVAR_ash_pGA{0x[363]       //
	{0x0F12,0x00B9, WORD_LEN, 0}, //TVAR_ash_pGA{0x[364]       //
	{0x0F12,0x007B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[365]       //
	{0x0F12,0x004C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[366]       //
	{0x0F12,0x002D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[367]       //
	{0x0F12,0x0018, WORD_LEN, 0}, //TVAR_ash_pGA{0x[368]       //
	{0x0F12,0x000B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[369]       //
	{0x0F12,0x0008, WORD_LEN, 0}, //TVAR_ash_pGA{0x[370]       //
	{0x0F12,0x000F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[371]       //
	{0x0F12,0x0020, WORD_LEN, 0}, //TVAR_ash_pGA{0x[372]       //
	{0x0F12,0x0038, WORD_LEN, 0}, //TVAR_ash_pGA{0x[373]       //
	{0x0F12,0x0059, WORD_LEN, 0}, //TVAR_ash_pGA{0x[374]       //
	{0x0F12,0x0090, WORD_LEN, 0}, //TVAR_ash_pGA{0x[375]       //
	{0x0F12,0x00D3, WORD_LEN, 0}, //TVAR_ash_pGA{0x[376]       //
	{0x0F12,0x00DA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[377]       //
	{0x0F12,0x0097, WORD_LEN, 0}, //TVAR_ash_pGA{0x[378]       //
	{0x0F12,0x0063, WORD_LEN, 0}, //TVAR_ash_pGA{0x[379]       //
	{0x0F12,0x0042, WORD_LEN, 0}, //TVAR_ash_pGA{0x[380]       //
	{0x0F12,0x002C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[381]       //
	{0x0F12,0x0020, WORD_LEN, 0}, //TVAR_ash_pGA{0x[382]       //
	{0x0F12,0x001C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[383]       //
	{0x0F12,0x0023, WORD_LEN, 0}, //TVAR_ash_pGA{0x[384]       //
	{0x0F12,0x0032, WORD_LEN, 0}, //TVAR_ash_pGA{0x[385]       //
	{0x0F12,0x0049, WORD_LEN, 0}, //TVAR_ash_pGA{0x[386]       //
	{0x0F12,0x006E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[387]       //
	{0x0F12,0x00AA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[388]       //
	{0x0F12,0x00F0, WORD_LEN, 0}, //TVAR_ash_pGA{0x[389]       //
	{0x0F12,0x010C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[390]       //
	{0x0F12,0x00C1, WORD_LEN, 0}, //TVAR_ash_pGA{0x[391]       //
	{0x0F12,0x0086, WORD_LEN, 0}, //TVAR_ash_pGA{0x[392]       //
	{0x0F12,0x0061, WORD_LEN, 0}, //TVAR_ash_pGA{0x[393]       //
	{0x0F12,0x004A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[394]       //
	{0x0F12,0x003E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[395]       //
	{0x0F12,0x003A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[396]       //
	{0x0F12,0x003F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[397]       //
	{0x0F12,0x004E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[398]       //
	{0x0F12,0x0067, WORD_LEN, 0}, //TVAR_ash_pGA{0x[399]       //
	{0x0F12,0x0090, WORD_LEN, 0}, //TVAR_ash_pGA{0x[400]       //
	{0x0F12,0x00D5, WORD_LEN, 0}, //TVAR_ash_pGA{0x[401]       //
	{0x0F12,0x011D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[402]       //
	{0x0F12,0x0152, WORD_LEN, 0}, //TVAR_ash_pGA{0x[403]       //
	{0x0F12,0x0102, WORD_LEN, 0}, //TVAR_ash_pGA{0x[404]       //
	{0x0F12,0x00BC, WORD_LEN, 0}, //TVAR_ash_pGA{0x[405]       //
	{0x0F12,0x008F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[406]       //
	{0x0F12,0x0075, WORD_LEN, 0}, //TVAR_ash_pGA{0x[407]       //
	{0x0F12,0x0066, WORD_LEN, 0}, //TVAR_ash_pGA{0x[408]       //
	{0x0F12,0x0061, WORD_LEN, 0}, //TVAR_ash_pGA{0x[409]       //
	{0x0F12,0x0066, WORD_LEN, 0}, //TVAR_ash_pGA{0x[410]       //
	{0x0F12,0x0076, WORD_LEN, 0}, //TVAR_ash_pGA{0x[411]       //
	{0x0F12,0x0093, WORD_LEN, 0}, //TVAR_ash_pGA{0x[412]       //
	{0x0F12,0x00C6, WORD_LEN, 0}, //TVAR_ash_pGA{0x[413]       //
	{0x0F12,0x0110, WORD_LEN, 0}, //TVAR_ash_pGA{0x[414]       //
	{0x0F12,0x0146, WORD_LEN, 0}, //TVAR_ash_pGA{0x[415]       //
	{0x0F12,0x0188, WORD_LEN, 0}, //TVAR_ash_pGA{0x[416]       //
	{0x0F12,0x014E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[417]       //
	{0x0F12,0x0102, WORD_LEN, 0}, //TVAR_ash_pGA{0x[418]       //
	{0x0F12,0x00CD, WORD_LEN, 0}, //TVAR_ash_pGA{0x[419]       //
	{0x0F12,0x00AD, WORD_LEN, 0}, //TVAR_ash_pGA{0x[420]       //
	{0x0F12,0x0098, WORD_LEN, 0}, //TVAR_ash_pGA{0x[421]       //
	{0x0F12,0x0091, WORD_LEN, 0}, //TVAR_ash_pGA{0x[422]       //
	{0x0F12,0x0095, WORD_LEN, 0}, //TVAR_ash_pGA{0x[423]       //
	{0x0F12,0x00A8, WORD_LEN, 0}, //TVAR_ash_pGA{0x[424]       //
	{0x0F12,0x00CD, WORD_LEN, 0}, //TVAR_ash_pGA{0x[425]       //
	{0x0F12,0x0107, WORD_LEN, 0}, //TVAR_ash_pGA{0x[426]       //
	{0x0F12,0x014A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[427]       //
	{0x0F12,0x015B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[428]       //
	{0x0F12,0x00DC, WORD_LEN, 0}, //TVAR_ash_pGA{0x[429]       //
	{0x0F12,0x00BA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[430]       //
	{0x0F12,0x0093, WORD_LEN, 0}, //TVAR_ash_pGA{0x[431]       //
	{0x0F12,0x007A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[432]       //
	{0x0F12,0x006B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[433]       //
	{0x0F12,0x0064, WORD_LEN, 0}, //TVAR_ash_pGA{0x[434]       //
	{0x0F12,0x0062, WORD_LEN, 0}, //TVAR_ash_pGA{0x[435]       //
	{0x0F12,0x0067, WORD_LEN, 0}, //TVAR_ash_pGA{0x[436]       //
	{0x0F12,0x0074, WORD_LEN, 0}, //TVAR_ash_pGA{0x[437]       //
	{0x0F12,0x0090, WORD_LEN, 0}, //TVAR_ash_pGA{0x[438]       //
	{0x0F12,0x00BC, WORD_LEN, 0}, //TVAR_ash_pGA{0x[439]       //
	{0x0F12,0x00F3, WORD_LEN, 0}, //TVAR_ash_pGA{0x[440]       //
	{0x0F12,0x0104, WORD_LEN, 0}, //TVAR_ash_pGA{0x[441]       //
	{0x0F12,0x00AA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[442]       //
	{0x0F12,0x0081, WORD_LEN, 0}, //TVAR_ash_pGA{0x[443]       //
	{0x0F12,0x005E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[444]       //
	{0x0F12,0x004B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[445]       //
	{0x0F12,0x0041, WORD_LEN, 0}, //TVAR_ash_pGA{0x[446]       //
	{0x0F12,0x003C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[447]       //
	{0x0F12,0x003C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[448]       //
	{0x0F12,0x003F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[449]       //
	{0x0F12,0x0048, WORD_LEN, 0}, //TVAR_ash_pGA{0x[450]       //
	{0x0F12,0x005A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[451]       //
	{0x0F12,0x007A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[452]       //
	{0x0F12,0x00B0, WORD_LEN, 0}, //TVAR_ash_pGA{0x[453]       //
	{0x0F12,0x00D7, WORD_LEN, 0}, //TVAR_ash_pGA{0x[454]       //
	{0x0F12,0x007E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[455]       //
	{0x0F12,0x0058, WORD_LEN, 0}, //TVAR_ash_pGA{0x[456]       //
	{0x0F12,0x0039, WORD_LEN, 0}, //TVAR_ash_pGA{0x[457]       //
	{0x0F12,0x002A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[458]       //
	{0x0F12,0x0022, WORD_LEN, 0}, //TVAR_ash_pGA{0x[459]       //
	{0x0F12,0x001E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[460]       //
	{0x0F12,0x001E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[461]       //
	{0x0F12,0x0022, WORD_LEN, 0}, //TVAR_ash_pGA{0x[462]       //
	{0x0F12,0x002A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[463]       //
	{0x0F12,0x0038, WORD_LEN, 0}, //TVAR_ash_pGA{0x[464]       //
	{0x0F12,0x0052, WORD_LEN, 0}, //TVAR_ash_pGA{0x[465]       //
	{0x0F12,0x0081, WORD_LEN, 0}, //TVAR_ash_pGA{0x[466]       //
	{0x0F12,0x00B4, WORD_LEN, 0}, //TVAR_ash_pGA{0x[467]       //
	{0x0F12,0x0065, WORD_LEN, 0}, //TVAR_ash_pGA{0x[468]       //
	{0x0F12,0x0041, WORD_LEN, 0}, //TVAR_ash_pGA{0x[469]       //
	{0x0F12,0x0025, WORD_LEN, 0}, //TVAR_ash_pGA{0x[470]       //
	{0x0F12,0x0017, WORD_LEN, 0}, //TVAR_ash_pGA{0x[471]       //
	{0x0F12,0x000F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[472]       //
	{0x0F12,0x000A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[473]       //
	{0x0F12,0x000A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[474]       //
	{0x0F12,0x000F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[475]       //
	{0x0F12,0x001A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[476]       //
	{0x0F12,0x0026, WORD_LEN, 0}, //TVAR_ash_pGA{0x[477]       //
	{0x0F12,0x003D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[478]       //
	{0x0F12,0x0068, WORD_LEN, 0}, //TVAR_ash_pGA{0x[479]       //
	{0x0F12,0x0099, WORD_LEN, 0}, //TVAR_ash_pGA{0x[480]       //
	{0x0F12,0x005D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[481]       //
	{0x0F12,0x0038, WORD_LEN, 0}, //TVAR_ash_pGA{0x[482]       //
	{0x0F12,0x001D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[483]       //
	{0x0F12,0x000F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[484]       //
	{0x0F12,0x0006, WORD_LEN, 0}, //TVAR_ash_pGA{0x[485]       //
	{0x0F12,0x0000, WORD_LEN, 0}, //TVAR_ash_pGA{0x[486]       //
	{0x0F12,0x0000, WORD_LEN, 0}, //TVAR_ash_pGA{0x[487]       //
	{0x0F12,0x0006, WORD_LEN, 0}, //TVAR_ash_pGA{0x[488]       //
	{0x0F12,0x0010, WORD_LEN, 0}, //TVAR_ash_pGA{0x[489]       //
	{0x0F12,0x001D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[490]       //
	{0x0F12,0x0031, WORD_LEN, 0}, //TVAR_ash_pGA{0x[491]       //
	{0x0F12,0x005A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[492]       //
	{0x0F12,0x0089, WORD_LEN, 0}, //TVAR_ash_pGA{0x[493]       //
	{0x0F12,0x005F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[494]       //
	{0x0F12,0x003A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[495]       //
	{0x0F12,0x001F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[496]       //
	{0x0F12,0x000F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[497]       //
	{0x0F12,0x0006, WORD_LEN, 0}, //TVAR_ash_pGA{0x[498]       //
	{0x0F12,0x0000, WORD_LEN, 0}, //TVAR_ash_pGA{0x[499]       //
	{0x0F12,0x0000, WORD_LEN, 0}, //TVAR_ash_pGA{0x[500]       //
	{0x0F12,0x0005, WORD_LEN, 0}, //TVAR_ash_pGA{0x[501]       //
	{0x0F12,0x000F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[502]       //
	{0x0F12,0x001B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[503]       //
	{0x0F12,0x002F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[504]       //
	{0x0F12,0x0056, WORD_LEN, 0}, //TVAR_ash_pGA{0x[505]       //
	{0x0F12,0x0084, WORD_LEN, 0}, //TVAR_ash_pGA{0x[506]       //
	{0x0F12,0x006F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[507]       //
	{0x0F12,0x0047, WORD_LEN, 0}, //TVAR_ash_pGA{0x[508]       //
	{0x0F12,0x0028, WORD_LEN, 0}, //TVAR_ash_pGA{0x[509]       //
	{0x0F12,0x0018, WORD_LEN, 0}, //TVAR_ash_pGA{0x[510]       //
	{0x0F12,0x000E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[511]       //
	{0x0F12,0x0008, WORD_LEN, 0}, //TVAR_ash_pGA{0x[512]       //
	{0x0F12,0x0007, WORD_LEN, 0}, //TVAR_ash_pGA{0x[513]       //
	{0x0F12,0x000B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[514]       //
	{0x0F12,0x0014, WORD_LEN, 0}, //TVAR_ash_pGA{0x[515]       //
	{0x0F12,0x0021, WORD_LEN, 0}, //TVAR_ash_pGA{0x[516]       //
	{0x0F12,0x0035, WORD_LEN, 0}, //TVAR_ash_pGA{0x[517]       //
	{0x0F12,0x005E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[518]       //
	{0x0F12,0x008D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[519]       //
	{0x0F12,0x008C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[520]       //
	{0x0F12,0x005F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[521]       //
	{0x0F12,0x003C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[522]       //
	{0x0F12,0x0029, WORD_LEN, 0}, //TVAR_ash_pGA{0x[523]       //
	{0x0F12,0x001E, WORD_LEN, 0}, //TVAR_ash_pGA{0x[524]       //
	{0x0F12,0x0019, WORD_LEN, 0}, //TVAR_ash_pGA{0x[525]       //
	{0x0F12,0x0017, WORD_LEN, 0}, //TVAR_ash_pGA{0x[526]       //
	{0x0F12,0x001A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[527]       //
	{0x0F12,0x0023, WORD_LEN, 0}, //TVAR_ash_pGA{0x[528]       //
	{0x0F12,0x002F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[529]       //
	{0x0F12,0x0047, WORD_LEN, 0}, //TVAR_ash_pGA{0x[530]       //
	{0x0F12,0x0073, WORD_LEN, 0}, //TVAR_ash_pGA{0x[531]       //
	{0x0F12,0x00A5, WORD_LEN, 0}, //TVAR_ash_pGA{0x[532]       //
	{0x0F12,0x00B3, WORD_LEN, 0}, //TVAR_ash_pGA{0x[533]       //
	{0x0F12,0x0083, WORD_LEN, 0}, //TVAR_ash_pGA{0x[534]       //
	{0x0F12,0x005A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[535]       //
	{0x0F12,0x0043, WORD_LEN, 0}, //TVAR_ash_pGA{0x[536]       //
	{0x0F12,0x0037, WORD_LEN, 0}, //TVAR_ash_pGA{0x[537]       //
	{0x0F12,0x0031, WORD_LEN, 0}, //TVAR_ash_pGA{0x[538]       //
	{0x0F12,0x002F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[539]       //
	{0x0F12,0x0031, WORD_LEN, 0}, //TVAR_ash_pGA{0x[540]       //
	{0x0F12,0x003A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[541]       //
	{0x0F12,0x0048, WORD_LEN, 0}, //TVAR_ash_pGA{0x[542]       //
	{0x0F12,0x0064, WORD_LEN, 0}, //TVAR_ash_pGA{0x[543]       //
	{0x0F12,0x0098, WORD_LEN, 0}, //TVAR_ash_pGA{0x[544]       //
	{0x0F12,0x00CA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[545]       //
	{0x0F12,0x00ED, WORD_LEN, 0}, //TVAR_ash_pGA{0x[546]       //
	{0x0F12,0x00BA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[547]       //
	{0x0F12,0x008A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[548]       //
	{0x0F12,0x006C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[549]       //
	{0x0F12,0x005C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[550]       //
	{0x0F12,0x0053, WORD_LEN, 0}, //TVAR_ash_pGA{0x[551]       //
	{0x0F12,0x0050, WORD_LEN, 0}, //TVAR_ash_pGA{0x[552]       //
	{0x0F12,0x0052, WORD_LEN, 0}, //TVAR_ash_pGA{0x[553]       //
	{0x0F12,0x005B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[554]       //
	{0x0F12,0x0070, WORD_LEN, 0}, //TVAR_ash_pGA{0x[555]       //
	{0x0F12,0x0094, WORD_LEN, 0}, //TVAR_ash_pGA{0x[556]       //
	{0x0F12,0x00CA, WORD_LEN, 0}, //TVAR_ash_pGA{0x[557]       //
	{0x0F12,0x00EF, WORD_LEN, 0}, //TVAR_ash_pGA{0x[558]       //
	{0x0F12,0x011D, WORD_LEN, 0}, //TVAR_ash_pGA{0x[559]       //
	{0x0F12,0x00FB, WORD_LEN, 0}, //TVAR_ash_pGA{0x[560]       //
	{0x0F12,0x00C6, WORD_LEN, 0}, //TVAR_ash_pGA{0x[561]       //
	{0x0F12,0x00A3, WORD_LEN, 0}, //TVAR_ash_pGA{0x[562]       //
	{0x0F12,0x008C, WORD_LEN, 0}, //TVAR_ash_pGA{0x[563]       //
	{0x0F12,0x007F, WORD_LEN, 0}, //TVAR_ash_pGA{0x[564]       //
	{0x0F12,0x0079, WORD_LEN, 0}, //TVAR_ash_pGA{0x[565]       //
	{0x0F12,0x007B, WORD_LEN, 0}, //TVAR_ash_pGA{0x[566]       //
	{0x0F12,0x0088, WORD_LEN, 0}, //TVAR_ash_pGA{0x[567]       //
	{0x0F12,0x00A2, WORD_LEN, 0}, //TVAR_ash_pGA{0x[568]       //
	{0x0F12,0x00CC, WORD_LEN, 0}, //TVAR_ash_pGA{0x[569]       //
	{0x0F12,0x00FB, WORD_LEN, 0}, //TVAR_ash_pGA{0x[570]       //
	{0x0F12,0x010A, WORD_LEN, 0}, //TVAR_ash_pGA{0x[571]       //
	{0x002A,0x0704, WORD_LEN, 0}, // 5. shading Alpha //
	{0x0F12,0x00ED, WORD_LEN, 0}, // param_start	TVAR_ash_AwbAshCord //
	{0x0F12,0x0124, WORD_LEN, 0},
	{0x0F12,0x012B, WORD_LEN, 0},
	{0x0F12,0x014A, WORD_LEN, 0},
	{0x0F12,0x0190, WORD_LEN, 0},
	{0x0F12,0x01B2, WORD_LEN, 0},
	{0x0F12,0x01C4, WORD_LEN, 0}, // param_end	TVAR_ash_AwbAshCord //
	{0x0F12,0x0100, WORD_LEN, 0}, // param_{0xtart	TVAR_a{0xh_GA{0xAlpha // //TVAR_a{0xh_GA{0xAlpha[0]    //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[1]    //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[2]    //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[3]    //        
	{0x0F12,0x0100, WORD_LEN, 0}, //200 TVAR_a{0xh_GA{0xAlpha[4] 0507   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[5]    //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[6]    //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[7]    //        
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[8]    //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[9]    //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[10]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[11]   //        
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[12]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[13]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[14]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[15]   //        
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[16]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[17]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[18]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[19]   //        
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[20]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[21]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[22]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[23]   //        
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[24]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[25]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xAlpha[26]   //
	{0x0F12,0x0100, WORD_LEN, 0}, //	param_end	TVAR_a{0xh_GA{0xAlpha // //TVAR_a{0xh_GA{0xAlpha[27]   //      
	{0x0F12,0x0100, WORD_LEN, 0}, //	param_{0xtart	TVAR_a{0xh_GA{0xOutdoorAlpha // //00FB	  TVAR_a{0xh_GA{0xOutdoorAlpha[0] //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xOutdoorAlpha[1]        //
	{0x0F12,0x0100, WORD_LEN, 0}, //TVAR_a{0xh_GA{0xOutdoorAlpha[2]        //
	{0x0F12,0x0100, WORD_LEN, 0}, //	param_end	TVAR_a{0xh_GA{0xOutdoorAlpha // //TVAR_a{0xh_GA{0xOutdoorAlpha[3]        //  
	{0x002A,0x075A, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0280, WORD_LEN, 0},
	{0x0F12,0x0200, WORD_LEN, 0},
	{0x0F12,0x000E, WORD_LEN, 0},
	{0x0F12,0x000F, WORD_LEN, 0},
	{0x002A,0x04C8, WORD_LEN, 0}, // 6. Gamma // // param_{0xtart	{0xARR_u{0xGammaLutRGBIndoor //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000   //
	{0x0F12,0x0004, WORD_LEN, 0}, //0007   //
	{0x0F12,0x0008, WORD_LEN, 0}, //000D   //
	{0x0F12,0x001a, WORD_LEN, 0}, //0013   //
	{0x0F12,0x0068, WORD_LEN, 0}, //0079   //
	{0x0F12,0x00fe, WORD_LEN, 0}, //00FE   //
	{0x0F12,0x0159, WORD_LEN, 0}, //0159   //
	{0x0F12,0x01A1, WORD_LEN, 0}, //01A1   //
	{0x0F12,0x0210, WORD_LEN, 0}, //0210   //
	{0x0F12,0x0263, WORD_LEN, 0}, //0263   //
	{0x0F12,0x02D5, WORD_LEN, 0}, //02D5   //
	{0x0F12,0x0330, WORD_LEN, 0}, //0330   //
	{0x0F12,0x0377, WORD_LEN, 0}, //0377   //
	{0x0F12,0x03BE, WORD_LEN, 0}, //03BE   //
	{0x0F12,0x03F0, WORD_LEN, 0}, //03F0   //
	{0x0F12,0x0400, WORD_LEN, 0}, //0400   //           
	{0x0F12,0x0000, WORD_LEN, 0}, //0000   //
	{0x0F12,0x0004, WORD_LEN, 0}, //0007   //
	{0x0F12,0x0008, WORD_LEN, 0}, //000D   //
	{0x0F12,0x001a, WORD_LEN, 0}, //0013   //
	{0x0F12,0x0068, WORD_LEN, 0}, //0079   //
	{0x0F12,0x00fe, WORD_LEN, 0}, //00FE   //
	{0x0F12,0x0159, WORD_LEN, 0}, //0159   //
	{0x0F12,0x01A1, WORD_LEN, 0}, //01A1   //
	{0x0F12,0x0210, WORD_LEN, 0}, //0210   //
	{0x0F12,0x0263, WORD_LEN, 0}, //0263   //
	{0x0F12,0x02D5, WORD_LEN, 0}, //02D5   //
	{0x0F12,0x0330, WORD_LEN, 0}, //0330   //
	{0x0F12,0x0377, WORD_LEN, 0}, //0377   //
	{0x0F12,0x03BE, WORD_LEN, 0}, //03BE   //
	{0x0F12,0x03F0, WORD_LEN, 0}, //03F0   //
	{0x0F12,0x0400, WORD_LEN, 0}, //0400   //           
	{0x0F12,0x0000, WORD_LEN, 0}, //0000   //
	{0x0F12,0x0004, WORD_LEN, 0}, //0007   //
	{0x0F12,0x0008, WORD_LEN, 0}, //000D   //
	{0x0F12,0x001a, WORD_LEN, 0}, //0013   //
	{0x0F12,0x0068, WORD_LEN, 0}, //0079   //
	{0x0F12,0x00fe, WORD_LEN, 0}, //00FE   //
	{0x0F12,0x0159, WORD_LEN, 0}, //0159   //
	{0x0F12,0x01A1, WORD_LEN, 0}, //01A1   //
	{0x0F12,0x0210, WORD_LEN, 0}, //0210   //
	{0x0F12,0x0263, WORD_LEN, 0}, //0263   //
	{0x0F12,0x02D5, WORD_LEN, 0}, //02D5   //
	{0x0F12,0x0330, WORD_LEN, 0}, //0330   //
	{0x0F12,0x0377, WORD_LEN, 0}, //0377   //
	{0x0F12,0x03BE, WORD_LEN, 0}, //03BE   //
	{0x0F12,0x03F0, WORD_LEN, 0}, //03F0   //
	{0x0F12,0x0400, WORD_LEN, 0}, //	param_end	{0xARR_u{0xGammaLutRGBIndoor // //0400   //
	{0x002A,0x0C50, WORD_LEN, 0}, // 7. AWB // // param_{0xtart	awbb_IndoorGrZone{0x_m_BGrid //
	{0x0F12,0x03B8, WORD_LEN, 0}, //03B8  //
	{0x0F12,0x03C8, WORD_LEN, 0}, //03C8  //
	{0x0F12,0x0384, WORD_LEN, 0}, //0384  //
	{0x0F12,0x03D0, WORD_LEN, 0}, //03D0  //
	{0x0F12,0x035E, WORD_LEN, 0}, //035E  //
	{0x0F12,0x03CC, WORD_LEN, 0}, //03CC  //
	{0x0F12,0x033E, WORD_LEN, 0}, //033E  //
	{0x0F12,0x03B2, WORD_LEN, 0}, //03B2  //
	{0x0F12,0x0322, WORD_LEN, 0}, //0322  //
	{0x0F12,0x0396, WORD_LEN, 0}, //0396  //
	{0x0F12,0x030C, WORD_LEN, 0}, //030C  //
	{0x0F12,0x0380, WORD_LEN, 0}, //0380  //
	{0x0F12,0x02F8, WORD_LEN, 0}, //02F8  //
	{0x0F12,0x0368, WORD_LEN, 0}, //0368  //
	{0x0F12,0x02DC, WORD_LEN, 0}, //02DC  //
	{0x0F12,0x0352, WORD_LEN, 0}, //034A  //
	{0x0F12,0x02C2, WORD_LEN, 0}, //02C2  //
	{0x0F12,0x033C, WORD_LEN, 0}, //0336  //
	{0x0F12,0x02AE, WORD_LEN, 0}, //02AE  //
	{0x0F12,0x032A, WORD_LEN, 0}, //031E  //
	{0x0F12,0x029A, WORD_LEN, 0}, //029A  //
	{0x0F12,0x031C, WORD_LEN, 0}, //0306  //
	{0x0F12,0x028C, WORD_LEN, 0}, //028C  //
	{0x0F12,0x030A, WORD_LEN, 0}, //02F4  //
	{0x0F12,0x027C, WORD_LEN, 0}, //027C  //
	{0x0F12,0x02FC, WORD_LEN, 0}, //02DC  //
	{0x0F12,0x0264, WORD_LEN, 0}, //0264  //
	{0x0F12,0x02EC, WORD_LEN, 0}, //02C2  //
	{0x0F12,0x0252, WORD_LEN, 0}, //0252  //
	{0x0F12,0x02DE, WORD_LEN, 0}, //02AE  //
	{0x0F12,0x0246, WORD_LEN, 0}, //0246  //
	{0x0F12,0x02CC, WORD_LEN, 0}, //029C  //
	{0x0F12,0x023C, WORD_LEN, 0}, //023C  //
	{0x0F12,0x02C2, WORD_LEN, 0}, //028A  //
	{0x0F12,0x022E, WORD_LEN, 0}, //022E  //
	{0x0F12,0x02B4, WORD_LEN, 0}, //027E  //
	{0x0F12,0x0222, WORD_LEN, 0}, //0222  //
	{0x0F12,0x02A8, WORD_LEN, 0}, //0272  //
	{0x0F12,0x0212, WORD_LEN, 0}, //0212  //
	{0x0F12,0x029C, WORD_LEN, 0}, //0266  //
	{0x0F12,0x0202, WORD_LEN, 0}, //0202  //
	{0x0F12,0x0292, WORD_LEN, 0}, //025A  //
	{0x0F12,0x01FA, WORD_LEN, 0}, //01FA  //
	{0x0F12,0x0288, WORD_LEN, 0}, //0250  //
	{0x0F12,0x01EC, WORD_LEN, 0}, //01EC  //
	{0x0F12,0x027E, WORD_LEN, 0}, //0246  //
	{0x0F12,0x01E6, WORD_LEN, 0}, //01E6  //
	{0x0F12,0x0272, WORD_LEN, 0}, //023C  //
	{0x0F12,0x01DC, WORD_LEN, 0}, //01DC  //
	{0x0F12,0x0264, WORD_LEN, 0}, //0234  //
	{0x0F12,0x01D4, WORD_LEN, 0}, //01D4  //
	{0x0F12,0x0256, WORD_LEN, 0}, //0228  //
	{0x0F12,0x01CE, WORD_LEN, 0}, //01CE  //
	{0x0F12,0x0248, WORD_LEN, 0}, //0220  //
	{0x0F12,0x01C6, WORD_LEN, 0}, //01C6  //
	{0x0F12,0x023E, WORD_LEN, 0}, //0216  //
	{0x0F12,0x01C0, WORD_LEN, 0}, //01C0  //
	{0x0F12,0x022E, WORD_LEN, 0}, //020C  //
	{0x0F12,0x01BE, WORD_LEN, 0}, //01BE  //
	{0x0F12,0x0222, WORD_LEN, 0}, //01FC  //
	{0x0F12,0x01C4, WORD_LEN, 0}, //01C4  //
	{0x0F12,0x020E, WORD_LEN, 0}, //01EE  //
	{0x0F12,0x01D0, WORD_LEN, 0}, //01D0  //
	{0x0F12,0x01E0, WORD_LEN, 0}, //01E0  //
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
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x0F12,0x0004, WORD_LEN, 0}, // param_end	awbb_IndoorGrZone{0x_m_BGrid ////awbb_IndoorGrZone{0x_m_Grid{0xtep//
	{0x0F12,0x0000, WORD_LEN, 0}, 
	{0x002A,0x0CF8, WORD_LEN, 0}, 
	{0x0F12,0x010F, WORD_LEN, 0}, //awbb_IndoorGrZone{0x_m_Boff{0x//
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x002A,0x0D84, WORD_LEN, 0}, // param_{0xtart	awbb_LowBrGrZone{0x_m_BGrid //
	{0x0F12,0x0406, WORD_LEN, 0},
	{0x0F12,0x0467, WORD_LEN, 0},
	{0x0F12,0x0371, WORD_LEN, 0},
	{0x0F12,0x04B0, WORD_LEN, 0},
	{0x0F12,0x02E5, WORD_LEN, 0},
	{0x0F12,0x0481, WORD_LEN, 0},
	{0x0F12,0x0298, WORD_LEN, 0},
	{0x0F12,0x042E, WORD_LEN, 0},
	{0x0F12,0x0260, WORD_LEN, 0},
	{0x0F12,0x03DE, WORD_LEN, 0},
	{0x0F12,0x022F, WORD_LEN, 0},
	{0x0F12,0x0391, WORD_LEN, 0},
	{0x0F12,0x0201, WORD_LEN, 0},
	{0x0F12,0x034D, WORD_LEN, 0},
	{0x0F12,0x01DA, WORD_LEN, 0},
	{0x0F12,0x0310, WORD_LEN, 0},
	{0x0F12,0x01B3, WORD_LEN, 0},
	{0x0F12,0x02D4, WORD_LEN, 0},
	{0x0F12,0x018F, WORD_LEN, 0},
	{0x0F12,0x0297, WORD_LEN, 0},
	{0x0F12,0x0181, WORD_LEN, 0},
	{0x0F12,0x0271, WORD_LEN, 0},
	{0x0F12,0x0181, WORD_LEN, 0},
	{0x0F12,0x022A, WORD_LEN, 0},
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
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0}, // param_end	awbb_LowBrGrZone{0x_m_BGrid //       
	{0x0F12,0x0006, WORD_LEN, 0}, //awbb_LowBrGrZone{0x_m_Grid{0xtep//
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x002A,0x0DF0, WORD_LEN, 0},
	{0x0F12,0x0081, WORD_LEN, 0}, //awbb_LowBrGrZone{0x_m_Boff{0x//
	{0x0F12,0x0000, WORD_LEN, 0}, // param_{0xtart	awbb_OutdoorGrZone{0x_m_BGrid //       
	{0x002A,0x0D08, WORD_LEN, 0},
	{0x0F12,0x0258, WORD_LEN, 0}, //250,	awbb_OutdoorGrZone{0x_m_BGrid[0]      //
	{0x0F12,0x0274, WORD_LEN, 0}, //260,	awbb_OutdoorGrZone{0x_m_BGrid[1]      //
	{0x0F12,0x0240, WORD_LEN, 0}, //245,	awbb_OutdoorGrZone{0x_m_BGrid[2]      //
	{0x0F12,0x027E, WORD_LEN, 0}, //26B,	awbb_OutdoorGrZone{0x_m_BGrid[3]      //
	{0x0F12,0x0234, WORD_LEN, 0}, //23A,	awbb_OutdoorGrZone{0x_m_BGrid[4]      //
	{0x0F12,0x0280, WORD_LEN, 0}, //26D,	awbb_OutdoorGrZone{0x_m_BGrid[5]      //
	{0x0F12,0x022A, WORD_LEN, 0}, //230,	awbb_OutdoorGrZone{0x_m_BGrid[6]      //
	{0x0F12,0x0282, WORD_LEN, 0}, //26F,	awbb_OutdoorGrZone{0x_m_BGrid[7]      //
	{0x0F12,0x021E, WORD_LEN, 0}, //226,	awbb_OutdoorGrZone{0x_m_BGrid[8]      //
	{0x0F12,0x0282, WORD_LEN, 0}, //26F,	awbb_OutdoorGrZone{0x_m_BGrid[9]      //
	{0x0F12,0x0214, WORD_LEN, 0}, //221,	awbb_OutdoorGrZone{0x_m_BGrid[10]     //
	{0x0F12,0x027E, WORD_LEN, 0}, //26A,	awbb_OutdoorGrZone{0x_m_BGrid[11]     //
	{0x0F12,0x020E, WORD_LEN, 0}, //21D,	awbb_OutdoorGrZone{0x_m_BGrid[12]     //
	{0x0F12,0x0278, WORD_LEN, 0}, //264,	awbb_OutdoorGrZone{0x_m_BGrid[13]     //
	{0x0F12,0x020A, WORD_LEN, 0}, //217,	awbb_OutdoorGrZone{0x_m_BGrid[14]     //
	{0x0F12,0x0274, WORD_LEN, 0}, //260,	awbb_OutdoorGrZone{0x_m_BGrid[15]     //
	{0x0F12,0x0204, WORD_LEN, 0}, //210,	awbb_OutdoorGrZone{0x_m_BGrid[16]     //
	{0x0F12,0x026C, WORD_LEN, 0}, //259,	awbb_OutdoorGrZone{0x_m_BGrid[17]     //
	{0x0F12,0x0200, WORD_LEN, 0}, //20A,	awbb_OutdoorGrZone{0x_m_BGrid[18]     //
	{0x0F12,0x0264, WORD_LEN, 0}, //251,	awbb_OutdoorGrZone{0x_m_BGrid[19]     //
	{0x0F12,0x01FE, WORD_LEN, 0}, //204,	awbb_OutdoorGrZone{0x_m_BGrid[20]     //
	{0x0F12,0x025E, WORD_LEN, 0}, //24B,	awbb_OutdoorGrZone{0x_m_BGrid[21]     //
	{0x0F12,0x01FE, WORD_LEN, 0}, //1FD,	awbb_OutdoorGrZone{0x_m_BGrid[22]     //
	{0x0F12,0x0258, WORD_LEN, 0}, //245,	awbb_OutdoorGrZone{0x_m_BGrid[23]     //
	{0x0F12,0x01FE, WORD_LEN, 0}, //1F6,	awbb_OutdoorGrZone{0x_m_BGrid[24]     //
	{0x0F12,0x0252, WORD_LEN, 0}, //23E,	awbb_OutdoorGrZone{0x_m_BGrid[25]     //
	{0x0F12,0x0200, WORD_LEN, 0}, //1F1,	awbb_OutdoorGrZone{0x_m_BGrid[26]     //
	{0x0F12,0x0238, WORD_LEN, 0}, //238,	awbb_OutdoorGrZone{0x_m_BGrid[27]     //
	{0x0F12,0x0204, WORD_LEN, 0}, //1EC,	awbb_OutdoorGrZone{0x_m_BGrid[28]     //
	{0x0F12,0x0224, WORD_LEN, 0}, //22F,	awbb_OutdoorGrZone{0x_m_BGrid[29]     //
	{0x0F12,0x0000, WORD_LEN, 0}, //1E9,	awbb_OutdoorGrZone{0x_m_BGrid[30]     //
	{0x0F12,0x0000, WORD_LEN, 0}, //219,	awbb_OutdoorGrZone{0x_m_BGrid[31]     //
	{0x0F12,0x0000, WORD_LEN, 0}, //1EF,	awbb_OutdoorGrZone{0x_m_BGrid[32]     //
	{0x0F12,0x0000, WORD_LEN, 0}, //1F8,	awbb_OutdoorGrZone{0x_m_BGrid[33]     //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[34]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[35]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[36]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[37]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[38]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[39]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[40]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[41]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[42]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[43]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[44]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[45]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[46]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[47]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_BGrid[48]            //
	{0x0F12,0x0000, WORD_LEN, 0}, //	param_end	awbb_OutdoorGrZone{0x_m_BGrid // //awbb_OutdoorGrZone{0x_m_BGrid[49]            //       
	{0x0F12,0x0003, WORD_LEN, 0}, //awbb_OutdoorGrZone{0x_m_Grid{0xtep//
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x002A,0x0D70, WORD_LEN, 0},
	{0x0F12,0x000F, WORD_LEN, 0}, //0011//       
	{0x002A,0x0D74, WORD_LEN, 0},
	{0x0F12,0x021f, WORD_LEN, 0}, //2F, awbb_OutdoorGrZone{0x_m_Boff{0x//
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x002A,0x0E00, WORD_LEN, 0},
	{0x0F12,0x034A, WORD_LEN, 0}, //awbb_CrclLowT_R_c                //
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0176, WORD_LEN, 0}, //awbb_CrclLowT_B_c                //
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x71B8, WORD_LEN, 0}, //awbb_CrclLowT_Rad_c              //
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x002A,0x0E1A, WORD_LEN, 0},
	{0x0F12,0x012F, WORD_LEN, 0},
	{0x0F12,0x0120, WORD_LEN, 0},      
	{0x002A,0x0E68, WORD_LEN, 0}, // awbb_LowTempRB //
	{0x0F12,0x04F2, WORD_LEN, 0},       
	{0x002A,0x0D78, WORD_LEN, 0},
	{0x0F12,0x0020, WORD_LEN, 0}, //AWB min.	                                   //       
	{0x002A,0x0D80, WORD_LEN, 0},
	{0x0F12,0x00E0, WORD_LEN, 0}, //AWB Max.	                                   //       
	{0x002A,0x0E40, WORD_LEN, 0}, //awbb_U{0xe_Filter{0x                            //
	{0x0F12,0x0061, WORD_LEN, 0}, //AWB option	                               //       
	{0x002A,0x0EE4, WORD_LEN, 0},
	{0x0F12,0x0003, WORD_LEN, 0}, //awbb_OutdoorFltr{0xz                          //
	{0x002A,0x0E3C, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0}, //awbb_U{0xe_InvalidOutDoor                     //
	{0x002A,0x0F3A, WORD_LEN, 0},
	{0x0F12,0x024C, WORD_LEN, 0}, //awbb_OutdoorWP_r	   //
	{0x0F12,0x0290, WORD_LEN, 0}, //awbb_OutdoorWP_b	   //
	{0x002A,0x0E46, WORD_LEN, 0},
	{0x0F12,0x0fa0, WORD_LEN, 0}, //awbb_{0xunnyBr	  	                       //
	{0x0F12,0x0096, WORD_LEN, 0}, //awbb_{0xunny_NBzone  	                   //
	{0x0F12,0x0fa0, WORD_LEN, 0}, //awbb_CloudyBr                     //
	{0x002A,0x0E5E, WORD_LEN, 0},
	{0x0F12,0x071A, WORD_LEN, 0}, //awbb_GamutWidthThr1                         //
	{0x0F12,0x03A4, WORD_LEN, 0},       
	{0x002A,0x0E50, WORD_LEN, 0},
	{0x0F12,0x001B, WORD_LEN, 0}, //awbb_MacbethGamut_WidthZone                 //
	{0x0F12,0x000E, WORD_LEN, 0},
	{0x0F12,0x0008, WORD_LEN, 0},
	{0x0F12,0x0004, WORD_LEN, 0},       
	{0x002A,0x0E36, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0}, //awbb_ByPa{0x{0x_LowTempMode                     //         
	{0x002a,0x0e18, WORD_LEN, 0},
	{0x0f12,0x0000, WORD_LEN, 0}, // 32 awbb_dark  //       
	{0x002A,0x0E3A, WORD_LEN, 0}, // AWB etc //
	{0x0F12,0x02C2, WORD_LEN, 0}, //awbb_Alpha_Comp_Mode                   //
	{0x002A,0x0F12, WORD_LEN, 0},
	{0x0F12,0x02C9, WORD_LEN, 0}, //awbb_GLocu{0xR                           //
	{0x0F12,0x033F, WORD_LEN, 0}, //awbb_GLocu{0xB                           //
	{0x002A,0x0E1A, WORD_LEN, 0},
	{0x0F12,0x0138, WORD_LEN, 0}, //awbb_IntcR                             //       
	{0x002A,0x236C, WORD_LEN, 0}, //2180//
	{0x0F12,0x0000, WORD_LEN, 0}, //AWBBTune_EVT4_uInitPo{0xtToleranceCnt    //    
	{0x002A,0x0c48, WORD_LEN, 0}, // AWB {0xtart Point // //awbb_Gain{0xInit //
	{0x0F12,0x053C, WORD_LEN, 0}, //R Gain         //
	{0x0F12,0x03b0, WORD_LEN, 0}, //400,           //
	{0x0F12,0x055C, WORD_LEN, 0}, //B Gain         //       
	{0x002A,0x0E42, WORD_LEN, 0}, // 8. Grid Correction //
	{0x0F12,0x0002, WORD_LEN, 0},       
	{0x002A,0x0EE0, WORD_LEN, 0},
	{0x0F12,0x00B5, WORD_LEN, 0}, //awbb_GridCoeff_R_2                                          //
	{0x0F12,0x00B5, WORD_LEN, 0}, //awbb_GridCoeff_B_2                                          //
	{0x002A,0x0ED0, WORD_LEN, 0}, //                                                            //
	{0x0F12,0x0EC8, WORD_LEN, 0}, //awbb_GridCon{0xt_2[0]                                         //
	{0x0F12,0x1022, WORD_LEN, 0}, //awbb_GridCon{0xt_2[1]                                         //
	{0x0F12,0x10BB, WORD_LEN, 0}, //awbb_GridCon{0xt_2[2]                                         //
	{0x0F12,0x10C9, WORD_LEN, 0}, //awbb_GridCon{0xt_2[3]                                         //
	{0x0F12,0x1149, WORD_LEN, 0}, //awbb_GridCon{0xt_2[4]                                         //
	{0x0F12,0x11FD, WORD_LEN, 0}, //awbb_GridCon{0xt_2[5]                                         //
	{0x0F12,0x00B8, WORD_LEN, 0}, //awbb_GridCoeff_R_1                                          //
	{0x0F12,0x00B2, WORD_LEN, 0}, //awbb_GridCoeff_B_1                                          //
	{0x002A,0x0ECA, WORD_LEN, 0}, //                                                            //
	{0x0F12,0x029A, WORD_LEN, 0}, //awbb_GridCon{0xt_1[0]                                         //
	{0x0F12,0x0344, WORD_LEN, 0}, //awbb_GridCon{0xt_1[1]                                         //
	{0x0F12,0x03FB, WORD_LEN, 0}, //awbb_GridCon{0xt_1[2]                                         //       
	{0x002A,0x0E82, WORD_LEN, 0}, // 0507                                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000,  awbb_GridCorr_R[0][0] d65                          //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[0][1]  d50                                //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[0][2]  cw                                 //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[0][3]  ww                                 //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[0][4]  a                                  //
	{0x0F12,0x0000, WORD_LEN, 0}, //58, a8, C8,  awbb_GridCorr_R[0][5] h                      //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000,  awbb_GridCorr_R[1][0]                              //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[1][1]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[1][2]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[1][3]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[1][4]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //58, a8, C8,  awbb_GridCorr_R[1][5]                        //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000,  awbb_GridCorr_R[2][0]                              //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[2][1]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[2][2]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[2][3]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_R[2][4]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //58, a8, C8,  awbb_GridCorr_R[2][5]                        //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000, FFC4,  awbb_GridCorr_B[0][0]                        //
	{0x0F12,0x0000, WORD_LEN, 0}, //FFC4,  awbb_GridCorr_B[0][1]                              //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000,  awbb_GridCorr_B[0][2]                              //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_B[0][3]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //ff70 FF7a, 6A,  awbb_GridCorr_B[0][4]                          //
	{0x0F12,0x0000, WORD_LEN, 0}, //fee0 ff00, fe00, FD84, 44,  awbb_GridCorr_B[0][5]              //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000, FFC4,  awbb_GridCorr_B[1][0]                        //
	{0x0F12,0x0000, WORD_LEN, 0}, //FFC4,  awbb_GridCorr_B[1][1]                              //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000,  awbb_GridCorr_B[1][2]                              //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_B[1][3]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //ff70 FF7a, 6A,  awbb_GridCorr_B[1][4]                          //
	{0x0F12,0x0000, WORD_LEN, 0}, //fee0 ff00, fe00, FD84, 44,  awbb_GridCorr_B[1][5]              //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000, FFC4,  awbb_GridCorr_B[2][0]                        //
	{0x0F12,0x0000, WORD_LEN, 0}, //FFC4,  awbb_GridCorr_B[2][1]                              //
	{0x0F12,0x0000, WORD_LEN, 0}, //0000,  awbb_GridCorr_B[2][2]                              //
	{0x0F12,0x0000, WORD_LEN, 0}, //awbb_GridCorr_B[2][3]                                     //
	{0x0F12,0x0000, WORD_LEN, 0}, //ff70 FF7a, 6A,  awbb_GridCorr_B[2][4]                          //
	{0x0F12,0x0000, WORD_LEN, 0}, //fee0 ff00, fe00, FD84, 44,  awbb_GridCorr_B[2][5]              //
	{0x002A,0x06D4, WORD_LEN, 0}, // 9. CCM //
	{0x0F12,0x2380, WORD_LEN, 0}, //TVAR_wbt_pOutdoorCcm//
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x002A,0x06CC, WORD_LEN, 0},
	{0x0F12,0x23A4, WORD_LEN, 0}, //TVAR_wbt_pBa{0xeCcm{0x  //
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x002A,0x06E8, WORD_LEN, 0},
	{0x0F12,0x23A4, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x23C8, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x23EC, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x2410, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x2434, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x0F12,0x2458, WORD_LEN, 0},
	{0x0F12,0x7000, WORD_LEN, 0},
	{0x002A,0x06DA, WORD_LEN, 0},
	{0x0F12,0x00BF, WORD_LEN, 0}, //{0xARR_AwbCcmCord[0]  2   700006DA   //
	{0x0F12,0x00E6, WORD_LEN, 0}, //{0xARR_AwbCcmCord[1]  2   700006DC   //
	{0x0F12,0x00F2, WORD_LEN, 0}, //{0xARR_AwbCcmCord[2]  2   700006DE   //
	{0x0F12,0x0143, WORD_LEN, 0}, //{0xARR_AwbCcmCord[3]  2   700006E0   //
	{0x0F12,0x0178, WORD_LEN, 0}, //{0xARR_AwbCcmCord[4]  2   700006E2   //
	{0x0F12,0x01A3, WORD_LEN, 0}, //{0xARR_AwbCcmCord[5]                 //   
	{0x002A,0x23A4, WORD_LEN, 0}, // param_{0xtart	TVAR_wbt_pBa{0xeCcm{0x //
	{0x0F12,0x01F3, WORD_LEN, 0}, //01FA     23A4       //
	{0x0F12,0xFFAA, WORD_LEN, 0}, //FFB9                //
	{0x0F12,0xFFD5, WORD_LEN, 0}, //FFF8                //
	{0x0F12,0x0139, WORD_LEN, 0}, //00EC                //
	{0x0F12,0x00E6, WORD_LEN, 0}, //00EB                //
	{0x0F12,0xFEFF, WORD_LEN, 0}, //FF33                //
	{0x0F12,0xFF4B, WORD_LEN, 0}, //FF23                //
	{0x0F12,0x01EB, WORD_LEN, 0}, //01AB                //
	{0x0F12,0xFF1E, WORD_LEN, 0}, //FF81                //
	{0x0F12,0xFEED, WORD_LEN, 0}, //FF0D                //
	{0x0F12,0x0158, WORD_LEN, 0}, //0169                //
	{0x0F12,0x0111, WORD_LEN, 0}, //00DE                //
	{0x0F12,0xFF61, WORD_LEN, 0}, //FFEF                //
	{0x0F12,0xFF9E, WORD_LEN, 0}, //FFCA                //
	{0x0F12,0x01C6, WORD_LEN, 0}, //014D                //
	{0x0F12,0x0168, WORD_LEN, 0}, //01C3                //
	{0x0F12,0xFF58, WORD_LEN, 0}, //FF7E                //
	{0x0F12,0x01A3, WORD_LEN, 0}, //016F                //       
	{0x0F12,0x01F3, WORD_LEN, 0}, //01FA    A23C8       //
	{0x0F12,0xFFAA, WORD_LEN, 0}, //FFB9                //
	{0x0F12,0xFFD5, WORD_LEN, 0}, //FFF8                //
	{0x0F12,0x0139, WORD_LEN, 0}, //00EC                //
	{0x0F12,0x00E6, WORD_LEN, 0}, //00EB                //
	{0x0F12,0xFEFF, WORD_LEN, 0}, //FF33                //
	{0x0F12,0xFF4B, WORD_LEN, 0}, //FF23                //
	{0x0F12,0x01EB, WORD_LEN, 0}, //01AB                //
	{0x0F12,0xFF1E, WORD_LEN, 0}, //FF81                //
	{0x0F12,0xFEED, WORD_LEN, 0}, //FF0D                //
	{0x0F12,0x0158, WORD_LEN, 0}, //0169                //
	{0x0F12,0x0111, WORD_LEN, 0}, //00DE                //
	{0x0F12,0xFF61, WORD_LEN, 0}, //FFEF                //
	{0x0F12,0xFF9E, WORD_LEN, 0}, //FFCA                //
	{0x0F12,0x01C6, WORD_LEN, 0}, //014D                //
	{0x0F12,0x0168, WORD_LEN, 0}, //01C3                //
	{0x0F12,0xFF58, WORD_LEN, 0}, //FF7E                //
	{0x0F12,0x01A3, WORD_LEN, 0}, //016F                //       
	{0x0F12,0x01F3, WORD_LEN, 0}, //01FA	WW23EC         //
	{0x0F12,0xFFAA, WORD_LEN, 0}, //FFB9                //
	{0x0F12,0xFFD5, WORD_LEN, 0}, //FFF8                //
	{0x0F12,0x0139, WORD_LEN, 0}, //00EC                //
	{0x0F12,0x00E6, WORD_LEN, 0}, //00EB                //
	{0x0F12,0xFEFF, WORD_LEN, 0}, //FF33                //
	{0x0F12,0xFF4B, WORD_LEN, 0}, //FF23                //
	{0x0F12,0x01EB, WORD_LEN, 0}, //01AB                //
	{0x0F12,0xFF1E, WORD_LEN, 0}, //FF81                //
	{0x0F12,0xFEED, WORD_LEN, 0}, //FF0D                //
	{0x0F12,0x0158, WORD_LEN, 0}, //0169                //
	{0x0F12,0x0111, WORD_LEN, 0}, //00DE                //
	{0x0F12,0xFF61, WORD_LEN, 0}, //FFEF                //
	{0x0F12,0xFF9E, WORD_LEN, 0}, //FFCA                //
	{0x0F12,0x01C6, WORD_LEN, 0}, //014D                //
	{0x0F12,0x0168, WORD_LEN, 0}, //01C3                //
	{0x0F12,0xFF58, WORD_LEN, 0}, //FF7E                //
	{0x0F12,0x01A3, WORD_LEN, 0}, //016F                //       
	{0x0F12,0x01FC, WORD_LEN, 0}, //01D6  01E0, 01B7	CW2410       //
	{0x0F12,0xFF95, WORD_LEN, 0}, //FFBD  FFB9, FFD0              //
	{0x0F12,0xFFDC, WORD_LEN, 0}, //000D  000C, 0019              //
	{0x0F12,0x0111, WORD_LEN, 0}, //00CB  00C5                    //
	{0x0F12,0x00C4, WORD_LEN, 0}, //00F3  00E3                    //
	{0x0F12,0xFF49, WORD_LEN, 0}, //FF4C  FF5D                    //
	{0x0F12,0xFF24, WORD_LEN, 0}, //FF33  FF3E                    //
	{0x0F12,0x01F1, WORD_LEN, 0}, //019C  0186                    //
	{0x0F12,0xFF3D, WORD_LEN, 0}, //FF85  FF8C                    //
	{0x0F12,0xFEEB, WORD_LEN, 0}, //FF0E  FF0E                    //
	{0x0F12,0x014D, WORD_LEN, 0}, //016A  016A                    //
	{0x0F12,0x011F, WORD_LEN, 0}, //00DF  00DF                    //
	{0x0F12,0xFF9D, WORD_LEN, 0}, //FFEE  FFEE                    //
	{0x0F12,0xFFAC, WORD_LEN, 0}, //FFC9  FFC9                    //
	{0x0F12,0x017E, WORD_LEN, 0}, //014C  014C                    //
	{0x0F12,0x0179, WORD_LEN, 0}, //01C1  01C1                    //
	{0x0F12,0xFF4F, WORD_LEN, 0}, //FF7C  FF7C                    //
	{0x0F12,0x019A, WORD_LEN, 0}, //016D  016D                    //                                                                           
	{0x0F12,0x01CD, WORD_LEN, 0}, //01E2, 01E0, 01B7	D502434      //
	{0x0F12,0xFFC5, WORD_LEN, 0}, //FFB8, FFB9, FFD0              //
	{0x0F12,0x0013, WORD_LEN, 0}, //000B, 000C, 0019              //
	{0x0F12,0x00CF, WORD_LEN, 0}, //00CB, 00C5                    //
	{0x0F12,0x00F8, WORD_LEN, 0}, //00F3, 00E3                    //
	{0x0F12,0xFF44, WORD_LEN, 0}, //FF4C, FF5D                    //
	{0x0F12,0xFF62, WORD_LEN, 0}, //ff20, FF33, FF3E              //
	{0x0F12,0x01B3, WORD_LEN, 0}, //01bb, 019C, 0186              //
	{0x0F12,0xFF3E, WORD_LEN, 0}, //ff79, FF85, FF8C              //
	{0x0F12,0xFF06, WORD_LEN, 0}, //FF0E, FF0E                    //
	{0x0F12,0x0170, WORD_LEN, 0}, //016A, 016A                    //
	{0x0F12,0x00E1, WORD_LEN, 0}, //00DF, 00DF                    //
	{0x0F12,0xFFE8, WORD_LEN, 0}, //FFEE, FFEE                    //
	{0x0F12,0xFFC1, WORD_LEN, 0}, //FFC9, FFC9                    //
	{0x0F12,0x015A, WORD_LEN, 0}, //014C, 014C                    //
	{0x0F12,0x01C0, WORD_LEN, 0}, //01C1, 01C1                    //
	{0x0F12,0xFF7E, WORD_LEN, 0}, //FF7C, FF7C                    //
	{0x0F12,0x016C, WORD_LEN, 0}, //016D, 016D                    //       
	{0x0F12,0x01CD, WORD_LEN, 0}, //01E4, 01EA, 0202, 01E2, 01E0, 01B7	D652458   //
	{0x0F12,0xFFC5, WORD_LEN, 0}, //FFAB, FF98, FFA2, FFB8, FFB9, FFD0             //
	{0x0F12,0x0013, WORD_LEN, 0}, //0000, FFF0, FFFD, 000B, 000C, 0019             //
	{0x0F12,0x00CF, WORD_LEN, 0}, //00C7, 00BE, 00CC, 00CB, 00CB, 00C5             //
	{0x0F12,0x00F8, WORD_LEN, 0}, //00EF, 00E5, 00F4, 00F3, 00F3, 00E3             //
	{0x0F12,0xFF44, WORD_LEN, 0}, //FF48, FF3E, FF4D, FF4C, FF4C, FF5D             //
	{0x0F12,0xFF62, WORD_LEN, 0}, //FF5D, FF5B, FF68, FF66, ff20, FF33, FF3E       //
	{0x0F12,0x01B3, WORD_LEN, 0}, //01BE, 019F, 01AD, 01AB, 01bb, 019C, 0186       //
	{0x0F12,0xFF3E, WORD_LEN, 0}, //FF36, FF38, FF45, FF43, ff79, FF85, FF8C       //
	{0x0F12,0xFF06, WORD_LEN, 0}, //FF07, FF01, FF0F, FF0E, FF0E, FF0E             //
	{0x0F12,0x0170, WORD_LEN, 0}, //0162, 015C, 016B, 016A, 016A, 016A             //
	{0x0F12,0x00E1, WORD_LEN, 0}, //00D8, 00D2, 00E0, 00DF, 00DF, 00DF             //
	{0x0F12,0xFFE8, WORD_LEN, 0}, //FFD7, FFD7, FFE7, FFE8, FFEE, FFEE             //
	{0x0F12,0xFFC1, WORD_LEN, 0}, //FFB1, FFB1, FFC0, FFC1, FFC9, FFC9             //
	{0x0F12,0x015A, WORD_LEN, 0}, //014A, 014A, 0159, 015A, 014C, 014C             //
	{0x0F12,0x01C0, WORD_LEN, 0}, //01B4, 01AE, 01BF, 01C1, 01C1, 01C1             //
	{0x0F12,0xFF7E, WORD_LEN, 0}, //FF6F, FF6A, FF7A, FF7C, FF7C, FF7C             //
	{0x0F12,0x016C, WORD_LEN, 0}, //	param_end	TVAR_wbt_pBa{0xeCcm{0x // //0160, 015B, 016B, 016D, 016D, 016D             //
	{0x002A,0x2380, WORD_LEN, 0}, //	param_{0xtart	TVAR_wbt_pOutdoorCcm //
	{0x0F12,0x01B7, WORD_LEN, 0}, //01B7, 01B9, 01DD, TVAR_wbt_pOutdoorCcm[0]  야외얼굴색감       하늘     //
	{0x0F12,0xFFBC, WORD_LEN, 0}, //FFBC, FFBC, FFA8, TVAR_wbt_pOutdoorCcm[1]                              //
	{0x0F12,0x0011, WORD_LEN, 0}, //0011, 0012, 0008, TVAR_wbt_pOutdoorCcm[2]                              //
	{0x0F12,0x00FC, WORD_LEN, 0}, //00FB, 00FD, TVAR_wbt_pOutdoorCcm[3]                                    //
	{0x0F12,0x013B, WORD_LEN, 0}, //013B, 0141, TVAR_wbt_pOutdoorCcm[4]                                    //
	{0x0F12,0xFF84, WORD_LEN, 0}, //FF82, FF74, TVAR_wbt_pOutdoorCcm[5]                                    //
	{0x0F12,0xFED9, WORD_LEN, 0}, //FED6, FEC6, TVAR_wbt_pOutdoorCcm[6]                                    //
	{0x0F12,0x01E6, WORD_LEN, 0}, //01E6, 01F6, TVAR_wbt_pOutdoorCcm[7]                                    //
	{0x0F12,0xFF16, WORD_LEN, 0}, //FF13, FF05, TVAR_wbt_pOutdoorCcm[8]                                    //
	{0x0F12,0xFF4A, WORD_LEN, 0}, //FF48, FF38, TVAR_wbt_pOutdoorCcm[9]                                    //
	{0x0F12,0x0179, WORD_LEN, 0}, //0179, 017F, TVAR_wbt_pOutdoorCcm[10]                                   //
	{0x0F12,0x014F, WORD_LEN, 0}, //014F, 0153, TVAR_wbt_pOutdoorCcm[11]                                   //
	{0x0F12,0xFFC2, WORD_LEN, 0}, //FFCB, FFCC, TVAR_wbt_pOutdoorCcm[12]                                   //
	{0x0F12,0xFF99, WORD_LEN, 0}, //FFA4, FFA6, TVAR_wbt_pOutdoorCcm[13]                                   //
	{0x0F12,0x0219, WORD_LEN, 0}, //0207, 020A, TVAR_wbt_pOutdoorCcm[14]                                   //
	{0x0F12,0x0172, WORD_LEN, 0}, //0174, 017E, TVAR_wbt_pOutdoorCcm[15]                                   //
	{0x0F12,0xFF51, WORD_LEN, 0}, //FF51, FF45, TVAR_wbt_pOutdoorCcm[16]                                   //
	{0x0F12,0x019B, WORD_LEN, 0}, // param_end	TVAR_wbt_pOutdoorCcm  // //019E, 01AA, TVAR_wbt_pOutdoorCcm[17]    
	{0x002A,0x07E8, WORD_LEN, 0}, // 10. AFIT // // param_{0xtart	{0xARR_uNormBrInDoor //
	{0x0F12,0x0016, WORD_LEN, 0}, //000A	{0xARR_uNormBrInDoor[0] //
	{0x0F12,0x0028, WORD_LEN, 0}, //0019	{0xARR_uNormBrInDoor[1] //
	{0x0F12,0x0096, WORD_LEN, 0}, //0096	{0xARR_uNormBrInDoor[2] //
	{0x0F12,0x01F4, WORD_LEN, 0}, //01F4	{0xARR_uNormBrInDoor[3] //
	{0x0F12,0x07D0, WORD_LEN, 0}, // param_end	{0xARR_uNormBrInDoor // //07D0	{0xARR_uNormBrInDoor[4] //    
	{0x002A,0x07D0, WORD_LEN, 0}, //	param_{0xtart	afit_uNoi{0xeIndInDoor //
	{0x0F12,0x0030, WORD_LEN, 0}, //afit_uNoi{0xeIndInDoor[0] //
	{0x0F12,0x0046, WORD_LEN, 0}, //afit_uNoi{0xeIndInDoor[1] //
	{0x0F12,0x0088, WORD_LEN, 0}, //afit_uNoi{0xeIndInDoor[2] //
	{0x0F12,0x0205, WORD_LEN, 0}, //afit_uNoi{0xeIndInDoor[3] //
	{0x0F12,0x02BC, WORD_LEN, 0}, //	param_end	afit_uNoi{0xeIndInDoor // //afit_uNoi{0xeIndInDoor[4] //       
	{0x002A,0x07E6, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0}, //afit_bU{0xeNoi{0xeInd //
	{0x002A,0x0828, WORD_LEN, 0},
	{0x0F12,0x0004, WORD_LEN, 0}, //10	//TVAR_afit_pBa{0xeVal{0x[0]     70000828  //BRIGHTNE{0x{0x                                                                
	{0x0F12,0x0014, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[1]     7000082A  //CONTRA{0xT                                                                  
	{0x0F12,0xffe7, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[2]     7000082C  //{0xATURATION                                                                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[3]     7000082E  //{0xHARP_BLUR                                                                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[4]     70000830  //GLAMOUR                                                                   
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[5]     70000832  //Di{0xparity_i{0xat{0xat                                                         
	{0x0F12,0x0021, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[6]     70000834  //Denoi{0xe1_iYDenThre{0xhLow                                                   
	{0x0F12,0x0028, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[7]     70000836  //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
	{0x0F12,0x0050, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[8]     70000838  //Denoi{0xe1_iYDenThre{0xhHigh                                                  
	{0x0F12,0x00FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[9]     7000083A  //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
	{0x0F12,0x0129, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[10]    7000083C  //Denoi{0xe1_iLowWWideThre{0xh                                                  
	{0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[11]    7000083E  //Denoi{0xe1_iHighWWideThre{0xh                                                 
	{0x0F12,0x0028, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[12]    70000840  //Denoi{0xe1_iLowWideThre{0xh                                                   
	{0x0F12,0x0028, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[13]    70000842  //Denoi{0xe1_iHighWideThre{0xh                                                  
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[14]    70000844  //Denoi{0xe1_i{0xat{0xat                                                          
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[15]    70000846  //Demo{0xaic4_iHy{0xtGrayLow                                                    
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[16]    70000848  //Demo{0xaic4_iHy{0xtGrayHigh                                                   
	{0x0F12,0x030c, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[17]    7000084A  //UVDenoi{0xe_iYLowThre{0xh                                                     
	{0x0F12,0x030c, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[18]    7000084C  //UVDenoi{0xe_iYHighThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[19]    7000084E  //UVDenoi{0xe_iUVLowThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[20]    70000850  //UVDenoi{0xe_iUVHighThre{0xh                                                   
	{0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[21]    70000852  //D{0xMix1_iLowLimit_Wide                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[22]    70000854  //D{0xMix1_iLowLimit_Wide_Bin                                                 
	{0x0F12,0x001E, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[23]    70000856  //D{0xMix1_iHighLimit_Wide                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[24]    70000858  //D{0xMix1_iHighLimit_Wide_Bin                                                
	{0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[25]    7000085A  //D{0xMix1_iLowLimit_Fine                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[26]    7000085C  //D{0xMix1_iLowLimit_Fine_Bin                                                 
	{0x0F12,0x0010, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[27]    7000085E  //D{0xMix1_iHighLimit_Fine                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[28]    70000860  //D{0xMix1_iHighLimit_Fine_Bin                                                
	{0x0F12,0x0106, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[29]    70000862  //D{0xMix1_iRGBOff{0xet                                                         
	{0x0F12,0x006F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[30]    70000864  //D{0xMix1_iDemClamp                                                          
	{0x0F12,0x0C0F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[31]    70000866  //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
	{0x0F12,0x0C0F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[32]    70000868  //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
	{0x0F12,0x0303, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[33]    7000086A  //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
	{0x0F12,0x0303, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[34]    7000086C  //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
	{0x0F12,0x140A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[35]    7000086E  //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
	{0x0F12,0x140A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[36]    70000870  //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
	{0x0F12,0x2828, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[37]    70000872  //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[38]    70000874  //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
	{0x0F12,0x020A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[39]    70000876  //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
	{0x0F12,0x0480, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[40]    70000878  //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
	{0x0F12,0x0E08, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[41]    7000087A  //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
	{0x0F12,0x030A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[42]    7000087C  //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
	{0x0F12,0x0A03, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[43]    7000087E  //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
	{0x0F12,0x0A11, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[44]    70000880  //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
	{0x0F12,0x000F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[45]    70000882  //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
	{0x0F12,0x0500, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[46]    70000884  //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
	{0x0F12,0x0914, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[47]    70000886  //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
	{0x0F12,0x0012, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[48]    70000888  //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[49]    7000088A  //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
	{0x0F12,0x0005, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[50]    7000088C  //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[51]    7000088E  //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[52]    70000890  //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[53]    70000892  //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[54]    70000894  //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
	{0x0F12,0x0A00, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[55]    70000896  //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
	{0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[56]    70000898  //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
	{0x0F12,0x0180, WORD_LEN, 0}, //014C	//TVAR_afit_pBa{0xeVal{0x[57]    7000089A  //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
	{0x0F12,0x014D, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[58]    7000089C  //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
	{0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[59]    7000089E  //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
	{0x0F12,0x8020, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[60]    700008A0  //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[61]    700008A2  //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
	{0x0F12,0x0008, WORD_LEN, 0}, //15 //18 //001a //05 //A	//TVAR_afit_pBa{0xeVal{0x[62]    700008A4  //RGB2YUV_iYOff{0xet                                                          
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[63]    700008A6            //BRIGHTNE{0x{0x                                                                
	{0x0F12,0x0028, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[64]    700008A8            //CONTRA{0xT                                                                  
	{0x0F12,0xfff1, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[65]    700008AA            //{0xATURATION                                                                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[66]    700008AC            //{0xHARP_BLUR                                                                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[67]    700008AE            //GLAMOUR                                                                   
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[68]    700008B0    //Di{0xparity_i{0xat{0xat                                                         
	{0x0F12,0x000C, WORD_LEN, 0}, //0E //0C	//0020	//TVAR_afit_pBa{0xeVal{0x[69]    700008B2    //Denoi{0xe1_iYDenThre{0xhLow                                                   
	{0x0F12,0x000E, WORD_LEN, 0},	//000E	//TVAR_afit_pBa{0xeVal{0x[70]    700008B4    //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
	{0x0F12,0x0050, WORD_LEN, 0},	//0080	//TVAR_afit_pBa{0xeVal{0x[71]    700008B6    //Denoi{0xe1_iYDenThre{0xhHigh                                                  
	{0x0F12,0x00FF, WORD_LEN, 0},	//00FF	//TVAR_afit_pBa{0xeVal{0x[72]    700008B8    //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
	{0x0F12,0x0129, WORD_LEN, 0},	//0129	//TVAR_afit_pBa{0xeVal{0x[73]    700008BA    //Denoi{0xe1_iLowWWideThre{0xh                                                  
	{0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[74]    700008BC    //Denoi{0xe1_iHighWWideThre{0xh                                                 
	{0x0F12,0x0028, WORD_LEN, 0},	//0028	//TVAR_afit_pBa{0xeVal{0x[75]    700008BE    //Denoi{0xe1_iLowWideThre{0xh                                                   
	{0x0F12,0x0028, WORD_LEN, 0},	//0028	//TVAR_afit_pBa{0xeVal{0x[76]    700008C0    //Denoi{0xe1_iHighWideThre{0xh                                                  
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[77]    700008C2    //Denoi{0xe1_i{0xat{0xat                                                          
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[78]    700008C4    //Demo{0xaic4_iHy{0xtGrayLow                                                    
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[79]    700008C6    //Demo{0xaic4_iHy{0xtGrayHigh                                                   
	{0x0F12,0x00c8, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[80]    700008C8    //UVDenoi{0xe_iYLowThre{0xh                                                     
	{0x0F12,0x0190, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[81]    700008CA    //UVDenoi{0xe_iYHighThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[82]    700008CC    //UVDenoi{0xe_iUVLowThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[83]    700008CE    //UVDenoi{0xe_iUVHighThre{0xh                                                   
	{0x0F12,0x0018, WORD_LEN, 0},	//000a	//TVAR_afit_pBa{0xeVal{0x[84]    700008D0    //D{0xMix1_iLowLimit_Wide                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[85]    700008D2    //D{0xMix1_iLowLimit_Wide_Bin                                                 
	{0x0F12,0x000A, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[86]    700008D4    //D{0xMix1_iHighLimit_Wide                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[87]    700008D6    //D{0xMix1_iHighLimit_Wide_Bin                                                
	{0x0F12,0x0028, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[88]    700008D8    //D{0xMix1_iLowLimit_Fine                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[89]    700008DA    //D{0xMix1_iLowLimit_Fine_Bin                                                 
	{0x0F12,0x0010, WORD_LEN, 0},	//00A0	//TVAR_afit_pBa{0xeVal{0x[90]    700008DC    //D{0xMix1_iHighLimit_Fine                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[91]    700008DE    //D{0xMix1_iHighLimit_Fine_Bin                                                
	{0x0F12,0x0106, WORD_LEN, 0},	//0106	//TVAR_afit_pBa{0xeVal{0x[92]    700008E0    //D{0xMix1_iRGBOff{0xet                                                         
	{0x0F12,0x006F, WORD_LEN, 0},	//006F	//TVAR_afit_pBa{0xeVal{0x[93]    700008E2    //D{0xMix1_iDemClamp                                                          
	{0x0F12,0x050F, WORD_LEN, 0},	//050F	//TVAR_afit_pBa{0xeVal{0x[94]    700008E4    //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
	{0x0F12,0x0A0F, WORD_LEN, 0},	//0A0F	//TVAR_afit_pBa{0xeVal{0x[95]    700008E6    //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
	{0x0F12,0x0203, WORD_LEN, 0},	//0203	//TVAR_afit_pBa{0xeVal{0x[96]    700008E8    //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
	{0x0F12,0x0303, WORD_LEN, 0},	//0203	//TVAR_afit_pBa{0xeVal{0x[97]    700008EA    //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
	{0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[98]    700008EC    //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
	{0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[99]    700008EE    //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
	{0x0F12,0x2828, WORD_LEN, 0},	//2828	//TVAR_afit_pBa{0xeVal{0x[100]   700008F0    //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[101]   700008F2    //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
	{0x0F12,0x020A, WORD_LEN, 0},	//020A	//TVAR_afit_pBa{0xeVal{0x[102]   700008F4    //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
	{0x0F12,0x0480, WORD_LEN, 0},	//0480	//TVAR_afit_pBa{0xeVal{0x[103]   700008F6    //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
	{0x0F12,0x0E08, WORD_LEN, 0},	//0E08	//TVAR_afit_pBa{0xeVal{0x[104]   700008F8    //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
	{0x0F12,0x030A, WORD_LEN, 0},	//020A	//TVAR_afit_pBa{0xeVal{0x[105]   700008FA    //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
	{0x0F12,0x1403, WORD_LEN, 0},	//0A03	//TVAR_afit_pBa{0xeVal{0x[106]   700008FC    //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
	{0x0F12,0x0A11, WORD_LEN, 0},	//0A11	//TVAR_afit_pBa{0xeVal{0x[107]   700008FE    //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
	{0x0F12,0x0A0F, WORD_LEN, 0},	//0A0F	//TVAR_afit_pBa{0xeVal{0x[108]   70000900    //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
	{0x0F12,0x050A, WORD_LEN, 0},	//050A	//TVAR_afit_pBa{0xeVal{0x[109]   70000902    //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
	{0x0F12,0x101E, WORD_LEN, 0}, //14 //1E	//101E	//TVAR_afit_pBa{0xeVal{0x[110]   70000904    //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
	{0x0F12,0x101E, WORD_LEN, 0},	//101E	//TVAR_afit_pBa{0xeVal{0x[111]   70000906    //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
	{0x0F12,0x0A08, WORD_LEN, 0},	//3030	//TVAR_afit_pBa{0xeVal{0x[112]   70000908    //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
	{0x0F12,0x0005, WORD_LEN, 0},	//0005	//TVAR_afit_pBa{0xeVal{0x[113]   7000090A    //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
	{0x0F12,0x0400, WORD_LEN, 0},	//0400	//TVAR_afit_pBa{0xeVal{0x[114]   7000090C    //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
	{0x0F12,0x0400, WORD_LEN, 0},	//0400	//TVAR_afit_pBa{0xeVal{0x[115]   7000090E    //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[116]   70000910    //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[117]   70000912    //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
	{0x0F12,0x0A00, WORD_LEN, 0},	//0A00	//TVAR_afit_pBa{0xeVal{0x[118]   70000914    //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
	{0x0F12,0x000A, WORD_LEN, 0},	//100A	//TVAR_afit_pBa{0xeVal{0x[119]   70000916    //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[120]   70000918            //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
	{0x0F12,0x0154, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[121]   7000091A            //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
	{0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[122]   7000091C            //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
	{0x0F12,0x8020, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[123]   7000091E            //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[124]   70000920            //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
	{0x0F12,0x000a, WORD_LEN, 0}, //07 //0	//TVAR_afit_pBa{0xeVal{0x[125]   70000922            //RGB2YUV_iYOff{0xet                                                          
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[126]   70000924            //BRIGHTNE{0x{0x                                                                
	{0x0F12,0x0024, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[127]   70000926            //CONTRA{0xT                                                                  
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[128]   70000928            //{0xATURATION                                                                
	{0x0F12,0x0005, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[129]   7000092A            //{0xHARP_BLUR                                                                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[130]   7000092C            //GLAMOUR                                                                   
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[131]   7000092E    //Di{0xparity_i{0xat{0xat                                                         
	{0x0F12,0x000b, WORD_LEN, 0}, //0e //08	//000E	//TVAR_afit_pBa{0xeVal{0x[132]   70000930    //Denoi{0xe1_iYDenThre{0xhLow                                                   
	{0x0F12,0x0006, WORD_LEN, 0},	//0006	//TVAR_afit_pBa{0xeVal{0x[133]   70000932    //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
	{0x0F12,0x003e, WORD_LEN, 0}, //50	//0064	//TVAR_afit_pBa{0xeVal{0x[134]   70000934    //Denoi{0xe1_iYDenThre{0xhHigh                                                  
	{0x0F12,0x0050, WORD_LEN, 0},	//0050	//TVAR_afit_pBa{0xeVal{0x[135]   70000936    //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
	{0x0F12,0x0002, WORD_LEN, 0},	//0002	//TVAR_afit_pBa{0xeVal{0x[136]   70000938    //Denoi{0xe1_iLowWWideThre{0xh                                                  
	{0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[137]   7000093A    //Denoi{0xe1_iHighWWideThre{0xh                                                 
	{0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[138]   7000093C    //Denoi{0xe1_iLowWideThre{0xh                                                   
	{0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[139]   7000093E    //Denoi{0xe1_iHighWideThre{0xh                                                  
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[140]   70000940    //Denoi{0xe1_i{0xat{0xat                                                          
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[141]   70000942    //Demo{0xaic4_iHy{0xtGrayLow                                                    
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[142]   70000944    //Demo{0xaic4_iHy{0xtGrayHigh                                                   
	{0x0F12,0x0014, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[143]   70000946    //UVDenoi{0xe_iYLowThre{0xh                                                     
	{0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[144]   70000948    //UVDenoi{0xe_iYHighThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[145]   7000094A    //UVDenoi{0xe_iUVLowThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[146]   7000094C    //UVDenoi{0xe_iUVHighThre{0xh                                                   
	{0x0F12,0x001C, WORD_LEN, 0},	//000a	//TVAR_afit_pBa{0xeVal{0x[147]   7000094E    //D{0xMix1_iLowLimit_Wide                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[148]   70000950    //D{0xMix1_iLowLimit_Wide_Bin                                                 
	{0x0F12,0x000A, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[149]   70000952    //D{0xMix1_iHighLimit_Wide                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[150]   70000954    //D{0xMix1_iHighLimit_Wide_Bin                                                
	{0x0F12,0x0028, WORD_LEN, 0},	//0050	//TVAR_afit_pBa{0xeVal{0x[151]   70000956    //D{0xMix1_iLowLimit_Fine                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[152]   70000958    //D{0xMix1_iLowLimit_Fine_Bin                                                 
	{0x0F12,0x0010, WORD_LEN, 0},	//0010	//TVAR_afit_pBa{0xeVal{0x[153]   7000095A    //D{0xMix1_iHighLimit_Fine                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[154]   7000095C    //D{0xMix1_iHighLimit_Fine_Bin                                                
	{0x0F12,0x0106, WORD_LEN, 0},	//0106	//TVAR_afit_pBa{0xeVal{0x[155]   7000095E    //D{0xMix1_iRGBOff{0xet                                                         
	{0x0F12,0x006F, WORD_LEN, 0},	//006F	//TVAR_afit_pBa{0xeVal{0x[156]   70000960    //D{0xMix1_iDemClamp                                                          
	{0x0F12,0x0205, WORD_LEN, 0},	//020A	//TVAR_afit_pBa{0xeVal{0x[157]   70000962    //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
	{0x0F12,0x0505, WORD_LEN, 0},	//050A	//TVAR_afit_pBa{0xeVal{0x[158]   70000964    //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
	{0x0F12,0x0101, WORD_LEN, 0},	//0101	//TVAR_afit_pBa{0xeVal{0x[159]   70000966    //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
	{0x0F12,0x0202, WORD_LEN, 0},	//0102	//TVAR_afit_pBa{0xeVal{0x[160]   70000968    //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
	{0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[161]   7000096A    //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
	{0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[162]   7000096C    //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
	{0x0F12,0x2828, WORD_LEN, 0},	//2828	//TVAR_afit_pBa{0xeVal{0x[163]   7000096E    //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
	{0x0F12,0x0606, WORD_LEN, 0},	//0606	//TVAR_afit_pBa{0xeVal{0x[164]   70000970    //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
	{0x0F12,0x0205, WORD_LEN, 0},	//0205	//TVAR_afit_pBa{0xeVal{0x[165]   70000972    //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
	{0x0F12,0x0480, WORD_LEN, 0},	//0480	//TVAR_afit_pBa{0xeVal{0x[166]   70000974    //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
	{0x0F12,0x000A, WORD_LEN, 0},	//000F	//TVAR_afit_pBa{0xeVal{0x[167]   70000976    //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
	{0x0F12,0x0005, WORD_LEN, 0},	//0005	//TVAR_afit_pBa{0xeVal{0x[168]   70000978    //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
	{0x0F12,0x1903, WORD_LEN, 0},	//1903	//TVAR_afit_pBa{0xeVal{0x[169]   7000097A    //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
	{0x0F12,0x1611, WORD_LEN, 0}, //0f11 //1911	//1911	//TVAR_afit_pBa{0xeVal{0x[170]   7000097C    //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
	{0x0F12,0x0A0F, WORD_LEN, 0},	//0A0F	//TVAR_afit_pBa{0xeVal{0x[171]   7000097E    //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
	{0x0F12,0x050A, WORD_LEN, 0},	//050A	//TVAR_afit_pBa{0xeVal{0x[172]   70000980    //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
	{0x0F12,0x2025, WORD_LEN, 0}, //14 //28	//2028	//TVAR_afit_pBa{0xeVal{0x[173]   70000982    //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
	{0x0F12,0x2025, WORD_LEN, 0}, //1e //28	//2028	//TVAR_afit_pBa{0xeVal{0x[174]   70000984    //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
	{0x0F12,0x0A08, WORD_LEN, 0},	//2000	//TVAR_afit_pBa{0xeVal{0x[175]   70000986    //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
	{0x0F12,0x0007, WORD_LEN, 0},	//0007	//TVAR_afit_pBa{0xeVal{0x[176]   70000988    //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
	{0x0F12,0x0403, WORD_LEN, 0},	//0403	//TVAR_afit_pBa{0xeVal{0x[177]   7000098A    //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
	{0x0F12,0x0402, WORD_LEN, 0},	//0402	//TVAR_afit_pBa{0xeVal{0x[178]   7000098C    //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[179]   7000098E    //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
	{0x0F12,0x0203, WORD_LEN, 0},	//0203	//TVAR_afit_pBa{0xeVal{0x[180]   70000990    //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[181]   70000992    //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
	{0x0F12,0x0006, WORD_LEN, 0},	//1006	//TVAR_afit_pBa{0xeVal{0x[182]   70000994    //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[183]   70000996            //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
	{0x0F12,0x0173, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[184]   70000998            //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
	{0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[185]   7000099A            //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
	{0x0F12,0x8032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[186]   7000099C            //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[187]   7000099E            //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[188]   700009A0            //RGB2YUV_iYOff{0xet                                                          
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[189]   700009A2            //BRIGHTNE{0x{0x                                                                
	{0x0F12,0x0014, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[190]   700009A4            //CONTRA{0xT                                                                  
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[191]   700009A6            //{0xATURATION                                                                
	{0x0F12,0x0005, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[192]   700009A8            //{0xHARP_BLUR                                                                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[193]   700009AA            //GLAMOUR                                                                   
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[194]   700009AC    //Di{0xparity_i{0xat{0xat                                                         
	{0x0F12,0x000b, WORD_LEN, 0}, //0e //08	//000E	//TVAR_afit_pBa{0xeVal{0x[195]   700009AE    //Denoi{0xe1_iYDenThre{0xhLow                                                   
	{0x0F12,0x0006, WORD_LEN, 0},	//0006	//TVAR_afit_pBa{0xeVal{0x[196]   700009B0    //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
	{0x0F12,0x003e, WORD_LEN, 0}, //50	//0064	//TVAR_afit_pBa{0xeVal{0x[197]   700009B2    //Denoi{0xe1_iYDenThre{0xhHigh                                                  
	{0x0F12,0x0050, WORD_LEN, 0},	//0050	//TVAR_afit_pBa{0xeVal{0x[198]   700009B4    //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
	{0x0F12,0x0002, WORD_LEN, 0},	//0002	//TVAR_afit_pBa{0xeVal{0x[199]   700009B6    //Denoi{0xe1_iLowWWideThre{0xh                                                  
	{0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[200]   700009B8    //Denoi{0xe1_iHighWWideThre{0xh                                                 
	{0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[201]   700009BA    //Denoi{0xe1_iLowWideThre{0xh                                                   
	{0x0F12,0x000A, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[202]   700009BC    //Denoi{0xe1_iHighWideThre{0xh                                                  
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[203]   700009BE    //Denoi{0xe1_i{0xat{0xat                                                          
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[204]   700009C0    //Demo{0xaic4_iHy{0xtGrayLow                                                    
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[205]   700009C2    //Demo{0xaic4_iHy{0xtGrayHigh                                                   
	{0x0F12,0x0014, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[206]   700009C4    //UVDenoi{0xe_iYLowThre{0xh                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//000A	//TVAR_afit_pBa{0xeVal{0x[207]   700009C6    //UVDenoi{0xe_iYHighThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[208]   700009C8    //UVDenoi{0xe_iUVLowThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//03FF	//TVAR_afit_pBa{0xeVal{0x[209]   700009CA    //UVDenoi{0xe_iUVHighThre{0xh                                                   
	{0x0F12,0x001C, WORD_LEN, 0},	//000a	//TVAR_afit_pBa{0xeVal{0x[210]   700009CC    //D{0xMix1_iLowLimit_Wide                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[211]   700009CE    //D{0xMix1_iLowLimit_Wide_Bin                                                 
	{0x0F12,0x000A, WORD_LEN, 0},	//0014	//TVAR_afit_pBa{0xeVal{0x[212]   700009D0    //D{0xMix1_iHighLimit_Wide                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[213]   700009D2    //D{0xMix1_iHighLimit_Wide_Bin                                                
	{0x0F12,0x0028, WORD_LEN, 0},	//0050	//TVAR_afit_pBa{0xeVal{0x[214]   700009D4    //D{0xMix1_iLowLimit_Fine                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[215]   700009D6    //D{0xMix1_iLowLimit_Fine_Bin                                                 
	{0x0F12,0x0010, WORD_LEN, 0},	//0010	//TVAR_afit_pBa{0xeVal{0x[216]   700009D8    //D{0xMix1_iHighLimit_Fine                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//0032	//TVAR_afit_pBa{0xeVal{0x[217]   700009DA    //D{0xMix1_iHighLimit_Fine_Bin                                                
	{0x0F12,0x0106, WORD_LEN, 0},	//0106	//TVAR_afit_pBa{0xeVal{0x[218]   700009DC    //D{0xMix1_iRGBOff{0xet                                                         
	{0x0F12,0x006F, WORD_LEN, 0},	//006F	//TVAR_afit_pBa{0xeVal{0x[219]   700009DE    //D{0xMix1_iDemClamp                                                          
	{0x0F12,0x0205, WORD_LEN, 0},	//0205	//TVAR_afit_pBa{0xeVal{0x[220]   700009E0    //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
	{0x0F12,0x0505, WORD_LEN, 0},	//0505	//TVAR_afit_pBa{0xeVal{0x[221]   700009E2    //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
	{0x0F12,0x0101, WORD_LEN, 0},	//0101	//TVAR_afit_pBa{0xeVal{0x[222]   700009E4    //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
	{0x0F12,0x0202, WORD_LEN, 0},	//0102	//TVAR_afit_pBa{0xeVal{0x[223]   700009E6    //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
	{0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[224]   700009E8    //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
	{0x0F12,0x140A, WORD_LEN, 0},	//140A	//TVAR_afit_pBa{0xeVal{0x[225]   700009EA    //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
	{0x0F12,0x2828, WORD_LEN, 0},	//2828	//TVAR_afit_pBa{0xeVal{0x[226]   700009EC    //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
	{0x0F12,0x0606, WORD_LEN, 0},	//0606	//TVAR_afit_pBa{0xeVal{0x[227]   700009EE    //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
	{0x0F12,0x0205, WORD_LEN, 0},	//0205	//TVAR_afit_pBa{0xeVal{0x[228]   700009F0    //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
	{0x0F12,0x0480, WORD_LEN, 0},	//0480	//TVAR_afit_pBa{0xeVal{0x[229]   700009F2    //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
	{0x0F12,0x000A, WORD_LEN, 0},	//000F	//TVAR_afit_pBa{0xeVal{0x[230]   700009F4    //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
	{0x0F12,0x0005, WORD_LEN, 0},	//0005	//TVAR_afit_pBa{0xeVal{0x[231]   700009F6    //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
	{0x0F12,0x1903, WORD_LEN, 0},	//1903	//TVAR_afit_pBa{0xeVal{0x[232]   700009F8    //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
	{0x0F12,0x1611, WORD_LEN, 0}, //0f11 //1911	//1911	//TVAR_afit_pBa{0xeVal{0x[233]   700009FA    //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
	{0x0F12,0x0A0F, WORD_LEN, 0},	//0A0F	//TVAR_afit_pBa{0xeVal{0x[234]   700009FC    //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
	{0x0F12,0x050A, WORD_LEN, 0},	//050A	//TVAR_afit_pBa{0xeVal{0x[235]   700009FE    //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
	{0x0F12,0x2025, WORD_LEN, 0}, //14 //28	//2028	//TVAR_afit_pBa{0xeVal{0x[236]   70000A00    //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
	{0x0F12,0x2025, WORD_LEN, 0}, //1E //28	//2028	//TVAR_afit_pBa{0xeVal{0x[237]   70000A02    //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
	{0x0F12,0x0A08, WORD_LEN, 0},	//2000	//TVAR_afit_pBa{0xeVal{0x[238]   70000A04    //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
	{0x0F12,0x0007, WORD_LEN, 0},	//0007	//TVAR_afit_pBa{0xeVal{0x[239]   70000A06    //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
	{0x0F12,0x0403, WORD_LEN, 0},	//0403	//TVAR_afit_pBa{0xeVal{0x[240]   70000A08    //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
	{0x0F12,0x0402, WORD_LEN, 0},	//0402	//TVAR_afit_pBa{0xeVal{0x[241]   70000A0A    //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[242]   70000A0C    //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
	{0x0F12,0x0203, WORD_LEN, 0},	//0203	//TVAR_afit_pBa{0xeVal{0x[243]   70000A0E    //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
	{0x0F12,0x0000, WORD_LEN, 0},	//0000	//TVAR_afit_pBa{0xeVal{0x[244]   70000A10    //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
	{0x0F12,0x0006, WORD_LEN, 0},	//1006	//TVAR_afit_pBa{0xeVal{0x[245]   70000A12    //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[246]   70000A14            //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[247]   70000A16            //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
	{0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[248]   70000A18            //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
	{0x0F12,0x803c, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[249]   70000A1A            //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[250]   70000A1C            //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[251]   70000A1E            //RGB2YUV_iYOff{0xet                                                          
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[252]   70000A20                //BRIGHTNE{0x{0x                                                                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[253]   70000A22                //CONTRA{0xT                                                                  
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[254]   70000A24                //{0xATURATION                                                                
	{0x0F12,0x0014, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[255]   70000A26                //{0xHARP_BLUR                                                                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[256]   70000A28                //GLAMOUR                                                                   
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[257]   70000A2A                //Di{0xparity_i{0xat{0xat                                                         
	{0x0F12,0x0008, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[258]   70000A2C                //Denoi{0xe1_iYDenThre{0xhLow                                                   
	{0x0F12,0x0006, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[259]   70000A2E                //Denoi{0xe1_iYDenThre{0xhLow_Bin                                               
	{0x0F12,0x0018, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[260]   70000A30                //Denoi{0xe1_iYDenThre{0xhHigh                                                  
	{0x0F12,0x0050, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[261]   70000A32                //Denoi{0xe1_iYDenThre{0xhHigh_Bin                                              
	{0x0F12,0x0002, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[262]   70000A34                //Denoi{0xe1_iLowWWideThre{0xh                                                  
	{0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[263]   70000A36                //Denoi{0xe1_iHighWWideThre{0xh                                                 
	{0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[264]   70000A38                //Denoi{0xe1_iLowWideThre{0xh                                                   
	{0x0F12,0x000A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[265]   70000A3A                //Denoi{0xe1_iHighWideThre{0xh                                                  
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[266]   70000A3C                //Denoi{0xe1_i{0xat{0xat                                                          
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[267]   70000A3E                //Demo{0xaic4_iHy{0xtGrayLow                                                    
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[268]   70000A40                //Demo{0xaic4_iHy{0xtGrayHigh                                                   
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[269]   70000A42                //UVDenoi{0xe_iYLowThre{0xh                                                     
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[270]   70000A44                //UVDenoi{0xe_iYHighThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[271]   70000A46                //UVDenoi{0xe_iUVLowThre{0xh                                                    
	{0x0F12,0x03FF, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[272]   70000A48                //UVDenoi{0xe_iUVHighThre{0xh                                                   
	{0x0F12,0x0014, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[273]   70000A4A                //D{0xMix1_iLowLimit_Wide                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[274]   70000A4C                //D{0xMix1_iLowLimit_Wide_Bin                                                 
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[275]   70000A4E                //D{0xMix1_iHighLimit_Wide                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[276]   70000A50                //D{0xMix1_iHighLimit_Wide_Bin                                                
	{0x0F12,0x0020, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[277]   70000A52                //D{0xMix1_iLowLimit_Fine                                                     
	{0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[278]   70000A54                //D{0xMix1_iLowLimit_Fine_Bin                                                 
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[279]   70000A56                //D{0xMix1_iHighLimit_Fine                                                    
	{0x0F12,0x0032, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[280]   70000A58                //D{0xMix1_iHighLimit_Fine_Bin                                                
	{0x0F12,0x0106, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[281]   70000A5A                //D{0xMix1_iRGBOff{0xet                                                         
	{0x0F12,0x006F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[282]   70000A5C                //D{0xMix1_iDemClamp                                                          
	{0x0F12,0x0202, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[283]   70000A5E                //"Di{0xparity_iDi{0xpTH_LowDi{0xparity_iDi{0xpTH_Low_Bin"                          
	{0x0F12,0x0502, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[284]   70000A60                //"Di{0xparity_iDi{0xpTH_High Di{0xparity_iDi{0xpTH_High_Bin"                       
	{0x0F12,0x0101, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[285]   70000A62                //"De{0xpeckle_iCorrectionLevelColdDe{0xpeckle_iCorrectionLevelCold_Bin"        
	{0x0F12,0x0202, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[286]   70000A64                //De{0xpeckle_iCorrectionLevelHotDe{0xpeckle_iCorrectionLevelHot_Bin            
	{0x0F12,0x140A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[287]   70000A66                //"De{0xpeckle_iColdThre{0xhLowDe{0xpeckle_iColdThre{0xhHigh"                       
	{0x0F12,0x140A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[288]   70000A68                //"De{0xpeckle_iHotThre{0xhLowDe{0xpeckle_iHotThre{0xhHigh"                         
	{0x0F12,0x2828, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[289]   70000A6A                //"Denoi{0xe1_iLowMax{0xlopeAllowedDenoi{0xe1_iHighMax{0xlopeAllowed"               
	{0x0F12,0x0606, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[290]   70000A6C                //"Denoi{0xe1_iLow{0xlopeThre{0xhDenoi{0xe1_iHigh{0xlopeThre{0xh"                       
	{0x0F12,0x0205, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[291]   70000A6E                //"Denoi{0xe1_iRadialPowerDenoi{0xe1_iRadialDivide{0xhift"                        
	{0x0F12,0x0880, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[292]   70000A70                //"Denoi{0xe1_iRadialLimitDenoi{0xe1_iLWBNoi{0xe"                                 
	{0x0F12,0x000F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[293]   70000A72                //"Denoi{0xe1_iWideDenoi{0xe1_iWideWide"                                        
	{0x0F12,0x0005, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[294]   70000A74                //"Demo{0xaic4_iHy{0xtGrayRangeUVDenoi{0xe_iY{0xupport"                             
	{0x0F12,0x1903, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[295]   70000A76                //"UVDenoi{0xe_iUV{0xupportD{0xMix1_iLowPower_Wide"                               
	{0x0F12,0x1611, WORD_LEN, 0}, //0f11 //1911	//TVAR_afit_pBa{0xeVal{0x[296]   70000A78                //"D{0xMix1_iLowPower_Wide_BinD{0xMix1_iHighPower_Wide"                         
	{0x0F12,0x0A0F, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[297]   70000A7A                //"D{0xMix1_iHighPower_Wide_BinD{0xMix1_iLowThre{0xh_Wide"                        
	{0x0F12,0x050A, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[298]   70000A7C                //"D{0xMix1_iHighThre{0xh_WideD{0xMix1_iReduceNegativeWide"                       
	{0x0F12,0x2020, WORD_LEN, 0}, //14 //20	//TVAR_afit_pBa{0xeVal{0x[299]   70000A7E                //"D{0xMix1_iLowPower_FineD{0xMix1_iLowPower_Fine_Bin"                          
	{0x0F12,0x2020, WORD_LEN, 0}, //1e //20	//TVAR_afit_pBa{0xeVal{0x[300]   70000A80                //"D{0xMix1_iHighPower_FineD{0xMix1_iHighPower_Fine_Bin"                        
	{0x0F12,0x0A08, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[301]   70000A82                //"D{0xMix1_iLowThre{0xh_FineD{0xMix1_iHighThre{0xh_Fine"                           
	{0x0F12,0x0007, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[302]   70000A84                //"D{0xMix1_iReduceNegativeFineD{0xMix1_iRGBMultiplier"                         
	{0x0F12,0x0408, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[303]   70000A86                //"Mixer1_iNLowNoi{0xePowerMixer1_iNLowNoi{0xePower_Bin"                        
	{0x0F12,0x0406, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[304]   70000A88                //"Mixer1_iNVeryLowNoi{0xePowerMixer1_iNVeryLowNoi{0xePower_Bin"                
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[305]   70000A8A                //"Mixer1_iNHighNoi{0xePowerMixer1_iNHighNoi{0xePower_Bin"                      
	{0x0F12,0x0608, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[306]   70000A8C                //"Mixer1_iWLowNoi{0xePowerMixer1_iWVeryLowNoi{0xePower"                        
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[307]   70000A8E                //"Mixer1_iWHighNoi{0xePowerMixer1_iWLowNoi{0xeCeilGain"                        
	{0x0F12,0x0006, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[308]   70000A90                //"Mixer1_iWHighNoi{0xeCeilGainMixer1_iWNoi{0xeCeilGain"                        
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[309]   70000A92 //180 173 164  //"CCM_O{0xcar_i{0xaturationCCM_O{0xcar_b{0xaturation"                              
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[310]   70000A94 //Linearity    //"RGBGamma2_iLinearityRGBGamma2_bLinearity"                                
	{0x0F12,0x0100, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[311]   70000A96                //"RGBGamma2_iDarkReduceRGBGamma2_bDarkReduce"                              
	{0x0F12,0x8040, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[312]   70000A98                //"byr_ga{0x2_i{0xhadingPowerRGB2YUV_iRGBGain"                                  
	{0x0F12,0x0180, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[313]   70000A9A                //"RGB2YUV_i{0xaturationRGB2YUV_bGainOff{0xet"                                  
	{0x0F12,0x0000, WORD_LEN, 0},	//TVAR_afit_pBa{0xeVal{0x[314]   70000A9C                //RGB2YUV_iYOff{0xet          
	{0x0F12,0x00FF, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[0]                            //Denoi{0xe1_iUVDenThre{0xhLow                             
	{0x0F12,0x00FF, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[1]                            //Denoi{0xe1_iUVDenThre{0xhHigh                            
	{0x0F12,0x0800, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[2]                            //Denoi{0xe1_{0xen{0xor_width                                
	{0x0F12,0x0600, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[3]                            //Denoi{0xe1_{0xen{0xor_height                               
	{0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[4]                            //Denoi{0xe1_{0xtart_x                                     
	{0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[5]                            //Denoi{0xe1_{0xtart_y                                     
	{0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[6]                            //"Denoi{0xe1_iYDen{0xmoothDenoi{0xe1_iW{0xharp  "             
	{0x0F12,0x0300, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[7]                            //"Denoi{0xe1_iWW{0xharp Denoi{0xe1_iRadialTune  "           
	{0x0F12,0x0002, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[8]                            //"Denoi{0xe1_iOutputBrightne{0x{0xDenoi{0xe1_binning_x  "     
	{0x0F12,0x0400, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[9]                            //"Denoi{0xe1_binning_yDemo{0xaic4_iFDeriv  "              
	{0x0F12,0x0106, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[10]                           //"Demo{0xaic4_iFDerivNeiDemo{0xaic4_i{0xDeriv  "            
	{0x0F12,0x0005, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[11]                           //"Demo{0xaic4_i{0xDerivNeiDemo{0xaic4_iEnhancerG  "         
	{0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[12]                           //"Demo{0xaic4_iEnhancerRBDemo{0xaic4_iEnhancerV  "        
	{0x0F12,0x0703, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[13]                           //"Demo{0xaic4_iDeci{0xionThre{0xhDemo{0xaic4_iDe{0xatThre{0xh"    
	{0x0F12,0x0000, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[14]                           //Demo{0xaic4_iBypa{0x{0x{0xelect                              
	{0x0F12,0xFFD6, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[15]
	{0x0F12,0x53C1, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[16]//hy{0x off : 4341
	{0x0F12,0xE1FE, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[17]//mixer on :E0FA
	{0x0F12,0x0001, WORD_LEN, 0},	//afit_pCon{0xtBa{0xeVal{0x[18]       
	{0x1000,0x0001, WORD_LEN, 0}, // 11. Flicker, CLK //  //{0xet ho{0xt interrupt {0xo main {0xtart run //
	{0xFFFF,0x000A, WORD_LEN, 0}, //W,0xait1},0m{0xec                           //
	{0x002A,0x0400, WORD_LEN, 0},
	{0x0F12,0x007F, WORD_LEN, 0},
	{0x002A,0x03DC, WORD_LEN, 0},
	{0x0F12,0x0002, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x01B8, WORD_LEN, 0},
	{0x0F12,0x5DC0, WORD_LEN, 0}, //MCLK
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x002A,0x01C6, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x01CC, WORD_LEN, 0},
	{0x0F12,0x1f40, WORD_LEN, 0},
	{0x0F12,0x2EE0, WORD_LEN, 0},
	{0x0F12,0x2EE0, WORD_LEN, 0},
	{0x0F12,0x1f40, WORD_LEN, 0},
	{0x0F12,0x2EE0, WORD_LEN, 0},
	{0x0F12,0x2EE0, WORD_LEN, 0},
	{0x002A,0x01E0, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0xFFFF,0x0064, WORD_LEN, 0}, 
	{0x002A,0x0242, WORD_LEN, 0}, //PREVIEW CONFIGURATION 0 (SXGA, YUV, 15fps)
	{0x0F12,0x0280, WORD_LEN, 0},	//1280
	{0x0F12,0x01e0, WORD_LEN, 0},	//200 1024
	{0x0F12,0x0005, WORD_LEN, 0},	//YUV
	{0x002A,0x024E, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x0248, WORD_LEN, 0},
	{0x0F12,0x2EE0, WORD_LEN, 0},
	{0x0F12,0x2EE0, WORD_LEN, 0},
	{0x0F12,0x0042, WORD_LEN, 0},
	{0x002A,0x0252, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x0250, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0}, 
	{0x002A,0x0262, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0}, //REG_0TC_PCFG_uPrevMirror   [0]x [1]y [2]xy                                 //
	{0x0F12,0x0000, WORD_LEN, 0}, //REG_0TC_PCFG_uCaptureMirror 
	{0x002A,0x0254, WORD_LEN, 0},
	{0x0F12,0x0535, WORD_LEN, 0},
	{0x0F12,0x029A, WORD_LEN, 0},       
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x0F12,0x0500, WORD_LEN, 0},	//1280 //PREVIEW CONFIGURATION 1 (SXGA, YUV, 15fps)
	{0x0F12,0x0400, WORD_LEN, 0},	//1024
	{0x0F12,0x0005, WORD_LEN, 0},	//YUV
	{0x002A,0x0274, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x026E, WORD_LEN, 0},
	{0x0F12,0x2EE0, WORD_LEN, 0},
	{0x0F12,0x2EE0, WORD_LEN, 0},
	{0x0F12,0x0042, WORD_LEN, 0},
	{0x002A,0x0278, WORD_LEN, 0},
	{0x0F12,0x0002, WORD_LEN, 0},
	{0x002A,0x0276, WORD_LEN, 0},
	{0x0F12,0x0002, WORD_LEN, 0},
	{0x002A,0x0288, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
	{0x0F12,0x0000, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
	{0x002A,0x027A, WORD_LEN, 0},
	{0x0F12,0x0535, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},       
	{0x002A,0x030E, WORD_LEN, 0}, //CAPTURE CONFIGURATION 0                         
	{0x0F12,0x0500, WORD_LEN, 0},  //REG_0TC_CCFG_u{0xWidth	//1280
	{0x0F12,0x03c0, WORD_LEN, 0},  //400 REG_0TC_CCFG_u{0xHeight	//960
	{0x0F12,0x0005, WORD_LEN, 0},  //REG_0TC_PCFG_Format	//YUV
	{0x002A,0x031A, WORD_LEN, 0},                        
	{0x0F12,0x0001, WORD_LEN, 0},  //REG_0TC_CCFG_uClockInd
	{0x002A,0x0314, WORD_LEN, 0},
	{0x0F12,0x2EE0, WORD_LEN, 0}, //1770 //2200 //1800 //1770  //REG_0TC_CCFG_u{0xMaxOut4KHzRate
	{0x0F12,0x2EE0, WORD_LEN, 0}, //1770 //2200 //1800 //1770  //REG_0TC_CCFG_u{0xMinOut4KHzRate
	{0x0F12,0x0042, WORD_LEN, 0},  //REG_0TC_CCFG_PVIMa{0xk
	{0x002A,0x031E, WORD_LEN, 0},
	{0x0F12,0x0002, WORD_LEN, 0},  //REG_0TC_CCFG_FrRateQualityType
	{0x002A,0x031C, WORD_LEN, 0},
	{0x0F12,0x0002, WORD_LEN, 0},  //REG_0TC_CCFG_u{0xFrTimeType
	{0x002A,0x0320, WORD_LEN, 0},
	{0x0F12,0x0535, WORD_LEN, 0},  //REG_0TC_CCFG_u{0xMaxFrTimeM{0xecMult10	//Don't change!
	{0x0F12,0x0000, WORD_LEN, 0},  //REG_0TC_CCFG_u{0xMinFrTimeM{0xecMult10	//Don't change!       
	{0x002A,0x0226, WORD_LEN, 0},     //REG_TC_GP_CapConfigChanged             
	{0x0F12,0x0001, WORD_LEN, 0},   
	{0x002A,0x10EE, WORD_LEN, 0}, //{0xenHal_uMinCol{0xNoBin *FACTORY ONLY *No Delete//
	{0x0F12,0x097A, WORD_LEN, 0}, // REG_TC_GP_Input{0xChangeReque{0xt/REG_TC_GP_PrevConfigChanged/REG_TC_GP_CapConfigChanged //       
	{0x002A,0x03B6, WORD_LEN, 0}, //preview
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x002A,0x03FA, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x0F12,0x00C3, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x021C, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x002A,0x0220, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x01F8, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x021E, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x01F0, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},       
	{0x002A,0x020A, WORD_LEN, 0}, // change InPut //
	{0x0F12,0x0500, WORD_LEN, 0}, //REG_TC_GP_PrevZoomReqInputWidth     //
	{0x0F12,0x03c0, WORD_LEN, 0}, //REG_TC_GP_PrevZoomReqInputHeight    //
	{0x0F12,0x0000, WORD_LEN, 0}, //REG_TC_GP_PrevZoomReqInputWidthOf{0x  //
	{0x0F12,0x0020, WORD_LEN, 0}, //REG_TC_GP_PrevZoomReqInputHeightOf{0x //
	{0x0F12,0x0500, WORD_LEN, 0}, //REG_TC_GP_CapZoomReqInputWidth      //
	{0x0F12,0x03C0, WORD_LEN, 0}, //REG_TC_GP_CapZoomReqInputHeight     //
	{0x0F12,0x0000, WORD_LEN, 0}, //REG_TC_GP_CapZoomReqInputWidthOf{0x   //
	{0x0F12,0x0020, WORD_LEN, 0}, //REG_TC_GP_CapZoomReqInputHeightOf{0x  //       
	{0x0F12,0x0001, WORD_LEN, 0}, //REG_TC_GP_Input{0xChangeReque{0xt//       
	{0xFFFF,0x0064, WORD_LEN, 0},         
	{0x0028,0xD000, WORD_LEN, 0},
	{0x002A,0xB0CC, WORD_LEN, 0},
	{0x0F12,0x000B, WORD_LEN, 0}
#endif
#endif
};

#ifdef BURST_MODE_INIT
static /*const*/ unsigned char s5k6aafx13_burst_cfg_1[] = 
{
    0x0F,0x12,
    0xB5, 0x70, 0x49, 0x36, 0x48, 0x36, 0x22, 0x05, 0xF0, 0x00, 0xFA, 0x4E, 0x49, 0x35, 0x20, 0x02, 
    0x83, 0xC8, 0x20, 0x01, 0x31, 0x20, 0x80, 0x88, 0x49, 0x33, 0x02, 0x00, 0x80, 0x08, 0x49, 0x33, 
    0x80, 0x48, 0x49, 0x33, 0x48, 0x33, 0x22, 0x04, 0xF0, 0x00, 0xFA, 0x3E, 0x49, 0x32, 0x48, 0x33, 
    0x22, 0x06, 0xF0, 0x00, 0xFA, 0x39, 0x49, 0x32, 0x48, 0x32, 0x22, 0x07, 0xF0, 0x00, 0xFA, 0x34, 
    0x49, 0x31, 0x48, 0x32, 0x22, 0x08, 0xF0, 0x00, 0xFA, 0x2F, 0x49, 0x31, 0x48, 0x31, 0x22, 0x09, 
    0xF0, 0x00, 0xFA, 0x2A, 0x49, 0x30, 0x48, 0x31, 0x22, 0x0A, 0xF0, 0x00, 0xFA, 0x25, 0x49, 0x30, 
    0x48, 0x30, 0x22, 0x0B, 0xF0, 0x00, 0xFA, 0x20, 0x48, 0x2F, 0x49, 0x30, 0x61, 0x08, 0x48, 0x30, 
    0x39, 0xFF, 0x39, 0x01, 0x67, 0x48, 0x48, 0x2F, 0x1C, 0x0A, 0x32, 0xC0, 0x63, 0x90, 0x48, 0x2E, 
    0x67, 0x08, 0x49, 0x1A, 0x48, 0x2D, 0x31, 0x08, 0x60, 0xC1, 0x68, 0x82, 0x1A, 0x51, 0x82, 0x01, 
    0x4C, 0x2B, 0x26, 0x07, 0x68, 0x21, 0x07, 0x36, 0x42, 0xB1, 0xDA, 0x05, 0x48, 0x29, 0x22, 0xD8, 
    0x1C, 0x05, 0xF0, 0x00, 0xFA, 0x09, 0x60, 0x25, 0x68, 0xA1, 0x42, 0xB1, 0xDA, 0x07, 0x48, 0x25, 
    0x22, 0x24, 0x38, 0x24, 0xF0, 0x00, 0xFA, 0x00, 0x48, 0x22, 0x38, 0x24, 0x60, 0xA0, 0x4D, 0x22, 
    0x6D, 0x29, 0x42, 0xB1, 0xDA, 0x07, 0x48, 0x1F, 0x22, 0x8F, 0x00, 0xD2, 0x30, 0xD8, 0x1C, 0x04, 
    0xF0, 0x00, 0xF9, 0xF2, 0x65, 0x2C, 0xBC, 0x70, 0xBC, 0x08, 0x47, 0x18, 0x21, 0x8B, 0x70, 0x00, 
    0x12, 0x7B, 0x00, 0x00, 0x03, 0x98, 0x70, 0x00, 0x13, 0x76, 0x70, 0x00, 0x23, 0x70, 0x70, 0x00, 
    0x1F, 0x0D, 0x70, 0x00, 0x89, 0x0D, 0x00, 0x00, 0x1F, 0x2F, 0x70, 0x00, 0x27, 0xA9, 0x00, 0x00, 
    0x1F, 0xE1, 0x70, 0x00, 0x27, 0xC5, 0x00, 0x00, 0x20, 0x43, 0x70, 0x00, 0x28, 0x5F, 0x00, 0x00, 
    0x20, 0x03, 0x70, 0x00, 0x28, 0xFF, 0x00, 0x00, 0x20, 0xCD, 0x70, 0x00, 0x61, 0x81, 0x00, 0x00, 
    0x20, 0xEF, 0x70, 0x00, 0x66, 0x63, 0x00, 0x00, 0x21, 0x23, 0x70, 0x00, 0x01, 0x00, 0x70, 0x00, 
    0x1E, 0xC1, 0x70, 0x00, 0x1E, 0xAD, 0x70, 0x00, 0x1F, 0x79, 0x70, 0x00, 0x04, 0xAC, 0x70, 0x00, 
    0x06, 0xCC, 0x70, 0x00, 0x23, 0xA4, 0x70, 0x00, 0x07, 0x04, 0x70, 0x00, 0xB5, 0x10, 0xF0, 0x00, 
    0xF9, 0xB9, 0x48, 0xC3, 0x49, 0xC3, 0x88, 0x00, 0x80, 0x48, 0xBC, 0x10, 0xBC, 0x08, 0x47, 0x18, 
    0xB5, 0xF8, 0x1C, 0x06, 0x4D, 0xC0, 0x68, 0xAC, 0x1C, 0x30, 0xF0, 0x00, 0xF9, 0xB3, 0x68, 0xA9, 
    0x4A, 0xBC, 0x42, 0xA1, 0xD0, 0x03, 0x4B, 0xBD, 0x8A, 0x1B, 0x33, 0x01, 0x80, 0x13, 0x88, 0x13, 
    0x1C, 0x14, 0x2B, 0x00, 0xD0, 0x0F, 0x22, 0x01, 0x42, 0x81, 0xD0, 0x03, 0x8C, 0x2F, 0x42, 0xB9, 
    0xD3, 0x00, 0x22, 0x00, 0x60, 0xAE, 0x2A, 0x00, 0xD0, 0x03, 0x8C, 0x28, 0x42, 0xB0, 0xD8, 0x00, 
    0x1C, 0x30, 0x1E, 0x59, 0x80, 0x21, 0xBC, 0xF8, 0xBC, 0x08, 0x47, 0x18, 0xB5, 0x10, 0x1C, 0x04, 
    0x48, 0xAF, 0xF0, 0x00, 0xF9, 0x97, 0x4A, 0xAD, 0x4B, 0xAE, 0x88, 0x11, 0x88, 0x5B, 0x88, 0x52, 
    0x43, 0x59, 0x18, 0x89, 0x42, 0x88, 0xD8, 0x00, 0x1C, 0x08, 0x60, 0x20, 0xE7, 0xC5, 0xB5, 0x70, 
    0x1C, 0x05, 0xF0, 0x00, 0xF9, 0x8F, 0x49, 0xA5, 0x89, 0x89, 0x43, 0x48, 0x02, 0x00, 0x0C, 0x00, 
    0x21, 0x01, 0x03, 0x49, 0xF0, 0x00, 0xF9, 0x8E, 0x1C, 0x04, 0x48, 0x9F, 0x8F, 0x80, 0xF0, 0x00, 
    0xF9, 0x91, 0x1C, 0x01, 0x20, 0xFF, 0x43, 0xC0, 0xF0, 0x00, 0xF9, 0x94, 0xF0, 0x00, 0xF9, 0x98, 
    0x1C, 0x01, 0x48, 0x98, 0x88, 0x40, 0x43, 0x60, 0x02, 0x00, 0x0C, 0x00, 0xF0, 0x00, 0xF9, 0x7A, 
    0x60, 0x28, 0xBC, 0x70, 0xBC, 0x08, 0x47, 0x18, 0xB5, 0xF1, 0xB0, 0x82, 0x4D, 0x96, 0x4E, 0x91, 
    0x88, 0xA8, 0x1C, 0x2C, 0x34, 0x20, 0x4F, 0x90, 0x28, 0x00, 0xD0, 0x18, 0xF0, 0x00, 0xF9, 0x88, 
    0x90, 0x01, 0x98, 0x02, 0x6B, 0x39, 0x02, 0x00, 0xF0, 0x00, 0xF9, 0x74, 0xF0, 0x00, 0xF9, 0x78, 
    0x99, 0x01, 0xF0, 0x00, 0xF9, 0x5F, 0x80, 0x20, 0x88, 0x71, 0x02, 0x00, 0xF0, 0x00, 0xF9, 0x6A, 
    0x04, 0x00, 0x0C, 0x00, 0x21, 0xFF, 0x31, 0x01, 0xF0, 0x00, 0xF9, 0x7A, 0x80, 0x20, 0x88, 0xE8, 
    0x28, 0x00, 0xD0, 0x0A, 0x49, 0x80, 0x88, 0x20, 0x31, 0x28, 0xF0, 0x00, 0xF9, 0x79, 0x8D, 0x38, 
    0x88, 0x71, 0x43, 0x48, 0x02, 0x00, 0x0C, 0x00, 0x85, 0x38, 0xBC, 0xFE, 0xBC, 0x08, 0x47, 0x18, 
    0xB5, 0x10, 0x1C, 0x04, 0xF0, 0x00, 0xF9, 0x74, 0x68, 0x21, 0x04, 0x09, 0x0C, 0x09, 0x1A, 0x40, 
    0x49, 0x76, 0x68, 0x49, 0x42, 0x81, 0xD8, 0x00, 0x1C, 0x08, 0xF0, 0x00, 0xF9, 0x71, 0x60, 0x20, 
    0xE7, 0x5B, 0xB5, 0x70, 0x68, 0x01, 0x04, 0x0D, 0x0C, 0x2D, 0x68, 0x44, 0x48, 0x6F, 0x89, 0x81, 
    0x1C, 0x28, 0xF0, 0x00, 0xF9, 0x27, 0x80, 0x60, 0x49, 0x70, 0x69, 0xC9, 0xF0, 0x00, 0xF9, 0x68, 
    0x1C, 0x01, 0x80, 0xA0, 0x02, 0x28, 0xF0, 0x00, 0xF9, 0x2D, 0x04, 0x00, 0x0C, 0x00, 0x80, 0x20, 
    0x49, 0x6B, 0x23, 0x00, 0x5E, 0xC9, 0x42, 0x88, 0xDA, 0x02, 0x20, 0xFF, 0x30, 0x01, 0x80, 0x20, 
    0xE7, 0x97, 0xB5, 0xF8, 0x1C, 0x04, 0x48, 0x67, 0x4E, 0x65, 0x78, 0x00, 0x6A, 0xB7, 0x28, 0x00, 
    0xD1, 0x00, 0x6A, 0x37, 0x49, 0x5D, 0x28, 0x00, 0x68, 0x8D, 0xD1, 0x00, 0x68, 0x4D, 0x48, 0x59, 
    0x88, 0x41, 0x68, 0x20, 0x02, 0x00, 0xF0, 0x00, 0xF9, 0x4B, 0x8D, 0xF1, 0x43, 0x4F, 0x0A, 0x3A, 
    0x42, 0x82, 0xD3, 0x0C, 0x4D, 0x5C, 0x26, 0xFF, 0x88, 0x29, 0x36, 0x01, 0x43, 0xB1, 0x80, 0x29, 
    0xF0, 0x00, 0xF9, 0x44, 0x60, 0x20, 0x88, 0x28, 0x43, 0x30, 0x80, 0x28, 0xE7, 0x3B, 0x1C, 0x0A, 
    0x43, 0x6A, 0x0A, 0x12, 0x42, 0x82, 0xD3, 0x04, 0x02, 0x00, 0xF0, 0x00, 0xF8, 0xF3, 0x60, 0x20, 
    0xE7, 0xF4, 0x60, 0x25, 0xE7, 0xF2, 0xB4, 0x10, 0x48, 0x48, 0x49, 0x50, 0x89, 0xC0, 0x23, 0x16, 
    0x5E, 0xCC, 0x1C, 0x02, 0x42, 0xA0, 0xDC, 0x00, 0x1C, 0x22, 0x82, 0xCA, 0x23, 0x18, 0x5E, 0xCA, 
    0x42, 0x90, 0xDC, 0x00, 0x1C, 0x10, 0x83, 0x08, 0xBC, 0x10, 0x47, 0x70, 0xB5, 0x70, 0x1C, 0x06, 
    0x4C, 0x45, 0x25, 0x01, 0x88, 0x20, 0x02, 0xAD, 0x43, 0xA8, 0x80, 0x20, 0xF0, 0x00, 0xF9, 0x1E, 
    0x60, 0x30, 0xF7, 0xFF, 0xFF, 0xE0, 0x88, 0x20, 0x43, 0x28, 0x80, 0x20, 0xE7, 0x41, 0xB5, 0x70, 
    0x4C, 0x3D, 0x25, 0x01, 0x88, 0x20, 0x02, 0xED, 0x43, 0xA8, 0x80, 0x20, 0xF0, 0x00, 0xF9, 0x16, 
    0xF7, 0xFF, 0xFF, 0xD1, 0x88, 0x20, 0x43, 0x28, 0x80, 0x20, 0xE7, 0x32, 0x23, 0x0D, 0x07, 0x1B, 
    0x18, 0xC3, 0x88, 0x18, 0x2A, 0x00, 0xD0, 0x01, 0x43, 0x08, 0xE0, 0x00, 0x43, 0x88, 0x80, 0x18, 
    0x47, 0x70, 0xB5, 0x70, 0x24, 0x02, 0x49, 0x32, 0x88, 0x09, 0x07, 0x8A, 0xD5, 0x00, 0x24, 0x06, 
    0x29, 0x00, 0xD0, 0x1F, 0x1C, 0x02, 0x20, 0x7D, 0x00, 0xC0, 0x26, 0x00, 0x4D, 0x2D, 0x2A, 0x00, 
    0xD0, 0x19, 0x21, 0x01, 0x82, 0x29, 0xF0, 0x00, 0xF8, 0xF9, 0x22, 0x00, 0x21, 0x01, 0x48, 0x2A, 
    0x03, 0x09, 0xF7, 0xFF, 0xFF, 0xDB, 0x20, 0x08, 0x43, 0x04, 0x1C, 0x21, 0x4C, 0x26, 0x22, 0x00, 
    0x3C, 0x14, 0x1C, 0x20, 0xF7, 0xFF, 0xFF, 0xD2, 0x22, 0x00, 0x21, 0x21, 0x1C, 0x20, 0xF7, 0xFF, 
    0xFF, 0xCD, 0x80, 0x2E, 0xE6, 0xFD, 0x82, 0x2E, 0x07, 0x89, 0x0F, 0xC9, 0x00, 0x89, 0x22, 0x3B, 
    0x43, 0x11, 0x80, 0x29, 0xF0, 0x00, 0xF8, 0xDA, 0xE7, 0xF4, 0xB5, 0x10, 0x49, 0x1B, 0x8F, 0xC8, 
    0x28, 0x00, 0xD0, 0x07, 0x20, 0x00, 0x87, 0xC8, 0x8F, 0x88, 0x4C, 0x19, 0x28, 0x00, 0xD0, 0x02, 
    0x20, 0x08, 0x80, 0x20, 0xE6, 0x89, 0x48, 0x15, 0x30, 0x60, 0x89, 0x00, 0x28, 0x00, 0xD1, 0x03, 
    0x48, 0x14, 0x21, 0x01, 0xF0, 0x00, 0xF8, 0xCA, 0x20, 0x10, 0x80, 0x20, 0xE7, 0xF2, 0x00, 0x00, 
    0x13, 0x76, 0x70, 0x00, 0x23, 0x70, 0x70, 0x00, 0x14, 0xD8, 0x70, 0x00, 0x23, 0x5C, 0x70, 0x00, 
    0xF4, 0xB0, 0x00, 0x00, 0x15, 0x54, 0x70, 0x00, 0x1A, 0xB8, 0x70, 0x00, 0x00, 0x80, 0x70, 0x00, 
    0x04, 0x6C, 0x70, 0x00, 0x04, 0x68, 0x70, 0x00, 0x11, 0x00, 0xD0, 0x00, 0x19, 0x8C, 0x70, 0x00, 
    0x0A, 0xC4, 0x70, 0x00, 0xB0, 0xA0, 0xD0, 0x00, 0xB0, 0xB4, 0x00, 0x00, 0x01, 0xB8, 0x70, 0x00, 
    0x04, 0x4E, 0x70, 0x00, 0x04, 0x50, 0x70, 0x00, 0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 
    0xFF, 0x1C, 0xE1, 0x2F, 0x9C, 0xE7, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 0xF0, 0x04, 0xE5, 0x1F, 
    0x9F, 0xB8, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 
    0x14, 0xC1, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 
    0x27, 0xE1, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 
    0x88, 0xDF, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 
    0x27, 0x5D, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 
    0x1E, 0xD3, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 
    0x27, 0xC5, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 0xF0, 0x04, 0xE5, 0x1F, 0xA1, 0x44, 0x00, 0x00, 
    0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x1F, 0x87, 0x00, 0x00, 
    0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x27, 0xA9, 0x00, 0x00, 
    0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x1E, 0xCB, 0x00, 0x00, 
    0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x28, 0xFF, 0x00, 0x00, 
    0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x26, 0xF9, 0x00, 0x00, 
    0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x40, 0x27, 0x00, 0x00, 
    0x47, 0x78, 0x46, 0xC0, 0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x9F, 0x03, 0x00, 0x00, 
    0x47, 0x78, 0x46, 0xC0, 0xF0, 0x04, 0xE5, 0x1F, 0x9D, 0x9C, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 
    0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x28, 0x5F, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 
    0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x61, 0x81, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 
    0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x66, 0x63, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 
    0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x85, 0xD9, 0x00, 0x00, 0x47, 0x78, 0x46, 0xC0, 
    0xC0, 0x00, 0xE5, 0x9F, 0xFF, 0x1C, 0xE1, 0x2F, 0x20, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0xE8, 0x48, 0x00, 0x01, 0xE8, 0x48, 0x00, 0x01, 0x05, 0x00, 0x00, 0x64, 0x00, 0x02, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

static unsigned char s5k6aafx13_burst_cfg_2[] = 
{
    0x0F,0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static unsigned char s5k6aafx13_burst_cfg_3[] = 
{
    0x0F,0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

static unsigned char s5k6aafx13_burst_cfg_4[] = 
{
    0x0F,0x12,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x04, 0x00, 0x04, 0x00, 0x05, 0x00, 0x05,
};

static unsigned char s5k6aafx13_burst_cfg_5[] = 
{
    0x0F,0x12,
    0x00, 0xC7, 0x00, 0xF7, 0x01, 0x07, 0x01, 0x42, 0x01, 0x7A, 0x01, 0xA0, 0x01, 0xB6, 0x01, 0x00, 
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00,
};

static unsigned char s5k6aafx13_burst_cfg_6[] = 
{
    0x0F,0x12,
    0x41, 0x0a, 0x00, 0x00, 0xA3, 0x16, 0x00, 0x00,
};

static unsigned char s5k6aafx13_burst_cfg_7[] = 
{
    0x0F,0x12,
    0x41, 0x0a, 0x00, 0x00, 0xA3, 0x16, 0x00, 0x00,
};

static unsigned char s5k6aafx13_burst_cfg_8[] = 
{
    0x0F,0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x03, 0x02, 0x02, 0x03, 0x01, 0x01, 0x01, 0x01, 0x04, 0x03, 0x03, 0x04, 0x01, 0x01, 
    0x01, 0x01, 0x04, 0x03, 0x03, 0x04, 0x01, 0x01, 0x01, 0x01, 0x03, 0x02, 0x02, 0x03, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
};

static unsigned char s5k6aafx13_burst_cfg_9[] = 
{
    0x0F,0x12,
    0x01, 0x2E, 0x01, 0x0A, 0x00, 0xCA, 0x00, 0x9B, 0x00, 0x7A, 0x00, 0x68, 0x00, 0x62, 0x00, 0x67, 
    0x00, 0x79, 0x00, 0x97, 0x00, 0xC6, 0x01, 0x06, 0x01, 0x34, 0x01, 0x11, 0x00, 0xD7, 0x00, 0x98, 
    0x00, 0x6C, 0x00, 0x52, 0x00, 0x42, 0x00, 0x3E, 0x00, 0x43, 0x00, 0x51, 0x00, 0x6A, 0x00, 0x94, 
    0x00, 0xD7, 0x01, 0x1D, 0x00, 0xEA, 0x00, 0xA8, 0x00, 0x6E, 0x00, 0x49, 0x00, 0x33, 0x00, 0x25, 
    0x00, 0x21, 0x00, 0x24, 0x00, 0x32, 0x00, 0x49, 0x00, 0x6E, 0x00, 0xAC, 0x00, 0xF7, 0x00, 0xCD, 
    0x00, 0x8C, 0x00, 0x56, 0x00, 0x36, 0x00, 0x20, 0x00, 0x12, 0x00, 0x0D, 0x00, 0x11, 0x00, 0x20, 
    0x00, 0x37, 0x00, 0x59, 0x00, 0x93, 0x00, 0xDD, 0x00, 0xBE, 0x00, 0x7E, 0x00, 0x4D, 0x00, 0x2C, 
    0x00, 0x16, 0x00, 0x07, 0x00, 0x01, 0x00, 0x06, 0x00, 0x16, 0x00, 0x2F, 0x00, 0x51, 0x00, 0x89, 
    0x00, 0xD2, 0x00, 0xC0, 0x00, 0x7F, 0x00, 0x4D, 0x00, 0x2D, 0x00, 0x16, 0x00, 0x06, 0x00, 0x00, 
    0x00, 0x05, 0x00, 0x16, 0x00, 0x30, 0x00, 0x53, 0x00, 0x8B, 0x00, 0xD3, 0x00, 0xD1, 0x00, 0x8E, 
    0x00, 0x59, 0x00, 0x37, 0x00, 0x1F, 0x00, 0x0F, 0x00, 0x09, 0x00, 0x0F, 0x00, 0x20, 0x00, 0x3A, 
    0x00, 0x5E, 0x00, 0x98, 0x00, 0xE1, 0x00, 0xF4, 0x00, 0xAC, 0x00, 0x70, 0x00, 0x4C, 0x00, 0x33, 
    0x00, 0x25, 0x00, 0x1E, 0x00, 0x23, 0x00, 0x33, 0x00, 0x4E, 0x00, 0x74, 0x00, 0xB2, 0x01, 0x00, 
    0x01, 0x27, 0x00, 0xD9, 0x00, 0x95, 0x00, 0x6B, 0x00, 0x50, 0x00, 0x41, 0x00, 0x3B, 0x00, 0x3F, 
    0x00, 0x4F, 0x00, 0x6B, 0x00, 0x97, 0x00, 0xDC, 0x01, 0x2F, 0x01, 0x62, 0x01, 0x1C, 0x00, 0xCF, 
    0x00, 0x9B, 0x00, 0x7A, 0x00, 0x69, 0x00, 0x64, 0x00, 0x69, 0x00, 0x7C, 0x00, 0x9B, 0x00, 0xCD, 
    0x01, 0x1B, 0x01, 0x68, 0x01, 0x86, 0x01, 0x5F, 0x01, 0x15, 0x00, 0xD9, 0x00, 0xB1, 0x00, 0x9B, 
    0x00, 0x95, 0x00, 0x9A, 0x00, 0xB1, 0x00, 0xDA, 0x01, 0x12, 0x01, 0x5F, 0x01, 0x8B, 0x01, 0x1D, 
    0x00, 0xF6, 0x00, 0xBA, 0x00, 0x8D, 0x00, 0x70, 0x00, 0x60, 0x00, 0x5A, 0x00, 0x5F, 0x00, 0x6D, 
    0x00, 0x89, 0x00, 0xB2, 0x00, 0xED, 0x01, 0x1B, 0x00, 0xFC, 0x00, 0xC2, 0x00, 0x88, 0x00, 0x62, 
    0x00, 0x4A, 0x00, 0x3D, 0x00, 0x39, 0x00, 0x3C, 0x00, 0x48, 0x00, 0x5F, 0x00, 0x83, 0x00, 0xC0, 
    0x01, 0x02, 0x00, 0xD5, 0x00, 0x95, 0x00, 0x62, 0x00, 0x41, 0x00, 0x2E, 0x00, 0x22, 0x00, 0x1E, 
    0x00, 0x22, 0x00, 0x2D, 0x00, 0x41, 0x00, 0x61, 0x00, 0x98, 0x00, 0xE0, 0x00, 0xB8, 0x00, 0x7A, 
    0x00, 0x4B, 0x00, 0x30, 0x00, 0x1C, 0x00, 0x10, 0x00, 0x0C, 0x00, 0x10, 0x00, 0x1D, 0x00, 0x31, 
    0x00, 0x4F, 0x00, 0x82, 0x00, 0xC6, 0x00, 0xAA, 0x00, 0x6D, 0x00, 0x42, 0x00, 0x26, 0x00, 0x13, 
    0x00, 0x06, 0x00, 0x01, 0x00, 0x06, 0x00, 0x14, 0x00, 0x2A, 0x00, 0x48, 0x00, 0x79, 0x00, 0xBC, 
    0x00, 0xAA, 0x00, 0x6E, 0x00, 0x42, 0x00, 0x26, 0x00, 0x12, 0x00, 0x05, 0x00, 0x00, 0x00, 0x05, 
    0x00, 0x15, 0x00, 0x2C, 0x00, 0x4B, 0x00, 0x7B, 0x00, 0xBE, 0x00, 0xB9, 0x00, 0x7A, 0x00, 0x4D, 
    0x00, 0x2F, 0x00, 0x1B, 0x00, 0x0D, 0x00, 0x09, 0x00, 0x0F, 0x00, 0x1F, 0x00, 0x36, 0x00, 0x56, 
    0x00, 0x87, 0x00, 0xCD, 0x00, 0xD8, 0x00, 0x95, 0x00, 0x61, 0x00, 0x43, 0x00, 0x2E, 0x00, 0x23, 
    0x00, 0x1E, 0x00, 0x23, 0x00, 0x32, 0x00, 0x49, 0x00, 0x6B, 0x00, 0xA1, 0x00, 0xEB, 0x01, 0x09, 
    0x00, 0xBE, 0x00, 0x82, 0x00, 0x5F, 0x00, 0x49, 0x00, 0x3E, 0x00, 0x3B, 0x00, 0x3F, 0x00, 0x4D, 
    0x00, 0x65, 0x00, 0x8B, 0x00, 0xC9, 0x01, 0x19, 0x01, 0x40, 0x00, 0xFC, 0x00, 0xB7, 0x00, 0x8A, 
    0x00, 0x70, 0x00, 0x63, 0x00, 0x60, 0x00, 0x65, 0x00, 0x76, 0x00, 0x92, 0x00, 0xBF, 0x01, 0x05, 
    0x01, 0x50, 0x01, 0x67, 0x01, 0x3E, 0x00, 0xFA, 0x00, 0xC3, 0x00, 0xA3, 0x00, 0x91, 0x00, 0x8E, 
    0x00, 0x93, 0x00, 0xA8, 0x00, 0xCC, 0x01, 0x00, 0x01, 0x47, 0x01, 0x75, 0x01, 0x2E, 0x01, 0x01, 
    0x00, 0xC1, 0x00, 0x90, 0x00, 0x70, 0x00, 0x5E, 0x00, 0x58, 0x00, 0x5F, 0x00, 0x72, 0x00, 0x93, 
    0x00, 0xC5, 0x01, 0x0C, 0x01, 0x41, 0x01, 0x10, 0x00, 0xD0, 0x00, 0x91, 0x00, 0x67, 0x00, 0x4C, 
    0x00, 0x3C, 0x00, 0x38, 0x00, 0x3D, 0x00, 0x4E, 0x00, 0x69, 0x00, 0x95, 0x00, 0xDB, 0x01, 0x27, 
    0x00, 0xEB, 0x00, 0xA4, 0x00, 0x6C, 0x00, 0x48, 0x00, 0x32, 0x00, 0x24, 0x00, 0x1F, 0x00, 0x24, 
    0x00, 0x33, 0x00, 0x4C, 0x00, 0x71, 0x00, 0xAF, 0x00, 0xFE, 0x00, 0xCD, 0x00, 0x8A, 0x00, 0x57, 
    0x00, 0x37, 0x00, 0x20, 0x00, 0x12, 0x00, 0x0D, 0x00, 0x12, 0x00, 0x22, 0x00, 0x3A, 0x00, 0x5C, 
    0x00, 0x94, 0x00, 0xDF, 0x00, 0xC0, 0x00, 0x7E, 0x00, 0x4F, 0x00, 0x2E, 0x00, 0x17, 0x00, 0x08, 
    0x00, 0x02, 0x00, 0x07, 0x00, 0x17, 0x00, 0x30, 0x00, 0x51, 0x00, 0x86, 0x00, 0xCD, 0x00, 0xC2, 
    0x00, 0x7F, 0x00, 0x4F, 0x00, 0x2F, 0x00, 0x17, 0x00, 0x07, 0x00, 0x00, 0x00, 0x05, 0x00, 0x14, 
    0x00, 0x2D, 0x00, 0x4E, 0x00, 0x81, 0x00, 0xC7, 0x00, 0xD2, 0x00, 0x8D, 0x00, 0x5A, 0x00, 0x38, 
    0x00, 0x20, 0x00, 0x0F, 0x00, 0x08, 0x00, 0x0C, 0x00, 0x1B, 0x00, 0x33, 0x00, 0x53, 0x00, 0x86, 
    0x00, 0xCD, 0x00, 0xF2, 0x00, 0xA7, 0x00, 0x6E, 0x00, 0x4B, 0x00, 0x32, 0x00, 0x23, 0x00, 0x1B, 
    0x00, 0x1D, 0x00, 0x2A, 0x00, 0x41, 0x00, 0x62, 0x00, 0x99, 0x00, 0xE0, 0x01, 0x24, 0x00, 0xD2, 
    0x00, 0x90, 0x00, 0x67, 0x00, 0x4D, 0x00, 0x3D, 0x00, 0x36, 0x00, 0x37, 0x00, 0x43, 0x00, 0x59, 
    0x00, 0x7E, 0x00, 0xBA, 0x01, 0x06, 0x01, 0x5A, 0x01, 0x10, 0x00, 0xC5, 0x00, 0x93, 0x00, 0x73, 
    0x00, 0x61, 0x00, 0x5B, 0x00, 0x5C, 0x00, 0x69, 0x00, 0x82, 0x00, 0xAC, 0x00, 0xEF, 0x01, 0x35, 
    0x01, 0x7F, 0x01, 0x54, 0x01, 0x0A, 0x00, 0xCE, 0x00, 0xA7, 0x00, 0x90, 0x00, 0x88, 0x00, 0x8A, 
    0x00, 0x9B, 0x00, 0xBA, 0x00, 0xEA, 0x01, 0x2D, 0x01, 0x55, 0x00, 0xC5, 0x00, 0xA7, 0x00, 0x7A, 
    0x00, 0x58, 0x00, 0x43, 0x00, 0x38, 0x00, 0x37, 0x00, 0x3E, 0x00, 0x4F, 0x00, 0x6B, 0x00, 0x94, 
    0x00, 0xCD, 0x00, 0xF8, 0x00, 0xAD, 0x00, 0x81, 0x00, 0x56, 0x00, 0x39, 0x00, 0x29, 0x00, 0x21, 
    0x00, 0x21, 0x00, 0x27, 0x00, 0x35, 0x00, 0x4C, 0x00, 0x71, 0x00, 0xA9, 0x00, 0xE5, 0x00, 0x8E, 
    0x00, 0x5E, 0x00, 0x38, 0x00, 0x22, 0x00, 0x17, 0x00, 0x10, 0x00, 0x11, 0x00, 0x17, 0x00, 0x23, 
    0x00, 0x36, 0x00, 0x54, 0x00, 0x86, 0x00, 0xC5, 0x00, 0x77, 0x00, 0x49, 0x00, 0x28, 0x00, 0x17, 
    0x00, 0x0C, 0x00, 0x06, 0x00, 0x06, 0x00, 0x0C, 0x00, 0x18, 0x00, 0x2A, 0x00, 0x43, 0x00, 0x6F, 
    0x00, 0xA9, 0x00, 0x6C, 0x00, 0x40, 0x00, 0x23, 0x00, 0x11, 0x00, 0x07, 0x00, 0x01, 0x00, 0x00, 
    0x00, 0x06, 0x00, 0x11, 0x00, 0x22, 0x00, 0x3A, 0x00, 0x61, 0x00, 0x99, 0x00, 0x6F, 0x00, 0x42, 
    0x00, 0x24, 0x00, 0x13, 0x00, 0x08, 0x00, 0x02, 0x00, 0x00, 0x00, 0x05, 0x00, 0x10, 0x00, 0x20, 
    0x00, 0x37, 0x00, 0x5C, 0x00, 0x92, 0x00, 0x7C, 0x00, 0x4E, 0x00, 0x2E, 0x00, 0x1C, 0x00, 0x10, 
    0x00, 0x09, 0x00, 0x07, 0x00, 0x0B, 0x00, 0x14, 0x00, 0x23, 0x00, 0x39, 0x00, 0x60, 0x00, 0x96, 
    0x00, 0x98, 0x00, 0x65, 0x00, 0x40, 0x00, 0x2C, 0x00, 0x20, 0x00, 0x1A, 0x00, 0x16, 0x00, 0x18, 
    0x00, 0x1F, 0x00, 0x2E, 0x00, 0x45, 0x00, 0x6E, 0x00, 0xA5, 0x00, 0xC1, 0x00, 0x89, 0x00, 0x5D, 
    0x00, 0x44, 0x00, 0x36, 0x00, 0x2E, 0x00, 0x2C, 0x00, 0x2C, 0x00, 0x33, 0x00, 0x41, 0x00, 0x5C, 
    0x00, 0x8A, 0x00, 0xC4, 0x00, 0xF2, 0x00, 0xBF, 0x00, 0x8A, 0x00, 0x69, 0x00, 0x56, 0x00, 0x4C, 
    0x00, 0x49, 0x00, 0x49, 0x00, 0x52, 0x00, 0x65, 0x00, 0x84, 0x00, 0xB7, 0x00, 0xEB, 0x01, 0x16, 
    0x00, 0xF7, 0x00, 0xC3, 0x00, 0x99, 0x00, 0x80, 0x00, 0x71, 0x00, 0x6D, 0x00, 0x6F, 0x00, 0x7B, 
    0x00, 0x93, 0x00, 0xB7, 0x00, 0xE5, 0x01, 0x05, 
};

static unsigned char s5k6aafx13_burst_cfg_10[] = 
{
    0x0F,0x12,
    0x00, 0xED, 0x01, 0x24, 0x01, 0x2B, 0x01, 0x4A, 0x01, 0x90, 0x01, 0xB2, 0x01, 0xC4, 0x01, 0x10, 
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x08, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x20, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x20, 
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x20, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x20, 
    0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 0x01, 0x20, 0x01, 0x00, 0x01, 0x00, 0x01, 0x00, 
};

static unsigned char s5k6aafx13_burst_cfg_11[] = 
{
    0x0F,0x12,
    0x00, 0x00, 0x02, 0x80, 0x02, 0x00, 0x00, 0x0E, 0x00, 0x0F, 
};

static unsigned char s5k6aafx13_burst_cfg_12[] = 
{
    0x0F,0x12,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x1a, 0x00, 0x68, 0x00, 0xfe, 0x01, 0x59, 0x01, 0xA1, 
    0x02, 0x10, 0x02, 0x63, 0x02, 0xD5, 0x03, 0x30, 0x03, 0x77, 0x03, 0xBE, 0x03, 0xF0, 0x04, 0x00, 
    0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x1a, 0x00, 0x68, 0x00, 0xfe, 0x01, 0x59, 0x01, 0xA1, 
    0x02, 0x10, 0x02, 0x63, 0x02, 0xD5, 0x03, 0x30, 0x03, 0x77, 0x03, 0xBE, 0x03, 0xF0, 0x04, 0x00, 
    0x00, 0x00, 0x00, 0x04, 0x00, 0x08, 0x00, 0x1a, 0x00, 0x68, 0x00, 0xfe, 0x01, 0x59, 0x01, 0xA1, 
    0x02, 0x10, 0x02, 0x63, 0x02, 0xD5, 0x03, 0x30, 0x03, 0x77, 0x03, 0xBE, 0x03, 0xF0, 0x04, 0x00, 
};

static unsigned char s5k6aafx13_burst_cfg_13[] = 
{
    0x0F,0x12,
    0x03, 0x9A, 0x03, 0xA8, 0x03, 0x7F, 0x03, 0xAA, 0x03, 0x5E, 0x03, 0x97, 0x03, 0x3E, 0x03, 0x88, 
    0x03, 0x22, 0x03, 0x78, 0x03, 0x0C, 0x03, 0x6C, 0x02, 0xF8, 0x03, 0x5A, 0x02, 0xDC, 0x03, 0x4C, 
    0x02, 0xC2, 0x03, 0x39, 0x02, 0xAE, 0x03, 0x27, 0x02, 0x9A, 0x03, 0x11, 0x02, 0x8C, 0x02, 0xFA, 
    0x02, 0x7C, 0x02, 0xE9, 0x02, 0x64, 0x02, 0xDA, 0x02, 0x52, 0x02, 0xCE, 0x02, 0x46, 0x02, 0xC4, 
    0x02, 0x3C, 0x02, 0xBB, 0x02, 0x2E, 0x02, 0xB0, 0x02, 0x22, 0x02, 0xA8, 0x02, 0x12, 0x02, 0x9C, 
    0x02, 0x02, 0x02, 0x92, 0x01, 0xFA, 0x02, 0x88, 0x01, 0xEC, 0x02, 0x7E, 0x01, 0xE6, 0x02, 0x72, 
    0x01, 0xDC, 0x02, 0x64, 0x01, 0xD4, 0x02, 0x56, 0x01, 0xCE, 0x02, 0x48, 0x01, 0xC6, 0x02, 0x3E, 
    0x01, 0xC0, 0x02, 0x2E, 0x01, 0xBE, 0x02, 0x22, 0x01, 0xC4, 0x02, 0x0E, 0x01, 0xD0, 0x01, 0xE0, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x04, 0x00, 0x00, 
};

static unsigned char s5k6aafx13_burst_cfg_14[] = 
{
    0x0F,0x12,
    0x04, 0x06, 0x04, 0x67, 0x03, 0x71, 0x04, 0xB0, 0x02, 0xE5, 0x04, 0x81, 0x02, 0x98, 0x04, 0x2E, 
    0x02, 0x60, 0x03, 0xDE, 0x02, 0x2F, 0x03, 0x91, 0x02, 0x01, 0x03, 0x4D, 0x01, 0xDA, 0x03, 0x10, 
    0x01, 0xB3, 0x02, 0xD4, 0x01, 0x8F, 0x02, 0x97, 0x01, 0x81, 0x02, 0x71, 0x01, 0x81, 0x02, 0x2A, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 
};

static unsigned char s5k6aafx13_burst_cfg_15[] = 
{
    0x0F,0x12,
    0x02, 0x74, 0x02, 0x86, 0x02, 0x62, 0x02, 0x8E, 0x02, 0x54, 0x02, 0x90, 0x02, 0x48, 0x02, 0x90, 
    0x02, 0x40, 0x02, 0x8E, 0x02, 0x34, 0x02, 0x8A, 0x02, 0x2A, 0x02, 0x88, 0x02, 0x1E, 0x02, 0x82, 
    0x02, 0x14, 0x02, 0x7E, 0x02, 0x0E, 0x02, 0x78, 0x02, 0x0A, 0x02, 0x74, 0x02, 0x04, 0x02, 0x6C, 
    0x02, 0x00, 0x02, 0x64, 0x01, 0xFE, 0x02, 0x5E, 0x01, 0xFE, 0x02, 0x58, 0x01, 0xFE, 0x02, 0x4C, 
    0x02, 0x00, 0x02, 0x38, 0x02, 0x04, 0x02, 0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 
};

static unsigned char s5k6aafx13_burst_cfg_16[] = 
{
    0x0F,0x12,
    0x03, 0x4A, 0x00, 0x00, 0x01, 0x76, 0x00, 0x00, 0x71, 0xB8, 0x00, 0x00,
};

static unsigned char s5k6aafx13_burst_cfg_17[] = 
{
    0x0F,0x12,
    0x0b, 0xb8, 0x00, 0x96, 0x0B, 0xB8,
};

static unsigned char s5k6aafx13_burst_cfg_18[] = 
{
    0x0F,0x12,
    0x00, 0x1B, 0x00, 0x0E, 0x00, 0x08, 0x00, 0x04,
};

static unsigned char s5k6aafx13_burst_cfg_19[] = 
{
    0x0F,0x12,
    0x05, 0x3C, 0x03, 0xb0, 0x05, 0x5C,
};

static unsigned char s5k6aafx13_burst_cfg_20[] = 
{
    0x0F,0x12,
    0x0E, 0xC8, 0x10, 0x22, 0x10, 0xBB, 0x10, 0xC9, 0x11, 0x49, 0x11, 0xFD, 0x00, 0xB8, 0x00, 0xB2,
};

static unsigned char s5k6aafx13_burst_cfg_21[] = 
{
    0x0F,0x12,
    0x02, 0x9A, 0x03, 0x44, 0x03, 0xFB,
};

static unsigned char s5k6aafx13_burst_cfg_22[] = 
{
    0x0F,0x12,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xff, 0xb8, 0xff, 0xb8, 0xff, 0xb8, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x10, 0xff, 0xb8, 0xff, 0xb8, 0xff, 0xb8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0xff, 0xb8, 
    0xff, 0xb8, 0xff, 0xb8, 0xFF, 0xF0, 0xFF, 0xD0, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 
    0xFF, 0xF0, 0xFF, 0xD0, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 0xFF, 0xF0, 0xFF, 0xD0, 
    0x00, 0x50, 0x00, 0x50, 0x00, 0x50, 0x00, 0x50,
};

static unsigned char s5k6aafx13_burst_cfg_23[] = 
{
    0x0F,0x12,
    0x23, 0xA4, 0x70, 0x00, 0x23, 0xC8, 0x70, 0x00, 0x23, 0xEC, 0x70, 0x00, 0x24, 0x10, 0x70, 0x00, 
    0x24, 0x34, 0x70, 0x00, 0x24, 0x58, 0x70, 0x00,
};

static unsigned char s5k6aafx13_burst_cfg_24[] = 
{
    0x0F,0x12,
    0x00, 0xBF, 0x00, 0xE6, 0x01, 0x04, 0x01, 0x30, 0x01, 0x70, 0x01, 0xA3,
};

static unsigned char s5k6aafx13_burst_cfg_25[] = 
{
    0x0F,0x12,
    0x01, 0xDC, 0xFF, 0xC4, 0x00, 0x00, 0x01, 0x14, 0x00, 0xF0, 0xFF, 0x10, 0xFF, 0x48, 0x01, 0xB9, 
    0xFF, 0x67, 0xFF, 0x19, 0x01, 0x57, 0x00, 0xF0, 0xFF, 0xD3, 0xFF, 0xD4, 0x01, 0x54, 0x01, 0x7D, 
    0xFF, 0x88, 0x01, 0x94, 0x01, 0xDC, 0xFF, 0xC4, 0x00, 0x00, 0x01, 0x14, 0x00, 0xF0, 0xFF, 0x10, 
    0xFF, 0x48, 0x01, 0xB9, 0xFF, 0x67, 0xFF, 0x19, 0x01, 0x57, 0x00, 0xF0, 0xFF, 0xD3, 0xFF, 0xD4, 
    0x01, 0x54, 0x01, 0x7D, 0xFF, 0x88, 0x01, 0x94, 0x01, 0xDC, 0xFF, 0xC4, 0x00, 0x00, 0x01, 0x14, 
    0x00, 0xF0, 0xFF, 0x10, 0xFF, 0x48, 0x01, 0xB9, 0xFF, 0x67, 0xFF, 0x19, 0x01, 0x57, 0x00, 0xF0, 
    0xFF, 0xD3, 0xFF, 0xD4, 0x01, 0x54, 0x01, 0x7D, 0xFF, 0x88, 0x01, 0x94, 0x01, 0xF8, 0xFF, 0xB7, 
    0xFF, 0xF6, 0x01, 0x23, 0x00, 0xC4, 0xFF, 0x26, 0xFF, 0x32, 0x01, 0x9C, 0xFF, 0x89, 0xFF, 0x1A, 
    0x01, 0x1D, 0x01, 0x21, 0xFF, 0xEE, 0xFF, 0xC8, 0x01, 0x4C, 0x01, 0xC0, 0xFF, 0x7B, 0x01, 0x6C, 
    0x01, 0xD7, 0xFF, 0xBD, 0x00, 0x0E, 0x00, 0xCC, 0x00, 0xF4, 0xFF, 0x4D, 0xFF, 0x68, 0x01, 0xAD, 
    0xFF, 0x45, 0xFF, 0x1B, 0x01, 0x1E, 0x01, 0x21, 0xFF, 0xEB, 0xFF, 0xC6, 0x01, 0x4F, 0x01, 0xBF, 
    0xFF, 0x7A, 0x01, 0x6B, 0x01, 0xD7, 0xFF, 0xBD, 0x00, 0x0E, 0x00, 0xCC, 0x00, 0xF4, 0xFF, 0x4D, 
    0xFF, 0x68, 0x01, 0xAD, 0xFF, 0x45, 0xFF, 0x1B, 0x01, 0x1E, 0x01, 0x21, 0xFF, 0xEB, 0xFF, 0xC6, 
    0x01, 0x4F, 0x01, 0xBF, 0xFF, 0x7A, 0x01, 0x6B, 
};

static unsigned char s5k6aafx13_burst_cfg_26[] = 
{
    0x0F,0x12,
    0x01, 0xB7, 0xFF, 0xBC, 0x00, 0x11, 0x00, 0xFC, 0x01, 0x3B, 0xFF, 0x84, 0xFE, 0xD9, 0x01, 0xE6, 
    0xFF, 0x16, 0xFF, 0x4A, 0x01, 0x79, 0x01, 0x4F, 0xFF, 0xC2, 0xFF, 0x99, 0x02, 0x19, 0x01, 0x72, 
    0xFF, 0x51, 0x01, 0x9B, 
};

static unsigned char s5k6aafx13_burst_cfg_27[] = 
{
    0x0F,0x12,
    0x00, 0x16, 0x00, 0x28, 0x00, 0x96, 0x01, 0xF4, 0x07, 0xD0,
};

static unsigned char s5k6aafx13_burst_cfg_28[] = 
{
    0x0F,0x12,
    0x00, 0x30, 0x00, 0x46, 0x00, 0x88, 0x02, 0x05, 0x02, 0xBC,
};

static unsigned char s5k6aafx13_burst_cfg_29[] = 
{
    0x0F,0x12,
    0x00, 0x10, 0x00, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x61, 0x00, 0x30, 
    0x00, 0x61, 0x00, 0xFF, 0x01, 0x29, 0x00, 0x0A, 0x00, 0x28, 0x00, 0x28, 0x03, 0xFF, 0x03, 0xFF, 
    0x00, 0x00, 0x03, 0x44, 0x03, 0x3A, 0x03, 0xFF, 0x03, 0xFF, 0x00, 0x0A, 0x00, 0x32, 0x00, 0x1E, 
    0x00, 0x32, 0x00, 0x32, 0x00, 0x32, 0x00, 0x10, 0x00, 0x32, 0x01, 0x06, 0x00, 0x6F, 0x0C, 0x0F, 
    0x0C, 0x0F, 0x03, 0x03, 0x03, 0x03, 0x14, 0x0A, 0x14, 0x0A, 0x28, 0x28, 0x00, 0x00, 0x02, 0x0A, 
    0x04, 0x80, 0x0E, 0x08, 0x03, 0xFF, 0x0A, 0x03, 0x0A, 0x11, 0x00, 0x0F, 0x05, 0x00, 0x09, 0x00, 
    0x00, 0x12, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 
    0x00, 0x0A, 0x01, 0x4C, 0x01, 0x4D, 0x01, 0x00, 0x80, 0x30, 0x01, 0x80, 0x00, 0x18, 0x00, 0x00, 
    0x00, 0x28, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x20, 0x00, 0x0E, 0x00, 0x50, 
    0x00, 0xFF, 0x01, 0x29, 0x00, 0x0A, 0x00, 0x28, 0x00, 0x28, 0x03, 0xFF, 0x03, 0xFF, 0x00, 0x00, 
    0x01, 0x14, 0x02, 0x0A, 0x03, 0xFF, 0x03, 0xFF, 0x00, 0x18, 0x00, 0x32, 0x00, 0x0A, 0x00, 0x32, 
    0x00, 0x28, 0x00, 0x32, 0x00, 0x10, 0x00, 0x32, 0x01, 0x06, 0x00, 0x6F, 0x05, 0x0F, 0x0A, 0x0F, 
    0x02, 0x03, 0x03, 0x03, 0x14, 0x0A, 0x14, 0x0A, 0x28, 0x28, 0x00, 0x00, 0x02, 0x0A, 0x04, 0x80, 
    0x0E, 0x08, 0x03, 0x0A, 0x14, 0x03, 0x0A, 0x11, 0x0A, 0x0F, 0x05, 0x0A, 0x10, 0x14, 0x10, 0x1E, 
    0x0A, 0x08, 0x00, 0x05, 0x04, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0A, 0x00, 0x00, 0x0A, 
    0x01, 0x80, 0x01, 0x54, 0x01, 0x00, 0x80, 0x30, 0x01, 0x80, 0x00, 0x10, 0x00, 0x00, 0x00, 0x24, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x0E, 0x00, 0x06, 0x00, 0x50, 0x00, 0x50, 
    0x00, 0x02, 0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x03, 0xFF, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x14, 
    0x00, 0x0A, 0x03, 0xFF, 0x03, 0xFF, 0x00, 0x1C, 0x00, 0x32, 0x00, 0x0A, 0x00, 0x32, 0x00, 0x28, 
    0x00, 0x32, 0x00, 0x10, 0x00, 0x32, 0x01, 0x06, 0x00, 0x6F, 0x02, 0x05, 0x05, 0x05, 0x01, 0x01, 
    0x02, 0x02, 0x14, 0x0A, 0x14, 0x0A, 0x28, 0x28, 0x06, 0x06, 0x02, 0x05, 0x04, 0x80, 0x00, 0x0A, 
    0x00, 0x05, 0x19, 0x03, 0x0F, 0x11, 0x0A, 0x0F, 0x05, 0x0A, 0x20, 0x14, 0x20, 0x1E, 0x0A, 0x08, 
    0x00, 0x07, 0x04, 0x03, 0x04, 0x02, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x06, 0x01, 0x80, 
    0x01, 0x73, 0x01, 0x00, 0x80, 0x42, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 0x00, 0x0E, 0x00, 0x06, 0x00, 0x50, 0x00, 0x50, 0x00, 0x02, 
    0x00, 0x0A, 0x00, 0x0A, 0x00, 0x0A, 0x03, 0xFF, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x14, 0x00, 0x32, 
    0x03, 0xFF, 0x03, 0xFF, 0x00, 0x1C, 0x00, 0x32, 0x00, 0x0A, 0x00, 0x32, 0x00, 0x28, 0x00, 0x32, 
    0x00, 0x10, 0x00, 0x32, 0x01, 0x06, 0x00, 0x6F, 0x02, 0x05, 0x05, 0x05, 0x01, 0x01, 0x02, 0x02, 
    0x14, 0x0A, 0x14, 0x0A, 0x28, 0x28, 0x06, 0x06, 0x02, 0x05, 0x04, 0x80, 0x00, 0x0A, 0x00, 0x05, 
    0x19, 0x03, 0x0F, 0x11, 0x0A, 0x0F, 0x05, 0x0A, 0x20, 0x14, 0x20, 0x1E, 0x0A, 0x08, 0x00, 0x07, 
    0x04, 0x03, 0x04, 0x02, 0x00, 0x00, 0x02, 0x03, 0x00, 0x00, 0x00, 0x06, 0x01, 0x80, 0x01, 0x80, 
    0x01, 0x00, 0x80, 0x4c, 0x01, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 
    0x00, 0x00, 0x03, 0xFF, 0x00, 0x0e, 0x00, 0x06, 0x00, 0x20, 0x00, 0x50, 0x00, 0x02, 0x00, 0x0A, 
    0x00, 0x0A, 0x00, 0x0A, 0x03, 0xFF, 0x03, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xFF, 
    0x03, 0xFF, 0x00, 0x14, 0x00, 0x32, 0x00, 0x00, 0x00, 0x32, 0x00, 0x20, 0x00, 0x32, 0x00, 0x00, 
    0x00, 0x32, 0x01, 0x06, 0x00, 0x6F, 0x02, 0x02, 0x05, 0x02, 0x01, 0x01, 0x02, 0x02, 0x14, 0x0A, 
    0x14, 0x0A, 0x28, 0x28, 0x06, 0x06, 0x02, 0x05, 0x08, 0x80, 0x00, 0x0F, 0x00, 0x05, 0x19, 0x03, 
    0x0F, 0x11, 0x0A, 0x0F, 0x05, 0x0A, 0x20, 0x14, 0x20, 0x1E, 0x0A, 0x08, 0x00, 0x07, 0x04, 0x08, 
    0x04, 0x06, 0x00, 0x00, 0x06, 0x08, 0x00, 0x00, 0x00, 0x06, 0x01, 0x80, 0x01, 0x80, 0x01, 0x00, 
    0x80, 0x50, 0x01, 0x80, 0x00, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x08, 0x00, 0x06, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x02, 0x04, 0x00, 0x01, 0x06, 0x00, 0x05, 0x00, 0x00, 
    0x07, 0x03, 0x00, 0x00, 0xFF, 0xD6, 0x53, 0xC1, 0xE1, 0xFE, 0x00, 0x01, 
};

static unsigned char s5k6aafx13_burst_cfg_30[] = 
{
    0x0F,0x12,
    0x17, 0x70, 0x2E, 0xE0, 0x2E, 0xE0, 0x17, 0x70, 0x2E, 0xE0, 0x2E, 0xE0,
};

static unsigned char s5k6aafx13_burst_cfg_31[] = 
{
    0x0F,0x12,
    0x02, 0x80, 0x01, 0xE0, 0x00, 0x05,
};

static unsigned char s5k6aafx13_burst_cfg_32[] = 
{
    0x0F,0x12,
    0x2E, 0xE0, 0x2E, 0xE0, 0x00, 0x42,
};

static unsigned char s5k6aafx13_burst_cfg_33[] = 
{
    0x0F,0x12,
    0x02, 0x80, 0x01, 0xE0, 0x00, 0x05,
};

static unsigned char s5k6aafx13_burst_cfg_34[] = 
{
    0x0F,0x12,
    0x2E, 0xE0, 0x2E, 0xE0, 0x00, 0x42,
};

static unsigned char s5k6aafx13_burst_cfg_35[] = 
{
    0x0F,0x12,
    0x05, 0x00, 0x03, 0xc0, 0x00, 0x05,
};

static unsigned char s5k6aafx13_burst_cfg_36[] = 
{
    0x0F,0x12,
    0x2E, 0xE0, 0x2E, 0xE0, 0x00, 0x42,
};

static unsigned char s5k6aafx13_burst_cfg_37[] = 
{
    0x0F,0x12,
    0x00, 0x01, 0x00, 0xC3, 0x00, 0x01,
};

static unsigned char s5k6aafx13_burst_cfg_38[] = 
{
    0x0F,0x12,
    0x05, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x20, 0x05, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x20,
};

static unsigned char s5k6aafx13_burst_cfg_39[] = 
{
    0x0F, 0x12, 
    0x05, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x05, 0x00, 0x03, 0xC0, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x01,
};

#endif

#if 1
static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_preview_cfg[] = {
    {0xFCFC,0xD000, WORD_LEN, 0},
    {0x0028,0x7000, WORD_LEN, 0},
    {0x002A,0x10EE, WORD_LEN, 0}, //{0xenHal_uMinCol{0xNoBin *FACTORY ONLY *No Delete//
    {0x0F12,0x097A, WORD_LEN, 0}, // REG_TC_GP_Input{0xChangeReque{0xt/REG_TC_GP_PrevConfigChanged/REG_TC_GP_CapConfigChanged // 
    {0x002A,0x021C, WORD_LEN, 0},
    {0x0F12,0x0000, WORD_LEN, 0},
    {0x002A,0x0220, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x01F8, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x021E, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x002A,0x01F0, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 0},
    {0x0F12,0x0001, WORD_LEN, 150},
    //{0xFFFF,0x012c, WORD_LEN, 150}, //delay 300ms
};


static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_snapshot_cfg[] = {	
    {0xFCFC,0xD000, WORD_LEN, 0},
    {0x0028,0x7000, WORD_LEN, 0}, 
    {0x002A,0x10EE, WORD_LEN, 0},                                                       
    {0x0F12,0x0B45, WORD_LEN, 0},    //senHal_uMinColsNoBin *FACTORY ONLY *No Delete   
    {0x002A,0x0224, WORD_LEN, 0},                                   
    {0x0F12,0x0000, WORD_LEN, 0},   //REG_TC_GP_ActiveCapConfig       
    {0x002A,0x01F8, WORD_LEN, 0},                                   
    {0x0F12,0x0001, WORD_LEN, 0}, 	//REG_TC_GP_NewConfigSync       
    {0x002A,0x0226, WORD_LEN, 0},                                   
    {0x0F12,0x0001, WORD_LEN, 0},   //REG_TC_GP_CapConfigChanged      
    {0x002A,0x01F4, WORD_LEN, 0},                                   
    {0x0F12,0x0001, WORD_LEN, 0},   //REG_TC_GP_EnableCapture         
    {0x0F12,0x0001, WORD_LEN, 150},	//REG_TC_GP_EnableCaptureChanged  
    //{0xFFFF,0x0096, WORD_LEN, 150}, //DELAY 150ms
};
#else
static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_preview_cfg[] = {
	{0xFCFC,0xD000, WORD_LEN, 0},
	{0x0028,0x7000, WORD_LEN, 0},
	{0x002A,0x10EE, WORD_LEN, 0}, //senHal_uMinColsNoBin *FACTORY ONLY *No Delete//
	{0x0F12,0x097A, WORD_LEN, 0}, // REG_TC_GP_InputsChangeRequest/REG_TC_GP_PrevConfigChanged/REG_TC_GP_CapConfigChanged //
	{0x0FFF,0x0096, WORD_LEN, 150}, //p150
	{0x002A,0x03B6, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x002A,0x03FA, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x0F12,0x00C3, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x021C, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0},
	{0x002A,0x0220, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x01F8, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x021E, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x002A,0x01F0, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0},
	{0x0FFF,0x03E8, WORD_LEN, 1000}, //p1000
	{0x0028,0xD000, WORD_LEN, 0},
	{0x002A,0xB0CC, WORD_LEN, 0},
	{0x0F12,0x000B, WORD_LEN, 0}
};


static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_snapshot_cfg[] = {	
	{0xFCFC,0xD000, WORD_LEN, 0},
	{0x0028,0x7000, WORD_LEN, 0}, 
	{0x002A,0x0224, WORD_LEN, 0},     //REG_TC_GP_ActiveCapConfig             
	{0x0F12,0x0000, WORD_LEN, 0}, 
	{0x002A,0x0226, WORD_LEN, 0},     //REG_TC_GP_CapConfigChanged             
	{0x0F12,0x0001, WORD_LEN, 0}, 
	{0x002A,0x01F8, WORD_LEN, 0},     //REG_TC_GP_NewConfigSync             
	{0x0F12,0x0001, WORD_LEN, 0}, 
	{0x0FFF,0x0096, WORD_LEN, 150}, //p150
	{0x002A,0x10EE, WORD_LEN, 0},
	{0x0F12,0x0D7A, WORD_LEN, 0},     //senHal_uMinColsNoBin *FACTORY ONLY *No Delete                  
	{0x002A,0x01F4, WORD_LEN, 0},     //REG_TC_GP_EnableCapture             
	{0x0F12,0x0001, WORD_LEN, 0},                  
	{0x0F12,0x0001, WORD_LEN, 0}, 
	{0x0FFF,0x03E8, WORD_LEN, 1000}, //p1000
	{0x0028,0xD000, WORD_LEN, 0},
	{0x002A,0xB0CC, WORD_LEN, 0},
	{0x0F12,0x000B, WORD_LEN, 0},
};
#endif

static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_arm_go_cfg[] = {	
	{0xFCFC,0xD000, WORD_LEN, 0},
	{0x0010,0x0001, WORD_LEN, 0}, // Reset
	{0x0004,0x0000, WORD_LEN, 0}, // Disable Auto Address Increment 
	{0x1030,0x0000, WORD_LEN, 0}, // Clear host interrupt so main will wait
	{0x0014,0x0001, WORD_LEN, 0} // ARM go
};

static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_standby_enter_cfg[] = {	
	{0xFCFC,0xD000, WORD_LEN, 0},
	{0x0028,0x7000, WORD_LEN, 0},
	{0x002A,0x022E, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0}, //sleep mode
	{0x0F12,0x0001, WORD_LEN, 0} //sleep mode changed
};

static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_standby_exit_cfg[] = {	
	{0xFCFC,0xD000, WORD_LEN, 0},
	{0x0028,0xD000, WORD_LEN, 0},
	{0x002A,0x003A, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0}, // Enable clocks
	{0x0028,0x7000, WORD_LEN, 0},
	{0x002A,0x022E, WORD_LEN, 0},
	{0x0F12,0x0000, WORD_LEN, 0}, //sleep mode
	{0x0F12,0x0001, WORD_LEN, 0}, //sleep mode changed
	{0x0028,0xD000, WORD_LEN, 0},
	{0x002A,0x1000, WORD_LEN, 0},
	{0x0F12,0x0001, WORD_LEN, 0} //Set host int
};

static const struct s5k6aafx13_i2c_reg_conf s5k6aafx13_preview_fps_settings_cfg[][S5K6AAFX13_CFG_PREVIEW_FPS_MAX_PARAMS]={
	{
		// 0fps(5~14.2 variable frame rate)
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150},
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
        {0, 0, ZERO_LEN, 0},
        {0, 0, ZERO_LEN, 0},
        {0, 0, ZERO_LEN, 0}
	},
	{
		// 1fps(dummy)
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		// 2fps(dummy)
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		// 3fps(dummy)
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		// 4fps(dummy)
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//5fps(dummy)
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//6fps(dummy)
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//7fps (include tunning value for skt VT call)
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x416E, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x416E, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x01E8, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0d00, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x0268, WORD_LEN, 0}, //42
        {0x0F12,0x0280, WORD_LEN, 0},   //1280
        {0x0F12,0x01E0, WORD_LEN, 0},   //1024
        {0x0F12,0x0005, WORD_LEN, 0},   //YUV
        {0x002A,0x0274, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},  //clock 1 사용
        {0x002A,0x026E, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x0042, WORD_LEN, 0},
        {0x002A,0x0278, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0}, //2
        {0x002A,0x0276, WORD_LEN, 0},
        {0x0F12,0x0002, WORD_LEN, 0},
        {0x002A,0x0288, WORD_LEN, 0},
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
        {0x002A,0x027A, WORD_LEN, 0},
        {0x0F12,0x0594, WORD_LEN, 0}, //535
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150}, 
	},		
	{
		//8fps
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x0268, WORD_LEN, 0}, //42
        {0x0F12,0x0280, WORD_LEN, 0},   //1280
        {0x0F12,0x01E0, WORD_LEN, 0},   //1024
        {0x0F12,0x0005, WORD_LEN, 0},   //YUV
        {0x002A,0x0274, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},  //clock 1 사용
        {0x002A,0x026E, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x0042, WORD_LEN, 0},
        {0x002A,0x0278, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0}, //2
        {0x002A,0x0276, WORD_LEN, 0},
        {0x0F12,0x0002, WORD_LEN, 0},
        {0x002A,0x0288, WORD_LEN, 0},
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
        {0x002A,0x027A, WORD_LEN, 0},
        {0x0F12,0x04e2, WORD_LEN, 0}, //8fps
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150},		
	},
	{
		//9fps
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x0268, WORD_LEN, 0}, //42
        {0x0F12,0x0280, WORD_LEN, 0},   //1280
        {0x0F12,0x01E0, WORD_LEN, 0},   //1024
        {0x0F12,0x0005, WORD_LEN, 0},   //YUV
        {0x002A,0x0274, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},  //clock 1 사용
        {0x002A,0x026E, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x0042, WORD_LEN, 0},
        {0x002A,0x0278, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0}, //2
        {0x002A,0x0276, WORD_LEN, 0},
        {0x0F12,0x0002, WORD_LEN, 0},
        {0x002A,0x0288, WORD_LEN, 0},
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
        {0x002A,0x027A, WORD_LEN, 0},
        {0x0F12,0x0457, WORD_LEN, 0}, //9fps
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150}, 
	},
	{
		//10fps
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x0268, WORD_LEN, 0}, //42
        {0x0F12,0x0280, WORD_LEN, 0},   //1280
        {0x0F12,0x01E0, WORD_LEN, 0},   //1024
        {0x0F12,0x0005, WORD_LEN, 0},   //YUV
        {0x002A,0x0274, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},  //clock 1 사용
        {0x002A,0x026E, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x0042, WORD_LEN, 0},
        {0x002A,0x0278, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0}, //2
        {0x002A,0x0276, WORD_LEN, 0},
        {0x0F12,0x0002, WORD_LEN, 0},
        {0x002A,0x0288, WORD_LEN, 0},
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
        {0x002A,0x027A, WORD_LEN, 0},
        {0x0F12,0x03e8, WORD_LEN, 0}, //10fps
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150},
	},
	{
		//11fps
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x0268, WORD_LEN, 0}, //42
        {0x0F12,0x0280, WORD_LEN, 0},   //1280
        {0x0F12,0x01E0, WORD_LEN, 0},   //1024
        {0x0F12,0x0005, WORD_LEN, 0},   //YUV
        {0x002A,0x0274, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},  //clock 1 사용
        {0x002A,0x026E, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x0042, WORD_LEN, 0},
        {0x002A,0x0278, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0}, //2
        {0x002A,0x0276, WORD_LEN, 0},
        {0x0F12,0x0002, WORD_LEN, 0},
        {0x002A,0x0288, WORD_LEN, 0},
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
        {0x002A,0x027A, WORD_LEN, 0},
        {0x0F12,0x038d, WORD_LEN, 0}, //11fps
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150}, 
	},
	{
		//12fps
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x0268, WORD_LEN, 0}, //42
        {0x0F12,0x0280, WORD_LEN, 0},   //1280
        {0x0F12,0x01E0, WORD_LEN, 0},   //1024
        {0x0F12,0x0005, WORD_LEN, 0},   //YUV
        {0x002A,0x0274, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},  //clock 1 사용
        {0x002A,0x026E, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x0042, WORD_LEN, 0},
        {0x002A,0x0278, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0}, //2
        {0x002A,0x0276, WORD_LEN, 0},
        {0x0F12,0x0002, WORD_LEN, 0},
        {0x002A,0x0288, WORD_LEN, 0},
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
        {0x002A,0x027A, WORD_LEN, 0},
        {0x0F12,0x0341, WORD_LEN, 0}, //12fps
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150}, 
	},
	{
		//13fps
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x0268, WORD_LEN, 0}, //42
        {0x0F12,0x0280, WORD_LEN, 0},   //1280
        {0x0F12,0x01E0, WORD_LEN, 0},   //1024
        {0x0F12,0x0005, WORD_LEN, 0},   //YUV
        {0x002A,0x0274, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},  //clock 1 사용
        {0x002A,0x026E, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x0042, WORD_LEN, 0},
        {0x002A,0x0278, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0}, //2
        {0x002A,0x0276, WORD_LEN, 0},
        {0x0F12,0x0002, WORD_LEN, 0},
        {0x002A,0x0288, WORD_LEN, 0},
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
        {0x002A,0x027A, WORD_LEN, 0},
        {0x0F12,0x0301, WORD_LEN, 0}, //13fps
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150},
	},
	{
		//14fps(actually max frame rate)
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x0268, WORD_LEN, 0}, //42
        {0x0F12,0x0280, WORD_LEN, 0},   //1280
        {0x0F12,0x01E0, WORD_LEN, 0},   //1024
        {0x0F12,0x0005, WORD_LEN, 0},   //YUV
        {0x002A,0x0274, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},  //clock 1 사용
        {0x002A,0x026E, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x2EE0, WORD_LEN, 0},
        {0x0F12,0x0042, WORD_LEN, 0},
        {0x002A,0x0278, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0}, //2
        {0x002A,0x0276, WORD_LEN, 0},
        {0x0F12,0x0002, WORD_LEN, 0},
        {0x002A,0x0288, WORD_LEN, 0},
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uPrevMirror                            //
        {0x0F12,0x0003, WORD_LEN, 0}, //REG_1TC_PCFG_uCaptureMirror  
        {0x002A,0x027A, WORD_LEN, 0},
        {0x0F12,0x02ca, WORD_LEN, 0}, //14fps
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150}, 
	},
	{
		//15fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//16fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//17fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//18fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}	
	},
	{
		//19fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//20fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//21fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//22fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//23fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//24fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//25fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//26fps		
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//27fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//28fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//29fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//30fps
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
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},		
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//31fps(5~14.2 variable frame rate)
        {0xFCFC,0xD000, WORD_LEN, 0}, 
        {0x0028,0x7000, WORD_LEN, 0},
        {0x002A,0x0488, WORD_LEN, 0},
        {0x0F12,0x410a, WORD_LEN, 0}, //416E,33.3m      lt_uMaxExp1        //
        {0x002A,0x0490, WORD_LEN, 0}, //lt_uCapMaxExp1                     //
        {0x0F12,0x410a, WORD_LEN, 0}, //416E, 33.3m                        //
        {0x002A,0x0498, WORD_LEN, 0},
        {0x0F12,0x0200, WORD_LEN, 0}, //01E8,     lt_uMaxAnGain1       700lux shutter       //
        {0x002A,0x2368, WORD_LEN, 0}, //217C                                              //
        {0x0F12,0x0690, WORD_LEN, 0}, //a0,  AWBBTune_EVT4_uMaxAnGain3                      //
        {0x002A,0x021C, WORD_LEN, 0},
        {0x0F12,0x0000, WORD_LEN, 0},
        {0x002A,0x0220, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F8, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x021E, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x002A,0x01F0, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 0},
        {0x0F12,0x0001, WORD_LEN, 150},
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
        {0, 0, ZERO_LEN, 0},
        {0, 0, ZERO_LEN, 0},
        {0, 0, ZERO_LEN, 0}
	}		
};

//S5K6AAFX13 EFFECT
//================================================================
static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_effect_settings_cfg[][S5K6AAFX13_CFG_EFFECT_MAX_PARAMS] = {
	{
		//none
		{ 0xFCFC, 0xD000, WORD_LEN, 0},
	    { 0x0028, 0x7000, WORD_LEN, 0},
	    { 0x002A, 0x01EE, WORD_LEN, 0},
	    { 0x0F12, 0x0000, WORD_LEN, 0},
	    { 0xFFFF, 0xFFFF, WORD_LEN, 0}
	},
	{
		//mono
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x01EE, WORD_LEN, 0},
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0xFFFF, 0xFFFF, WORD_LEN, 0}
	},
	{
		//negative
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x01EE, WORD_LEN, 0},
		{0x0F12, 0x0002, WORD_LEN, 0},
		{0xFFFF, 0xFFFF, WORD_LEN, 0}
	},		
	{
		//sola
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		//sepia
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x01EE, WORD_LEN, 0},
		{0x0F12, 0x0003, WORD_LEN, 0},
		{0xFFFF, 0xFFFF, WORD_LEN, 0}
	}
};

//S5K6AAFX13 WHITE BALANCE
//================================================================
static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_wb_settings_cfg[][S5K6AAFX13_CFG_WB_MAX_PARAMS] = { 
	{ 
		//auto
		{0xFCFC, 0xD000, WORD_LEN, 0}, 
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0400, WORD_LEN, 0},
		{0x0F12, 0x007F, WORD_LEN, 0},
		{0xFFFF, 0xFFFF, WORD_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{ 
		//custom(auto)
		{0xFCFC, 0xD000, WORD_LEN, 0}, 
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0400, WORD_LEN, 0},
		{0x0F12, 0x007F, WORD_LEN, 0},
		{0xFFFF, 0xFFFF, WORD_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
 	},
	{
		//incandescent
		{0xFCFC, 0xD000, WORD_LEN, 0}, 
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0400, WORD_LEN, 0},
		{0x0F12, 0x0077, WORD_LEN, 0}, //REG_TC_DBG_AutoAlgEnBits AWB OFF   //
		{0xFFFF, 0x008C, WORD_LEN, 140}, //140ms delay  //
		{0x002A, 0x03d0, WORD_LEN, 0},
		{0x0F12, 0x03A0, WORD_LEN, 0}, //380 3A0 3c0 3d0 400	REG_SF_USER_Rgain R Gain//
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0x002A, 0x03d4, WORD_LEN, 0},
		{0x0F12, 0x0400, WORD_LEN, 0}, //430 REG_SF_USER_Ggain G Gain   //
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0x002A, 0x03d8, WORD_LEN, 0},
		{0x0F12, 0x0990, WORD_LEN, 0}, //980 950 930 900	 REG_SF_USER_Bgain B Gain//
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0xFFFF, 0xFFFF, WORD_LEN, 0}
	},
	{
		//fluorescent
		{0xFCFC, 0xD000, WORD_LEN, 0}, 
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0400, WORD_LEN, 0},
		{0x0F12, 0x0077, WORD_LEN, 0}, //REG_TC_DBG_AutoAlgEnBits, AWB OFF//
		{0xFFFF, 0x008C, WORD_LEN, 140}, // 140ms delay  //
		{0x002A, 0x03d0, WORD_LEN, 0},
		{0x0F12, 0x0520, WORD_LEN, 0}, //5A0, REG_SF_USER_Rgain, R Gain   *//
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0x002A, 0x03d4, WORD_LEN, 0},
		{0x0F12, 0x0400, WORD_LEN, 0}, //REG_SF_USER_Ggain, G Gain  //
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0x002A, 0x03d8, WORD_LEN, 0},
		{0x0F12, 0x0840, WORD_LEN, 0}, //7C0, REG_SF_USER_Bgain, B Gain    //
		{0x0F12, 0x0001, WORD_LEN, 0},   
		{0xFFFF, 0xFFFF, WORD_LEN, 0}
	},
	{
		// daylight
		{0xFCFC, 0xD000, WORD_LEN, 0},  
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0400, WORD_LEN, 0},
		{0x0F12, 0x0077, WORD_LEN, 0}, //REG_TC_DBG_AutoAlgEnBits AWB OFF   //
		{0xFFFF, 0x008C, WORD_LEN, 140}, //140ms delay  //
		{0x002A, 0x03d0, WORD_LEN, 0},
		{0x0F12, 0x05D0, WORD_LEN, 0}, //5D8 REG_SF_USER_Rgain R Gain     //
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0x002A, 0x03d4, WORD_LEN, 0},
		{0x0F12, 0x0400, WORD_LEN, 0}, //REG_SF_USER_Ggain G Gain  //
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0x002A, 0x03d8, WORD_LEN, 0},
		{0x0F12, 0x0520, WORD_LEN, 0}, // 4E0 4C0	REG_SF_USER_Bgain B Gain     //
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0xFFFF, 0xFFFF, WORD_LEN, 0}
	},
	{
		//cloudy
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0400, WORD_LEN, 0},
		{0x0F12, 0x0077, WORD_LEN, 0}, //REG_TC_DBG_AutoAlgEnBits AWB OFF   //
		{0xFFFF, 0x008C, WORD_LEN, 140}, //140ms delay  //
		{0x002A, 0x03d0, WORD_LEN, 0},
		{0x0F12, 0x06e0, WORD_LEN, 0}, //790 REG_SF_USER_Rgain R Gain  //
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0x002A, 0x03d4, WORD_LEN, 0},
		{0x0F12, 0x0400, WORD_LEN, 0}, //REG_SF_USER_Ggain G Gain  //
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0x002A, 0x03d8, WORD_LEN, 0},
		{0x0F12, 0x04c0, WORD_LEN, 0}, //450 REG_SF_USER_Bgain B Gain     //
		{0x0F12, 0x0001, WORD_LEN, 0},
		{0xFFFF, 0xFFFF, WORD_LEN, 0}
	}
};


//S5K6AAFX13 BRIGHTNESS
//================================================================
static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_bright_settings_cfg[][S5K6AAFX13_CFG_BRIGHT_MAX_PARAMS] = {
	{
		//-4
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0f4c, WORD_LEN, 0},
		{0x0F12, 0x02B0, WORD_LEN, 0},
		{0x002A, 0x0f52, WORD_LEN, 0},
		{0x0F12, 0x02F0, WORD_LEN, 0},
		{0x002A, 0x01e4, WORD_LEN, 0},
		{0x0F12, 0xFF90, WORD_LEN, 0}
	},
	{
		//-3
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0f4c, WORD_LEN, 0},
		{0x0F12, 0x02B0, WORD_LEN, 0},
		{0x002A, 0x0f52, WORD_LEN, 0},
		{0x0F12, 0x02F0, WORD_LEN, 0},
		{0x002A, 0x01e4, WORD_LEN, 0},
		{0x0F12, 0xFFac, WORD_LEN, 0}
	},
	{
		//-2
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0f4c, WORD_LEN, 0},
		{0x0F12, 0x02B0, WORD_LEN, 0},
		{0x002A, 0x0f52, WORD_LEN, 0},
		{0x0F12, 0x02F0, WORD_LEN, 0},
		{0x002A, 0x01e4, WORD_LEN, 0},
		{0x0F12, 0xFFc8, WORD_LEN, 0}
	},
	{
		//-1
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0f4c, WORD_LEN, 0},
		{0x0F12, 0x02B0, WORD_LEN, 0},
		{0x002A, 0x0f52, WORD_LEN, 0},
		{0x0F12, 0x02F0, WORD_LEN, 0},
		{0x002A, 0x01e4, WORD_LEN, 0},
		{0x0F12, 0xFFe4, WORD_LEN, 0}
 	},
	{
		//0
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0f4c, WORD_LEN, 0},
		{0x0F12, 0x02B0, WORD_LEN, 0},
		{0x002A, 0x0f52, WORD_LEN, 0},
		{0x0F12, 0x02F0, WORD_LEN, 0},
		{0x002A, 0x01e4, WORD_LEN, 0},
		{0x0F12, 0x0000, WORD_LEN, 0}
 	},

	{
		// 1
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0f4c, WORD_LEN, 0},
		{0x0F12, 0x02B0, WORD_LEN, 0},
		{0x002A, 0x0f52, WORD_LEN, 0},
		{0x0F12, 0x02F0, WORD_LEN, 0},
		{0x002A, 0x01e4, WORD_LEN, 0},
		{0x0F12, 0x001f, WORD_LEN, 0}
	},

	{
		// 2
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0f4c, WORD_LEN, 0},
		{0x0F12, 0x02B0, WORD_LEN, 0},
		{0x002A, 0x0f52, WORD_LEN, 0},
		{0x0F12, 0x02F0, WORD_LEN, 0},
		{0x002A, 0x01e4, WORD_LEN, 0},
		{0x0F12, 0x003e, WORD_LEN, 0}
 	},

	{
		// 3
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0f4c, WORD_LEN, 0},
		{0x0F12, 0x02B0, WORD_LEN, 0},
		{0x002A, 0x0f52, WORD_LEN, 0},
		{0x0F12, 0x02F0, WORD_LEN, 0},
		{0x002A, 0x01e4, WORD_LEN, 0},
		{0x0F12, 0x005d, WORD_LEN, 0}
	},

	{
		// 4
		{0xFCFC, 0xD000, WORD_LEN, 0},
		{0x0028, 0x7000, WORD_LEN, 0},
		{0x002A, 0x0f4c, WORD_LEN, 0},
		{0x0F12, 0x02B0, WORD_LEN, 0},
		{0x002A, 0x0f52, WORD_LEN, 0},
		{0x0F12, 0x02F0, WORD_LEN, 0},
		{0x002A, 0x01e4, WORD_LEN, 0},
		{0x0F12, 0x007d, WORD_LEN, 0}
	}, 
};

//S5K6AAFX13 EXPOSURE
//================================================================
static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_exposure_settings_cfg[][S5K6AAFX13_CFG_EXPOSURE_MAX_PARAMS] = {

	{
		//normal
		{0xFCFC,0xD000, WORD_LEN, 0},    
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1c72, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0}, 
		{0x0F12,0x0000, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0302, WORD_LEN, 0}, 
		{0x0F12,0x0203, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0403, WORD_LEN, 0}, 
		{0x0F12,0x0304, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0403, WORD_LEN, 0}, 
		{0x0F12,0x0304, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0302, WORD_LEN, 0}, 
		{0x0F12,0x0203, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0101, WORD_LEN, 0}, 
		{0x0F12,0x0000, WORD_LEN, 0}, 
		{0x0F12,0x0000, WORD_LEN, 0}, 
		{0x0F12,0x0000, WORD_LEN, 0}
	},	
	{
		//average
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1c72, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0},
		{0x0F12,0x0101, WORD_LEN, 0}
	},
	{
		//spot
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1c72, WORD_LEN, 0}, 
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
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0F00, WORD_LEN, 0},
		{0x0F12,0x000F, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0F00, WORD_LEN, 0},
		{0x0F12,0x000F, WORD_LEN, 0},
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
		{0x0F12,0x0000, WORD_LEN, 0}	
	},
	{
		//spot
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x1c72, WORD_LEN, 0}, 
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
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0F00, WORD_LEN, 0},
		{0x0F12,0x000F, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x0F12,0x0F00, WORD_LEN, 0},
		{0x0F12,0x000F, WORD_LEN, 0},
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
		{0x0F12,0x0000, WORD_LEN, 0}
	}
};

static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_flicker_settings_cfg[][S5K6AAFX13_CFG_FLICKER_MAX_PARAMS] = {
	{
		/* S5K6AAFX13_Auto_FD */
		{0, 0, ZERO_LEN, 0}, 	
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		/* S5K6AAFX13_Manual_FD_60Hz */
		{0, 0, ZERO_LEN, 0}, 	
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		/* S5K6AAFX13_Manual_FD_50Hz */
		{0, 0, ZERO_LEN, 0}, 	
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	},
	{
		/* S5K6AAFX13_Auto_FD */
		{0, 0, ZERO_LEN, 0}, 	
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0},
		{0, 0, ZERO_LEN, 0}
	}
};

//S5K6AAFX13 REFLECT
//================================================================
static const struct s5k6aafx13_i2c_reg_conf const s5k6aafx13_reflect_settings_cfg[][S5K6AAFX13_CFG_REFLECT_MAX_PARAMS] = {

	{
		//NONE
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x0262, WORD_LEN, 0},
		{0x0F12,0x0003, WORD_LEN, 0},  //REG_0TC_PCFG_uPrevMirror   [0]x [1]y [2]xy
		{0x0F12,0x0003, WORD_LEN, 0},  //REG_0TC_PCFG_uCaptureMirror  
		{0x002A,0x021C, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x0220, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x01F8, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x021E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x01F0, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0}, 
		{0xffff,0x0064, WORD_LEN, 100}, //delay 150ms
		{0x0028,0xD000, WORD_LEN, 0}, //mipi
		{0x002A,0xB0CC, WORD_LEN, 0},
		{0x0F12,0x000B, WORD_LEN, 0} 
	}, 
	{ 
		//MIRROR, WORD_LEN, 0
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x0262, WORD_LEN, 0},
		{0x0F12,0x0002, WORD_LEN, 0},  //REG_0TC_PCFG_uPrevMirror   [0]x [1]y [2]xy
		{0x0F12,0x0002, WORD_LEN, 0},  //REG_0TC_PCFG_uCaptureMirror  
		{0x002A,0x021C, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x0220, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x01F8, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x021E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x01F0, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0}, 
		{0xffff,0x0064, WORD_LEN, 100}, //delay 150ms
		{0x0028,0xD000, WORD_LEN, 0}, //mipi
		{0x002A,0xB0CC, WORD_LEN, 0},
		{0x0F12,0x000B, WORD_LEN, 0} 
	},
	{
		//WATER
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x0262, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},  //REG_0TC_PCFG_uPrevMirror   [0]x [1]y [2]xy
		{0x0F12,0x0001, WORD_LEN, 0},  //REG_0TC_PCFG_uCaptureMirror  
		{0x002A,0x021C, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x0220, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x01F8, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x021E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x01F0, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0}, 
		{0xffff,0x0064, WORD_LEN, 100}, //delay 150ms
		{0x0028,0xD000, WORD_LEN, 0}, //mipi
		{0x002A,0xB0CC, WORD_LEN, 0},
		{0x0F12,0x000B, WORD_LEN, 0}
 	},
	{
		//MIRROR + WATER
		{0xFCFC,0xD000, WORD_LEN, 0},
		{0x0028,0x7000, WORD_LEN, 0},
		{0x002A,0x0262, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},  //REG_0TC_PCFG_uPrevMirror   [0]x [1]y [2]xy
		{0x0F12,0x0000, WORD_LEN, 0},  //REG_0TC_PCFG_uCaptureMirror  
		{0x002A,0x021C, WORD_LEN, 0},
		{0x0F12,0x0000, WORD_LEN, 0},
		{0x002A,0x0220, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x01F8, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x021E, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x002A,0x01F0, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0},
		{0x0F12,0x0001, WORD_LEN, 0}, 
		{0xffff,0x0064, WORD_LEN, 100}, //delay 150ms
		{0x0028,0xD000, WORD_LEN, 0}, //mipi
		{0x002A,0xB0CC, WORD_LEN, 0},
		{0x0F12,0x000B, WORD_LEN, 0}
 	}
};

struct s5k6aafx13_reg s5k6aafx13_regs = {
	.init_parm = &s5k6aafx13_init_parm[0],
	.init_parm_size = ARRAY_SIZE(s5k6aafx13_init_parm),
	.preview_cfg_settings = s5k6aafx13_preview_cfg,
	.preview_cfg_settings_size = ARRAY_SIZE(s5k6aafx13_preview_cfg),
	.snapshot_cfg_settings = s5k6aafx13_snapshot_cfg,
	.snapshot_cfg_settings_size = ARRAY_SIZE(s5k6aafx13_snapshot_cfg),

// inside arm processor go	
	.arm_go_cfg_settings = s5k6aafx13_arm_go_cfg,
	.arm_go_cfg_settings_size = ARRAY_SIZE(s5k6aafx13_arm_go_cfg),

// S/W standby enter		
	.standby_enter_cfg_settings = s5k6aafx13_standby_enter_cfg,
	.standby_enter_cfg_settings_size = ARRAY_SIZE(s5k6aafx13_standby_enter_cfg),

// S/W standby exit
	.standby_exit_cfg_settings = s5k6aafx13_standby_exit_cfg,
	.standby_exit_cfg_settings_size = ARRAY_SIZE(s5k6aafx13_standby_exit_cfg),

//preview frame rate
//	.preview_fps_cfg_settings = &s5k6aafx13_preview_fps_settings_cfg,
	.preview_fps_cfg_settings = s5k6aafx13_preview_fps_settings_cfg,
	.preview_fps_cfg_settings_size = S5K6AAFX13_CFG_PREVIEW_FPS_MAX_PARAMS,	
	
//effect
	.effect_cfg_settings = s5k6aafx13_effect_settings_cfg,
	.effect_cfg_settings_size = S5K6AAFX13_CFG_EFFECT_MAX_PARAMS,

//wb
	.wb_cfg_settings = s5k6aafx13_wb_settings_cfg,
	.wb_cfg_settings_size = S5K6AAFX13_CFG_WB_MAX_PARAMS,

//brightness	
	.bright_cfg_settings = s5k6aafx13_bright_settings_cfg,
	.bright_cfg_settings_size = S5K6AAFX13_CFG_BRIGHT_MAX_PARAMS,

//exposure
	.exposure_cfg_settings = s5k6aafx13_exposure_settings_cfg,
	.exposure_cfg_settings_size = S5K6AAFX13_CFG_EXPOSURE_MAX_PARAMS,

//flicker
	.flicker_cfg_settings = s5k6aafx13_flicker_settings_cfg,
	.flicker_cfg_settings_size = S5K6AAFX13_CFG_FLICKER_MAX_PARAMS,
//reflect	
       .reflect_cfg_settings = s5k6aafx13_reflect_settings_cfg,
	.reflect_cfg_settings_size = S5K6AAFX13_CFG_REFLECT_MAX_PARAMS,

#ifdef BURST_MODE_INIT
    .init_burst_settings_1 = s5k6aafx13_burst_cfg_1,
    .init_burst_settings_size_1 = ARRAY_SIZE(s5k6aafx13_burst_cfg_1),
    .init_burst_settings_2 = s5k6aafx13_burst_cfg_2,
    .init_burst_settings_size_2 = ARRAY_SIZE(s5k6aafx13_burst_cfg_2),
    .init_burst_settings_3 = s5k6aafx13_burst_cfg_3,
    .init_burst_settings_size_3 = ARRAY_SIZE(s5k6aafx13_burst_cfg_3),
    .init_burst_settings_4 = s5k6aafx13_burst_cfg_4,
    .init_burst_settings_size_4 = ARRAY_SIZE(s5k6aafx13_burst_cfg_4),
    .init_burst_settings_5 = s5k6aafx13_burst_cfg_5,
    .init_burst_settings_size_5 = ARRAY_SIZE(s5k6aafx13_burst_cfg_5),
    .init_burst_settings_6 = s5k6aafx13_burst_cfg_6,
    .init_burst_settings_size_6 = ARRAY_SIZE(s5k6aafx13_burst_cfg_6),
    .init_burst_settings_7 = s5k6aafx13_burst_cfg_7,
    .init_burst_settings_size_7 = ARRAY_SIZE(s5k6aafx13_burst_cfg_7),
    .init_burst_settings_8 = s5k6aafx13_burst_cfg_8,
    .init_burst_settings_size_8 = ARRAY_SIZE(s5k6aafx13_burst_cfg_8),
    .init_burst_settings_9 = s5k6aafx13_burst_cfg_9,
    .init_burst_settings_size_9 = ARRAY_SIZE(s5k6aafx13_burst_cfg_9),
    .init_burst_settings_10 = s5k6aafx13_burst_cfg_10,
    .init_burst_settings_size_10 = ARRAY_SIZE(s5k6aafx13_burst_cfg_10),

    .init_burst_settings_11 = s5k6aafx13_burst_cfg_11,
    .init_burst_settings_size_11 = ARRAY_SIZE(s5k6aafx13_burst_cfg_11),
    .init_burst_settings_12 = s5k6aafx13_burst_cfg_12,
    .init_burst_settings_size_12 = ARRAY_SIZE(s5k6aafx13_burst_cfg_12),
    .init_burst_settings_13 = s5k6aafx13_burst_cfg_13,
    .init_burst_settings_size_13 = ARRAY_SIZE(s5k6aafx13_burst_cfg_13),
    .init_burst_settings_14 = s5k6aafx13_burst_cfg_14,
    .init_burst_settings_size_14 = ARRAY_SIZE(s5k6aafx13_burst_cfg_14),
    .init_burst_settings_15 = s5k6aafx13_burst_cfg_15,
    .init_burst_settings_size_15 = ARRAY_SIZE(s5k6aafx13_burst_cfg_15),
    .init_burst_settings_16 = s5k6aafx13_burst_cfg_16,
    .init_burst_settings_size_16 = ARRAY_SIZE(s5k6aafx13_burst_cfg_16),
    .init_burst_settings_17 = s5k6aafx13_burst_cfg_17,
    .init_burst_settings_size_17 = ARRAY_SIZE(s5k6aafx13_burst_cfg_17),
    .init_burst_settings_18 = s5k6aafx13_burst_cfg_18,
    .init_burst_settings_size_18 = ARRAY_SIZE(s5k6aafx13_burst_cfg_18),
    .init_burst_settings_19 = s5k6aafx13_burst_cfg_19,
    .init_burst_settings_size_19 = ARRAY_SIZE(s5k6aafx13_burst_cfg_19),
    .init_burst_settings_20 = s5k6aafx13_burst_cfg_20,
    .init_burst_settings_size_20 = ARRAY_SIZE(s5k6aafx13_burst_cfg_20),

    .init_burst_settings_21 = s5k6aafx13_burst_cfg_21,
    .init_burst_settings_size_21 = ARRAY_SIZE(s5k6aafx13_burst_cfg_21),
    .init_burst_settings_22 = s5k6aafx13_burst_cfg_22,
    .init_burst_settings_size_22 = ARRAY_SIZE(s5k6aafx13_burst_cfg_22),
    .init_burst_settings_23 = s5k6aafx13_burst_cfg_23,
    .init_burst_settings_size_23 = ARRAY_SIZE(s5k6aafx13_burst_cfg_23),
    .init_burst_settings_24 = s5k6aafx13_burst_cfg_24,
    .init_burst_settings_size_24 = ARRAY_SIZE(s5k6aafx13_burst_cfg_24),
    .init_burst_settings_25 = s5k6aafx13_burst_cfg_25,
    .init_burst_settings_size_25 = ARRAY_SIZE(s5k6aafx13_burst_cfg_25),
    .init_burst_settings_26 = s5k6aafx13_burst_cfg_26,
    .init_burst_settings_size_26 = ARRAY_SIZE(s5k6aafx13_burst_cfg_26),
    .init_burst_settings_27 = s5k6aafx13_burst_cfg_27,
    .init_burst_settings_size_27 = ARRAY_SIZE(s5k6aafx13_burst_cfg_27),
    .init_burst_settings_28 = s5k6aafx13_burst_cfg_28,
    .init_burst_settings_size_28 = ARRAY_SIZE(s5k6aafx13_burst_cfg_28),
    .init_burst_settings_29 = s5k6aafx13_burst_cfg_29,
    .init_burst_settings_size_29 = ARRAY_SIZE(s5k6aafx13_burst_cfg_29),
    .init_burst_settings_30 = s5k6aafx13_burst_cfg_30,
    .init_burst_settings_size_30 = ARRAY_SIZE(s5k6aafx13_burst_cfg_30),

    .init_burst_settings_31 = s5k6aafx13_burst_cfg_31,
    .init_burst_settings_size_31 = ARRAY_SIZE(s5k6aafx13_burst_cfg_31),
    .init_burst_settings_32 = s5k6aafx13_burst_cfg_32,
    .init_burst_settings_size_32 = ARRAY_SIZE(s5k6aafx13_burst_cfg_32),
    .init_burst_settings_33 = s5k6aafx13_burst_cfg_33,
    .init_burst_settings_size_33 = ARRAY_SIZE(s5k6aafx13_burst_cfg_33),
    .init_burst_settings_34 = s5k6aafx13_burst_cfg_34,
    .init_burst_settings_size_34 = ARRAY_SIZE(s5k6aafx13_burst_cfg_34),
    .init_burst_settings_35 = s5k6aafx13_burst_cfg_35,
    .init_burst_settings_size_35 = ARRAY_SIZE(s5k6aafx13_burst_cfg_35),
    .init_burst_settings_36 = s5k6aafx13_burst_cfg_36,
    .init_burst_settings_size_36 = ARRAY_SIZE(s5k6aafx13_burst_cfg_36),
    .init_burst_settings_37 = s5k6aafx13_burst_cfg_37,
    .init_burst_settings_size_37 = ARRAY_SIZE(s5k6aafx13_burst_cfg_37),
    .init_burst_settings_38 = s5k6aafx13_burst_cfg_38,
    .init_burst_settings_size_38 = ARRAY_SIZE(s5k6aafx13_burst_cfg_38),
    .init_burst_settings_39 = s5k6aafx13_burst_cfg_39,
    .init_burst_settings_size_39 = ARRAY_SIZE(s5k6aafx13_burst_cfg_39),    
#endif
};

