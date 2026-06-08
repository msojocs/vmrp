/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2005
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

/*******************************************************************************
 *
 * Filename:
 * ---------
 * wav_drv_thumb.c
 *
 * Project:
 * --------
 *   MAUI
 *
 * Description:
 * ------------
 *   WAV Decoder Driver
 *
 * Author:
 * -------
 * -------
 *
 *==============================================================================
 *             HISTORY
 * Below this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *------------------------------------------------------------------------------
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *
 * removed!
 * removed!
 * removed!
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by PVCS VM. DO NOT MODIFY!!
 *==============================================================================
 *******************************************************************************/

#ifndef __L1_STANDALONE__ // avoid link error

#include "kal_public_defs.h"
#include "kal_public_api.h"
#include "kal_general_types.h"
//#include "kal_non_specific_general_types.h"
//#include "kal_release.h"
//#include "kal_common_defs.h"
#include "reg_base.h"
#include "kal_trace.h"
//#include "ps_trace.h"
//#include "app_buff_alloc.h"
#include "DraExport.h"
//#include "med_smalloc.h"
//#include "med_utility.h"
#include "fsal.h"
#include "string.h"

#include "l1audio.h"
#include "l1audio_def.h"
#include "media.h"
#include "ast.h"
#include "am.h"
#include "ddload.h"
#include "wav.h"
#include "afe.h"
#include "l1sp_trc.h"

#if defined(MT6253)
#include "custom_EMI_release.h"
#endif

#if defined(WAV_CODEC)

#if defined(MT6235) || defined(MT6235B) || defined(MT6268) || defined(MT6236) || defined(MT6236B) || defined(MT6256)
#define PAGE_NUM 2
#elif defined(MT6253) || defined(MT6251) || defined(MT6253E) || defined(MT6253L)|| defined(MT6252) || defined(MT6252H)
#define PAGE_NUM 1
#else
#error The new chip should define DM page number
#endif

#define WAV_ABORT_STATE 0x40

extern const kal_uint16 samplePerFrame[];
extern const kal_uint8  framDuration[];
extern const kal_uint8 SampleRateTable[];

#ifdef __MMI_DSM_NEW__
#include "mmidsmwav.h"
#endif

#ifdef __HQA_AUDIO__
static kal_uint8 WaveOutputDev  = L1SP_BUFFER_0;
#endif

extern kal_semid  WAVsema;
extern void SPT_PutPcmData( const kal_int16 *pcm, kal_int32 len, kal_bool mono );

#if defined(__DRA_DECODE_SUPPORT__)
static const GUID KsDataFormat_Subtype_DRA = {0x50e4f3e5, 0x090d, 0x4443, {0x9a, 0xa7, 0xc7, 0x7f, 0x07, 0x0a, 0x15, 0xa1}};
extern AST_Status wavASTParseFrame(void *hdlptr, kal_uint32 *CurOffset, kal_uint32 *ParseFrameNum);
#endif

#define WAV_MAX_SEARCH_CHUNK 100

//----------- Functions for EOF Handling -----------
// For wav_drv_thumb.c / pcm.c / pcm_strm_drv.c

/* Count the number of remained samples in DSP buffer */
#if 0
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
/* under construction !*/
#endif
//--------------------------------------------------

static kal_int32 LPCM16BitPutData( MHdl *hdl, kal_int32 uMBufLen )
{
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;
   kal_int32 cnt, uGetNByte;
   kal_uint8 *pMBwrite = &hdl->rbInfo.rb_base[hdl->rbInfo.write];
   kal_uint8 *pInBuf = (kal_uint8 *)&ihdl->buf[0];

   ASSERT( (uMBufLen & 0x01)==0 );
   if(uMBufLen > WAV_MAX_DATA_REQUESTED)
      uMBufLen = WAV_MAX_DATA_REQUESTED;
   uGetNByte = autGetBytes(hdl, ihdl->pstFSAL, uMBufLen, pInBuf);
   if(uGetNByte < 0) return -1;

   if( uGetNByte != uMBufLen )
      uGetNByte &= 0xFFFFFFFE;

   for(cnt=uGetNByte; cnt > 0; cnt-=2){
      kal_uint8 tmp;
      tmp = *pInBuf++;
      *pMBwrite++ = *pInBuf++;
      *pMBwrite++ = tmp;
   }
   return uGetNByte;
}

static kal_int32 wav_DecodeSample( MHdl *hdl, kal_int32 uMBufLen, const uint16* Table )
{
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;
   kal_int32   cnt, uGetNByte;
   kal_uint16  *pMBwrite = (kal_uint16 *)&hdl->rbInfo.rb_base[hdl->rbInfo.write];
   kal_uint16  *ptr = &ihdl->buf[0]; // word-aligned

   ASSERT( uMBufLen > 3 );

   uMBufLen >>= 1;
   if(uMBufLen > WAV_MAX_DATA_REQUESTED)
      uMBufLen = WAV_MAX_DATA_REQUESTED;
   uGetNByte = autGetBytes(hdl, ihdl->pstFSAL, uMBufLen, (kal_uint8 *)&ihdl->buf[0]);
   if(uGetNByte < 0) return -1;

   // to avoid the noise induced by zero padding in last odd byte
   if ( hdl->eof && (uGetNByte>=2) ) {
      kal_uint8 *ptr = (kal_uint8 *) ihdl->buf;
      
      if (ptr[uGetNByte-1] == 0) {
         ptr[uGetNByte-1] = ptr[uGetNByte-2];
      }
   }

   if( uGetNByte != uMBufLen )
      uGetNByte &= 0xFFFFFFFC;

   if( Table ){
      for(cnt=uGetNByte; cnt > 0; cnt-=2){
         *pMBwrite++ = Table[(*ptr) & 0x00FF];
         *pMBwrite++ = Table[(*ptr++)>>8];
      }
   }else if(hdl->mediaType == MEDIA_FORMAT_WAV){
      for(cnt=uGetNByte; cnt > 0; cnt-=2){
         *pMBwrite++ = (*ptr<<8) - 0x8000;
         *pMBwrite++ = ((*ptr++) & 0xFF00) - 0x8000;
      }
   }else{
      for(cnt=uGetNByte; cnt > 0; cnt-=2){
         *pMBwrite++ = (*ptr<<8);
         *pMBwrite++ = ((*ptr++) & 0xFF00);
      }
    }
   return (uGetNByte << 1);
}

static kal_int32 wavPutData8bit( MHdl *hdl, kal_int32 mbBufLen) /* 8 bit linear pcm */
{
   return wav_DecodeSample( hdl, mbBufLen, NULL);
}

static kal_int32 wavAlawPutData( MHdl *hdl, kal_int32 mbBufLen) /* A-law decoder */
{
   return wav_DecodeSample( hdl, mbBufLen, Alaw_Tab );
}

static kal_int32 wavUlawPutData( MHdl *hdl, kal_int32 mbBufLen)  /* U-law decoder */
{
   return wav_DecodeSample( hdl, mbBufLen, Ulaw_Tab );
}

static void wavUpdateDataLen( wavMediaHdl *ihdl, kal_uint32 *uMBufLen)
{
   kal_uint32 wDataLength = ihdl->wav.wDataLength;
   kal_uint32 MBLen = *uMBufLen;
   
   if(ihdl->mh.mediaType != MEDIA_FORMAT_WAV) return;

#if defined(__DRA_DECODE_SUPPORT__)
   if (ihdl->wav.isDRA) return;
#endif

   if(wDataLength >= MBLen){
      ihdl->wav.wDataLength -= MBLen;
   }else{
      *uMBufLen = wDataLength;
      ihdl->wav.wDataLength = 0;
   }
}

static kal_bool wavSeek( wavMediaHdl *ihdl )
{   /* file offset must be aligned the first frame header */
   kal_int64  frameNum, totalSamples;
   kal_uint32 uSkipSampleBytes;

   frameNum = ihdl->mh.start_time  / framDuration[ihdl->wav.sr_index];
   totalSamples = frameNum * samplePerFrame[ihdl->wav.sr_index];

   switch( ihdl->wav.formatTag ) {
      case 0x0001:
         uSkipSampleBytes = totalSamples * ihdl->wav.channel_num * 2;
         if( ihdl->wav.BitsPerSample == 8 )
            ihdl->fileOffset += totalSamples * ihdl->wav.channel_num;
         else if( ihdl->wav.BitsPerSample == 16 )
            ihdl->fileOffset += totalSamples * ihdl->wav.channel_num * 2;
         else
            return false; // never reach here, memory corrupt
         break;
      case 0x0006:
      case 0x0007:
         uSkipSampleBytes = totalSamples * ihdl->wav.channel_num * 2;
         ihdl->fileOffset += totalSamples * ihdl->wav.channel_num;
         break;

      case 0x0011:
      case 0x0002:
         {
            kal_int64 blockNum;
            blockNum = totalSamples / ihdl->wav.samplesPerBlock;
            uSkipSampleBytes = blockNum * ihdl->wav.samplesPerBlock * ihdl->wav.channel_num * 2;
            ihdl->fileOffset += blockNum * ihdl->wav.wBlockAlign;
            ihdl->wav.data_count = 0;
            ihdl->wav.BlockCount = 0;
         }
         break;

#if defined(__DRA_DECODE_SUPPORT__)
      case 0xFFFE:
         if (ihdl->wav.isDRA) 
         {
            kal_uint32 CurFrames, CurOffset, StartFrame;
            StartFrame = (kal_uint64)ihdl->mh.start_time * 1000000 / ihdl->frameDur;
            CurFrames=StartFrame;

            if(ihdl->AudioSeekTbl && (ihdl->AudioSeekTbl->CurrentIndex!=1) )
            {
               CurOffset=AST_Seek((MHdl *)ihdl, ihdl->AudioSeekTbl, &CurFrames);
            }
            else
            {
               CurOffset=ihdl->fileOffset;
               wavASTParseFrame((MHdl *)ihdl, &CurOffset, &CurFrames);
            }
            ihdl->fileOffset = CurOffset;          
            if (CurFrames != StartFrame)
            {
                return false;
            }
         }
         break;
#endif

      default:
         return false;
   }

   wavUpdateDataLen(ihdl, &uSkipSampleBytes);

   FSAL_GetFileSize( ihdl->pstFSAL, &ihdl->file_size);
   if( ihdl->fileOffset >= ihdl->file_size){
      ihdl->fileOffset = ihdl->file_size;
      return false;
   }

   return true;
}

static void wavIncTime( MHdl *hdl, uint32 num_bytes )
{
   wavMediaHdl *ihdl = (wavMediaHdl *) hdl;
   kal_int32  totalConsumedData = (ihdl->reidualDataCnt + num_bytes) >> 1;
   if(!num_bytes) return;
   ihdl->reidualDataCnt = totalConsumedData  % (samplePerFrame[ihdl->wav.sr_index]*ihdl->wav.channel_num);
   ihdl->reidualDataCnt <<= 1;
   ihdl->mh.current_time += ( totalConsumedData / (samplePerFrame[ihdl->wav.sr_index]*ihdl->wav.channel_num)) * framDuration[ihdl->wav.sr_index];
}

static void wavHisr( void *data )
{
   wavMediaHdl *ihdl = (wavMediaHdl *)data;
   kal_int16 rb_w, rb_r_cache, tmp1;

   if( ihdl->end_status == MEDIA_TERMINATED )
      return;

   if( *DSP_PCM_CTRL == 0 ) {  // DSP in idle state
      kal_dev_trace( TRACE_STATE, WAV_L1AUDIO_IDLE_STATE);
   	  if(ihdl->end_status == MEDIA_STOP_TIME_UP){
   	  	  mhdlException( (MHdl*)ihdl, MEDIA_STOP_TIME_UP );
   	  }else{
   	  	  mhdlException( (MHdl*)ihdl, MEDIA_END );
   	  }
   	  ihdl->end_status = MEDIA_TERMINATED;
      return;
   }

   if( ihdl->end_status == MEDIA_END || ihdl->end_status == MEDIA_STOP_TIME_UP)
      return;

   if (ihdl->decode_EOF) {
      if (ihdl->zeroCount > 0) {
         kal_int32 count;
         count = autPutSilenceToDSP(ihdl->mh.dsp_rb_base, ihdl->mh.dsp_rb_size, ihdl->wav.dsp_last_sample);
         //count = wavPutSilence(ihdl->mh.dsp_rb_base, ihdl->mh.dsp_rb_size, ihdl->wav.dsp_last_sample);
         ihdl->zeroCount -= count;
      }
      
      if (ihdl->zeroCount <= 0) 
      {
         ihdl->end_status = MEDIA_END;
         *DSP_PCM_CTRL = 0x10;
         kal_dev_trace( TRACE_STATE, WAV_L1AUDIO_END_STATE);
      }
      return;
   }

   if( *DSP_PCM_CTRL == 2 ) {   // DSP RingBuf initialization
      kal_dev_trace( TRACE_STATE, WAV_L1AUDIO_INIT_STATE);
      ihdl->mh.dsp_rb_base = *DSP_PCM_W;
      ihdl->mh.dsp_rb_size = *DSP_PCM_R;
      ihdl->mh.dsp_rb_end = ihdl->mh.dsp_rb_base + ihdl->mh.dsp_rb_size;
      *DSP_PCM_W = 0;
      *DSP_PCM_R = 0;
      *DSP_PCM_CTRL = 4;
   }

   do {
      rb_r_cache = (int16)*DSP_PCM_R;
   } while( rb_r_cache != (int16)*DSP_PCM_R );

   if ( ihdl->mh.GetDataCount(&ihdl->mh) > 1 ) {
      autWriteDataToDSP((MHdl*)ihdl, (uint16*)DSP_PCM_W, (uint16*)DSP_PCM_R, PAGE_NUM, wavIncTime);
      if (ihdl->mh.rbInfo.read == 0)
         ihdl->wav.dsp_last_sample = *((uint16 *)(ihdl->mh.rbInfo.rb_base + ihdl->mh.rbInfo.rb_size - 2));
      else
         ihdl->wav.dsp_last_sample = *((uint16 *)(ihdl->mh.rbInfo.rb_base + ihdl->mh.rbInfo.read - 2));
   }

   if( ihdl->mh.current_time >= ihdl->mh.stop_time) {
      ihdl->end_status = MEDIA_STOP_TIME_UP;
      *DSP_PCM_CTRL = 0x10;
      kal_dev_trace( TRACE_STATE, WAV_L1AUDIO_TIME_UP_STATE);
      return;
   }

   if( ( *DSP_PCM_CTRL == 8) && (  ( (ihdl->mh.eof) && (ihdl->mh.GetDataCount((MHdl*)ihdl) <= 1)) ||
       (ihdl->mh.current_time >= ihdl->mh.stop_time) || ((ihdl->mh.mediaType == MEDIA_FORMAT_WAV) && 
       (ihdl->wav.wDataLength == 0) && (ihdl->mh.GetDataCount((MHdl*)ihdl) <= 1))  )) {
      ihdl->decode_EOF = true;
      ihdl->zeroCount = autGetDSPDataCnt(ihdl->mh.dsp_rb_size) + 4096; //prevent camera popup noise
      //ihdl->zeroCount = wavGetDSPDataCnt(ihdl->mh.dsp_rb_size);
   }

   // fill silence, Media Buffer underflow
   rb_w = (int16)*DSP_PCM_W;
   rb_r_cache = rb_r_cache - rb_w - 1;
   if ( rb_r_cache < 0 )
      rb_r_cache += (int16)ihdl->mh.dsp_rb_size;
   rb_r_cache &= 0xFFFFFFFE;

   if (rb_r_cache)   // if MCU should fill silence to DSP...
   {
   	  kal_dev_trace( TRACE_WARNING, WAV_L1AUDIO_BUF_UF); // buffer underflow
      tmp1 = (int16)ihdl->mh.dsp_rb_size - rb_w;
      if (rb_r_cache > tmp1)
      {
         putSilence( ihdl->mh.dsp_rb_base, ihdl->mh.dsp_rb_size, ihdl->wav.dsp_last_sample, tmp1 );
         rb_r_cache -= tmp1;
      }
      putSilence( ihdl->mh.dsp_rb_base, ihdl->mh.dsp_rb_size, ihdl->wav.dsp_last_sample, rb_r_cache );

#if APM_SUPPORT
      if (!ihdl->mh.eof)
         APM_NotifyDropFrame(true);
#endif

      if(ihdl->mh.isUnderflow == KAL_FALSE && !ihdl->mh.eof)
      {
   	     ihdl->mh.isUnderflow = KAL_TRUE;
   	     mhdlException( (MHdl*)ihdl, MEDIA_BUFFER_UNDERFLOW );
      }
   }

#if defined(__DRA_DECODE_SUPPORT__)
   if ((ihdl->isAudioTask) && (ihdl->wav.isDRA) && (ihdl->mh.GetFreeSpace((MHdl *)ihdl) >= DRA_TEMP_OUTPUT_BUFFER_SIZE)) {
      L1Audio_SetEvent( ihdl->aud_id_decoder, (MHdl *)ihdl );
   }
#endif

   mhdlDataRequest((MHdl*)ihdl, ihdl->end_status);
}

static void wavOpenDevice( wavMediaHdl *ihdl, kal_bool DDFlag)
{
   MHdl *hdl = (MHdl *)ihdl;
   mhdlHookInit( hdl, DP_D2C_PCM_P, wavHisr );
   ihdl->end_status = MEDIA_NONE; // Clear for streaming case
   ihdl->mh.dsp_rb_base = 0;

   mhdlDisallowSleep( (MHdl*)ihdl );

   KT_StopAndWait();
   TONE_StopAndWait();
   AM_Force_VBI_END_IN_TASK();
   
   ASSERT(*DSP_PCM_CTRL == 0);

   if (ihdl->wav.is_8KVoice) {
#if defined(__HQA_AUDIO__)
      Media_SetOutputDevice(WaveOutputDev);
#endif
      AM_PCM8K_PlaybackOn();
   } else {
#if defined( DSP_WT_SYN )
      if(DDFlag)
         DSP_DynamicDownload( DDID_WAVETABLE );
#endif

#if defined(__HQA_AUDIO__)
      Media_SetOutputDevice(L1SP_BUFFER_ST);
#endif

      AM_AudioPlaybackOn( ASP_TYPE_PCM_HI, (uint16)SampleRateTable[ihdl->wav.sr_index] );
   }

   *DSP_PCM_CTRL = 1;
   kal_trace( TRACE_STATE, L1AUDIO_INIT_STATE );
   {
      kal_uint16 I;
      for( I = 0; ; I++ ) {
         if( *DSP_PCM_CTRL == 8 || ihdl->end_status != MEDIA_NONE ){
            kal_trace( TRACE_STATE, L1AUDIO_RUNNING_STATE );
            break;
         }
         ASSERT_REBOOT( I < 40 );
         kal_sleep_task( 2 );
      }
   }
   kal_trace( TRACE_INFO, WAV_L1AUDIO_FREQ_CHANNEL, ihdl->wav.sampleRate, ihdl->wav.channel_num);
}

static void pcmOpenDevice( wavMediaHdl *ihdl)
{
   uint16 I;
   MHdl *hdl = (MHdl *)ihdl;
   mhdlHookInit( hdl, DP_D2C_PCM_P, wavHisr );
   ihdl->end_status = MEDIA_NONE;
   ihdl->mh.dsp_rb_base = 0;
   mhdlDisallowSleep( (MHdl*)ihdl );

   KT_StopAndWait();
   TONE_StopAndWait();
   AM_Force_VBI_END_IN_TASK();
   
   ASSERT(*DSP_PCM_CTRL == 0);

   *DSP_PCM_MS |= 0x0001;
   if( ihdl->wav.is_8KVoice ) {
      AM_PCM8K_PlaybackOn();
   } else {
#if defined( DSP_WT_SYN )
      DSP_DynamicDownload( DDID_WAVETABLE );
#endif
      if( ihdl->mh.mediaType == MEDIA_FORMAT_PCM_16K )
         AM_AudioPlaybackOn( ASP_TYPE_PCM_HI, ASP_FS_16K );
      else
         AM_AudioPlaybackOn( ASP_TYPE_PCM_HI, ASP_FS_8K );
   }

   *DSP_PCM_CTRL = 1;
   for( I = 0; ; I++ ) {
      if( *DSP_PCM_CTRL == 8 || ihdl->end_status != MEDIA_NONE ){
         kal_trace( TRACE_STATE, L1AUDIO_RUNNING_STATE );
         break;
      }
      ASSERT_REBOOT( I < 40 );
      kal_sleep_task( 2 );
   }
}

#if defined(__DRA_DECODE_SUPPORT__)
static void wavReleaseDRAbuf(wavMediaHdl *ihdl)
{
   if (ihdl->wav.isDRA){
      if (ihdl->wav.dra.pDecoder) {
         audio_free_mem( (void **) &ihdl->wav.dra.pDecoder );
      }

      if (ihdl->wav.dra.pTempOutputBuf) {
         audio_free_mem( (void **) &ihdl->wav.dra.pTempOutputBuf );
      }
      
      if (ihdl->wav.bsBase) {
         audio_free_mem( (void **) &ihdl->wav.bsBase );
      }
   }
}
#endif

static void wavCloseDevice( wavMediaHdl *ihdl )
{
   uint16 I, ctl;
   MHdl *hdl = (MHdl *)ihdl;
   for( I = 0; ; I++ ) {
      ctl = *DSP_PCM_CTRL;
      if( ctl == 0 )                /* DSP returns to idle state */
         break;
      if( ctl == 8 ){
         *DSP_PCM_CTRL = WAV_ABORT_STATE;       /* give ABORT command to the DSP */
         kal_trace( TRACE_STATE, L1AUDIO_STOP_STATE );
      }
      ASSERT_REBOOT( I < 40 );
      kal_sleep_task( 2 );
   }

   if (ihdl->wav.is_8KVoice)
      AM_PCM8K_PlaybackOff( true );
   else
      AM_AudioPlaybackOff( true );

#if defined(__DRA_DECODE_SUPPORT__)
   wavReleaseDRAbuf(ihdl);
#endif

   mhdlAllowSleep( (MHdl*)ihdl );
   if( hdl->handler != NULL )
      mhdlHookFinalize( DP_D2C_PCM_P );
}

static Media_Status wavMFClose( MHdl *hdl )
{
   wavMediaHdl *ihdl = (wavMediaHdl *) hdl;
   ASSERT( hdl != 0 && hdl->state != WAV_STATE_PLAYING);
   hdl->FreeBufferInternal(hdl);   
   if( hdl->handler != NULL )
      mhdlFinalize( hdl, DP_D2C_PCM_P );

   L1Audio_FreeAudioID( ihdl->aud_id_decoder );

#if defined(__DRA_DECODE_SUPPORT__)
   wavReleaseDRAbuf(ihdl);
#endif

#if APM_SUPPORT
   if (ihdl->fUseTCMFromAPM)
   {
      APM_Release_Buffer();
   }
   else
#endif
   {
      free_ctrl_buffer( hdl );
   }

   kal_trace( TRACE_FUNC, L1AUDIO_CLOSE, hdl->mediaType );
   return MEDIA_SUCCESS;
}

static bool wavGetByte(wavMediaHdl *ihdl, kal_uint8 *data)
{
   *data = ihdl->mh.rbInfo.rb_base[ihdl->mh.rbInfo.read];
   ihdl->mh.rbInfo.read++;
   if( ihdl->mh.rbInfo.read > ihdl->mh.rbInfo.write)
      return false;
   return true;
}

static bool wavGetWord(wavMediaHdl *ihdl, kal_uint16 *data)
{
   kal_uint8 buf_8;

   if(!wavGetByte(ihdl, &buf_8))
      return false;
   *data = (kal_uint16)buf_8;

   if(!wavGetByte(ihdl, &buf_8))
      return false;

   *data |= (kal_uint16)buf_8 << 8;
   return true;
}

static bool wavGetDword(wavMediaHdl *ihdl, kal_uint32 *data)
{
   kal_uint16 buf_16;
   if(!wavGetWord(ihdl, &buf_16))
      return false;
   *data = (kal_uint32)buf_16;

   if(!wavGetWord(ihdl, &buf_16))
      return false;

   *data |= (kal_uint32)buf_16 << 16;
   return true;
}

#if defined(__DRA_DECODE_SUPPORT__)
static bool wavGetGUID(wavMediaHdl *ihdl, GUID *g)
{
   wavGetDword( ihdl, &g->Data1);
   wavGetWord( ihdl, &g->Data2);
   wavGetWord( ihdl, &g->Data3);
   wavGetByte( ihdl, &g->Data4[0]);
   wavGetByte( ihdl, &g->Data4[1]);
   wavGetByte( ihdl, &g->Data4[2]);
   wavGetByte( ihdl, &g->Data4[3]);
   wavGetByte( ihdl, &g->Data4[4]);
   wavGetByte( ihdl, &g->Data4[5]);
   wavGetByte( ihdl, &g->Data4[6]);
   wavGetByte( ihdl, &g->Data4[7]);
   
   if( ihdl->mh.rbInfo.read >= ihdl->mh.rbInfo.write)
      return false;
   return true;
}
#endif

static bool wavSkipByte(wavMediaHdl *ihdl)
{
   ihdl->mh.rbInfo.read++;
   if( ihdl->mh.rbInfo.read >= ihdl->mh.rbInfo.write)
      return false;
   return true;
}

static bool wavSkipNBytes(wavMediaHdl *ihdl, kal_uint32 NBytes)
{
   kal_uint32 cnt;

   for(cnt=NBytes; cnt > 0; cnt--){
      if(!wavSkipByte(ihdl))
         return false;
   }
   return true;
}

/* search "data" chunk descriptor */
static bool wavReachDataChunk( wavMediaHdl *ihdl )
{
   uint32 chunk_num = 0;
   uint32 data_32, uRead = ihdl->wav.dataChunkOffset;
   do {
      kal_int32 num_bytes;
      num_bytes = autGetBytesFromFile((MHdl *)ihdl, ihdl->pstFSAL, uRead, 8, ihdl->mh.rbInfo.rb_base);
      
      if (num_bytes < 8)
         return false;

      // Get chunk id
      ihdl->mh.rbInfo.read = 0;
      ihdl->mh.rbInfo.write = 8;
      wavGetDword(ihdl, &data_32);
      uRead += 4;
      if( data_32 == 0x61746164 )   break;  // "data"
      if( chunk_num ++ > WAV_MAX_SEARCH_CHUNK )   return MEDIA_BAD_FORMAT;
      	      
      wavGetDword(ihdl, &ihdl->wav.nextChunkOffset);
      uRead += 4;
      uRead += ihdl->wav.nextChunkOffset;
   }while( 1 );

   wavGetDword(ihdl, &ihdl->wav.wDataChunkLength);
   
   uRead += 4;
   ihdl->wav.dataChunkOffset = uRead;
   ihdl->fileOffset = ihdl->wav.dataChunkOffset;

   // Compared with file size to update the data chunk size
/*
   {
      kal_int32 avaliableLength;
      
      avaliableLength = ihdl->file_size - ihdl->wav.dataChunkOffset;
      if (ihdl->wav.wDataChunkLength > avaliableLength) {
         ihdl->wav.wDataChunkLength = avaliableLength;
      }
   }
*/
   if(4 != ihdl->wav.BitsPerSample){
      ihdl->wav.wDataLength = ihdl->wav.wDataChunkLength * (16/ihdl->wav.BitsPerSample);
   }else{
     kal_int64 len, samplePerBlock;
     len = (kal_int64)ihdl->wav.wDataChunkLength;
     samplePerBlock = (kal_int64)ihdl->wav.samplesPerBlock;
     ihdl->wav.wDataLength = len * samplePerBlock * 2 * ihdl->wav.channel_num / ihdl->wav.wBlockAlign;
   }

   return true;
}

static kal_bool getSampleRateIdx( wavMediaHdl *ihdl, kal_uint32 SampleRate )
{
   switch( SampleRate / 1000 ){
      case 7: case 8:   ihdl->wav.sr_index = 0;
         break;
      case 10: case 11: ihdl->wav.sr_index = 1;
         break;
      case 12:          ihdl->wav.sr_index = 2;
         break;
      case 15: case 16: ihdl->wav.sr_index = 3;
         break;
      case 21: case 22: ihdl->wav.sr_index = 4;
         break;
      case 23: case 24: ihdl->wav.sr_index = 5;
         break;
      case 31: case 32: ihdl->wav.sr_index = 6;
         break;
      case 43: case 44: ihdl->wav.sr_index = 7;
         break;
      case 47: case 48: ihdl->wav.sr_index = 8;
         break;
      default:
         return false;   // The sample rate is not supported by ASP.
   }
   return true;
}

static Media_Status wavReadHeader( wavMediaHdl *ihdl )
{
   uint32 dummy_32, ckSize, bytes_read, uRead, chunk_num;
   uint16 FormatTag;
   
/* Check to see if header is invalid, "RIFF" chunk descriptor */
   wavGetDword(ihdl, &dummy_32);
   if( dummy_32 != 0x46464952 )
      return MEDIA_BAD_FORMAT;
   wavSkipNBytes(ihdl, 4); // Skip 2 word
   wavGetDword(ihdl, &dummy_32);
   if( dummy_32 != 0x45564157 )
      return MEDIA_BAD_FORMAT;

   uRead = 12;
   chunk_num = 0;
   do {
      kal_int32 num_bytes;
      num_bytes = autGetBytesFromFile((MHdl *)ihdl, ihdl->pstFSAL, uRead, 8, ihdl->mh.rbInfo.rb_base);
      
      if (num_bytes < 8)
         return MEDIA_BAD_FORMAT;

      // Get chunk id
      ihdl->mh.rbInfo.read = 0;
      ihdl->mh.rbInfo.write = 8;
      wavGetDword(ihdl, &dummy_32);
      uRead += 4;
      if( dummy_32 == 0x20746D66 )// seek sub-chunk "fmt chunk"
      {
      	 kal_uint32 buf_len;
      	 uint8 *pBuf;
         ihdl->mh.rbInfo.read = 0;
         ihdl->mh.rbInfo.write = 0;
         ihdl->mh.GetWriteBuffer((MHdl *)ihdl, &pBuf, &buf_len);
         num_bytes = autGetBytesFromFile((MHdl *)ihdl, ihdl->pstFSAL, uRead, buf_len, pBuf);
         if(num_bytes<16)   return MEDIA_BAD_FORMAT;
         ihdl->mh.WriteDataDone( (MHdl *)ihdl, (uint32)num_bytes );
         break;  
      }
      if( chunk_num ++ > WAV_MAX_SEARCH_CHUNK )
         return MEDIA_BAD_FORMAT;
      wavGetDword(ihdl, &ihdl->wav.nextChunkOffset);
      uRead += 4;
      uRead += ihdl->wav.nextChunkOffset;
   }while( 1 );
   
   // Get the fmt ckSize
   wavGetDword(ihdl, &ckSize);
   if( ckSize < 16 )
      return MEDIA_BAD_FORMAT;
   uRead += 4;
   /* Check fmt chunk */
   wavGetWord(ihdl, &FormatTag);
   ihdl->wav.formatTag = FormatTag;
   wavGetWord(ihdl, &ihdl->wav.channel_num);
   if( ihdl->wav.channel_num == 1 ) {
      *DSP_PCM_MS |= 0x0001;
      ihdl->mh.mono2stereo = true;
      ihdl->wav.is_stereo = false;
   } else if( ihdl->wav.channel_num == 2 ) {
      *DSP_PCM_MS |= 0x0001;
      ihdl->wav.is_stereo = true;
   } else {
      return MEDIA_UNSUPPORTED_CHANNEL;
   }

   // sample rate
   wavGetDword(ihdl, (kal_uint32 *)&ihdl->wav.sampleRate);

   if( false == getSampleRateIdx( ihdl, ihdl->wav.sampleRate ) )
      return MEDIA_UNSUPPORTED_FREQ; // The sample rate is not supported by ASP.

   wavGetDword(ihdl, (kal_uint32 *)&ihdl->wav.AvgBytesPerSec);
   wavGetWord(ihdl, (kal_uint16 *)&ihdl->wav.wBlockAlign);
   wavGetWord(ihdl, (kal_uint16 *)&ihdl->wav.BitsPerSample); // 36

   if ( (ihdl->wav.wBlockAlign == 0) || (ihdl->wav.BitsPerSample == 0) ) {
      return MEDIA_UNSUPPORTED_TYPE;
   }

   bytes_read = 16;
   switch( FormatTag ) {
      case 0x0001:
         if( ihdl->wav.BitsPerSample == 8 )
            ihdl->wav.Wav_PutData = wavPutData8bit;
         else if( ihdl->wav.BitsPerSample != 16 )
            return MEDIA_UNSUPPORTED_TYPE;
         break;
      case 0x0006:
      case 0x0007:
         ihdl->wav.Wav_PutData = ( FormatTag == 0x0006 )?
            wavAlawPutData : wavUlawPutData;
         break;
      case 0x0011:
         if( ihdl->wav.BitsPerSample != 4 )   // support 4 bit dvi-adpcm only
            return MEDIA_UNSUPPORTED_TYPE;
         wavSkipNBytes(ihdl, 2);
         wavGetWord(ihdl, &ihdl->wav.samplesPerBlock);
         ihdl->wav.Wav_PutData = ( ihdl->wav.is_stereo )?
         wavDviADPCMPutData_ST : wavDviADPCMPutDataMono;
         bytes_read += 4;
         break;
      case 0x0002:                           // MS ADPCM
         if( ihdl->wav.BitsPerSample != 4 )  // support 4 bit ms-adpcm only
            return MEDIA_UNSUPPORTED_TYPE;
         ihdl->wav.Wav_PutData = wavMSADPCMPutDataMonoST;
         if( ihdl->wav.is_stereo )              // test if stereo
            wavMS_ADPCM_Decode = wavMSADPCM_decode_stereo;
         else
            wavMS_ADPCM_Decode = wavMSADPCM_decode_mono;

         wavSkipNBytes(ihdl, 2);
         wavGetWord(ihdl, &ihdl->wav.samplesPerBlock);
         wavGetWord(ihdl, (kal_uint16 *)&ihdl->wav.nNumCoef);
         if(( ihdl->wav.nNumCoef > MSADPCM_EXTRA_COEFF + 7 )||(ihdl->wav.nNumCoef < 7))
            return MEDIA_UNSUPPORTED_TYPE;

        wavSkipNBytes(ihdl, 28); // Skip 14 word

         for( dummy_32=0; dummy_32 < ihdl->wav.nNumCoef - 7; dummy_32++ )
         {
            // These values are stored in 8.8 signed format !!!
            wavGetWord(ihdl, (kal_uint16 *)&ihdl->wav.aCoeff[dummy_32][0]);
            wavGetWord(ihdl, (kal_uint16 *)&ihdl->wav.aCoeff[dummy_32][1]);
         }
         bytes_read += 6 + ihdl->wav.nNumCoef * 4;
         break;
         
#if defined(__DRA_DECODE_SUPPORT__)
      case 0xFFFE:
         // 0xFFFE is WAVE_FORMAT_EXTENSIBLE
         
         {
            kal_uint16 cbSize;
            GUID guid;
            
            // DRA case, cbSize must be 22
            wavGetWord(ihdl, (kal_uint16 *)&cbSize);
            bytes_read += 2;
            if (cbSize != 22)
               return MEDIA_UNSUPPORTED_TYPE;
            
            // Skip Word (samples) and DWord (channel mask)
            wavSkipNBytes(ihdl, 6);
            bytes_read += 6;
            
            // Check GUID
            if ( !wavGetGUID(ihdl , &guid) )
               return MEDIA_UNSUPPORTED_TYPE;
            if ( kal_mem_cmp((void *)&KsDataFormat_Subtype_DRA, (void *)&guid, sizeof(GUID)) )
               return MEDIA_UNSUPPORTED_TYPE;
         
            bytes_read += sizeof(GUID);
            if (ihdl->wav.wBlockAlign == 4) {
               ihdl->wav.dra.isVBR = true;
            }
            ihdl->wav.isDRA = true;
            ihdl->wav.Wav_PutData = wavDraPutData;
            break;
         }
#endif

      default:
         return MEDIA_UNSUPPORTED_TYPE;
   }

   {
      kal_uint64 framLen = 0;
      switch (ihdl->wav.sr_index) {
         case 0: case 1: case 2:
            framLen = 72;
            break;
         case 3: case 4: case 5:
            framLen = 144;
            break;
         case 6: case 7: case 8:
            framLen = 72;
            break;
      }
#if defined(__DRA_DECODE_SUPPORT__)
      if (ihdl->wav.isDRA) {
         framLen = 1024;
      }
#endif

      ihdl->frameDur = framLen * 1000000000 / ihdl->wav.sampleRate;
   }

   //wavSkipNBytes(ihdl, ckSize - bytes_read);
   ihdl->wav.nextChunkOffset = 0;
   ihdl->wav.dataChunkOffset = uRead + ckSize;

   return MEDIA_SUCCESS;
}

// Look for SSND chunk
// Bedofe calling this function, aiff.mb_count should be assigned correct value.
// Return value:
//    0: ssnd not found yet
//    1: ssnd found but the actual sound data is not found yet
//    2: sound data is found
static kal_uint32 aiffLookForSSND( wavMediaHdl *ihdl )
{
   MHdl *hdl = (MHdl *)ihdl;
   kal_uint32 data_32;

   while( 1 )
   {
      if( ihdl->wav.word_left_in_ck )
      {
         data_32 = (kal_uint32) hdl->GetDataCount(hdl);
         if(data_32 >= (ihdl->wav.word_left_in_ck << 1)){
            wavSkipNBytes(ihdl, (ihdl->wav.word_left_in_ck << 1));
            ihdl->wav.word_left_in_ck = 0;
            ihdl->wav.word_left_in_form -= ihdl->wav.word_left_in_ck;
         }
         if( ihdl->wav.word_left_in_ck )
            return 0;
      }
      if( hdl->GetDataCount(hdl) < 16 )
         return 0;
      wavGetDword(ihdl, &data_32 );   // Get chunk id
      ihdl->wav.word_left_in_form -= 2;
      if( data_32 == 0x444e5353 )
         break;
      wavGetDword(ihdl, &data_32 );    // Get chunk size
      ihdl->wav.word_left_in_form -= 2;
      _bytes_reverse_32( data_32, ihdl->wav.word_left_in_ck )
      ihdl->wav.word_left_in_ck = ( ihdl->wav.word_left_in_ck + 1 ) >> 1;
   }
   // the ssnd is found
   wavGetDword(ihdl, &data_32 ); // chunk size
   _bytes_reverse_32( data_32, ihdl->wav.word_left_in_ck );
   ihdl->wav.word_left_in_ck = ( ihdl->wav.word_left_in_ck + 1 ) >> 1;
   wavGetDword(ihdl, &data_32 ); // SSND offset
   _bytes_reverse_32( data_32, ihdl->wav.ssnd_offset );
   wavSkipNBytes(ihdl, 4);  // blockSize
   ihdl->wav.word_left_in_ck -= 4;
   ihdl->wav.word_left_in_form -= 6;
   if( ihdl->wav.ssnd_offset == 0 )
   {
      ihdl->fileOffset += ihdl->mh.rbInfo.read;
      return 2;
   }
   else if( ihdl->wav.ssnd_offset & 0x01 )
      return 0;   // We don't support odd offset.
   else
   {
      ihdl->fileOffset += (ihdl->mh.rbInfo.read + ihdl->wav.ssnd_offset);
      ihdl->wav.ssnd_offset = 0;
      return 2;
   }

}

static bool aiffReadHeader( wavMediaHdl *ihdl )
{
   kal_uint16 data_16_a, data_16_b;
   kal_uint32 data_32;

   // file magic, 0x4d524f46 for ASCII "MROF"
   if( wavGetDword( ihdl, &data_32 ) == false || data_32 != 0x4d524f46 )
      return false;

   // chunk size
   if( wavGetDword( ihdl, &data_32 ) == false )
      return false;
   _bytes_reverse_32( data_32, ihdl->wav.word_left_in_form );
   ihdl->wav.word_left_in_form = ( ihdl->wav.word_left_in_form + 1 ) >> 1;

   // aiff type
   if( wavGetDword( ihdl, &data_32 ) == false )
      return false;

   if( data_32 == 0x46464941 )   // 0x46464941 for ASCII "FFIA"
      ihdl->wav.aiff_type = IS_AIFF;
   else if( data_32 == 0x43464941 ) // 0x46464941 for ASCII "CFIA"
      ihdl->wav.aiff_type = IS_AIFF_C;
   else
      return false;
   ihdl->wav.word_left_in_form -= 2;

   // Look for the COMM chunk
   while( 1 )
   {
      if( wavGetDword( ihdl, &data_32 ) == false || data_32 == 0x444e5353 )
         return false;  // Counld not find the COMM chunk or the SSND chunk is before COMM chunk, error!
      ihdl->wav.word_left_in_form -= 2;
      if( data_32 == 0x4d4d4f43 )
         break;

      if( wavGetDword( ihdl, &data_32 ) == false )
         return false;
      _bytes_reverse_32( data_32, ihdl->wav.word_left_in_ck );
      if( wavSkipNBytes( ihdl, ihdl->wav.word_left_in_ck ) == false )
         return false;
      ihdl->wav.word_left_in_ck = ( ihdl->wav.word_left_in_ck + 1 ) >> 1;
      ihdl->wav.word_left_in_form -= (ihdl->wav.word_left_in_ck + 2);
   }
   if( wavGetDword( ihdl, &data_32 ) == false )
      return false;
   ihdl->wav.word_left_in_form -= 2;
   _bytes_reverse_32( data_32, ihdl->wav.word_left_in_ck );
   if( ihdl->wav.aiff_type == IS_AIFF && ihdl->wav.word_left_in_ck != 18 )
      return false;   // The file is AIFF and ckSize of COMM chunk is not equal to 18, error!
   ihdl->wav.word_left_in_ck = ( ihdl->wav.word_left_in_ck + 1 ) >> 1;

   // Check channel count
   if( wavGetWord( ihdl, &data_16_a) == false )
      return false;
   if( data_16_a == 0x0100 ) {
      *DSP_PCM_MS |= 0x0001;
      ihdl->wav.channel_num = 1;
      ihdl->wav.is_stereo = false;
      ihdl->mh.mono2stereo = true;
   } else if( data_16_a == 0x0200 ) {
      *DSP_PCM_MS |= 0x0001;
      ihdl->wav.channel_num = 2;
      ihdl->wav.is_stereo = true;
      ihdl->mh.mono2stereo = false;
   } else
      return false;

   // numSampleFrames
   if( wavSkipNBytes(ihdl, 4) == false )
      return false;

   if( wavGetWord( ihdl, &data_16_a) == false )
      return false;
   _bytes_reverse_16( data_16_a, ihdl->wav.data_size );
   ihdl->wav.data_size >>= 3;
   if( ihdl->wav.data_size != 1 && ihdl->wav.data_size != 2 )
      return false;   // Support only 8/16 bit data

   if( wavGetWord( ihdl, &data_16_a ) == false )
      return false;
   if( wavGetWord( ihdl, &data_16_b ) == false )
      return false;
   _bytes_reverse_16( data_16_a, data_16_a );
   _bytes_reverse_16( data_16_b, data_16_b );
   data_16_a -= 16383;
   data_16_b >>= ( 15 - data_16_a );
   ihdl->wav.sampleRate = data_16_b;

   if( false == getSampleRateIdx( ihdl, (kal_uint32)data_16_b ) )
      return 0; // The sample rate is not supported by ASP.

   if( wavSkipNBytes(ihdl, 6) == false )
      return false;
   ihdl->wav.word_left_in_form -= 9;
   ihdl->wav.word_left_in_ck -= 9;
   if( ihdl->wav.aiff_type == IS_AIFF)
   {
      if( ihdl->wav.data_size == 1 ){
         ihdl->wav.Wav_PutData = wavPutData8bit;
         ihdl->wav.BitsPerSample = 8;
      }else{
         ihdl->wav.Wav_PutData = LPCM16BitPutData;
         ihdl->wav.BitsPerSample = 16;
       }
      ihdl->wav.formatTag = 0x0001;
   }
   else
   {
      if( wavGetDword( ihdl, &data_32 ) == false )
         return false;
      _bytes_reverse_32( data_32, data_32 );
      switch( data_32 )
      {
         case 0x4e4f4e45:  // "NONE"
            ihdl->wav.formatTag = 0x0001;
            if( ihdl->wav.data_size == 1 ){
               ihdl->wav.Wav_PutData = wavPutData8bit;
               ihdl->wav.BitsPerSample = 8;
            }else{
               ihdl->wav.Wav_PutData = LPCM16BitPutData;
               ihdl->wav.BitsPerSample = 16;
            }
            break;

         case 0x756c6177:  // "ulaw"
         case 0x554c4157:  // "ULAW"
            ihdl->wav.formatTag = 0x0007;
            ihdl->wav.BitsPerSample = 8;
            ihdl->wav.Wav_PutData = wavUlawPutData;
            ihdl->wav.data_size = 1;
            break;
         case 0x616c6177:  // "alaw"
         case 0x414c4157:  // "ALAW"
            ihdl->wav.formatTag = 0x0006;
            ihdl->wav.BitsPerSample = 8;
            ihdl->wav.Wav_PutData = wavAlawPutData;
            ihdl->wav.data_size = 1;
            break;
         default:
            return false;
      }
      ihdl->wav.word_left_in_form -= 2;
      ihdl->wav.word_left_in_ck -= 2;
   }
   ihdl->wav.ssnd_state = 0;
   return true;
}

static bool auReadHeader( wavMediaHdl *ihdl )
{
   kal_uint32 data_32;

   // file magic, 0x646e732e for ASCII "dns."
   if( wavGetDword( ihdl, &data_32 ) == false || data_32 != 0x646e732e )
      return false;

   // data offset
   if( wavGetDword( ihdl, &data_32 ) == false )
      return false;
   _bytes_reverse_32( data_32, ihdl->wav.data_offset );

   // data length
   if( wavGetDword( ihdl, &data_32 ) == false )
      return false;
   _bytes_reverse_32( data_32, ihdl->wav.data_length );

   // format id
   if( wavGetDword( ihdl, &data_32 ) == false )
      return false;

   switch( data_32 )
   {
      case 0x01000000:
         ihdl->wav.formatTag = 0x0007;
         ihdl->wav.BitsPerSample = 8;
         ihdl->wav.Wav_PutData = wavUlawPutData;
         break;
      case 0x1b000000:
         ihdl->wav.formatTag = 0x0006;
         ihdl->wav.BitsPerSample = 8;
         ihdl->wav.Wav_PutData = wavAlawPutData;
         break;
      case 0x02000000:
         ihdl->wav.formatTag = 0x0001;
         ihdl->wav.BitsPerSample = 8;
         ihdl->wav.Wav_PutData = wavPutData8bit;
         break;
      case 0x03000000:
         ihdl->wav.formatTag = 0x0001;
         ihdl->wav.BitsPerSample = 16;
         ihdl->wav.Wav_PutData = LPCM16BitPutData;
         break;
      default:
         return false;
   }

   // sample rate
   if( wavGetDword( ihdl, &data_32 ) == false )
      return false;
   _bytes_reverse_32( data_32, data_32 );
   ihdl->wav.sampleRate = data_32;

   if( false == getSampleRateIdx( ihdl, data_32 ) )
      return false; // The sample rate is not supported by ASP.

   // channel count
   if( wavGetDword( ihdl, &data_32 ) == false )
      return false;

   if( data_32 == 0x01000000 ) {
      *DSP_PCM_MS |= 0x0001;
      ihdl->wav.channel_num = 1;
      ihdl->wav.is_stereo = false;
      ihdl->mh.mono2stereo = true;
   } else if( data_32 == 0x02000000 ) {
      *DSP_PCM_MS |= 0x0001;
      ihdl->wav.channel_num = 2;
      ihdl->wav.is_stereo = true;
      ihdl->mh.mono2stereo = false;
   } else
      return false;

   if( ihdl->wav.data_offset < 24 )
      return false;

   ihdl->fileOffset = ihdl->wav.data_offset;
   return true;
}

#if defined(__DRA_DECODE_SUPPORT__)
static Media_Event wavDraFillBitstream(MHdl *hdl)
{
   wavMediaHdl *ihdl = (wavMediaHdl *) hdl;
   
   // Read bitstream in MED task
   if (ihdl->wav.isDRA) {
      if (!hdl->eof) {
         kal_int32 freeLen, segment, uMBufLen;
         
         freeLen = ihdl->wav.bsRead - ihdl->wav.bsWrite - 1;
         if (freeLen < 0) {
            freeLen += ihdl->wav.bsSize;
         }
         
         // First segment
         segment = ihdl->wav.bsSize - ihdl->wav.bsWrite;
         if (segment >= freeLen) {
            segment = freeLen;
         }
         uMBufLen = autGetBytes(hdl, ihdl->pstFSAL, segment, ihdl->wav.bsBase+ihdl->wav.bsWrite);
         if (uMBufLen < 0) {
            return MEDIA_READ_ERROR;
         }
         ihdl->wav.bsWrite += uMBufLen;
         if (ihdl->wav.bsWrite >= ihdl->wav.bsSize) {
            ihdl->wav.bsWrite = 0;
         }
         
         // Secong segment
         if (uMBufLen == segment) {
            segment = freeLen - segment;
            if (segment) {
               uMBufLen = autGetBytes(hdl, ihdl->pstFSAL, segment, ihdl->wav.bsBase);
               if (uMBufLen < 0) {
                  return MEDIA_READ_ERROR;
               }
               ihdl->wav.bsWrite = uMBufLen;
            }
         }
      }
   }
   return MEDIA_NONE;
}
#endif

static Media_Status wavMFPlayFile( MHdl *hdl )
{
   kal_uint32 buf_len;
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;

   hdl->SetBufferInternal(hdl, AUDIO_RB_SIZE_SELF_ALLOC);
   ihdl->fileOffset = 0;
   ihdl->end_status = MEDIA_NONE;
   hdl->rbInfo.read = 0;
   hdl->rbInfo.write = 0;
   hdl->eof = KAL_FALSE;
   hdl->waiting = KAL_FALSE;
   hdl->current_time = hdl->start_time;
   ihdl->decode_EOF = false;
   ihdl->zeroCount = 0;
   ihdl->read_error = false;
   hdl->isUnderflow = KAL_FALSE;
   hdl->LastRingBufferRead = 0;   
   {  /* Fill Media Buffer to parse file header */
      kal_uint8 *pBuf;
      kal_int32 num_bytes;
      hdl->GetWriteBuffer(hdl, &pBuf, &buf_len);
      num_bytes = autGetBytesFromFile(hdl, ihdl->pstFSAL, ihdl->fileOffset, buf_len, pBuf);
      if(num_bytes < 0) return MEDIA_READ_FAIL;
      hdl->WriteDataDone( hdl, num_bytes );
   }

   switch(hdl->mediaType){
      case MEDIA_FORMAT_WAV:
           {
              Media_Status mResult;
              
              /* parse file header */
              mResult = wavReadHeader(ihdl);
              if (mResult != MEDIA_SUCCESS)
                 return mResult;
           }

           if ( wavReachDataChunk(ihdl) == KAL_FALSE ) // reach data offset, "ihdl->fileOffset"
              return MEDIA_BAD_FORMAT;
           break;

      case MEDIA_FORMAT_AU:
           if( !auReadHeader(ihdl) )
              return MEDIA_BAD_FORMAT;
           break;

      case MEDIA_FORMAT_AIFF:
           if( !aiffReadHeader(ihdl) )   /* parse file header */
              return MEDIA_BAD_FORMAT;

           if ( aiffLookForSSND(ihdl) != 2 ) // reach data offset, "ihdl->fileOffset"
              return MEDIA_BAD_FORMAT;
           break;

      default: ASSERT(0);
   }

   hdl->rbInfo.write = 0;
   hdl->rbInfo.read = 0;
   hdl->eof = KAL_FALSE;

   if (wavSeek(ihdl)==KAL_FALSE) // update file offset at start frame
      return MEDIA_SEEK_EOF;

#if defined(MT6251)
   // Due to loading issue, MT6251 doesn't support 44.1 kHz and 48 kHz 
   if (ihdl->wav.sr_index >= 7) {   	  
      return MEDIA_UNSUPPORTED_TYPE;
   }
#endif
#if (defined(MT6253) && defined(__EMI_CLK_52MHZ__)) || defined(MT6253E) || defined(MT6253L)|| defined(MT6252) || defined(MT6252H)
   // Due to loading issue, MT6253D doesn't support 44.1 kHz and 48 kHz
   if (ihdl->wav.sr_index >= 7) {
      return MEDIA_UNSUPPORTED_TYPE;
   }
#endif

#if defined(__DRA_DECODE_SUPPORT__)
   if (ihdl->wav.isDRA) {
      int DraMemSize;
      
      // Configuration of DRA decoder
      ihdl->wav.dra.config.initMode = 0;   // 0: manual, 1: using one frame
      ihdl->wav.dra.config.channel = 2;    // stereo
      ihdl->wav.dra.mixMode = 0;           // Disable down mix
      ihdl->wav.dra.nBitPcm = 16;          // PCM is 16 bits per sample
      
      DraMemSize = DRA_GetMemSize(ihdl->wav.dra.config.channel, ihdl->wav.dra.mixMode);
      ihdl->wav.dra.pDecoder = (void *) audio_alloc_mem_cacheable( sizeof(kal_uint8)*DraMemSize );
      ASSERT(ihdl->wav.dra.pDecoder != NULL);
      DRA_InitDecode(&ihdl->wav.dra.pDecoder, &ihdl->wav.dra.config, 0, 0, 0);
      
      ihdl->wav.dra.pTempOutputBuf = (kal_uint8 *)audio_alloc_mem( sizeof(kal_uint8)*DRA_TEMP_OUTPUT_BUFFER_SIZE );
      ASSERT(ihdl->wav.dra.pTempOutputBuf != NULL);
      ihdl->wav.dra.tempInputBufSize = WAV_MAX_DATA_REQUESTED;
      ihdl->wav.dra.tempOutputBufSize = DRA_TEMP_OUTPUT_BUFFER_SIZE;
      ihdl->wav.dra.remainOutputCount = 0;
      
      ihdl->wav.bsBase = (kal_uint8 *)audio_alloc_mem( sizeof(kal_uint8)*DRA_BITSTREAM_BUFFER_SIZE );
      ASSERT(ihdl->wav.bsBase != NULL);
      ihdl->wav.bsRead = ihdl->wav.bsWrite = 0;
      ihdl->wav.bsSize = DRA_BITSTREAM_BUFFER_SIZE;
      ihdl->wav.isFileAccess = true;
      
      // Execute in L1Audio task
      ihdl->isAudioTask = true;
   }
#endif

   FSAL_Seek( ihdl->pstFSAL, ihdl->fileOffset);

   {
      kal_uint8  *pBuf;
      kal_uint32 uMBufLen;
      while(1){
         hdl->GetWriteBuffer(hdl, &pBuf, &uMBufLen); // Len in byte

         if(ihdl->wav.formatTag == 0x01 && ihdl->wav.BitsPerSample == 16){
            if(hdl->mediaType == MEDIA_FORMAT_WAV){
               if(uMBufLen <= 1 || hdl->eof) break;
               uMBufLen &= 0xFFFFFFFE;
               uMBufLen = autGetBytes(hdl, ihdl->pstFSAL, uMBufLen, pBuf);
            }else{
                if(uMBufLen <= 1 || hdl->eof) break;
                uMBufLen &= 0xFFFFFFFE;
                uMBufLen = ihdl->wav.Wav_PutData(hdl, uMBufLen);
             }
            if((kal_int32)uMBufLen < 0) return MEDIA_READ_ERROR;
            wavUpdateDataLen( ihdl, &uMBufLen);
            hdl->WriteDataDone( hdl, uMBufLen );
         }else if(((ihdl->wav.formatTag == 0x01) && (ihdl->wav.BitsPerSample == 8)) ||
                 (ihdl->wav.formatTag == 0x06) || (ihdl->wav.formatTag == 0x07)){
             if(uMBufLen <= 3 || hdl->eof) break;
             uMBufLen &= 0xFFFFFFFC;
             uMBufLen = ihdl->wav.Wav_PutData(hdl, uMBufLen);
             if((kal_int32)uMBufLen < 0) return MEDIA_READ_ERROR;
             wavUpdateDataLen( ihdl, &uMBufLen);
             hdl->WriteDataDone( hdl, uMBufLen );
          }else{
              if(uMBufLen <= 1 || hdl->eof) break;
              uMBufLen &= 0xFFFFFFFE;
              uMBufLen = ihdl->wav.Wav_PutData(hdl, uMBufLen);
              if((kal_int32)uMBufLen < 0) return MEDIA_READ_ERROR;
              wavUpdateDataLen( ihdl, &uMBufLen);
              hdl->WriteDataDone( hdl, uMBufLen );
           }
#if !APM_SUPPORT
         SPT_PutPcmData( (int16*)pBuf, (uMBufLen>>1), !ihdl->wav.is_stereo );
         /* Send data for spectrum analysis */
#endif
         if((hdl->mediaType == MEDIA_FORMAT_WAV)&&(!ihdl->wav.wDataLength))
            hdl->eof = KAL_TRUE;
      }
   }

#if defined(__DRA_DECODE_SUPPORT__)
   if (ihdl->wav.isDRA) {
      ihdl->wav.isFileAccess = false;
      // Read bitstream in MED task
      wavDraFillBitstream(hdl);
   }
#endif

#if defined(CYBERON_DIC_TTS) || defined(IFLY_TTS) || defined(SINOVOICE_TTS) || defined(VRSI_ENABLE)
   if ( (ihdl->wav.sr_index == 0) && (ihdl->wav.is_stereo == false) ) {
#if defined(CYBERON_DIC_TTS) || defined(IFLY_TTS) || defined(SINOVOICE_TTS)
      if (ttsIsEnable()) {
         hdl->mono2stereo = false;
         ihdl->wav.is_8KVoice = true;
      }
#endif
#if defined(VRSI_ENABLE)
      if (vrsiIsPlaying()) {
         hdl->mono2stereo = false;
         ihdl->wav.is_8KVoice = true;
      }
#endif
   }
#endif

#if defined(__HQA_AUDIO__)
   if ( (ihdl->wav.sr_index == 0) && (ihdl->wav.is_stereo == false) ) {
      hdl->mono2stereo = false;
      ihdl->wav.is_8KVoice = true;
   }
#endif

   wavOpenDevice(ihdl, true);
   hdl->state = WAV_STATE_PLAYING;

   {
      kal_uint32 uBitRate = ihdl->wav.sampleRate * ( ihdl->wav.is_stereo + 1 ) * ihdl->wav.BitsPerSample;
      if(uBitRate)
         hdl->uTotalDuration = (kal_uint64)ihdl->file_size * 8000 / (kal_uint64)uBitRate; // in ms
   }

   kal_trace( TRACE_FUNC, L1AUDIO_PLAY_FILE, hdl->mediaType );
   return MEDIA_SUCCESS;
}

static Media_Status pcmMFPlayFile( MHdl *hdl )
{
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;

   hdl->SetBufferInternal(hdl, AUDIO_RB_SIZE_SELF_ALLOC);
   ihdl->fileOffset = 0;
   ihdl->end_status = MEDIA_NONE;
   hdl->rbInfo.read = 0;
   hdl->rbInfo.write = 0;
   hdl->eof = KAL_FALSE;
   hdl->waiting = KAL_FALSE;
   hdl->current_time = hdl->start_time;
   ihdl->wav.sr_index = 0;  // 8k Hz
   ihdl->wav.channel_num = 1;
   ihdl->wav.is_stereo = false;
   ihdl->wav.sampleRate = 8000;
   hdl->mono2stereo = true;
   ihdl->wav.is_8KVoice = false;
   ihdl->decode_EOF = false;
   ihdl->zeroCount = 0;
   hdl->isUnderflow = KAL_FALSE;
   hdl->LastRingBufferRead = 0;
   switch (hdl->mediaType) {
      case MEDIA_FORMAT_PCM_16K:
         ihdl->wav.sampleRate = 16000;
         ihdl->wav.sr_index = 3;
      case MEDIA_FORMAT_PCM_8K:
         ihdl->wav.BitsPerSample = 16;
         ihdl->wav.formatTag   = 0x0001;
         ihdl->wav.Wav_PutData = LPCM16BitPutData;
         break;
      case MEDIA_FORMAT_G711_ALAW:
         ihdl->wav.BitsPerSample = 8;
         ihdl->wav.formatTag   = 0x0006;
         ihdl->wav.Wav_PutData = wavAlawPutData;
         break;
      case MEDIA_FORMAT_G711_ULAW:
         ihdl->wav.BitsPerSample = 8;
         ihdl->wav.formatTag   = 0x0007;
         ihdl->wav.Wav_PutData = wavUlawPutData;
         break;
      case MEDIA_FORMAT_DVI_ADPCM:
         ihdl->wav.BitsPerSample = 4;
         ihdl->wav.formatTag   = 0x0011;
         ihdl->wav.wBlockAlign  = 256;
         ihdl->wav.samplesPerBlock = 512;
         ihdl->wav.Wav_PutData = pcmDviADPCMPutDataMono;
         break;
      default:
         ASSERT(0);
   }

   if (hdl->mediaType == MEDIA_FORMAT_DVI_ADPCM) {
   	  if (hdl->start_time != 0)
         return MEDIA_SEEK_FAIL;
   } else if (wavSeek(ihdl)==KAL_FALSE) {
      return MEDIA_SEEK_FAIL;
   }
   FSAL_Seek( ihdl->pstFSAL, ihdl->fileOffset);

   {
      kal_uint8  *pBuf;
      kal_uint32 uMBufLen;
      while(1){
         hdl->GetWriteBuffer(hdl, &pBuf, &uMBufLen); // Len in byte

         if ( (hdl->mediaType == MEDIA_FORMAT_PCM_8K) || (hdl->mediaType == MEDIA_FORMAT_PCM_16K) ) {
            if(uMBufLen == 0 || hdl->eof) break;
#ifdef __MMI_DSM_NEW__
	    if(dsmLoopPCM > 0)
               uMBufLen = dsmAudGetBytes(hdl, ihdl->pstFSAL, uMBufLen, pBuf);
          else
#endif
            uMBufLen = autGetBytes(hdl, ihdl->pstFSAL, uMBufLen, pBuf);
            if((kal_int32)uMBufLen < 0) return MEDIA_READ_ERROR;
            hdl->WriteDataDone( hdl, uMBufLen );
         } else if (hdl->mediaType == MEDIA_FORMAT_DVI_ADPCM){
             if(uMBufLen <= 1 || hdl->eof) break;
             uMBufLen &= 0xFFFFFFFE;
             uMBufLen = ihdl->wav.Wav_PutData(hdl, uMBufLen);
             if((kal_int32)uMBufLen < 0) return MEDIA_READ_ERROR;
             hdl->WriteDataDone( hdl, uMBufLen );
         } else {  // A-law and U-law
             if(uMBufLen <= 3 || hdl->eof) break;
             uMBufLen &= 0xFFFFFFFC;
             uMBufLen = ihdl->wav.Wav_PutData(hdl, uMBufLen);
             if((kal_int32)uMBufLen < 0) return MEDIA_READ_ERROR;
             hdl->WriteDataDone( hdl, uMBufLen );
         }
#if !APM_SUPPORT
         SPT_PutPcmData( (int16*)pBuf, (uMBufLen>>1), !ihdl->wav.is_stereo );
         /* Send data for spectrum analysis */
#endif

#ifdef __MMI_DSM_NEW__
	if(dsmLoopPCM >0 )
		break;
#endif
      }
   }


#if defined(CYBERON_DIC_TTS) || defined(IFLY_TTS) || defined(SINOVOICE_TTS) || defined(VRSI_ENABLE)
   if ( (hdl->mediaType != MEDIA_FORMAT_PCM_16K) && (ihdl->wav.is_stereo == false) ) {
#if defined(CYBERON_DIC_TTS) || defined(IFLY_TTS) || defined(SINOVOICE_TTS)
      if (ttsIsEnable()) {
         hdl->mono2stereo = false;
         ihdl->wav.is_8KVoice = true;
      }
#endif
#if defined(VRSI_ENABLE)
      if (vrsiIsPlaying()) {
         hdl->mono2stereo = false;
         ihdl->wav.is_8KVoice = true;
      }
#endif
   }
#endif

   pcmOpenDevice(ihdl);
   hdl->state = WAV_STATE_PLAYING;
   return MEDIA_SUCCESS;
}

static Media_Status wavMFStop( MHdl *hdl )
{
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;

   kal_take_sem( WAVsema, KAL_INFINITE_WAIT );
   if( hdl->state == WAV_STATE_PLAY_PAUSE || hdl->state == WAV_STATE_IDLE ){
      kal_give_sem( WAVsema );
      hdl->FreeBufferInternal(hdl);   
      return MEDIA_SUCCESS;
   }

#if 1 //APM_SUPPORT
   if (AM_IsAudioPlaybackOn()!=-1)
      AFE_SetHardwareMute(true);
#endif

   ihdl->end_status = MEDIA_TERMINATED;
   wavCloseDevice( ihdl );
   hdl->FreeBufferInternal(hdl);   
   hdl->state = WAV_STATE_IDLE;
   
   kal_give_sem( WAVsema );

   kal_trace( TRACE_FUNC, L1AUDIO_STOP, hdl->mediaType );
   return MEDIA_SUCCESS;
}

static Media_Status wavMFPause( MHdl *hdl )
{
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;

   kal_take_sem( WAVsema, KAL_INFINITE_WAIT );

   ASSERT( hdl->state == WAV_STATE_PLAYING );

#if 1 //APM_SUPPORT
   if (AM_IsAudioPlaybackOn()!=-1)
      AFE_SetHardwareMute(true);
#endif

   ihdl->end_status = MEDIA_TERMINATED;
   wavCloseDevice( ihdl );

   hdl->state = WAV_STATE_PLAY_PAUSE;
   
   kal_give_sem( WAVsema );

   kal_trace( TRACE_FUNC, L1AUDIO_PAUSE, hdl->mediaType );
   return MEDIA_SUCCESS;
}

static Media_Status pcmMFPause( MHdl *hdl )
{
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;

   ASSERT( hdl->state == WAV_STATE_PLAYING );

   if (hdl->mediaType == MEDIA_FORMAT_DVI_ADPCM)
      return MEDIA_UNSUPPORTED_OPERATION;

#if 1 //APM_SUPPORT
   if (AM_IsAudioPlaybackOn()!=-1)
      AFE_SetHardwareMute(true);
#endif

   ihdl->end_status = MEDIA_TERMINATED;
   wavCloseDevice( ihdl );

   hdl->state = WAV_STATE_PLAY_PAUSE;
   kal_trace( TRACE_FUNC, L1AUDIO_PAUSE, hdl->mediaType );
   return MEDIA_SUCCESS;
}

static Media_Status wavMFResumeFile( MHdl *hdl )
{
   hdl->start_time = hdl->current_time;
   kal_trace( TRACE_FUNC, L1AUDIO_RESUME, hdl->mediaType );
   return wavMFPlayFile(hdl);
}

static Media_Status pcmMFResumeFile( MHdl *hdl )
{
   hdl->start_time = hdl->current_time;
   kal_trace( TRACE_FUNC, L1AUDIO_RESUME, hdl->mediaType );
   return pcmMFPlayFile(hdl);
}

static void wavDecodeTask( void *data )
{
   MHdl *hdl = (MHdl *)data;
   wavMediaHdl *ihdl = (wavMediaHdl *) hdl;

   if (ihdl->isAudioTask) {
      kal_take_sem( WAVsema, KAL_INFINITE_WAIT );
#if defined(__DRA_DECODE_SUPPORT__)
      ihdl->wav.isRunned = false;
#endif
   }
   
   if( hdl->state != WAV_STATE_PLAYING ){
      if (ihdl->isAudioTask)
         kal_give_sem( WAVsema );
      return;
   }

   {
      kal_uint8  *pBuf;
      kal_uint32 uMBufLen;
      while(1){
         hdl->GetWriteBuffer(hdl, &pBuf, &uMBufLen); // Len in byte

         if(ihdl->wav.formatTag == 0x01 && ihdl->wav.BitsPerSample == 16){
            if ( (hdl->mediaType == MEDIA_FORMAT_WAV)    ||
                 (hdl->mediaType == MEDIA_FORMAT_PCM_8K) ||
                 (hdl->mediaType == MEDIA_FORMAT_PCM_16K) ) {
               if(uMBufLen == 0 || hdl->eof) break;
               uMBufLen = autGetBytes(hdl, ihdl->pstFSAL, uMBufLen, pBuf);
            }else{
                if(uMBufLen <= 1 || hdl->eof) break;
                uMBufLen &= 0xFFFFFFFE;
                uMBufLen = ihdl->wav.Wav_PutData(hdl, uMBufLen);
             }
            if((kal_int32)uMBufLen < 0){
               ihdl->read_error = true;
               if (ihdl->isAudioTask)
                  kal_give_sem( WAVsema );
               return;
            }
            wavUpdateDataLen( ihdl, &uMBufLen);
            hdl->WriteDataDone( hdl, uMBufLen );
         }else if(((ihdl->wav.formatTag == 0x01) && (ihdl->wav.BitsPerSample == 8)) ||
                 (ihdl->wav.formatTag == 0x06) || (ihdl->wav.formatTag == 0x07)){
            if(uMBufLen <= 3 || hdl->eof) break;
            uMBufLen &= 0xFFFFFFFC;
            uMBufLen = ihdl->wav.Wav_PutData(hdl, uMBufLen);
            if((kal_int32)uMBufLen < 0){
               ihdl->read_error = true;
               if (ihdl->isAudioTask)
                  kal_give_sem( WAVsema );
               return;
            }
            wavUpdateDataLen( ihdl, &uMBufLen);
            hdl->WriteDataDone( hdl, uMBufLen );
         }
#if defined(__DRA_DECODE_SUPPORT__)
         else if (ihdl->wav.isDRA) {
            if(uMBufLen <= 1 || ((hdl->eof)&&(ihdl->wav.bsWrite == ihdl->wav.bsRead)) ) break;
            uMBufLen &= 0xFFFFFFFE;
            uMBufLen = ihdl->wav.Wav_PutData(hdl, uMBufLen);
            if((kal_int32)uMBufLen < 0){
               ihdl->read_error = true;
               break;
            }
            wavUpdateDataLen( ihdl, &uMBufLen);
            hdl->WriteDataDone( hdl, uMBufLen );
            break;
         }
#endif
         else
         {
            if(uMBufLen <= 1 || hdl->eof) break;
            uMBufLen &= 0xFFFFFFFE;
            uMBufLen = ihdl->wav.Wav_PutData(hdl, uMBufLen);
            if((kal_int32)uMBufLen < 0){
               ihdl->read_error = true;
               break;
            }
            wavUpdateDataLen( ihdl, &uMBufLen);
            hdl->WriteDataDone( hdl, uMBufLen );
         }
#if !APM_SUPPORT
         SPT_PutPcmData( (int16*)pBuf, (uMBufLen>>1), !ihdl->wav.is_stereo );
         /* Send data for spectrum analysis */
#endif
         if((hdl->mediaType == MEDIA_FORMAT_WAV)&&(!ihdl->wav.wDataLength))
            hdl->eof = KAL_TRUE;
      }
   }

   if (ihdl->isAudioTask)
      kal_give_sem( WAVsema );
}

Media_Event wavMFProcess( MHdl *hdl, Media_Event event )
{
   wavMediaHdl *ihdl = (wavMediaHdl *) hdl;
   ASSERT(ihdl != NULL);

   kal_trace( TRACE_GROUP_AUD_PROCESS, L1AUDIO_PROCESS, hdl->mediaType );

   switch(event)
   {
      case MEDIA_END:
      case MEDIA_STOP_TIME_UP:
         if( hdl->state == WAV_STATE_PLAYING ) {
#if 1 //APM_SUPPORT
            if (AM_IsAudioPlaybackOn()!=-1)
               AFE_SetHardwareMute(true);
#endif
            wavCloseDevice( ihdl );
         }
         hdl->state = WAV_STATE_IDLE;
         return event;
      case MEDIA_ERROR:
         if( hdl->state == WAV_STATE_PLAYING )
            wavCloseDevice( ihdl );
         hdl->state = WAV_STATE_IDLE;
         return event;
      case MEDIA_BUFFER_UNDERFLOW:
      case MEDIA_DATA_REQUEST:
         if (ihdl->isAudioTask) {
#if defined(__DRA_DECODE_SUPPORT__)
            // Read bitstream in MED task
            wavDraFillBitstream(hdl);
#endif
         } else {
#ifdef __MMI_DSM_NEW__
		if(dsmLoopPCM > 0)
			dsmWavDecodeTask( hdl );
		else
#endif
            wavDecodeTask( hdl );
         }
         hdl->waiting = KAL_FALSE;
         
         if( ihdl->read_error ) return MEDIA_READ_ERROR;
         
         if ( ihdl->isUpdateDur ){
            ihdl->isUpdateDur = false;
            return MEDIA_NONE;
         }
         return MEDIA_NONE;
      default:
         ASSERT(0); // should never reach here
   }
   return MEDIA_NONE;
}

kal_uint32 wavMFGetTotalDuration( MHdl *hdl )
{
   kal_uint32 uDur = 0;
   wavMediaHdl *ihdl = (wavMediaHdl *) hdl;
   
   
#if defined(__DRA_DECODE_SUPPORT__)
   if (ihdl->wav.isDRA)
   {
      kal_uint32 TotalFrames;
      ASSERT(ihdl->AudioSeekTbl != 0);
      //ASSERT(ihdl->frameDur != 0);
      if(1==ihdl->AudioSeekTbl->IsFail || 1==ihdl->AudioSeekTbl->IsDone)
         TotalFrames=AST_GetIndexedFrames(ihdl->AudioSeekTbl);
      else
         TotalFrames=AST_EstimateTotalFrames(ihdl->AudioSeekTbl,ihdl->file_size);
      if(TotalFrames==0)
      {
         uDur = 0;
      }
      uDur = (kal_uint64)TotalFrames*AST_GetFrameDuration(ihdl->AudioSeekTbl)/1000000;
   }
   else
#endif
   {
      kal_int64 sampleCount;
      
      if (4 != ihdl->wav.BitsPerSample) 
      {
         if(0 == ihdl->wav.BitsPerSample)
            sampleCount = 0;
         else
            sampleCount = (kal_int64)ihdl->wav.wDataChunkLength * 8 / ihdl->wav.BitsPerSample;
      } else {
        kal_int64 len, samplePerBlock;
        len = (kal_int64)ihdl->wav.wDataChunkLength;
        samplePerBlock = (kal_int64)ihdl->wav.samplesPerBlock;
        if(0 == ihdl->wav.wBlockAlign)
           sampleCount = 0;
        else
           sampleCount = (kal_int64)len * samplePerBlock * ihdl->wav.channel_num / ihdl->wav.wBlockAlign;
      }
      if( 0 == ihdl->wav.sampleRate || 0 == ihdl->wav.sampleRate)
         uDur = 0;
      else
         uDur = sampleCount * 1000 / (ihdl->wav.channel_num * ihdl->wav.sampleRate);
   }

   kal_trace( TRACE_GROUP_CACHE, L1AUDIO_CACHE_GET_TOTAL_DUR, hdl->mediaType, uDur );
   return uDur;
}

kal_uint32 wavMFGetCacheDur(MHdl *hdl)
{
#if defined(__DRA_DECODE_SUPPORT__)
   wavMediaHdl *ihdl = (wavMediaHdl *) hdl;

   if (ihdl->wav.isDRA) 
   { 
      kal_uint32 uDur, CurFrames;
      ASSERT(ihdl->AudioSeekTbl != 0);
      CurFrames=AST_GetIndexedFrames(ihdl->AudioSeekTbl);
      uDur = (kal_uint64)CurFrames*AST_GetFrameDuration(ihdl->AudioSeekTbl)/1000000;
      kal_trace( TRACE_GROUP_CACHE, L1AUDIO_CACHE_GET_DUR, hdl->mediaType, uDur );
      return uDur;
   }
#endif
   return wavMFGetTotalDuration(hdl);
}

AST_Status wavASTParseFileHeader(void *hdlptr, kal_uint32 *FirstFrameOffset)
{
   MHdl *hdl = hdlptr;
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;	
   kal_uint8 *pBuf;
   kal_int32 num_bytes;
   kal_uint32 buf_len; 
   hdl->rbInfo.read = 0;
   hdl->rbInfo.write = 0;
   hdl->GetWriteBuffer(hdl, &pBuf, &buf_len);
   ihdl->fileOffset=0;
   hdl->current_time = hdl->start_time;
   ihdl->decode_EOF = false;
   ihdl->zeroCount = 0;
   num_bytes = autGetBytesFromFile(hdl, ihdl->pstFSAL, ihdl->fileOffset, buf_len, pBuf);
   if (num_bytes >= 0) 
   {
         hdl->WriteDataDone( hdl, num_bytes );
   }

   if (wavReadHeader(ihdl)!=MEDIA_SUCCESS )
   {
      return AST_PARSE_FILE_HEADER_FAIL;
   }
   wavReachDataChunk(ihdl);
   *FirstFrameOffset=ihdl->wav.dataChunkOffset;
#if defined(__DRA_DECODE_SUPPORT__) 
   if((ihdl->wav.isDRA) && (ihdl->AudioSeekTbl))
   {
      AST_SetFrameDuration(ihdl->AudioSeekTbl, ihdl->frameDur);
   }
#endif
   kal_trace( TRACE_GROUP_CACHE, L1AUDIO_CACHE_STROFFSET, hdl->mediaType, 0, ihdl->fileOffset );
   return AST_SUCCESS;
}

#if defined(__DRA_DECODE_SUPPORT__)

AST_Status wavASTParseFrame(void *hdlptr, kal_uint32 *CurOffset, kal_uint32 *ParseFrameNum)
{
   MHdl *hdl = hdlptr;
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;	

   AST_Status ParseResult;
   kal_uint32 AccOffset, Remains;
   kal_int32 num_bytes;
   kal_uint8 *pTempIn;
   kal_uint16 sync, msg, frameLen;

   AccOffset = *CurOffset;
   Remains=*ParseFrameNum;
   pTempIn = ihdl->wav.dra.pTempInputBuf;  
   ParseResult = AST_SUCCESS;

   while(Remains!=0)
   {
 
      // Get 4 bytes
      num_bytes = autGetBytesFromFile((MHdl *)ihdl, ihdl->pstFSAL, AccOffset, 4, pTempIn);
      if(num_bytes < 0)
      {
         ParseResult = AST_EOF;
	  break;
      	}
      if(num_bytes != 4)
      {
         ParseResult = AST_EOF;
	  break;
      	}    
      sync = (pTempIn[1]<<8) | (pTempIn[0]);
      msg =  (pTempIn[3]<<8) | (pTempIn[2]);
      if (sync != 0x7FFF)
      {
         ParseResult = AST_PARSE_FRAME_FAIL;
	  break;
      	}      
      // To parse the msg.
      // For bit 32, 0: normal with 10 bits; 1: extended with 13 bits
      if(msg >> 15)
      {
         // extended
         frameLen = (msg<<1)>>3;
      }
      else
      {
         // normal
         frameLen = (msg<<1)>>6;
      }
      frameLen = frameLen*4;

      AccOffset+=frameLen;
      frameLen=0;
      Remains--;
   }
   //AccOffset+=frameLen;	  
   *CurOffset=AccOffset;
   *ParseFrameNum -= Remains;
   return ParseResult;
}

#endif

static void wavMFBuildCache( MHdl *hdl, Media_Status *eLastRet, kal_uint32 *uProgress )
{
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;
   if(0 == hdl->uGetDurProgress)
   {
      kal_uint32 FirstFrameOffset;
      *uProgress=0;
      wavASTParseFileHeader(hdl,&FirstFrameOffset);
   }
   else
      *uProgress = hdl->uGetDurProgress;
    
#if defined(__DRA_DECODE_SUPPORT__)
   if (ihdl->wav.isDRA)
   {
     AST_Status ParseResult;
     ParseResult = AST_BuildTable(hdl, ihdl->AudioSeekTbl, uProgress, ihdl->file_size );
   if (AST_SUCCESS == ParseResult || AST_PARSE_FRAME_BREAK == ParseResult)
   { 
      *eLastRet = MEDIA_SUCCESS;
   }
   else if (AST_EOF == ParseResult)
   {
      *uProgress=100;
      *eLastRet = MEDIA_SUCCESS;
   }
   else if (AST_PARSE_FRAME_FAIL == ParseResult )
   {
      *uProgress=100;
      *eLastRet = MEDIA_SEEK_FAIL;
   }
   else
   {
       *eLastRet = MEDIA_FAIL;
   }
   
     hdl->uGetDurProgress = *uProgress;
     if( 100 == hdl->uGetDurProgress )
     {
      *eLastRet = MEDIA_SUCCESS;
      hdl->start_time = 0;
      ihdl->fileOffset = 0;
      }
   }
   else
#endif
   {
      *uProgress = 100;
      hdl->uGetDurProgress = *uProgress;
      *eLastRet = MEDIA_SUCCESS;
   }
   kal_trace( TRACE_GROUP_CACHE, L1AUDIO_BUILD_CACHE, hdl->mediaType, *eLastRet, *uProgress );
   (void) ihdl;
}

static void wavMFSetCacheTbl( MHdl *hdl, kal_uint8 *ptr, kal_uint32 usize )
{
#if defined(__DRA_DECODE_SUPPORT__)
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;
   ASSERT(((uint32)ptr & 0x03) == 0 );   /* make sure 4-byte alignment */
   ihdl->AudioSeekTbl = (AudioSeekTable *)ptr;

   if(ihdl->AudioSeekTbl->CurrentIndex==0)
   {
      AST_InitTable(ihdl->AudioSeekTbl,DRA_AST_MIN_ENTRY_SIZE,usize,wavASTParseFrame,wavASTParseFileHeader);
   }
   kal_trace( TRACE_GROUP_CACHE, L1AUDIO_SET_CACHE_TBL, hdl->mediaType );
#endif
}

static kal_uint32 wavMFGetFileOffset(MHdl *hdl)
{
   wavMediaHdl *ihdl = (wavMediaHdl *)hdl;
   return ihdl->fileOffset;
}

static void wavHdlInit(wavMediaHdl *ihdl)
{
   MHdl *hdl = (MHdl *) ihdl;

   hdl->state     = WAV_STATE_IDLE;
   hdl->Close     = wavMFClose;
   hdl->Pause     = wavMFPause;
   hdl->Stop      = wavMFStop;
   hdl->Process   = wavMFProcess;

   hdl->Play      = wavMFPlayFile;
   hdl->Resume    = wavMFResumeFile;

   hdl->BuildCache    = wavMFBuildCache;
   hdl->SetCacheTbl  = wavMFSetCacheTbl;
   hdl->GetCacheDuration = wavMFGetCacheDur;
   hdl->GetTotalDuration = wavMFGetTotalDuration;
   hdl->GetFileOffset = wavMFGetFileOffset;
   FSAL_GetFileSize( ihdl->pstFSAL, &ihdl->file_size);
}

static void wavAllocAndInit(void(*handler)( MHdl *handle, Media_Event event ), STFSAL *pstFSAL, MHdl **pHdl)
{
   wavMediaHdl *ihdl;
   MHdl *hdl;
#if APM_SUPPORT
   ihdl = (wavMediaHdl*) APM_Allocate_Buffer(sizeof(wavMediaHdl));
   if (ihdl != NULL)
   {
      memset(ihdl, 0, sizeof(wavMediaHdl));
      ihdl->fUseTCMFromAPM = true;
   }
   else
#endif
   {
      ihdl = (wavMediaHdl*)get_ctrl_buffer( sizeof(wavMediaHdl) );
      memset(ihdl, 0, sizeof(wavMediaHdl));
   }

#if defined(__DRA_DECODE_SUPPORT__)
   ihdl->wav.dra.pTempInputBuf = (kal_uint8 *) ihdl->buf;
#endif

   hdl = (MHdl *) ihdl;
   if( handler != NULL )
      mhdlInit( hdl, DP_D2C_PCM_P, wavHisr );
   else
      mhdlInitEx( hdl, DP_D2C_PCM_P, wavHisr );

   hdl->handler   = handler;
   ihdl->pstFSAL  = pstFSAL;
   wavHdlInit(ihdl);
   ihdl->aud_id_decoder = L1Audio_GetAudioID();
   L1Audio_SetEventHandler( ihdl->aud_id_decoder, wavDecodeTask );

   kal_trace( TRACE_FUNC, L1AUDIO_OPEN_FILE, hdl->mediaType );
   *pHdl = hdl;
}

MHdl *WAV_Open(void(*handler)( MHdl *handle, Media_Event event ), STFSAL *pstFSAL, void *param)
{
   MHdl *hdl = 0;

   wavAllocAndInit(handler , pstFSAL, &hdl);
   hdl->mediaType = MEDIA_FORMAT_WAV;
   return hdl;
}

#if defined( AIFF_DECODE )
MHdl *AIFF_Open(void(*handler)( MHdl *handle, Media_Event event ), STFSAL *pstFSAL, void *param)
{
   MHdl *hdl = 0;

   wavAllocAndInit(handler , pstFSAL, &hdl);
   hdl->mediaType = MEDIA_FORMAT_AIFF;
   return hdl;
}
#endif

#if defined( AU_DECODE )
MHdl *AU_Open(void(*handler)( MHdl *handle, Media_Event event ), STFSAL *pstFSAL, void *param)
{
   MHdl *hdl = 0;

   wavAllocAndInit(handler , pstFSAL, &hdl);
   hdl->mediaType = MEDIA_FORMAT_AU;
   return hdl;
}
#endif

MHdl *PCM_Open(void(*handler)( MHdl *handle, Media_Event event ), STFSAL *pstFSAL, void *param)
{
   MHdl *hdl;
   wavMediaHdl *ihdl;
   Media_VM_PCM_Param *pcm_param = (Media_VM_PCM_Param *) param;

#if APM_SUPPORT
   ihdl = (wavMediaHdl*) APM_Allocate_Buffer(sizeof(wavMediaHdl));
   if (ihdl != NULL)
   {
      memset(ihdl, 0, sizeof(wavMediaHdl));
      ihdl->fUseTCMFromAPM = true;
   }
   else
#endif
   {
      ihdl = (wavMediaHdl*)get_ctrl_buffer( sizeof(wavMediaHdl) );
      memset(ihdl, 0, sizeof(wavMediaHdl));
   }

   hdl = (MHdl *) ihdl;

   if( handler != NULL )
      mhdlInit( hdl, DP_D2C_PCM_P, wavHisr );
   else
      mhdlInitEx( hdl, DP_D2C_PCM_P, wavHisr );  

   ihdl->aud_id_decoder = L1Audio_GetAudioID();
#ifdef __MMI_DSM_NEW__
	if(dsmLoopPCM > 0 )
		L1Audio_SetEventHandler( ihdl->aud_id_decoder, dsmWavDecodeTask );
	else
#endif
   L1Audio_SetEventHandler( ihdl->aud_id_decoder, wavDecodeTask );

   hdl->handler   = handler;
   hdl->mediaType = pcm_param->mediaType;
   ihdl->pstFSAL  = pstFSAL;

   hdl->state     = WAV_STATE_IDLE;
   hdl->Close     = wavMFClose;
   hdl->Pause     = pcmMFPause;
   hdl->Stop      = wavMFStop;
   hdl->Process   = wavMFProcess;
   hdl->Play      = pcmMFPlayFile;
   hdl->Resume    = pcmMFResumeFile;
   FSAL_GetFileSize( ihdl->pstFSAL, &ihdl->file_size);

   kal_trace( TRACE_FUNC, L1AUDIO_OPEN_FILE, hdl->mediaType );

   return hdl;
}

Media_Status wavCommonInit(STFSAL *pstFSAL, audInfoStruct *contentInfo, MHdl **retHdl)
{
   MHdl *hdl;
   wavMediaHdl *ihdl;

   ihdl = (wavMediaHdl*)get_ctrl_buffer( sizeof(wavMediaHdl) );
   memset(ihdl, 0, sizeof(wavMediaHdl));
   hdl = (MHdl *) ihdl;
   mhdlInit( hdl, 0xFFFF, wavHisr );
   ihdl->pstFSAL = pstFSAL;
   hdl->rbInfo.rb_base = (kal_uint8 *)&ihdl->buf[0];
   hdl->rbInfo.rb_size = WAV_MAX_DATA_REQUESTED;

   memset(contentInfo, 0, sizeof(audInfoStruct));

   {  /* Fill Media Buffer to parse file header */
      kal_int32 num_bytes;
      num_bytes = autGetBytesFromFile(hdl, ihdl->pstFSAL, ihdl->fileOffset, WAV_MAX_DATA_REQUESTED, (kal_uint8 *)&ihdl->buf[0]);
      if(num_bytes < 0){
         mhdlFinalize( hdl, 0xFFFF );
         free_ctrl_buffer( ihdl );
         return MEDIA_READ_FAIL;
      }
      hdl->rbInfo.write += num_bytes;
   }
   *retHdl = (MHdl *) ihdl;
   return MEDIA_SUCCESS;
}

kal_bool WAV_IsPPSupport(STFSAL *pstFSAL)
{
#if defined(__DRA_DECODE_SUPPORT__)
   wavMediaHdl *ihdl;
   MHdl *hdl;
   Media_Status result;
   bool fPPSupport;

   if (pstFSAL == NULL)
      return true;

   ihdl = (wavMediaHdl*)get_ctrl_buffer( sizeof(wavMediaHdl) );
   memset(ihdl, 0, sizeof(wavMediaHdl));
   ihdl->pstFSAL = pstFSAL;
   hdl = (MHdl *) ihdl;
   mhdlInit( hdl, 0xFFFF, wavHisr );
   
   hdl->SetBuffer(hdl, (kal_uint8 *)&ihdl->buf, WAV_MAX_DATA_REQUESTED);

   {  /* Fill Media Buffer to parse file header */
      kal_uint8 *pBuf;
      kal_int32 num_bytes;
      kal_uint32 buf_len;
      
      hdl->GetWriteBuffer(hdl, &pBuf, &buf_len);
      num_bytes = autGetBytesFromFile(hdl, ihdl->pstFSAL, ihdl->fileOffset, buf_len, pBuf);
      if(num_bytes < 0) return MEDIA_READ_FAIL;
      hdl->WriteDataDone( hdl, num_bytes );
   }

   /* parse file header */
   result = wavReadHeader(ihdl);
   if (result != MEDIA_SUCCESS) {
      fPPSupport = false;
   } else if (ihdl->wav.isDRA) {
      fPPSupport = false;
   } else {
      fPPSupport = true;
   }
   
   free_ctrl_buffer( ihdl );
   return fPPSupport;
#else
   return true;
#endif
}

Media_Status WAV_GetContentDescInfo( STFSAL *pstFSAL, audInfoStruct *contentInfo, void *param )
{
   MHdl *hdl;
   wavMediaHdl *ihdl;
   Media_Status mResult;

   if( MEDIA_SUCCESS != (mResult = wavCommonInit(pstFSAL, contentInfo, &hdl)))
      return mResult;

   ihdl = (wavMediaHdl *)hdl;
   /* parse file header */
   mResult = wavReadHeader(ihdl);
   if (mResult != MEDIA_SUCCESS) {
      mhdlFinalize( hdl, 0xFFFF );
      free_ctrl_buffer( ihdl );
      return mResult;
   }

   if ( wavReachDataChunk(ihdl) == KAL_FALSE ){ // reach data offset, "ihdl->fileOffset"
      mhdlFinalize( hdl, 0xFFFF );
      free_ctrl_buffer( ihdl );
      return MEDIA_BAD_FORMAT;
   }

   contentInfo->bitRate = ihdl->wav.AvgBytesPerSec * 8;
   contentInfo->stereo = ihdl->wav.is_stereo;
   contentInfo->sampleRate = ihdl->wav.sampleRate;

#if defined(__DRA_DECODE_SUPPORT__)
   if (ihdl->wav.isDRA) {
      if (ihdl->wav.AvgBytesPerSec) {
         contentInfo->time = (kal_uint64)ihdl->wav.wDataChunkLength * 1000 / ihdl->wav.AvgBytesPerSec;
      }
   } else
#endif
   {
      kal_uint32 file_size;
      kal_uint32 timeData = 0, timeFile = 0;
      
      timeData = (kal_uint64)ihdl->wav.wDataLength * 1000 / (2*ihdl->wav.channel_num * ihdl->wav.sampleRate);
      
      FSAL_GetFileSize( ihdl->pstFSAL, &file_size);
      if (file_size >= ihdl->fileOffset) {
         file_size -= ihdl->fileOffset;
      } else {
         file_size = 0;
      }
      if(contentInfo->bitRate)
         timeFile = (kal_uint64)file_size * 8000 / (kal_uint64)contentInfo->bitRate; // in ms
      
      if (timeData < timeFile)
         contentInfo->time = timeData;
      else
         contentInfo->time = timeFile;
   }

   mhdlFinalize( hdl, 0xFFFF );
   free_ctrl_buffer( ihdl );
   kal_trace( TRACE_FUNC, L1AUDIO_CONTENT, MEDIA_FORMAT_WAV );
   return MEDIA_SUCCESS;
}

Media_Status WAV_GetMetaData( kal_wchar *filename, kal_uint8 *pbBuf, kal_uint32 uSize, audInfoStruct *contentInfo, void *param )
{
   return autGetMetaData( filename, pbBuf, uSize, contentInfo, param, WAV_GetContentDescInfo );
}

#if defined( AIFF_DECODE ) || defined(AU_DECODE)
static void getDataCommon(wavMediaHdl *ihdl, audInfoStruct *contentInfo)
{
   contentInfo->stereo = ihdl->wav.is_stereo;
   contentInfo->sampleRate = ihdl->wav.sampleRate;
   contentInfo->bitRate = ihdl->wav.sampleRate * ( ihdl->wav.is_stereo + 1 ) * ihdl->wav.BitsPerSample;

   {
      kal_uint32 file_size;
      FSAL_GetFileSize( ihdl->pstFSAL, &file_size);
      file_size -= ihdl->wav.data_offset;
      if (contentInfo->bitRate)
         contentInfo->time = (kal_uint64)file_size * 8000 / (kal_uint64)contentInfo->bitRate; // in ms
   }

   mhdlFinalize( (MHdl *)ihdl, 0xFFFF );
   free_ctrl_buffer( ihdl );
}
#endif // #if defined( AIFF_DECODE ) || defined(AU_DECODE)

#if defined( AIFF_DECODE )
Media_Status AIFF_GetContentDescInfo( STFSAL *pstFSAL, audInfoStruct *contentInfo, void *param )
{
   MHdl *hdl;
   wavMediaHdl *ihdl;
   Media_Status mResult;

   if( MEDIA_SUCCESS != (mResult = wavCommonInit(pstFSAL, contentInfo, &hdl)))
      return mResult;

   ihdl = (wavMediaHdl *)hdl;

   if( !aiffReadHeader(ihdl) ){   /* parse file header */
      mhdlFinalize( hdl, 0xFFFF );
      free_ctrl_buffer( ihdl );
      return MEDIA_BAD_FORMAT;
   }

   if ( aiffLookForSSND(ihdl) != 2 ){ // reach data offset, "ihdl->fileOffset"
      mhdlFinalize( hdl, 0xFFFF );
      free_ctrl_buffer( ihdl );
      return MEDIA_BAD_FORMAT;
   }

   getDataCommon(ihdl, contentInfo);

   kal_trace( TRACE_FUNC, L1AUDIO_CONTENT, MEDIA_FORMAT_AIFF );
   return MEDIA_SUCCESS;
}

Media_Status AIFF_GetMetaData( kal_wchar *filename, kal_uint8 *pbBuf, kal_uint32 uSize, audInfoStruct *contentInfo, void *param )
{
   return autGetMetaData( filename, pbBuf, uSize, contentInfo, param, AIFF_GetContentDescInfo );
}
#endif

#if defined( AU_DECODE )
Media_Status AU_GetContentDescInfo( STFSAL *pstFSAL, audInfoStruct *contentInfo, void *param )
{
   MHdl *hdl;
   wavMediaHdl *ihdl;
   Media_Status mResult;

   if( MEDIA_SUCCESS != (mResult = wavCommonInit(pstFSAL, contentInfo, &hdl)))
      return mResult;

   ihdl = (wavMediaHdl *)hdl;

   if( !auReadHeader(ihdl) ){
      mhdlFinalize( hdl, 0xFFFF );
      free_ctrl_buffer( ihdl );
      return MEDIA_BAD_FORMAT;
   }

   getDataCommon(ihdl, contentInfo);

   kal_trace( TRACE_FUNC, L1AUDIO_CONTENT, MEDIA_FORMAT_AU );
   return MEDIA_SUCCESS;
}

Media_Status AU_GetMetaData( kal_wchar *filename, kal_uint8 *pbBuf, kal_uint32 uSize, audInfoStruct *contentInfo, void *param )
{
   return autGetMetaData( filename, pbBuf, uSize, contentInfo, param, AU_GetContentDescInfo );
}
#endif

Media_Status PCM_GetContentDescInfo( STFSAL *pstFSAL, audInfoStruct *contentInfo, void *param )
{
   kal_uint32 file_size;
   Media_Format eFormat;

   memset(contentInfo, 0, sizeof(audInfoStruct));
   contentInfo->sampleRate = 8000;

   eFormat = (Media_Format) ((kal_uint32)param);
   FSAL_GetFileSize(pstFSAL, &file_size);
   switch (eFormat) {
      case MEDIA_FORMAT_DVI_ADPCM:
         contentInfo->time = (file_size >> 2);
         contentInfo->bitRate = 32000;
         break;
      case MEDIA_FORMAT_G711_ALAW:
      case MEDIA_FORMAT_G711_ULAW:
         contentInfo->time = (file_size >> 3);
         contentInfo->bitRate = 64000;
         break;
      case MEDIA_FORMAT_PCM_8K:
         contentInfo->time = (file_size >> 4);
         contentInfo->bitRate = 128000;
         break;
      case MEDIA_FORMAT_PCM_16K:
         contentInfo->sampleRate = 16000;
         contentInfo->time = (file_size >> 5);
         contentInfo->bitRate = 256000;
      default:
         ASSERT(0);
   }
   kal_trace( TRACE_FUNC, L1AUDIO_CONTENT, eFormat );
   return MEDIA_SUCCESS;
}

#ifdef __HQA_AUDIO__
void WAV_HQASetSpOutputDevice( uint8 device )
{
    WaveOutputDev = device;            
}
#endif

#endif //#if defined(WAV_CODEC)
#endif //#ifndef __L1_STANDALONE__
