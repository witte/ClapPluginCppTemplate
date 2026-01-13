module;
#include <cstring>
#include <clap/plugin.h>
#include <clap/entry.h>
#include <clap/host.h>
#include <clap/factory/plugin-factory.h>
#include <clap/helpers/plugin.hh>
#include <clap/helpers/plugin.hxx>
// #include <clap/helpers/host-proxy.hxx>


export module Clap;

export using std::uint32_t;
export using ::strcmp;
export using ::clap_plugin_entry;
export using ::clap_plugin_descriptor;
export using ::clap_plugin;
export using ::clap_plugin_factory;
export using ::clap_host;
export using ::clap::helpers::Plugin;

// namespace clap::helpers {
//
// export using ::clap::helpers::MisbehaviourHandler;
// export using ::clap::helpers::CheckingLevel;
// }

