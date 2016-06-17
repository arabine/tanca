# Tanca, un gestionnaire de club et de concours de pétanque (petanque club & contests manager)

Tanca est un logiciel permettant de gérer un club de pétanque et d'organiser des concours internes.
Tanca will help petanque clubs to follow subscribers and organize in-house contests.

## Historique des versions

### V1.1

 - Le pointage des scores est possible
 - Nouvelles options lors de la création d'un événement
 - Correction sur la mise à jour de la fiche des joueurs
 - Correction de la génération des parties
 - Import des joueurs/adhérents à partir d'une liste CSV

### V1.0

 - Version initiale pour tests


## TODO

 - L'affichage du classement de la saison
 - L'ajout et l'édition manuel de parties
 - Edition d'un événement
 - Affichage des points dans la fenêtre des matchs
 - Exporter le classement
 - Exporter la base de joueurs
 - Imprimer le classement
 - Imprimer les matches


## Swisss tournament algorithm



The classic algorithm works like this:

Number the teams 1..n. (Here I'll take n=8.)

Write all the teams in two rows.

1 2 3 4
8 7 6 5

The columns show which teams will play in that round (1 vs 8, 2 vs 7, ...).

Now, keep 1 fixed, but rotate all the other teams. In week 2, you get

1 8 2 3
7 6 5 4

and in week 3, you get

1 7 8 2
6 5 4 3

This continues through week n-1, in this case,

1 3 4 5
2 8 7 6

If n is odd, do the same thing but add a dummy team. Whoever is matched against the dummy team gets a bye that week.

