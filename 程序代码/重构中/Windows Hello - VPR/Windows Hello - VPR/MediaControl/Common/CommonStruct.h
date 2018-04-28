#pragma once

struct VoiceParagraph
{
	unsigned long begin;                                                    // 语音段落开始点
	unsigned long end;                                                      // 语音段落结束点
	unsigned long voiceLength;                                              // 语音段落长度
	VoiceParagraph(unsigned long be, unsigned long en, unsigned long vo) {  // 构造初始化
		begin = be;
		end = en;
		voiceLength = vo;
	}
};