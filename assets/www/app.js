

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
    },
    // Sub modules
    modules: {
      players: PlayersStore,
    },
    mutations: {
        SET_FINISHED_LOADING: (state) => {
            state.finishedLoading = true;
        },
    }
});


// =================================================================================================
// SYNCHRONOUS LOADING AT START-UP
// =================================================================================================
async function loadEverything()
{
    Api.initialize();
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

