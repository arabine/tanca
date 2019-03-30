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

    createRounds(session) {
        // Math.floor((Math.random() * 10) + 1);
        return new Promise ( (resolve, reject) => {
            if (this.isEven(session.teams)) {
                if (session.rounds.length == 0) {
                    // First round: random
                    // Algorithm is very simple:
                    // 1. Create an array of team IDs
                    // 2. Shuffle it
                    // 3. Create games with pairs starting at index 0

                    // Let's do step 1.
                    let array = [];
                    for (let i = 0; i < session.teams.length; i++) {
                        array.push(session.teams[i].id);
                    }

                    // Now step 2.
                    console.log("[GAMES] Array before: " + array);
                    this.shuffle(array);
                    console.log("[GAMES] Array after: " + array);

                } else {
                    // Blossom
                }

            } else {
                reject(new error("Nombre d'équipes impair, ajouter une équipe"));
            }
        });
    }
}

const Games = new GamesManager();
