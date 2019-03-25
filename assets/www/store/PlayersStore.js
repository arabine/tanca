
const PlayersStore = {
    namespaced: true,
    state: { 
        status: {
            loggedIn: false
        }, 
        user: null
    },
    actions: {
        login({ dispatch, commit }, { username, password }) {
            console.log("[STORE][AUTH] Login request");
            Api.login(username, password)
                .then(
                    user => {
                        commit('LOGIN_SUCCESS', user);
                        console.log("[STORE][AUTH] Login success");
                    },
                    error => {
                        commit('LOGIN_FAILURE', error);
                        console.log("[STORE][AUTH] Login failure");
                    }
                );
        },
    },
    mutations: {
        LOGIN_SUCCESS(state, user) {
            state.status = { loggedIn: true };
            state.user = user;
        },
        LOGIN_FAILURE(state) {
            state.status =  { loggedIn: false };
            state.user = null;
        },
        LOGOUT(state) {
            state.status = {};
            state.user = null;
            Api.logout();
        }
    }
}
