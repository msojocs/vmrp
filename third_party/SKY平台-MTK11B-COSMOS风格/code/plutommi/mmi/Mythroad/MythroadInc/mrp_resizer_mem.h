
#ifndef __RESIZER_MEM_H__
#define __RESIZER_MEM_H__


/* error code */
#define NO_ERROR                0
#define RESZ_ERR_INVALID_SIZE   1
#define RESZ_ERR_OUTOFMEMORY	2

/* status code */
#define RESZ_MEM_STATUS_FSTART	0x01
#define RESZ_MEM_STATUS_MEM1O 	0x02


typedef struct
{
    kal_bool            yuv_to_rgb;

    /* YUV src base address */
    kal_uint32          y_src_base;
    kal_uint32          u_src_base;
    kal_uint32          v_src_base;

    /* RGB565 target base address */
    kal_uint32          rgb_tar_base1;
    kal_uint32          rgb_tar_base2;

    /* YUV420 target base address */
    kal_uint32          y_tar_base;
    kal_uint32          u_tar_base;
    kal_uint32          v_tar_base;
    
    kal_uint32          source_width;
    kal_uint32          source_height;
    kal_uint32          target_width;
    kal_uint32          target_height;
} RESZ_MEM_CFG_STRUCT;


typedef struct {
    /* base address of working memory */
    kal_uint32          wmbase; 
    kal_uint32          wmsize;
}RESZ_MEM_DCB_STRUCT;


int mr_media_hwRsz_init();
void mr_media_hwRsz_image(kal_uint8 * pY, kal_uint8 * pU, kal_uint8 * pV, unsigned char* pBGR, int srcwidth, int srcheight,int dstwidth,int dstheight);
void mr_media_hwRsz_free(void);
void mr_media_hwRsz_set_yuvOut(kal_uint8* pY, kal_uint8* pU, kal_uint8* pV);

extern kal_int32 RESZ_Mem_Init(void);
extern kal_int32 RESZ_Mem_Open(kal_uint32 width);
extern void RESZ_Mem_Config(RESZ_MEM_CFG_STRUCT* resizer_cfg);
extern kal_int32 RESZ_Mem_Start(void);
extern kal_int32 RESZ_Mem_Stop(void);
extern kal_int32 RESZ_Mem_Close(void);
extern kal_int32 RESZ_Mem_Status(void);
extern void RESZ_Mem_SetYuvOut(kal_uint8* pY, kal_uint8* pU, kal_uint8* pV);

#endif

