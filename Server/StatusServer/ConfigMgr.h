#pragma once
#include "const.h"

struct SectionInfo
{
	//构造
	SectionInfo() {}
	//拷贝构造
	SectionInfo( const SectionInfo& src) {
		_section_datas = src._section_datas;
	}
	//拷贝赋值
	SectionInfo& operator = (const SectionInfo& src) {
		if (&src == this)
		{
			return *this;
		}
		this->_section_datas = src._section_datas;
		return *this;
	}

	//析构
	~SectionInfo() { 
		_section_datas.clear();
	}

	std::map<std::string, std::string> _section_datas;
	//重载运算符
	std::string operator[](const std::string& key) {
		if (_section_datas.find(key) == _section_datas.end())//没找到
		{
			return "";
		}
		return _section_datas[key];
	}
};

class ConfigMgr
{
public:
	//拷贝构造
	ConfigMgr(const ConfigMgr& src);
	//拷贝赋值
	ConfigMgr& operator = (const ConfigMgr& src);
	//析构
	~ConfigMgr();
	//C++11之后的标准，多线程访问Inst函数时，只会在第一次线程访问时初始化一个局部的静态变量，之后的线程访问是这共有的一份
	static ConfigMgr& Inst();
	//重载方括号运算符
	SectionInfo operator[](const std::string& section);
	//获取值
	std::string GetValue(const std::string& section, const std::string& key);

private:
	//构造
	ConfigMgr();

	// 存储section和key-value对的map
	std::map<std::string, SectionInfo> _config_map;
};
