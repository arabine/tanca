class Backend
{
    constructor() {
        this.db = null;
        this.sessionId = '';
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

    getSessionId() {
        return this.sessionId;
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
                console.log('[DB] Found valid session in DB');
                this.sessionId = doc._id;
                resolve();
            }).catch((error) => {
                console.log('[DB] Create new session because: ' + error);
                // Tout est faux, on crée une nouvelle session
                this.createNewSession().then( (doc) => {
                        this.sessionId = doc.id;
                        localStorage.setItem('fr.tanca.session.id', this.sessionId);
                        console.log('[DB] Session created with id: ' + this.sessionId);
                        resolve();
                }).catch( (err) => {
                        console.log('[DB] Cannot create session in DB: ' + err);
                        reject();
                });
                
            });
        });
    }

    async createNewSession() {
        var s = {
            _id: 'session:' + new Date().toISOString(),
            teams: [],
            counter: 1, // unique counter used for team numbering
            rounds: []
        };
        return this.db.put(s);
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
    
    setScores(scores) {
        return this.db.get(this.sessionId).then((doc) => {
            for (let i = 0; i < doc.teams.length; i++) {
             = doc.teams.filter(t => t.id == teamId);
			}
			
			return doc;
            
        }).then( (updatedDoc) => {
            return this.db.put(updatedDoc);
        });
    }

    addTeam(players, teamId) {
        var team = {
            players: players,
            opponents: [],
            wons: [],
            loses: []
        };
        
        return this.db.get(this.sessionId).then((doc) => {
            var idIsUnique = true;
            for (var i = 0; i < doc.teams.length; i++) {
                if (doc.teams[i].id == teamId) {
                    idIsUnique = false;
                    break;
                }
            }

            if ((teamId === undefined) || (!idIsUnique)) {
                team.id = doc.counter++; // generate unique id for this team
            } else {
                team.id = teamId; // specified id is ok
            }
            doc.teams.push(team);
            return this.db.put(doc);
        });
    }

    deleteTeam(indexList) {
        return this.db.get(this.sessionId).then((doc) => {
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

    createRounds() {
        return this.db.get(this.sessionId).then((doc) => {
        
            return Games.createRounds(doc);
        }).then( (updatedDoc) => {
            return this.db.put(updatedDoc);
        });
        
    }

}

const Api = new Backend();
