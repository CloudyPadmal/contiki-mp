#include "net/packetbuf.h"
#include "net/netstack.h"
#include "nullmac-zoul.h"

/* Log configuration */
#include "sys/log.h"
#define LOG_MODULE "NLMC-ZL"
#define LOG_LEVEL LOG_LEVEL_WARN

/*---------------------------------------------------------------------------*/
static void
send_packet(mac_callback_t sent, void *ptr)
{
  packetbuf_set_attr(PACKETBUF_ATTR_FRAME_TYPE, FRAME802154_DATAFRAME);

  if (NETSTACK_FRAMER.create() < 0)
  {
    /* Failed to allocate space for headers */
    LOG_ERR("failed to create packet\n");
  }
  else
  {
    NETSTACK_RADIO.prepare(packetbuf_hdrptr(), packetbuf_totlen());
    NETSTACK_RADIO.transmit(packetbuf_totlen());
  }
}
/*---------------------------------------------------------------------------*/
static void
input_packet(void)
{
  if (NETSTACK_FRAMER.parse() < 0)
  {
    LOG_ERR("failed to parse %u\n", packetbuf_datalen());
  }
  else
  {
    NETSTACK_NETWORK.input();
  }
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  return NETSTACK_RADIO.on();
}
/*---------------------------------------------------------------------------*/
static int
off(void)
{
  return NETSTACK_RADIO.off();
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
  on();
}
/*---------------------------------------------------------------------------*/
static int
max_payload(void)
{
  return PACKETBUF_SIZE;
}
/*---------------------------------------------------------------------------*/
const struct mac_driver nullmac_zoul_driver = {
    "nullmac-zoul",
    init,
    send_packet,
    input_packet,
    on,
    off,
    max_payload,
};
/*---------------------------------------------------------------------------*/
