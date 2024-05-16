#!/usr/bin/perl

use strict;
use warnings;
use CGI qw(:standard);
use List::Util qw(shuffle);

# Liste des fruits
my @fruits = ('Pomme', 'Banane', 'Orange', 'Fraise', 'Raisin', 'Mangue', 'Ananas', 'Cerise', 'Kiwi', 'Papaye');

# Mélanger les fruits et sélectionner le premier (aléatoirement)
my $fruit_aleatoire = (shuffle @fruits)[0];

# Générer la page HTML
print start_html('Fruit Aléatoire');

print h1('Fruit Aléatoire');
print p("Le fruit sélectionné aléatoirement est : $fruit_aleatoire");

print end_html;
