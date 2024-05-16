#!/usr/bin/perl
# showDateTime.pl - Un script CGI pour afficher la date et l'heure actuelles.

use strict;
use warnings;
use CGI qw(:standard);

# Créer un nouvel objet CGI
my $query = CGI->new;

# Début du document HTML directement
print $query->start_html(-title => 'Date et Heure Actuelles',
                         -style => {'text-align' => 'center'});

# Corps du document HTML
print $query->h1('Date et Heure Actuelles');
print $query->p(scalar localtime);

# Fin du document HTML
print $query->end_html;

# Fin du script
exit 0;