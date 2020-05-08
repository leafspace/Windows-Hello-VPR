#pragma once

struct VoiceParagraph
{
	unsigned int begin;                                                               // 语音段落开始点
	unsigned int end;                                                                 // 语音段落结束点
	unsigned int voiceLength;                                                         // 语音段落长度
	VoiceParagraph(unsigned int be, unsigned int en, unsigned int vo) {               // 构造初始化
		begin = be;
		end = en;
		voiceLength = vo;
	}
};