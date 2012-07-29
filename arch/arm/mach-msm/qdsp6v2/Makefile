obj-$(CONFIG_MSM8X60_RTAC) += rtac.o
obj-y += audio_dev_ctl.o
obj-y += board-msm8x60-audio.o
obj-$(CONFIG_TIMPANI_CODEC) += snddev_icodec.o
obj-y += snddev_ecodec.o snddev_mi2s.o snddev_virtual.o
obj-y += apr.o apr_tal.o q6core.o dsp_debug.o
obj-y += audio_acdb.o
obj-y += q6asm.o q6adm.o q6afe.o
obj-y += pcm_out.o pcm_in.o fm.o
obj-y += audio_lpa.o
obj-y += q6voice.o
obj-y += snddev_hdmi.o
obj-y += aac_in.o qcelp_in.o evrc_in.o amrnb_in.o audio_utils.o
obj-y += audio_mvs.o
obj-y += audio_wma.o audio_wmapro.o audio_aac.o
obj-$(CONFIG_FB_MSM_HDMI_MSM_PANEL) += lpa_if_hdmi.o
ifeq ($(CONFIG_PANTECH_AUDIO_PRESTO_FAB2200), y)
obj-y += snd_presto_sub_fab2200.o
obj-$(CONFIG_PANTECH_AUDIO_SUBCHIP_CAL_TOOL) += sky_snd_presto_extampctrl.o
endif
obj-$(CONFIG_PANTECH_AUDIO_PRESTO_AUDIENCE2020) += snd_presto_audience_a2020.o
obj-$(CONFIG_SKY_SND_EXTAMP) += sky_snd_ext_amp_max97001.o
obj-$(CONFIG_SKY_SND_MVS) += mvs_pcm.o
obj-$(CONFIG_SKY_SND_MVS) += mvs_amr.o
obj-$(CONFIG_SKY_SND_MVS) += amr_transcode.o
obj-$(CONFIG_SKY_SND_MVS) += audio_mvs_adv.o
obj-$(CONFIG_SKY_SND_MVS) += audio_fileio.o
