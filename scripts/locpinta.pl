#!/usr/bin/perl


# Graph my evolution, I need feedback :)


use strict;


my $file = "loccount.txt";
my $cmd = "/usr/bin/gnuplot";


open my $f, "< $file" or
        die "Can't open file: $!";

my @l = <$f>;

close $f;


open CMD, "| $cmd > loccount.png";

print CMD <<EOG;
reset
set terminal png
set xdata time
set timefmt "%Y%m%d"
set format x "%y%m"
set ylabel "locs"
set grid
EOG

foreach (@l) {
	my ($tim, $lns, @text) = split /\s+/;

	next unless @text;

	print CMD qq{set label "}, join ( ' ', @text ), qq{" at "$tim",$lns\n};
}


print CMD <<EOG;
plot "$file" using 1:2 with lines title "loc evolution"
show label
EOG



