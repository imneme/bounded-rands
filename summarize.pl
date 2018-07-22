#!/usr/bin/perl -lw

use strict; 
use Getopt::Long;

my %timeFor;
my $byPRNG;
my $byMethod;
my $byCompiler;
my $bySeed;

GetOptions ("method"   => \$byMethod,
	    "prng"     => \$byPRNG,
            "compiler" => \$byCompiler,
            "seed"     => \$bySeed)
    or die("Error in command line arguments\n");


foreach my $file (@ARGV) { 
    my $id = $file;
    $id =~ s{\.([^.]+)\.([^.]+)\.([^.]+)\.([^.]+)\.}{
	($byMethod ? ".$1" : "") . ($byPRNG ? ".$2" : "") .
        ($byCompiler ? ".$3" : "") . ($bySeed ? ".$4" : "") . "."}e; 
    $id =~ s{.*/}{};
    $id =~ s{\.out}{};
    open my $fh, "<", $file or die "$!"; 
    while (<$fh>) { 
	chomp; 
	next unless m{^(.*?) completed \((\S+) seconds\)}; 
	push @{$timeFor{$id}{$1}}, $2; 
    }
}

foreach my $id (sort keys %timeFor) { 
    my @cols;
    foreach my $kind (sort keys %{$timeFor{$id}}) { 
	my $sum = 0;
	$sum += log $_ foreach @{$timeFor{$id}{$kind}}; 
	my $avg = $sum/@{$timeFor{$id}{$kind}}; 
	push @cols, exp $avg; 
    } 
    print join("\t", $id, @cols); 
}
