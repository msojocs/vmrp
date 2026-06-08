
//#include "MMIDataType.h"
//#include "mmi_include.h"

//#include "Mmidsmapi.h"

//#define __SUPPORT_BIND_FUNC__
/*wuwenjie 2012-11-19 start*/
#define __KEYWORD_NO_POSTFIX__
/*wuwenjie 2012-11-19 end*/
void Debug_FilePrintf(const char *pcFormat, ...);
#define LOG(a) Debug_FilePrintf a
#ifdef __SUPPORT_BIND_FUNC__
#define DSM_BIND_FLAG  "bind"
#else
#define DSM_BIND_FLAG  "no_bind"
#endif

#ifdef __KEYWORD_NO_POSTFIX__
#define DSM_POSTFIX    "no_postfix"
#else
#define DSM_POSTFIX    "postfix"
#endif

#if !(defined(WIN32) ||defined(PLATFORM_SC6530))
#define __PLAT_BIG_ENDIAN___
#endif

#define DSM_VERSION_STRING  "dsm_version_"DSM_BIND_FLAG"_"DSM_POSTFIX

//#define DSM_DATA_PATH		"E:\\mythroad\\DSM\\"
#define DSM_MENU_TXT_LEN	50
#define MAX_PLMN_LEN_MMI    (6)
#define DSM_MAX_MENU_NAME_LEN   (60)
#define DSM_MASTER_SIM                                   (-1)
#define DSM_SLAVE_SIM                                      (-2)

#define MAX_CONTENT_LEN                                  (1024)

typedef unsigned int   uint32;
typedef signed int    int32;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned char uint8;
typedef signed char int8;

#define U8      uint8
#define U16     uint16
#define U32     uint32
#define	S8      int8
#define	S16     int16
#define	S32     int32
#define PU8     uint8*
#define PU16    uint16*
#define PS32    int32*

#define NULL  ((void *)0)
#define FALSE   (0)
#define TRUE      (1)
#define FS_READ_ONLY            0x00000001L //modify by zack@20121224
#define FS_NO_ERROR               (0)

#include "DynamicMenuApi.h"

typedef struct
{
	uint8 totalNum;             //sim numbers, max to 4
	uint8 curActive;            //index of simId[] witch actived
	uint16 dummy;              //no use, just for align
	int32 simId[4];              //mapping to plat sim socket id
	char* simNamePtr[4];   //sim卡名字,unicode
	uint16 simNameLen[4]; //名字长度
}T_DSM_DUALSIM_SET;

typedef struct
{
   uint32 width;                  //屏幕宽
   uint32 height;                 //屏幕高
   uint32 bit;                    //屏幕象素深度，单位bit
}mr_screeninfo;

typedef struct
{
	uint8       IMEI[16];	      //IMEI len eq 15
	uint8       IMSI[16];	      //IMSI len not more then 15
	char        manufactory[8];  //厂商名，最大7个字符，空字节填\0
	char        type[8];         //mobile type，最大7个字符，空字节填\0
	uint32      ver;            //SW ver
	uint8       spare[12];       //备用
}mr_userinfo;

extern T_DSM_DUALSIM_SET mr_OdsmDualSim;
static MMIDYNAMICMENU_HEAD_T dsmMenuHeadData = {0};
static DSM_NODE_T *Hilightnode = NULL;
static DSM_NODE_T *pRootNode =NULL;
static uint8  LoadDefault = FALSE;
static uint16 str_title[DSM_MAX_MENU_NAME_LEN]={0};  //产品描述标识
static uint16 root_title[DSM_MAX_MENU_NAME_LEN] ={0};
#ifdef WIN32
static int8  dsm_items_list[DSM_MAX_List_ITEM][DSM_MAX_MENU_NAME_LEN]={0}; 
#else
static __align(4) int8  dsm_items_list[DSM_MAX_List_ITEM][DSM_MAX_MENU_NAME_LEN]={0}; 
#endif
static DSM_SHEET_T *pMenuData =NULL;
static int menuDataSize = 0;
static uint16 sim_index=0;
static uint16 dsm_index=0;
static uint16 Hightlight_index=0;
static char GtmpBuf[MAX_CONTENT_LEN]={0};
static uint16 nodeoffset=0;


static uint8 MenuEntryType=0;

typedef int (*ReadFunc)(int fh,void *data,int size,int *readlen);

static ReadFunc pRead = NULL;
	
int (*pOpen)(char *filename,int type);
//int (*pWrite)(int fh,void *data,int size,int *readlen);
//int (*pSeek)(int ,int ,int);
int (*pClose)(int fh);
int gsmOnly=0;
/*========================================================================================================*/

//extern int FS_Read(int32 FileHandle, void *DataPtr, uint32 Length, uint32 *Read);
extern int32 MMIDMENU_read(int32 f,void *p,uint32 l,int32 *readlen);
extern int32 MMIDMENU_close(int32 f);
extern int32 MMIDMENU_open(const char* filename,  uint32 mode);
//extern int FS_Close(int32 FileHandle);
static void Dmenu_mmi_ucs2cpy(char *des,char*str);
static int dsmWstrlen(char *str);
static int16 ShortBigLittleEndSwitch(int16 data)
{
#ifdef __PLAT_BIG_ENDIAN___
	uint8 d0,d1;
	d0 = (data&0x00ff);
	d1 = ((data&0xff00)>>8);
	return (int16)((d0<<8)+d1);
#else
	return data;
#endif
}

static int IntBigLittleEndSwitch(int data)
{
#ifdef __PLAT_BIG_ENDIAN___
	uint8 d0,d1,d2,d3;
	d0 = (data&0x000000ff);
	d1 = ((data&0x0000ff00)>>8);
	d2 = ((data&0x00ff0000)>>16);
	d3 = ((data&0xff000000)>>24);
	return (int)((d0<<24)+(d1<<16)+(d2<<8)+d3);
#else
	return data;
#endif
}
void SetSupportGSMOnly(int gsm)
{
	gsmOnly=gsm;
}

void SetMenuEntryType(int type)
{
	MenuEntryType=type;
}

static void UCS2ASCII(char *w_in, char *a_out)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    int i = 0;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/
    while (w_in[i * 2] != '\0' || w_in[i * 2 + 1] != '\0')
    {
#ifndef __PLAT_BIG_ENDIAN___
	a_out[i] = w_in[i * 2];
#else
        a_out[i] = w_in[i * 2+1];
#endif
        i += 1;
    }
    a_out[i] = '\0';
}

int8* StringBigEndianToLittleEndian(int8* dstBuffer, int8* srcBuffer)
{
    /*----------------------------------------------------------------*/
    /* Local Variables                                                */
    /*----------------------------------------------------------------*/
    uint32 i = 0;
    uint32 len;

    /*----------------------------------------------------------------*/
    /* Code Body                                                      */
    /*----------------------------------------------------------------*/

    len = dsmWstrlen(srcBuffer) * 2;
	
    while (i != len)
    {
        dstBuffer[i] = srcBuffer[i + 1];
        dstBuffer[i + 1] = srcBuffer[i];
        i += 2;
    }
    dstBuffer[i] = '\0';
    dstBuffer[i + 1] = '\0';

    return dstBuffer;
}

uint8 Go2TopNode()
	
{
	if(Hilightnode){
	 while(Hilightnode->prev!=NULL)
	 	{
	 	Hilightnode=Hilightnode->prev;
	 	}
	}
	return 0;
}

void GoToLastChild(DSM_NODE_T *node)
{
	while(node->child!=NULL)
 	{
	 	node=node->child;
 	}
	while(node->next!=NULL)
	{
		node=node->next;
	}
}
static DSM_NODE_T* DSMGetFirstNode(DSM_NODE_T *node)
{
	while(node->parent!=NULL)
	{
		node=node->parent;
	}
	
	while(node->prev!=NULL)
	{
		node=node->prev;
	}
	
	return node;
}

static DSM_NODE_T* DSMGetLastNode(DSM_NODE_T *node)
{
	while(node->next!=NULL)
	{
		node=node->next;
	}
	
	while(node->child!=NULL)
	{
		node=node->child;
	}
	return node;
}

/*====================================================================================================*/
uint8 *pdsmFile = NULL;
int dsmFilePos = 0;
int dsmFileSize =0;

int dsm_open_file(char *filename,int type)
{
	uint32 i;
	
	if(pdsmFile||!pMenuData||menuDataSize<=0)
		return -1;
	
	dsmFilePos=0;
	
	for(i=0;i<menuDataSize;i++)
	{
		if(strcmp((char*)pMenuData[i].op_code, (char*)filename)==0)
		{
			pdsmFile=(uint8*)pMenuData[i].data;
			return i+1;
		}
	}
	pdsmFile = NULL;
	return 0;
}

int dsm_read_file(int fh,void *data,int size,int *readlen)
{
	if(fh<=0||data ==NULL||size<0||pdsmFile==NULL)
		return -1;
	if(size ==0)
		return 0;
		*readlen = size;
	memcpy(data,pdsmFile+dsmFilePos,*readlen);
	dsmFilePos += (*readlen);
	return 0;
}

int dsm_write_file(int fh,void *data,int size,int *readlen)
{
	return -1;
}

int dsm_close_file(int fh)
{
	if(fh>0&&pdsmFile!=NULL)
	{
		pdsmFile = NULL;
		dsmFilePos = 0;
		return 0;
	}
	else
	{
		return -1;
	}
}

static DSM_NODE_T* addnode(DSM_NODE_T *curNode,MMIDYNAMICMENU_DETAIL_T* tmp)
{
	DSM_NODE_T * Newnode;

	
	Newnode= (DSM_NODE_T *)MMIDMENU_memMalloc(sizeof(DSM_NODE_T));
	memset(Newnode, 0, sizeof(DSM_NODE_T));

	Newnode->data=(MMIDYNAMICMENU_DETAIL_T*)MMIDMENU_memMalloc(sizeof(MMIDYNAMICMENU_DETAIL_T));
	memcpy(Newnode->data, tmp, sizeof(MMIDYNAMICMENU_DETAIL_T));
	
	if(tmp->pid== curNode ->data->pid)
	{
		curNode->next= Newnode;
		Newnode ->prev=curNode;
		Newnode ->parent= curNode->parent;
		curNode = Newnode;
		return curNode;
	}
	else if(tmp->pid== curNode ->data->id)
	{
		Newnode ->parent= curNode;
		curNode->child= Newnode;
		curNode = Newnode;
		return curNode;
	}
	else
	{
		while(curNode->parent!=NULL)
		{	
			if(tmp->pid== curNode ->data->pid)
			{
				curNode->next= Newnode;
				Newnode ->prev=curNode;
				Newnode ->parent= curNode->parent;
				curNode = Newnode;
				return curNode;
				break;
			}
			else if(tmp->pid== curNode ->data->id)
			{
				Newnode ->parent= curNode;
				curNode->child= Newnode;
				curNode = Newnode;
				return curNode;
				break;
			}
			curNode=curNode->parent;
		}
		curNode->next= Newnode;
		Newnode ->prev=curNode;
		curNode=Newnode;
		return curNode;
	}
}


static DSM_NODE_T* DeleteOneNode(DSM_NODE_T* nod)
{
	DSM_NODE_T *curNode=NULL;

	if(nod->next!=NULL)
	{
		curNode=nod->next;
	}
	else if(nod->child!=NULL)
	{
		curNode=nod->child;
	}
	else if(nod->child==NULL&&nod->prev!=NULL)
	{
		curNode=nod->prev;
		curNode->next=NULL;
		MMIDMENU_memFree(nod);
	}
	else if(nod->child==NULL&&nod->prev==NULL)
	{
		if(nod->parent!=NULL)
		{
			curNode=nod->parent;
			curNode->child=NULL;
		}
		MMIDMENU_memFree(nod);
	}
	return curNode;
		
}

static void dsmFreeNodeMem(DSM_NODE_T *pNode)
{
	if(pNode)
	{
		if(pNode->data)
		{
			if(pNode->data->data)
				MMIDMENU_memFree((void *)pNode->data->data);
			if(pNode->data->name)
				MMIDMENU_memFree((void *)pNode->data->name);
			MMIDMENU_memFree((void *)pNode->data);
		}
		MMIDMENU_memFree((void *)pNode);
	}
}

static void dsmFreeNode(DSM_NODE_T *pNode)
{
	if(pNode)
	{
		dsmFreeNode(pNode->next);
		dsmFreeNode(pNode->child);
		dsmFreeNodeMem(pNode);
	}
}

void DestoryDSMTreeStruct(void )
{
	DSM_NODE_T *curNode;

	if(!pRootNode)
		return;
	
	dsmFreeNode(pRootNode);
	pRootNode = NULL;
}

extern int32 dsmIsSlaveSimSelected(void);

static  MMIDYNAMICMENU_DETAIL_T ReadOneNodeContent(int32 handle)
{
	
	MMIDYNAMICMENU_DETAIL_T  temp={0};
	int RealGetSize =0;

	pRead(handle, (void *)&temp, 12, &RealGetSize);
	temp.id = IntBigLittleEndSwitch(temp.id);
	temp.pid = IntBigLittleEndSwitch(temp.pid);
	temp.dataLen = IntBigLittleEndSwitch(temp.dataLen);
	temp.reserve = ShortBigLittleEndSwitch(temp.reserve);
	temp.name=(uint8*)MMIDMENU_memMalloc(temp.name_len+2);
	pRead(handle, (void *)GtmpBuf, temp.name_len, &RealGetSize);
	*(GtmpBuf+temp.name_len)='\0';
	*(GtmpBuf+temp.name_len+1)='\0';
#ifndef __PLAT_BIG_ENDIAN___ 	
	StringBigEndianToLittleEndian((int8*)temp.name,(int8*)GtmpBuf);
#else
	Dmenu_mmi_ucs2cpy((char *)temp.name, (char *) GtmpBuf);
#endif
	
	if(temp.type!='m')
	{
		pRead(handle, (void *)&(temp.dataLen), 4, &RealGetSize);
		temp.dataLen=IntBigLittleEndSwitch(temp.dataLen);
		temp.data=(uint8*)MMIDMENU_memMalloc(temp.dataLen+2);
		memset(temp.data, 0, temp.dataLen+2);
		pRead(handle, (void *)GtmpBuf, temp.dataLen, &RealGetSize);
		*(GtmpBuf+temp.dataLen)='\0';
		*(GtmpBuf+temp.dataLen+1)='\0';
#ifndef __PLAT_BIG_ENDIAN___		
		StringBigEndianToLittleEndian((int8*)temp.data,(int8*)GtmpBuf);
#else
		Dmenu_mmi_ucs2cpy((char *)temp.data, (char *) GtmpBuf);
#endif
	}
	nodeoffset++;
	return temp;
}

static DSM_NODE_T* GetOneNode(int32 handle,uint16 Total)
{
	MMIDYNAMICMENU_DETAIL_T  temp={0};
	DSM_NODE_T *curNode =NULL;

	memset(&temp,0,sizeof(temp));
	temp=ReadOneNodeContent(handle);
	if(gsmOnly==1)
	{
		while((temp.type=='w')&&nodeoffset<Total)
		{
			temp=ReadOneNodeContent(handle);
		}
		if(temp.type=='w')
			return NULL;
	}

	curNode=(DSM_NODE_T*)MMIDMENU_memMalloc(sizeof(DSM_NODE_T));
	memset(curNode, 0, sizeof(DSM_NODE_T));
	curNode->data=(MMIDYNAMICMENU_DETAIL_T*)MMIDMENU_memMalloc(sizeof(MMIDYNAMICMENU_DETAIL_T));
	memcpy(curNode->data, &temp, sizeof(MMIDYNAMICMENU_DETAIL_T));
	return curNode;
}

static DSM_NODE_T* CreateDSMTreeStruct(void)
{
	int32 handle = 0;
	MMIDYNAMICMENU_DETAIL_T  temp={0};
	MMIDYNAMICMENU_HEAD_T *filehead =NULL;
	DSM_NODE_T *firstNode=NULL,*curNode =NULL;
	uint16 Name_WChar[100] ={0};
	uint32 filesize =0;
	uint16 i =0;
	char dsmfilename[15] ={0};
	//char tmpBuf[200]={0};
	int RealGetSize =0;
	char plmn[7] ={0};
	
	nodeoffset=0;

	MMIDMENU_getActivedPlmn(plmn);
	
	sprintf(dsmfilename,"%s%s%s","DSM_",plmn,".dat");
	//kal_wsprintf(Name_WChar, "%s%s", DSM_DATA_PATH,  dsmfilename);
		
	//if ((FS_CheckFile((uint16 *) Name_WChar)) >= FS_NO_ERROR)
	if(handle = MMIDMENU_open(dsmfilename,FS_READ_ONLY))
	{
		//handle = MMIDMENU_open((uint16 *) dsmfilename, FS_READ_ONLY);
		pRead = (ReadFunc)MMIDMENU_read;
		//pWrite = FS_Write;
		pClose = MMIDMENU_close;
	}
	else
	{		
		handle = dsm_open_file((char*)plmn,FS_READ_ONLY);
		pRead = dsm_read_file;
		//pWrite = dsm_write_file;
		pClose = dsm_close_file;
	}

	if(handle<=0)
		return NULL;

		memset(&dsmMenuHeadData,0,sizeof(dsmMenuHeadData));
		memset(&temp,0,sizeof(temp));
		
		filehead = &dsmMenuHeadData;
		
		pRead(handle, (void *)filehead, sizeof(MMIDYNAMICMENU_HEAD_T), &RealGetSize);
		
		filehead->TotalMenu = ShortBigLittleEndSwitch(filehead->TotalMenu);
		filehead->versin = ShortBigLittleEndSwitch(filehead->versin);  
		
		firstNode=GetOneNode(handle,filehead->TotalMenu);
		/*
		pRead(handle, (void *)&temp, 12, &RealGetSize);
		temp.name=(U8*)DsmMalloc(temp.name_len+2);
		pRead(handle, (void *)tmpBuf, temp.name_len, &RealGetSize);
		*(tmpBuf+temp.name_len)='\0';
		*(tmpBuf+temp.name_len+1)='\0';
		StringBigEndianToLittleEndian((PS8)temp.name,(PS8)tmpBuf);		
		if(temp.type!='m')
		{
			pRead(handle, (void *)&(temp.dataLen), 4, &RealGetSize);
			temp.data=(U8*)DsmMalloc(temp.dataLen+2);
			memset(temp.data, 0, temp.dataLen+2);
			pRead(handle, (void *)tmpBuf, temp.dataLen, &RealGetSize);
			*(tmpBuf+temp.dataLen)='\0';
			*(tmpBuf+temp.dataLen+1)='\0';
			StringBigEndianToLittleEndian((PS8)temp.data,(PS8)tmpBuf);
		}

		//if((gsmOnly==0)||(temp.type!='w'))
		//{
		firstNode=(DSM_NODE_T*)DsmMalloc(sizeof(DSM_NODE_T));
		memset(firstNode, 0, sizeof(DSM_NODE_T));
		firstNode->data=(MMIDYNAMICMENU_DETAIL_T*)DsmMalloc(sizeof(MMIDYNAMICMENU_DETAIL_T));
		memcpy(firstNode->data, &temp, sizeof(MMIDYNAMICMENU_DETAIL_T));
		//}*/

		curNode=GetOneNode(handle,filehead->TotalMenu);

		if(curNode==NULL)
			{
			pClose(handle);
			return firstNode;
			}
		
		//memset(&temp,0,sizeof(temp));
		//temp=ReadOneNodeContent(handle);
		/*
		pRead(handle, (void *)&temp, 12, &RealGetSize);
		temp.name=(U8*)DsmMalloc(temp.name_len+2);
		pRead(handle, (void *)tmpBuf, temp.name_len, &RealGetSize);
		*(tmpBuf+temp.name_len)='\0';
		*(tmpBuf+temp.name_len+1)='\0';
		StringBigEndianToLittleEndian((PS8)temp.name,(PS8)tmpBuf);
		
		if(temp.type!='m')
		{
			pRead(handle, (void *)&(temp.dataLen), 4, &RealGetSize);
			temp.data=(U8*)DsmMalloc(temp.dataLen+2);
			memset(temp.data, 0, temp.dataLen+2);
			pRead(handle, (void *)tmpBuf, temp.dataLen, &RealGetSize);
			*(tmpBuf+temp.dataLen)='\0';
			*(tmpBuf+temp.dataLen+1)='\0';
			StringBigEndianToLittleEndian((PS8)temp.data,(PS8)tmpBuf);
		}

		//if((gsmOnly==0)||(temp.type!='w'))
		//{
		curNode=(DSM_NODE_T*)DsmMalloc(sizeof(DSM_NODE_T));
		memset(curNode, 0, sizeof(DSM_NODE_T));
		curNode->data=(MMIDYNAMICMENU_DETAIL_T*)DsmMalloc(sizeof(MMIDYNAMICMENU_DETAIL_T));
		memcpy(curNode->data, &temp, sizeof(MMIDYNAMICMENU_DETAIL_T));
		//}*/	
		
		memset(&temp,0,sizeof(temp));
		
		if(curNode->data->pid==0)
		{
			curNode->prev=firstNode;
			firstNode->next=curNode;
		}
		else
		{
			curNode->parent=firstNode;
			firstNode->child=curNode;
		}
		
		//for(i=0;i<filehead->TotalMenu-nodeoffset;i++)
		while(nodeoffset<filehead->TotalMenu)
		{
			memset(&temp,0,sizeof(temp));
			temp=ReadOneNodeContent(handle);
			/*
			pRead(handle, (void *)&temp, 12, &RealGetSize);
			temp.name=(U8*)DsmMalloc(temp.name_len+2);
			pRead(handle, (void *)tmpBuf, temp.name_len, &RealGetSize);
			*(tmpBuf+temp.name_len)='\0';
			*(tmpBuf+temp.name_len+1)='\0';
			
			StringBigEndianToLittleEndian((PS8)temp.name,(PS8)tmpBuf);
			
			if(temp.type!='m')
			{
				pRead(handle, (void *)&(temp.dataLen), 4, &RealGetSize);
				temp.data=(U8*)DsmMalloc(temp.dataLen+2);
				memset(temp.data, 0, temp.dataLen+2);
				pRead(handle, (void *)tmpBuf, temp.dataLen, &RealGetSize);
				*(tmpBuf+temp.dataLen)='\0';
				*(tmpBuf+temp.dataLen+1)='\0';
				StringBigEndianToLittleEndian((PS8)(temp.data),(PS8)tmpBuf);
			}*/
			if(gsmOnly==1)
			{
			while((temp.type=='w')&&nodeoffset<filehead->TotalMenu)
				{
				temp=ReadOneNodeContent(handle);
				}
			if(temp.type=='w')
				{
				pClose(handle);
				return firstNode;
				}
			}
			curNode=addnode(curNode,&temp);//*/
		}
		
		pClose(handle);

		return firstNode;	
}

static int dsmWstrlen(char *str)
{
	int i =0;

	if(!str)
		return 0;

	while(str[i]||str[i+1])
	{
		i+=2;
	}
	
	return i/2;
}

static void Dmenu_mmi_ucs2ncpy(char *strDestination, const char *strSource, U32 size)
{
	int i= 0;
	
	if(strDestination == 0||strSource==0)
		return;
	i = dsmWstrlen(strSource);
	if(i>size)
		i=size;
	memcpy(strDestination,strSource,i*2);
}

static void Dmenu_mmi_ucs2cpy(char *des,char*str)
{
	if(!des||!str)
		return;
	memcpy(des,str,(dsmWstrlen(str)+1)*2);
}

static U16 mmi_ucs2_n_to_asc(char *pOutBuffer, char *pInBuffer, U32 len)
{
	U16 count = 0;
		
	if(!pOutBuffer||!pInBuffer)
		return 0;
 
     /*----------------------------------------------------------------*/
     /* Code Body                                                      */
     /*----------------------------------------------------------------*/
     while ((len) && (!((*pInBuffer == 0) && (*(pInBuffer + 1) == 0))))
     {
     
         if(((U8)*pInBuffer == 0xAC) && (*(pInBuffer+1) == 0x20))
         {
             *pOutBuffer = (S8)(0x1F);
         }
         else
         {
             *pOutBuffer = *pInBuffer;
         }
		 
         pInBuffer += 2;
         pOutBuffer++;
         count++;
         len -= 2;
     }
 
     return strlen(pOutBuffer);

}

/*========================================================================================================*/
int dsmDymInitMenuData(DSM_SHEET_T * menu,int size,int8 *title)
{
	if(!menu||size<=0)
		return -1;
	pMenuData = menu;
	menuDataSize = size;
	
	memset(str_title,0,sizeof(str_title));
	memset(root_title,0,sizeof(root_title));
	Dmenu_mmi_ucs2ncpy((char*)root_title,(char*)title,DSM_MAX_MENU_NAME_LEN/2-1);
	return 0;
}

int8 * dsmDymGetCurTitle(void)
{
	return (int8 *)str_title;
}

int dsmDymGetCurHilight(void)
{
	if(MenuEntryType==DSM_MENU_REFRESH)
		Hightlight_index=dsm_index;
	else if(MenuEntryType==DSM_MENU_ENTERY)
		Hightlight_index=0;
	return Hightlight_index;//dsm_index;//
}


static uint8 DsmParseSmsData(MMIDYNAMICMENU_DETAIL_T*Pdata,char*num,char*content)
{
	uint16 len,numlen=0;
	char *tmpASCII;

	tmpASCII=(char*)MMIDMENU_memMalloc(Pdata->dataLen/2+1);
	UCS2ASCII((char*)Pdata->data, tmpASCII);
	

	len=strlen(tmpASCII);
	
	for(numlen=1;numlen<=len;numlen++)
	{
		if(tmpASCII[numlen-1]==':')
		{
		break;
		}
	}

	strncpy(num, tmpASCII, numlen-1);
	*(num+numlen)='\0';
	strncpy(content, tmpASCII+numlen, len-numlen+1);
	MMIDMENU_memFree(tmpASCII);	
	return TRUE;
	
}

static uint8 DynamicMenu_send_msg( int8*pNumber,int8*pContent,uint32 type)	
{

	char sms_content[141] ={0};
	mr_userinfo userInfo={0};
	//mr_Odsm_getuserinfo(&userInfo);
	MMIDMENU_getUserInfo(&userInfo);
	memset(sms_content,0,sizeof(sms_content));
	if(!pContent)
		return FALSE;

#ifdef __KEYWORD_NO_POSTFIX__	
	sprintf(sms_content,"%s",pContent);
#else
	sprintf(sms_content,"%s *0*%s*%s*%s*%s*2",pContent,userInfo.manufactory,userInfo.type,userInfo.IMSI,userInfo.IMEI);
#endif

	if(MMIDMENU_sendSms((char*)pNumber, sms_content, type) == 0)
		return TRUE;
	else
		return FALSE;
}

#if 0
void DSMDescriptionGoBack(void)
{
	GoBackHistory();
}
#endif

uint8 * dsmDymGetCurDescription(void)
{
	return Hilightnode->data->data;
}

uint8 * dsmDesScreenGetTitle(void)
{
	return Hilightnode->data->name;
}

void mmi_dynamic_get_index(int32 nIndex)
{

   dsm_index =  (uint16) nIndex;
}

void sim_select_get_index(int32 nIndex)
{
	sim_index =  (uint16) nIndex;
}

int dsmDymGetMenuItem(int8**items_list)
{
	int16 i;
	
	//dsm_index=0;
	memset(dsm_items_list, 0, sizeof(dsm_items_list));

	Go2TopNode();	//Hightlight_index=
	for(i=0;i<DSM_MAX_List_ITEM;i++)
	{
		Dmenu_mmi_ucs2cpy((int8*)dsm_items_list[i],(int8*)Hilightnode->data->name);
		items_list[i]=dsm_items_list[i];

		if(Hilightnode->next!=NULL)
		{
			Hilightnode=Hilightnode->next;
		}
		else
		{
		break;
		}
	}

	return (i+1);
}

void GoTodefaultWAP(void)
{
	char url[200];
	mr_userinfo info={0};
	mr_screeninfo  screeninfo ={0};

	//mr_Odsm_getuserinfo(&info);
	//mr_Odsm_getscreeninfo(&screeninfo);
	MMIDMENU_getUserInfo(&info);
	MMIDMENU_getScreenInfo();
	memset(url,0,sizeof(url));
	//mmi_ucs2_n_to_asc((S8 *) url, (S8 *) Hilightnode->data->data,100);
	sprintf(url,"http://114.80.75.130:6060/dsm/forward.do?skyp=%s_%s_%dx%d&imsi=%s&imei=%s&type=2",info.manufactory,info.type,screeninfo.width,screeninfo.height,info.IMSI,info.IMEI);
	MMIDMENU_connectWAP((char*)url);
}


void EntryDynamicMenu(void)
{
	uint16 i;
	//int left1 = med_ext_left_size();
	//int left2 =0;
	//strcpy((char*)gPLMN,"46000");
#if 0
	if(mmi_usb_is_in_mass_storage_mode())
	{
		DSMPopupFail();
		return;
	}
	
	if(mr_OdsmDualSim.totalNum >1)
	{
		if(sim_index==0)
			mr_Odsm_setactivesim(DSM_MASTER_SIM);
		else
			mr_Odsm_setactivesim(DSM_SLAVE_SIM);
	}
#endif

	//if(dsmDualSim.curActive==0)
	//{
	Hilightnode=CreateDSMTreeStruct();
	//}
	//else
	//{
	//		Hilightnode=CreateDSMTreeStruct();
	//}

	pRootNode = Hilightnode;
	//DestoryDSMTreeStruct();
	
	//left2 = med_ext_left_size();

	if(pRootNode==NULL)
	{
		#if 1
		DSMPopupNotSupport();
		#else
		/*add pop*/
		if(gsmOnly==1)
		DSMPopupNotSupport();
		else
		GoTodefaultWAP();
		#endif
		return;
	}
	else
	{
		Dmenu_mmi_ucs2ncpy((char*)str_title,(char*)root_title,DSM_MAX_MENU_NAME_LEN/2-1);
		MenuEntryType=DSM_MENU_ENTERY;
		MMIDMENU_showDynMenuWin();
	}
}

void mmi_dynamic_set_index(int32 nIndex)
{
	mmi_dynamic_get_index(nIndex);	
}

 void dsm_left_handle(void)
{
	uint16 i=0;
	char num[21+1]={0};
	char content[DSM_MENU_TXT_LEN+1]={0};

	while(Hilightnode->prev!=NULL)
	{
		Hilightnode=Hilightnode->prev;
	}
	
	for(i=0;i<dsm_index;i++)
	{
		Hilightnode=Hilightnode->next;
	}
	
	switch(Hilightnode->data->type)
	{
	case 'm':
		memset(str_title,0,sizeof(str_title));
		Dmenu_mmi_ucs2cpy((int8*)str_title,(int8*)Hilightnode->data->name);
		Hilightnode=Hilightnode->child;
		MenuEntryType=DSM_MENU_ENTERY;
		MMIDMENU_showDynMenuWin();
		break;
	case 'w':
	{
		if((Hilightnode->data->data)&&dsmWstrlen((int8*)Hilightnode->data->data) >0)
		{
			char url[200];
			mr_userinfo info={0};
			mr_screeninfo  screeninfo ={0};
			//mr_Odsm_getuserinfo(&info);
			//mr_Odsm_getscreeninfo(&screeninfo);
			MMIDMENU_getUserInfo(&info);
			MMIDMENU_getScreenInfo();
			memset(url,0,sizeof(url));
			mmi_ucs2_n_to_asc((int8*) url, (int8 *) Hilightnode->data->data,100);
			
#ifdef __KEYWORD_NO_POSTFIX__
			;
#else
			sprintf(url+strlen(url),"?skyp=%s_%s_%dx%d&imsi=%s&imei=%s&type=2",info.manufactory,info.type,screeninfo.width,screeninfo.height,info.IMSI,info.IMEI);
#endif

			MMIDMENU_connectWAP((char*)url);
		}
		break;
	}
	case 's':
		DsmParseSmsData(Hilightnode->data,num,content);
		DynamicMenu_send_msg((int8*)num,(int8*)content,0);
	 	Go2TopNode();
		break;
	case 't':
		MMIDMENU_showTextWin();
		//Go2TopNode();//pop 被覆盖后在返回，显示的内容不对
		break;
	default:
		break;
	}
}

void dsmGoBackHistory(void)
{
	Hightlight_index=0;
	
	if(Hilightnode->parent==NULL)
	{
		DestoryDSMTreeStruct();
		dsmDymGotoBase();
		return;
	}
	
	Hilightnode=Hilightnode->parent;
	
	while(Hilightnode->prev!=NULL&&Hightlight_index<=DSM_MAX_List_ITEM)
	{
		Hilightnode=Hilightnode->prev;
		Hightlight_index++;
	}
	
	if(Hilightnode->parent==NULL)
	{
	       memset(str_title,0,sizeof(str_title));
		Dmenu_mmi_ucs2cpy((int8*)str_title,(int8*)root_title);
	}
	else
	{
		memset(str_title,0,sizeof(str_title));
		Dmenu_mmi_ucs2cpy((int8*)str_title,(int8*)Hilightnode->parent->data->name);
	}
	
	MenuEntryType=DSM_MENU_BACK;
	MMIDMENU_showDynMenuWin();
}

#if 0
void EntryDsmDynamicMenu(void)
{
	mr_Odsm_initsiminfo();
	if(mr_OdsmDualSim.totalNum==0)
	{
		DSMPopupNoSim();
		return;
	}
	if(mr_OdsmDualSim.totalNum >1)
	{
		entry_sim_select();
	}
	else
	{
		EntryDynamicMenu();
	}
}
#endif

char *dsmGetVersion(void)
{
	return DSM_VERSION_STRING;
}

#ifdef __SUPPORT_BIND_FUNC__
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define MR_KEYINFO_FMT			"%s$%x,%x&%s@%x"
#define MR_APPINFO_LEN			128
#define MR_APPINFO_FILENAME	"appinfo.txt"

#ifdef _WIN32
#define snprintf kal_snprintf
#endif

#define MR_FAILED     (-1)
#define MR_SUCCESS   (0)
#define MR_PLAT_VALUE_BASE  (1000)
//#include "mr_encode.h"

/*
 * "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
 *
 * 返回0xFF表示失败
 */
static unsigned char _mr_decode_table ( unsigned char in )
{
    unsigned char   out = 0xFF;

    if ( in == 'D') //14
    {
        out = 7;
    }
    else if ( in == 'h') //7
    {
        out = 14;
    }
    else if ( in == 'x') //59
    {
        out = 63;
    }
    else if ( in >= 'A' && in <= 'Z' )
    {
        out = in - 'A' + 11;
    }
    else if ( in >= 'a' && in <= 'k' )
    {
        out = in - 'a' ;
    }
    else if ( in >= 'l' && in <= 'z' )
    {
        out = in - 'l' + 47;
    }
    else if ( in >= '0' && in <= '9' )
    {
        out = in - '0' + 37;
    }
    else if ( '+' == in )
    {
        out = 62;
    }
    else if ( '/' == in )
    {
        out = 59;
    }
    else if ( '=' == in )
    {
        out = 64;
    }
    return( out );
}  /* end of base64decodetable */


/*
 * BASE64解码算法的本质是char 转byte
return byte的个数
 * 返回-1表示失败
 */
int32 _mr_decode(uint8 *in, uint32 len, uint8 *out)
{
    unsigned int    x, y, z;
    int    i, j;
    unsigned char   bufa[4];
    unsigned char   bufb[3];

    if (len == 0)
   {
      return 0;
   }
    /*
     * 由主调函数确保形参有效性
     */
    x           = ( len - 4 ) / 4;
    i           =
    j           = 0;
    for ( z = 0; z < x; z++ )
    {
        for ( y = 0; y < 4; y++ )
        {
            if(( bufa[y] = _mr_decode_table( in[j+y]) ) == 0xff)
               return MR_FAILED;
        }  /* end of for */
        out[i]      = bufa[0] << 2 | ( bufa[1] & 0x30 ) >> 4;
        out[i+1]    = ( bufa[1] & 0x0F ) << 4 | ( bufa[2] & 0x3C ) >> 2;
        out[i+2]    = ( bufa[2] & 0x03 ) << 6 | ( bufa[3] & 0x3F );
        i          += 3;
        j          += 4;
    }  /* end of for */
    for ( z = 0; z < 4; z++ )
    {
        if(( bufa[z]     = _mr_decode_table(in[j+z]) ) == 0xff)
           return MR_FAILED;
    }  /* end of for */
    /*
     * 编码算法确保了结尾最多有两个'='
     */
    if ( '=' == in[len-2] )
    {
        y   = 2;
    }
    else if ( '=' == in[len-1] )
    {
        y   = 1;
    }
    else
    {
        y   = 0;
    }
    /*
     * BASE64算法所需填充字节个数是自识别的
     */
    for ( z = 0; z < y; z++ )
    {
        bufa[4-z-1] = 0x00;
    }  /* end of for */
    bufb[0]     = bufa[0] << 2 | ( bufa[1] & 0x30 ) >> 4;
    bufb[1]     = ( bufa[1] & 0x0F ) << 4 | ( bufa[2] & 0x3C ) >> 2;
    bufb[2]     = ( bufa[2] & 0x03 ) << 6 | ( bufa[3] & 0x3F );
    /*
     * y必然小于3
     */
    for ( z = 0; z < 3 - y; z++ )
    {
        out[i+z]    = bufb[z];
    }  /* end of for */
    /*
     * 离开for循环的时候已经z++了
     */
    i          += z;
    return( i );
}  /* end of base64decode */


/*
 *
 * "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"
 *
 * 返回0xFF表示失败
 */
static unsigned char _mr_encode_table ( unsigned char in )
{
    unsigned char   out = 0xFF;

    if ( in == 7) //14
    {
        out = 'D';
    }
    else if ( in == 14) //7
    {
        out = 'h';
    }
    else if ( 59 == in )
    {
        out = '/';
    }
    else if ( in >= 11 && in <= 36  )
    {
        out = in + 'A' - 11;
    }
    else if ( in >= 47 && in <= 61 )
    {
        out = in + 'l' - 47;
    }
    else if ( in <= 10 )
    {
        out = in + 'a' ;
    }
    else if ( in >= 37 && in <= 46 )
    {
        out = in + '0'- 37;
    }
    else if ( 62 == in )
    {
        out = '+';
    }
    else if ( in == 63) //59
    {
        out = 'x';
    }
    return( out );
}  /* end of base64encodetable */


/*
 * BASE64编码算法的本质是byte -> char
return char的个数
 * 返回-1表示失败
 */
int32 _mr_encode(uint8  *in, uint32 len,uint8 *out)
{
    unsigned int    x, y, z;
    int    i, j;
    unsigned char   buf[3];

    x   = len / 3;
    y   = len % 3;
    i   =
    j   = 0;
    for ( z = 0; z < x; z++ )
    {
        out[i]      = _mr_encode_table( (uint8)(in[j] >> 2) );
        out[i+1]    = _mr_encode_table( (uint8)(( in[j] & 0x03 ) << 4 | in[j+1] >> 4) );
        out[i+2]    = _mr_encode_table( (uint8)(( in[j+1] & 0x0F ) << 2 | in[j+2] >> 6) );
        out[i+3]    = _mr_encode_table( (uint8)(in[j+2] & 0x3F) );
        if( (out[i]|out[i+1]|out[i+2]|out[i+3]) == 0xff )
          return MR_FAILED;
        i          += 4;
        j          += 3;
    }  /* end of for */
    if ( 0 != y )
    {
        buf[0]      =
        buf[1]      =
        buf[2]      = 0x00;
        for ( z = 0; z < y; z++ )
        {
            buf[z]  = in[j+z];
        }  /* end of for */
        out[i]      = _mr_encode_table( (uint8)(buf[0] >> 2) );
        out[i+1]    = _mr_encode_table( (uint8)(( buf[0] & 0x03 ) << 4 | buf[1] >> 4) );
        out[i+2]    = _mr_encode_table( (uint8)(( buf[1] & 0x0F ) << 2 | buf[2] >> 6) );
        out[i+3]    = _mr_encode_table( (uint8)(buf[2] & 0x3F) );
        if( (out[i]|out[i+1]|out[i+2]|out[i+3]) == 0xff )
          return MR_FAILED;
        i          += 4;
        /*
         * BASE64算法所需填充字节个数是自识别的
         */
        for ( z = 0; z < 3 - y; z++ )
        {
            out[i-z-1]  = '=';
        }  /* end of for */
    }
    out[i] = 0;
    return( i );
}  /* end of base64encode */



typedef struct {
	const char* appname; //max: 32
	int32 appid;
	int32 version;
	const char* info; //max: 64
	uint32 reserve;
}mr_appinfo_t;


typedef struct {
	void* key;
	int32 len;
	const char* fmt;
}mr_keyinfo_t;


typedef struct
{
   uint16 year;                 //年
   uint8  month;                //月
   uint8  day;                  //日
   uint8  hour;                 //时，24小时制
   uint8  minute;               //分
   uint8  second;               //秒
}mr_datetime;
			
typedef void(*mr_license_validator_f)(void*);

typedef struct{
	mr_license_validator_f validator;
	uint32 context;
	int32 hAppinfo;
	char* buffer;
}mr_license_mgr_ctrl_t;


//extern int32 _mr_encode(uint8  *in, uint32 len,uint8 *out);
//extern const mr_lapp_license_entry_t g_mr_lapp_license_entries[];
//extern const uint32 g_mr_preinstalled_appinfo_table[];
static mr_license_mgr_ctrl_t s_license_mgr_ctrl;


void mr_license_register_validator(mr_license_validator_f validator)
{
	s_license_mgr_ctrl.validator = validator;
}

#if 0
static void mr_license_collect_appinfo_begin(void)
{
	//mr_fs_switch_to_root_dir();

	if(mr_info(MR_APPINFO_FILENAME) == MR_IS_FILE) mr_remove(MR_APPINFO_FILENAME);
	s_license_mgr_ctrl.hAppinfo = mr_open(MR_APPINFO_FILENAME, MR_FILE_WRONLY |MR_FILE_CREATE);
	if(s_license_mgr_ctrl.hAppinfo > 0){
		s_license_mgr_ctrl.buffer = mr_mem_get_ex(MR_APPINFO_LEN * 4);
	}
}



static void mr_license_collect_appinfo(uint32 menuid, mr_appinfo_t* appinfo)
{
	int len;
	char buf[64]={0};
	
	if(s_license_mgr_ctrl.hAppinfo <= 0 || !s_license_mgr_ctrl.buffer || !appinfo || !appinfo->appname)
		return;

	len = snprintf(s_license_mgr_ctrl.buffer, MR_APPINFO_LEN * 4, "%d,%d,%s,%s,", appinfo->appid, appinfo->version, appinfo->appname, appinfo->info? appinfo->info:"");
	if(menuid > 0){ 
		UI_string_type str = get_string(GetStringIdOfItem(menuid));
		if(str){
			//mmi_chset_convert(MMI_CHSET_UCS2, MMI_CHSET_UTF8, (char*)str, s_license_mgr_ctrl.buffer+len, MR_APPINFO_LEN * 4 - len);
			mmi_chset_ucs2_to_utf8_string((kal_uint8*)(s_license_mgr_ctrl.buffer+len), MR_APPINFO_LEN * 4 - len, (kal_uint8*)str);
		}
	}
	mr_write(s_license_mgr_ctrl.hAppinfo, s_license_mgr_ctrl.buffer, strlen((char *)s_license_mgr_ctrl.buffer));
	mr_write(s_license_mgr_ctrl.hAppinfo, "\n", 1);
}


static void mr_license_collect_appinfo_end(void)
{
	if(s_license_mgr_ctrl.hAppinfo){
		mr_close(s_license_mgr_ctrl.hAppinfo);
		s_license_mgr_ctrl.hAppinfo = 0;
	}

	if(s_license_mgr_ctrl.buffer) {
		mr_mem_free_ex((void**)&s_license_mgr_ctrl.buffer);
		s_license_mgr_ctrl.buffer = 0;
	}
}

#endif

int32 mr_license_register_appinfo(void* context, mr_appinfo_t* appinfo, mr_keyinfo_t* keyinfo)
{
	static char keybuf[MR_APPINFO_LEN*3/2];

	if(!appinfo
		|| !keyinfo
		|| (!context && s_license_mgr_ctrl.context != (uint32)context))
	{
		return MR_FAILED;
	}

	{//generate the  keyinfo
		char buf[MR_APPINFO_LEN] = {0};
		int buf_len;
		mr_datetime datetime;
		mr_getDatetime(&datetime);
		buf_len = snprintf(buf, MR_APPINFO_LEN-1, MR_KEYINFO_FMT, appinfo->appname, appinfo->appid, appinfo->version, appinfo->info, datetime.month);
		memset(keybuf, 0, sizeof(keybuf));
		_mr_encode((uint8*)buf, buf_len, (uint8*)keybuf);
	}
	
	keyinfo->fmt = MR_KEYINFO_FMT;
	keyinfo->len = strlen(keybuf);
	keyinfo->key = keybuf;
	//mr_license_collect_appinfo(0, appinfo);
	return MR_SUCCESS;
}

#if 0
static void mr_license_collect_preinstalled_appinfo(int32 preinstalled_appinfo_tbl_size)
{
	int tlv, len;
	char entry_buf[MR_APPINFO_LEN];
	mr_appinfo_t appinfo;
	uint8* mrp_data;

	for(tlv = 0; tlv < preinstalled_appinfo_tbl_size;)
	{
		if(MR_APPINFO_TLV_IS_HEADER(tlv))
		{
			memset(&appinfo, 0, sizeof(appinfo));
			appinfo.appid = MR_APPINFO_TLV_VALUE(tlv, MR_APPINFO_TLV_HEADER_APPID);
			appinfo.reserve = MR_APPINFO_TLV_VALUE(tlv, MR_APPINFO_TLV_HEADER_MENUID);
			appinfo.appname = (char*)MR_APPINFO_TLV_VALUE(tlv, MR_APPINFO_TLV_HEADER_ENTRY);
			if(MR_APPINFO_TLV_TYPE(tlv) == MR_APPINFO_TLV_TYPE_HEADER_CALLBACK){
				appinfo.appname = ((mr_app_entry_f)(uint32)(appinfo.appname))();
			}
		
			len = 0;
			memset(entry_buf, 0, sizeof(entry_buf));
			appinfo.info = entry_buf;
			for(tlv += MR_APPINFO_TLV_LEN(tlv); tlv < preinstalled_appinfo_tbl_size;)
			{	
				if(MR_APPINFO_TLV_TYPE(tlv) != MR_APPINFO_TLV_TYPE_ENTRY){
					break;
				}

				mrp_data = (uint8*)MR_APPINFO_TLV_VALUE(tlv, MR_APPINFO_TLV_ENTRY_DATA);
				if(mrp_data[0]==0x4d && mrp_data[1] == 0x52 && mrp_data[2] ==0x50 && mrp_data[3]==0x47){
					uint32 appid=0, verno=0;
					memcpy(&appid, &mrp_data[0x44], sizeof(uint32));
					memcpy(&verno, &mrp_data[0x50], sizeof(uint32));
					len += snprintf(&entry_buf[len], MR_APPINFO_LEN-len-1, "(%s,%d,%d,%d)", 
						&mrp_data[0x10], appid, verno, MR_APPINFO_TLV_VALUE(tlv, MR_APPINFO_TLV_ENTRY_INDEX)); //shortname, appid, version
				}
				tlv += MR_APPINFO_TLV_LEN(tlv);
			}

			mr_license_collect_appinfo(appinfo.reserve, &appinfo);
			continue;
		}
		
		tlv += MR_APPINFO_TLV_LEN(tlv);
	}
}
#endif

int32 mr_plat(int32 code, int32 param)
{
	if(code == 1211)
	{
		srand(mr_getTime());
		return MR_PLAT_VALUE_BASE+rand()%param;
	}
	else
	{
		return 0;
	}
}

void mr_license_mgr_initialize(int32 preinstalled_appinfo_tbl_size)
{
//#ifndef __MR_LICENSE_MGR_DISABLE__
#if 1
	//int i;
	//mr_license_collect_appinfo_begin();
#if 0	
	//initialize the license manager
	for(i = 0; g_mr_lapp_license_entries[i].mr_lapp_license_register_f; i++)
	{
		s_license_mgr_ctrl.validator = 0;
		g_mr_lapp_license_entries[i].mr_lapp_license_register_f();
		if(!s_license_mgr_ctrl.validator) {
			continue;
		}

		s_license_mgr_ctrl.context = mr_plat(1211, 777755999) | 0x80000001;
		s_license_mgr_ctrl.validator((void*)s_license_mgr_ctrl.context);
		s_license_mgr_ctrl.context = 0;
	}
#else
	animation_engine_init();
	if(s_license_mgr_ctrl.validator)
	{
		s_license_mgr_ctrl.context = mr_plat(1211, 777755999) | 0x80000001;
		s_license_mgr_ctrl.validator((void*)s_license_mgr_ctrl.context);
		s_license_mgr_ctrl.context = 0;
	}
#endif
	//preinstalled application information collect
	//mr_license_collect_preinstalled_appinfo(preinstalled_appinfo_tbl_size);
	//mr_license_collect_appinfo_end();
#endif
}

int FF_Divide( int dividend, int divisor )
{
	int i;
	int remainder1,remainder2,div;
	int temp;
	int result=0;
	int flag;
	if (divisor==0)
		return 0x7fFFFFFF; 
	remainder2=dividend;
	if(remainder2<0)remainder2 =-remainder2;
	div=divisor;
	if(div < 0)div = -div;
	result=(int)(remainder2/div);
	result=(result<<15); //INT fraction
	//FLOAT fraction
	remainder1=(int)(remainder2%div);
	remainder1=(remainder1<<1);
	remainder2=0;
	for (i=0;i<15;i++)
	{
	temp=remainder1;
	remainder1-=div;
	flag=1;
	if(remainder1<0){
	remainder1=temp;
	flag=0;
	}
	remainder1=(remainder1<<1);
	remainder2=(remainder2<<1);
	if (flag)
	remainder2=remainder2+1;
	}
	result+=remainder2;
	if(((dividend>0) && (divisor<0)) || ((dividend<0) && (divisor>0)))
	   result=-result;
	return result;
}

#endif

