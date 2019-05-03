// =================================================================================================
// VUE STORE
// =================================================================================================
const store = new Vuex.Store({
    // Global generic purpose states
    state: {
        finishedLoading: false,
        sessionId: '',
        docs: null, // Tous les documents sont mis en mémoire !! Optimisation possible plus tard ... il faudrait juste garder les joueurs et la session en cours
    },
    getters: {
        getSessions: (state) => {
            let sessions = [];
            if (state.finishedLoading && (state.docs !== undefined)) {
                state.docs.forEach( doc => {
                    if ( doc._id.includes('session:')) {
                        if (doc.state !== undefined) {
                            if (doc.state === 'ok') {
                                let s = {
                                    id: doc._id,
                                    isCurrent: state.sessionId == doc._id
                                }
                                sessions.push(s);
                            }
                        }
                    }
                });
            }
            return sessions;
        },
        sessionExists: (state) => (sessionIsoDate) => {
            let exists = false;
            if (state.finishedLoading && (state.docs !== undefined)) {
                state.docs.forEach( doc => {
                    if ( doc._id.includes(sessionIsoDate)) {
                        exists = true;
                    }
                });
            }
            return exists;
        },
        getTeams: (state) => {
            let teams = [];
            if (state.finishedLoading && (state.docs !== undefined)) {
                let docs = state.docs.filter(doc => doc._id.includes(state.sessionId));
                if (docs.length == 1) {
                    teams = docs[0].teams;
                } else {
                    console.log("[STORE] No session found!");
                }
            } else {
                console.log("[STORE] Database not loaded!");
            }
            return teams;
        },
        getRounds: (state) => {
            let rounds = [];
            if (state.finishedLoading && (state.docs !== undefined)) {
                let docs = state.docs.filter(doc => doc._id.includes(state.sessionId));
                if (docs.length == 1) {
                    rounds = docs[0].rounds;
                } else {
                    console.log("[STORE] No session found!");
                }
            } else {
                console.log("[STORE] Database not loaded!");
            }
            return rounds;
        },
        getPlayer: (state) => (playerId) => {
            let player = null;
            if (state.finishedLoading && (state.docs !== undefined)) {
                let docs = state.docs.filter(doc => doc._id.includes(playerId));
                if (docs.length == 1) {
                    player = docs[0];
                }
            }
            return player;
        },
        getTeamById: (state, getters) => (teamId) =>  {
            let teams = getters.getTeams;
            let oneTeam = teams.filter(t => t.id == teamId);
            return oneTeam[0];
        },
        getTeamName: (state, getters) => (team) =>  {
            let teamName = '';

            var players = [];
            for (var i = 0; i < team.players.length; i++) {
                players.push(getters.getPlayer(team.players[i]).firstname);
            }
            teamName = players.join(' / ');

            return teamName;
        }
    },
    actions: {
        addPlayer: (context, player) => {
            return Api.addPlayer(player);
        },
        addTeam: (context, players) => {
            return Api.addTeam(players, null, context.state.sessionId);
        },
        deleteTeam: (context, indexList) => {
            return Api.deleteTeam(indexList, context.state.sessionId)
        },
        createRounds: (context) => {
            return Api.createRounds(context.state.sessionId);
        },
        updateRanking: (context) => {
            return Api.updateRanking(context.state.sessionId);
        },
        setScores: (context, scores) => {
			return Api.setScores(scores, context.state.sessionId);
        },
        loadSession: (context, sessionId) => {
            context.commit('SET_SESSION', sessionId);
            return Api.loadSession(sessionId);
        },
        createSession: (context) =>  {
            return Api.createNewSession();
        },
        deleteSession: (context, sessionId) => {
			return Api.deleteSession(sessionId);
        },
    },
    mutations: {
        SET_FINISHED_LOADING: (state) => {
            state.finishedLoading = true;
        },
        SET_SESSION(state, newSessionId) {
            state.sessionId = newSessionId;
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
