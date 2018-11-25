#include <kern/e1000.h>
#include <kern/pmap.h>
#include <inc/string.h>


// __attribute__((__aligned__(16)))
// struct tx_desc_t tx_descs[N_TX_DESCS];

volatile uint32_t* e1000;
// volatile struct tx_desc_t* tx_descs;
__attribute__((__aligned__(16)))
volatile struct tx_desc_t tx_descs[N_TX_DESCS]  ;
#define E1000_SET_REG(offset,val) *((uint32_t*)(E1000_REG(e1000,offset))) = val
static physaddr_t tx_buffer[N_TX_DESCS];
static size_t current = 0;
volatile struct e1000_reg_tdlen* tdlen;
volatile struct e1000_reg_tdh* tdh;
volatile struct e1000_reg_tdt* tdt;
volatile struct e1000_reg_tctl* tctl;
volatile struct e1000_reg_tipg* tipg;

// LAB 6: Your driver code here
//initialize the card to transmit
static int tx_init(){
    //alloc list of tx_desc_t
    // struct PageInfo* pp = page_alloc(ALLOC_ZERO);
    // if(!pp) panic("failed to alloc tx_descs");
    
    // tx_descs = (struct tx_desc_t*)page2kva(pp);
    E1000_SET_REG(E1000_TDBAL,PADDR((void*)tx_descs));
    E1000_SET_REG(E1000_TDBAH,0);

    //alloc memory for packets
    int i=0;
    for(i=0;i<N_TX_DESCS;++i){
        struct PageInfo* pp = page_alloc(ALLOC_ZERO);
        if(!pp) panic("failed to alloc mem for packets");
        tx_buffer[i] = page2pa(pp);
        tx_descs[i].addr = 0;
        tx_descs[i].cmd = 0;
        // tx_descs[i].status_reg.DD = 1;

    }
tdlen = ( struct e1000_reg_tdlen*)(E1000_REG(e1000,E1000_TDLEN));
    tdlen->len =N_TX_DESCS;
    Dprintf("tdlen= %d\n",tdlen->len);
    tdlen->zero = 0;
    tdlen->rsv = 0;
    
tdh =(struct e1000_reg_tdh*)(E1000_REG(e1000,E1000_TDH));
    tdh->rsv =0;
    tdh->tdh = 0;
    
tdt = ( struct e1000_reg_tdt*)(E1000_REG(e1000,E1000_TDT));
    tdt->rsv =0;
    tdt->tdt = 0;

tctl = ( struct e1000_reg_tctl*)(E1000_REG(e1000,E1000_TCTL));
    tctl->en = 1;
    tctl->psp = 1;
    tctl->ct = 0x10; 
    tctl->cold = 0x40;

tipg = ( struct e1000_reg_tipg*)(E1000_REG(e1000,E1000_TIPG));
    tipg->ipgt = 10;
    tipg->ipgr1= 4;
    tipg->ipgr2 = 6;


    return 0;

}
int e1000_transmit(uint32_t* ta, size_t len){
    // while( !(tx_descs[current].status&0xf) ){
    //     //wait;
    // }
    tx_descs[current].addr = tx_buffer[current];
    tx_descs[current].length = len;

    tx_descs[current].cmd_reg.RS = 1;
    tx_descs[current].cmd_reg.IDE = 1;
    tx_descs[current].cmd_reg.EOP = 1;
    // Dprintf("tx_descs[current].cmd_reg = %b",tx_descs[current].cmd_reg);
    memcpy(KADDR(tx_buffer[current]),ta,len);

    uint32_t next = (current + 1) % N_TX_DESCS;
    tdt->tdt = next;
    // Dprintf("tdt->tdt %d  tdh->tdh %d",tdt->tdt,tdh->tdh);
    Dprintf("tx_descs[current] %08x",tx_descs[current].status);
    while(!(tx_descs[current].status&0xf)){
        //wait
    }
   
    current = next;
    return 0;
}
int e1000_attachfn(struct pci_func *pcif){
    pci_func_enable(pcif);
    e1000 = mmio_map_region((physaddr_t)pcif->reg_base[0],pcif->reg_size[0]);
    const uint32_t* status = E1000_REG(e1000,E1000_STATUS);
    assert(*status == 0x80080783);
    tx_init();
    return 0;

}

