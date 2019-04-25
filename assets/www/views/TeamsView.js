
var teams_view_template = /*template*/`

<v-layout column>

      
      <v-layout row justify-end>
        <v-btn dark fab small color="red" @click="deleteSelectedTeams"><v-icon>delete</v-icon></v-btn>
      </v-layout>
      
      <v-list>
        <template v-for="(item, index) in teams">
          
          <v-list-tile @click.capture.stop="toggleTeam(index)">

              <v-list-tile-action>
                <v-checkbox v-model="selected" multiple :value="index" />
              </v-list-tile-action>

              <v-list-tile-content>
                  <v-list-tile-title v-html="item.id"></v-list-tile-title>
                </v-list-tile-content>
  
                <v-list-tile-content>
                  <v-list-tile-title v-html="getTeamName(item)"></v-list-tile-title>
                </v-list-tile-content>

          </v-list-tile>

          <v-divider></v-divider>

        </template>
      </v-list>

      <pre>{{ selected }}</pre> 

</v-layout>
`;

TeamsView = {
  name: 'teams-view',
  template: teams_view_template,
  //====================================================================================================================
  components: {
    
  },
  data () {
    return {
      selected: []
    }
  },
  computed: {
    teams () {
      return this.$store.getters.getTeams;
    },
    // mix the getters into computed with object spread operator
    ...Vuex.mapGetters([
      'getTeamName',
      // ...
    ])
  },
  //====================================================================================================================
  created() {
 
  },
  //====================================================================================================================
  beforeDestroy() {
 
  },
  //====================================================================================================================
  methods : {
    toggleTeam (id) {
      if (this.selected.includes(id)) {
        this.selected.splice(this.selected.indexOf(id), 1);
      } else {
        this.selected.push(id);
      }
    },
    deleteSelectedTeams() {
      if (this.selected.length == 0) {
        this.$eventHub.$emit('alert', 'Vous devez sélectionner au moins une équipe', 'info');
      } else {
        this.$store.dispatch('deleteTeam', this.selected).then((doc) => {
          this.selected = [];
          console.log('[TEAMS] Delete team: ' + JSON.stringify(doc));
          this.$eventHub.$emit('alert', "Équipe(s) supprimée(s)", 'success');
        }).catch((err) => {
          this.selected = [];
          console.log('[TEAMS] Delete team failure: ' + err);
          this.$eventHub.$emit('alert', "Erreur: impossible de supprimer l'(es) équipe(s)", 'error');
        });
      }
    }
  },
  //====================================================================================================================
  mounted: function() {
    console.log('[VUE] Mounted component TeamsView');
  },
}
