// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef msr_airlib_vehicles_SimpleFlightQuadX_hpp
#define msr_airlib_vehicles_SimpleFlightQuadX_hpp

#include "vehicles/multirotor/firmwares/simple_flight/SimpleFlightApi.hpp"
#include "vehicles/multirotor/MultiRotorParams.hpp"
#include "common/AirSimSettings.hpp"
#include "sensors/SensorFactory.hpp"

namespace msr
{
namespace airlib
{

    class SimpleFlightQuadXParams : public MultiRotorParams
    {
    public:
        SimpleFlightQuadXParams(const AirSimSettings::VehicleSetting* vehicle_setting, std::shared_ptr<const SensorFactory> sensor_factory)
            : vehicle_setting_(vehicle_setting), sensor_factory_(sensor_factory)
        {
        }

        virtual ~SimpleFlightQuadXParams() = default;

        virtual std::unique_ptr<MultirotorApiBase> createMultirotorApi() override
        {
            return std::unique_ptr<MultirotorApiBase>(new SimpleFlightApi(this, vehicle_setting_));
        }

    protected:
        virtual void setupParams() override
        {
            auto& params = getParams();

            Utils::log(Utils::stringf("Vehicle model %s", vehicle_setting_->model.c_str()), Utils::kLogLevelInfo);
            // Use vehicle_setting_->model for the model name, see Px4MultiRotorParams for example
            if (vehicle_setting_->model == "HEIFU") {
                setupFrameHEIFU(params);
            }
            else if (vehicle_setting_->model == "Hexacopter") {
                setupFrameGenericHex(params);
            }
            else if (vehicle_setting_->model == "Octocopter") {
                setupFrameGenericOcto(params);
            }
            else {
                Utils::log(Utils::stringf("Vehicle model not recognized! GenericQuad is used."), Utils::kLogLevelWarn);
                // Only Generic for now
                setupFrameGenericQuad(params);
            }
        }

        virtual const SensorFactory* getSensorFactory() const override
        {
            return sensor_factory_.get();
        }

    private:
        const AirSimSettings::VehicleSetting* vehicle_setting_; //store as pointer because of derived classes
        std::shared_ptr<const SensorFactory> sensor_factory_;
    };
}
} //namespace
#endif
