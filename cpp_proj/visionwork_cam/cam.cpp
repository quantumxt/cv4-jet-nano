/*
# Copyright (c) 2015-2016, NVIDIA CORPORATION. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditionQs and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#  * Neither the name of NVIDIA CORPORATION nor the names of its
#    contributors may be used to endorse or promote products derived
#    from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <iostream>
#include <sstream>
#include <iomanip>
#include <memory>

#include <VX/vx.h>
#include <NVX/nvx_timer.hpp>

#include "OVX/FrameSourceOVX.hpp"
#include "OVX/RenderOVX.hpp"
#include "NVX/Application.hpp"
#include "OVX/UtilityOVX.hpp"

struct EventData
{
    EventData(): alive(true), pause(false) {}

    bool alive;
    bool pause;
};

static void keyboardEventCallback(void* context, vx_char key, vx_uint32 /*x*/, vx_uint32 /*y*/)
{
    EventData* eventData = static_cast<EventData*>(context);
    if (key == 27) // escape
    {
        eventData->alive = false;
    }
    else if (key == 32)
    {
        eventData->pause = !eventData->pause;
    }
}

static void parseResolution(const std::string & resolution, ovxio::FrameSource::Parameters & config)
{
    std::istringstream stream(resolution);
    std::string item;
    vx_uint32 * frameParams[] = { &config.frameWidth, &config.frameHeight };
    vx_uint32 index = 0;

    while (std::getline(stream, item, 'x'))
    {
        std::stringstream ss(item);
        ss >> *frameParams[index++];
    }
}

//
// main - Application entry point
//

int main(int argc, char** argv)
{
    nvxio::Application &app = nvxio::Application::get();
    ovxio::printVersionInfo();

    ovxio::FrameSource::Parameters config;
    config.frameWidth = 1280;
    config.frameHeight = 720;

    //
    // Parse command line arguments
    //

    std::string resolution = "1280x720", input = "device:///nvcamera";

    app.setDescription("This sample captures frames from NVIDIA GStreamer camera");
    app.addOption('r', "resolution", "Input frame resolution", nvxio::OptionHandler::oneOf(&resolution,
        { "2592x1944", "2592x1458", "1280x720", "640x480" }));

    app.init(argc, argv);

    parseResolution(resolution, config);

    //
    // Create OpenVX context
    //

    ovxio::ContextGuard context;

    //
    // Messages generated by the OpenVX framework will be processed by ovxio::stdoutLogCallback
    //

    vxRegisterLogCallback(context, &ovxio::stdoutLogCallback, vx_false_e);

    //
    // Create a Frame Source
    //

    std::unique_ptr<ovxio::FrameSource> source(ovxio::createDefaultFrameSource(context, input));
    if (!source)
    {
        std::cout << "Error: cannot open source!" << std::endl;
        return nvxio::Application::APP_EXIT_CODE_NO_RESOURCE;
    }

    if (!source->setConfiguration(config))
    {
        std::cout << "Error: cannot setup configuration the framesource!" << std::endl;
        return nvxio::Application::APP_EXIT_CODE_INVALID_VALUE;
    }

    if (!source->open())
    {
        std::cout << "Error: cannot open source!" << std::endl;
        return nvxio::Application::APP_EXIT_CODE_NO_RESOURCE;
    }

    config = source->getConfiguration();

    //
    // Create a Render
    //

    std::unique_ptr<ovxio::Render> render(ovxio::createDefaultRender(
                context, "NVIDIA GStreamer Camera Capture Sample", config.frameWidth, config.frameHeight));
    if (!render)
    {
        std::cout << "Error: Cannot open default render!" << std::endl;
        return nvxio::Application::APP_EXIT_CODE_NO_RENDER;
    }

    EventData eventData;
    render->setOnKeyboardEventCallback(keyboardEventCallback, &eventData);

    vx_image frame = vxCreateImage(context, config.frameWidth,
                                   config.frameHeight, config.format);
    NVXIO_CHECK_REFERENCE(frame);

    ovxio::Render::TextBoxStyle style = {{255,255,255,255}, {0,0,0,127}, {10,10}};

    nvx::Timer totalTimer;
    totalTimer.tic();

    while (eventData.alive)
    {
        ovxio::FrameSource::FrameStatus status = ovxio::FrameSource::OK;
        if (!eventData.pause)
        {
            status = source->fetch(frame);
        }

        switch(status)
        {
        case ovxio::FrameSource::OK:
            {
                double total_ms = totalTimer.toc();
                totalTimer.tic();

                std::ostringstream txt;
                txt << std::fixed << std::setprecision(1);

                txt << "Camera mode: " << config.frameWidth << 'x' << config.frameHeight << ' ' << config.fps << " FPS" << std::endl;
                txt << "Algorithm: " << "No Processing" << std::endl;
                txt << "Display: " << total_ms  << " ms / " << 1000.0 / total_ms << " FPS" << std::endl;

                txt << std::setprecision(6);
                txt.unsetf(std::ios_base::floatfield);

                txt << "FRAME RATE IS NOT CONSTRAINED" << std::endl;

                std::cout << txt.str();

                txt << "Space - pause/resume" << std::endl;
                txt << "Esc - close the demo";

                render->putImage(frame);
                render->putTextViewport(txt.str(), style);

                if (!render->flush())
                    eventData.alive = false;
            }
            break;
        case ovxio::FrameSource::TIMEOUT:
            {
                // Do nothing
            }
            break;
        case ovxio::FrameSource::CLOSED:
            eventData.alive = false;
            break;
        }
    }

    //
    // Release all objects
    //
    vxReleaseImage(&frame);

    return nvxio::Application::APP_EXIT_CODE_SUCCESS;
}