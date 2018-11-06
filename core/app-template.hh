/*
 * This file is open source software, licensed to you under the terms
 * of the Apache License, Version 2.0 (the "License").  See the NOTICE file
 * distributed with this work for additional information regarding copyright
 * ownership.  You may not use this file except in compliance with the License.
 *
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */
/*
 * Copyright (C) 2014 Cloudius Systems, Ltd.
 */
#pragma once

#include <boost/program_options.hpp>
#include <boost/optional.hpp>
#include <functional>
#include <core/future.hh>
#include <core/sstring.hh>
#include <chrono>

namespace seastar {

class app_template {
public:
    struct config {
        sstring name = "App";
        sstring version = "commit-hash";
        std::chrono::duration<double> default_task_quota = std::chrono::microseconds(500);
        config() {}
    };

    using configuration_reader = std::function<void (boost::program_options::variables_map&)>;
private:
    config _cfg;
    static app_template _current_app;
    boost::program_options::options_description _opts;
    boost::program_options::options_description _opts_conf_file;
    boost::program_options::positional_options_description _pos_opts;
    boost::optional<boost::program_options::variables_map> _configuration;
    configuration_reader _conf_reader;

    configuration_reader get_default_configuration_reader();
public:
    struct positional_option {
        const char* name;
        const boost::program_options::value_semantic* value_semantic;
        const char* help;
        int max_count;
    };
public:
    explicit app_template(config cfg = config());

    boost::program_options::options_description& get_options_description();
    boost::program_options::options_description& get_conf_file_options_description();
    boost::program_options::options_description_easy_init add_options();
    void add_positional_options(std::initializer_list<positional_option> options);
    boost::program_options::variables_map& configuration();
    int run_deprecated(int ac, char ** av, std::function<void ()>&& func);

    void set_configuration_reader(configuration_reader conf_reader);

    // Runs given function and terminates the application when the future it
    // returns resolves. The value with which the future resolves will be
    // returned by this function.
    int run(int ac, char ** av, std::function<future<int> ()>&& func);

    // Like run() which takes std::function<future<int>()>, but returns
    // with exit code 0 when the future returned by func resolves
    // successfully.
    int run(int ac, char ** av, std::function<future<> ()>&& func);

    // Return the current application template object.
    static app_template get_current_app();

    config get_app_config();
};

}
