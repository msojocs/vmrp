#ifndef __MMI_DSM_KEY_H__
#define __MMI_DSM_KEY_H__
#ifdef __MMI_DSM_NEW_JSKY__

typedef enum{
	MR_PLATEX_CODE_JSKY_SUPPORT = 0,
	MR_SKY_FUNCTION                                            	,// = 6999,
	MR_SKY_JVM_BASE                      				,//= 7000,
	MR_SKY_JVM_INIT                       				 ,//= (MR_SKY_JVM_BASE + 1),
	MR_SKY_JVM_VECTOR_FONT_GET_BITMAP     	 ,//= (MR_SKY_JVM_BASE + 2),
	MR_SKY_JVM_VECTOR_FONT_ENV            		 ,//= (MR_SKY_JVM_BASE + 3),
	MR_SKY_JVM_FILE_TRUNCATE              			 ,//= (MR_SKY_JVM_BASE + 4),
	MR_SKY_JVM_FILE_ATTRIBUTE             			 ,//= (MR_SKY_JVM_BASE + 5),
	MR_SKY_JVM_FILE_FOLDERSIZE            		 ,//= (MR_SKY_JVM_BASE + 6),
	MR_SKY_JVM_FILE_SETATTRIBUTE          		 ,//= (MR_SKY_JVM_BASE + 7),
	
	MR_SKY_JVM_FILE_KEYPRESSED                        	 ,//= MR_SKY_JVM_BASE + 20,
	MR_SKY_JVM_FILE_GETPRESSEDKEYINDEX          ,// = MR_SKY_JVM_BASE + 21,
	MR_SKY_JVM_FILE_GETLOGICKEYMAP                  ,// = MR_SKY_JVM_BASE + 22,
	MR_SKY_JVM_RETURN_IDLE                        		 ,//= MR_SKY_JVM_BASE + 23,
	MR_SKY_JVM_CLIP_PUSH				               ,//= MR_SKY_JVM_BASE + 33,
	MR_SKY_JVM_CLIP_POP                 				 ,//= MR_SKY_JVM_BASE + 34,
	MR_SKY_JVM_CLIP_SET                				 ,//= MR_SKY_JVM_BASE + 35,
	MR_SKY_JVM_CLIP_GET               				 ,//= MR_SKY_JVM_BASE + 36,
	MR_SKY_JVM_GETSTRING_WIDTHHEIGHT            ,//= MR_SKY_JVM_BASE + 37,
	MR_SKY_JVM_CHARNUMINWIDTH                       	 ,//= MR_SKY_JVM_BASE + 38,
	MR_SKY_JVM_DRAWTEXT                             		,// = MR_SKY_JVM_BASE + 39
	MR_SKY_JVM_GET_TOTAL_TIME_MS 			,
	MR_SKY_JVM_SET_OPT,
	MR_SKY_JVM_GET_OPT,
	MR_SKY_JVM_SET_ZOOM,	// __MR_CFG_FEATURE_AUTO_ZOOMIN_ZOOMOUT__
	MR_SKY_JVM_INPUT_CAPABILITY
	
}mr_platEx_code_jsky_enum;

typedef struct VectorCharReq
{
	int unicode;
	int fontSize;
}VectorCharReq;

typedef struct FTruncateReq
{
	int32 hFile;
	int32 size;
}FTruncateReq;
typedef struct SetAttributeReq
{
	char *fileName;
	uint8 attr;
}SetAttributeReq;

typedef struct skyclip
{
	int32 x1;
	int32 y1;
	int32 x2;
	int32 y2;
}skyclip,*pskyclip;

typedef struct _tagTextwidthheight
{
	uint8* pStr;
	int32  nCharCount;
	int32  fontSize;	
}TextWidthHeightReq;

typedef struct _tagTextWidthHeight
{
	int32 width;
	int32 height;
}TextWidthHeightResp;

typedef struct _tagCharNumInWidthReq
{
	uint8* pStr;
	int32 width;
	int32 checklinebreak;
	int32 fontSize;
	int32 n;
}CharNumInWidthReq;

typedef struct _tagDrawTextReq
{
	uint8 *pStr;
	int32 fontSize;
	int32 x;
	int32 y;
	int32 n;
	uint8 r;
	uint8 g;
	uint8 b;
}DrawTextReq;

typedef struct _tagNetSetOpt
{
	int32   soc;
	uint32  option;
	void*   value;
	uint8   valueLen;
}NetOpt;

/* this feature add for __MMI_DSM_NEW_JSKY__, later will effect all */
typedef struct _tagOriAppScreenSize
{
	uint16 width;
	uint16 height;
} OriAppScreenSize;


typedef enum _tagMRSOCKETOPTION
{
	MR_SOCKET_OPTION_DELAY = 0,   
	MR_SOCKET_OPTION_KEEPALIVE,
	MR_SOCKET_OPTION_LINGER,
	MR_SOCKET_OPTION_RCVBUF,
	MR_SOCKET_OPTION_SNDBUF,
	MR_SOCKET_OPTION_NREAD
}MRSocketOption;

#define INPUT_CAP_TOUCH_SCREEN    				0x00000001   
#define INPUT_CAP_KEY_ARROW   					0x00000002   
#define INPUT_CAP_KEY_NUMBER    				0x00000004   
#define INPUT_CAP_KEY_QWERTY    				0x00000008   
#define INPUT_CAP_G_SENSOR		    			0x00000010   
#define INPUT_CAP_MULTI_TOUCH		    		0x00000020   


static char bmpVChar[36*36*2];
static int32 isSkyVectorFontLoad;
extern int32 sky_jvm_init(uint8* input, int32 input_len, uint8** output, int32* output_len);
extern int32 sky_jvm_getVersion(void);
extern int SKY_getVectorBitmap(U16 unicode, U16 size, int *width, 
	int *height,char *pBuf);


int32 dsmKeyScanInit(void);
int32 dsmKeyScanKeyPressed(uint32 keyIndex);
int32 dsmKeyScanGetPressedKeyIndex(void);
const int32* dsmKeyScanGetLogicKeyMap(int32 *count);
#endif

#endif

