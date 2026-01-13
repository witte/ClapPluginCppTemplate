module;
// #include "Plugin.h"
#include <clap/helpers/host-proxy.hxx>
// #include <clap/helpers/plugin.hxx>
// #include "Utils.h"
#include <clap/version.h>
#include <clap/plugin-features.h>

export module Plugin;
import Utils;
import Clap;


export class GainPlugin : public Plugin<clap::helpers::MisbehaviourHandler::Terminate,
                                            clap::helpers::CheckingLevel::Maximal>
{
public:
    static constexpr const char* features[4] =
    {
        CLAP_PLUGIN_FEATURE_AUDIO_EFFECT,
        CLAP_PLUGIN_FEATURE_UTILITY,
        CLAP_PLUGIN_FEATURE_STEREO,
        nullptr
    };

    static constexpr clap_plugin_descriptor descriptor =
    {
        .clap_version = CLAP_VERSION,
        .id = "your.reversed.domain.name.PluginName",
        .name = "Name of the plugin",
        .vendor = "You/your company name",
        .url = "https://your.website",
        .manual_url = "",
        .support_url = "",
        .version = "1.0.0",
        .description = "A short description of your plugin",
        .features = features
    };


    explicit GainPlugin(const clap_host* host);
    ~GainPlugin() override = default;


    [[nodiscard]] bool implementsAudioPorts() const noexcept override
    {
        return true;
    }

    [[nodiscard]] uint32_t audioPortsCount(bool /*isInput*/) const noexcept override
    {
        return 1;
    }

    bool audioPortsInfo(uint32_t index, bool isInput, clap_audio_port_info* info) const noexcept override;


    [[nodiscard]] bool implementsParams() const noexcept override
    {
        return true;
    }

    [[nodiscard]] bool isValidParamId(clap_id paramId) const noexcept override
    {
        return paramId == gainPrmId_;
    }

    [[nodiscard]] uint32_t paramsCount() const noexcept override
    {
        return 1;
    }

    bool paramsInfo(uint32_t paramIndex, clap_param_info* info) const noexcept override;

    bool paramsValue(clap_id paramId, double* value) noexcept override;

    bool paramsValueToText(clap_id paramId, double value, char* display, uint32_t size) noexcept override;
    bool paramsTextToValue(clap_id paramId, const char* display, double* value) noexcept override;

    bool activate(double sampleRate, uint32_t, uint32_t) noexcept override;

    clap_process_status process(const clap_process* process) noexcept override;


private:
    static constexpr clap_id gainPrmId_ = 2345;

    double gain_ = 0.0;
    double targetGain_ = 0.0;
    double stepSizeToTargetGain_ = 0.0;
    int fadeLengthInSamples_ = 0;
    int currentFadeIndex_ = fadeLengthInSamples_;
};


GainPlugin::GainPlugin(const clap_host* host)
    : Plugin<clap::helpers::MisbehaviourHandler::Terminate, clap::helpers::CheckingLevel::Maximal>(
        &descriptor, host)
{
}

bool GainPlugin::audioPortsInfo(const uint32_t index, bool /*isInput*/, clap_audio_port_info* info) const noexcept
{
    if (index != 0)
        return false;

    info->id = 0;
    info->in_place_pair = CLAP_INVALID_ID;
    strncpy(info->name, "main", sizeof(info->name));
    info->flags = CLAP_AUDIO_PORT_IS_MAIN;
    info->channel_count = 2;
    info->port_type = CLAP_PORT_STEREO;

    return true;
}

bool GainPlugin::paramsInfo(const uint32_t paramIndex, clap_param_info* info) const noexcept
{
    if (paramIndex >= 1)
        return false;

    info->id = gainPrmId_;
    info->flags = CLAP_PARAM_IS_AUTOMATABLE;
    strncpy(info->name, "GainHEHE", CLAP_NAME_SIZE);
    strncpy(info->module, "", CLAP_NAME_SIZE);
    info->min_value = 0.0;
    info->max_value = 1.0;
    info->default_value = utils::decibelsToGain(0.0);

    return true;
}

bool GainPlugin::paramsValue(const clap_id paramId, double* value) noexcept
{
    if (paramId != gainPrmId_)
        return false;

    *value = utils::toLinearCurve(gain_);
    return true;
}

bool GainPlugin::paramsValueToText(const clap_id paramId, const double value, char* const display, const uint32_t size) noexcept
{
    if (paramId != gainPrmId_)
        return false;

    if (const auto valueIndB = utils::gainToDecibels(utils::toExponentialCurve(value));
        valueIndB <= utils::minusInfinitydB)
    {
        snprintf(display, size, "-inf dB");
    }
    else
    {
        snprintf(display, size, "%.2f dB", valueIndB);
    }

    return true;
}

bool GainPlugin::paramsTextToValue(const clap_id paramId, const char* display, double* value) noexcept
{
    if (paramId != gainPrmId_)
        return false;

    const double value_ = strtod(display, nullptr);
    *value = utils::toLinearCurve(utils::decibelsToGain(value_));

    return true;
}

bool GainPlugin::activate(const double sampleRate, uint32_t /*minFrameCount*/, uint32_t /*maxFrameCount*/) noexcept
{
    const auto samplesInOneMs = sampleRate / 1000.0;
    static constexpr auto fadeLengthInMs = 5;
    fadeLengthInSamples_ = static_cast<int>(samplesInOneMs) * fadeLengthInMs;

    return true;
}

clap_process_status GainPlugin::process(const clap_process* process) noexcept
{
    if (process->audio_outputs_count <= 0)
        return CLAP_PROCESS_CONTINUE;

    float** input = process->audio_inputs[0].data32;
    float** output = process->audio_outputs[0].data32;
    const auto outputChannelsCount = process->audio_outputs->channel_count;

    const auto event = process->in_events;
    const auto eventsSize = event->size(event);

    const clap_event_header_t* nextEvent{nullptr};
    uint32_t nextEventIndex{0};
    if (eventsSize != 0)
    {
        nextEvent = event->get(event, nextEventIndex);
    }

    for (uint32_t index = 0; index < process->frames_count; index++)
    {
        while (nextEvent && nextEvent->time == index)
        {
            const auto gainValue = reinterpret_cast<const clap_event_param_value*>(nextEvent);

            if (nextEvent->space_id != CLAP_CORE_EVENT_SPACE_ID ||
                nextEvent->type != CLAP_EVENT_PARAM_VALUE)
            {
                continue;
            }

            if (gainValue->param_id == gainPrmId_)
            {
                targetGain_ = utils::toExponentialCurve(gainValue->value);
                currentFadeIndex_ = 0;

                stepSizeToTargetGain_ = (targetGain_ - gain_) / fadeLengthInSamples_;
            }

            nextEventIndex++;
            nextEvent = (nextEventIndex >= eventsSize)? nullptr : event->get(event, nextEventIndex);
        }

        for (uint32_t channel = 0; channel < outputChannelsCount; ++channel)
        {
            if (currentFadeIndex_ < fadeLengthInSamples_)
            {
                currentFadeIndex_++;
                gain_ += stepSizeToTargetGain_;
            }
            else if (currentFadeIndex_ == fadeLengthInSamples_)
            {
                gain_ = targetGain_;
            }

            output[channel][index] = input[channel][index] * static_cast<float>(gain_);
        }
    }

    return CLAP_PROCESS_CONTINUE;
}
