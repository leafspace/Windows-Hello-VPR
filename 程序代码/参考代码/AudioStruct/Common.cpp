#include "../stdafx.h"
#include "Common.h"

short DataType::sizeFlag = i8Bit;

DataType::DataType()
{
	this->data8B = NULL;
	this->data16B = NULL;
	this->data32B = NULL;
	switch (DataType::sizeFlag)
	{
	case i8Bit: this->data8B = new int8_t(); break;
	case i16Bit: this->data16B = new int16_t(); break;
	case i32Bit: this->data32B = new int32_t(); break;
	default: break;
	}
}

DataType::DataType(const int data)
{
	//DataType();
	this->data8B = NULL;
	this->data16B = NULL;
	this->data32B = NULL;
	switch (DataType::sizeFlag)
	{
	case i8Bit: this->data8B = new int8_t(); break;
	case i16Bit: this->data16B = new int16_t(); break;
	case i32Bit: this->data32B = new int32_t(); break;
	default: break;
	}

	switch (DataType::sizeFlag)
	{
	case i8Bit: (*this->data8B) = data; break;
	case i16Bit: (*this->data16B) = data; break;
	case i32Bit: (*this->data32B) = data; break;
	default: break;
	}
}

DataType::~DataType()
{
	DataType::clearBuffer(this->data8B);
	DataType::clearBuffer(this->data16B);
	DataType::clearBuffer(this->data32B);
}

inline void DataType::clearBuffer(void* pointer)                            // 提供与本类清理数据用
{
	if (pointer != NULL) {
		delete pointer;
		pointer = NULL;
	}
}

DataType::operator int() const                                              // 用于返回本类数据 例如：int result = (int) dataType;
{
	switch (DataType::sizeFlag)
	{
	case i8Bit: return *(this->data8B); break;
	case i16Bit: return *(this->data16B); break;
	case i32Bit: return *(this->data32B); break;
	default: return 0;
	}
}

int DataType::operator=(const int &dataType) const                          // 用于支持赋值作用 例如：DataType dataType = (int) result;
{
	switch (DataType::sizeFlag)
	{
	case i8Bit: *this->data8B = dataType; break;
	case i16Bit: *this->data16B = dataType; break;
	case i32Bit: *this->data32B = dataType; break;
	default: return 0;
	}
	return dataType;
}

void DataType::setValue(const int value)
{
	switch (DataType::sizeFlag)
	{
	case i8Bit: *this->data8B = value; break;
	case i16Bit: *this->data16B = value; break;
	case i32Bit: *this->data32B = value; break;
	}
}

void DataType::setSizeFlag(const short sizeFlag)                            // 用于设置本类的数据类型
{
	DataType::sizeFlag = sizeFlag;
}