#!/usr/bin/perl -w

use strict;
use warnings;
use File::Find;

my $count_regular = 0;
my $count_directory = 0;
my $count_fifo = 0;
my $count_socket = 0;
my $count_link = 0;
my $count_block = 0;
my $count_character = 0;

my $home_directory = $ARGV[0];

sub process_file {
	#my $filename = $File::Find::name;

	if (-f $_) {
		$count_regular++;
		#print $_ . "\tregular\n";
	} elsif (-d $_) {
		$count_directory++;
		#print $_ . "\tdirectory\n";
	} elsif (-p $_) {
		$count_fifo++;
		#print $_ . "\tFIFO\n";
	} elsif (-S $_) {
		$count_socket++;
		#print $_ . "\tsocket\n";
	} elsif (-l $_) {
		$count_link++;
		#print $_ . "\tsymlink\n";
	} elsif (-b $_) {
		$count_block++;
		#print $_ . "\tblock\n";
	} elsif (-c $_) {
		$count_character++;
		#print $_ . "\tcharacter\n"; 
	} else {
		print $_ . "\tunknown type\n";
	}
}
find(\&process_file, $home_directory);

print "regular\t$count_regular\n";
print "directory\t$count_directory\n";
print "FIFO\t$count_fifo\n";
print "socket\t$count_socket\n";
print "symbolic link\t$count_link\n";
print "block\t$count_block\n";
print "character\t$count_character\n";

open (GNUPLOT, "|gnuplot") or die "Error opening GNUPLOT";

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
