//
// Created by igor on 24.06.19.
//

#ifndef UCAL_COMMON_CONFIG_H
#define UCAL_COMMON_CONFIG_H

#include <string>
#include "keys.h"
#include <fstream>
#include <memory>

namespace common {

    static const char FILENAME[] = "override_config.json";

    struct ConfigData;
    using ConfigDataPtr = std::unique_ptr<ConfigData>;

    class Config {
    public:

        Config();

        explicit Config(std::string const& filename);

        ~Config();

        bool write(ConfigStringKey key, const std::string& value);

        bool write(ConfigDoubleKey key, double value);

        std::string readStr(ConfigStringKey key) const;

        double readDouble(ConfigDoubleKey key) const;

        bool reset();

    private:
        ConfigDataPtr data;
        std::string override_filename;
    };

    using ConfigPtr = std::shared_ptr<Config>;

    ConfigPtr acquireConfig();
    ConfigPtr acquireConfig(std::string const& filename);

}

#endif //UCAL_COMMON_CONFIG_H
