#pragma once

#include <vector>
#include <algorithm>
#include "Params.hpp"
#include "interfaces/CommonStructs.hpp"
#include <stdexcept>
#include "common/common_utils/Utils.hpp"

namespace simple_flight
{

class Mixer
{
public:
    Mixer(const Params* params)
        : params_(params)
    {
    }

    void getMotorOutput(const Axis4r& controls, std::vector<float>& motor_outputs) const
    {
        if (motor_outputs.size() == 4) {
            getMotorOutputGeneric(controls, motor_outputs, mixerQuadX);
        } else if (motor_outputs.size() == 6) {
            getMotorOutputGeneric(controls, motor_outputs, mixerHex6X);
        } else {
            throw std::invalid_argument("Cannot assign mixer to motor number.");
        }   
    }

private:
    const Params* params_;

    // Custom mixer data per motor
    typedef struct motorMixer_t
    {
        float throttle;
        float roll;
        float pitch;
        float yaw;
    } motorMixer_t;

    //only thing that this matrix does is change the sign
    const motorMixer_t mixerQuadX[4] = {
        //QuadX config
        { 1.0f, -1.0f, 1.0f, 1.0f }, // FRONT_R
        { 1.0f, 1.0f, -1.0f, 1.0f }, // REAR_L
        { 1.0f, 1.0f, 1.0f, -1.0f }, // FRONT_L
        { 1.0f, -1.0f, -1.0f, -1.0f }, // REAR_R
    };

    const motorMixer_t mixerHex6X[6] = {
        { 1.0f, -1.0f,  0.0f,      -1.0f },     // RIGHT
        { 1.0f,  1.0f,  0.0f,       1.0f },     // LEFT
        { 1.0f,  0.5f, -0.866025f, -1.0f },     // FRONT_L
        { 1.0f, -0.5f,  0.866025f,  1.0f },     // REAR_R
        { 1.0f, -0.5f, -0.866025f,  1.0f },     // FRONT_R
        { 1.0f,  0.5f,  0.866025f, -1.0f },     // REAR_L
    };

    void getMotorOutputGeneric(const Axis4r& controls, std::vector<float>& motor_outputs, const motorMixer_t* mixer_) const
    {
        if (controls.throttle() < params_->motor.min_angling_throttle) {
            motor_outputs.assign(motor_outputs.size(), controls.throttle());
            return;
        }

        for (int motor_index = 0; motor_index < motor_outputs.size(); ++motor_index) {
            motor_outputs[motor_index] =
                controls.throttle() * mixer_[motor_index].throttle 
                + controls.pitch() * mixer_[motor_index].pitch 
                + controls.roll() * mixer_[motor_index].roll 
                + controls.yaw() * mixer_[motor_index].yaw;
        }

        float min_motor = *std::min_element(motor_outputs.begin(), motor_outputs.begin() + motor_outputs.size());
        if (min_motor < params_->motor.min_motor_output) {
            float undershoot = params_->motor.min_motor_output - min_motor;
            for (int motor_index = 0; motor_index < motor_outputs.size(); ++motor_index)
                motor_outputs[motor_index] += undershoot;
        }

        float max_motor = *std::max_element(motor_outputs.begin(), motor_outputs.begin() + motor_outputs.size());
        float scale = max_motor / params_->motor.max_motor_output;
        if (scale > params_->motor.max_motor_output) {
            for (int motor_index = 0; motor_index < motor_outputs.size(); ++motor_index)
                motor_outputs[motor_index] /= scale;
        }

        for (int motor_index = 0; motor_index < motor_outputs.size(); ++motor_index){
            motor_outputs[motor_index] = std::max(params_->motor.min_motor_output,
                                                  std::min(motor_outputs[motor_index], params_->motor.max_motor_output));
            common_utils::Utils::log(common_utils::Utils::stringf("Motor Actual CMD %d: %f", motor_index, motor_outputs[motor_index]));
        }
    }
};

} //namespace