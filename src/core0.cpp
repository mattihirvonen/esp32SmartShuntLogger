
#include <stdint.h>
#include "Arduino.h"
//#include <task.h>
#include <PubSubClient.h>     // MQTT

#define LED_PIN    2
#define PEROIO_us  49         // Abs min 33 us

#define OFFSET 5
#define RANGE  30

hw_timer_t *timer = NULL;

static int time_stamp;

volatile int  datatable[1002];

// https://esp32.co.uk/testing-esp32-hardware-timers-with-interrupts-beginner-friendly-guide/

void IRAM_ATTR onTimer()
{
    static int init = 0;

    int  time_now = esp_timer_get_time();           // uint64_t [us]

    digitalWrite(LED_PIN, !digitalRead(LED_PIN));   // Toggle LED

    if ( ! init )
    {
        time_stamp = time_now;
        init = 1;
    }
    else
    {
        int  time_diff = (time_now - time_stamp) - PEROIO_us;

        if ( time_diff <= -OFFSET )  {
            datatable[0] += 1;
        }
        else if ( time_diff >= (RANGE - OFFSET) ) {
            datatable[RANGE] += 1;
        }
        else {
            datatable[time_diff + OFFSET] += 1;
        }
        time_stamp += PEROIO_us;
    }
}


void setup_timer()
{
    timer = timerBegin(0, 80, true);               // Timer 0, prescaler 80 (1 µs tick)
    timerAttachInterrupt(timer, &onTimer, true);
    timerAlarmWrite(timer, PEROIO_us, true);       // Trigger every PEROIO_us
    timerAlarmEnable(timer);                       // Start timer

    pinMode(LED_PIN, OUTPUT);
}


#if  0
void setup_task0( void )
{
    pinMode(LED_PIN, OUTPUT);
}


void vTaskCore0( void * pvParameters )
{
    while ( 1 )
    {
        int  time_now = esp_timer_get_time();    // [us]

        datatable[1001] += 1;
    }
}
#endif

//------------------------------------------------------------------------------------

void publish_histogram( PubSubClient &mqttClient )
{
  static char buffer[1500];

  *buffer = 0;
  sprintf( &buffer[ strlen(buffer)], "**: %d\n", datatable[0] );
  for ( int ix = 1; ix < RANGE; ix++ )
  {
    sprintf( &buffer[ strlen(buffer)], "%2d: %d\n", ix - OFFSET, datatable[ix] );
  }
  sprintf( &buffer[ strlen(buffer)], "**: %d\n", datatable[RANGE] );

  mqttClient.publish( "histogram", (const uint8_t*) buffer, strlen(buffer)+1 );
}
