

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
    let docs = await Api.initializeDb(function onUpdatedOrInserted(newDoc) {
        store.commit('DB_UPDATE', newDoc);
    }, function(id) {
        store.commit('DB_DELETE', id);
    });

    console.log("[APP] Database loaded");
    store.commit('SET_DOCS', docs);

    let response = await fetch(Api.getRootUrl() + "/i18n/i18n.json", {referrerPolicy: "no-referrer", cache: "no-store"});
    let locales = await response.json();

    for (let key in locales) {
        i18n.setLocaleMessage(key, locales[key]);
    }

    let sessionId = await Api.loadCurrentSession();

    store.commit('SET_SESSION', sessionId);
    store.commit('SET_FINISHED_LOADING');
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
    store,
    i18n,
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
        localStorage.setItem('log', 'debug');

        this.$eventHub.$on('alert', this.showAlert);
        loadEverything();

        let locale = navigator.language || navigator.userLanguage;
        locale = locale.substring(0, 2); // sometimes the format returned is fr-FR or us-US , so take only the first two characters

        i18n.locale = locale;
        ulog.info('Detected language: ' + locale);
        moment.locale(locale);

        this.$vuetify.lang.current = locale;
        this.$vuetify.lang.locales = {
            en: {
                noDataText: 'Nothing',
                dataIterator: {
                    rowsPerPageText: "Items per page:",
                    rowsPerPageAll: "All",
                    pageText: "{0}-{1} of {2}",
                    noResultsText: "No matching records found",
                    nextPage: "Next page",
                    prevPage: "Previous page"
                },
                dataTable: {
                    rowsPerPageText: "Rows per page:"
                }
            },
            fr: {
                noDataText: 'Aucune donnée',
                dataIterator: {
                    rowsPerPageText: "Éléments par page:",
                    rowsPerPageAll: "Tous",
                    pageText: "{0}-{1} de {2}",
                    noResultsText: "Aucun élément correspondant",
                    nextPage: "Page suivante",
                    prevPage: "Page précédente"
                },
                dataTable: {
                    rowsPerPageText: "Lignes par page :"
                }
            }
        }

        console.log('[APP] Tanca created');
        

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

