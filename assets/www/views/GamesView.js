
var games_view_template = /*template*/`

<v-layout column>
  <v-flex xs12 >
    
    <v-layout row >
      <v-pagination v-model="page" :total-visible="3" :length="3"></v-pagination>
      <v-spacer />
      <v-btn dark fab small color="green" @click="createRounds()"><v-icon>play_circle_outline</v-icon></v-btn>
    </v-layout>
   
      <v-card v-for="(item, index) in getGames" :key="index" class="mx-auto" color="blue" tile flat dark>
 
          <v-card-actions>

            <v-flex xs6 >
              <v-layout align-center justify-space-between row fill-height>
                <span>({{item[0]}}) {{getTeamNameById(item[0])}}</span>

                <v-avatar color="blue darken-4" size="30px">
                  <span class="white--text">13</span>
                </v-avatar>

              </v-layout>
            </v-flex>

            <v-flex xs6>
              <v-layout align-center justify-space-between row fill-height>
                <span>&nbsp;&nbsp;({{item[1]}}) {{getTeamNameById(item[1])}}</span>
                
                <v-avatar color="purple" size="30px">
                  <span class="white--text">7</span>
                </v-avatar>

              </v-layout>
            </v-flex>
           
          </v-card-actions>
      </v-card>

    </v-flex>
</v-layout>
`;



GamesView = {
  name: 'games-view',
  template: games_view_template,
  //====================================================================================================================
  components: {
    
  },
  data () {
    return {
      page: 1
    }
  },
  computed: {
    rounds () {
      return this.$store.getters.getRounds;
    },
    getGames() {     
      let round = this.rounds.filter((round) => {
         return round.id == this.page;
      });

      let gList = [];
      if (round.length == 1) {
        gList = round[0].list; // list of games
      }

      return gList;
    },
      
  },
  //====================================================================================================================
  created() {
 
  },
  //====================================================================================================================
  beforeDestroy() {
 
  },
  //====================================================================================================================
  methods : {
    createRounds() {
      this.$store.dispatch('createRounds').then( () => {
        this.$eventHub.$emit('alert', "Création des parties réussie", 'success');
      }).catch( (err) => {
        this.$eventHub.$emit('alert', "Impossible de créer les parties: " + err, 'error');
      });
    },
    getTeamNameById(teamId) {
      let team = this.$store.getters.getTeamById(teamId);
      let teamName = '';
      if (team !== undefined) {
        teamName = this.$store.getters.getTeamName(team);
      }
      return teamName;
    }
  },
  //====================================================================================================================
  mounted: function() {
    console.log('[VUE] Mounted component GamesView');
  },
}
