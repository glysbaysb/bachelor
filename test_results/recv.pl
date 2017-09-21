#!/usr/bin/perl
#
# UDP Listen
#
use IO::Socket::INET;
use strict;

my ($sock);
my $data;

$sock=IO::Socket::INET->new(Proto => 'udp',
LocalPort => 7777) or die "Can't bind: $@\n";

while($sock->recv($data, 1024)) {
my ($port, $ipaddr) = sockaddr_in($sock->peername);
my ($peerhost)=gethostbyaddr($ipaddr, AF_INET);
my ($peerip) = inet_ntoa($ipaddr);

print "$data";
}

