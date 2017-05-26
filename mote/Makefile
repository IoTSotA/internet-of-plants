APPLICATION = plants-demo

BOARD ?= pba-d-01-kw2x
 
RIOTBASE ?= $(CURDIR)/../../RIOT

FEATURES_REQUIRED = periph_adc

BOARD_INSUFFICIENT_MEMORY := chronos msb-430 msb-430h nucleo32-f031 nucleo32-f042 \
                             nucleo32-l031 nucleo-f030 nucleo-f334 nucleo-l053 \
                             stm32f0discovery telosb weio wsn430-v1_3b wsn430-v1_4 z1

# Must read nordic_softdevice_ble package before nanocoap package. However,
# can't read it explicitly here because it is read later, as a dependency for
# the nrf52dk board.
BOARD_BLACKLIST := nrf52dk

USEPKG += nanocoap
# Required by nanocoap, but only due to issue #5959.
USEMODULE += posix

# Include packages that pull up and auto-init the link layer.
# NOTE: 6LoWPAN will be included if IEEE802.15.4 devices are present
USEMODULE += gnrc_netdev_default
USEMODULE += auto_init_gnrc_netif
# Specify the mandatory networking modules
USEMODULE += gnrc_ipv6_default
USEMODULE += gnrc_sock_udp
USEMODULE += gcoap

USEMODULE += saul_default
USEMODULE += xtimer

CFLAGS += -DDEVELHELP

include $(RIOTBASE)/Makefile.include