// readme.txt - general comments and build instructions.
// JStroke 1.2 - Japanese Kanji handwriting recognition technology demo.
// Copyright (C) 1997  Robert E. Wells
// http://wellscs.com/pilot
// mailto:robert@wellscs.com
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program (gpl.html); if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//
// Derived from prior work by Todd David Rudick on JavaDict and StrokeDic.
// Makes use of KANJIDIC data from Jim Breen of Monash University.
// Further credit details available at http://wellscs.com/pilot
// ---------------------------------------------------------------------------

Version 1.2 of JStroke has a one-line fix to the SJIS code calculation to
avoid sign extension problems, and fix problem reported by Peter Cumpson with
previous version.  -rwells, 19970926.

Version 1.1 of JStroke includes the reorganization of the code into a more
modular set of files, separating Pilot dependent code from platform
independent recognition code.  It also includes a feature enhancement
requested by Peter Cumpson <pjc2@npl.co.uk>, to display the SJIS code number
of recognized characters so it can be entered into a Canon Wordtank for more
information about the Kanji, as a stop-gap until the Pilot dictionary app is
available.  It also includes dos2unix.pl, a little perl script to remove
Carriage Returns (Control-M's) from the source files, for cleaner
distribution, particularly to Unix.  The source files that are not specific to
the Pilot are now ANSI-C files that use only C style comments, no C++ //
comments.  Owen Taylor <owt1@cornell.edu> made most of these changes so he
could use the recognition code on Linux in his KanjiPad application.  Thanks,
Owen!  -rwells, 19970922.

Version 1.0 of JStroke was intended as a demonstration of this Kanji
handwriting recognition technology on the Pilot.  I believe the technology
shows great promise.  I intend to rework in a later version as one or more
shared libraries for the Pilot, and that is my intended production software
vehicle, for incorporation into other applications and utilities.  My
long-term interest in this technology is primarily in relation to
Japanese-English dictionaries and other learning aids for students of
Japanese.

This version is written in C and intended to be built with GCC for the Pilot;
it builds correctly with the Feb 26 1997 Release 0.4.0 of win32-pilot-gcc,
available for download by following links from
http://www.massena.com/darrin/pilot/index.html.  It goes beyond ANSI-C in
using C++ // comments, which are supported by both the GCC and CodeWarrior C
compilers for the Pilot.  I currently just use a brutally simple compile.bat
file to build the application on Windows/95; I'll add a Gnu make file in a
later release, and will also upgrade to using release 0.5.0 of
win32-pilot-gcc.

This program builds on the previous work of Todd David Rudick on JavaDict;
the stroke database is constructed from his StrokeDic data file, and the C
code was developed by reading Todd's Java code and writing C code to
accomplish similar results, with adaptations for language and platform
differences, and to improve performance.  In this release my major algorithmic
innovation is to "thin" the strokes as they are entered by a series of
smoothing and simplifying filters.  Reducing the number of raw input strokes
produces a near-linear reduction in runtime for the current algorithms.  Todd
and I have exchanged a series of email messages in August 1997 developing a
set of algorithmic ideas which we expect will provide very significant further
reductions in runtime for recognition - I hope to have these implemented for
the Pilot for a release in late October.

This program also builds on the work of Jim Breen and others who have built up
the Monash Nihongo ftp archive, and the EDICT and KANJIDIC files of
information on Japanese words and Kanji.  More information and links to this
work are on my web page at http://wellscs.com/pilot.

I include the PERL5 program compact.pl that I used to convert the StrokeDic
data into C string initializations which are embedded in the current jstroke.c
file.  This version of compact.pl does not exactly match the version I used to
generate the initializations, but should work fine - I have been using it for
other investigations and future planning.  I intend to convert over to using a
Java application to generate the Pilot database directly from StrokeDic and
other data files, rather than using the current bootstrapping method of
creating the database on the pilot and backing it up to the PC system where it
can subsequently be loaded into the pilot directly.  

If you want to create the database the way I do here, you will need to create
three successive versions of the JStroke.prc file; one with
FOR_STROKE_DIC_CREATE_1 defined on line 65 of jstroke.c, one with
FOR_STROKE_DIC_CREATE_1 defined on line 66, and the final version with neither
defined.  Download and run the first two versions successively, then hotsync
to capture a copy of the database before downloading and running the final
version, which will inhibit further backups of the database to the PC once it
is run.

jstroke.bmp is a Windows bitmap binary format file which contains the Pilot
icon for JStroke.  It can be edited with most any paint program on Windows,
such as PaintShopPro.  It is included by jstroke.rcp, and processed by pilrc
1.5.

Previously I had been holding off on releasing the sources because I am
intending to rework them extensively fairly soon.  However, I've decided to go
ahead and release the current sources due to requests, and the realization
that I probably won't have my next release ready until near the end of
October, since I am currently occupied with another project that needs to be
finished by the end of September.  I do intend a fairly major reworking of the
code, so keep that in mind if you start working with it.  

Please email me (mailto:robert@wellscs.com) if you are interested in this
code, so we can benefit from each others work and avoid contradictory changes
if possible.  If you email me I will put you on a mailing list to be notified
of future releases.  Please comment your changes with your name and the date
and the reason for the change, and log them in ChangeLog before sending them
back to me.  Thanks.

If you edit ChangeLog using NTEMACS on Windows, you will want the following
settings in your .emacs file to support using C-X 4 a to add to the ChangeLog.
See http://www.cs.washington.edu/homes/voelker/ntemacs.html to download a copy
of NTEMACS for Win32 machines.  Revise the user stuff to match your own
identity, of course :-).

(setq user-full-name "Robert E. Wells")	;9709
(setq user-mail-address "robert@wellscs.com") ;9709
(setq change-log-default-name "ChangeLog") ; 9709: windows defaults to changelo

Robert E. Wells
http://wellscs.com/pilot
mailto:robert@wellscs.com
Arlington, MA
September 1, 1997

// ----- end of readme.txt ---------------------------------------------------
