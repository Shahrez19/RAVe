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

# Counts the frequency of each filetype in the directory recursively
sub count_files {
	#my $filename = $File::Find::name;

	if (-f $_) {
		$count_regular++;
	#	print $_ . "\t" . $filename . "\tregular\n";
	} elsif (-d $_) {
		$count_directory++;
	#	print $_ . "\tdirectory\n";
	} elsif (-p $_) {
		$count_fifo++;
	#	print $_ . "\tFIFO\n";
	} elsif (-S $_) {
		$count_socket++;
	#	print $_ . "\tsocket\n";
	} elsif (-l $_) {
		$count_link++;
	#	print $_ . "\tsymlink\n";
	} elsif (-b $_) {
		$count_block++;
	#	print $_ . "\tblock\n";
	} elsif (-c $_) {
		$count_character++;
	#	print $_ . "\tcharacter\n"; 
	} else {
		print $_ . "\tunknown type\n";
	}
}
find(\&count_files, $home_directory);


# Just some print statements to help me with debugging
print "regular\t$count_regular\n";
print "directory\t$count_directory\n";
print "FIFO\t$count_fifo\n";
print "socket\t$count_socket\n";
print "symbolic link\t$count_link\n";
print "block\t$count_block\n";
print "character\t$count_character\n";

# Write all of the data to a file

my $filename = "filetypes.dat";

open(my $fh, '>', "filetypes.dat") or die "Error writing data to file.";

say $fh "Filetype\tFrequency";
say $fh "regular\t$count_regular";
say $fh "directory\t$count_directory";
say $fh "link\t$count_link";
say $fh "fifo\t$count_fifo";
say $fh "socket\t$count_socket";
say $fh "block\t$count_block";
say $fh "character\t$count_character";

close $fh;


# Pipe into GNUPLOT
open (GNUPLOT, "|gnuplot") or die "Error opening GNUPLOT";

say GNUPLOT "reset";
say GNUPLOT "set autoscale";
say GNUPLOT "set terminal jpeg";
say GNUPLOT "set output \"histogram.jpeg\"";
say GNUPLOT "set title \"My Histogram\"";
say GNUPLOT "set xlabel \"File Type\"";
say GNUPLOT "set ylabel \"Frequency\"";
say GNUPLOT "set boxwidth 0.5";
say GNUPLOT "set style fill solid border -1";
say GNUPLOT "set nokey";
say GNUPLOT "plot 'filetypes.dat' using 2:xtic(1) with boxes";

close GNUPLOT;

# Pretty print the output back to the client

print "Content-type: text/html\n\n";
print "<html>";
print "<body>";
print "<center>";
print "<h1 style='color:red' size='16'>CS410 Webserver</h1>";
print "<br>";
print "<img src='histogram.jpeg'>";
print "</center>";
print "</body>";
print "</html>";

