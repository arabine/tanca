
var teams_view_template = /*template*/`

<v-layout column>
  <v-flex xs12 >
      
      <v-layout row justify-end>
        <v-btn dark fab small color="red" @click="deleteSelectedTeams"><v-icon>delete</v-icon></v-btn>
      </v-layout>
      
      <v-card-text>

          <v-list>
            <template v-for="(item, index) in teams">
              
              <v-list-tile @click.capture.stop="toggleTeam(index)">

                  <v-list-tile-action>
                    <v-checkbox v-model="selected" multiple :value="index" />
                  </v-list-tile-action>

                  <v-list-tile-content>
                      <v-list-tile-title v-html="index"></v-list-tile-title>
                    </v-list-tile-content>
      
                    <v-list-tile-content>
                      <v-list-tile-title v-html="getTeamName(item)"></v-list-tile-title>
                    </v-list-tile-content>

              </v-list-tile>

              <v-divider></v-divider>

            </template>
          </v-list>

          <pre>{{ selected }}</pre> 
      </v-card-text>
    </v-flex>
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
    }
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
        Api.deleteTeams(this.selected);
        this.selected = [];
        this.$eventHub.$emit('alert', "Équipe(s) supprimée(s)", 'success');
      }
    },
    getTeamName(item) {
      var players = [];
      for (var i = 0; i < item.players.length; i++) {
        players.push(this.$store.getters.getPlayer(item.players[i]).firstname);
      }
      return players.join(' ');
    }
  },
  //====================================================================================================================
  mounted: function() {
    console.log('[VUE] Mounted component TeamsView');
  },
}
