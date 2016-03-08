

#ifndef _PN544_H_
#define _PN544_H_

#include <linux/i2c.h>


#define PN544_MAGIC	0xE9


#define PN544_SET_PWR	_IOW(PN544_MAGIC, 0x01, unsigned int)

struct pn544_i2c_platform_data{
	unsigned int irq_gpio;
	unsigned int ven_gpio;
	unsigned int firm_gpio;
	unsigned int dcdc_gpio;
	unsigned int clock_gpio;
	unsigned int int_active_low;
};
//zte-modify by zuojianfang for nfc end
#define PN544_DRIVER_NAME	"pn544"
#define PN544_MAXWINDOW_SIZE	7
#define PN544_WINDOW_SIZE	4
#define PN544_RETRIES		10
#define PN544_MAX_I2C_TRANSFER	0x0400
#define PN544_MSG_MAX_SIZE	0x21 /* at normal HCI mode */

/* ioctl */
#define PN544_CHAR_BASE		'P'
#define PN544_IOR(num, dtype)	_IOR(PN544_CHAR_BASE, num, dtype)
#define PN544_IOW(num, dtype)	_IOW(PN544_CHAR_BASE, num, dtype)
#define PN544_GET_FW_MODE	PN544_IOW(1, unsigned int)
#define PN544_SET_FW_MODE	PN544_IOW(2, unsigned int)
#define PN544_GET_DEBUG		PN544_IOW(3, unsigned int)
#define PN544_SET_DEBUG		PN544_IOW(4, unsigned int)

/* Timing restrictions (ms) */
#define PN544_RESETVEN_TIME	30 /* 7 */
#define PN544_PVDDVEN_TIME	0
#define PN544_VBATVEN_TIME	0
#define PN544_GPIO4VEN_TIME	0
#define PN544_WAKEUP_ACK	5
#define PN544_WAKEUP_GUARD	(PN544_WAKEUP_ACK + 1)
#define PN544_INACTIVITY_TIME	1000
#define PN544_INTERFRAME_DELAY	200 /* us */
#define PN544_BAUDRATE_CHANGE	150 /* us */

/* Debug bits */
#define PN544_DEBUG_BUF		0x01
#define PN544_DEBUG_READ	0x02
#define PN544_DEBUG_WRITE	0x04
#define PN544_DEBUG_IRQ		0x08
#define PN544_DEBUG_CALLS	0x10
#define PN544_DEBUG_MODE	0x20

/* Normal (HCI) mode */
#define PN544_LLC_HCI_OVERHEAD	3 /* header + crc (to length) */
#define PN544_LLC_MIN_SIZE	(1 + PN544_LLC_HCI_OVERHEAD) /* length + */
#define PN544_LLC_MAX_DATA	(PN544_MSG_MAX_SIZE - 2)
#define PN544_LLC_MAX_HCI_SIZE	(PN544_LLC_MAX_DATA - 2)

struct pn544_llc_packet {
	unsigned char length; /* of rest of packet */
	unsigned char header;
	unsigned char data[PN544_LLC_MAX_DATA]; /* includes crc-ccitt */
};

/* Firmware upgrade mode */
#define PN544_FW_HEADER_SIZE	3
/* max fw transfer is 1024bytes, but I2C limits it to 0xC0 */
#define PN544_MAX_FW_DATA	(PN544_MAX_I2C_TRANSFER - PN544_FW_HEADER_SIZE)

struct pn544_fw_packet {
	unsigned char command; /* status in answer */
	unsigned char length[2]; /* big-endian order (msf) */
	unsigned char data[PN544_MAX_FW_DATA];
};

#ifdef __KERNEL__
/* board config */
struct pn544_nfc_platform_data {
	int (*request_resources) (struct i2c_client *client);
	void (*free_resources) (void);
	void (*enable) (int fw);
	int (*test) (void);
	void (*disable) (void);
};
#endif /* __KERNEL__ */

#endif /* _PN544_H_ */
