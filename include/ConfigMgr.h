#ifndef CONFIGMGR_H
#define CONFIGMGR_H

#include "Global.h"
#include <boost/filesystem.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
struct SectionInfo
{
    SectionInfo();
    ~SectionInfo();
    SectionInfo(const SectionInfo &src);
    SectionInfo &operator=(const SectionInfo &src);
    std::string operator[](const std::string &key);
    std::map<std::string, std::string> _section_datas;
};

class ConfigMgr
{
public:
    ConfigMgr();
    ~ConfigMgr();
    SectionInfo operator[](const std::string &section);

    ConfigMgr &operator=(const ConfigMgr &src);

    ConfigMgr(const ConfigMgr &src);

private:
    // 存储section和key-value对的map
    std::map<std::string, SectionInfo> _config_map;
};

extern ConfigMgr gConfigMgr;

#endif