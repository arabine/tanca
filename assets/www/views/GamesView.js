
var games_view_template = /*template*/`

<v-layout column>
  <v-flex xs12 >

    
    <v-layout row >
      <v-pagination v-model="page" :total-visible="3" :length="3"></v-pagination>
      <v-spacer />
      <v-btn dark fab small color="green" @click="createRounds()"><v-icon>play_circle_outline</v-icon></v-btn>
    </v-layout>


    <v-list>
      <template v-for="(item, index) in getGames">
        
        <v-list-tile>
              <v-list-tile-content>
              
              <v-list-tile-action>
                <p>{{ index + 1 }}: {{item[0]}}  </p>
                </v-list-tile-action>

                <v-list-tile-action>
                <v-icon>compare_arrows</v-icon>
              </v-list-tile-action>


                <v-list-tile-action>
                <p> {{item[1]}}</p>
                </v-list-tile-action>
              </v-list-tile-content>
  </div>
        </v-list-tile>

        <v-divider></v-divider>

      </template>
    </v-list>

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
    }
  },
  //====================================================================================================================
  mounted: function() {
    console.log('[VUE] Mounted component GamesView');
  },
}
