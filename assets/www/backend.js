class Backend
{
    constructor() {
        this.db = null;
        this.session = null;
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
        // Now we look if we have a current session in working state
        if (localStorage.getItem('tancasession')) {
            this.session = JSON.parse(localStorage.getItem('tancasession'));
        } else {
            // On on la crée alors
            this.session = this.createNewSession();
            localStorage.setItem('tancasession', JSON.stringify(this.session));
        }

        console.log('[DB] Current session is: ' + JSON.stringify(this.session));
    }

    createNewSession() {
        var session = {
            _id: 'session:' + new Date().toISOString(),
            title: '',
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

    addPlayer(firstname, lastname) {
        var person = {
            _id: 'player:' + new Date().toISOString(),
            firstname: firstname,
            lastname: lastname
        };
        this.db.put(person, function callback(err, result) {
            if (!err) {
                console.log('[DB] Successfully saved a person!');
            }
        });
    }

    addTeam(players) {
        var team = {
            players: players,
            opponents: [],
            wons: [],
            loses: []
        };
        //Reading the contents of a Document
        this.db.get(this.session._id, (err, doc) => {
        if (err) {
            return console.log(err);
        } else {
            // Inserting Document
            doc.teams.push(team);
            this.db.put(doc);
            console.log(doc);
        }
 });
    }


}

const Api = new Backend();
