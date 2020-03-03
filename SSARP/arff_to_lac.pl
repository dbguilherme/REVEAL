#!/usr/bin/perl

my $infile = $ARGV[0];
my $train_file = $ARGV[1];
my $featnum = $ARGV[2];
my $outfile = $ARGV[3];

my @bins;
my @vals;
my $linecount = 1;

open (F1, $train_file) || die ("Could not open $file!");
open (F3, ">$outfile") || die ("Could not open $file!");
#open (F4, ">>$outfile.weka") || die ("Could not open $file!");
$maxbins=0;


while ($line = <F1>) {
    (@vals) = split ',', $line;
    $class = $vals[@vals-1];
    $class =~ s/\n/ /;
    print F3 "$linecount CLASS=$class";
#    print F4 "$linecount CLASS=$class";
#     for ($i=1; $i<=$featnum; $i++) {
#        print F3 "w[$i]=$vals[$i-1] ";
#     }
    $linecount++;
    print F3 "\n";
}