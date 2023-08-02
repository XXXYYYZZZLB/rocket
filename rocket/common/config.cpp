#include <tinyxml/tinyxml.h>
#include <string>
#include "rocket/common/config.h"

#define READ_XML_NODE(name, parent)                                              \
    TiXmlElement *name##_node = parent->FirstChildElement(#name);                \
    if (!(name##_node))                                                          \
    {                                                                            \
        printf("Start rocket server error, failed to read [%s] node.\n", #name); \
        exit(0);                                                                 \
    }

#define READ_STR_FROM_XML_NODE(name, parent)                                              \
    TiXmlElement *name##_node = parent->FirstChildElement(#name);                         \
    if (!log_level_node || !log_level_node->GetText())                                    \
    {                                                                                     \
        printf("Start rocket server error, failed to read str from [%s] node.\n", #name); \
        exit(0);                                                                          \
    }                                                                                     \
    std::string name##_str = std::string(name##_node->GetText());

namespace rocket
{

    static Config *g_config = NULL;

    Config::Config(const char *xmlfile)
    {
        TiXmlDocument *xml_document = new TiXmlDocument();
        bool rt = xml_document->LoadFile(xmlfile);
        if (!rt)
        {
            printf("Start rocket server error, failed to read config: %s.\n", xmlfile);
            printf("error info: %s.\n", xml_document->ErrorDesc());
            exit(0);
        }
        // 遍历结点
        READ_XML_NODE(root, xml_document);
        READ_XML_NODE(log, root_node);
        READ_STR_FROM_XML_NODE(log_level, log_node);
        m_log_level = log_level_str;

        // TODO
    }

    Config *Config::GetGlobalConfig()
    {
        return g_config;
    }

    void Config::SetGlobalConfig(const char *xmlfile)
    {
        if (g_config == NULL)
        {
            g_config = new Config(xmlfile);
        }
    }
}