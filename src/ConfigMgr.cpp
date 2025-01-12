#include "ConfigMgr.h"

SectionInfo::SectionInfo() {}
SectionInfo::~SectionInfo() { _section_datas.clear(); }

SectionInfo::SectionInfo(const SectionInfo &src) {
	_section_datas = src._section_datas;
}

SectionInfo &SectionInfo::operator=(const SectionInfo &src) {
	if (&src == this) {
		return *this;
	}

	this->_section_datas = src._section_datas;
	return *this;
}

std::string SectionInfo::operator[](const std::string &key) {
	if (_section_datas.find(key) == _section_datas.end()) {
		return "";
	}
	// 这里可以添加一些边界检查
	return _section_datas[key];
}

ConfigMgr gConfigMgr;

ConfigMgr::ConfigMgr() {

	// 获取当前工作目录
	// boost::filesystem::path current_path = boost::filesystem::current_path();
	// // 构建config.ini文件的完整路径
	// boost::filesystem::path config_path = current_path / "config.ini";
	std::string config_path = PROJECT_DIR + std::string("/config.ini");
	// logger->info("配置文件路径为：{}", config_path.c_str());

	// 使用Boost.PropertyTree来读取INI文件
	boost::property_tree::ptree pt;
	boost::property_tree::read_ini(config_path, pt);

	// 遍历INI文件中的所有section
	for (const auto &section_pair : pt) {
		const std::string &section_name = section_pair.first;
		const boost::property_tree::ptree &section_tree = section_pair.second;

		// 对于每个section，遍历其所有的key-value对
		std::map<std::string, std::string> section_config;
		for (const auto &key_value_pair : section_tree) {
			const std::string &key = key_value_pair.first;
			const std::string &value =
				key_value_pair.second.get_value<std::string>();
			section_config[key] = value;
		}
		SectionInfo sectionInfo;
		sectionInfo._section_datas = section_config;
		// 将section的key-value对保存到config_map中
		_config_map[section_name] = sectionInfo;
	}

	// 输出所有的section和key-value对
	for (const auto &section_entry : _config_map) {
		const std::string &section_name = section_entry.first;
		SectionInfo section_config = section_entry.second;
	}
}

ConfigMgr::~ConfigMgr() { _config_map.clear(); }
SectionInfo ConfigMgr::operator[](const std::string &section) {
	if (_config_map.find(section) == _config_map.end()) {
		return SectionInfo();
	}
	return _config_map[section];
}

ConfigMgr &ConfigMgr::operator=(const ConfigMgr &src) {
	if (&src == this) {
		return *this;
	}

	this->_config_map = src._config_map;
	return *this;
}

ConfigMgr::ConfigMgr(const ConfigMgr &src) {
	this->_config_map = src._config_map;
}