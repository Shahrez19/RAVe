#!/usr/bin/perl -w

use strict;
use warnings;
use File::Find;

# Inilialize all filetype counts
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
	if (-p $_) {
		$count_fifo++;
	} elsif (-S $_) {
		$count_socket++;
	} elsif (-l $_) {
		$count_link++;
	} elsif (-b $_) {
		$count_block++;
	} elsif (-c $_) {
		$count_character++;
	} elsif (-f $_) {
		$count_regular++;
	} elsif (-d $_) {
		$count_directory++;
	}
}

find(\&count_files, $home_directory);

# Write all of the data to a file called filetypes.dat
open(my $fh, ">filetypes.dat") or die "Error writing data to file.";

say $fh "Filetype\tFrequency";
say $fh "regular\t$count_regular";
say $fh "directory\t$count_directory";
say $fh "link\t$count_link";
say $fh "fifo\t$count_fifo";
say $fh "socket\t$count_socket";
say $fh "block\t$count_block";
say $fh "character\t$count_character";

close $fh;


# Pipe the format of the histogram into GNUPLOT
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
# print "HTTP/1.0 200 OK\n";
# print "Content-type: text/html\n";
# print "Content-Length: 168\n\n";
# print "<!DOCTYPE html>";
# print "<html>";
# print "<body>";
# print "<div style='text-align:center'>";
# print "<br>";
# print "<p style='color:red; font-size:16pt'>CS410 Webserver</p>";
# print "<br>";
# print "<img src='histogram.jpeg'>";
# print "</div>";
# print "</body>";
# print "</html>";
