#!/usr/bin/perl
## dos2unix.pl - PERL5 script to remove ^M from line-end before release.
## JStroke 1.x - Japanese Kanji handwriting recognition technology demo.
## Copyright (C) 1997  Robert E. Wells
## http://wellscs.com/pilot
## mailto:robert@wellscs.com
## 
## This program is free software; you can redistribute it and/or
## modify it under the terms of the GNU General Public License
## as published by the Free Software Foundation; either version 2
## of the License, or (at your option) any later version.
## 
## This program is distributed in the hope that it will be useful,
## but WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
## GNU General Public License for more details.
## 
## You should have received a copy of the GNU General Public License
## along with this program (gpl.html); if not, write to the Free Software
## Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
##
## Commentary:
##
## To run this perl script you need to acquire the following free software:
## 
## 1. A PERL executable.  See http://www.perl.com for information on
##    downloading a copy of perl for your computer.
##
## You run it with a command line like this:
##
##    perl dos2unix.pl
##
## It looks for text files that have trailing \r\n combos, and removes the
## \r (Control-M), renaming the original file to file.ext.cr.  It uses a 
## list of extensions to decide which are text files.  DOS programs seem
## determined to put in the Control-M before the newline, but on Unix it
## tends to upset some compilers and is generally an annoyance, so we try
## to remove them before zipping and posting to the net. -rwells, 19970922.
## 
## ---------------------------------------------------------------------------

# ----- fileHasCarriageReturn ------------------------------------------------

sub fileHasCarriageReturn {
    my ($FILE) = @_;
    my $line;
    my $retval = 0;

    open FILE,$FILE or die "open error: $FILE: $!\n";
    binmode FILE;		# Do raw reads even on Windows/DOS.
    while ($line = <FILE>) {
	if ($line =~ /\r/) {
	    $retval = 1;
	    break;
	}
    }
    close FILE;
    return $retval;
}

# ----- fixFile --------------------------------------------------------------

sub fixFile {
    my ($FILE) = @_;
    my $OLDFILE = "$FILE.old";
    my $NEWFILE = "$FILE.new";
    my $line;

    print "Fixing $FILE...";

    open FILE,$FILE or die "open error: $FILE: $!\n";
    binmode FILE;		# Do raw reads even on Windows/DOS.

    unlink $NEWFILE;
    open NEWFILE,">$NEWFILE" or die "create error: $NEWFILE: $!\n";
    binmode NEWFILE;		# Do raw writes even on Windows/DOS.

    while ($line = <FILE>) {
	$line =~ s/\r//g;
	print NEWFILE $line or die "print error: $NEWFILE: $!\n";
    }
    close NEWFILE;
    close FILE;

    unlink $OLDFILE;
    rename $FILE,$OLDFILE or die "rename error: $FILE -> $OLDFILE: $!\n";
    rename $NEWFILE,$FILE or die "rename error: $NEWFILE -> $FILE: $!\n";

    print " done.\n";
}

# ----- main -----------------------------------------------------------------

opendir THISDIR,"." or die "Error: .: $!\n";

my @allFiles = readdir THISDIR;
closedir THISDIR;

foreach $file (sort @allFiles) {
    # Ignore directory entries.
    next if $file eq '.' or $file eq '..';
    next unless -f $file;

    # Ignore known binary files that are installed.
    next if ($file =~ /^.+\.pdb$/i or
	     $file =~ /^.+\.prc$/i or
	     $file =~ /^.+\.bmp$/i);
	
    # Ignore backup files we created ourselves...
    next if ($file =~ /^.+\.old$/i);

    # Ignore files which cause errors on Windows if Control-M's are removed.
    next if ($file =~ /^.+\.bat$/i);

    fixFile($file) if fileHasCarriageReturn($file);
}

exit 0;

# ----- End of dos2unix.pl ---------------------------------------------------
