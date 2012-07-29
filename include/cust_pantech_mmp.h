#ifndef __CUST_PANTECH_MMP_H__
#define __CUST_PANTECH_MMP_H__

/*
  2011/03/02 �ǿ���
  PANTECH multimedia ���� ���� ���� �ֻ��� feature.
  ���� feature�� �����ϱ� ����� �κ� �Ǵ� multimedia���� �ҽ��� �ƴ� �κ���
  �����ϴ� ��� ���. (make file ���� ��� #�ּ� �κп� �߰�)
*/
#define FEATURE_PANTECH_MMP

/*
  2011/03/02 �ǿ���
  VisuialOn VOME engine���� /external/vome/... ���� �κ��� �����ϰų�
  VisualOn �ҽ��� �Ϻ� �����ؼ� ����ϴ� �κп� ���� feature�۾�
*/
#define FEATURE_PANTECH_MMP_VOME

/* 
  2011/03/10 �ֺ���
  QualComm Patch ������ �κп� ���� frature �۾�
*/
#define FEATURE_PANTECH_MMP_QCOM_CR

/* 
  2011/03/11 �ǿ���
  StageFright ���� �����ϴ� �κе��� ã�� ���� �ϱ� ���� ���
*/
#define FEATURE_PANTECH_MMP_STAGEFRIGHT
 
/* 
  2011/03/11 �ǿ���
  OpenCORE ���� �����ϴ� �κе��� ã�� ���� �ϱ� ���� ���
*/
#define FEATURE_PANTECH_MMP_OPENCORE

/* 
  2011/04/16 �̼���
  MP4 QuickTime(ftypqt) ������ SF���� �������� �ʾ� PV�� ���� ���(KakaoTok issue)
*/
#define FEATURE_PANTECH_MMP_QUICKTIME

/*
  2011/04/29 Heekyoung Seo
  Add WMA S/W Decoder and code to use VC-1 H/W Decoder with Stagefright for 
  SKT HOPPIN Service.
  If Don't need HOPPIN Service, it also need to modify 
  frameworks/base/media/libstagefright/Android.mk. (remove 
  BUILD_WITH_WMA_SW_DECODER:=true)
  */
#define FEATURE_PANTECH_MMP_HOPPIN

/*
  2011/05/13 Heekyoung Seo
  Add Qualcomm VC-1 Patch.
  Fis timeStamp order is up so down with a few WMV8 streams.
  */
//#define FEATURE_PANTECH_MMP_VC1_DEC_PATCH

/*
 2011/06/22 �ֺ���
  ������ �ؿܸ� �����ϱ� ���� FEATURE�� �����Ͽ� ����
*/

#define FEATURE_PANTECH_MMP_DOMESTIC


/*
 2011/06/26 �ֺ���

 TestSBA_M8260AAABQNLZA3040_Pantech_EF33S-EF34K_05252011_Case00518255

 Qcom H/W Dec�� ����Ͽ� XivD ���� ����� ȭ���� �ϱ׷����� ������ �ذ��ϱ� ���� Test SBA
 
 ( simple profile B-frame )
 */
#define FEATURE_PANTECH_MMP_XVID_QCOM_HWDEC_SBA


/*
 2011/07/12 �ֺ���

 SBA_M8660AAABQNLYA109020_Pantech_EF33S_07122011_Case00522374_00522374

 */
#define FEATURE_PANTECH_MMP_QCOM_SBA_TIMESTAMP

/*
 2011/08/11 Heekyoung Seo 

 Apply Honeycomb Version to resolve wrong duration value and seek position for ogg file.

 Only Modify OggExtractor.h/cpp
*/
#define FEATURE_PANTECH_MMP_OGG_DURATION

/*
  2011/mm/dd who
  ...description...
*/
#define FEATURE_PANTECH_MMP_xxx

/*
  2011/mm/dd who
  ...description...
*/
#define FEATURE_PANTECH_MMP_zzz


#endif/* __CUST_PANTECH_MMP_H__ */
