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
export using ::strncpy;
export using ::snprintf;
export using ::strtod;
export using ::clap_plugin_entry;
export using ::clap_plugin_descriptor;
export using ::clap_plugin;
export using ::clap_plugin_factory;
export using ::clap_host;
export using ::clap::helpers::Plugin;

export using ::clap_process_status;
export using ::clap_process;
export using ::clap_id;


export inline CLAP_CONSTEXPR auto mCLAP_PLUGIN_FEATURE_AUDIO_EFFECT = CLAP_PLUGIN_FEATURE_AUDIO_EFFECT;
export inline CLAP_CONSTEXPR auto mCLAP_PLUGIN_FEATURE_UTILITY = CLAP_PLUGIN_FEATURE_UTILITY;
export inline CLAP_CONSTEXPR auto mCLAP_PLUGIN_FEATURE_STEREO = CLAP_PLUGIN_FEATURE_STEREO;





export using ::clap_version;

export inline CLAP_CONSTEXPR clap_version_t mCLAP_VERSION = CLAP_VERSION_INIT;

// export using ::mCLAP_VERSION;
export using ::clap_audio_port_info;
export using ::clap_param_info;

export inline CLAP_CONSTEXPR clap_id mCLAP_INVALID_ID = UINT32_MAX;
// export using ::CLAP_INVALID_ID;

export inline CLAP_CONSTEXPR uint16_t mCLAP_CORE_EVENT_SPACE_ID = 0;
// export using ::CLAP_CORE_EVENT_SPACE_ID;
export using ::CLAP_EVENT_PARAM_VALUE;

export using ::CLAP_AUDIO_PORT_IS_MAIN;

export inline CLAP_CONSTEXPR const char mCLAP_PORT_STEREO[] = "stereo";
// export using ::CLAP_PORT_STEREO;

export using ::CLAP_PARAM_IS_AUTOMATABLE;
export using ::CLAP_NAME_SIZE;


export using ::CLAP_PROCESS_CONTINUE;

export using ::clap_event_header_t;
export using ::clap_event_param_value;


namespace clap::helpers {

export using ::clap::helpers::MisbehaviourHandler;
export using ::clap::helpers::CheckingLevel;

}

