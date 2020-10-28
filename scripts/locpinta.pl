#!/usr/bin/perl


# Graph my evolution, I need feedback :)


use strict;


my $file = "loccount.txt";
my $cmd = "/usr/bin/gnuplot";


open my $f, "< $file" or
        die "Can't open file: $!";

my @l = <$f>;

close $f;


open CMD, "| $cmd > loccount.svg";
#open CMD, "| cat";

print CMD <<EOG;
reset
set terminal svg size 640,480 fname "FreeSans"
set xdata time
set timefmt "%Y%m%d"
set format x "%y%m"
set ylabel "locs"
set style line 80 lt 0
set style line 80 lt rgb "#808080"
set border 3 back linestyle 80
set style line 81 lt 3
set style line 81 lt rgb "#808080" lw 0.5
set grid back linestyle 81
set xtics nomirror
set ytics nomirror
EOG

foreach( @l ) {
	my( $tim, $lns, @text ) = split /\s+/;

	next unless @text;

	# add/substract an offset to move the labels out of the way
	print CMD qq{set label "}, join( ' ', @text ), qq{" at "},
		$tim + 12, qq{",}, $lns - 170, "\n";
}

print CMD <<EOG;
set style line 82 lt 1
set style line 82 lt rgb "#005000" lw 2.5
plot "$file" using 1:2 with lines title "loc evolution" linestyle 82
show label
EOG



