#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <SDL2/SDL.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <string.h>
#include <getopt.h>


typedef struct {
    char ip[64];
    int port;
} StreamOptions;


StreamOptions g_options ; // = { "127.0.0.1", 10001 };  // Default: localhost:10001
char g_out_file[256] = "";


void parse_arguments(int argc, char *argv[]) {
    int opt;
        struct option long_options[] = {
            {"ip",   required_argument, 0, 'i'},
            {"port", required_argument, 0, 'p'},
            {"out",  required_argument, 0, 'o'},
            {0, 0, 0, 0}
        };

        while ((opt = getopt_long(argc, argv, "i:p:o:", long_options, NULL)) != -1) {
            switch (opt) {
                case 'i':
                    strncpy(g_options.ip, optarg, sizeof(g_options.ip) - 1);
                    g_options.ip[sizeof(g_options.ip) - 1] = '\0';  // ensure null-terminated
                    break;
                case 'p':
                    g_options.port = atoi(optarg);
                    break;
            case 'o':
                    strncpy(g_out_file, optarg, sizeof(g_out_file) - 1);
                    g_out_file[sizeof(g_out_file) - 1] = '\0';
                    break;
                default:
                fprintf(stderr, "Usage: %s [-i ip_address] [-p port] [-o output_file]\n", argv[0]);
                    exit(EXIT_FAILURE);
            }
        }
}

void check_outPut(char* out_file, FILE** file) {
    if (strlen(out_file) > 0) {
        *file = fopen(out_file, "wb");
        if (!file) {
            perror("Failed to open output file");
            exit(EXIT_FAILURE);
        }
        printf("Saving stream to: %s\n", out_file);
    }
}


int main(int argc, char *argv[]) {
//    const char *url = "tcp://127.0.0.1:10001";
    parse_arguments(argc, argv);

    printf("Connecting to %s:%d\n", g_options.ip, g_options.port);

    FILE *outputF = NULL;
    check_outPut(g_out_file, &outputF);

    char url[256];
    snprintf(url, sizeof(url), "tcp://%s:%d", g_options.ip, g_options.port);

    avformat_network_init();

    AVFormatContext *pFormatCtx = NULL;
    if (avformat_open_input(&pFormatCtx, url, NULL, NULL) != 0) {
        printf("Couldn't open stream.\n");
        return -1;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Couldn't find stream information.\n");
        return -1;
    }

    int videoStream = -1;
    for (int i = 0; i < pFormatCtx->nb_streams; i++) {
        
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoStream = i;
            break;
        }
    }
    if (videoStream == -1) {
        printf("Didn't find a video stream.\n");
        return -1;
    }

    AVCodecParameters *pCodecParams = pFormatCtx->streams[videoStream]->codecpar;
    const AVCodec *pCodec = avcodec_find_decoder(pCodecParams->codec_id);
    if (!pCodec) {
        printf("Unsupported codec!\n");
        return -1;
    }

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pCodecParams);
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec.\n");
        return -1;
    }

    AVFrame *pFrame = av_frame_alloc();
    AVPacket *packet = av_packet_alloc();

    // SDL setup
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *screen = SDL_CreateWindow("iOS iPhone",
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          pCodecCtx->width / 2,
                                          pCodecCtx->height / 2,
                                          0);
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer,
                                             SDL_PIXELFORMAT_YV12,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             pCodecCtx->width,
                                             pCodecCtx->height);

    struct SwsContext *sws_ctx = sws_getContext(
        pCodecCtx->width,
        pCodecCtx->height,
        pCodecCtx->pix_fmt,
        pCodecCtx->width,
        pCodecCtx->height,
        AV_PIX_FMT_YUV420P,
        SWS_BILINEAR,
        NULL, NULL, NULL
    );

    AVFrame *pFrameYUV = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P,
                                            pCodecCtx->width,
                                            pCodecCtx->height, 1);
    uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, buffer,
                         AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

    SDL_Event event;
    int quit = 0;

    while (!quit) {
        if (av_read_frame(pFormatCtx, packet) >= 0) {
    
            if (packet->stream_index == videoStream) {
                if (outputF) {
                    fwrite(packet->data, 1, packet->size, outputF);
                }
                
                
                if (avcodec_send_packet(pCodecCtx, packet) == 0) {
                    while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
                        sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
                                  pFrame->linesize, 0, pCodecCtx->height,
                                  pFrameYUV->data, pFrameYUV->linesize);

                        SDL_UpdateYUVTexture(texture, NULL,
                                             pFrameYUV->data[0], pFrameYUV->linesize[0],
                                             pFrameYUV->data[1], pFrameYUV->linesize[1],
                                             pFrameYUV->data[2], pFrameYUV->linesize[2]);

                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, texture, NULL, NULL);
                        SDL_RenderPresent(renderer);
                    }
                }
            }
            av_packet_unref(packet);
        } else {
            SDL_Delay(10);
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = 1;
        }
    }

    
    if (outputF) {
        fclose(outputF);
    }
    
    // Cleanup
    av_frame_free(&pFrame);
    av_frame_free(&pFrameYUV);
    av_packet_free(&packet);
    avcodec_free_context(&pCodecCtx);
    avformat_close_input(&pFormatCtx);
    sws_freeContext(sws_ctx);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(screen);
    SDL_Quit();

    return 0;
}
