#ifndef MEMORY_MANAGER_MEMORY_MANAGER_H
#define MEMORY_MANAGER_MEMORY_MANAGER_H

#include <inttypes.h>

typedef union
{
    struct{
        float timestamp;
        float accelerometer [ 3 ];
        float gyroscope     [ 3 ];
    };
    uint8_t bytes[sizeof(float)  * 1   +
                  sizeof(float ) * 3   +
                  sizeof(float ) * 3
    ];
} IMUDataU;

typedef union
{
    struct{
        float pressure;
    };
    uint8_t bytes[sizeof(float)  * 1];
} GroundDataU;

typedef struct
{
    uint8_t updated;
    IMUDataU data;
} IMUData;


typedef union
{
    struct {
        float timestamp;
        float pressure;
        float temperature;
    };
    uint8_t bytes[sizeof(float)    * 1 +
                  sizeof(float)    * 1 +
                  sizeof(float)    * 1
    ];
} PressureDataU;


typedef struct
{
    uint8_t updated;
    PressureDataU data;
} PressureData;

typedef enum { Open      = 0, Short     = 1 } ContinuityStatus;
typedef enum { Launchpad = 0, PreApogee = 1, Apogee = 2, PostApogee = 3, MainChute = 4, PostMain = 5, Landed = 6, Exited = 7, Invalid = 8} FlightEventStatus;
typedef enum { IMU       = 0, Pressure  = 1,  Cont   = 2, Event      = 3, SectorsCount } Sector;

typedef union
{
    uint8_t updated;
    struct {
        uint32_t timestamp;
        ContinuityStatus status;
    };
    uint8_t bytes[sizeof(uint32_t) * 1 +
                  sizeof(uint8_t ) * 1
    ];
} ContinuityU;

typedef struct
{
    uint8_t updated;
    ContinuityU data;
} Continuity;

typedef union
{
    struct {
        uint32_t timestamp;
        FlightEventStatus status;
    };
    uint8_t bytes[sizeof(float) * 1 +
                  sizeof(uint8_t ) * 1
    ];
} FlightEventU;


typedef struct
{
    uint8_t updated;
    FlightEventU data;
} FlightEvent;

typedef struct
{
    uint32_t timestamp;
    IMUData inertial;
    PressureData pressure;
    Continuity continuity;
    FlightEvent event;
} Data;

#endif //MEMORY_MANAGER_MEMORY_MANAGER_H
