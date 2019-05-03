
var players_view_template = /*template*/`

<v-layout column>
  
      <AddPlayerDialog  :visible="showAddPlayerDialog" :key="showAddPlayerDialog" @close="showAddPlayerDialog=false" ></AddPlayerDialog>
      
      <v-layout row justify-end>
        <v-btn dark fab small color="indigo" @click="addSelectedToTeam"><v-icon>group_add</v-icon></v-btn>
        <v-btn dark fab small color="pink" @click="showAddPlayerDialog=true"><v-icon>add</v-icon></v-btn>
      </v-layout>

      <v-text-field v-model="searchWord" prepend-icon="search" name="search" label="Search" type="text"></v-text-field>
      

      <v-list>
        <template v-for="(item, index) in filteredPlayers">
          
          <v-list-tile @click.capture.stop="togglePlayer(item._id)">

              <v-list-tile-action>
                <v-checkbox v-model="selected" multiple :value="item._id" />
              </v-list-tile-action>

                <v-list-tile-avatar>
                  <v-icon>account_circle</v-icon>
                </v-list-tile-avatar>
  
                <v-list-tile-content>
                <p>{{ item.firstname }} {{item.lastname }}</p>
    
                </v-list-tile-content>

          </v-list-tile>

          <v-divider></v-divider>

        </template>
      </v-list>

   <!--   <pre>{{ selected }}</pre> -->
  
</v-layout>
`;

PlayersView = {
  name: 'players-view',
  template: players_view_template,
  //====================================================================================================================
  components: {
    AddPlayerDialog
  },
  data () {
    return {
      showAddPlayerDialog: false,
      searchWord: '',
      selected: []
    }
  },
  computed: {
      players() {
        var pList = [];
        
        if (this.$store.state.docs !== null) { 
          pList = this.$store.state.docs.filter(function(doc) {
              return doc._id.includes('player:');
          });
        }
        return pList;
      },
      filteredPlayers() {
        var pList = [];
        
        pList = this.players.filter((doc) => {
            var matchFilter = true;

            if (this.searchWord !== '') {
              var word = Api.removeDiacritics(this.searchWord.trim().toLowerCase());
              matchFilter = doc.firstname.toLowerCase().includes(word) || 
                            doc.lastname.toLowerCase().includes(word);
            }

            return matchFilter;
        });

        return pList;
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
    togglePlayer (id) {
      if (this.selected.includes(id)) {
        // Removing the color
        this.selected.splice(this.selected.indexOf(id), 1);
      } else {
        this.selected.push(id);
      }
    },
    addSelectedToTeam() {
      if (this.selected.length == 0) {
        this.$eventHub.$emit('alert', 'Vous devez sélectionner au moins un joueur', 'info');
      } else {
        this.$store.dispatch('addTeam', this.selected).then((doc) => {
          this.selected = [];
          console.log('[PLAYERS] Add team: ' + JSON.stringify(doc));
          this.$eventHub.$emit('alert', "L'équipe a été créée", 'success');
        }).catch((err) => {
          this.selected = [];
          console.log('[PLAYERS] Add team failure: ' + err);
          this.$eventHub.$emit('alert', "Erreur: impossible de créer l'équipe", 'error');
        });
        
      }
    }
  },
  //====================================================================================================================
  mounted: function() {
    console.log('[VUE] Mounted component PlayersView');
  },
}
