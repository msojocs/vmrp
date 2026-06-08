#ifndef _DYNAMIC_MENU_API_H
#define _DYNAMIC_MENU_API_H

#ifdef DMENU_SUPPORT

typedef struct
{
uint16						versin;		//动态菜单版本
uint16						TotalMenu;		//动态菜单最大菜单数
}MMIDYNAMICMENU_HEAD_T;//动态菜单头的内容

typedef struct
{
	uint32					id;			//动态菜单编号
	uint32					pid;			//动态菜单父节点编号
	uint8                             type;		//'w'连WAP,'s'发信息，'m'菜单,'t'文本信息
	uint8					name_len;		//动态菜单名字的长度
	uint16					reserve;
	uint8*                              name;		//动态菜单名字字串信息
	uint32					dataLen	;		//url,number,sms content,text......长度。
	uint8*                              data;  	//号码短信内容，文本内容或url的内容
}MMIDYNAMICMENU_DETAIL_T;		//动态菜单每一项的内容

typedef struct
{

const uint8*			data;		//运营商对应的预置数组
uint8*			op_code;		//运营商号码
}DSM_SHEET_T;


typedef  struct _Node
{
  MMIDYNAMICMENU_DETAIL_T* data;
  struct _Node *next;		//下一个节点
  struct _Node *prev;		//上一个节点
  struct _Node *parent;	//父节点
  struct _Node *child;		//子节点集的第一个节点。
}DSM_NODE_T;

typedef enum
{
    DSM_MENU_ENTERY = 0,
    DSM_MENU_BACK = 1,
    DSM_MENU_REFRESH  = 2,
}DSM_MENU_TYPE;

#define DSM_MAX_List_ITEM	60

void dsmDymGotoBase(void);
uint8 * dsmDesScreenGetTitle(void);
void DSMPopupFail(void);

void DSMDescriptionGoBack(void);
void mmi_dynamic_get_index(int32 nIndex);
int dsmDymGetMenuItem(int8**items_list);
void dsmGoBackHistory(void);
void EntryDsmDynamicMenu(void);
 void dsm_left_handle(void);
 void sim_select_get_index(int32 nIndex);
 void EntryDynamicMenu(void);
 uint8 * dsmDymGetCurDescription(void);
 void DSMPopupNotSupport(void);

 void DsmProductDescription(void);
void DynamicMenu_connectWAP(char* wap_url);

uint8 Go2TopNode(void);
uint8 dsmGetActiveSim(void);
void DSMPopupNoSim(void);


//DSM_NODE_T* addnode(DSM_NODE_T *curNode,MMIDYNAMICMENU_DETAIL_T* tmp);
//DSM_NODE_T* CreateDSMTreeStruct();
//DSM_NODE_T* DSMGetFirstNode(DSM_NODE_T *node);
//BOOL DsmParseSmsData(MMIDYNAMICMENU_DETAIL_T*Pdata,char*num,char*content);
#endif
#endif
