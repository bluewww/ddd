#! /usr/bin/ddd --perl
#########################################################
# Erzeugt aus csv-Datei Liste
#
# Datum: 26.05.1998
# Autor: mc
#
#########################################################
use strict 'vars';

############# globale ###################################
my  $datei      = "";
my  $ext        = ".lst";
my  $h_datei    = "";
my  $b_datei    = "";
my  $txt        = "";
my  $wrk_dir    = "./";
my  @bez_arr;
my  ($l_nr,$l_bez,$l_anz,$l_len,$l_bre,$l_hoh,$l_fla,$l_vol,$l_lfd);
my  $lines_per_page = 63;           # Zeilen pro Seite
my  $date;
my  $seite;

###############################################################
#
# Ausgabe Versionsnummer und Benutzung
#
###############################################################
sub usage {
    my $name;

    chop($name = `basename $0`);
    print <<EODQ, <<'EOSQ';

NAME
    $name - Erzeugung einer Liste aus csv-Datei

SYNOPSIS
    $name [options] <Eingabedatei> 

DESCRIPTION
    Verarbeitet die csv-Datei zu einer Liste

OPTIONS
    -o Ausgabedatei (default "Eingabedatei".lst)
    -d Arbeitsverzeichnis
    -h Textdatei über Liste
    -b Textdatei unterhalb Liste

VERSION
    $name, V 1.0 28.05.1998, GranIT GmbH Reutlingen

EODQ
EOSQ
    exit;
}

###############################################################
# Formatdefinitionen
###############################################################
format LST_TOP =              # Kopfzeilen
@||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
"Firma GranIT GmbH Reutlingen"
============================================================================
Bauteilliste                                         Datum:     @<<<<<<<<<<<
$date 
	  				             Seite:     @<<<
++$seite
----------------------------------------------------------------------------
.

format LST_TRENN =
----------------------------------------------------------------------------
.
format LST_HEAD =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$txt
.
format LST_BASE =
@<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
$txt
.

format LST_UEBER =
ArtNr.   Artikel-Bez.         Stück     LxBxH [m]      Fl [m2]  V [m3]  lfdm
.

format LST_RUMPF =           # Rumpf
@<<<<<<@<<<<<<<<<<<<<<<<<<<<@>>>>>>@>>>>> @>>>> @>>>>@>>>>>>>@>>>>>>>@>>>>>>>
$l_nr,$l_bez,$l_anz,$l_len,$l_bre,$l_hoh,$l_fla,$l_vol,$l_lfd
.

###############################################################
#
# Extrahiert Argumente
#
###############################################################
sub GetArgs {
    my $count = $#ARGV;
    my $ii;

    for ( $ii = 0; $ii < $count; $ii++ ) {
        if ( $ARGV[$ii] eq "-o" ) {
            $datei = $ARGV[++$ii];
        } elsif ( $ARGV[$ii] eq "-h" ) {
            $h_datei = $ARGV[++$ii];
        } elsif ( $ARGV[$ii] eq "-b" ) {
            $b_datei = $ARGV[++$ii];
        } elsif ( $ARGV[$ii] eq "-d" ) {
            $wrk_dir = $ARGV[++$ii];
        } else {
            usage();
        }
    }
}
##########################################################
# Sortiert Platten nach INNEN/AUSSEN und Lage
##########################################################
sub cmpfkt {
    my ($ix1,$anr1,$stk1,$fla1,$vol1,$len1);
    my ($ix2,$anr2,$stk2,$fla2,$vol2,$len2);

    ($ix1,$anr1,$stk1,$fla1,$vol1,$len1) = unpack("iiifff",$a);
    ($ix2,$anr2,$stk2,$fla2,$vol2,$len2) = unpack("iiifff",$b);

    if ( @bez_arr[$ix1] eq @bez_arr[$ix2] ) {
	if ( $anr1 == $anr2 ) {
	    return ( $len2 <=> $len1 ); 
	} else {
	    return ( $anr1 <=> $anr2 ); 
	}
    } else {
        if ( @bez_arr[$ix1] lt @bez_arr[$ix2] )  {
	    return(1);
	} else {
	    return(-1);
	}
    }  

}


main: {
    my $infile = $ARGV[$#ARGV];
    my ($line, $shit);
    my ($ii, $ix);
    my ($grp,$art,$artnr,$bez,$stk,$len,$bre,$hoh,$fst,$url,$urh,$fla,$ofl,$vol,$spz);
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst);

    my $cmd;
    my $idx = 0;
    my $progname = `basename $0`;
    my @zle;

    chop($progname);

    die usage() if ( $infile eq "" );

    GetArgs(); 

    if ( !$datei ) {
        $datei = sprintf("$infile$ext");
    }
    die "Fehler beim Öffnen von $wrk_dir$datei !" if ( ! open(STDOUT, ">$wrk_dir$datei"));
    die "Fehler beim Öffnen von $wrk_dir$infile !" if ( ! open(FH, "<$wrk_dir$infile"));

    ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) =
                                             gmtime(time);
    $date = sprintf("%d.%d.%d", $mday, $mon+1, $year);


#    $^ = LST_TOP;
#    $= = $lines_per_page;

    if ($h_datei) {
        $~ = LST_HEAD;
        die "Fehler beim Öffnen von $wrk_dir$h_datei !" if ( ! open(FT, "<$wrk_dir$h_datei"));
        while (($txt = <FT>)) {
            write;
	}
        close(FT);
    }

    $~ = LST_TRENN;
    write;

    $~ = LST_UEBER;
    write;

    $~ = LST_TRENN;
    write;

    $~ = LST_RUMPF;



LINE:    while (($line = <FH>)) {
        ($grp,$art,$artnr,$bez,$stk,$len,$bre,$hoh,$fst,$url,$urh,$fla,$ofl,$vol,$spz) =
            split(/[\n;]+/, $line);

	next LINE if ($grp eq  "\"\"");	# Leerzeile nicht
	next LINE if (index ($grp, "\"") != 0);


        $len =~ tr/,/./;
        $bre =~ tr/,/./;
        $hoh =~ tr/,/./;
        $fla =~ tr/,/./;
        $ofl =~ tr/,/./;
        $vol =~ tr/,/./;
        $spz =~ tr/,/./;

	@bez_arr[$idx] = sprintf("%s",$bez);
        @zle[$idx] = pack("iiifffff",$idx,$artnr,$stk,$fla,$vol,$len,$bre,$hoh);
	$idx++;
    }

    if ($idx == 0) {         # leere Datei loeschen	
        close(FH);
        $cmd = sprintf("rm %s%s",$wrk_dir,$datei);
	system($cmd);
	exit;
    }

    @zle = sort cmpfkt @zle; 


    for ($ii = 0; $ii < $idx; $ii++) {
        ($ix,$artnr,$stk,$fla,$vol,$len,$bre,$hoh) = unpack("iiifffff",@zle[$ii]); 

	$l_nr  = $artnr;
	$l_bez = @bez_arr[$ix];
	$l_anz = $stk;
	$l_len = sprintf("%5.3f",$len);
	$l_bre = sprintf("%5.3f",$bre);
	$l_hoh = sprintf("%5.3f",$hoh);
	$l_fla = sprintf("%5.3f",$stk * $fla);
	$l_vol = sprintf("%5.3f",$stk * $vol);
	$l_lfd = sprintf("%5.3f",$stk * $len);
	write;
    }

    $~ = LST_TRENN;
    write;

    if ($b_datei) {
        $~ = LST_BASE;
        die "Fehler beim Öffnen von $wrk_dir$b_datei !" if ( ! open(FT, "<$wrk_dir$b_datei"));
        while (($txt = <FT>)) {
            write;
	}
        close(FT);
    }

    close(FH);
}

Process tm-plain finished
