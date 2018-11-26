#include "ns.h"

extern union Nsipc nsipcbuf;
void
sleep(int msec)
{
	unsigned now = sys_time_msec();
	unsigned end = now + msec;

	if ((int)now < 0 && (int)now > -MAXERROR)
		panic("sys_time_msec: %e", (int)now);
	if (end < now)
		panic("sleep: wrap");

	while (sys_time_msec() < end)
		sys_yield();
}
void
input(envid_t ns_envid)
{
	binaryname = "ns_input";

size_t size;
	char buf[1518];
	while(1) {
		if (sys_pkt_rx(buf, &size) < 0) {
			continue;
		}
		memcpy(nsipcbuf.pkt.jp_data, buf, size);
		nsipcbuf.pkt.jp_len = size;
		ipc_send(ns_envid, NSREQ_INPUT, &nsipcbuf, PTE_P|PTE_W|PTE_U);
		sleep(50);
	}
}
