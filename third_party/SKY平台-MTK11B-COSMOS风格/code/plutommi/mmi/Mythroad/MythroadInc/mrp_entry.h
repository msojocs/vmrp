#ifndef __MR_CFG_FEATURE_OVERSEA__  


/*********************************************************************************
 * 国内固化的应用列表信息
 *********************************************************************************/
 
#if 1//def __MR_CFG_ENTRY_GAMECENTER__  	
	#ifndef __MR_CFG_ENTRY_APPSTORE__
		MR_APPINFO_BEGIN(MENU_DSM_GAME_CENTER, MR_APPID_COOKIE, "*J,games")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 18)
	#else
		MR_APPINFO_BEGIN(MENU_DSM_GAME_CENTER, MR_APPID_COOKIE, "*J,istore,2935,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 10)
	#endif
	MR_APPINFO_END()
#endif	


#ifdef __MR_CFG_ENTRY_NETGAME__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
		MR_APPINFO_BEGIN(MENU_DSM_NETGAME, MR_APPID_NET_GAMES, "*J,maopao")
		MR_APPINFO_ENTRY(mr_netgameshow, 1)
		MR_APPINFO_ENTRY(mr_all_list, 9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 18)
	#else//todo
		MR_APPINFO_BEGIN(MENU_DSM_NETGAME, MR_APPID_NET_GAMES, "*J,hxsg,1007,0")
		MR_APPINFO_ENTRY(mr_all_list, 9)
		//MR_APPINFO_ENTRY(mr_netgame_enty, 11)
	#endif
	MR_APPINFO_END()
#endif	


#ifdef __MR_CFG_ENTRY_EBOOK__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_BOOK, MR_APPID_SKYREAD, "*J,skyread")
		MR_APPINFO_ENTRY(mr_all_list, 9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 18)
	#else	
	MR_APPINFO_BEGIN(MENU_DSM_BOOK, MR_APPID_SKYREAD, "*J,skyread,2908,0")
		MR_APPINFO_ENTRY(mr_all_list, 9)
	#endif
	MR_APPINFO_END()
#endif	


#ifdef __MR_CFG_ENTRY_QQ__
#if defined(__MMI_NGUAQ__) && !defined(WIN32) 
	MR_APPINFO_BEGIN2(MENU_DSM_QQ, MR_APPID_NGUAQ, GetSKYQQEntryMalloc)
		MR_APPINFO_ENTRY(mr_all_list, 9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 18)
		#ifdef __MR_CFG_QQ_INROM__
		MR_APPINFO_ENTRY(mr_skyqq, 10)
		#endif
	MR_APPINFO_END()
#elif defined(__MMI_SKYQQ__) && !defined(WIN32) 
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN2(MENU_DSM_QQ, MR_APPID_SKYQQ, GetSKYQQEntryMalloc)
		MR_APPINFO_ENTRY(mr_all_list, 9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 18)
	#else
	MR_APPINFO_BEGIN2(MENU_DSM_QQ, MR_APPID_SKYQQ, GetSKYQQEntryMalloc)
		MR_APPINFO_ENTRY(mr_all_list, 9)
	#endif
		#ifdef __MR_CFG_QQ_INROM__
		MR_APPINFO_ENTRY(mr_skyqq, 10)
		#endif
	MR_APPINFO_END()
#else
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_QQ, MR_APPID_QQ2008, "*J,qq2008")
		MR_APPINFO_ENTRY(mr_all_list, 9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 18)
	#else
	MR_APPINFO_BEGIN(MENU_DSM_QQ, MR_APPID_QQ2008, "*J,qq2008,203103,0")
		MR_APPINFO_ENTRY(mr_all_list, 9)
	#endif
	MR_APPINFO_END()
#endif
#endif

#ifdef __MR_CFG_ENTRY_SHOUXIN__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_SHOUXIN, MR_APPID_SHOUXIN, "*J,shouxin,2934")
	#else
	MR_APPINFO_BEGIN(MENU_DSM_SHOUXIN, MR_APPID_SHOUXIN, "*J,shouxin,2934,0")
	#endif
		MR_APPINFO_ENTRY(mr_all_list,  9)
		#ifndef __MR_CFG_FEATURE_SLIM__
		MR_APPINFO_ENTRY(mr_shouxin,  10)
		#endif
	MR_APPINFO_END()
#endif


#ifdef __MR_CFG_ENTRY_MSN__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_MSN, MR_APPID_MSN, "*J,msn")
		MR_APPINFO_ENTRY(mr_all_list, 9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 18)
	#else
	MR_APPINFO_BEGIN(MENU_DSM_MSN, MR_APPID_MSN, "*J,msn,203102,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#endif
		
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_FETION__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_FETION, MR_APPID_FETION, "*J,otafx3")
		MR_APPINFO_ENTRY(mr_all_list, 9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 18)
	#else
	MR_APPINFO_BEGIN(MENU_DSM_FETION, MR_APPID_FETION, "*J,otafx3,6005,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#endif

	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_KAIXIN__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_KAIXIN, MR_APPID_KAIXIN, "*J,kx001")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)	
	#else
	MR_APPINFO_BEGIN(MENU_DSM_KAIXIN, MR_APPID_KAIXIN, "*J,kx001,6020,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#endif

	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_TIANQI__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_TIANQI, MR_APPID_TIANQI, "*J,otatq")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)	
	#else
	MR_APPINFO_BEGIN(MENU_DSM_TIANQI, MR_APPID_TIANQI, "*J,otatq,2919,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#endif

	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_TOMCAT__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_TOMCAT, MR_APPID_TOMCAT, "*J,talkcat")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)	
	#else
	MR_APPINFO_BEGIN(MENU_DSM_TOMCAT, MR_APPID_TOMCAT, "*J,talkcat,2924,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#endif
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_FRUIT__
#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_FRUIT, MR_APPID_FRUIT, "*J,rzsgqd")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)
#else
	MR_APPINFO_BEGIN(MENU_DSM_FRUIT, MR_APPID_FRUIT, "*J,rzsgqd,2937,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)
#endif
	MR_APPINFO_END()
#endif


#ifdef __MR_CFG_ENTRY_JS__
#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_JS, MR_APPID_JS, "*J,pvz")//gzwdzjs
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)
#else
	MR_APPINFO_BEGIN(MENU_DSM_JS, MR_APPID_JS, "*J,pvz,70000020,0")//gzwdzjs,332005
		MR_APPINFO_ENTRY(mr_all_list,  9)
#endif
	MR_APPINFO_END()
#endif


#ifdef __MR_CFG_ENTRY_ANGRYBIRD__
#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_ANGRYBIRD, MR_APPID_ANGRYBIRD, "*J,gfkxn")
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)
#else
	MR_APPINFO_BEGIN(MENU_DSM_ANGRYBIRD, MR_APPID_ANGRYBIRD, "*J,gfkxn,1016,0")
#endif
	MR_APPINFO_ENTRY(mr_all_list,  9)
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_SKIRT__
#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_SKIRT, MR_APPID_SKIRT, "*J,skirt")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)
#else
	MR_APPINFO_BEGIN(MENU_DSM_SKIRT, MR_APPID_SKIRT, "*J,skirt,1020,0")
	MR_APPINFO_ENTRY(mr_all_list,  9)
#endif
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_BYDR__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_BYDR, MR_APPID_BYDR, "*J,bydr")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)
	#else
		MR_APPINFO_BEGIN(MENU_DSM_BYDR, MR_APPID_BYDR, "*J,bydr,2933,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#endif
	MR_APPINFO_END()
#endif



#ifdef __MR_CFG_ENTRY_MPLIVE__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_MPLIVE, MR_APPID_MPLIVE, "*J,mplive,2917")
	#else
	MR_APPINFO_BEGIN(MENU_DSM_MPLIVE, MR_APPID_MPLIVE, "*J,mplive,2917,0")
	#endif
		MR_APPINFO_ENTRY(mr_all_list,  9)
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_MPCHAT__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_MPCHAT, MR_APPID_MPCHAT, "*J,mpchat,2918")
	#else
	MR_APPINFO_BEGIN(MENU_DSM_MPCHAT, MR_APPID_MPCHAT, "*J,mpchat,2918,0")
	#endif
		MR_APPINFO_ENTRY(mr_all_list,  9)
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_SINAWEIBO__
#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_SINAWEIBO, MR_APPID_WEIBO, "*J,axlwb")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)	
#else
	MR_APPINFO_BEGIN(MENU_DSM_SINAWEIBO, MR_APPID_WEIBO, "*J,axlwb,391049,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
#endif
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_NETCLOCK__
#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_NETCLOCK, MR_APPID_NETCLOCK, "*J,clockc")
		MR_APPINFO_ENTRY(mr_all_list,  9)
		MR_APPINFO_ENTRY( mr_alllist_cfg, 18)
#else
	MR_APPINFO_BEGIN(MENU_DSM_NETCLOCK, MR_APPID_NETCLOCK, "*J,clockc,6008,0")
	MR_APPINFO_ENTRY(mr_all_list,  9)
#endif
		#ifdef __MR_CFG_NETCLOCK_INROM__
		MR_APPINFO_ENTRY( mr_clock, 10)
		#endif
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_QQBROWSER__
	MR_APPINFO_BEGIN(MENU_DSM_QQBROWSER, MR_APPID_QQBROWSER, "*J,atxllq,391033")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	MR_APPINFO_END()
#endif

#ifdef __SKY_SECURITY_GUARD__
	#ifndef __MR_CFG_ENTRY_APPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_SAFE, MR_APPID_GUARD, "*J,mpsjws")
		MR_APPINFO_ENTRY(mr_all_list, 9)
		MR_APPINFO_ENTRY(mr_alllist_cfg, 18)
	#else
	MR_APPINFO_BEGIN(MENU_DSM_SAFE, MR_APPID_GUARD, "*J,mpsjws,2925,0")
		MR_APPINFO_ENTRY(mr_all_list, 9)
	#endif
	MR_APPINFO_END()
#endif	

#ifdef __MR_CFG_ENTRY_NES__
	MR_APPINFO_BEGIN(MENU_DSM_NES, MR_APPID_NES, "*J")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	MR_APPINFO_END()
#endif

/*********************************************************************************
 * 海外固化的应用列表信息
 *********************************************************************************/
#else 

#ifdef __MR_CFG_ENTRY_SKYBUDDY__
	MR_APPINFO_BEGIN(MENU_DSM_SKYBUDDY, MR_APPID_SKYBUDDY, "*J")
		MR_APPINFO_ENTRY(mr_eim,  9)
		MR_APPINFO_ENTRY(mr_edg,  8)
		#ifndef __MR_CFG_FEATURE_SLIM__
		MR_APPINFO_ENTRY(mr_ftalk,10)
		#endif
		
		#ifdef __MR_CFG_FIRM_IMS__
		MR_APPINFO_ENTRY(mr_yahoo,  11)
		MR_APPINFO_ENTRY(mr_twitter,   12)
		MR_APPINFO_ENTRY(mr_msn,	13)
		MR_APPINFO_ENTRY(mr_facebook,14)
		#endif
	MR_APPINFO_END() 
#endif 


#ifdef __MR_CFG_ENTRY_MRPSTORE__
	MR_APPINFO_BEGIN(MENU_DSM_MRPSTORE, MR_APPID_MRPSTORE, "*J,1000,1,0,3,3,5,0,0")
		#if defined(__DSM_MRPSTORE__) ||defined(__DSM_DSMGAME__)
		MR_APPINFO_ENTRY(mr_all_list,  9)
		#elif defined(__DSM_SKYBUDDY__)
		MR_APPINFO_ENTRY(mr_eim,  9)
		MR_APPINFO_ENTRY(mr_edg,  8)
		#endif
		#ifdef __MRP_CDSM_INROM__
		MR_APPINFO_ENTRY(mr_cdsm,  1)
		#endif
	MR_APPINFO_END()
#endif


#ifdef __MR_CFG_ENTRY_TOMCAT__
	#if 0//defined(__DSM_MRPSTORE__) || defined(__DSM_DSMGAME__)
	MR_APPINFO_BEGIN(MENU_DSM_TOMCAT, MR_APPID_TOMCAT, "*J,1000,1,0,3,3,5,0,0,Tom Cat,814459,rglm.mrp,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#else
	MR_APPINFO_BEGIN(MENU_DSM_TOMCAT, MR_APPID_TOMCAT, "*J,1000,3,0,3,3,5,0,0,Tom Cat,814459,rglm.mrp,0")
		MR_APPINFO_ENTRY(mr_eim,  9)
		MR_APPINFO_ENTRY(mr_edg,  8)
	#endif
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_FRUIT__
	#if 0//defined(__DSM_MRPSTORE__) || defined(__DSM_DSMGAME__)
	MR_APPINFO_BEGIN(MENU_DSM_FRUIT, MR_APPID_FRUIT, "*J,1000,1,0,3,3,5,0,0,Fruit Ninja,814042,rqshg.mrp,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#else
	MR_APPINFO_BEGIN(MENU_DSM_FRUIT, MR_APPID_FRUIT, "*J,1000,3,0,3,3,5,0,0,Fruit Ninja,814042,rqshg.mrp,0")
		MR_APPINFO_ENTRY(mr_eim,  9)
		MR_APPINFO_ENTRY(mr_edg,  8)
	#endif
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_ANGRYBIRD__
	#if 0//defined(__DSM_MRPSTORE__) || defined(__DSM_DSMGAME__)
	MR_APPINFO_BEGIN(MENU_DSM_ANGRYBIRD, MR_APPID_ANGRYBIRD, "*J,1000,1,0,3,3,5,0,0,Angry Birds,814041,gfkxn.mrp,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#else
	MR_APPINFO_BEGIN(MENU_DSM_ANGRYBIRD, MR_APPID_ANGRYBIRD, "*J,1000,3,0,3,3,5,0,0,Angry Birds,814041,gfkxn.mrp,0")
		MR_APPINFO_ENTRY(mr_eim,  9)
		MR_APPINFO_ENTRY(mr_edg,  8)
	#endif
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_SKIRT__
	#if 0//defined(__DSM_MRPSTORE__) || defined(__DSM_DSMGAME__)
	MR_APPINFO_BEGIN(MENU_DSM_SKIRT, MR_APPID_SKIRT, "*J,1000,1,0,3,3,5,0,0,Skirt,819995,rchqz.mrp,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#else
	MR_APPINFO_BEGIN(MENU_DSM_SKIRT, MR_APPID_SKIRT, "*J,1000,3,0,3,3,5,0,0,Skirt,819995,rchqz.mrp,0")
		MR_APPINFO_ENTRY(mr_eim,  9)
		MR_APPINFO_ENTRY(mr_edg,  8)
	#endif
	MR_APPINFO_END()
#endif

#ifdef __MR_CFG_ENTRY_BYDR__
	#if 0//defined(__DSM_MRPSTORE__) || defined(__DSM_DSMGAME__)
	MR_APPINFO_BEGIN(MENU_DSM_BYDR, MR_APPID_BYDR, "*J,1000,1,0,3,3,5,0,0,Fish Man,819994,rbydr.mrp,0")
		MR_APPINFO_ENTRY(mr_all_list,  9)
	#else
	MR_APPINFO_BEGIN(MENU_DSM_BYDR, MR_APPID_BYDR, "*J,1000,3,0,3,3,5,0,0,Fish Man,819994,rbydr.mrp,0")
		MR_APPINFO_ENTRY(mr_eim,  9)
		MR_APPINFO_ENTRY(mr_edg,  8)
	#endif
	MR_APPINFO_END()
#endif

#endif/*__MR_CFG_FEATURE_OVERSEA__*/  

