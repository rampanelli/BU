#include <BrewUNO/ActiveStatus.h>

ActiveStatus::ActiveStatus(FS *fs) : _fs(fs)
{
}

ActiveStatus::~ActiveStatus() {}

boolean ActiveStatus::LoadActiveStatusSettings()
{
    File configFile = _fs->open(ACTIVE_STATUS_FILE, "r");
    if (configFile)
    {
        size_t size = configFile.size();
        if (size <= MAX_ACTIVESTATUS_SIZE)
        {
            DynamicJsonDocument _activeStatusJsonDocument = DynamicJsonDocument(MAX_ACTIVESTATUS_SIZE);
            DeserializationError error = deserializeJson(_activeStatusJsonDocument, configFile);
            if (error == DeserializationError::Ok && _activeStatusJsonDocument.is<JsonObject>())
            {
                JsonObject _activeStatus = _activeStatusJsonDocument.as<JsonObject>();
                ActiveStep = _activeStatus["active_step"];
                ActiveMashStepIndex = _activeStatus["active_mash_step_index"];
                ActiveMashStepName = _activeStatus["active_mash_step_name"] | "";
                ActiveMashStepSufixName = _activeStatus["active_mash_step_sufix_name"] | "";
                ActiveBoilStepIndex = _activeStatus["active_boil_step_index"] | "";
                ActiveBoilStepName = _activeStatus["active_boil_step_name"] | "";
                BoilTime = _activeStatus["boil_time"];
                BoilTargetTemperature = _activeStatus["boil_target_temperature"];
                TargetTemperature = _activeStatus["target_temperature"];
                EndTime = _activeStatus["end_time"];
                StartTime = _activeStatus["start_time"];
                TimeNow = _activeStatus["time_now"];
                BrewStarted = _activeStatus["brew_started"];
                PWM = _activeStatus["pwm"];
                SpargePWM = _activeStatus["sparge_pwm"];
                Recirculation = _activeStatus["recirculation"];
                HeaterOn = _activeStatus["heater_on"];
                StepLock = _activeStatus["step_lock"];
                BoilPowerPercentage = _activeStatus["boil_power_percentage"];
                PIDTuning = _activeStatus["pid_tuning"];
                configFile.close();
            }
        }
        configFile.close();
    }

    return true;
}

String ActiveStatus::GetJson()
{
    return "{\"active_step\":" + String(ActiveStep) + "," +
           "\"active_mash_step_index\":" + String(ActiveMashStepIndex) + "," +
           "\"active_mash_step_name\":\"" + String(ActiveMashStepName) + "\"" + "," +
           "\"active_mash_step_sufix_name\":\"" + String(ActiveMashStepSufixName) + "\"" + "," +
           "\"active_boil_step_index\":\"" + String(ActiveBoilStepIndex) + "\"" + "," +
           "\"active_boil_step_name\":\"" + String(ActiveBoilStepName) + "\"" + "," +
           "\"boil_time\":" + String(BoilTime) + "," +
           "\"boil_target_temperature\":" + String(BoilTargetTemperature) + "," +
           "\"target_temperature\":" + String(TargetTemperature) + "," +
           "\"start_time\":" + String(StartTime) + "," +
           "\"end_time\":" + String(EndTime) + "," +
           "\"time_now\":" + String(TimeNow) + "," +
           "\"brew_started\":" + String(BrewStarted) + "," +
           "\"temperature\":" + String(Temperature) + "," +
           "\"sparge_temperature\":" + String(SpargeTemperature) + "," +
           "\"temperatures\":" + Temperatures + "," +
           "\"main_sensor\": \"" + MainSensor + "\"," +
           "\"sparge_sensor\": \"" + SpargeSensor + "\"," +
           "\"pwm\":" + String(PWM, 2) + ',' +
           "\"pwm_percentage\":" + String(PWMPercentage, 2) + ',' +
           "\"sparge_pwm\":" + String(SpargePWM, 2) + ',' +
           "\"sparge_pwm_percentage\":" + String(SpargePWMPercentage, 2) + ',' +
           "\"enable_sparge\":" + String(EnableSparge) + ',' +
           "\"sparge_target_temperature\":" + String(SpargeTargetTemperature) + ',' +
           "\"recirculation\":" + String(Recirculation) + "," +
           "\"heater_on\":" + String(HeaterOn) + "," +
           "\"step_lock\":" + String(StepLock) + "," +
           "\"step_locked\":" + String(StepLocked) + "," +
           "\"pid_tuning\":" + String(PIDTuning) + "," +
           "\"pump_on\":" + String(PumpOn) + "," +
           "\"pump_is_resting\":" + String(PumpIsResting) + "," +
           "\"boil_power_percentage\":" + String(BoilPowerPercentage) +
           "}";
}

void ActiveStatus::SaveActiveStatus(time_t startTime,
                                    time_t endTime,
                                    time_t timeNow,
                                    float targetTemperature,
                                    int activeMashStepIndex,
                                    String activeBoilStepIndex,
                                    int boilTime,
                                    float boilTargetTemperature,
                                    int activeStep,
                                    boolean brewStarted)
{
    ActiveStep = activeStep;

    ActiveMashStepIndex = activeMashStepIndex;
    ActiveMashStepName = "";
    ActiveMashStepSufixName = "";
    ActiveBoilStepIndex = activeBoilStepIndex;
    ActiveBoilStepName = "";
    BoilTime = boilTime;
    BoilTargetTemperature = boilTargetTemperature;
    TargetTemperature = targetTemperature;
    StartTime = startTime;
    EndTime = endTime;
    TimeNow = timeNow;
    BrewStarted = brewStarted;
    Temperature = 0;
    PWM = 0;
    PWMPercentage = 0;
    SpargePWM = 0;
    SpargePWMPercentage = 0;
    Recirculation = false;
    HeaterOn = true;
    StepLock = false;
    StepLocked = false;
    PIDTuning = false;
    BoilPowerPercentage = 0;

    SaveActiveStatus();
}

time_t lastWrite = now();

void ActiveStatus::SaveActiveStatusLoop()
{
    if ((!BrewStarted) || (now() - lastWrite < 60))
        return;

    SaveActiveStatus();
}

void ActiveStatus::SaveActiveStatus()
{
    lastWrite = now();

    DynamicJsonDocument _activeStatusJsonDocument = DynamicJsonDocument(MAX_ACTIVESTATUS_SIZE);
    JsonObject _activeStatus = _activeStatusJsonDocument.to<JsonObject>();

    _activeStatus["active_step"] = ActiveStep;
    _activeStatus["active_mash_step_index"] = ActiveMashStepIndex;
    _activeStatus["active_mash_step_name"] = ActiveMashStepName;
    _activeStatus["active_mash_step_sufix_name"] = ActiveMashStepSufixName;
    _activeStatus["active_boil_step_index"] = ActiveBoilStepIndex;
    _activeStatus["active_boil_step_name"] = ActiveBoilStepName;
    _activeStatus["boil_time"] = BoilTime;
    _activeStatus["boil_target_temperature"] = BoilTargetTemperature;
    _activeStatus["target_temperature"] = TargetTemperature;
    _activeStatus["start_time"] = StartTime;
    _activeStatus["end_time"] = EndTime;
    _activeStatus["time_now"] = TimeNow;
    _activeStatus["brew_started"] = BrewStarted;
    _activeStatus["temperature"] = Temperature;
    _activeStatus["pwm"] = PWM;
    _activeStatus["sparge_pwm"] = SpargePWM;
    _activeStatus["recirculation"] = Recirculation;
    _activeStatus["heater_on"] = HeaterOn;
    _activeStatus["step_lock"] = StepLock;
    _activeStatus["boil_power_percentage"] = BoilPowerPercentage;
    _activeStatus["pid_tuning"] = PIDTuning;

    File configFile = _fs->open(ACTIVE_STATUS_FILE, "w");
    if (configFile)
        serializeJson(_activeStatus, configFile);

    configFile.close();
}

void ActiveStatus::SetTemperature(float temperature)
{
    if (temperature >= 0)
        Temperature = temperature;
}

void ActiveStatus::SetSpargeTemperature(float temperature)
{
    if (temperature >= 0)
        SpargeTemperature = temperature;
}

void ActiveStatus::SetJsonTemperatures(String json)
{
    Temperatures = json;
}