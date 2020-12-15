#! /usr/bin/perl
use strict;
use warnings;

if ($#ARGV != 0 ) {
    print "usage: tagxref.pl dir\n";
    exit;
}

my $src_dir = $ARGV[0];
my $tag_file = '/tmp/ctags.out';

# generate tags
system("ctags --recurse -x $src_dir > $tag_file") and die "couldn't open directory $!\n";
# print "$!\n";

# read each symbol
open my $tag_info, $tag_file or die "Could not open $tag_file: $!";

while( my $line = <$tag_info>)  {
    my ($symbol, $type, $line_no, $file) = (split ' ', $line);

    system("/bin/echo -n `grep -R -w $symbol $src_dir | wc -l`");
    print(" $symbol: $type in $file\n");
    # system("grep -R -w -c $symbol $src_dir | grep -v ':0'");
}

close $tag_info;
