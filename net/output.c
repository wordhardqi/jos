#include "ns.h"
#include <inc/lib.h>
extern union Nsipc nsipcbuf;

void
output(envid_t ns_envid)
{
	binaryname = "ns_output";

	// LAB 6: Your code here:
	// 	- read a packet from the network server
	//	- send the packet to the device driver
	extern union Nsipc nsipcbuf;
	envid_t who;
	int32_t perm=0;
	int req;
	 


	while(true){
		req = ipc_recv(&who, (void*)(&nsipcbuf),&perm);
		if((perm & PTE_P) ==0){
			cprintf("Invalid request from %08x: no argument page\n",
				who);
			continue; 
		}
		assert(req == NSREQ_OUTPUT);
		if (req != NSREQ_OUTPUT) {
			cprintf("Not a NSREQ_OUTPUT\n");
			continue;
		}
		struct jif_pkt *pkt = &(nsipcbuf.pkt);
		sys_pkt_tx((uint32_t)(pkt->jp_data), pkt->jp_len);
		sys_page_unmap(0,(void*)(&nsipcbuf));
	}
}
