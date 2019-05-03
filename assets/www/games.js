


class GamesManager
{
    constructor() {
        this.eventRanking = [];
        this.config = {
            pointsWin: 13,
            maxRounds: 3
        }
    }

    isEven(n) {
        return ((n % 2) == 0);
    }

    shuffle(array) {
        array.sort(() => Math.random() - 0.5);
    }

    printRounds(session, index) {

        console.log("========= ROUND " + parseInt(index) + 1 + " =========");
        for (let i = 0; i < session.rounds[index].games.length; i++) {
            
            console.log(session.rounds[index].games[i].team1Id + " <=> " + session.rounds[index].games[i].team2Id);
        }
    }

    /**
     *  Sort best score on top 
     */
    sortEventRanking() {
        this.eventRanking.sort( function(a, b) {
            if (a.total_wins < b.total_wins) {
                return 1;
            } else if (a.total_wins > b.total_wins) {
                return -1;
            } else {
                // Equal, use diff to see who is best
                if (a.diff < b.diff) {
                    return 1;
                } else if (a.diff > b.diff) {
                    return -1;
                } else {
                    // Still equal, use SOS
                    if (a.sos < b.sos) {
                        return 1;
                    } else if (a.sos > b.sos) {
                        return -1;
                    } else {
                        // stricly equals!!
                        return 0;
                    }
                }
            }
        });

    }

    updateEventRanking(session) {

        // Clear then compute ranking
        this.eventRanking = [];

        for (let i = 0; i < session.teams.length; i++) {
            let team = session.teams[i];

            let rankEntry = {
                id: team.id,
                total_wins: 0,
                total_losses: 0,
                diff: 0,
                sos: 0, // Sum of Opponent Scores (SPA == Somme des Points des Adversaires)
                opponents: [] // opponents Ids
            }

            for (let j = 0; j < session.rounds.length; j++) {

                for (let k = 0; k < session.rounds[j].games.length; k++) {
                    let g = session.rounds[j].games[k];
                    if (g.team1Id == team.id) {
                        rankEntry.total_wins += g.team1Score;
                        rankEntry.total_losses += g.team2Score;
                        rankEntry.opponents.push(g.team2Id);
                    }
                    if (g.team2Id == team.id) {
                        rankEntry.total_wins += g.team2Score;
                        rankEntry.total_losses += g.team1Score;
                        rankEntry.opponents.push(g.team1Id);
                    }
                }
            }

            rankEntry.diff = rankEntry.total_wins - rankEntry.total_losses;
            
            this.eventRanking.push(rankEntry);
        }

        // Compute SOS for all teams
        for (let i = 0; i < this.eventRanking.length; i++) {
            for (let j = 0; j < this.eventRanking[i].opponents.length; j++) {

                for (let k = 0; k < this.eventRanking.length; k++) {
                
                    if (this.eventRanking[k].id == this.eventRanking[i].opponents[j]) {
                        this.eventRanking[i].sos += this.eventRanking[k].total_wins;
                    }
                }
            }
        }

    }

    createRounds(session) {
        // Math.floor((Math.random() * 10) + 1);
        return new Promise ( (resolve, reject) => {

            if (session.rounds.length >= this.config.maxRounds) {
                reject(new Error("Max rounds played."));
            }

            // First update the ranking
            this.updateEventRanking(session);

            if (session.teams.length > 0) {
                if (this.isEven(session.teams.length)) {

                    let round = {
                        id: session.rounds.length + 1,
                        date: new Date().toISOString(),
                        games: []
                    };

                    if (session.rounds.length == 0) {
                        // First round: random
                        // Algorithm is very simple:
                        // 1. Create an array of team IDs
                        // 2. Shuffle it
                        // 3. Create games with pairs starting at index 0

                        // STEP 1
                        let array = [];
                        for (let i = 0; i < session.teams.length; i++) {
                            array.push(session.teams[i].id);
                        }

                        // STEP 2
                        console.log("[GAMES] Array before: " + array);
                        this.shuffle(array);
                        console.log("[GAMES] Array after: " + array);

                        // STEP 3        
                        for (let i = 0; i < array.length; i += 2) {
                            let game = {
                                team1Id: array[i],
                                team2Id: array[i+1],
                                // 0 for this round means 'not started'
                                team1Score: 0,
                                team2Score: 0
                            };

                            round.games.push(game);
                        }

                    } else {

                        let finished = true;
                        // All current rounds must be finished to continue on next round
                        session.rounds[session.rounds.length - 1].games.forEach(g => {
                            if ((g.team1Score == 0) && (g.team2Score == 0)) {
                                finished = false;
                            }
                        });

                        if (!finished) {
                            reject(new Error("Le tour de jeu en cours n'est pas terminé"));
                        }


                        //=========== Blossom for all other rounds ===========
                        let  maxDiff = (session.rounds.length) * this.config.pointsWin;

                        // each entry is an array:
                        // index 0: id 1
                        // index 1: id 2
                        // index 2: poids (+ élevé, plus de chance d'être appairé, moins élevé 0 par exemple signifie un rejet)
                        let possiblePairs = [];

                        this.eventRanking.forEach( (player, idx1)  => {
                            this.eventRanking.forEach( (opponent, idx2)  => {

                                // Si les deux équipes n'ont jamais jouées ensemble
                                if ((player.id !== opponent.id) &&
                                    (player.opponents.indexOf(opponent.id) === -1)
                                ) {
                                    let match = [idx1, idx2];
                            
                                    match.push(maxDiff - Math.abs(player.total_wins - opponent.total_wins))

                                    if (this.searchForArray(possiblePairs, match) === -1) {
                                        possiblePairs.push(match)
                                    }
                                }
                            })
                        });

                        let blossom = new Edmonds(possiblePairs, true);
                        let rawPairing = blossom.maxWeightMatching();
                        rawPairing.forEach((match, index) => {
                            if ((match !== -1) && (match < index)) {
                            
                          //      console.log(this.eventRanking[index].id + " <> " + this.eventRanking[match].id);

                                let game = {
                                    team1Id: this.eventRanking[index].id,
                                    team2Id: this.eventRanking[match].id,
                                    // 0 for this round means 'not started'
                                    team1Score: 0,
                                    team2Score: 0
                                };
    
                                round.games.push(game);
                            }
                        });
                    }

                    session.rounds.push(round);
                    this.printRounds(session, session.rounds.length - 1);
                    resolve(session);

                } else {
                    reject(new Error("Nombre d'équipes impair, ajouter une équipe"));
                }
            } else {
                reject(new Error("Aucune équipe !"));
            }
        });
    }

    searchForArray(array, match) {
        let notFound = -1;
    
        for (let i = 0; i < array.length; i++) {
            let entry = array[i];
            let counterOk = 0;
            if (
              ((entry[0] == match[0]) || (entry[0] == match[1])) &&
              ((entry[1] == match[0]) || (entry[1] == match[1]))
            ) {
              notFound = 1;
            }
        }
    
        return notFound;
    }
}

const Games = new GamesManager();
