

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
        Api.loadCurrentSession().then( () => {
            store.commit('SET_FINISHED_LOADING');
            console.log("[DB] Finished loading data.");
        });

        
    });
    
}

// =================================================================================================
// MAIN APP TEMPLATE LAYOUT
// =================================================================================================
const app_template = /*template*/`
<v-app dark>

    <v-snackbar v-model="alert" top :color="alertType" :timeout="alertTimeout">
        {{ alertText }}
    </v-snackbar>

    <TopToolbar></TopToolbar>

    <v-content>
   

        <template v-if="!isReady">
            <v-container fluid fill-height>
                <v-layout align-center justify-center>
                    <v-flex class="text-xs-center">
                        <v-progress-circular indeterminate color="primary"></v-progress-circular>
                    </v-flex>
                </v-layout>
            </v-container>
        </template>

        <template v-else>
            <router-view></router-view>					
        </template>
    </v-content>

    <BottomNav></BottomNav>

</v-app>
`

// =================================================================================================
// VUE APPLICATION TOP LEVEL COMPONENT
// =================================================================================================
Vue.prototype.$eventHub = new Vue(); // Global event bus

const app = new Vue({
    router: router,
    el: '#app',
    store: store,
    template: app_template,
    components: {TopToolbar, BottomNav},
    computed: {
        isReady() {
            return this.$store.state.finishedLoading;
        }
    },
    data () {
        return {
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
        console.log('[APP] Tanca created');
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

