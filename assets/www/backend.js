class Backend
{
    constructor() {
        this.db = null;
      //  this.sessionId = '';
    }

    initializeDb(updatedCb, deletedCb, loadedCb) {
        this.db = new PouchDB('tanca');

        this.db.allDocs({include_docs: true}).then(function (res) {
            var docs = res.rows.map(function (row) { return row.doc; });  
            loadedCb(docs);
        }).catch(console.log.bind(console));

        this.db.changes({live: true, since: 'now', include_docs: true})
        .on('change', function (change) {
            if (change.deleted) {
                // change.id holds the deleted id
                deletedCb(change.id);
              } else { // updated/inserted
                // change.doc holds the new doc
                updatedCb(change.doc);
              }
        })
        .on('error', function (err) {
            // handle errors
        });
    }

    async getSavedSession() {
        return new Promise( (resolve, reject) => {
            if (localStorage.getItem('fr.tanca.session.id')) {
                resolve(localStorage.getItem('fr.tanca.session.id'));
            } else {
                reject(new Error("[DB] No any Tanca session found in localStorage"));
            }
        });
    }

    async loadCurrentSession() {

        return new Promise( (resolve, reject) => {
            this.getSavedSession().then((sId) => {
                console.log('[DB] Found localStorage session');
                return this.db.get(sId);
            }).then((doc) => {
                let ok = false;
                if (doc.state !== undefined) {
                    if (doc.state == 'ok') {
                        this.loadSession(doc._id);
                        ok  = true;
                    }
                }

                if (ok) {
                    resolve(doc._id);
                } else {
                    throw new Error("Invalid session (deleted or obsolete)");
                }
                
            }).catch((error) => {
                console.log('[DB] Create new session because: ' + error);
                // Tout est faux, on crée une nouvelle session
                this.createNewSession().then( (sessionId) => {
                    resolve(sessionId);
                }).catch( (error) => {
                    reject(error);
                });
            });
        });
    }

    loadSession(sessionId) {
        console.log('[DB] Set current session: ' + sessionId);
        localStorage.setItem('fr.tanca.session.id', sessionId);
    }

    /**
     * @brief Not a real delete, just change the state of the session document
     */
    deleteSession(sessionId) {
        return this.db.get(sessionId).then((doc) => {
            doc.state = 'deleted';
            return doc;
        }).then( (updatedDoc) => {
            return this.db.put(updatedDoc);
        });
    }

    async createNewSession() {
        return new Promise( (resolve, reject) => {
            var s = {
                _id: 'session:' + new Date().toISOString(),
                state: 'ok',
                teams: [],
                counter: 1, // unique counter used for team numbering
                rounds: []
            };
            this.db.put(s).then( (doc) => {
                this.loadSession(doc.id);
                resolve(doc.id);
            }).catch( (err) => {
                console.log('[DB] Cannot create session in DB: ' + err);
                reject(err);
            });
        });
    }

    removeDiacritics(str) {
        return str.normalize('NFD').replace(/[\u0300-\u036f]/g, "");
    }

    // Utility function
    binarySearch(arr, docId) {
        var low = 0, high = arr.length, mid;
        while (low < high) {
          mid = (low + high) >>> 1; // faster version of Math.floor((low + high) / 2)
          arr[mid]._id < docId ? low = mid + 1 : high = mid
        }
        return low;
      }

    addPlayer(player) {
        player._id = 'player:' + new Date().toISOString();
        return this.db.put(player);
    }
    
    /*
    let scores = {
        round: this.round,
        team1Score: this.t1ScoreClone,
        team2Score: this.t2ScoreClone,
        team1Id: this.t1id,
        team2Id: this.t2id
      };
    */
    setScores(scores, sessionId) {
        return this.db.get(sessionId).then((doc) => {

            for (let i = 0; i < doc.rounds.length; i++) {
                // On recherche le round
                if (doc.rounds[i].id == scores.round) {
                    // on recherche le jeu en question
                    for (let j = 0; j < doc.rounds[i].games.length; j++) {
                        if ((doc.rounds[i].games[j].team1Id == scores.team1Id) &&
                            (doc.rounds[i].games[j].team2Id == scores.team2Id)
                            ) {
                            doc.rounds[i].games[j].team1Score = scores.team1Score;
                            doc.rounds[i].games[j].team2Score = scores.team2Score;
                        }
                    }
                }
			}
			
			return doc;
            
        }).then( (updatedDoc) => {
            return this.db.put(updatedDoc);
        });
    }

    addTeam(players, teamId, sessionId) {
        var team = {
            players: players,
            teamName: '' // optional team name
        };
        
        return this.db.get(sessionId).then((doc) => {
            var idIsUnique = true;

            if (teamId) {
                for (var i = 0; i < doc.teams.length; i++) {
                    if (doc.teams[i].id == teamId) {
                        idIsUnique = false;
                        break;
                    }
                }
            }

            if (teamId && idIsUnique) {
                team.id = teamId; // specified id is ok
            } else {
                team.id = doc.counter++; // generate unique id for this team
            }
            doc.teams.push(team);
            return this.db.put(doc);
        });
    }

    deleteTeam(indexList, sessionId) {
        return this.db.get(sessionId).then((doc) => {
            var newArray = [];
            for (var i = 0; i < doc.teams.length; i++) {
                if (indexList.indexOf(i) === -1) {
                    // save this team
                    newArray.push(doc.teams[i]);
                } 
            }
            doc.teams = newArray;
            return this.db.put(doc);
        });
    }

    createRounds(sessionId) {
        return this.db.get(sessionId).then((doc) => {
        
            return Games.createRounds(doc);
        }).then( (updatedDoc) => {
            return this.db.put(updatedDoc);
        });
        
    }

    updateRanking(sessionId) {
        return this.db.get(sessionId).then((doc) => {
            return new Promise( (resolve, reject) => {
                Games.updateEventRanking(doc);
                Games.sortEventRanking();
                resolve(Games.eventRanking);
            });
        });
    }

}

const Api = new Backend();
