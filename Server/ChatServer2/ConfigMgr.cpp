#include "ConfigMgr.h"

ConfigMgr::ConfigMgr()
{
	boost::filesystem::path current_path = boost::filesystem::current_path(); 
	boost::filesystem::path config_path = current_path / "config.ini";//注意此处除号为boost重载的除号，用于文件路径拼接
	std::cout << "Config path : " << config_path << std::endl;

	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path.string(), pt);

	for (const auto & section_pair : pt)
	{
		const ::std::string& section_name = section_pair.first;//键
		const boost::property_tree::ptree& setction_tree = section_pair.second;//值

		std::map<std::string, std::string> section_config;//存这些个键值对
		for (const auto& key_value_pair : setction_tree)
		{
			const std::string& key = key_value_pair.first;
			const std::string& value = key_value_pair.second.get_value<std::string>();
			section_config[key] = value;
		}

		SectionInfo sectionInfo;
		sectionInfo._section_datas = section_config;
		_config_map[section_name] = sectionInfo;
	}

	// 输出所有的section和key-value对  
	for (const auto& section_entry : _config_map) {
		const std::string& section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;
		std::cout << "[" << section_name << "]" << std::endl;
		for (const auto& key_value_pair : section_config._section_datas) {
			std::cout << key_value_pair.first << "=" << key_value_pair.second << std::endl;
		}
	}
}

//拷贝构造函数
ConfigMgr::ConfigMgr(const ConfigMgr& src)
{
	_config_map = src._config_map;
}

ConfigMgr& ConfigMgr::operator=(const ConfigMgr& src)  
{  
   if (&src == this)  
   {  
       return *this;  
   }  
   _config_map = src._config_map;  
   return *this; // Ensure a return statement is present for all control paths  
}

//析构函数
ConfigMgr::~ConfigMgr()
{
	_config_map.clear();
}

//获取单例实例
ConfigMgr& ConfigMgr::Inst()
{
	static ConfigMgr cfg_mgr;
	return cfg_mgr;
}

//重载方括号运算符
SectionInfo ConfigMgr::operator[](const std::string& section)
{
	if (_config_map.find(section) == _config_map.end())//没找到
	{
		return SectionInfo();//返回空
	}
	return _config_map[section];
}

//获取值
std::string ConfigMgr::GetValue(const std::string& section, const std::string& key)
{
	//return std::string();

	return _config_map[section][key];
}