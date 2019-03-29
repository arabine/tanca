

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
        var session = Api.loadCurrentSession();
        store.commit('SET_SESSION', session);

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
        isReady() {
            return this.$store.state.finishedLoading;
        }
    },
    data () {
        return {
            bottomNav: '',
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
        this.$eventHub.$off('alert');
    },
    mounted: function() {
        console.log('[APP] Tanca initialized');
    }
});

