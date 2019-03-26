class Backend
{
    constructor() {
        this.db = null;
    }

    initialize(updatedCb, deletedCb, loadedCb) {
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
            _id: 'player_' + new Date().toISOString(),
            firstname: firstname,
            lastname: lastname
        };
        this.db.put(person, function callback(err, result) {
            if (!err) {
                console.log('Successfully saved a person!');
            }
        });
    }


}

const Api = new Backend();
