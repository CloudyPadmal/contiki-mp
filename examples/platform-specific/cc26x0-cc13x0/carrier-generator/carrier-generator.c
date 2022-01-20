#include "contiki.h"
#include "net/netstack.h"
#include "net/nullnet/nullnet.h"
#include "dev/leds.h"
#include "packetbuf.h"
#include <string.h>
#include <stdio.h>

#include "sys/log.h"
#define LOG_MODULE "CARRIER"
#define LOG_LEVEL LOG_LEVEL_INFO

/* Configuration */
#define SEND_INTERVAL (CLOCK_SECOND)

PROCESS(carrier_node, "Carrier Node");
AUTOSTART_PROCESSES(&carrier_node);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(carrier_node, ev, data)
{
  static struct etimer periodic_timer;
  
  PROCESS_BEGIN();

  static int configuration[3]; 
  configuration[0] = 24;      // Channel 
  configuration[1] = 0;       // Power [0(5 dBm) - 12(-21 dBm)]
  configuration[2] = 0;       // Modulated/Not

  etimer_set(&periodic_timer, SEND_INTERVAL*0.25);
  static bool carrier_on = true;
  static int power_level = 0;

  while (1)
  {
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&periodic_timer));
    leds_toggle(LEDS_GREEN);
    if (carrier_on) {
      configuration[1] = power_level;
      NETSTACK_CONF_RADIO.set_object(RADIO_POWER_MODE_CARRIER_ON, configuration, 3);
      if (power_level == 12) power_level = 0;
      power_level++;
    } else {
      NETSTACK_CONF_RADIO.set_object(RADIO_POWER_MODE_CARRIER_OFF, configuration, 3);
    }
    carrier_on = !carrier_on;
    etimer_reset(&periodic_timer);
  }

  PROCESS_END();
}