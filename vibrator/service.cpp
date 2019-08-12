/*
 * Copyright (C) 2019 The LineageOS Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "android.hardware.vibrator@1.1-service.hh"

#include <android/hardware/vibrator/1.1/IVibrator.h>
#include <hidl/HidlSupport.h>
#include <hidl/HidlTransportSupport.h>
#include <utils/Errors.h>
#include <utils/StrongPointer.h>

#include "Vibrator.h"

using android::hardware::configureRpcThreadpool;
using android::hardware::joinRpcThreadpool;
using android::hardware::vibrator::V1_1::IVibrator;
using android::hardware::vibrator::V1_1::implementation::Vibrator;

const static std::string ENABLE_PATH = "/sys/class/timed_output/vibrator/enable";
const static std::string AMPLITUDE_PATH = "/sys/class/timed_output/vibrator/vtg_level";
// const static std::string AMPLITUDE_DEF_PATH = "/sys/class/timed_output/vibrator/vtg_default";
// const static std::string AMPLITUDE_MAX_PATH = "/sys/class/timed_output/vibrator/vtg_max";
// const static std::string AMPLITUDE_MIN_PATH = "/sys/class/timed_output/vibrator/vtg_min";

android::status_t registerVibratorService() {
    std::ofstream enable(ENABLE_PATH);
    if (!enable) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", ENABLE_PATH.c_str(), error, strerror(error));
        return -error;
    }

    std::ofstream amplitude(AMPLITUDE_PATH);
    if (!amplitude) {
        int error = errno;
        ALOGE("Failed to open %s (%d): %s", AMPLITUDE_PATH.c_str(), error, strerror(error));
        return -error;
    }

    android::sp<IVibrator> service = new Vibrator(std::move(enable), std::move(amplitude));
    if (!service) {
        ALOGE("Cannot allocate Vibrator HAL service");
        return 1;
    }

    android::status_t status = service->registerAsService();
    if (status != android::OK) {
        ALOGE("Cannot register Vibrator HAL service");
        return 1;
    }

    return android::OK;
}

int main() {
    configureRpcThreadpool(1, true);

    android::status_t status = registerVibratorService();
    if (status != android::OK) {
        return status;
    }

    joinRpcThreadpool();
}
