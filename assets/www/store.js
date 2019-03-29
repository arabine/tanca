// =================================================================================================
// VUE STORE
// =================================================================================================
const store = new Vuex.Store({
    // Global generic purpose states
    state: {
        finishedLoading: false,
        docs: null, // Tous les documents sont mis en mémoire !! Optimisation possible plus tard ... il faudrait juste garder les joueurs et la session en cours
        sessionId: ''
    },
    getters: {
        getTeams: (state) => {
            var teams = [];
            if (state.docs !== undefined) {
                var docs = state.docs.filter(doc => doc._id.includes(state.sessionId));
                if (docs.length == 1) {
                    teams = docs[0].teams;
                }
            }
            return teams;
        },
        getPlayer: (state) => (playerId) => {
            var player = null;
            if (state.docs !== undefined) {
                var docs = state.docs.filter(doc => doc._id.includes(playerId));
                if (docs.length == 1) {
                    player = docs[0];
                }
            }
            return player;
        }
    },
    actions: {
        addPlayer: (context, player) => {
            Api.addPlayer(player, context.state.sessionId);
        },
        addTeam: (context, players) => {
            return Api.addTeam(players, context.state.sessionId);
        }
    },
    mutations: {
        SET_SESSION: (state, session) => {
            state.sessionId = session._id;
        },
        SET_FINISHED_LOADING: (state) => {
            state.finishedLoading = true;
        },
        SET_DOCS: (state, docs) => {
            state.docs = docs;
        },
        DB_UPDATE: (state, newDoc) => {
            var index = Api.binarySearch(state.docs, newDoc._id);
            var doc = state.docs[index];
            if (doc && doc._id === newDoc._id) { // update
                // state.docs[index] = newDoc; // This is not detected by Vue
                Vue.set(state.docs, index, newDoc); // instead use this

                console.log("[DB] Document updated");
            } else { // insert
                state.docs.splice(index, 0, newDoc);
                console.log("[DB] Document inserted");
            }
        },
        DB_DELETE: (state, id) => {
            var index = Api.binarySearch(docs, id);
            var doc = state.docs[index];
            if (doc && doc._id === id) {
                console.log("[DB] Document deleted");
                state.docs.splice(index, 1);
            }
        }
    },
    strict: true
});
