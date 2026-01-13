import std;
import Clap;
import Plugin;


namespace
{

bool init(const char* /*plugin_path*/)
{
    return true;
}

void deinit()
{}

uint32_t getPluginCount(const clap_plugin_factory* /*factory*/)
{
    return 1;
}

const clap_plugin_descriptor* getPluginDescriptor(const clap_plugin_factory* /*factory*/, const uint32_t index)
{
    if (index == 0)
        return &GainPlugin::descriptor;

    return nullptr;
}

const clap_plugin* createPluginInstance(const clap_plugin_factory* /*factory*/, const clap_host* host, const char* plugin_id)
{
    if (strcmp(plugin_id, GainPlugin::descriptor.id) != 0)
    {
        std::cerr << "Error: plugin_id '" << plugin_id << "' not found!" << std::endl;
        return nullptr;
    }

    // Host will own 'plugin'
    const auto plugin = new GainPlugin(host);
    return plugin->clapPlugin();
}

constexpr clap_plugin_factory factoryStruct =
{
    .get_plugin_count = getPluginCount,
    .get_plugin_descriptor = getPluginDescriptor,
    .create_plugin = createPluginInstance,
};

const void* getFactory(const char* /*factory_id*/)
{
    return &factoryStruct;
}

}


extern "C"
{

// CLAP_EXPORT
__attribute__((visibility("default")))
const clap_plugin_entry clap_entry =
{
    .clap_version = mCLAP_VERSION,
    .init = init,
    .deinit = deinit,
    .get_factory = getFactory
};

}
