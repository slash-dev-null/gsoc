//Lightweight player to test gpu assisted h264 decoding
//Renders a couple frames using libavcodec
//ode based on a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)

#include "avcodec.h"
#include "avformat.h"
#include <stdio.h>

void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame) {
  FILE *pFile;
  char szFilename[32];
  int  y;
  
  // Open file
  sprintf(szFilename, "frame%d.ppm", iFrame);
  pFile=fopen(szFilename, "wb");
  if(pFile==NULL)
    return;
  
  // Write header
  fprintf(pFile, "P6\n%d %d\n255\n", width, height);
  
  // Write pixel data
  for(y=0; y<height; y++)
    fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
  
  // Close file
  fclose(pFile);
}

int main(int argc, char *argv[])
{
  AVFormatContext *pFormatCtx;
  int             i, err, videoStream;
  AVCodecContext  *pCodecCtx;
  AVCodec         *pCodec;
  AVFrame         *pFrame; 
  AVFrame         *pFrameRGB;
  AVPacket        packet;
  int             frameFinished;
  int             numBytes;
  uint8_t         *buffer;
  int numFrames = 5;

  if(argc < 2)
  {
    printf("Usage: gpu_player file [frames test]\n");
    return -1;
  }

  av_register_all();

  if(av_open_input_file(&pFormatCtx,argv[1], NULL, 0, NULL)!=0)
    {
      printf("Couldn't open video file\n");
      return -1;
    }


  if(av_find_stream_info(pFormatCtx)<0)
    return -1; // Couldn't find stream information
  
  // Dump information about file onto standard error
  dump_format(pFormatCtx, 0, argv[1], 0);
  
  videoStream=-1;
  for(i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO) {
      videoStream=i;
      break;
    }

  if(videoStream==-1)
  {
    printf("Didn't find a video stream\n");
    return -1;
  }

  pCodecCtx=pFormatCtx->streams[videoStream]->codec;

#if 0 //disabe gpu
  if(argc > 2)
  {
    if(argc > 3)
    {
      if(!strcmp("dct", argv[3]))
	pCodecCtx->dct_test = 1;
      else if(!strcmp("motion", argv[3]))
	pCodecCtx->mo_comp_test = 1;
      else
      {
	printf("invalid test specified. Valid tests are: dct, motion\n");
	return -1;
      }
    }
  }
  pCodecCtx->gpu = 1;
#endif


  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);

  if(pCodec==NULL)
  {
    fprintf(stderr, "Unsupported codec!\n");
    return -1;
  }


  if(avcodec_open(pCodecCtx, pCodec)<0)
  {
    printf("Could not open codec\n");
    return -1;
  }

  // Allocate video frame
  pFrame=avcodec_alloc_frame();

  // Allocate an AVFrame structure
  pFrameRGB=avcodec_alloc_frame();
  if(pFrameRGB==NULL)
    return -1;

  // Determine required buffer size and allocate buffer
  numBytes=avpicture_get_size(PIX_FMT_RGB24, pCodecCtx->width,
			      pCodecCtx->height);
  buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

  // Assign appropriate parts of buffer to image planes in pFrameRGB
  // Note that pFrameRGB is an AVFrame, but AVFrame is a superset
  // of AVPicture
  avpicture_fill((AVPicture *)pFrameRGB, buffer, PIX_FMT_RGB24,
		 pCodecCtx->width, pCodecCtx->height);


  // Read frames and save first five frames to disk
  i=0;
  while(av_read_frame(pFormatCtx, &packet)>=0) {
    // Is this a packet from the video stream?
    if(packet.stream_index==videoStream) {
      // Decode video frame
      avcodec_decode_video(pCodecCtx, pFrame, &frameFinished, 
         packet.data, packet.size);
#if 0 
      // Did we get a video frame?
      if(frameFinished) {
  // Convert the image from its native format to RGB
  img_convert((AVPicture *)pFrameRGB, PIX_FMT_RGB24, 
                    (AVPicture*)pFrame, pCodecCtx->pix_fmt, pCodecCtx->width, 
                    pCodecCtx->height);
  
  // Save the frame to disk
  if(++i<=1)
    SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height, 
        i);
      }
#endif
    }
    
    // Free the packet that was allocated by av_read_frame
    av_free_packet(&packet);
  }
  
  // Free the RGB image
  av_free(buffer);
  av_free(pFrameRGB);
  
  // Free the YUV frame
  av_free(pFrame);
  
  // Close the codec
  avcodec_close(pCodecCtx);
  
  // Close the video file
  av_close_input_file(pFormatCtx);
  
  return 0;
}

#if 0
  //Decode a couple frames
  for(i=0; i < numFrames; i++)
  {
    av_read_frame(pFormatCtx, &packet);
    if(packet.stream_index==videoStream)
    {
      avcodec_decode_video(pCodecCtx, pFrame, &frameFinished,
                           packet.data, packet.size);
    }


  
  }
  // Close the codec
  avcodec_close(pCodecCtx);

  // Close the video file
  av_close_input_file(pFormatCtx);
  return 0;
}
#endif
