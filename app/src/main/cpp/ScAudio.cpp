//
// Created by SuperChen on 8/6/21.
//

#include "ScAudio.h"
#include "ScManager.h"
#include "ScLog.h"

void ScAudio::startPlay() {
    //拿到音频流
    AVStream *avStream = ScManager::getInstance().getAvstream(ScManager::getInstance().streamIndexAudio);
    //拿到解码器的参数信息
    AVCodecParameters *parameters = avStream->codecpar;
    //根据视频数据的编码信息来生成解码器
    const AVCodec *codec = avcodec_find_decoder(parameters->codec_id);
    //实例化音频流上下文
    avCodecContext = avcodec_alloc_context3(codec);
    if (!avCodecContext){
        LOGI("初始化音频流上下文失败");
        return;
    }
    //将流的参数信息数据填充到解码器上下文
    int ret = avcodec_parameters_to_context(avCodecContext, parameters);
    if (ret < 0){
        LOGI("avcodec_parameters_to_context() failed: %d\n", ret);
        return;
    }
    //开始让解码器工作
    ret = avcodec_open2(avCodecContext, codec, 0);
    if (ret < 0){
        LOGI("avcodec_open2 failed: %d\n", ret);
        return;
    }

    LOGEA("音频总帧数frame_size=%d", parameters->frame_size);
    LOGEA("音频延迟时间video_delay=%d", parameters->video_delay);
    LOGEA("音频采样率bit_rate=%ld", (long) avCodecContext->bit_rate);

//    if (swrContext == nullptr) {
        swrContext = swr_alloc();
//    }

    //配置音频转换器参数  上下文
    swr_alloc_set_opts(swrContext,
                       //输入输入配置  通道数（立体声)        采样位数为16位                      采样率
                       AV_CH_LAYOUT_STEREO, AV_SAMPLE_FMT_S16, avCodecContext->sample_rate,
                       //输入数据配置  通道数（立体声)        采样位数为16位
                       avCodecContext->channel_layout, avCodecContext->sample_fmt,
                       //采样率
                       avCodecContext->sample_rate, 0, nullptr);
    swr_init(swrContext);

    //回调方法
    if (ScManager::getInstance().env != nullptr && ScManager::getInstance().joPlayer != nullptr && ScManager::getInstance().methodCreateAudioTrack != nullptr) {
        if (LOGS_ENABLED) LOGI("video start callback onSizeChange() method");
//        ScManager::getInstance().env->CallVoidMethod(ScManager::getInstance().joPlayer, ScManager::getInstance().methodCreateAudioTrack, 441avCodecContext->sample_rate00, avCodecContext->channel_layout);
        ScManager::getInstance().env->CallVoidMethod(ScManager::getInstance().joPlayer, ScManager::getInstance().methodCreateAudioTrack, 44100, av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO));
    }

    //获取时间基,根据哥伦布编码存的时间序号需要依赖于timeBase
    singleAudioTime = av_q2d(avStream->time_base);
    LOGEA("decodeAudio ===> singleAudioTime=%f", singleAudioTime);

    pthread_create(&thread_decode_audio, nullptr, decodeAudio, nullptr);
}

void *ScAudio::decodeAudio(void *pVoid) {
    LOGEA("decodeVideo ===> start");
    //输出数据的缓冲区
    uint8_t *outBuffer = (uint8_t *)(av_malloc(44100 * 2));
    AVFrame *audioFrame;

    while (ScManager::getInstance().isStart) {
        //如果是在拖动进度条的过程中
        if (ScManager::getInstance().isSeek) {
            av_usleep(1000*10);
            continue;
        }
        //从队列中取出数据 h264数据
        AVPacket *audioPacket = av_packet_alloc();
//        ScManager::getInstance().audioQueue->getAvPacket(audioPacket);
          audioPacket = ScManager::getInstance().audioQueue->getAvPacket();
        if (audioPacket == nullptr) {
            av_packet_free(&audioPacket);
//            av_free(audioPacket);
            audioPacket = nullptr;
//            LOGEA("decodeAudio ===> audioPacket is null");
            continue;
        }
//        LOGEA("decodeAudio ===> audioPacket.size=%d ,index=%d ,data=%s", audioPacket->size, audioPacket->stream_index, audioPacket->data);
        LOGEA("decodeAudio ===> audioPacket.size=%d ,index=%d", audioPacket->size, audioPacket->stream_index);
        //解码
        int ret = avcodec_send_packet(ScManager::getInstance().scAudio->avCodecContext, audioPacket);
        if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF) {
            LOGEAERROR("decodeAudio ===> 解码出错");
            continue;
        }
        LOGEA("decodeAudio ===> ret1=%d", ret);
        audioFrame = av_frame_alloc();
        //返回结果==0，代表解码成功，但并不代表渲染成功
        ret = avcodec_receive_frame(ScManager::getInstance().scAudio->avCodecContext, audioFrame);
        LOGEA("decodeAudio ===> ret2=%d", ret);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
            //已经读完了
            break;
        }
        if (ret != 0) {
            av_packet_free(&audioPacket);
            av_free(audioPacket);
            audioPacket = nullptr;
            continue;
        }
        //audioFrame.data 为原始数据
//        LOGEA("decodeAudio ===> avFrame.data=%s, length=%d,", audioFrame->data, audioFrame->linesize[0]);

        //获取当前帧的时间
        ScManager::getInstance().scAudio->audioNowTime = audioFrame->pts * ScManager::getInstance().scAudio->singleAudioTime;
        LOGEA("decodeAudio ===> audioNowTime=%f", ScManager::getInstance().scAudio->audioNowTime);

        swr_convert(ScManager::getInstance().scAudio->swrContext, &outBuffer, 44100 * 2,
                    //输入的数据
                    (const uint8_t **) (audioFrame->data),
                   //输入数据的长度
                    audioFrame->nb_samples);
        //获取立体声通道数
        int outChannelLayoutNbCount = av_get_channel_layout_nb_channels(AV_CH_LAYOUT_STEREO);
        //算出这一帧的大小
        int size = av_samples_get_buffer_size(nullptr, outChannelLayoutNbCount, audioFrame->nb_samples, AV_SAMPLE_FMT_S16, 1);
        LOGEA("decodeAudio ===> size=%d", size);

        //绑定线程
        JNIEnv *jniEnv;
        //回调方法
        if (ScManager::getInstance().vm == nullptr ||
            ScManager::getInstance().vm->AttachCurrentThread(&jniEnv, 0) != JNI_OK) {
            /*av_frame_free(&audioFrame);
            av_free(audioFrame);
            audioFrame = nullptr;
            av_packet_free(&audioPacket);
            av_free(audioPacket);
            audioPacket = nullptr;*/
            continue;
        }
        LOGEA("decodeAudio ===> startCallBack");
        //将c++的 uint_8 数据copy到java的byte[]
        jbyteArray byteArray = jniEnv->NewByteArray(size);
        jniEnv->SetByteArrayRegion(byteArray, 0, size, (const jbyte *)(outBuffer));
//            jniEnv->CallVoidMethod(ScManager::getInstance().joPlayer, ScManager::getInstance().methodCreateAudioTrack, audioContext->sample_rate, audioContext->channel_layout);
        //回调给java层
        jniEnv->CallVoidMethod(ScManager::getInstance().joPlayer, ScManager::getInstance().methodPlayAudio, byteArray, size);
        jniEnv->DeleteLocalRef(byteArray);
        //将线程给剥离出来
        ScManager::getInstance().vm->DetachCurrentThread();

        av_frame_free(&audioFrame);
        av_free(audioFrame);
        audioFrame = nullptr;
    }

    av_frame_free(&audioFrame);
    ScManager::getInstance().scAudio->stopPlay();
    return nullptr;
}

void ScAudio::stopPlay() {
    swr_free(&swrContext);
    avcodec_close(avCodecContext);
    LOGEA("decodeAudio ===> stopPlay");
}

void ScAudio::realease() {

}

