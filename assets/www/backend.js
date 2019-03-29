class Backend
{
    constructor() {
        this.db = null;
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

    loadCurrentSession() {
        var session = {}
        // Now we look if we have a current session in working state
        if (localStorage.getItem('tancasession')) {
            session = JSON.parse(localStorage.getItem('tancasession'));
        } else {
            // On on la crée alors
            session = this.createNewSession();
            localStorage.setItem('tancasession', JSON.stringify(session));
        }

        console.log('[DB] Current session is: ' + JSON.stringify(session));
        return session;
    }

    createNewSession() {
        var session = {
            _id: 'session:' + new Date().toISOString(),
            teams: []
        };
        this.db.put(session, function callback(err, result) {
            if (!err) {
                console.log('[DB] Successfully saved a session!');
            }
        });
        return session;
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
        this.db.put(player, function callback(err, result) {
            if (!err) {
                console.log('[DB] Successfully saved a person!');
            }
        });
    }

    addTeam(players, sessionId) {
        var team = {
            players: players,
            opponents: [],
            wons: [],
            loses: []
        };
        
        return this.db.get(sessionId).then((doc) => {
            doc.teams.push(team);
            return this.db.put(doc);
        });
    }

    deleteTeams(teams) {
        // FIXME
    }


}

const Api = new Backend();
