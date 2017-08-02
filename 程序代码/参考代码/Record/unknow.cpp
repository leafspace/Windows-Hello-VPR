int setAudioInputVolumn(DWORD dwVolume)
{
    //初始化相关结构体
    MMRESULT          rc;
    HMIXER            hMixer;
    MIXERLINE         mxl;
    MIXERLINECONTROLS mxlc;
    MIXERCONTROL      mxc;

    //①遍历系统的混音器，直到找到麦克风的混音器，记录该设备ID
    for (int deviceID = 0; true; deviceID++)
    {
        //打开当前的混音器，deviceID为混音器的id
        rc = mixerOpen(&hMixer, deviceID, 0, 0, MIXER_OBJECTF_MIXER);

        if (MMSYSERR_NOERROR != rc)
        {
            break;
        }

        ZeroMemory(&mxl, sizeof(MIXERLINE));

        mxl.cbStruct = sizeof(MIXERLINE);
        /* 指出需要获取的通道，
        声卡的音频输出用MIXERLINE_COMPONENTTYPE_DST_SPEAKERS
        声卡的音频输入用MIXERLINE_COMPONENTTYPE_DST_WAVEIN
        */
        mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;
        // 取得混合器设备的指定线路信息
        rc = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE);
        if (rc != MMSYSERR_NOERROR)
        {
            continue;
        }

        // 取得混合器设备的指定线路信息成功的话，则将连接数保存 
        DWORD   dwConnections = mxl.cConnections;
        DWORD   dwLineID = -1;
        for (DWORD i = 0; i<dwConnections;)
        {
            mxl.dwSource = i;

            // 根据SourceID获得连接的信息 
            rc = mixerGetLineInfo((HMIXEROBJ)hMixer, &mxl, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_SOURCE);

            if (MMSYSERR_NOERROR == rc)
            {
                // ①如果当前设备类型是麦克风，则跳出循环。 
                if (mxl.dwComponentType == MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE)
                {
                    // 存储麦克风设备的ID
                    dwLineID = mxl.dwLineID;
                    break;
                }
            }
        }
        if (dwLineID == -1)
        {
            continue;
        }

        // MIXERCONTROL 结构体变量清零
        ZeroMemory(&mxc, sizeof(MIXERCONTROL));

        mxc.cbStruct = sizeof(mxc);

        ZeroMemory(&mxlc, sizeof(MIXERLINECONTROLS));

        // 控制类型为控制音量
        mxlc.cbStruct = sizeof(mxlc);
        mxlc.dwLineID = dwLineID;
        mxlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
        mxlc.cControls = 1;
        mxlc.pamxctrl = &mxc;
        mxlc.cbmxctrl = sizeof(mxc);

        // ②取得控制器信息
        rc = mixerGetLineControls((HMIXEROBJ)hMixer, &mxlc, MIXER_GETLINECONTROLSF_ONEBYTYPE);

        if (MMSYSERR_NOERROR == rc)
        {
            MIXERCONTROLDETAILS        mxcd;
            MIXERCONTROLDETAILS_SIGNED volStruct;

            ZeroMemory(&mxcd, sizeof(mxcd));

            mxcd.cbStruct = sizeof(mxcd);
            mxcd.dwControlID = mxc.dwControlID;
            mxcd.paDetails = &volStruct;
            mxcd.cbDetails = sizeof(volStruct);
            mxcd.cChannels = 1;

            //③获得音量值，取得的信息放在mxcd中
            rc = mixerGetControlDetails((HMIXEROBJ)hMixer, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE);

            //初始化录音大小的信息   
            MIXERCONTROLDETAILS_UNSIGNED mxcdVolume_Set = { mxc.Bounds.dwMaximum * dwVolume / 100 };
            MIXERCONTROLDETAILS mxcd_Set;
            mxcd_Set.cbStruct = sizeof(MIXERCONTROLDETAILS);
            mxcd_Set.dwControlID = mxc.dwControlID;
            mxcd_Set.cChannels = 1;
            mxcd_Set.cMultipleItems = 0;
            mxcd_Set.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
            mxcd_Set.paDetails = &mxcdVolume_Set;
            //③设置录音大小
            mixerSetControlDetails((HMIXEROBJ)(hMixer),&mxcd_Set,MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE) 
            mixerClose(hMixer);
        }
    }

    return -1;
}


// 初始化设备并且开始录音
void WaveRecordStart(void) {
    //额外信息的大小，以字节为单位，额外信息添加在WAVEFORMATEX结构的结尾。这个信息可以作为非PCM格式的wFormatTag额外属性，如果wFormatTag不需要额外的信息，此值必需为0，对于PCM格式此值被忽略。
    WaveFormat.cbSize = 0;
    //声道，2代表立体声，1代表单声道
    WaveFormat.nChannels = 2;
    //采样频率
    WaveFormat.nSamplesPerSec = 44100; 
    //对齐方式
    WaveFormat.nBlockAlign = 4;
    //请求的平均数据传输率，单位byte/s
    WaveFormat.nAvgBytesPerSec = 88200;
    //每次采样样本的大小，以bit为单位
    WaveFormat.wBitsPerSample = 16;
    WaveFormat.wFormatTag = 1;
    MMRESULT result;
    result = waveInGetNumDevs();//获取设备数量
    if (result == 0)
    {
        printf("No Sound Device\n");
        return ;
    }

    //获取指定波形音频设备的功能，第一个参数为设备id，第二个参数保存设备功能信息，第三个参数为设备结构体大小
    result = waveInGetDevCaps(0, &m_WaveInDevCaps, sizeof(WAVEINCAPS));
    if (result != MMSYSERR_NOERROR)
    {
        printf("Cannot determine sound card capabilities !\n");
    }
    //打开录音设备，WaveProc为录音的回调函数
    result = waveInOpen(&WaveIn, WAVE_MAPPER, &WaveFormat, (DWORD_PTR)&WaveProc, 0, CALLBACK_FUNCTION);
    if (result != MMSYSERR_NOERROR)
    {
        printf("Opne_Mic_Error\n");
    }

    //lpData：波形数据的缓冲区地址
    //dwBufferLength：波形数据的缓冲区地址的长度
    //dwBytesRecorded:当设备用于录音时，标志已经录入的数据长度
    //dwUser:用户数据
    //dwFlags:波形数据的缓冲区的属性
    //dwLoops:播放循环的次数，仅用于播放控制中
    //lpNext和reserved均为保留值
    //因为是左右两声道，所以要设置两次
    WaveHdr[0].lpData = (LPSTR)Data1;
    WaveHdr[0].dwBufferLength = MAX_SAMPLES *2;
    WaveHdr[1].lpData = (LPSTR)Data2;
    WaveHdr[1].dwBufferLength = MAX_SAMPLES * 2;
    WaveHdr[0].dwBytesRecorded = WaveHdr[1].dwBytesRecorded = 0;
    WaveHdr[0].dwUser = WaveHdr[1].dwUser = 0;
    WaveHdr[0].dwFlags = WaveHdr[1].dwFlags = 0;
    WaveHdr[0].dwLoops = WaveHdr[1].dwLoops = 0;
    WaveHdr[0].lpNext = WaveHdr[1].lpNext = 0;
    WaveHdr[0].reserved = WaveHdr[1].reserved = 0;

    waveInPrepareHeader(WaveIn, WaveHdr, sizeof(WAVEHDR));
    waveInAddBuffer(WaveIn, WaveHdr, sizeof(WAVEHDR));
    waveInPrepareHeader(WaveIn, WaveHdr + 1, sizeof(WAVEHDR));
    waveInAddBuffer(WaveIn, WaveHdr + 1, sizeof(WAVEHDR));
     //开始获取声音 
    result=waveInStart(WaveIn);
    if (result != MMSYSERR_NOERROR)
    {
        printf("waveInStart_Fail\n");
    }
}


 // 回调函数
void CALLBACK WaveProc(HWAVEIN hwi, UINT uMsg, DWORD dwInstance, DWORD dwParam1, DWORD) {
    if ((uMsg != WIM_DATA)) {
        return;
    }

    //取得录音的数据
    MMRESULT result;
    WAVEHDR *pWaveHeader = (WAVEHDR  *)dwParam1;

    //pWaveHeader->lpData为录音数据
    OutputWaveData((short *)pWaveHeader->lpData, pWaveHeader->dwBytesRecorded/2);

    result = waveInAddBuffer(hwi,pWaveHeader, sizeof(WAVEHDR));  
    if (result != MMSYSERR_NOERROR)
    {
        printf("Cannot Add Buffer !");//WAVERR_UNPREPARED  
    }
}

// 导出数据
void OutputWaveData(short *data, DWORD dataLen)
{
    WAVEMEASUMERESULT tmpResult = { 0 };
    double dbVal = data[0]; //第一个点  
    tmpResult.dwPtSize = dataLen; //点数  
    tmpResult.fWave_Frame_Avg = dbVal; //平均值  
    tmpResult.fWave_Frame_RMS = pow(dbVal, 2); //有效值  
    tmpResult.fWave_Frame_Max = dbVal; //最大值  
    tmpResult.fWave_Frame_Min = dbVal; //最小值  
    tmpResult.fWave_Frame_AbsMax = fabs(dbVal); //绝对最大值  
    tmpResult.fWave_Frame_AbsMin = fabs(dbVal); //绝对最小值  

    for (DWORD i = 1; i<dataLen; i++)
    {
        dbVal = data[i];//单声道  
        tmpResult.fWave_Frame_Avg += dbVal; //平均值  
        tmpResult.fWave_Frame_RMS += pow(dbVal, 2); //有效值  
        tmpResult.fWave_Frame_Max = max(dbVal, tmpResult.fWave_Frame_Max); //最大值  
        tmpResult.fWave_Frame_Min = min(dbVal, tmpResult.fWave_Frame_Min); //最小值  
        tmpResult.fWave_Frame_AbsMax = max(fabs(dbVal), tmpResult.fWave_Frame_AbsMax); //绝对最大值  
        tmpResult.fWave_Frame_AbsMin = min(fabs(dbVal), tmpResult.fWave_Frame_AbsMin); //绝对最小值  
    }
    tmpResult.fWave_Frame_Avg /= dataLen; //平均值  
    tmpResult.fWave_Frame_RMS = sqrt(tmpResult.fWave_Frame_RMS /dataLen);//有效值  

    printf("VoiceInputLevel:%f", tmpResult.fWave_Frame_Max);

}