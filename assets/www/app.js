

// =================================================================================================
// VUE ROUTER
// =================================================================================================
const router = new VueRouter({
    mode: 'hash',
    routes: [
      { path: '/players', name: 'players', component: PlayersView },
      { path: '*', redirect: '/players'}
    ]
});

// =================================================================================================
// VUE STORE
// =================================================================================================
const store = new Vuex.Store({
    // Global generic purpose states
    state: {
        finishedLoading: false,
        docs: null
    },
    // Sub modules
    modules: {
      players: PlayersStore,
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
    Api.initialize(function onUpdatedOrInserted(newDoc) {
        store.commit('DB_UPDATE', newDoc);
    }, function(id) {
        store.commit('DB_DELETE', id);
    }, function(docs) {
        console.log("[DB] Loaded");
        store.commit('SET_DOCS', docs);
    });
    store.commit('SET_FINISHED_LOADING');
}

// =================================================================================================
// VUE APPLICATION TOP LEVEL COMPONENT
// =================================================================================================
const app = new Vue({
    router: router,
    el: '#app',
    store: store,
    computed: {

    },
    data () {
        return {
            bottomNav: 'players'
        }
    },
    created: function() {
        loadEverything();
    },
    beforeMount: function() {

    },
    mounted: function() {
        console.log('Tanca initialized');
    }
});

