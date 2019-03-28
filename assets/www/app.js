

// =================================================================================================
// VUE ROUTER
// =================================================================================================
const router = new VueRouter({
    mode: 'hash',
    routes: [
      { path: '/home', name: 'home', component: HomeView },
      { path: '/players', name: 'players', component: PlayersView },
      { path: '/teams', name: 'teams', component: TeamsView },
      { path: '/games', name: 'games', component: GamesView },
      { path: '/results', name: 'results', component: ResultsView },
      { path: '*', redirect: '/home'}
    ]
});

// =================================================================================================
// VUE STORE
// =================================================================================================
const store = new Vuex.Store({
    // Global generic purpose states
    state: {
        finishedLoading: false,
        docs: null, // Tous les documents sont mis en mémoire !! Optimisation possible plus tard ... il faudrait juste garder les joueurs et la session en cours
        currentSession: null

    },
    mutations: {
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
                state.docs[index] = newDoc;
                console.log("[DB] Document updated");
            } else { // insert
                state.docs.splice(index, 0, newDoc);
                console.log("[DB] Document inserted");
            }
        },
        DB_DELETE: (state, id) => {
            var index = Api.binarySearch(docs, id);
            var doc = this.docs[index];
            if (doc && doc._id === id) {
                console.log("[DB] Document deleted");
                this.docs.splice(index, 1);
            }
        }
    }
});


// =================================================================================================
// SYNCHRONOUS LOADING AT START-UP
// =================================================================================================
async function loadEverything()
{
    Api.initializeDb(function onUpdatedOrInserted(newDoc) {
        store.commit('DB_UPDATE', newDoc);
    }, function(id) {
        store.commit('DB_DELETE', id);
    }, function(docs) {
        console.log("[DB] Loaded");
        store.commit('SET_DOCS', docs);

        // Add here all other inits
        Api.loadCurrentSession();

        store.commit('SET_FINISHED_LOADING');
    });
    
}

// =================================================================================================
// VUE APPLICATION TOP LEVEL COMPONENT
// =================================================================================================
Vue.prototype.$eventHub = new Vue(); // Global event bus

const app = new Vue({
    router: router,
    el: '#app',
    store: store,
    computed: {

    },
    data () {
        return {
            bottomNav: 'players',
            alert: false,
            alertType: 'success', // success, info, warning, error
            alertText: '',
            alertTimeout: 2000
        }
    },
    methods: {
        showAlert(text, type, timeout) {
            this.alertText = text;
            this.alertType = type;
            this.alert = true;
            if (timeout !== undefined) {
                this.alertTimeout = timeout;
            }
        }
    },
    created: function() {
        this.$eventHub.$on('alert', this.showAlert);
        loadEverything();

    },
    beforeMount: function() {

    },
    beforeDestroy() {
        this.$eventHub.$off('logged-in');
    },
    mounted: function() {
        console.log('Tanca initialized');
    }
});

