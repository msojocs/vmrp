#sky start....
DSM_SUPPORT = TRUE
DSM_HAIWAI_SUPPORT = FALSE
DSM_QQ_SUPPORT = TRUE
DSM_GAME_SUPPORT = TRUE
DSM_NETGAME_SUPPORT = TRUE
DSM_SHOUXIN_SUPPORT = TRUE
DSM_MPCHAT_SUPPORT = TRUE
DSM_MPTV_SUPPORT = TRUE
DSM_EBOOK_SUPPORT  = TRUE
DSM_TOMCAT_SUPPORT = TRUE
DSM_FRUIT_SUPPORT = TRUE
DSM_BIRD_SUPPORT = TRUE
DSM_SKIRT_SUPPORT = TRUE
DSM_BYDR_SUPPORT =  TRUE
DSM_MSN_SUPPORT = TRUE
DSM_FETION_SUPPORT = TRUE
DSM_KAIXIN_SUPPORT = TRUE
DSM_SINAWEIBO_SUPPORT  = TRUE
DSM_TIANQI_SUPPORT = TRUE
DSM_NETCLOCK_SUPPORT = TRUE
DSM_HC_CALL_NEW = FALSE
ifeq ($(strip $(DSM_SUPPORT)),TRUE)

	COM_DEFS += __MMI_DSM_NEW__ __SKYENGINE_V2__
	COMPOBJS += plutommi\mmi\mythroad\mythroadlib\dsm.lib
	COMPOBJS += plutommi\mmi\mythroad\mythroadlib\mmidsm111.lib

	DSM_MEDIA_SUPPORT = TRUE
	ifeq ($(strip $(DSM_MEDIA_SUPPORT)),TRUE)
		COM_DEFS   += __DSM_MEDIA_EXT_H264_DECODE__
		COM_DEFS   += __DSM_MEDIA_EXT_H264_ENCODE__
		COM_DEFS   += __DSM_MEDIA_EXT_AUD_REC__
		COMPOBJS   += plutommi\mmi\Mythroad\mythroadlib\dsmmediaex.lib
	endif
	
	ifeq ($(strip $(DSM_MINI_SUPPORT)),TRUE)
		COM_DEFS   += __MR_CFG_FEATURE_SLIM__
		COMPOBJS += plutommi\mmi\mythroad\mythroadlib\mmidsmex_mini.lib
	else
		COMPOBJS += plutommi\mmi\mythroad\mythroadlib\mmidsmex.lib
	endif
	

	#海外应用开关
	ifeq ($(strip $(DSM_HAIWAI_SUPPORT)),TRUE)
		DSM_SKYBUDDY_SUPPORT = TRUE
		COM_DEFS   += __MR_CFG_FEATURE_OVERSEA__
	endif

	
	#RMVB播放器,需要单独再移植，不要直接打开
	ifeq ($(strip $(DSM_SPLAYER_SUPPORT)),TRUE) 
		COM_DEFS += __DSM_SPLAYER__ 
		COM_DEFS += __DSM_SPLAYER_FLV_SUPPORT__ 
		COM_DEFS += __DSM_SPLAYER_MPEG4_SUPPORT__
		COM_DEFS += __DSM_SPLAYER_MJPEG_SUPPORT__
		COM_DEFS += __DSM_SPLAYER_RMVB_SUPPORT__
		COM_DEFS += __DSM_SPLAYER_H264_SUPPORT__
		ifeq ($(findstring __DSM_SPLAYER_RMVB_SUPPORT__,$(COM_DEFS)),__DSM_SPLAYER_RMVB_SUPPORT__)
			COM_DEFS += __DSM_SPLAYER_RMVB_HD_SUPPORT__
			COMPOBJS += plutommi\mmi\mythroad\MMLib\SPlayerLib_rmvb.lib 
		else
			COMPOBJS += plutommi\mmi\mythroad\MMLib\SPlayerLib_no_rmvb 
		endif
	endif


	#视频聊天
	ifeq ($(strip $(DSM_MPCHAT_SUPPORT)),TRUE)
		COM_DEFS 	+=	__MR_CFG_ENTRY_MPCHAT__
	endif


	#手机电视
	ifeq ($(strip $(DSM_MPTV_SUPPORT)),TRUE)
		COM_DEFS 	+=	__MR_CFG_ENTRY_MPLIVE__
	endif


	#QQ
	ifeq ($(strip $(DSM_QQ_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_QQ__
	endif


	#游戏中心
	ifeq ($(strip $(DSM_GAME_SUPPORT)),TRUE)
		ifeq ($(strip $(DSM_HAIWAI_SUPPORT)),TRUE)
			COM_DEFS	+=	__MR_CFG_ENTRY_MRPSTORE__
		else
			COM_DEFS	+=	__MR_CFG_ENTRY_GAMECENTER__
			COM_DEFS	+=	__MR_CFG_ENTRY_APPSTORE__
		endif
	endif

	
	#SKYBUDDY,外单应用,类似EBUDDY，包括MSN,YAHOO,FACEBOOK,TWITTER
	ifeq ($(strip $(DSM_SKYBUDDY_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_SKYBUDDY__
	endif


	#网游
	ifeq ($(strip $(DSM_NETGAME_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_NETGAME__
	endif
	

	#手信
	ifeq ($(strip $(DSM_SHOUXIN_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_SHOUXIN__
	endif


	#电子书
	ifeq ($(strip $(DSM_EBOOK_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_EBOOK__
	endif

	#给力猫
	ifeq ($(strip $(DSM_TOMCAT_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_TOMCAT__
	endif

	#植物大战僵尸
	ifeq ($(strip $(DSM_JS_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_JS__
	endif

	#忍者水果
	ifeq ($(strip $(DSM_FRUIT_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_FRUIT__
	endif

	#疯狂小鸟
	ifeq ($(strip $(DSM_BIRD_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_ANGRYBIRD__
	endif

	#吹裙子
	ifeq ($(strip $(DSM_SKIRT_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_SKIRT__
	endif

	#捕鱼达人
	ifeq ($(strip $(DSM_BYDR_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_BYDR__
	endif

	
	#MSN
	ifeq ($(strip $(DSM_MSN_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_MSN__
	endif

	#飞信
	ifeq ($(strip $(DSM_FETION_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_FETION__
	endif

	#开心网
	ifeq ($(strip $(DSM_KAIXIN_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_KAIXIN__
	endif

	#新浪微博
	ifeq ($(strip $(DSM_SINAWEIBO_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_SINAWEIBO__
	endif

	#天气助手
	ifeq ($(strip $(DSM_TIANQI_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_TIANQI__
	endif

	#网络时钟
	ifeq ($(strip $(DSM_NETCLOCK_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_NETCLOCK__
		#网络时钟固化开关
		ifeq ($(strip $(DSM_NETCLOCK_INROM_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_NETCLOCK_INROM__
		endif
	endif
	
	#英汉词典
	ifeq ($(strip $(DSM_CIDIAN_SUPPORT)),TRUE)
		COM_DEFS	+=	__MR_CFG_ENTRY_CIDIAN__
	endif

	#归属地
	ifeq ($(strip $(DSM_HC_CALL_NEW)),TRUE)
		COM_DEFS   += __HC_CALL_NEW__
		COM_DEFS   += __SUPPORT_PBCALL__
		COMPOBJS  += plutommi\mmi\mythroad\HcCall\HcCallLib\hccall_parse.lib	
	endif

	#手机安全卫士
	ifeq ($(strip $(DSM_GUARD_SUPPORT)),TRUE)
		COM_DEFS  += __SKY_SECURITY_GUARD__
		COM_DEFS  += __SKY_ATS__
		COMPOBJS  += plutommi\mmi\Mythroad\SecurityGuard\lib\sky_sg.lib
	endif

endif
#sky end...
