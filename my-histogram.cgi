#!/usr/bin/perl -w

use strict;
use warnings;
use File::Find wq(finddepth);
my @files;
finddepth( sub {
	return if ( $_ eq '.' || $_ eq '..');
	push @files, $File::Find::name;
	}, $home_directory);

$home_directory = $ARGV[0];

#open (GNUPLOT, "|/usr/bin/gnuplot") or die "Error opening GNUPLOT";

opendir(D, $home_directory) or die "Error opening directory\n";

while (my $file = readdir(D)) {
	print "\$f = $file\n";
}

closedir(D);

print "No errors so far";

#reset
#set autoscale
#set xtics (Regular, Directory, FIFO, Socket, Sym Link, Block, Character)
#set ylabel
#set title "My Histogram" 
#set terminal
##set term post eps color 24
##set output
#
#plot
#
#GNU
#print GNU $gnuplot_string
