#!/usr/bin/perl
## compact.pl - PERL5 script to convert StrokeDic into C initializations
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
## Derived from prior work by Todd David Rudick on JavaDict and StrokeDic.
## Makes use of KANJIDIC data from Jim Breen of Monash University.
## Further credit details available at http://wellscs.com/pilot
## See readme.txt, changelo, and gpl.html for more information.
##
## Commentary:
##
## To run this perl script you need to acquire the following free software:
## 
## 1. A PERL executable.  See http://www.perl.com for information on
##    downloading a copy of perl for your computer.
##
## 2. A copy of the StrokeDic data file, renamed to StrokeDic.txt.
##    See http://www.cs.arizona.edu/japan/JavaDict/index.html or contact 
##    Todd David Rudick (mailto:rudick@cs.arizona.edu) to get a copy of this
##    file.
##
## 3. A copy of the KanjiDic data file, renamed to KanjiDic.txt, 
##    used to convert Unicode to Kuten to Shift-JIS codes.  
##    See ftp://ftp.cc.monash.edu.au/pub/nihongo/00INDEX.html
##    or contact Jim Breen at Monash University for how to download this file.
## 
## 4. kkindex.txt, included in this source distribution.  I entered the
##    codes used in Hadamitzky and Spahn's "Kanji & Kana: A Handbook and
##    dictionary of the Japanese Writing System", and also in their "A Guide
##    to Writing Kanji & Kana, Book 1 and 2", published by Tuttle.  The
##    handbook contains the correspondence between their numbers and
##    Nelson numbers; this code uses KanjiDic to map Nelson numbers to
##    corresponding Kanji.  This is all in preparation for future development
##    related to building up a regression test suite of raw handwritten
##    strokes for each Kanji and Kana, for establishing and maintaining the
##    accuracy of the recognition algorithm.  By the by, I learned from
##    Jim Breen after I entered this data that he recently got equivalent
##    data from Hadamitzky and Spahn, and it will be merged into KanjiDic
##    in the future.
##
## You run it with a command line like this:
##
##    perl compact.pl >newdata.c
##
## It generates a single warning about no Unicode for Hepburn code 2268,
## and this warning can be safely ignored.
##
## The output file newdata.c is easier to look at if your computer can
## display Shift-JIS encoded Japanese characters; I use KanjiKit97 for this
## on Windows95.
##
## I intend to supercede this with a Java application to generate the Pilot
## database directly; this will use a Java package available in the Pilot
## development community, and probably some of Todd David Rudick's Java code
## for reading StrokeDic as well.  PERL is a language I'm very comfortable
## with, and thus it was expedient for me to do initial text hacking in it.
##
## -rwells, 19970901.
## ---------------------------------------------------------------------------

$KANJIDIC = 'KanjiDic.txt';
open(KANJIDIC) || die "$KANJIDIC: $!\n";

$STROKEDIC = 'StrokeDic.txt';
open(STROKEDIC) || die "$STROKEDIC: $!\n";

$KKINDEX = 'kkindex.txt';
open(KKINDEX) || die "$KKINDEX: $!\n";

while (<KANJIDIC>)
{
    my ($jisCode1, $jisCode2, $jisInt1, $jisInt2);
    my ($rowOffset, $cellOffset, $sjis1, $sjis2, $sjisText, $kuten);
    my ($uniCode, $halCode, $strokes, $nelson);

    next if (/^ *\#/);		# Skip comment at top...

    if (/^[^ ][^ ] +([0-9a-f][0-9a-f])([0-9a-f][0-9a-f]) +/)
    {
	$jisCode1 = $1;
	$jisCode2 = $2;

	$jisInt1 = hex($jisCode1);
	$jisInt2 = hex($jisCode2);

	# This conversion algorithm is found in Ken Lunde's 
	# "Understanding Japanese Information Processing", 
	# published by O'Reilly & Associates, 1993.

	if ($jisInt1 < 95) {
	    $rowOffset = 112; }
	else {
	    $rowOffset = 176; }

	if ($jisInt1 % 2) {
	    if ($jisInt2 > 95) {
		$cellOffset = 32; }
	    else {
		$cellOffset = 31; }
	}
	else {
	    $cellOffset = 126;
	}

	$sjis1 = (($jisInt1 + 1) >> 1) + $rowOffset;
	$sjis2 = $jisInt2 + $cellOffset;

	$sjisText = sprintf('%c%c', $sjis1, $sjis2);
	$sjisEscaped = sprintf("\\%03o\\%03o", $sjis1, $sjis2);
	$sjisHex = sprintf('%02x%02x', $sjis1, $sjis2);
	$kuten = sprintf('%02d%02d', $jisInt1-32, $jisInt2-32);
    }

    if (/ +(U[0-9a-f]+) +/)
    {
	$uniCode = $1;
    }

    if (/ +(N[0-9]+) +/)
    {
	$nelson = $1;
    }

    if (/ +(H[0-9]+) +/)
    {
	$halCode = $1;
    }

    if (/ +S([0-9]+) +/)
    {
	$strokes = $1;
    }

    $uniLookup{$uniCode} =
	"$strokes:$sjisEscaped:$kuten:$jisCode1$jisCode2:$sjisText:$halCode:$nelson";
}

while (<KKINDEX>)
{
    my ($kkNum, $kkNelson);

    s/\#.*$//;			# Ignore comments starting with '#'

    if (/^ *kk([0-9]+) +(N[0-9]+)[ \t\r\n]*$/) {
	$kkNum = $1;
	$kkNelson = $2;
	$kkLookup{$kkNelson} = $kkNum;
    }
}

while (<STROKEDIC>)
{
    my ($uniCode, $halCode, $strokeCodes, $filters, $nelson);

    next if (/^ *\#/);		# Skip comment lines...
    next if (/^[ \t]*$/);	# Skip blank lines...

    if (/^[ \t]*([0-9a-f]+)[ \t]*([0-9]+)[ \t]*\|(.+)$/)
    {
	$uniCode = "U$1";
	$halCode = "H$2";
	$strokeCodes = $3;
	$strokeCodes =~ s/^[ \t]+([^ \t])/$1/; # Strip white space at front...
	$strokeCodes =~ s/\|.*$//;	# Strip filters at end...
	$strokeCodes =~ s/[ \t\r\n]+$//; # Strip white space at end...
    }

    # One line for Halpern 2268 has no unicode...
    if ($uniCode eq 'U0')
    {
	print STDERR "Warning: No Unicode for $_";
	next;
    }

    if (/^[^\|]+\|[^\|]+\|(.*)$/)
    {
	$filters = $1;
	$filters =~ s/^[ \t]+([^ \t])/$1/; # Strip white space at front...
	$filters =~ s/[ \t\r\n]+$//;	# Strip white space at end...
    }

    @strokeCodeList = split(/ +/, $strokeCodes);
    $realStrokeCount = @strokeCodeList;

    $comStrokes = '';
    foreach $stroker (@strokeCodeList)
    {
	if ($stroker =~ /^(.)(.*)$/)
	{
	    ($firstChar, $otherChars) = ($1, $2);

	    $firstChar =~ tr/123456789xycb/ABCDEFGHIJKLM/;
	    $otherChars =~ tr/123456789xycb/abcdefghijklm/;
	    $stroker = "$firstChar$otherChars";
	}
	$comStrokes .= $stroker;
    }

    $kanjiInfo = $uniLookup{$uniCode};
    my ($kStrokes, $kSJIS, $kKuten, $kJIScode, $kSJISText, $kHalpern, $kNelson) =
	split(/:/, $kanjiInfo);

###    $warn = '';
###
###    if ($kStrokes != $realStrokeCount)
###    {
###	print STDERR "Warning: $uniCode Stroke Count mismatch (s=$realStrokeCount k=$kStrokes)\n";
###	$warn .= "*S$kStrokes";
###    }
###
###    print STDERR "Warning: $uniCode Halpern code mismatch (s=$halCode k=$kHalpern)\n"
###	if ($kHalpern ne $halCode and $kHalpern ne '');
###

    $realStrokeChar = sprintf('%c', 64+$realStrokeCount);

###    print "$realStrokeChar:$kSJIS:$kKuten:$uniCode:$halCode:$comStrokes:$strokeCodes:$filters:$warn\n";


    $kkNum = $kkLookup{$kNelson};
    $kkNum = "kk$kkNum" if ($kkNum);

    $filters = "|$filters" if $filters ne '';
    push(@results,
	 "\"$realStrokeChar$kSJIS$comStrokes$filters\" // '$kSJISText' $kNelson $kkNum");
}

foreach $result (sort(@results))
{
    print "    $result\n";
}
