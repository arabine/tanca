class GamesManager
{
    constructor() {
        this.eventRanking = [];
    }

    isEven(n) {
        return ((n % 2) == 0);
    }

    shuffle(array) {
        array.sort(() => Math.random() - 0.5);
    }

    printRounds(session, index) {

        console.log("========= ROUND " + index + 1 + " =========");
        for (let i = 0; i < session.rounds[index].list.length; i++) {
            
            console.log(session.rounds[index].list[i][0] + " <=> " + session.rounds[index].list[i][1]);
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

            for (let j = 0; j < team.wins.length; j++) {
                rankEntry.total_wins += team.wins[j];
                rankEntry.total_losses += team.losses[j];
            }

            rankEntry.diff = rankEntry.total_wins - rankEntry.total_losses;
            rankEntry.opponents = team.opponents;
            
            // Compute SOS for this team
            
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
            if (session.teams.length > 0) {
                if (this.isEven(session.teams.length)) {
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
                        let round = {
                            id: 1,
                            date: new Date().toISOString(),
                            list: []
                        };

                        for (let i = 0; i < array.length; i += 2) {
                            let current = array[i];
                            let opponent = array[i+1];

                            round.list.push([current, opponent]);

                            for (let j = 0; j < session.teams.length; j++) {
                                if (session.teams[j].id == current) {
                                    session.teams[j].opponents.push(opponent);

                                    // 0 for this round means 'not started'
                                    session.teams[j].wins.push(0);
                                    session.teams[j].losses.push(0);
                                }

                                if (session.teams[j].id == opponent) {
                                    session.teams[j].opponents.push(current);
                                    
                                    // 0 for this round means 'not started'
                                    session.teams[j].wins.push(0);
                                    session.teams[j].losses.push(0);
                                }
                            }
                        }

                        session.rounds.push(round);
                        
                        this.printRounds(session, session.rounds.length - 1);

                        resolve(session);

                    } else {
                        // Blossom
                    }

                } else {
                    reject(new Error("Nombre d'équipes impair, ajouter une équipe"));
                }
            } else {
                reject(new Error("Aucune équipe !"));
            }
        });
    }
}

const Games = new GamesManager();
