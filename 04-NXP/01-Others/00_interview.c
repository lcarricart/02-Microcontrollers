// Interview 1
// Objective: calculate the battery load; input is Voltage. Battery load should be expressed in percentage.

typedef enum {
    BATTERY_OK = 0,
    BATTERY_ERROR_PARAM
} battery_status_t;

battery_status_t batteryPercentage(float battery, float maxCharge, float* batteryPerc) {
    if ((battery < 0) || (battery > maxCharge)) {
        return BATTERY_ERROR_PARAM;
    }

    *batteryPerc = (battery * 100) / maxCharge;

    return BATTERY_OK;
}

// This solution is provided in C. However, in C++, one could turn exceptions ON and use handlers.