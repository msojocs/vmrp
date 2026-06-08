# Define source file lists to SRC_LIST
ifneq ($(filter __ARM9_MMU__ __ARM11_MMU__, $(strip $(MODULE_DEFS))),)
CFLAGS += --forceinline
CPLUSFLAGS += --forceinline
endif

SRC_LIST = venusmmi\app\Cosmos\HelloWorld\vapp_helloworld.cpp \
           venusmmi\app\Cosmos\WidgetBaiduSearch\vapp_baidu_search.cpp \
           venusmmi\app\Cosmos\WidgetBaiduSearch\vapp_search_page_base_class.cpp \
           venusmmi\app\Cosmos\WidgetBaiduSearch\vapp_search_widget_base_class.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_flow.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_wait_evt.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_init.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_disk.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_app.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_mode.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_cfg_sim.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_ani.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_sec.cpp \
           venusmmi\app\Cosmos\Bootup\vapp_bootup_sim_str.cpp \
           plutommi\mmi\Bootup\BootupSrc\BootupMoDIS.c \
           venusmmi\app\Cosmos\PasswordPage\vapp_password_page.cpp \
           venusmmi\app\Cosmos\PasswordPage\vapp_cfm_pwd_page.cpp \
           venusmmi\app\Cosmos\AniPage\vapp_ani_page.cpp \
           venusmmi\app\Cosmos\StyleDyer\vapp_style_dyer.cpp \
           venusmmi\app\Cosmos\Shutdown\vapp_shutdown_flow.cpp \
           venusmmi\app\Cosmos\Shutdown\vapp_shutdown_app.cpp \
           venusmmi\app\Cosmos\NwInfo\vapp_nw_info.cpp \
           venusmmi\app\Cosmos\NwInfo\vapp_nw_info_icon.cpp \
           venusmmi\app\Cosmos\NwInfo\vapp_nw_info_signal.cpp \
           venusmmi\app\Cosmos\NwInfo\vapp_nw_info_signal_custom.cpp \
           plutommi\MMI\NwInfo\NwInfoSrc\NwInfoIcon.c \
           venusmmi\app\Cosmos\SimCtrl\vapp_sim_ctrl.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_main.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_phone.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_str.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_chv.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_page_seq.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_ubchv.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_cui.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_cui_chv.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_cui_phone.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_ssc.cpp \
           venusmmi\app\Cosmos\SecSet\vapp_secset_hp_veri.cpp \
           venusmmi\app\Cosmos\Launcher\vapp_launcher_app.cpp

ifneq ($(filter __MMI_VUI_SETTING_LAUNCHER_SELECTION__, $(strip $(MODULE_DEFS))),) 
SRC_LIST += venusmmi\app\Cosmos\LauncherShuttle\vapp_launcher_shuttle.cpp
endif
  
ifneq ($(filter __MMI_VUI_LAUNCHER_COSMOS__, $(strip $(MODULE_DEFS))),)         
SRC_LIST += venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_director.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_primitive.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_bar.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_desktop.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_page_selector.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_widget_selector.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_shortcut_selector.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_preinstaller.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_page_transition.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\Home\vapp_launcher_cosmos_page_transition_factory.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\MM\vapp_launcher_cosmos_mm.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\MM\vapp_launcher_cosmos_mm_builder.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\MM\vapp_launcher_cosmos_mm_page_menu.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\MM\vapp_launcher_cosmos_mm_shortcut_bar.cpp \
           venusmmi\app\Cosmos\LauncherCosmos\MM\vapp_launcher_cosmos_mm_barrel_setting.cpp
endif

ifneq ($(filter __MMI_VUI_LAUNCHER_ROBOT__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_director.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_primitive.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_bar.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_desktop.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_page_selector.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_widget_selector.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_shortcut_selector.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_preinstaller.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_page_transition.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Home\vapp_launcher_robot_page_transition_factory.cpp
endif
      
ifneq ($(filter __MMI_VUI_LAUNCHER_MM__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\LauncherMM\vapp_launcher_mm.cpp \
           venusmmi\app\Cosmos\LauncherMM\vapp_launcher_mm_builder.cpp \
           venusmmi\app\Cosmos\LauncherMM\vapp_launcher_mm_page_menu.cpp \
           venusmmi\app\Cosmos\LauncherMM\vapp_launcher_mm_barrel_setting.cpp
endif       
  
SRC_LIST += venusmmi\app\Cosmos\LauncherWin7\Home\vapp_launcher_win7_director.cpp 
SRC_LIST += venusmmi\app\Cosmos\LauncherWin7\SubMenu\vapp_launcher_win7_submenu.cpp 
SRC_LIST += venusmmi\app\Cosmos\LauncherWin7\SubMenu\vapp_launcher_win7_submenu_builder.cpp 
SRC_LIST += venusmmi\app\Cosmos\LauncherWin7\SubMenu\vapp_launcher_win7_submenu_page.cpp
#__MMI_LAUNCHER_WIN7__
  
SRC_LIST += venusmmi\app\Cosmos\ScreenLock\vapp_screen_lock_main.cpp \
           venusmmi\app\Cosmos\ScreenLock\vapp_screen_lock_screen.cpp \
           venusmmi\app\Cosmos\ScreenLock\vapp_screen_lock_setting.cpp \
           venusmmi\app\Cosmos\ScreenLock\vapp_screen_lock_unlock_item.cpp \
           venusmmi\app\Cosmos\ScreenLockZipper\vapp_screen_lock_zipper.cpp \
           venusmmi\app\Cosmos\ScreenLockZipper\vapp_screen_lock_zipper_time.cpp \
           venusmmi\app\Cosmos\ScreenLockPainting\vapp_screen_lock_painting.cpp

ifneq ($(filter __MMI_VUI_SETTING_SCREEN_LOCK_SELECTION__, $(strip $(MODULE_DEFS))),) 
SRC_LIST += venusmmi\app\Cosmos\ScreenLockShuttle\vapp_screen_lock_shuttle.cpp
endif

ifneq ($(filter __MMI_VUI_SCREEN_LOCK_COSMOS__, $(strip $(MODULE_DEFS))),)  
SRC_LIST += venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos.cpp \
           venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos_card.cpp \
           venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos_time.cpp \
           venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos_frame.cpp \
           venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos_call_card.cpp \
           venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos_msg_card.cpp \
           venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos_unlock_card.cpp \
           venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos_camcoder_card.cpp \
           venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos_unlock_item_kit.cpp \
           venusmmi\app\Cosmos\ScreenLockCosmos\vapp_screen_lock_cosmos_unlock_item_factory.cpp
endif           

ifneq ($(filter __MMI_VUI_SCREEN_LOCK_STEAM__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_time.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_frame.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_icon.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_icon_call.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_icon_msg.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_icon_unlock.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_icon_camcoder.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_owner_draw.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_unlock_item_kit.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\vapp_screen_lock_steam_unlock_item_factory.cpp \
           venusmmi\app\Cosmos\ScreenLockSteam\stroke.cpp
endif

ifneq ($(filter __MMI_VUI_SCREEN_LOCK_PATTERN__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\ScreenLockPattern\vapp_screen_lock_pattern.cpp \
           venusmmi\app\Cosmos\ScreenLockPattern\vapp_screen_lock_pattern_frame.cpp \
           venusmmi\app\Cosmos\ScreenLockPattern\vapp_screen_lock_pattern_time.cpp
endif

# __MMI_VUI_SCREEN_LOCK_POKER__
SRC_LIST += venusmmi\app\Cosmos\ScreenLockPoker\vapp_screen_lock_poker.cpp \
           venusmmi\app\Cosmos\ScreenLockPoker\vapp_screen_lock_poker_frame.cpp \
           venusmmi\app\Cosmos\ScreenLockPoker\vapp_screen_lock_poker_time.cpp



SRC_LIST += venusmmi\app\Cosmos\AppManager\vapp_app_manager.cpp \
           venusmmi\app\Cosmos\AppManager\vapp_mre_app_operator.cpp \
           venusmmi\app\Cosmos\AppManager\vapp_mre_app_ncenter.cpp \
           venusmmi\app\Cosmos\FontMgr\vapp_fnt_mgr.cpp\
           venusmmi\app\Cosmos\FileMgr\vapp_fmgr.cpp \
           venusmmi\app\Cosmos\FileMgr\vapp_fmgr_cui.cpp \
           venusmmi\app\Cosmos\FileMgr\vapp_fmgr_cp.cpp \
           venusmmi\app\Cosmos\FileMgr\vapp_fmgr_table.cpp \
           venusmmi\app\Cosmos\FileMgr\vapp_fmgr_pluto_dummy.cpp \
           plutommi\mmi\ucm\ucmsrc\UcmUha.c \
           plutommi\mmi\ucm\ucmsrc\UcmKernel.c \
           plutommi\mmi\ucm\ucmsrc\ucminterface.c \
           venusmmi\app\cosmos\ucm\vapp_ucm_app.cpp \
           venusmmi\app\cosmos\ucm\vapp_ucm_event_hdlr.cpp \
           venusmmi\app\cosmos\ucm\vapp_ucm_ui.cpp \
           venusmmi\app\Cosmos\ucm\vapp_ucm_video_frame.cpp \
           venusmmi\app\Cosmos\CallSetting\vapp_callset.cpp \
           venusmmi\app\Cosmos\CallSetting\vapp_callset_cf.cpp \
           venusmmi\app\Cosmos\CallSetting\vapp_callset_cb.cpp \
           venusmmi\app\Cosmos\SupplementaryService\vapp_ss.cpp \
           venusmmi\APP\Cosmos\Message\vapp_msg.cpp \
           venusmmi\APP\Cosmos\Message\vapp_msg_viewer.cpp \
           venusmmi\APP\Cosmos\Message\vapp_msg_misc_cp.cpp \
           venusmmi\APP\Cosmos\Message\vapp_msg_dialog_cell.cpp \
           venusmmi\App\Cosmos\Message\vapp_msg_contact_bar.cpp \
           venusmmi\App\Cosmos\Message\vapp_msg_use_detail.cpp \
           venusmmi\App\Cosmos\Message\vapp_sms_base.cpp \
           venusmmi\App\Cosmos\Message\vapp_sms_operator.cpp \
           venusmmi\App\Cosmos\Message\vapp_sms_provider.cpp \
           venusmmi\App\Cosmos\Message\vapp_sms_viewer.cpp \
           venusmmi\APP\Cosmos\Message\vapp_sms_setting_core.cpp \
           venusmmi\APP\Cosmos\Message\vapp_sms_setting_scrn.cpp \
           venusmmi\APP\Cosmos\Message\vapp_sms_voicemail_core.cpp \
           venusmmi\APP\Cosmos\Message\vapp_sms_voicemail_scrn.cpp \
           venusmmi\APP\Cosmos\Message\vapp_sms_notifier_core.cpp \
           venusmmi\APP\Cosmos\Message\vapp_sms_notifier_scrn.cpp \
           venusmmi\APP\Cosmos\Message\vapp_sms_util.cpp \
           venusmmi\APP\Cosmos\Message\vapp_sms_um_sap.cpp \
           venusmmi\APP\Cosmos\Message\vapp_text_template_core.cpp \
           venusmmi\APP\Cosmos\Message\vapp_text_template_scrn.cpp \
           venusmmi\app\Cosmos\CallLog\vapp_clog_list.cpp \
           venusmmi\app\Cosmos\CallLog\vapp_clog_action.cpp \
           venusmmi\app\Cosmos\CallLog\vapp_clog_detail.cpp \
           venusmmi\app\Cosmos\CallLog\vapp_clog_data_provider.cpp \
           venusmmi\app\Cosmos\CallLog\vapp_clog_usage.cpp \
           venusmmi\app\Cosmos\CallLog\vapp_clog_main.cpp \
           venusmmi\app\Cosmos\CallLog\vapp_clog_temp_dummy.c \
           plutommi\mmi\athandler\athandlersrc\athandler.c \
           plutommi\mmi\gpio\gpiosrc\generaldeviceinterface.c \
           venusmmi\app\Cosmos\DeviceApp\vapp_charger_main.cpp \
           venusmmi\app\Cosmos\DeviceApp\vapp_gpio_main.cpp \
           venusmmi\app\Cosmos\DeviceApp\vapp_gpio_setting.cpp \
           venusmmi\app\Cosmos\UnifiedComposer\vapp_uc_core.cpp \
           venusmmi\app\Cosmos\UnifiedComposer\vapp_uc_contactlist.cpp \
           venusmmi\app\Cosmos\UnifiedComposer\vapp_uc_screen.cpp \
           venusmmi\app\Cosmos\UnifiedComposer\vapp_uc_emontics.cpp \
           venusmmi\app\Cosmos\setting\setting_framework\vapp_setting_framework.cpp \
           venusmmi\app\Cosmos\setting\setting_framework\vapp_setting_framework_cell_list.cpp \
           venusmmi\app\Cosmos\setting\setting_framework\vapp_setting_framework_caption.cpp \
           venusmmi\app\Cosmos\setting\date_time_setting\vapp_date_time_setting.cpp \
           venusmmi\app\Cosmos\setting\dual_sim_setting\vapp_dual_sim_setting.cpp \
           venusmmi\app\Cosmos\setting\language_input_method\vapp_language_setting.cpp \
           venusmmi\app\Cosmos\setting\schedule_power_on_off\vapp_schedule_power_on_off.cpp \
           venusmmi\app\Cosmos\setting\pen_calibration\vapp_pen_calibration.cpp \
           venusmmi\app\Cosmos\setting\network_connectivity\vapp_network_selection.cpp \
           venusmmi\app\Cosmos\setting\network_connectivity\vapp_mobile_network_setting.cpp \
           venusmmi\app\Cosmos\setting\network_connectivity\vapp_flight_mode_setting.cpp \
           venusmmi\app\Cosmos\setting\network_connectivity\vapp_nfc_setting.cpp \
           venusmmi\app\Cosmos\setting\network_connectivity\vapp_plmn_list.cpp \
           venusmmi\app\Cosmos\setting\network_connectivity\vapp_preferred_network.cpp \
           venusmmi\app\Cosmos\setting\network_connectivity\vapp_network_connectivity_caption.cpp \
           venusmmi\app\Cosmos\setting\network_connectivity\vapp_network_checking.cpp \
           venusmmi\app\Cosmos\setting\display_setting\vapp_wallpaper_setting_method.cpp \
           venusmmi\app\Cosmos\setting\display_setting\vapp_setting_wallpaper_setting.cpp \
           venusmmi\app\Cosmos\setting\display_setting\vapp_setting_bootup_animation_setting.cpp \
           venusmmi\app\Cosmos\setting\display_setting\vcui_wallpaper_setting.cpp \
           venusmmi\app\Cosmos\setting\motion\vapp_setting_motion.cpp \
           venusmmi\app\Cosmos\setting\restore\vapp_restore.cpp \
           venusmmi\app\Cosmos\setting\network_connectivity\vapp_data_lock.cpp \
           plutommi\MMI\Setting\SettingSrc\NetSetServiceSelection.c \
           venusmmi\app\Cosmos\Theme\vapp_theme.cpp \
           venusmmi\app\Cosmos\Theme\vapp_theme_list.cpp \
           venusmmi\app\Cosmos\Theme\vapp_theme_menu.cpp \
           venusmmi\APP\cosmos\WorldClock\vapp_worldclock.cpp \
           venusmmi\APP\cosmos\WorldClock\vapp_worldclock_cui.cpp \
           venusmmi\APP\cosmos\WorldClock\vapp_worldclock_map_view.cpp \
           venusmmi\APP\cosmos\WorldClock\vapp_worldclock_select_city.cpp \
           venusmmi\APP\cosmos\WorldClock\vapp_worldclock_data_control.cpp \
           venusmmi\APP\cosmos\WorldClock\vapp_worldclock_panel.cpp \
           venusmmi\APP\cosmos\WorldClock\vapp_worldclock_srv.c \
           venusmmi\APP\cosmos\WorldClock\resource_world_clock_city.c \
           venusmmi\app\Cosmos\Alarm\vapp_alarm.cpp \
           venusmmi\app\Cosmos\Alarm\vapp_alarm_list.cpp \
           venusmmi\app\Cosmos\Alarm\vapp_alarm_edit.cpp \
           venusmmi\app\Cosmos\Alarm\vapp_alarm_indication.cpp \
           venusmmi\app\Cosmos\Alarm\vapp_alarm_setting.cpp \
           venusmmi\app\Cosmos\Alarm\vapp_alarm_clock.cpp \
           venusmmi\app\Cosmos\Alarm\vapp_alarm_list_menu.cpp \
           venusmmi\app\Cosmos\Alarm\vapp_alarm_evt_hdlr.cpp \
           venusmmi\app\Cosmos\Alarm\vapp_alarm_bootup_init.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_main.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_main_view.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_group_list.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_edit.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_view_detail.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_view_detail_ext.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_search.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_reminder.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_utility.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_list_mgr.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_add_birthday.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_java_entry.cpp \
           venusmmi\app\cosmos\calendar\vapp_cal_at_entry.cpp \
           venusmmi\app\cosmos\calendar\vcui_cal_list.cpp \
           venusmmi\app\cosmos\calendar\vapp_task_main.cpp \
           venusmmi\app\cosmos\calendar\vapp_task_all_list.cpp
           
ifneq ($(filter __MMI_VUI_LAUNCHER_ROBOT__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\LauncherRobot\Mainmenu\vapp_launcher_robot_mainmenu.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Mainmenu\vapp_launcher_robot_mainmenu_page.cpp \
           venusmmi\app\Cosmos\LauncherRobot\Mainmenu\vapp_launcher_robot_mainmenu_builder.cpp
endif

ifneq ($(filter __MMI_VUI_SETTING_LAUNCHER_SELECTION__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\setting\launcher_screen_lock\vapp_setting_launcher_content_provider.cpp
endif

ifneq ($(filter __MMI_VUI_SETTING_SCREEN_LOCK_SELECTION__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\setting\launcher_screen_lock\vapp_setting_scr_lock_content_provider.cpp
endif

SRC_LIST += venusmmi\app\Cosmos\setting\launcher_screen_lock\vapp_setting_launcher_setting.cpp
           
SRC_LIST += venusmmi\app\Cosmos\NCenter\vapp_ncenter.cpp \
           venusmmi\app\Cosmos\NCenter\vapp_ncenter_base_cell.cpp \
           venusmmi\app\Cosmos\NCenter\vapp_ncenter_table.cpp \
           venusmmi\app\Cosmos\NCenter\vapp_ncenter_test_cell.cpp \
           venusmmi\app\Cosmos\NCenter\vapp_ncenter_entry.cpp \
           venusmmi\app\Cosmos\NCenter\vapp_ncenter_system_cell.cpp \
           venusmmi\app\Cosmos\NCenter\vapp_nmgr.cpp \
           venusmmi\app\Cosmos\Sat\SAT_temp.c \
           venusmmi\app\Cosmos\Sat\SatCore.c \
           venusmmi\app\Cosmos\Sat\vapp_sat.cpp \
           venusmmi\app\Cosmos\Sat\vapp_sat_base.cpp \
           venusmmi\app\Cosmos\Sat\vapp_sat_view.cpp \
           venusmmi\app\Cosmos\Sat\vapp_sat_editor.cpp \
           venusmmi\app\Cosmos\Sat\vapp_sat_list_menu.cpp \
           venusmmi\app\Cosmos\Sat\vapp_sat_title_bar.cpp \
           venusmmi\app\Cosmos\Sat\vapp_sat_ncenter.cpp \
           venusmmi\app\Cosmos\Sat\Service_dummy.c \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_copy_delete.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_editor.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_extra.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_group.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_info.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_launch.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_list.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_multi.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_mycard.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_res.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_app.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_setting.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_speed.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_cui.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_vcard.cpp \
           venusmmi\app\Cosmos\Contact\ContactUI\vapp_phb_backup_restore.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_entry.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_group.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_list.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_op.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_setting.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_several.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_storage.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_task.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_util.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_vcard.cpp \
           venusmmi\app\Cosmos\Contact\ContactCore\vapp_contact_backup_restore.cpp \
           venusmmi\app\Cosmos\Dialer\vapp_contact_list_ctrl.cpp \
           venusmmi\app\Cosmos\Dialer\vapp_contact_list_data.cpp \
           venusmmi\app\Cosmos\Dialer\vapp_dialer_core.cpp \
           venusmmi\app\Cosmos\Dialer\vapp_dialer_cp.cpp \
           venusmmi\app\Cosmos\Dialer\vapp_dialer_cui.cpp \
           venusmmi\app\Cosmos\Dialer\vapp_dialer_main.cpp \
           plutommi\AppCore\SSC\sscstringhandle.c \
           plutommi\AppCore\SSC\sscoperate.c \
           plutommi\AppCore\SSC\sscpassengine.c \
           venusmmi\app\Cosmos\Calculator\vapp_calculator.cpp \
           venusmmi\app\Cosmos\Calculator\vapp_calculator_logical.cpp \
           venusmmi\app\Cosmos\CustomerService\vapp_customer_service.cpp \
           venusmmi\app\Cosmos\PhonebookManager\vapp_phonebook_manager.cpp \
           venusmmi\app\Cosmos\Converter\vapp_converter.cpp \
           venusmmi\app\Cosmos\Notes\Vapp_notes_app.cpp \
           venusmmi\app\Cosmos\Notes\Vapp_notes_editor.cpp \
           venusmmi\app\Cosmos\Notes\Vapp_notes_list.cpp \
           venusmmi\app\Cosmos\Notes\Vapp_notes_base.c \
           venusmmi\app\Cosmos\Notes\Vapp_notes_utils.c \
           venusmmi\app\Cosmos\CBApp\vapp_cbs_setting_page.cpp \
           venusmmi\app\Cosmos\CBApp\vapp_cbs_msgs_page.cpp \
           venusmmi\app\Cosmos\CBApp\vapp_cbs_main.cpp \
           venusmmi\app\Cosmos\CBApp\vapp_cbs_language_page.cpp \
           venusmmi\app\Cosmos\CBApp\vapp_cbs_channel_page.cpp \
           venusmmi\app\Cosmos\CBApp\vapp_cbs_channel_editor_page.cpp \
           venusmmi\app\Cosmos\CBApp\vapp_cbs_new_msg.cpp \
           venusmmi\app\Cosmos\CBApp\vapp_cbs_viewer_page.cpp \
           venusmmi\app\Cosmos\RightsMgr\Vapp_rmgr.cpp \
           venusmmi\app\Cosmos\RightsMgr\Vapp_rmgr_adv.cpp \
           venusmmi\app\Cosmos\RightsMgr\Vapp_rmgr_cntx.c \
           venusmmi\app\Cosmos\RightsMgr\Vapp_rmgr_service.c \
           venusmmi\app\Cosmos\RightsMgr\Vapp_rmgr_sweep.c \
           venusmmi\app\Cosmos\RightsMgr\Vapp_rmgr_util.cpp \
           venusmmi\app\Cosmos\setting\language_input_method\vapp_input_method_setting.cpp \
           venusmmi\app\Cosmos\OomScr\vapp_oom.cpp \
           venusmmi\app\Cosmos\Message\vapp_prov_um_sap.cpp \
           venusmmi\app\Cosmos\Message\vapp_prov_util.cpp \
           venusmmi\app\Cosmos\Message\vapp_prov_viewer.cpp \
           plutommi\mmi\datetime\datetimesrc\datetime.c

ifneq ($(filter __MMI_CLOG_MARK_SEVERAL__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\CallLog\vapp_clog_multi_delete.cpp
endif

SRC_LIST += venusmmi\app\Cosmos\Widget\vapp_widget_gadget_factory.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_baidu_search.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_quick_switch.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_music_player.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_3d_clock.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_3d_music_player.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_fm_radio.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_calendar.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_vip_contact.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_clock.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_gallery.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_gallery_setting.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_gallery_slide_show_interval_setting.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_gallery_viewer.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_gallery_control_bar.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_sns.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_sns_dummy.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_sns_cp.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_sns_agent.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_sns_view.cpp \
	    venusmmi\app\Cosmos\Widget\vapp_widget_sns_page.cpp \
            venusmmi\app\Cosmos\Widget\vapp_widget_3d_pet.cpp
ifneq ($(filter __GADGET_SUPPORT__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\Widget\vapp_widget_delegator.cpp
endif

ifneq ($(filter __TCPIP__, $(strip $(MODULE_DEFS))),)
ifneq ($(filter __MMI_CBM_CONNECTION_MANAGER__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\setting\network_connectivity\vapp_cnmgr_setting.cpp
endif
endif

ifneq ($(filter __MMI_NITZ__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\setting\nitz\vapp_nitz.cpp
endif

ifneq ($(filter __NTPD_SUPPORT__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\setting\ntp\vapp_ntp_main.cpp
endif

ifneq ($(filter __MMI_MExE_SUPPORT__, $(strip $(MODULE_DEFS))),)
  SRC_LIST += plutommi\mmi\sat\satsrc\mexe.c
endif

ifneq ($(filter __MMI_EM_MISC_TOUCH_SCREEN__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\GestureTest\vapp_gesture_test.cpp
endif

ifneq ($(filter __OP02_0200__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\OPMmApp\Vapp_op_mm_util.c \
            venusmmi\app\Cosmos\OPMmApp\OP02MmApp\Vapp_op02_mm.c
endif

ifneq ($(filter __OP01_3G__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\OPMmApp\Vapp_op_mm_util.c \
            venusmmi\app\Cosmos\OPMmApp\OP01MmApp\Vapp_op01_mm.c
endif

ifneq ($(filter __MMI_OP02_LEMEI_FTO__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\Lemei\Vapp_lemei.cpp
endif

ifneq ($(filter __MMI_VIDEO_TELEPHONY__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\CallSetting\vapp_callset_vt.cpp 
endif

ifneq ($(filter __MMI_CALLSET_CUG__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\CallSetting\vapp_callset_cug.cpp 
endif

ifneq ($(filter __CTM_SUPPORT__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\CallSetting\vapp_callset_ctm.cpp 
endif

SRC_LIST += venusmmi\app\Cosmos\SC_game\vapp_sc_game_help.cpp
#  __SC_SHOOT_BASKET_GAME__
SRC_LIST += venusmmi\app\Cosmos\SC_game\vapp_shoot_basket_game.cpp
#  __SC_ROULETTE_GAME__
SRC_LIST += venusmmi\app\Cosmos\SC_game\vapp_roulette_game.cpp
#  __SC_MAGIC_BRUSH_GAME__
SRC_LIST += venusmmi\app\Cosmos\SC_game\vapp_magic_brush_browser.cpp

ifneq ($(filter __MMI_ENGINEER_MODE__, $(strip $(MODULE_DEFS))),)
SRC_LIST += plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeAudio.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeBT.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeDebugInfo.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeDevice.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeDM.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeFRMProfiling.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeFSTestCore.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeFSTestUI.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeGPRS.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeGPS.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeInetApp.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeIperf.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeMiniGPS.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeMisc.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeMMIFramework.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeMotion.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeMultimedia.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeCamera.cpp \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModePCSNetwork.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModePMIC.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeProfiling.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeRFtest.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeSocketApp.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeSocketDemoApp.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeStorage.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeVOIP.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeBTRFtest.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeLCD.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeNWEvent.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeNWInfo.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeWLAN.c \
           plutommi\mtkapp\EngineerMode\EngineerModeEngine\EngineerModeEngineSrc\EngineerModeMain.c \
           plutommi\mtkapp\EngineerMode\EngineerModeEngine\EngineerModeEngineSrc\EngineerModeUtil.c \
           plutommi\mtkapp\EngineerMode\EngineerModeEngine\EngineerModeEngineSrc\EngineerModeCommUI.c \
           plutommi\mtkapp\EngineerMode\EngineerModeEngine\EngineerModeEngineSrc\EngineerModeCommMem.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeSSPSettings.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerMode3GInfo.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeTouchPanel.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeConfTest.c \
           venusmmi\app\Cosmos\EngineerMode\vapp_engineer_mode.cpp \
           venusmmi\app\Cosmos\EngineerMode\vapp_em_dev_lcd.cpp \
           venusmmi\app\Cosmos\EngineerMode\vapp_em_nw_sel_ring.cpp \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModePXS.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeGDIProfiling.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeRecorderSetting.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeNfc.c \
           plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppSrc\EngineerModeCnmgr.c
endif

ifneq ($(filter __MMI_FACTORY_MODE__, $(strip $(MODULE_DEFS))),)
SRC_LIST += plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeAudio.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeDevice.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeKeypad.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeMain.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeMedia.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeMisc.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeNxMPoint.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeParallelLine.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeQuickTest.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeQuickTstSet.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeTC01.c\
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeTouchPanel.c \
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeUtil.c \
           plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeNFC.c
endif

ifneq ($(filter __MMI_FM_TC01_CAMERA_TEST__, $(strip $(MODULE_DEFS))),)
SRC_LIST += plutommi\mtkapp\FactoryMode\FactoryModeSrc\FactoryModeTC01.c
endif

ifneq ($(filter __GPS_SUPPORT__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\GPSSetting\vapp_gpssetting.cpp \
           venusmmi\app\Cosmos\GPSSetting\vapp_gps_mgr.cpp \
           venusmmi\app\Cosmos\GPSSetting\vapp_gps_agpssuplotap.cpp \
           plutommi\mtkapp\GPS\GPSSrc\GPSMgr.c \
           plutommi\mtkapp\GPS\GPSSrc\GPSSetting.c \
           plutommi\mtkapp\GPS\GPSSrc\GPSMgrAGPSCP.c \
           plutommi\mtkapp\GPS\GPSSrc\GPSMgrAGPSUP.c \
           plutommi\mtkapp\GPS\GPSSrc\gps_ate.c
endif

ifneq ($(filter __MMI_COSMOS_DOWNLOADABLE_THEMES_SUPPORT__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\Theme\vapp_theme_dlt.cpp

ifneq ($(filter __MMI_VUI_DOWNLOADABLE_APP_ICON__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\Theme\vapp_theme_appicon.cpp
SRC_LIST += venusmmi\app\Cosmos\Theme\vapp_theme_appicon_dlt.cpp
SRC_LIST += venusmmi\app\Cosmos\AppIcon\vapp_app_icon.cpp
endif

endif

ifneq ($(filter __MMI_SAFE_MODE__, $(strip $(MODULE_DEFS))),)
SRC_LIST += venusmmi\app\Cosmos\SafeMode\vapp_safe_mode.cpp
endif

# Define include path lists to INC_DIR
INC_DIR = vendor\google_app\venusmmi\GoogleWidget \
          venusmmi\vrt\interface \
          venusmmi\framework \
          venusmmi\framework\interface \
          venusmmi\visual \
          venusmmi\visual\interface \
          venusmmi\app\common \
          venusmmi\app\common\interface \
          venusmmi\app\common\interface\service \
          venusmmi\app\common\interface\cp \
          venusmmi\app\common\clipboard \
          venusmmi\app\common\wallpaper \
          venusmmi\app\common\platform \
          venusmmi\app\common\message \
          venusmmi\app\common\archive \
          venusmmi\app\common\widget \
          venusmmi\app\common\launcher \
          venusmmi\app\common\screen_lock \
          venusmmi\app\pluto_variation\adapter \
          venusmmi\app\pluto_variation\adapter\interface \
          venusmmi\app\pluto_variation\adapter\interface\res \
          venusmmi\test\common \
          venusmmi\test\app \
          venusmmi\app \
          venusmmi\app\pluto_variation \
          venusmmi\framework\mmi_core \
          venusmmi\framework\ui_core\base \
          venusmmi\framework\ui_core\mvc \
          venusmmi\framework\ui_core\pme \
          venusmmi\framework\xml \
          venusmmi\app\common\data \
          venusmmi\app\common\ime \
          venusmmi\app\pluto_variation\adapter\interface\trc \
          venusmmi\app\pluto_variation\adapter\Framework \
          venusmmi\visual\cp \
          venusmmi\visual\cp\effect \
          venusmmi\visual\custom \
          venusmmi\visual\custom\mtk \
          venusmmi\app\Cosmos\Launcher \
          venusmmi\app\Cosmos\LauncherCosmos\Home \
          venusmmi\app\Cosmos\LauncherCosmos\MM \
          venusmmi\app\Cosmos\LauncherRobot\Home \
          venusmmi\app\Cosmos\LauncherRobot\Mainmenu \
          venusmmi\app\Cosmos\LauncherMM \
          venusmmi\app\Cosmos\Widget \
          venusmmi\app\Cosmos\Gallery \
          venusmmi\app\Cosmos\WidgetDelegator \
          vendor\widget\google\adaptation \
          venusmmi\app\Cosmos\ScreenLock \
          venusmmi\app\Cosmos\ScreenLockSteam \
          venusmmi\app\Cosmos\ScreenLockCosmos \
          venusmmi\app\Cosmos\ScreenLockPattern \
          venusmmi\app\Cosmos\ScreenLockPainting \
          venusmmi\app\Cosmos\ScreenLockZipper \
          venusmmi\app\Cosmos\AppManager \
          venusmmi\app\Cosmos\CallSetting \
          venusmmi\app\Cosmos\SupplementaryService \
          venusmmi\app\Cosmos\CallLog \
          venusmmi\app\Cosmos\DeviceApp \
          venusmmi\app\Cosmos\Bluetooth \
          venusmmi\app\Cosmos\NCenter \
          venusmmi\app\Cosmos\interface\cui \
          venusmmi\app\Cosmos\interface\app \
          venusmmi\app\Cosmos\interface\cp \
          venusmmi\app\Cosmos\dataaccount \
          venusmmi\app\Cosmos\FMRadio \
          venusmmi\app\Cosmos\MusicPlayer \
          venusmmi\app\Cosmos\OomScr \
          venusmmi\app\Cosmos\Theme \
          venusmmi\app\Cosmos\SafeMode \
          venusmmi\app\pluto_variation\adapter\ime \
          applib\inet\engine\include \
          applib\mem\include \
          applib\misc\include \
          applib\kazlib \
          applib\sqlite3\inc \
          drv\include \
          fmt\include \
          fs\fat\include \
          interface\hwdrv \
          interface\wifi \
          interface\fs \
          interface\ps\enum \
          interface\nfc \
          $(strip $(PS_FOLDER))\interfaces\enum \
          $(strip $(PS_FOLDER))\interfaces\local_inc \
          gps\inc \
          plutommi\MtkApp\AGPSLog\AGPSLogInc \
          plutommi\customer\customerinc \
          plutommi\customer\customize \
          plutommi\customer\custresource \
          plutommi\mmi\asyncevents\asynceventsinc \
          plutommi\mmi\inc \
          plutommi\Framework\Interface \
          plutommi\mmi\inc\menuid \
          plutommi\Framework\CommonFiles\commoninc \
          plutommi\Framework\EventHandling\eventsinc \
          plutommi\Framework\History\historyinc \
          plutommi\Framework\MemManager\memmanagerinc \
          plutommi\Framework\NVRAMManager\nvrammanagerinc \
          plutommi\Framework\Tasks\tasksinc \
          plutommi\Framework\ThemeManager\thememanagerinc \
          plutommi\Framework\GUI\gui_inc \
          plutommi\Framework\GUI\oem_inc \
          plutommi\cui\inc \
          plutommi\mmi\miscframework\miscframeworkinc \
          plutommi\Framework\GDI\gdiinc \
          plutommi\service\mdi\mdiinc \
          plutommi\mtkapp\mtkappinc \
          plutommi\Service\ImgEdtSrv \
          plutommi\Framework\InputMethod\Engine\Engine_Inc \
          plutommi\Framework\InputMethod\Inc \
          plutommi\Framework\InputMethod\UI\UI_Inc \
          plutommi\mmi\CubeApp\CubeAppInc \
          plutommi\mmi\ShellApp\ShellAppInc \
          plutommi\mtkapp\GPS\GPSInc \
          plutommi\AppCore\SSC \
          vendor\inputmethod\cstar\adaptation\include \
          vendor\inputmethod\t9\adaptation\include \
          btstacka\inc \
          custom\common\VENUS_MMI \
          drm\include \
          irda\inc \
          j2me\interface \
          j2me\jal\include \
          j2me\jal\include \
          media\image\include \
          media\rtp\inc \
          media\stream\include \
          media\mtv\src \
          media\mtv\include \
          usb\include \
          verno \
          wapadp\include \
          xmlp\include \
          plutommi\mmi\athandler\athandlerinc \
          plutommi\mmi\audio\audioinc \
          plutommi\Framework\BIDI\bidiinc \
          plutommi\mmi\browserapp\browser\browserinc \
          plutommi\mmi\browserapp\pushinbox\pushinboxinc \
          plutommi\Framework\CommonScreens\commonscreensinc \
          plutommi\mmi\cphs\cphsinc \
          plutommi\mmi\customerservice\customerserviceinc \
          plutommi\mmi\dataaccount\dataaccountinc \
          plutommi\mmi\datetime\datetimeinc \
          plutommi\Framework\DebugLevels\debuglevelinc \
          plutommi\mmi\dictionary\dictinc \
          plutommi\mmi\ebookreader\ebookinc \
          plutommi\mmi\emailapp\emailappinc \
          plutommi\mmi\extra\extrainc \
          plutommi\mmi\funandgames\funandgamesinc \
          plutommi\mmi\gpio\gpioinc \
          plutommi\service\gpiosrv \
          plutommi\mmi\gsmcallapplication\commonfiles\commoninc \
          plutommi\mmi\gsmcallapplication\incomingcallmanagement\incominginc \
          plutommi\mmi\gsmcallapplication\outgoingcallmanagement\outgoinginc \
          plutommi\mmi\help\helpinc \
          plutommi\mmi\idlescreen\idlescreeninc \
          plutommi\mmi\idlescreen\idlescreeninc\demoappinc \
          plutommi\MMI\Idle\IdleInc \
          plutommi\MMI\Dialer\DialerInc \
          plutommi\MMI\ScrLocker\ScrLockerInc \
          plutommi\MMI\ScrSaver\ScrSaverInc \
          plutommi\MMI\NwInfo\NwInfoInc \
          plutommi\mmi\SecuritySetting\SecSetInc \
          plutommi\Framework\IndicLanguages\indiclanguagesinc \
          plutommi\Framework\LangModule\LangModuleInc \
          plutommi\mmi\mainmenu\mainmenuinc \
          plutommi\mmi\messages \
          plutommi\mmi\messages\messagesinc \
          plutommi\mmi\mobileservice\mobileserviceinc \
          plutommi\Framework\MTE\mteinc \
          plutommi\Framework\MSLT\msltinc \
          plutommi\mmi\nitzhandler\nitzinc \
          plutommi\mmi\organizer\organizerinc \
          plutommi\mmi\phonebook\phonebookinc \
          plutommi\mmi\profiles\profilesinc \
          plutommi\mmi\resource\inc \
          plutommi\mmi\sat\satinc \
          plutommi\mmi\setting\settinginc \
          plutommi\mmi\smartmessage\smartmessageinc \
          plutommi\AppCore \
          plutommi\mmi\ucm\ucminc \
          plutommi\Service\Inc \
          plutommi\Service\UmSrv \
          plutommi\Service\UmmsSrv \
          plutommi\mmi\unifiedcomposer\unifiedcomposerinc \
          plutommi\mmi\unifiedmessage\unifiedmessageinc \
          plutommi\mmi\unifiedmms\mmsapp\mmsappinc \
          plutommi\mmi\voip\voipinc \
          plutommi\mtkapp\abrepeater\abrepeaterinc \
          plutommi\mtkapp\audioplayer\audioplayerinc \
          plutommi\mtkapp\MediaPlayer\MediaPlayerInc \
          plutommi\mtkapp\avatar\avatarinc \
          plutommi\mtkapp\barcodereader\barcodereaderinc \
          plutommi\mtkapp\bgsound\bgsoundinc \
          plutommi\mtkapp\camera\camerainc \
          plutommi\mtkapp\camcorder\camcorderinc \
          plutommi\mtkapp\connectivity\connectivityinc \
          plutommi\mtkapp\connectivity\connectivityinc\btcommon \
          plutommi\mtkapp\connectivity\connectivityinc\btmtk \
          plutommi\Framework\CSBrowser\csbrowserinc \
          plutommi\mtkapp\dlagent\dlagentinc \
          plutommi\mtkapp\dmuiapp\dmuiappinc \
          plutommi\mtkapp\filemgr\filemgrinc \
          plutommi\mtkapp\fmradio\fmradioinc \
          plutommi\mtkapp\fmschedulerec\fmschedulerecinc \
          plutommi\mtkapp\GPS\GPSInc \
          plutommi\mtkapp\javaagency\javaagencyinc \
          plutommi\mtkapp\mmiapi\include \
          plutommi\mtkapp\mobiletvplayer\mobiletvplayerinc \
          plutommi\mtkapp\dtvplayer\dtvplayerinc \
          plutommi\mtkapp\myfavorite\myfavoriteinc \
          plutommi\mtkapp\photoeditor\photoeditorinc \
          plutommi\mtkapp\rightsmgr\rightsmgrinc \
          plutommi\MtkApp\SoftwareTracer\SoftwareTracerInc \
          plutommi\mtkapp\soundrecorder\soundrecorderinc \
          plutommi\mtkapp\sndrec\sndrecinc \
          plutommi\mtkapp\answermachine\answermachineinc \
          plutommi\mtkapp\syncml\syncmlinc \
          plutommi\mtkapp\video\videoinc \
          plutommi\mtkapp\vobjects\vobjectinc \
          plutommi\mtkapp\vrsd\vrsdinc \
          plutommi\mtkapp\vrsi\vrsiinc \
          plutommi\mtkapp\MREAPP\MREAPPInc \
          plutommi\vendorapp\devapp\devappinc \
          plutommi\Service\PlstSrv \
          plutommi\Service\MediaCacheSrv \
          vendor\player\ipeer\inc \
          plutommi\mmi\ipsecapp\ipsecappinc \
          plutommi\mmi\UDX\UDXInc \
          plutommi\MMI\Bootup\BootupInc \
          plutommi\Service\inc \
          plutommi\mmi\CallLog\CallLogInc \
          plutommi\Service\ProfilesSrv \
          plutommi\CUI\ImageViewCui \
          plutommi\MMI\PhoneBook\Core \
          plutommi\Service\WapPushSrv \
          plutommi\Service\AppMgrSrv \
          venusmmi\app\Cosmos\FontMgr\
          venusmmi\app\Cosmos\FileMgr \
          venusmmi\app\Cosmos\ucm \
          venusmmi\app\Cosmos\Message \
          venusmmi\app\pluto_variation\ShellApp\panel\MsgViewer \
          venusmmi\app\pluto_variation\adapter\MultiTouchTest \
          venusmmi\app\pluto_variation\MultiTouchTest \
          venusmmi\app\Cosmos\UnifiedComposer \
          venusmmi\app\Cosmos\WorldClock \
          venusmmi\app\Cosmos\Alarm \
          venusmmi\app\Cosmos\Sat \
          venusmmi\app\cosmos\Calendar \
          venusmmi\app\Cosmos\setting\setting_framework \
          venusmmi\app\Cosmos\setting\motion \
          venusmmi\app\cosmos\contact\ContactUI \
          venusmmi\app\cosmos\contact\ContactCore \
          venusmmi\app\cosmos\Calculator \
          venusmmi\app\Cosmos\JavaApp \
          venusmmi\app\cosmos\Dialer \
          venusmmi\app\Cosmos\Wlan \
          mmi_core\app\SupplementaryService \
          venusmmi\app\Cosmos\Notes \
          venusmmi\app\Cosmos\CBApp \
          venusmmi\app\Cosmos\RightsMgr \
          venusmmi\app\Cosmos\Dataaccount \
          vendor\opera\browser\adaptation\inc \
          plutommi\Service\ProvBoxSrv \
          venusmmi\app\Cosmos\Sync \
          venusmmi\app\Cosmos\Sync\RemoteSync \
          venusmmi\app\Cosmos\Sns \
          plutommi\Service\Inc \
          venusmmi\app\Cosmos\Tethering \
          plutommi\Service\UPPSrv \
          plutommi\mmi\BrowserApp\UPPApp\UPPAppInc \
          venusmmi\app\Cosmos\setting\network_connectivity \
          plutommi\Service\CnmgrSrv \
          venusmmi\app\Cosmos\GestureTest \
          interface\hal\display\common \
          hal\storage\mc\inc \
          venusmmi\app\Cosmos\Certman \
          meta\cct \
          plutommi\Service\DmSRsrv \
          plutommi\Service\JavaAppFactorySrv \
          venusmmi\app\Cosmos\PhonebookManager \
          venusmmi\app\Cosmos\CustomerService \
          venusmmi\app\Cosmos\WidgetBaiduSearch \
          venusmmi\app\Cosmos\Bootup \
          venusmmi\app\Cosmos\SecSet \
          interface\hal\video_codec \
          custom\drv\measure\Inc \
          custom\common\hal_public \
          plutommi\MtkApp\SearchWeb\SearchWebInc

INC_DIR += venusmmi\app\Cosmos\setting\launcher_screen_lock


INC_DIR += venusmmi\app\Cosmos\LauncherWin7\Home 
INC_DIR += venusmmi\app\Cosmos\LauncherWin7\SubMenu 
#__MMI_LAUNCHER_WIN7__


INC_DIR += venusmmi\app\Cosmos\ScreenLockPoker
#__MMI_VUI_SCREEN_LOCK_POKER__

ifneq ($(filter __MMI_ENGINEER_MODE__, $(strip $(MODULE_DEFS))),)
INC_DIR +=plutommi\mtkapp\EngineerMode\EngineerModeApp\EngineerModeAppInc \
           plutommi\mtkapp\EngineerMode\EngineerModeEngine\EngineerModeEngineInc \
           venusmmi\app\Cosmos\EngineerMode \
           interface\hal\fmr
endif

ifneq ($(filter __RF_DESENSE_TEST__, $(strip $(MODULE_DEFS))),)
  INC_DIR += BTMT\rf_desense\$(strip $(PLATFORM))
endif

ifneq ($(filter __MMI_FACTORY_MODE__, $(strip $(MODULE_DEFS))),)
INC_DIR +=plutommi\mtkapp\FactoryMode\FactoryModeInc
endif

ifeq ($(filter __MTK_TARGET__ , $(strip $(MODULE_DEFS))),)
INC_DIR += MoDIS_VC9\MoDIS  # for w32_utility.h
endif

ifneq ($(filter __MMI_OP02_LEMEI_FTO__, $(strip $(MODULE_DEFS))),)
INC_DIR +=venusmmi\app\Cosmos\Lemei
endif

ifneq ($(filter __OP02_0200__, $(strip $(MODULE_DEFS))),)
INC_DIR += venusmmi\app\Cosmos\OPMmApp
endif

ifneq ($(filter __OP01_3G__, $(strip $(MODULE_DEFS))),)
INC_DIR += venusmmi\app\Cosmos\OPMmApp
endif

INC_DIR += venusmmi\app\common\screen_lock

#  __SC_MAGIC_BRUSH_GAME__
INC_DIR += venusmmi\app\Cosmos\SC_game

# Define the specified compile options to COMP_DEFS
COMP_DEFS = MMI_ON_HARDWARE_P  \
            __EMS__ \
            SIM_APP_TOOLKIT \
            TETRIS_GAME_APP \
            CM_APPLICATION_ENABLE \
            MESSAGES_APPLICATION \
            __UCS2_ENCODING \
            CELLBROADCAST_APP \
            __CB__ \
            VM_SUPPORT \
            SHORTCUTS_APP \
            T9LANG_English \
            T9LANG_Chinese \
            T9LANG_Spanish \
            T9LANG_French \
            T9LANG_German \
            T9LANG_Italian \
            EMS_LITTLE_ENDIAN \
            EMS_MEM_MANAGEMENT \
            T9ALPHABETIC \
            T9CHINESE \
            MMS_VIRTUAL_FILE_SUPPORT \
            COMPILER_SUPPORTS_LONG=1 \
            __MTK__ \
            __MTK_PLUGIN_API_IMPORT__ \
            APCS_INTWORK
 
#tangjinxing add start
#ifneq ($(filter __CUSTOM_IPHONE_STYLE_TAB__, $(strip $(MODULE_DEFS))),)
SRC_LIST +=venusmmi\app\IphoneUI\visual\cp\Iphone_tab_bar.cpp \
		  	venusmmi\app\IphoneUI\visual\cp\Iphone_tab_title_bar.cpp \
		  	venusmmi\app\IphoneUI\visual\cp\Iphone_tabctrl_page.cpp
INC_DIR +=venusmmi\app\IphoneUI\visual\cp
SRC_PATH +=venusmmi\app\IphoneUI\visual\cp

##voicemail start
SRC_LIST +=venusmmi\app\IphoneUI\IphoneVoiceMail\IphoneVoiceMail.cpp
INC_DIR +=venusmmi\app\IphoneUI\IphoneVoiceMail
SRC_PATH +=venusmmi\app\IphoneUI\IphoneVoiceMail
##voicemail end

##favorite path start
SRC_LIST +=venusmmi\app\IphoneUI\Favorites\IphoneFavorites.cpp
INC_DIR +=venusmmi\app\IphoneUI\Favorites
SRC_PATH +=venusmmi\app\IphoneUI\Favorites
##favorite path end

##Password lock screen start
SRC_LIST +=venusmmi\app\IphoneUI\VappScreenIphoneUILockPassword\vapp_screen_iphoneUI_lock_password.cpp
INC_DIR +=venusmmi\app\IphoneUI\VappScreenIphoneUILockPassword
SRC_PATH +=venusmmi\app\IphoneUI\VappScreenIphoneUILockPassword
##Password lock screen end

##Iphone Lock Screen start
SRC_LIST +=venusmmi\app\IphoneUI\VappIphoneStyleLockScreen\Iphone_screen_lock_cosmos.cpp \
			venusmmi\app\IphoneUI\VappIphoneStyleLockScreen\Iphone_screen_lock_cosmos_frame.cpp \
			venusmmi\app\IphoneUI\VappIphoneStyleLockScreen\Iphone_screen_lock_cosmos_time.cpp \
						
INC_DIR +=venusmmi\app\IphoneUI\VappIphoneStyleLockScreen
SRC_PATH +=venusmmi\app\IphoneUI\VappIphoneStyleLockScreen
##Iphone Lock Screen end
#endif
#tangjinxing add end 

#__MMI_DSM_NEW__ start...
ifneq ($(filter __MMI_DSM_NEW__ , $(strip $(MODULE_DEFS))),)                  
	SRC_LIST += plutommi\mmi\mythroad\mythroadsrc\mrp_media.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_phonebook.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_fmgr.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_unet.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_core.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_extension.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_net.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_sms.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_fs.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_image.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_motion.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_backstage.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_misc.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_sysinfo.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_util.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_setting.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_nes.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_application.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_main.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_pal.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_factory.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_jsky.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_mpchat.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_stream_audio.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_camrecord.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_Resizer_mem.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_resizer_mem_6253_series.c \
							plutommi\mmi\mythroad\mythroadsrc\mrp_localui.cpp \
							plutommi\mmi\mythroad\mythroadsrc\vapp_mythroad_ncenter.cpp \
							plutommi\mmi\mythroad\mythroadsrc\vapp_mythroad.cpp

	INC_DIR += plutommi\mmi\mythroad\mythroadinc \
							plutommi\mmi\mythroad\mythroadinc\mrp \
							plutommi\Service\Dtcntsrv \
							hal\peripheral\inc \
							hal\display\common\include \
							venusmmi\app\Cosmos\Camcorder

	SRC_PATH += plutommi\mmi\mythroad\mythroadsrc
	
	ifneq ($(filter __HC_CALL_NEW__ , $(strip $(MODULE_DEFS))),)                  
	SRC_LIST += plutommi\mmi\Mythroad\HcCall\HcCallSrc\dsm_http.c \
							plutommi\mmi\mythroad\HcCall\HcCallSrc\dsm_socket.c \
							plutommi\mmi\mythroad\HcCall\HcCallSrc\HcCallIn_UI.c \
							plutommi\mmi\mythroad\HcCall\HcCallSrc\HcFileAdapter.c 
	INC_DIR += plutommi\mmi\mythroad\HcCall\HcCallInc
	SRC_PATH += plutommi\mmi\mythroad\HcCall\HcCallSrc
	endif
ifneq ($(filter __PME_SUPPORT__ , $(strip $(MODULE_DEFS))),)                  
	SRC_LIST += plutommi\mmi\pme\pmesrc\pme_porting.c \
				plutommi\mmi\pme\pmesrc\pme_ui.c 

endif
ifneq ($(filter __DSM_SPLAYER__ , $(strip $(MODULE_DEFS))),)                  
	SRC_LIST += plutommi\mmi\mythroad\mmsrc\sky_mm_resizer_mem.c \
				plutommi\mmi\mythroad\mmsrc\sky_mm_resizer_mem_6253_series.c \
				plutommi\mmi\mythroad\mmsrc\sky_mm_audio.c \
				plutommi\mmi\mythroad\mmsrc\sky_mm_port.c \
				plutommi\mmi\mythroad\mmsrc\sky_mm_splayer_display.c \
				plutommi\mmi\mythroad\mmsrc\sky_mm_main.c \
				plutommi\mmi\mythroad\mmsrc\sky_MM_VC_Platform.c
				

	INC_DIR += plutommi\mmi\mythroad\mminc \

	SRC_PATH += plutommi\mmi\mythroad\mmsrc
endif  

	INC_DIR += plutommi\mmi\pme\pmeinc

	SRC_PATH += plutommi\mmi\pme\pmesrc
endif

#__MMI_DSM_NEW__ end