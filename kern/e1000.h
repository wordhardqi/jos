#ifndef JOS_KERN_E1000_H
#define JOS_KERN_E1000_H
#endif  // SOL >= 6
#include <kern/pci.h>
#define E1000_DEV_ID_82540EM             0x100E

#define E1000_TDBAL    0x03800  /* TX Descriptor Base Address Low - RW */
#define E1000_TDBAH    0x03804  /* TX Descriptor Base Address High - RW */
#define E1000_TDLEN    0x03808  /* TX Descriptor Length - RW */
#define E1000_TDH      0x03810  /* TX Descriptor Head - RW */
#define E1000_TDT      0x03818  /* TX Descripotr Tail - RW */
#define E1000_TIDV     0x03820  /* TX Interrupt Delay Value - RW */
#define E1000_TXDCTL   0x03828  /* TX Descriptor Control - RW */

/* Transmit Control */
#define E1000_TCTL     0x00400  /* TX Control - RW */
// #define E1000_TCTL_RST    0x00000001    /* software reset */
// #define E1000_TCTL_EN     0x00000002    /* enable tx */
// #define E1000_TCTL_BCE    0x00000004    /* busy check enable */
// #define E1000_TCTL_PSP    0x00000008    /* pad short packets */
// #define E1000_TCTL_CT     0x00000ff0    /* collision threshold */
// #define E1000_TCTL_COLD   0x003ff000    /* collision distance */
// #define E1000_TCTL_SWXOFF 0x00400000    /* SW Xoff transmission */
// #define E1000_TCTL_PBE    0x00800000    /* Packet Burst Enable */
// #define E1000_TCTL_RTLC   0x01000000    /* Re-transmit on late collision */
// #define E1000_TCTL_NRTU   0x02000000    /* No Re-transmit on underrun */
// #define E1000_TCTL_MULR   0x10000000    /* Multiple request support */

#define E1000_TIPG     0x00410  /* TX Inter-packet gap -RW */

#define E1000_REG(reg,offset) (void *)((char*)reg + offset)
#define E1000_STATUS    0x008

#define N_TX_DESCS 16
#define N_RX_DESCS 32


struct e1000_reg_cmd{
	unsigned EOP : 1;
	unsigned IFCS : 1;
	unsigned IC :1;
	unsigned RS : 1;
	unsigned RPS: 1;
	unsigned DEXT:1;
	unsigned VLE : 1 ;
	unsigned IDE : 1;
};
struct e1000_reg_tdlen{
    unsigned int zero :7;
    unsigned int len : 13;
    unsigned int rsv : 12;
};
struct e1000_reg_tdh{
    unsigned int tdh: 16;
    unsigned int rsv: 16;
};
struct e1000_reg_tdt{
    unsigned int tdt: 16;
    unsigned int rsv: 16;
};
struct e1000_reg_tctl {
	unsigned int rsv1   : 1;
	unsigned int en     : 1;
	unsigned int rsv2   : 1;
	unsigned int psp    : 1;
	unsigned int ct     : 8;
	unsigned int cold   : 10;
	unsigned int swxoff : 1;
	unsigned int rsv3   : 1;
	unsigned int rtlc   : 1;
	unsigned int nrtu   : 1;
	unsigned int rsv4   : 6;
};
struct e1000_reg_tipg {
	unsigned ipgt  : 10;
	unsigned ipgr1 : 10;
	unsigned ipgr2 : 10;
	unsigned rsv   : 2;
};

struct tx_desc_t
{
	uint64_t addr;
	uint16_t length;
	uint8_t cso;
	union{
	uint8_t cmd;
	struct e1000_reg_cmd cmd_reg;
	};
	
	uint8_t status;
	uint8_t css;
	uint16_t special;
} __attribute__((packed));


int e1000_attachfn(struct pci_func *pcif);
int  e1000_transmit(uint32_t* ta, size_t len);
