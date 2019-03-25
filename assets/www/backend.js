class Backend
{
    constructor() {
        this.db = null;
    }

    initialize() {
       this.db = new PouchDB('tanca');
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
