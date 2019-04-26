class GamesManager
{
    constructor() {

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
                                    session.teams[j].wons.push(0);
                                    session.teams[j].loses.push(0);
                                }

                                if (session.teams[j].id == opponent) {
                                    session.teams[j].opponents.push(current);
                                    
                                    // 0 for this round means 'not started'
                                    session.teams[j].wons.push(0);
                                    session.teams[j].loses.push(0);
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
