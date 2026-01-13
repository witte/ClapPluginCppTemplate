#include "Plugin.h"
#include <clap/helpers/host-proxy.hxx>
#include <clap/helpers/plugin.hxx>
#include "Utils.h"


Plugin::Plugin(const clap_host* host)
    : clap::helpers::Plugin<clap::helpers::MisbehaviourHandler::Terminate, clap::helpers::CheckingLevel::Maximal>(
        &descriptor, host)
{
}

bool Plugin::audioPortsInfo(const uint32_t index, bool /*isInput*/, clap_audio_port_info* info) const noexcept
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

bool Plugin::paramsInfo(const uint32_t paramIndex, clap_param_info* info) const noexcept
{
    if (paramIndex >= 1)
        return false;

    info->id = gainPrmId_;
    info->flags = CLAP_PARAM_IS_AUTOMATABLE;
    strncpy(info->name, "Gain", CLAP_NAME_SIZE);
    strncpy(info->module, "", CLAP_NAME_SIZE);
    info->min_value = 0.0;
    info->max_value = 1.0;
    info->default_value = utils::decibelsToGain(0.0);

    return true;
}

bool Plugin::paramsValue(const clap_id paramId, double* value) noexcept
{
    if (paramId != gainPrmId_)
        return false;

    *value = utils::toLinearCurve(gain_);
    return true;
}

bool Plugin::paramsValueToText(const clap_id paramId, const double value, char* const display, const uint32_t size) noexcept
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

bool Plugin::paramsTextToValue(const clap_id paramId, const char* display, double* value) noexcept
{
    if (paramId != gainPrmId_)
        return false;

    const double value_ = strtod(display, nullptr);
    *value = utils::toLinearCurve(utils::decibelsToGain(value_));

    return true;
}

bool Plugin::activate(const double sampleRate, uint32_t /*minFrameCount*/, uint32_t /*maxFrameCount*/) noexcept
{
    const auto samplesInOneMs = sampleRate / 1000.0;
    static constexpr auto fadeLengthInMs = 5;
    fadeLengthInSamples_ = static_cast<int>(samplesInOneMs) * fadeLengthInMs;

    return true;
}

clap_process_status Plugin::process(const clap_process* process) noexcept
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
