/*
 * Check decoding of sockaddr fields under xlat styles.
 *
 * Copyright (c) 2015-2018 The strace developers.
 * All rights reserved.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "tests.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/ax25.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

static void
check_ll(void)
{
	struct sockaddr_ll c_ll = {
		.sll_family = AF_PACKET,
		.sll_protocol = htons(ETH_P_ALL),
		.sll_ifindex = 0xfacefeed,
		.sll_hatype = ARPHRD_ETHER,
		.sll_pkttype = PACKET_HOST,
		.sll_halen = sizeof(c_ll.sll_addr),
		.sll_addr = "abcdefgh"
	};
	unsigned int len = sizeof(c_ll);
	int rc = connect(-1, (void *) &c_ll, len);
	const char *errstr = sprintrc(rc);

#if XLAT_RAW
	printf("connect(-1, {sa_family=%#x, sll_protocol=", AF_PACKET);
	print_quoted_hex(&c_ll.sll_protocol, sizeof(c_ll.sll_protocol));
	printf(", sll_ifindex=%u, sll_hatype=%#x"
	       ", sll_pkttype=%u, sll_halen=%u, sll_addr="
	       "[%#02x, %#02x, %#02x, %#02x, %#02x, %#02x, %#02x, %#02x]"
	       "}, %u) = %s\n",
	       c_ll.sll_ifindex, ARPHRD_ETHER,
	       PACKET_HOST, c_ll.sll_halen,
	       c_ll.sll_addr[0], c_ll.sll_addr[1],
	       c_ll.sll_addr[2], c_ll.sll_addr[3],
	       c_ll.sll_addr[4], c_ll.sll_addr[5],
	       c_ll.sll_addr[6], c_ll.sll_addr[7],
	       len, errstr);
#elif XLAT_VERBOSE
	printf("connect(-1, {sa_family=%#x /* AF_PACKET */"
	       ", sll_protocol=", AF_PACKET);
	print_quoted_hex(&c_ll.sll_protocol, sizeof(c_ll.sll_protocol));
	printf(" /* htons(ETH_P_ALL) */"
	       ", sll_ifindex=%u, sll_hatype=%#x /* ARPHRD_ETHER */"
	       ", sll_pkttype=%u /* PACKET_HOST */, sll_halen=%u, sll_addr="
	       "[%#02x, %#02x, %#02x, %#02x, %#02x, %#02x, %#02x, %#02x]"
	       "}, %u) = %s\n",
	       c_ll.sll_ifindex, ARPHRD_ETHER,
	       PACKET_HOST, c_ll.sll_halen,
	       c_ll.sll_addr[0], c_ll.sll_addr[1],
	       c_ll.sll_addr[2], c_ll.sll_addr[3],
	       c_ll.sll_addr[4], c_ll.sll_addr[5],
	       c_ll.sll_addr[6], c_ll.sll_addr[7],
	       len, errstr);

#else /* XLAT_ABBREV */
	printf("connect(-1, {sa_family=AF_PACKET"
	       ", sll_protocol=htons(ETH_P_ALL)"
	       ", sll_ifindex=%u, sll_hatype=ARPHRD_ETHER"
	       ", sll_pkttype=PACKET_HOST, sll_halen=%u, sll_addr="
	       "[%#02x, %#02x, %#02x, %#02x, %#02x, %#02x, %#02x, %#02x]"
	       "}, %u) = %s\n",
	       c_ll.sll_ifindex, c_ll.sll_halen,
	       c_ll.sll_addr[0], c_ll.sll_addr[1],
	       c_ll.sll_addr[2], c_ll.sll_addr[3],
	       c_ll.sll_addr[4], c_ll.sll_addr[5],
	       c_ll.sll_addr[6], c_ll.sll_addr[7],
	       len, errstr);
#endif
}

static void
check_in(void)
{
	const unsigned short h_port = 12345;
	static const char h_addr[] = "127.0.0.1";
	struct sockaddr_in in = {
		.sin_family = AF_INET,
		.sin_port = htons(h_port),
		.sin_addr.s_addr = inet_addr(h_addr)
	};
	unsigned int len = sizeof(in);
	int rc = connect(-1, (void *) &in, len);
	const char * errstr = sprintrc(rc);
#if XLAT_RAW
	printf("connect(-1, {sa_family=%#x, sin_port=", AF_INET);
	print_quoted_hex((const void *) &in.sin_port, sizeof(in.sin_port));
	printf(", sin_addr=");
	print_quoted_hex((const void *) &in.sin_addr.s_addr,
			sizeof(in.sin_addr.s_addr));
	printf("}, %u) = %s\n", len, errstr);
#elif XLAT_VERBOSE
	printf("connect(-1, {sa_family=%#x /* AF_INET */, sin_port=", AF_INET);
	print_quoted_hex((const void *) &in.sin_port, sizeof(in.sin_port));
	printf(" /* htons(%hu) */, sin_addr=", h_port);
	print_quoted_hex((const void *) &in.sin_addr.s_addr,
				sizeof(in.sin_addr.s_addr));
	printf(" /* inet_addr(\"%s\") */}, %u) = %s\n",
			h_addr, len, errstr);
#else /* XLAT_ABBREV */
	printf("connect(-1, {sa_family=AF_INET, sin_port=htons(%hu)"
	       ", sin_addr=inet_addr(\"%s\")}, %u) = %s\n",
	       h_port, h_addr, len, errstr);
#endif
}

static void
validate_in6(struct sockaddr_in6 *const in6, const char *const h_addr)
{
	inet_pton(AF_INET6, h_addr, &in6->sin6_addr);

	unsigned int len = sizeof(*in6);
	int rc = connect(-1, (void *) in6, len);
	const char *errstr = sprintrc(rc);
#if XLAT_RAW
	printf("connect(-1, {sa_family=%#x, sin6_port=", AF_INET6);
	print_quoted_hex(&in6->sin6_port, sizeof(in6->sin6_port));
	printf(", sin6_addr=");
	print_quoted_hex(&in6->sin6_addr, sizeof(struct in6_addr));
	printf(", sin6_flowinfo=");
	print_quoted_hex(&in6->sin6_flowinfo, sizeof(in6->sin6_flowinfo));
	printf(", sin6_scope_id=%u}, %u)"
	       " = %s\n", in6->sin6_scope_id, len, errstr);
#elif XLAT_VERBOSE
	printf("connect(-1, {sa_family=%#x /* AF_INET6 */", AF_INET6);
	printf(", sin6_port=");
	print_quoted_hex(&in6->sin6_port, sizeof(in6->sin6_port));
	printf(" /* htons(%hu) */", ntohs(in6->sin6_port));
	printf(", sin6_addr=");
	print_quoted_hex(&in6->sin6_addr, sizeof(struct in6_addr));
	printf(" /* inet_pton(AF_INET6, \"%s\") */", h_addr);
	printf(", sin6_flowinfo=");
	print_quoted_hex(&in6->sin6_flowinfo, sizeof(in6->sin6_flowinfo));
	printf(" /* htonl(%u) */"
	       ", sin6_scope_id=%u}, %u)"
	       " = %s\n",
	       ntohl(in6->sin6_flowinfo), in6->sin6_scope_id,
		     len, errstr);
#else
	printf("connect(-1, {sa_family=AF_INET6, sin6_port=htons(%hu)"
	       ", inet_pton(AF_INET6, \"%s\", &sin6_addr)"
	       ", sin6_flowinfo=htonl(%u)"
	       ", sin6_scope_id=%u}, %u)"
	       " = %s\n",
	       ntohs(in6->sin6_port), h_addr,
	       ntohl(in6->sin6_flowinfo), in6->sin6_scope_id,
		     len, errstr);
#endif
}

static void
check_in6(void)
{
	struct sockaddr_in6 in6 = {
		.sin6_family = AF_INET6,
		.sin6_port = htons(12345),
		.sin6_flowinfo = htonl(123456890),
		.sin6_scope_id = 0xfacefeed
	};

	validate_in6(&in6, "12:34:56:78:90:ab:cd:ef");
	validate_in6(&in6, "::");
	validate_in6(&in6, "::1");
}

int
main(void)
{
	check_ll();
	check_in();
	check_in6();
	puts("+++ exited with 0 +++");
	return 0;
}
