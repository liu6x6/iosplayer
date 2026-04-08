#include "player.h"
#include "mux.h"
#include "cli.h"
#include "http_client.h"

#include <stdio.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <SDL2/SDL.h>
#include <libavutil/opt.h>
#include <libavutil/imgutils.h>
#include <unistd.h> // for close()
#include <stdint.h> // for intptr_t
#include <math.h> // For fabs

#define SWIPE_THRESHOLD 10.0 // Min distance to be considered a swipe

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

void run_player(int mux_fd) {
    FILE *outputF = NULL;
    check_outPut(g_out_file, &outputF);

    AVFormatContext *pFormatCtx = avformat_alloc_context();
    if (!pFormatCtx) {
        printf("Couldn't allocate format context.\n");
        close(mux_fd);
        return;
    }

    unsigned char *avio_ctx_buffer = NULL;
    size_t avio_ctx_buffer_size = 4096;
    avio_ctx_buffer = av_malloc(avio_ctx_buffer_size);
    if (!avio_ctx_buffer) {
        printf("Couldn't allocate avio buffer.\n");
        avformat_free_context(pFormatCtx);
        close(mux_fd);
        return;
    }

    AVIOContext *avio_ctx = avio_alloc_context(
        avio_ctx_buffer, avio_ctx_buffer_size,
        0, (void*)(intptr_t)mux_fd,
        &read_packet_usbmuxd, NULL, NULL
    );
    if (!avio_ctx) {
        printf("Couldn't allocate avio context.\n");
        av_free(avio_ctx_buffer);
        avformat_free_context(pFormatCtx);
        close(mux_fd);
        return;
    }
    pFormatCtx->pb = avio_ctx;

    if (avformat_open_input(&pFormatCtx, NULL, NULL, NULL) != 0) {
        printf("Couldn't open stream via custom IO.\n");
        av_free(avio_ctx->buffer);
        avio_context_free(&avio_ctx);
        avformat_free_context(pFormatCtx);
        close(mux_fd);
        return;
    }

    if (avformat_find_stream_info(pFormatCtx, NULL) < 0) {
        printf("Couldn't find stream information.\n");
        avformat_close_input(&pFormatCtx);
        close(mux_fd);
        return;
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
        avformat_close_input(&pFormatCtx);
        close(mux_fd);
        return;
    }

    AVCodecParameters *pCodecParams = pFormatCtx->streams[videoStream]->codecpar;
    const AVCodec *pCodec = avcodec_find_decoder(pCodecParams->codec_id);
    if (!pCodec) {
        printf("Unsupported codec!\n");
        avformat_close_input(&pFormatCtx);
        close(mux_fd);
        return;
    }

    AVCodecContext *pCodecCtx = avcodec_alloc_context3(pCodec);
    avcodec_parameters_to_context(pCodecCtx, pCodecParams);
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec.\n");
        avcodec_free_context(&pCodecCtx);
        avformat_close_input(&pFormatCtx);
        close(mux_fd);
        return;
    }

    AVFrame *pFrame = av_frame_alloc();
    AVPacket *packet = av_packet_alloc();

    SDL_Init(SDL_INIT_VIDEO);
    SDL_StartTextInput();
    SDL_Window *screen = SDL_CreateWindow("iOS iPhone", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, pCodecCtx->width / 2, pCodecCtx->height / 2, 0);
    SDL_Renderer *renderer = SDL_CreateRenderer(screen, -1, 0);
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_YV12, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

    struct SwsContext *sws_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BILINEAR, NULL, NULL, NULL);

    AVFrame *pFrameYUV = av_frame_alloc();
    int numBytes = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);
    uint8_t *buffer = (uint8_t *)av_malloc(numBytes * sizeof(uint8_t));
    av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize, buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height, 1);

    SDL_Event event;
    int quit = 0;

    int mouse_down = 0;
    SDL_Point start_point;
    Uint32 start_time;

    while (!quit) {
        if (av_read_frame(pFormatCtx, packet) >= 0) {
            if (packet->stream_index == videoStream) {
                if (outputF) {
                    fwrite(packet->data, 1, packet->size, outputF);
                }
                if (avcodec_send_packet(pCodecCtx, packet) == 0) {
                    while (avcodec_receive_frame(pCodecCtx, pFrame) == 0) {
                        sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
                        SDL_UpdateYUVTexture(texture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0], pFrameYUV->data[1], pFrameYUV->linesize[1], pFrameYUV->data[2], pFrameYUV->linesize[2]);
                        SDL_RenderClear(renderer);
                        SDL_RenderCopy(renderer, texture, NULL, NULL);
                        SDL_RenderPresent(renderer);
                    }
                }
            }
            av_packet_unref(packet);
        } else {
            quit = 1;
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = 1;
            
            int window_w, window_h;
            SDL_GetWindowSize(screen, &window_w, &window_h);

            if (event.type == SDL_MOUSEBUTTONDOWN) {
                mouse_down = 1;
                start_point.x = event.button.x;
                start_point.y = event.button.y;
                start_time = event.button.timestamp;
            }
            if (event.type == SDL_MOUSEBUTTONUP && mouse_down) {
                mouse_down = 0;
                double start_x = (double)start_point.x / window_w * pCodecCtx->width;
                double start_y = (double)start_point.y / window_h * pCodecCtx->height;
                double end_x = (double)event.button.x / window_w * pCodecCtx->width;
                double end_y = (double)event.button.y / window_h * pCodecCtx->height;

                double distance = fabs(end_x - start_x) + fabs(end_y - start_y);

                if (distance < SWIPE_THRESHOLD) {
                    printf("Tap at (%.2f, %.2f)\n", end_x, end_y);
                    send_tap_request(end_x, end_y);
                } else {
                    Uint32 end_time = event.button.timestamp;
                    double duration = (double)(end_time - start_time) / 1000.0;
                    printf("Swipe from (%.2f, %.2f) to (%.2f, %.2f) in %.2fs\n", start_x, start_y, end_x, end_y, duration);
                    send_swipe_request(start_x, start_y, end_x, end_y, duration);
                }
            }
            if (event.type == SDL_TEXTINPUT) {
                printf("Text input: %s\n", event.text.text);
                send_keys_request(event.text.text);
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_RETURN:
                        send_keys_request("\n");
                        break;
                    case SDLK_BACKSPACE:
                        send_keys_request("\b");
                        break;
                }
            }
        }
    }

    if (outputF) {
        fclose(outputF);
    }

    SDL_StopTextInput();
    close(mux_fd);
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
}
