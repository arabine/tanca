
var games_view_template = /*template*/`

<v-layout column>

  <ScoreDialog  :visible="showScoreDialog" :round="page" :t1="team1" :t2="team2" :t1score="team1Score" :t2score="team2Score" :t1id="team1Id" :t2id="team2Id" 
      @close="showScoreDialog=false"
      @success="editScoreSuccess">
  </ScoreDialog>
  <v-flex xs12 >
  
    <v-layout row >
      <v-pagination v-model="page" :total-visible="3" :length="3"></v-pagination>
      <v-spacer />
      <v-btn dark fab small color="green" @click="createRounds()"><v-icon>play_circle_outline</v-icon></v-btn>
    </v-layout>
   
    <template v-for="(item, index) in getGames">
      <v-card style="cursor:pointer;"  :color="getFinishedColor(item.finished)" tile raised dark @click.native="selectItem(item)">
 
          <v-card-actions>
            <template xs6 v-for="(team, i) in item.teams">
              <v-flex>
                <v-layout align-center justify-space-between row fill-height>
                  <span>&nbsp;</span>
                  <span>({{team.id}})&nbsp;{{team.name}}</span>

                  <v-avatar :color="getWinnerColor(team.winner)" size="30px">
                    <span class="white--text">{{team.score}}</span>
                  </v-avatar>
                </v-layout>
              </v-flex>

            </template>
           
          </v-card-actions>
      </v-card>

      <v-divider></v-divider>
      </template>

    </v-flex>
</v-layout>
`;


GamesView = {
  name: 'games-view',
  template: games_view_template,
  components: {
    ScoreDialog
  },
  //====================================================================================================================
  data () {
    return {
      page: 1,
      showScoreDialog: false,
      team1: '',
      team2: '',
      team1Score: 0,
      team2Score: 0,
      team1Id: 0,
      team2Id: 0
    }
  },
  //====================================================================================================================
  computed: {
    rounds () {
      return this.$store.getters.getRounds;
    },
    /**
     * Here we build a dynamic array of objects to easily generate HTML list
     * It is more future proof, easy to extend
     */
    getGames() {     
      let round = this.rounds.filter((round) => {
         return round.id == this.page;
      });

      let gList = [];
      if (round.length == 1) {
      //  gList = round[0].list; // list of games

        let games = round[0].list;
        let currentRound = this.page-1;
        for (let i = 0; i < games.length; i++) {
          let r = {
            finished: false,
            teams: []
          };

          for (let j = 0; j < 2; j++) {
            let g = {};
            g.id = games[i][j];
            let team = this.$store.getters.getTeamById(g.id);
            g.score = team.wins[currentRound];
            g.name = this.$store.getters.getTeamName(team);

            r.teams.push(g);
          }

          if (r.teams[0].score > r.teams[1].score) {
            r.teams[0].winner = true;
            r.teams[1].winner = false;
            r.finished = true;
          } else if (r.teams[0].score < r.teams[1].score) {
            r.teams[0].winner = false;
            r.teams[1].winner = true;
            r.finished = true;
          } else {
            r.teams[0].winner = false;
            r.teams[1].winner = false;
          }

          gList.push(r);
        }
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
    editScoreSuccess() {
      console.log("[GAMES] Edit score success");
      
    },
    getWinnerColor(winner) {
      return winner ? 'blue darken-4' : 'purple';
    },
    getFinishedColor(finished) {
      return finished ? 'blue' : 'grey darken-2';
    },
    createRounds() {
      this.$store.dispatch('createRounds').then( () => {
        this.$eventHub.$emit('alert', "Création des parties réussie", 'success');
      }).catch( (err) => {
        this.$eventHub.$emit('alert', "Impossible de créer les parties: " + err, 'error');
      });
    },
    selectItem(item) {

      console.log("[GAMES] Game finished: " + item.finished);
      this.team1 = '(' + item.teams[0].id + ') ' + item.teams[0].name;
      this.team2 = '(' + item.teams[1].id + ') ' + item.teams[1].name;
      this.team1Id = item.teams[0].id;
      this.team2Id = item.teams[1].id;
      this.team1Score = item.teams[0].score;
      this.team2Score = item.teams[1].score;
      this.showScoreDialog = true;
    }
  },
  //====================================================================================================================
  mounted: function() {
    console.log('[VUE] Mounted component GamesView');
  },
}
