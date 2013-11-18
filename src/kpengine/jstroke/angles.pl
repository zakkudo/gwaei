#!/usr/bin/perl
## angles.pl - PERL5 script to determine table for angle32
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
## You run it with a command line like this:
##
##    perl angles.pl >angles.txt
##
## It generates a lot of printout in angles.txt, testing the table driven
## angle32 code against the PERL5 floating point trig library functions.
##
## This code was used to generate various "magic" angle and score values
## for use in JStroke 1.x.
##
## -rwells, 19970901.
## ---------------------------------------------------------------------------

my $pi = atan2(1,1)*4;		# See perl5 manual p146.

# ----- test_ang32 -----------------------------------------------------------

sub test_ang32 {
    my ($ix, $iy) = @_;
    my $islope = !(-0.1<$iy&&$iy<0.1) ? int((100 * $ix) / $iy) : 'inf';

    $i32th = ang32($ix, $iy);
    $check = int(((atan2($ix, $iy)+$pi*2) / ($pi*2)) * 32 + 0.5) % 32;
    printf("%2dX %2dY => %2d %2d, islope=%3s\n", $ix, $iy, $i32th,
	   $check, $islope);
}
# ----- ang32 ----------------------------------------------------------------
# For given int $xdif and $ydif, calculate atan2 (the angle from origin)
# in 32th's of a circle from 0 to 32, rather than radians.  Note that it 
# returns 32 iff $xdif and $ydif are both zero, an ill-defined case.
# Origin and direction are clockwise:
# 0 => 12:00, 16 => 3:00, 32 => 6:00, 48 => 9:00.

sub ang32 {
    my ($xdif, $ydif) = @_;
    my ($xneg, $yneg, $xyflip) = (0,0,0);
    my $xtmp;
    
    if ($xdif < -0.1) {
	$xdif = -$xdif; $xneg = 1; 
    }
    if ($ydif < -0.1) {
	$ydif = -$ydif; $yneg = 1; 
    }
    if (int($ydif) < int($xdif)) {
	$xtmp = $xdif; $xdif = $ydif; $ydif = $xtmp; $xyflip = 1;
    }

    if (-0.1 < $xdif && $xdif < 0.1) {
	if (-0.1 < $ydif && $ydif < 0.1) {
	    return 32; }
	else {
	    $i32th = 0; }
    }
    else {
	# The 4 comparison values were generated with the accompanying
	# perl script, then open coded here for speed and reasonable
	# space efficiency.  The comparison operations (< or <=) were
	# chosen to make the results match those of atan2 in rounded double
	# precision floating point.  -rwells, 970713.

	$islope = int ((100 * $xdif) / $ydif);
	if ($islope < 54) {             #test #2, first test.
	    if ($islope < 10) {         #test #0, second test.
		$i32th = 0;	}       #         got  #0 after 2 tests.
	    elsif ($islope < 31) {      #test #1, third test.
		$i32th = 1; }           #         got  #1 after 3 tests.
	    else {
		$i32th = 2; }           #         got  #2 after 3 tests.
	} elsif ($islope < 83) {        #test #3, second test.
	    $i32th = 3; }               #         got  #3 after 2 tests.
	else {
	    $i32th = 4; }               #         got  #4 after 2 tests.
    }

    $i32th = ( 8 - $i32th) if ($xyflip);
    $i32th = (16 - $i32th) if ($yneg);
    $i32th = (32 - $i32th) if ($xneg);

    return $i32th % 32;
}

# ----- main ----------------------------------------------------------------

my $weight = 100;		# To preserve precision in slope.

my $angScale = 1000;
my $sCost = int((($pi/60.0)*$angScale)+0.5);

# Consider 0 = 3 O'Clock, 16 = 1:30 O'Clock, counter-clockwise.
# In radians, 0 => 0 radians, 16 => $pi/4 radians.

my $dRadMax = $pi/4;
my $dRadConv = $dRadMax/(32/4);

my $sCostConv = int(($dRadConv * $angScale) + 0.5);

print "dRadMax=$dRadMax dRadConv=$dRadConv sCost=$sCost sCostConv=$sCostConv\n";

my ($i64th, $dRad, $dCos, $dSin, $dTan, $iCentered, $retcost);
my $top64th = int(64/8);

for ($i64th = 1; $i64th <= $top64th; $i64th++) {
    $dRad = $i64th * $dRadConv;
    $dDegrees = ($dRad / $pi) * 180.0;

    $dCos = cos($dRad);
    $dSin = sin($dRad);
    $dTan = $dSin / $dCos;

    $iCentered = int(($dTan * $weight) + 0.5);

    $retcost = int(($dRad * $angScale) + 0.5) + $sCost;

    $mycost = $i64th * $sCostConv + $sCost;

    $minCentered = $iCentered if ($i64th == 1);
    $maxCentered = $iCentered if ($i64th == $top64th);

    printf("%2d: i64th=%2d rads=%4.2f deg=%2.0f dTan=%4.2f icen=%8d cost=%3d mycost=%3d\n",
	   int($i64th/2),$i64th, $dRad, $dDegrees, $dTan, $iCentered, $retcost, $mycost)
	if ($i64th % 2) == 1 || $i64th == $top64th;
}

$ratioCentered = $maxCentered / $minCentered;

print "Centered ratio = $ratioCentered\n";

print "\nCIRCLE THE CLOCK\n";
test_ang32(0, 1);
test_ang32(1, 1);
test_ang32(1, 0);
test_ang32(1, -1);
test_ang32(0, -1);
test_ang32(-1, -1);
test_ang32(-1, 0);
test_ang32(-1, 1);
test_ang32(0, 1);

print "\nDO THE HUNDRED at 12:00\n";
$iy = 100;
for ($ix = 0; $ix <= 100; $ix++) {
    test_ang32($ix, $iy);
}
    
print "\nDO THE FIFTY at 01:30\n";
$ix = 50;
for ($iy = 50; $iy >= 0; $iy--) {
    test_ang32($ix, $iy);
}
    
print "\nDO THE FIFTY at 03:00\n";
$ix = 50;
for ($iy = 0; $iy >= -50; $iy--) {
    test_ang32($ix, $iy);
}
    
print "\nDO THE FIFTY at 04:30\n";
$iy = -50;
for ($ix = 50; $ix >= 0; $ix--) {
    test_ang32($ix, $iy);
}
    
print "\nDO THE FIFTY at 06:00\n";
$iy = -50;
for ($ix = 0; $ix >= -50; $ix--) {
    test_ang32($ix, $iy);
}
    
print "\nDO THE FIFTY at 07:30\n";
$ix = -50;
for ($iy = -50; $iy <= 0; $iy++) {
    test_ang32($ix, $iy);
}
    
print "\nDO THE FIFTY at 09:00\n";
$ix = -50;
for ($iy = 0; $iy <= 50; $iy++) {
    test_ang32($ix, $iy);
}
    
print "\nDO THE FIFTY at 10:30\n";
$iy = 50;
for ($ix = -50; $ix <= 0; $ix++) {
    test_ang32($ix, $iy);
}
    
print "\nGOING NOWHERE\n";
test_ang32(int(0), int(0));
