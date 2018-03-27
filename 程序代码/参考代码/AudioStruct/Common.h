#pragma once
#include <math.h>
#include <vector>
#include <fstream>
#include <stdio.h>
#include <stdint.h>
#include <iostream>
using namespace std;

enum Type
{
	i8Bit = 1,                                                              // 8bit数据
	i16Bit,                                                                 // 16bit数据
	i24Bit,                                                                 // 不存在
	i32Bit,                                                                 // 32bit数据
};

class DataType
{
private:
	static short sizeFlag;                                                  // 标志：说明本类中的数据是什么类型的
	int8_t *data8B;                                                         // 8bit数据
	int16_t *data16B;                                                       // 16bit数据
	int32_t *data32B;                                                       // 32bit数据
	inline static void clearBuffer(void* pointer);                          // 提供与本类清理数据用
public:
	DataType();
	DataType(const int data);
	~DataType();
	operator int() const;                                                   // 用于返回本类数据 例如：int result = (int) dataType;
	int operator=(const int &dataType) const;                               // 用于支持赋值作用 例如：DataType dataType = (int) result;
	void setValue(const int value);
	static void setSizeFlag(const short sizeFlag);                          // 用于设置本类的数据类型
};
