#!/usr/local/bin/perl
#
# src2html.cgi -- A search script to file symbols in a src2html tree.
# You need to install this in your httpd script directory AND set the
# httpd web root below.
#
# We need the cgi-lib.pl package to be available to Perl
require 'cgi-lib.pl';
#
## We MUST know the root of the httpd server, so we can find the ctags
## 
#
$Http_root= "/usr/pub/pub/www";

						# Parse input -> %in
&ReadParse;					# and print first 2 lines

@keys= keys(%in);			# We only expect one name/val pair
if ($#keys != 0) {			# Otherwise, return an error document
    print <<"query_error";
Content-type: text/html

    <header><title>Query Error</title></header><body>
    <h1>Query Error</h1>
    The document you used to query this source tree has an error in it.
    You should email the maintainer of the source tree with a copy of the
    document with the query form in it.
    </body>
query_error
    exit(1);
}
					# Given the name, determine which tree
$Htmltree= "$Http_root/$keys[0]"; 
$Htmltree=~ s/\/[^\/]*$//;		# Location of the Html tree root
$Srctree= "/$keys[0]";			# URL of the actual source
$Ctags= "$Htmltree/ctags";		# Location of the ctags files

@symbol= split(/\s+/, $in{$keys[0]});	# Get one symbol to search for

if ($#symbol != 0) {			# Hmm, <> 1 symbol, return an error
    print <<"symbol_error";
Content-type: text/html

    <header><title>$Htmltree Search Error</title></header><body>
    <h1>$Htmltree Search Error</h1>
    Hmm, you either sent in no symbols to me to search, or a number of
    symbols to find, separated by whitespace.<p>
    The search function can only handle regexp expressions with no 
    whitespace. Try resubmitting your query.
    </body>
symbol_error
    exit(1);
}
					# Let's go to work, egrep the ctags

# Naftali Schwartz (nschwart@SLINKY.CS.NYU.EDU) reports that using the ^
# on the following line stops the script from working under SunOS 4.1.3.
# Sounds like ctags-new doesn't start the symbol in the 1st column.
open(IN, "cat $Ctags/* | egrep ^$symbol[0] |") || die "$0: egrep failed\n";
$cnt=0;
while(<IN>) {
    ($sym, $line, $file, $com)= split(/\s+/);
    $Sym[$cnt]=$sym; $Line[$cnt]= $line; $File[$cnt]= $file; $cnt++;
}
close(IN);
					# How many did we get? Zero, no symbol
if ($cnt == 0) {
    print <<"symbol_missing";
Content-type: text/html

    <header><title>$Htmltree Search Error</title></header><body>
    <h1>$Htmltree Search Error</h1>
    The symbol $symbol[0] does not appear in the source tree.
    </body>
symbol_missing
    exit(1);
}

if ($cnt == 1) {			# Exactly one, return ptr to that doc
    print "Location: $Srctree/$File[0]";

# I used to use the commented out line under NCSA httpd because the other
# line didn't work. You may need to try things out on your server.
    print ".html\#$Sym[0]\n\n";
#   print ".html\n\n";
    exit(0);
}
					# Else return a list of choices
print <<"many_found";
Content-type: text/html

<header><title>$Htmltree Search</title></header><body>
<h1>$Htmltree Search</h1><ul>
many_found
for ($i = 0; $i < $cnt; $i++) {
    print "<li><a href= \"$Srctree/$File[$i]";
    print ".html#$Sym[$i]\">$Sym[$i]</a> $File[$i]:$Line[$i]\n";
}
print "</ul></body>\n";
exit(0);
